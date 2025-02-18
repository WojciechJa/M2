
#define _cube_cpp_

#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "log.h"
#include "char.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"
#include <sstream>
#include "packet.h"
#include "desc_client.h"

static std::vector<CUBE_RENEWAL_DATA*>	s_cube_proto;

typedef std::vector<CUBE_RENEWAL_VALUE>	TCubeValueVector;

static bool s_isInitializedCubeMaterialInformation = false;

struct SCubeMaterialInfo
{
	SCubeMaterialInfo()
	{
		bHaveComplicateMaterial = false;
	};

	CUBE_RENEWAL_VALUE			reward;							// º¸»ó?? ¹¹³?
	TCubeValueVector	material;						// ?ç·áµé?º ¹¹³?
	long long			gold;							// µ·?º ¾ó¸¶µå³?
	long long			punktyos;
	long long			cheque;
	int 				percent;
	std::string		category;
	TCubeValueVector	complicateMaterial;				// º¹?â??-_- ?ç·áµé

	std::string			infoText;		
	bool				bHaveComplicateMaterial;		//
};

struct SItemNameAndLevel
{
	SItemNameAndLevel() { level = 0; }

	std::string		name;
	int				level;
};


typedef std::vector<SCubeMaterialInfo>								TCubeResultList;
typedef boost::unordered_map<DWORD, TCubeResultList>				TCubeMapByNPC;				// °¢°¢?? NPCº°·? ¾î¶² °? ¸¸µé ¼ö ??°í ?ç·á°¡ ¹º?ö...

TCubeMapByNPC cube_info_map;


static bool FN_check_valid_npc( WORD vnum )
{
	for ( std::vector<CUBE_RENEWAL_DATA*>::iterator iter = s_cube_proto.begin(); iter != s_cube_proto.end(); iter++ )
	{
		if ( std::find((*iter)->npc_vnum.begin(), (*iter)->npc_vnum.end(), vnum) != (*iter)->npc_vnum.end() )
			return true;
	}

	return false;
}


static bool FN_check_cube_data (CUBE_RENEWAL_DATA *cube_data)
{
	DWORD	i = 0;
	DWORD	end_index = 0;

	end_index = cube_data->npc_vnum.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->npc_vnum[i] == 0 )	return false;
	}

	end_index = cube_data->item.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->item[i].vnum == 0 )		return false;
		if ( cube_data->item[i].count == 0 )	return false;
	}

	end_index = cube_data->reward.size();
	for (i=0; i<end_index; ++i)
	{
		if ( cube_data->reward[i].vnum == 0 )	return false;
		if ( cube_data->reward[i].count == 0 )	return false;
	}
	return true;
}

static int FN_check_cube_item_vnum_material(const SCubeMaterialInfo& materialInfo, int index)
{
	if (index <= materialInfo.material.size()){
		return materialInfo.material[index-1].vnum;
	}
	return 0;
}

static int FN_check_cube_item_count_material(const SCubeMaterialInfo& materialInfo,int index)
{
	if (index <= materialInfo.material.size()){
		return materialInfo.material[index-1].count;
	}

	return 0;
}

CUBE_RENEWAL_DATA::CUBE_RENEWAL_DATA()
{
	this->gold = 0;
	this->punktyos = 0;
	this->cheque = 0;
	this->category = "WORLDARD";
}


static void CubeReload()
{
	cube_info_map.clear();
	// cube_result_info_map_by_npc.clear();
	Cube_InformationInitialize();
	for (DESC_MANAGER::DESC_SET::const_iterator it = DESC_MANAGER::instance().GetClientSet().begin(); it != DESC_MANAGER::instance().GetClientSet().end(); ++it) {
		LPCHARACTER ch = (*it)->GetCharacter();
		if (ch) {
			Cube_close(ch);
		}
	}
}

void Cube_init()
{
	CUBE_RENEWAL_DATA * p_cube = NULL;
	std::vector<CUBE_RENEWAL_DATA*>::iterator iter;

	char file_name[256+1];
	snprintf(file_name, sizeof(file_name), "%s/cube.txt", LocaleService_GetBasePath().c_str());

	sys_log(0, "Cube_Init %s", file_name);

	for (iter = s_cube_proto.begin(); iter!=s_cube_proto.end(); iter++)
	{
		p_cube = *iter;
		M2_DELETE(p_cube);
	}

	s_cube_proto.clear();

	if (false == Cube_load(file_name))
		sys_err("Cube_Init failed");

	if (s_isInitializedCubeMaterialInformation)
		CubeReload();
}

bool Cube_load (const char *file)
{
	FILE	*fp;


	const char	*value_string;

	char	one_line[256];
	int		value1, value2;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	char *v1;
	CUBE_RENEWAL_DATA	*cube_data = NULL;
	CUBE_RENEWAL_VALUE	cube_value = {0,0};

	if (0 == file || 0 == file[0])
		return false;

	if ((fp = fopen(file, "r")) == 0)
		return false;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);

		if (NULL == token_string)
			continue;

		// set value1, value2
		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);
		    value_string = v;

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		TOKEN("section")
		{
			cube_data = M2_NEW CUBE_RENEWAL_DATA;
		}
		else TOKEN("npc")
		{
			cube_data->npc_vnum.push_back((WORD)value1);
		}
		else TOKEN("item")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->item.push_back(cube_value);
		}
		else TOKEN("reward")
		{
			cube_value.vnum		= value1;
			cube_value.count	= value2;

			cube_data->reward.push_back(cube_value);
		}
		else TOKEN("percent")
		{

			cube_data->percent = value1;
		}

		else TOKEN("category")
		{
			cube_data->category = value_string;
		}

		else TOKEN("gold")
		{
			// ?¦?¶¿¡ ??¿ä?? ±?¾?
			cube_data->gold = value1;
		}

		else TOKEN("punktyos")
		{
			cube_data->punktyos = value1;
		}

		else TOKEN("cheque")
		{
			cube_data->cheque = value1;
		}

		else TOKEN("end")
		{

			// TODO : check cube data
			if (false == FN_check_cube_data(cube_data))
			{
				//dev_log(LOG_DEB0, "something wrong");
				sys_log(0, "something wrong");
				M2_DELETE(cube_data);
				continue;
			}
			s_cube_proto.push_back(cube_data);
		}
	}

	fclose(fp);
	return true;
}


SItemNameAndLevel SplitItemNameAndLevelFromName(const std::string& name)
{
	int level = 0;
	SItemNameAndLevel info;
	info.name = name;

	size_t pos = name.find("+");
	
	if (std::string::npos != pos)
	{
		const std::string levelStr = name.substr(pos + 1, name.size() - pos - 1);
		str_to_number(level, levelStr.c_str());

		info.name = name.substr(0, pos);
	}

	info.level = level;

	return info;
};


bool Cube_InformationInitialize()
{
	for (int i = 0; i < s_cube_proto.size(); ++i)
	{
		CUBE_RENEWAL_DATA* cubeData = s_cube_proto[i];

		const std::vector<CUBE_RENEWAL_VALUE>& rewards = cubeData->reward;

		if (1 != rewards.size())
		{
			sys_err("[CubeInfo] WARNING! Does not support multiple rewards (count: %d)", rewards.size());			
			continue;
		}

		const CUBE_RENEWAL_VALUE& reward = rewards.at(0);
		const WORD& npcVNUM = cubeData->npc_vnum.at(0);
		bool bComplicate = false;
		
		TCubeMapByNPC& cubeMap = cube_info_map;
		TCubeResultList& resultList = cubeMap[npcVNUM];
		SCubeMaterialInfo materialInfo;

		materialInfo.reward = reward;
		materialInfo.gold = cubeData->gold;
		materialInfo.punktyos = cubeData->punktyos;
		materialInfo.cheque = cubeData->cheque;
		materialInfo.percent = cubeData->percent;
		materialInfo.material = cubeData->item;
		materialInfo.category = cubeData->category;

		resultList.push_back(materialInfo);
	}

	s_isInitializedCubeMaterialInformation = true;

	return true;
}


void Cube_open (LPCHARACTER ch)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	if (npc)
	{
		DWORD npcVNUM = npc->GetRaceNum();
		
		if (NULL == ch)
		{
			return;
		}
		
		if (!npc)
		{
			return;
		}
		
		
		if (NULL==npc)
		{
			return;
		}
		
		if ( FN_check_valid_npc(npcVNUM) == false )
		{
			return;
		}


#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		if ( ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetOfflineShopOwner())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Cannot open refinement window");
			return;
		}
#else
		if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Cannot open refinement window");
			return;
		}
#endif

		long distance = DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY());

		if (distance >= CUBE_MAX_DISTANCE)
		{
			sys_log(1, "CUBE: TOO_FAR: %s distance %d", ch->GetName(), distance);
			return;
		}


		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,npcVNUM);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_DATES_LOADING);
		SendDateCubeRenewalPackets(ch,CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE);

		ch->SetCubeNpc(npc);
	}
	else
	{
		return;
		sys_log(1, "CUBE: Return Wait()");
	}
}

void Cube_close(LPCHARACTER ch)
{
	ch->SetCubeNpc(NULL);
}

void Cube_Make(LPCHARACTER ch, int index, int count_item, int index_item_improve)
{
	LPCHARACTER	npc;

	npc = ch->GetQuestNPC();

	if (!ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "To create an item you have to have the refinement window open");
		return;
	}

	if (NULL == npc)
	{
		return;
	}

	int index_value = 0;
	bool material_check = true;
	LPITEM pItem;
	int iEmptyPos;

	const TCubeResultList& resultList = cube_info_map[npc->GetRaceNum()];
	for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
	{
		if (index_value == index)
		{
			const SCubeMaterialInfo& materialInfo = *iter;

			for (int i = 0; i < materialInfo.material.size(); ++i)
			{
				if (ch->CountSpecifyItem(materialInfo.material[i].vnum) < (materialInfo.material[i].count*count_item))
				{
					material_check = false;
				}
			}

			if (materialInfo.gold != 0){
				if (ch->GetGold() < (materialInfo.gold*count_item))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Niewystarczaj�ca ilo�� yang.");
					return;
				}
			}

			if (materialInfo.punktyos != 0){
				if (ch->GetPktOsiag() < (materialInfo.punktyos*count_item))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Niewystarczaj�ca ilo�� punkt�w osi�gnie�.");
					return;
				}
			}

			if (materialInfo.cheque != 0){
				if (ch->GetCheque() < (materialInfo.cheque*count_item))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Niewystarczaj�ca ilo�� won.");
					return;
				}
			}

			if (material_check){
				
				int percent_number;
				int total_items_give = 0;

				
				int porcent_item_improve = 0;

				if (index_item_improve != -1)
				{

					LPITEM item = ch->GetInventoryItem(index_item_improve);
					if(item != NULL)
					{

						if(item->GetCount() <= 40){
							if (materialInfo.percent+item->GetCount() <= 100){
								porcent_item_improve = item->GetCount();
							}

							if(materialInfo.percent < 100)
							{
								if (materialInfo.percent+item->GetCount() > 100){
									porcent_item_improve = 100 - materialInfo.percent;
								}
							}
						}
					}

					if(porcent_item_improve != 0)
					{
						item->SetCount(item->GetCount()-porcent_item_improve);
					}
				}

				for (int i = 0; i < count_item; ++i)
				{
					percent_number = number(1,100);
					if ( percent_number<=materialInfo.percent+porcent_item_improve)
					{
						total_items_give++;
					}
				}

				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum,(materialInfo.reward.count*count_item));
				if (pItem->IsDragonSoul())
				{
					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
				}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
				else if (pItem->IsSkillBook())
				{
					iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
				}
				else if (pItem->IsUpgradeItem())
				{
					iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
				}
				else if (pItem->IsStone())
				{
					iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
				}
				else if (pItem->IsBox())
				{
					iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
				}
				else if (pItem->IsEfsun())
				{
					iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
				}
				else if (pItem->IsCicek())
				{
					iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
				}
#endif
				else{
					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
				}

				if (iEmptyPos < 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Nie masz miejsca w ekwipunku!");
					return;
				}

				for (int i = 0; i < materialInfo.material.size(); ++i)
				{
					ch->RemoveSpecifyItem(materialInfo.material[i].vnum, (materialInfo.material[i].count*count_item));
				}

				if (materialInfo.gold != 0){
					ch->PointChange(POINT_GOLD, -static_cast<long long>(materialInfo.gold*count_item), false);
				}

				if (materialInfo.punktyos != 0){
					ch->PointChange(POINT_PKT_OSIAG, -static_cast<long long>(materialInfo.punktyos*count_item), false);
				}

				if (materialInfo.cheque != 0){
					ch->PointChange(POINT_CHEQUE, -static_cast<long long>(materialInfo.cheque*count_item), false);
				}

				if(total_items_give <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Nie uda�o si�.");
					return;
				}

				pItem = ITEM_MANAGER::instance().CreateItem(materialInfo.reward.vnum,(materialInfo.reward.count*total_items_give));
// 				if (pItem->IsDragonSoul())
// 				{
// 					iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
// 					pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
// 				}

// #ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
// 				else if (pItem->IsSkillBook())
// 				{
// 					iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// 				else if (pItem->IsUpgradeItem())
// 				{
// 					iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// 				else if (pItem->IsStone())
// 				{
// 					iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// 				else if (pItem->IsBox())
// 				{
// 					iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// 				else if (pItem->IsEfsun())
// 				{
// 					iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// 				else if (pItem->IsCicek())
// 				{
// 					iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
// #endif
// 				else{
// 					iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
// 					pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 				}
				ch->AutoGiveItem(pItem->GetVnum(), pItem->GetCount(), -1, false);
				ch->ChatPacket(CHAT_TYPE_INFO, "Wytworzy�e�: %s (x%d)", pItem->GetName(), pItem->GetCount());
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO,"Nie posiadasz wymaganych przedmiot�w.");
			}
		}

		index_value++;
	}
}


void SendDateCubeRenewalPackets(LPCHARACTER ch, BYTE subheader, DWORD npcVNUM)
{
	if (!ch)
		return;

	TPacketGCCubeRenewalReceive pack={};
	pack.subheader = subheader;

	if(subheader == CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE)
	{
		const TCubeResultList& resultList = cube_info_map[npcVNUM];
		for (TCubeResultList::const_iterator iter = resultList.begin(); resultList.end() != iter; ++iter)
		{

			const SCubeMaterialInfo& materialInfo = *iter;

			pack.date_cube_renewal.vnum_reward = materialInfo.reward.vnum;
			pack.date_cube_renewal.count_reward = materialInfo.reward.count;

			const TItemTable* itemTable = ITEM_MANAGER::instance().GetTable(materialInfo.reward.vnum);

			if (itemTable) // Check if the item table exists
			{
				if (!IS_SET(itemTable->dwAntiFlags, ITEM_ANTIFLAG_STACK)) {
					pack.date_cube_renewal.item_reward_stackable = true;
				} else {
					pack.date_cube_renewal.item_reward_stackable = false;
				}
			}
			else {
				pack.date_cube_renewal.item_reward_stackable = false;

				sys_err("SendDateCubeRenewalPackets: Invalid item vnum (vnum: %u)", materialInfo.reward.vnum);
			}

			pack.date_cube_renewal.vnum_material_1 = FN_check_cube_item_vnum_material(materialInfo,1);
			pack.date_cube_renewal.count_material_1 = FN_check_cube_item_count_material(materialInfo,1);

			pack.date_cube_renewal.vnum_material_2 = FN_check_cube_item_vnum_material(materialInfo,2);
			pack.date_cube_renewal.count_material_2 = FN_check_cube_item_count_material(materialInfo,2);

			pack.date_cube_renewal.vnum_material_3 = FN_check_cube_item_vnum_material(materialInfo,3);
			pack.date_cube_renewal.count_material_3 = FN_check_cube_item_count_material(materialInfo,3);

			pack.date_cube_renewal.vnum_material_4 = FN_check_cube_item_vnum_material(materialInfo,4);
			pack.date_cube_renewal.count_material_4 = FN_check_cube_item_count_material(materialInfo,4);

			pack.date_cube_renewal.vnum_material_5 = FN_check_cube_item_vnum_material(materialInfo,5);
			pack.date_cube_renewal.count_material_5 = FN_check_cube_item_count_material(materialInfo,5);

			pack.date_cube_renewal.vnum_material_6 = FN_check_cube_item_vnum_material(materialInfo,6);
			pack.date_cube_renewal.count_material_6 = FN_check_cube_item_count_material(materialInfo,6);

			pack.date_cube_renewal.vnum_material_7 = FN_check_cube_item_vnum_material(materialInfo,7);
			pack.date_cube_renewal.count_material_7 = FN_check_cube_item_count_material(materialInfo,7);

			pack.date_cube_renewal.gold = materialInfo.gold;
			pack.date_cube_renewal.punktyos = materialInfo.punktyos;
			pack.date_cube_renewal.cheque = materialInfo.cheque;
			pack.date_cube_renewal.percent = materialInfo.percent;

			strlcpy (pack.date_cube_renewal.category, 	materialInfo.category.c_str(), 		sizeof(pack.date_cube_renewal.category));

			LPDESC d = ch->GetDesc();

			if (NULL == d)
			{
				sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
				return ;
			}

			d->Packet(&pack, sizeof(pack));
		}
	}
	else{

		LPDESC d = ch->GetDesc();

		if (NULL == d)
		{
			sys_err ("User SendDateCubeRenewalPackets (%s)'s DESC is NULL POINT.", ch->GetName());
			return ;
		}

		d->Packet(&pack, sizeof(pack));
	}
}