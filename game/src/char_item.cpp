#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "monarch.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "arena.h"
#include "threeway_war.h"
#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif
#include "safebox.h"
#include "shop.h"

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif

#ifdef ENABLE_GLOVE_SYSTEM
#include "glove_bonus_manager.h"
#endif

#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#include "../../common/PulseManager.h"
#endif

#include "../../common/item_length.h"

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"
#ifdef ENABLE_NEW_PET_SYSTEM
#include "PetSystem.h"
#endif

#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX
#define ENABLE_ITEM_RARE_ATTR_LEVEL_PCT

enum { ITEM_BROKEN_METIN_VNUM = 28960 };

// CHANGE_ITEM_ATTRIBUTES
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
	case 22000:
	case 22010:
	case 22011:
	case 22020:
	case ITEM_MARRIAGE_RING:
		return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
	case 5:
	case 25:
	case 45:
	case 108:
	case 109:
		return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;
	if (IS_CASTLE_MAP(map_index))
		return false;

	switch (map_index)
	{
	case 66:
	case 71:
	case 72:
	case 73:
	case 193:
#if 0
	case 184:
	case 185:
	case 186:
	case 187:
	case 188:
	case 189:
#endif

	case 216:
	case 217:
	case 208:

	case 113:
		return false;
	}

	if (map_index > 10000) return false;

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) return true;

	switch (nMapIndex)
	{
	case 1:
	case 2:
		return true;
	}

	return false;
}

static bool FN_check_item_socket(LPITEM item)
{
#ifdef ENABLE_SORT_INVENTORY_ITEMS
	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT)
		return true;
#endif

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE == GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE == GET_SEX(ch))
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif
#ifdef ENABLE_AURA_SYSTEM
	if ((m_bAuraRefine) || (m_bAuraAbsorption))
		return false;
#endif

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (!IsValidItemPosition(Cell))
		return NULL;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pDSItems[wCell];
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid Buff-EQ item cell %d >= %d", wCell, BUFF_WINDOW_SLOT_MAX_NUM);
			return NULL;
		}
		return m_pointsInstant.pBuffEquipmentItem[wCell];
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_PlayerSlots->pSwitchbotItems[wCell];
#endif
	default:
		return NULL;
	}
	return NULL;
}

void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	, bool bWereMine
#endif
)
{
	if (!m_PlayerSlots)
		return;

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
	{
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
			return;
		}

		LPITEM pOld = m_PlayerSlots->pItems[wCell];

		if (pOld)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
						continue;

					if (m_PlayerSlots->pItems[p] && m_PlayerSlots->pItems[p] != pOld)
						continue;

					m_PlayerSlots->bItemGrid[p] = 0;
				}
			}
			else
				m_PlayerSlots->bItemGrid[wCell] = 0;
		}

		if (pItem)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
						continue;

					m_PlayerSlots->bItemGrid[p] = wCell + 1;
				}
			}
			else
				m_PlayerSlots->bItemGrid[wCell] = wCell + 1;
		}

		m_PlayerSlots->pItems[wCell] = pItem;
	}
	break;

	case DRAGON_SOUL_INVENTORY:
	{
		LPITEM pOld = m_PlayerSlots->pDSItems[wCell];

		if (pOld)
		{
			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						continue;

					if (m_PlayerSlots->pDSItems[p] && m_PlayerSlots->pDSItems[p] != pOld)
						continue;

					m_PlayerSlots->wDSItemGrid[p] = 0;
				}
			}
			else
				m_PlayerSlots->wDSItemGrid[wCell] = 0;
		}

		if (pItem)
		{
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
				return;
			}

			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						continue;

					m_PlayerSlots->wDSItemGrid[p] = wCell + 1;
				}
			}
			else
				m_PlayerSlots->wDSItemGrid[wCell] = wCell + 1;
		}

		m_PlayerSlots->pDSItems[wCell] = pItem;
	}
	break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
	{
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM) {
			sys_err("CHARACTER::SetItem: invalid BUFF_EQUIPMENT item cell %d", wCell);
			return;
		}
		LPITEM pOld = m_pointsInstant.pBuffEquipmentItem[wCell];

		if (pOld && pItem) {
			return;
		}

		m_pointsInstant.pBuffEquipmentItem[wCell] = pItem;
		if (GetBuffNPCSystem() != NULL) {
			if (GetBuffNPCSystem()->IsSummoned()) {
				GetBuffNPCSystem()->UpdateBuffEquipment();
			}
		}
	}
	break;
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
	{
		LPITEM pOld = m_PlayerSlots->pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_PlayerSlots->pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif

	default:
		sys_err("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags = pItem->GetAntiFlag();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(pack.aApplyRandom, pItem->GetRandomApplies(), sizeof(pack.aApplyRandom));
#endif
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			memset(pack.aApplyRandom, 0, sizeof(pack.aApplyRandom));
#endif
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell) || (SKILL_BOOK_INVENTORY_SLOT_START <= wCell && SKILL_BOOK_INVENTORY_SLOT_END > wCell) || (UPGRADE_ITEMS_INVENTORY_SLOT_START <= wCell && UPGRADE_ITEMS_INVENTORY_SLOT_END > wCell) || (STONE_INVENTORY_SLOT_START <= wCell && STONE_INVENTORY_SLOT_END > wCell) || (BOX_INVENTORY_SLOT_START <= wCell && BOX_INVENTORY_SLOT_END > wCell) || (EFSUN_INVENTORY_SLOT_START <= wCell && EFSUN_INVENTORY_SLOT_END > wCell) || (CICEK_INVENTORY_SLOT_START <= wCell && CICEK_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#else
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				pItem->SetWindow(INVENTORY);
#endif
			else
				pItem->SetWindow(EQUIPMENT);
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			pItem->SetWindow(BUFF_EQUIPMENT);
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif		
		}
	}
}

#ifdef ENABLE_SYSTEM_RUNE
void CHARACTER::CheckRuneExtraBonus(LPITEM item, BYTE type)
{
	switch (type)
	{
		
		case 0:
		{
#ifdef ENABLE_EXTRABONUS_SYSTEM
			if (FindAffect(RUNE_AFFECT_START + item->GetType())) {
                RemoveAffect(RUNE_AFFECT_START + item->GetType());
            }

			if (item->GetSocket(0) > 0) {
				for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i) {
					int bApplyType = item->GetProto()->aApplies[i].bType;
					int bApplyValue = item->GetProto()->aApplies[i].lValue;
					int reinforcement = (bApplyValue*RUNE_EXTRABONUS_VALUE)/100;

					if (item->GetProto()->aApplies[i].bType == APPLY_NONE) {
						continue;
					}

					AddAffect(RUNE_AFFECT_START+item->GetType(), aApplyInfo[bApplyType].bPointType, reinforcement, AFF_NONE, 60*60*60*365, 0, false);
				}
			}
#endif
			bool allRunesEquipped = GetWear(WEAR_RUNE) && GetWear(WEAR_RUNE_RED) && GetWear(WEAR_RUNE_BLUE) &&
									GetWear(WEAR_RUNE_GREEN) && GetWear(WEAR_RUNE_YELLOW) && GetWear(WEAR_RUNE_BLACK);
					
			if (allRunesEquipped) {
				bool allRunesHaveValue5 = true;
				bool allRunesHaveValue9 = true;

				for (int i = WEAR_RUNE; i <= WEAR_RUNE_BLACK; i++) {
					LPITEM pItem = GetWear(i);
					if (pItem) {
						int value1 = pItem->GetValue(1);
						// ChatPacket(CHAT_TYPE_INFO, "item vnum: %d value1: %d", pItem->GetVnum(), value1);
						if (value1 != 5) {
							allRunesHaveValue5 = false; 
						}
						if (value1 != 9) {
							allRunesHaveValue9 = false;
						}
					}
				}
				
				if (allRunesHaveValue5) {
					if (!FindAffect(AFFECT_RUNE_DECK5)) {
						// ChatPacket(CHAT_TYPE_INFO, "apply deck5 apply values");
						AddAffect(AFFECT_RUNE_DECK5, POINT_FINAL_DMG_BONUS, 5, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
						AddAffect(AFFECT_RUNE_DECK5, POINT_NORMAL_HIT_DAMAGE_BONUS, 15, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
						AddAffect(AFFECT_RUNE_DECK5, POINT_ATTBONUS_HUMAN, 25, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
						AddAffect(AFFECT_RUNE_DECK5, POINT_ATTBONUS_KLASY, 10, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
					}
				} else {
					if (FindAffect(AFFECT_RUNE_DECK5)) {
						// ChatPacket(CHAT_TYPE_INFO, "remove deck5 apply values");
						RemoveAffect(AFFECT_RUNE_DECK5);
					}
				}

				if (allRunesHaveValue9) {
					if (!FindAffect(AFFECT_RUNE_DECK9)) {
						// ChatPacket(CHAT_TYPE_INFO, "apply deck9 apply values");
						AddAffect(AFFECT_RUNE_DECK9, POINT_FINAL_DMG_BONUS, 10, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
						AddAffect(AFFECT_RUNE_DECK9, POINT_NORMAL_HIT_DAMAGE_BONUS, 25, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
						AddAffect(AFFECT_RUNE_DECK9, POINT_ATTBONUS_HUMAN, 40, AFF_NONE, 60 * 60 * 60 * 365, 0, false);
					}
				} else {
					if (FindAffect(AFFECT_RUNE_DECK9)) {
						// ChatPacket(CHAT_TYPE_INFO, "remove deck9 apply values");
						RemoveAffect(AFFECT_RUNE_DECK9);
					}
				}
			}
			break;
		}

		case 1:
		{
#ifdef ENABLE_EXTRABONUS_SYSTEM
			if (FindAffect(RUNE_AFFECT_START+item->GetType())) {
				RemoveAffect(RUNE_AFFECT_START+item->GetType());
			}
#endif
			if (FindAffect(AFFECT_RUNE_DECK5)) {
				RemoveAffect(AFFECT_RUNE_DECK5);
			}
			if (FindAffect(AFFECT_RUNE_DECK9)) {
				RemoveAffect(AFFECT_RUNE_DECK9);
			}
			break;
		}

		default:
			break;
	}
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
void CHARACTER::CheckPetExtraBonus(LPITEM item, BYTE type)
{
	switch (type)
	{
		
		case 0:
		{
			if (FindAffect(PET_AFFECT_START + item->GetValue(0))) {
                RemoveAffect(PET_AFFECT_START + item->GetValue(0));
            }

			if (item->GetSocket(0) > 0) {
				for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i) {
					int bApplyType = item->GetProto()->aApplies[i].bType;
					int bApplyValue = item->GetProto()->aApplies[i].lValue;
					int reinforcement = (bApplyValue*PET_EXTRABONUS_VALUE)/100;

					if (item->GetProto()->aApplies[i].bType == APPLY_NONE) {
						continue;
					}

					AddAffect(PET_AFFECT_START+item->GetValue(0), aApplyInfo[bApplyType].bPointType, reinforcement, AFF_NONE, 60*60*60*365, 0, false);
				}
			}
			break;
		}

		case 1:
		{
			if (FindAffect(RUNE_AFFECT_START+item->GetValue(0))) {
				RemoveAffect(RUNE_AFFECT_START+item->GetValue(0));
			}
			break;
		}

		default:
			break;
	}
}
#endif

LPITEM CHARACTER::GetWear(WORD bCell) const
{
	if (!m_PlayerSlots)
		return nullptr;

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_PlayerSlots->pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(WORD bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos(INVENTORY, INVENTORY_MAX_NUM + bCell), item);

	/*/
	if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
	/*/
}


bool CHARACTER::OpenChestItem(TItemPos pos, WORD bOpenCount)
{
	LPITEM item;
	if (!CanHandleItem())
		return false;
	
	if (!IsValidItemPosition(pos) || !(item = GetItem(pos)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (IsStun())
		return false;
	
	OpenChestItem(item, bOpenCount);
	return true;
}

bool CHARACTER::GetItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums, std::vector<DWORD> &dwItemCounts)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		
		if (dwVnum)
		{
			bSuccess = true;
	
			if (bSuccess)
			{
				dwItemVnums.push_back(dwVnum);
				dwItemCounts.push_back(dwCount);
			}
			else
			{
				return false;
			}
		}
	}
	return bSuccess;
}

int CHARACTER::IsEmptyDragonSoulInventoryByGradeAndSize(EDragonSoulGradeTypes grade, BYTE size)
{	
	BYTE bFreeBlocks = 0;
	
    for (int subSlot = 0; subSlot < DS_SLOT_MAX; ++subSlot)
    {
        WORD basePosition = subSlot * DRAGON_SOUL_STEP_MAX * DRAGON_SOUL_BOX_SIZE + grade * DRAGON_SOUL_BOX_SIZE;
		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
        {
            WORD cellIndex = basePosition + i;
            if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, cellIndex), size))
            {
                bFreeBlocks++;
                break;
            }
        }
    }
	
	if (bFreeBlocks == DS_SLOT_MAX)
		return 1;

    return -1;
}

void CHARACTER::OpenChestItem(LPITEM item, WORD bCount)
{
	if (item->GetType() != ITEM_GIFTBOX)
	{
		sys_err("The Player tries to open non-chest item!");
		return;
	}
	
	if (item->isLocked())
		return;
	
	if (!CanHandleItem())
		return;
	
	if (item->IsExchanging())
		return;
	
	bCount = MINMAX(1, MIN(bCount, item->GetCount()), ITEM_MAX_COUNT);
	
	// PCT support
	auto group = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetVnum());
	if (!group)
		return;
	// PCT support
	
	// DSS Support
	if (std::find(v_dss_chest_items.begin(), v_dss_chest_items.end(), item->GetVnum()) != v_dss_chest_items.end())
	{
		if (IsEmptyDragonSoulInventoryByGradeAndSize(static_cast<EDragonSoulGradeTypes>(item->GetValue(0)), 1) == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o miejsca w ekwipunku alchemii.");
			return;
		}
	}
	else if (GetEmptyInventory(3) == -1)
	{
		ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o miejsca.");
		return;
	}

	item->Lock(true);

	std::vector<DWORD> dwVnums;
	std::vector<DWORD> dwCounts;

	WORD bSucceed = 0;
	for (WORD i = 0; i < bCount; i++)
	{
		if (GetItemFromSpecialItemGroup(item->GetVnum(), dwVnums, dwCounts))
			bSucceed++;
	}

	if (bSucceed != bCount)
	{
		item->Lock(false);
		return;
	}
	
	struct GroupedItem { DWORD vnum; DWORD count; DWORD opened_count; };
	std::vector<GroupedItem> grouped_items;
	
	for (size_t i = 0; i < dwVnums.size(); i++)
	{
		TItemTable* pProto = ITEM_MANAGER::instance().GetTable(dwVnums[i]);
		if (!pProto) continue;

		if (pProto->dwFlags & ITEM_FLAG_STACKABLE || dwVnums[i] == CSpecialItemGroup::GOLD)
		{
			auto it = std::find_if(grouped_items.begin(), grouped_items.end(),
				[&](const GroupedItem& item) { return item.vnum == dwVnums[i]; });

			if (it != grouped_items.end())
			{
				it->count += dwCounts[i];
				it->opened_count += 1;
			}
			else
				grouped_items.push_back({ dwVnums[i], dwCounts[i], 1 });
		}
		else
			grouped_items.push_back({ dwVnums[i], dwCounts[i], 1 });
    }
	
	int opened_chests = 0;

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("instant_open_received_from_%d_x_%s"), bCount, item->GetName());

	for (const auto& grouped_item : grouped_items)
	{
		DWORD vnum = grouped_item.vnum;
		DWORD count = grouped_item.count;
		DWORD opened_count = grouped_item.opened_count;
		
		if (vnum == CSpecialItemGroup::GOLD)
		{
			opened_chests += opened_count;

#ifdef FULL_YANG
			ChangeGold(count);
#else
			PointChange(POINT_GOLD, count);
#endif
			continue;
		}

		TItemTable* pProto = ITEM_MANAGER::instance().GetTable(vnum);
		if (!pProto) continue;

		if (std::find(v_dss_chest_items.begin(), v_dss_chest_items.end(), item->GetVnum()) != v_dss_chest_items.end())
		{
			if (IsEmptyDragonSoulInventoryByGradeAndSize(static_cast<EDragonSoulGradeTypes>(item->GetValue(0)), 1) == -1)
			{
				item->SetCount(item->GetCount() - opened_chests);
				item->Lock(false);

				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NO_FREE_SPACE_GIFTBOX_OPENING"));
				return;
			}
		}
		else if (GetEmptyInventory(3) == -1 && !group->m_bType == 1)  // PCT support
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NO_FREE_SPACE_GIFTBOX_OPENING"));
			item->SetCount(item->GetCount() - opened_chests);

			item->Lock(false);
			return;
		}

		DWORD max_per_stack = (pProto->dwFlags & ITEM_FLAG_STACKABLE) ? ITEM_MAX_COUNT : 1;

		LPITEM pItem;

		for (DWORD i = 0; i < count / max_per_stack; i++)
			pItem = AutoGiveItem(vnum, max_per_stack, -1, false);

		if (count % max_per_stack > 0)
			pItem = AutoGiveItem(vnum, count % max_per_stack, -1, false);

		if (pItem)
			ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("%s °¡ %d"), pItem->GetName(), count % max_per_stack);

		opened_chests += opened_count;
	}

	item->SetCount(item->GetCount() - (group->m_bType == 1 ? bSucceed : opened_chests)); // PCT support
	item->Lock(false);
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	for (i = 0; i < BUFF_WINDOW_SLOT_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BUFF_EQUIPMENT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
#ifdef ENABLE_SWITCHBOT
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (i = 0; i < SKILL_BOOK_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(SKILL_BOOK_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < UPGRADE_ITEMS_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(UPGRADE_ITEMS_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < STONE_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(STONE_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < BOX_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(BOX_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < EFSUN_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(EFSUN_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

	for (i = 0; i < CICEK_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(CICEK_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	if (!m_PlayerSlots)
		return false;

	switch (Cell.window_type)
	{
	case INVENTORY:
	{
		WORD bCell = Cell.cell;

		++iExceptionCell;

		if (Cell.IsBeltInventoryPosition())
		{
			LPITEM beltItem = GetWear(WEAR_BELT);

			if (NULL == beltItem)
				return false;

			if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
				return false;

			if (m_PlayerSlots->bItemGrid[bCell])
			{
				if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
					return true;

				return false;
			}

			if (bSize == 1)
				return true;

		}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (Cell.IsSkillBookInventoryPosition())
		{
			if (bCell < SKILL_BOOK_INVENTORY_SLOT_START)
				return false;

			if (bCell > SKILL_BOOK_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (SKILL_BOOK_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= SKILL_BOOK_INVENTORY_MAX_NUM)
						return false;

					if (p / (SKILL_BOOK_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsUpgradeItemsInventoryPosition())
		{
			if (bCell < UPGRADE_ITEMS_INVENTORY_SLOT_START)
				return false;

			if (bCell > UPGRADE_ITEMS_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (UPGRADE_ITEMS_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= UPGRADE_ITEMS_INVENTORY_MAX_NUM)
						return false;

					if (p / (UPGRADE_ITEMS_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsStoneInventoryPosition())
		{
			if (bCell < STONE_INVENTORY_SLOT_START)
				return false;

			if (bCell > STONE_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (STONE_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= STONE_INVENTORY_MAX_NUM)
						return false;

					if (p / (STONE_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsBoxInventoryPosition())
		{
			if (bCell < BOX_INVENTORY_SLOT_START)
				return false;

			if (bCell > BOX_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (BOX_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= BOX_INVENTORY_MAX_NUM)
						return false;

					if (p / (BOX_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsEfsunInventoryPosition())
		{
			if (bCell < EFSUN_INVENTORY_SLOT_START)
				return false;

			if (bCell > EFSUN_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (EFSUN_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= EFSUN_INVENTORY_MAX_NUM)
						return false;

					if (p / (EFSUN_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
		else if (Cell.IsCicekInventoryPosition())
		{
			if (bCell < CICEK_INVENTORY_SLOT_START)
				return false;

			if (bCell > CICEK_INVENTORY_SLOT_END)
				return false;

			if (m_PlayerSlots->bItemGrid[bCell] == (UINT)iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				UINT bPage = bCell / (CICEK_INVENTORY_MAX_NUM / 3);

				do
				{
					UINT p = bCell + (5 * j);

					if (p >= CICEK_INVENTORY_MAX_NUM)
						return false;

					if (p / (CICEK_INVENTORY_MAX_NUM / 3) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
		}
#endif
		else if (bCell >= INVENTORY_MAX_NUM)
			return false;

		if (m_PlayerSlots->bItemGrid[bCell])
		{
			if (m_PlayerSlots->bItemGrid[bCell] == iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
						return false;

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
						return false;

					if (m_PlayerSlots->bItemGrid[p])
						if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
			else
				return false;
		}

		if (1 == bSize)
			return true;
		else
		{
			int j = 1;
			BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

			do
			{
				BYTE p = bCell + (5 * j);

				if (p >= INVENTORY_MAX_NUM)
					return false;

				if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
					return false;

				if (m_PlayerSlots->bItemGrid[p])
					if (m_PlayerSlots->bItemGrid[p] != iExceptionCell)
						return false;
			} while (++j < bSize);

			return true;
		}
	}
	break;
	case DRAGON_SOUL_INVENTORY:
	{
		WORD wCell = Cell.cell;
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			return false;

		iExceptionCell++;

		if (m_PlayerSlots->wDSItemGrid[wCell])
		{
			if (m_PlayerSlots->wDSItemGrid[wCell] == iExceptionCell)
			{
				if (bSize == 1)
					return true;

				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						return false;

					if (m_PlayerSlots->wDSItemGrid[p])
						if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
							return false;
				} while (++j < bSize);

				return true;
			}
			else
				return false;
		}

		if (1 == bSize)
			return true;
		else
		{
			int j = 1;

			do
			{
				int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

				if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					return false;

				if (m_PlayerSlots->bItemGrid[p])
					if (m_PlayerSlots->wDSItemGrid[p] != iExceptionCell)
						return false;
			} while (++j < bSize);

			return true;
		}
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
	{
		UINT wCell = Cell.cell;
		if (wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			return false;
		}

		if (m_pointsInstant.pBuffEquipmentItem[wCell])
		{
			return false;
		}

		return true;
	}
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
	{
		WORD wCell = Cell.cell;
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			return false;
		}

		if (m_PlayerSlots->pSwitchbotItems[wCell])
		{
			return false;
		}

		return true;
	}
#endif
	}

	return false;
}

int CHARACTER::GetEmptyInventory(BYTE size) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;
	return -1;
}

int CHARACTER::GetEmptyInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > INVENTORY_MAX_NUM)
		return -1;
	
	for (WORD i = index; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyUpgradeItemsInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 548)
		return -1;

	for (WORD i = index; i < 548; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptySkillBookInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 413)
		return -1;
	
	for (WORD i = index; i < 413; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyStoneInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 683)
		return -1;
	
	for (WORD i = index; i < 683; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyBoxInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 818)
		return -1;
	
	for (WORD i = index; i < 818; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyEfsunInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 953)
		return -1;
	
	for (WORD i = index; i < 953; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyCicekInventoryFromIndex(WORD index, BYTE itemSize) const //SPLIT ITEMS
{
	if (index > 1088)
		return -1;
	
	for (WORD i = index; i < 1088; ++i)
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), itemSize))
			return i;
	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
int CHARACTER::GetEmptySkillBookInventory(BYTE size) const
{
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyUpgradeItemsInventory(BYTE size) const
{
	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyStoneInventory(BYTE size) const
{
	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyBoxInventory(BYTE size) const
{
	for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyEfsunInventory(BYTE size) const
{
	for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}

int CHARACTER::GetEmptyCicekInventory(BYTE size) const
{
	for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return -1;
}
#endif

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_PlayerSlots->wDSItemGrid, m_PlayerSlots->wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

// #ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
// 	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
// #else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
// #endif
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}

	}

	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if (ch != NULL)
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);
	int cost2 = ComputeRefineFee(prt->cost2);
	int cost3 = ComputeRefineFee(prt->cost3);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹«·á °³·® ±âÈ¸´Â 20 ÀÌÇÏÀÇ ¹«±â¸¸ °¡´ÉÇÕ´Ï´Ù"));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³·®À» ÇÏ±â À§ÇÑ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù."));
		return false;
	}

	if (GetCheque() < cost2)
	{
		ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o Won, aby móc ulepszyæ ten przedmiot.");
		return false;
	}

	if (GetPktOsiag() < cost3)
	{
		ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o PO, aby móc ulepszyæ ten przedmiot.");
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³·®À» ÇÏ±â À§ÇÑ Àç·á°¡ ºÎÁ·ÇÕ´Ï´Ù."));
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number(1, 100);

#ifdef TITLE_SYSTEM_BYLUZER
	if (m_pTitle[17].active == 2)
		prob -= 10;
	else if (m_pTitle[16].active == 2)
		prob -= 5;
	else if (m_pTitle[15].active == 2)
		prob -= 3;
#endif
	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			PointChange(POINT_CHEQUE, -cost2);
			PointChange(POINT_PKT_OSIAG, -cost3);
			sys_log(0, "PayPee End %d", cost);
#ifdef WEEKLY_RANK_BYLUZER
			PointChange(POINT_WEEKLY6, 1);
			CheckWeekly();
#endif
		}
		else
		{
			// DETAIL_REFINE_LOG

			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
		PayRefineFee(cost);
		PointChange(POINT_CHEQUE, -cost2);
		PointChange(POINT_PKT_OSIAG, -cost3);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL = 3,
	YAGONG_SCROLL = 4,
	MEMO_SCROLL = 5,
	BDRAGON_SCROLL = 6,
	METAL_10 = 9,
	METAL_20 = 8,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	LPITEM absorbedItem = NULL;
	bool isSashAbsorbedUpgrade = false;
	DWORD result_vnum = 0, result_fail_vnum = 0;
	if ((item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_ACCE) && item->GetSocket(ACCE_ABSORBED_SOCKET) != 0)
	{
		absorbedItem = ITEM_MANAGER::instance().CreateItem(item->GetSocket(ACCE_ABSORBED_SOCKET), 1);
		result_vnum = absorbedItem->GetRefinedVnum();
		result_fail_vnum = absorbedItem->GetRefineFromVnum();

		isSashAbsorbedUpgrade = true;

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "<test_server>SASH DoRefineWithScroll: vnum: %d / result: %d / result_fail: %d / refine_set: %d", absorbedItem->GetVnum(), result_vnum, result_fail_vnum, absorbedItem->GetRefineSet());
	}
	else
	{
		result_vnum = item->GetRefinedVnum();
		result_fail_vnum = item->GetRefineFromVnum();
	}

	TItemTable* pProto;
	const TRefineTable* prt;
	if (absorbedItem && isSashAbsorbedUpgrade)
	{
		pProto = ITEM_MANAGER::instance().GetTable(absorbedItem->GetRefinedVnum());
		prt = CRefineManager::instance().GetRefineRecipe(absorbedItem->GetRefineSet());
	}
	else
	{
		pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());
		prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());
	}
#else
	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());
	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();
#endif
	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}
	// MUSIN_SCROLL
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ °³·®¼­·Î ´õ ÀÌ»ó °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ °³·®¼­·Î °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀ¸·Î °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³·®À» ÇÏ±â À§ÇÑ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù."));
		return false;
	}

	if (GetCheque() < prt->cost2)
	{
		ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o Won, aby móc ulepszyæ ten przedmiot.");
		return false;
	}

	if (GetPktOsiag() < prt->cost3)
	{
		ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o PO, aby móc ulepszyæ ten przedmiot.");
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³·®À» ÇÏ±â À§ÇÑ Àç·á°¡ ºÎÁ·ÇÕ´Ï´Ù."));
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;
#ifdef TITLE_SYSTEM_BYLUZER
	if (m_pTitle[17].active == 2)
		success_prob += 10;
	else if (m_pTitle[16].active == 2)
		success_prob += 5;
	else if (m_pTitle[15].active == 2)
		success_prob += 3;
#endif
	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN ||
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL ||
		pkItemScroll->GetValue(0) == METAL_10 ||
		pkItemScroll->GetValue(0) == METAL_20 ||
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == HYUNIRON_CHN) {} // @fixme121
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;
	
		if (pkItemScroll->GetValue(0) == METAL_10)
			bDestroyWhenFail = true;
		
		if (pkItemScroll->GetValue(0) == METAL_20)
			bDestroyWhenFail = true;

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == REFINE_TYPE_METAL_10)
		{
			szRefineType = "METAL_10";
		}
		else if (pkItemScroll->GetValue(0) == REFINE_TYPE_METAL_20)
		{
			szRefineType = "METAL_20";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == METAL_10)
	{
		success_prob = MIN(100, success_prob+10);
	}
	else if (pkItemScroll->GetValue(0) == METAL_20)
	{
		success_prob = MIN(100, success_prob+20);
	}

	if (szRefineType != "HYUNIRON") {
		pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);
	}

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);
		if (pkNewItem)
		{
			BYTE bCell = item->GetCell();
			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (isSashAbsorbedUpgrade)
			{
				LogManager::instance().ItemLog(this, item, "SASH ITEM REFINE SUCCESS", pkNewItem->GetName());

				item->SetSocket(ACCE_ABSORBED_SOCKET, pkNewItem->GetVnum());
				ITEM_MANAGER::instance().FlushDelayedSave(item);
			}
			else
			{
				LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
				
#ifdef ENABLE_GLOVE_SYSTEM
				if (item != NULL) {
					if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_GLOVE) {
						for (int i = 0; i < 3; i++) {
							int adjustedValue = CGloveBonusManager::GetRefineNewGloveBonus(item->GetAttributeType(i), item->GetAttributeValue(i));
							pkNewItem->SetForceAttribute(i, item->GetAttributeType(i), adjustedValue);
						}
					}
				} else {
					return false;
				}
#endif
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
				pkNewItem->AttrLog();
			}
#else
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			
#ifdef ENABLE_GLOVE_SYSTEM
			if (item != NULL) {
				if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_GLOVE) {
					for (int i = 0; i < 3; i++) {
						int adjustedValue = CGloveBonusManager::GetRefineNewGloveBonus(item->GetAttributeType(i), item->GetAttributeValue(i));
						pkNewItem->SetForceAttribute(i, item->GetAttributeType(i), adjustedValue);
					}
				}
			} else {
				return false;
			}
#endif
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
#endif


// 			int refine_vnum = item->GetVnum() - 23000;
// 			for (int i = 0; i < 3; ++i) {
// 				AddAffect(AFFECT_GLOVE_BONUS, aApplyInfo[item->GetAttributeType(i)].bPointType, (GLOVE_BONUS_TABLE[item->GetAttributeType(i)]/9)*refine_vnum, AFF_NONE, 60*60*60*365, 0, false);

			//PointChange(POINT_GOLD, -prt->cost);
			PointChange(POINT_CHEQUE, -prt->cost2);
			PointChange(POINT_PKT_OSIAG, -prt->cost3);
			PayRefineFee(prt->cost);
#ifdef WEEKLY_RANK_BYLUZER
			PointChange(POINT_WEEKLY6, 1);
			CheckWeekly();
#endif
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (!isSashAbsorbedUpgrade)
			{
				ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
				LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

				BYTE bCell = item->GetCell();

				DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
				NotifyRefineFail(this, item, szRefineType, -1);
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

				pkNewItem->AttrLog();
			}
			else
			{
				LogManager::instance().ItemLog(this, pkNewItem, "SASH ITEM REFINE FAIL", pkNewItem->GetName());
				DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);

				NotifyRefineFail(this, pkNewItem, szRefineType, -1);

				item->SetSocket(ACCE_ABSORBED_SOCKET, pkNewItem->GetVnum());
				ITEM_MANAGER::instance().FlushDelayedSave(item);

				M2_DESTROY_ITEM(pkNewItem);
				M2_DESTROY_ITEM(absorbedItem);
			}
#else
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();
#endif

			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee(prt->cost);
			PointChange(POINT_CHEQUE, -prt->cost2);
			PointChange(POINT_PKT_OSIAG, -prt->cost3);
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);

		PayRefineFee(prt->cost);
		PointChange(POINT_CHEQUE, -prt->cost2);
		PointChange(POINT_PKT_OSIAG, -prt->cost3);
	}

	return true;
}

bool CHARACTER::RefineInformation(WORD bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	LPITEM item = GetInventoryItem(bCell);

	if (!item)
		return false;

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç±Í Å¸¿ö ¿Ï·á º¸»óÀº ÇÑ¹ø±îÁö »ç¿ë°¡´ÉÇÕ´Ï´Ù."));
		return false;
	}
	// END_OF_REFINE_COST

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	LPITEM absorbedItem = NULL;
	bool isSashAbsorbedUpgrade = false;
	if ((item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_ACCE) && item->GetSocket(ACCE_ABSORBED_SOCKET) != 0)
	{
		absorbedItem = ITEM_MANAGER::instance().CreateItem(item->GetSocket(ACCE_ABSORBED_SOCKET), 1);

		isSashAbsorbedUpgrade = true;

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "<test_server> SASH RefineInformation: vnum: %d / result_vnum: %d / refine_set: %d", absorbedItem->GetVnum(), absorbedItem->GetRefinedVnum(), absorbedItem->GetRefineSet());
	}
#endif

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (absorbedItem && isSashAbsorbedUpgrade)
	{
		p.src_vnum = absorbedItem->GetVnum();
		p.result_vnum = absorbedItem->GetRefinedVnum();
	}
	else
	{
		p.src_vnum = item->GetVnum();
		p.result_vnum = item->GetRefinedVnum();
	}
#else
	p.result_vnum = item->GetRefinedVnum();
#endif
	p.type = bType;
#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(&p.aApplyRandom, item->GetNextRandomApplies(), sizeof(p.aApplyRandom));
#endif

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº ÀÌ ¹æ½ÄÀ¸·Î´Â °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°°Àº °³·®¼­¸¦ ÇÕÄ¥ ¼ö´Â ¾ø½À´Ï´Ù."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ãàº¹ÀÇ ¼­¿Í ÇöÃ¶À» ÇÕÄ¥ ¼ö ÀÖ½À´Ï´Ù."));
				return false;
			}
		}
	}

	CRefineManager& rm = CRefineManager::instance();

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const TRefineTable* prt;
	if (absorbedItem && isSashAbsorbedUpgrade)
		prt = rm.GetRefineRecipe(absorbedItem->GetRefineSet());
	else
		prt = rm.GetRefineRecipe(item->GetRefineSet());
#else
	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());
#endif

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹«·á °³·® ±âÈ¸´Â 20 ÀÌÇÏÀÇ ¹«±â¸¸ °¡´ÉÇÕ´Ï´Ù"));
			return false;
		}
		p.cost = 0;
		p.cost2 = 0;
		p.cost3 = 0;
	}
	else {
		p.cost = ComputeRefineFee(prt->cost);
		p.cost2 = ComputeRefineFee(prt->cost2);
		p.cost3 = ComputeRefineFee(prt->cost3);
	}

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;
#ifdef TITLE_SYSTEM_BYLUZER
	if (m_pTitle[17].active == 2)
		p.prob += 10;
	else if (m_pTitle[16].active == 2)
		p.prob += 5;
	else if (m_pTitle[15].active == 2)
		p.prob += 3;
#endif
	if (bType == REFINE_TYPE_METAL_10)
	{
		p.prob = MIN(100, prt->prob+10);
	}
	
	if (bType == REFINE_TYPE_METAL_20)
	{
		p.prob = MIN(100, prt->prob+20);
	}
	
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// MUSIN_SCROLL
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		// END_OF_MUSIN_SCROLL
		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
		{
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		}
		else if (pkItem->GetValue(0) == METAL_10)
		{
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_METAL_10, pkItem->GetCell());
		}
		else if (pkItem->GetValue(0) == METAL_20)
		{
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_METAL_20, pkItem->GetCell());
		}
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

#ifdef ENABLE_EXTENDED_SOCKETS
		for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
#ifdef ENABLE_EXTENDED_SOCKETS
			for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);

					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»©³¾ ¼ö ÀÖ´Â ¸ÞÆ¾¼®ÀÌ ¾ø½À´Ï´Ù."));
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("kill_campfire_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±â¾ïÇØ µÑ ¼ö ¾ø´Â À§Ä¡ ÀÔ´Ï´Ù."));
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;

	case 301:
	case 302:
	case 303:
	case 304:
		if (GetLevel() < 90)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛÀÇ ·¹º§ Á¦ÇÑº¸´Ù ·¹º§ÀÌ ³·½À´Ï´Ù."));
			return;
		}
		else
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±â¾ïµÈ À§Ä¡°¡ Å¸Á¦±¹¿¡ ¼ÓÇØ ÀÖ¾î¼­ ±ÍÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
	case 0:
	case 1:
	case 2:
		ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
		break;
	default:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°©¿ÊÀ» ¹þ¾î¾ß °³ÀÎ »óÁ¡À» ¿­ ¼ö ÀÖ½À´Ï´Ù."));
		break;
	}
#endif
}

// MYSHOP_PRICE_LIST
#ifdef ENABLE_LONG_LONG
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#else
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}
#endif

//

//
void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)

		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[idx].dwVnum, pInfo[idx].dwPrice);
	}

	__OpenPrivateShop();
}

//

//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	}
	else {
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST

int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT = 30;
	static const int WARP_MIN_LIFE_PERCENT = 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife = ch->GetHP();
		const int needPercent = WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³²Àº »ý¸í·Â ¾çÀÌ ¸ðÀÚ¶ó »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return -1;
		}

		consumeLife = needLife;

		const int minPercent = WARP_MIN_LIFE_PERCENT;
		const int minLife = ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³²Àº Á¤½Å·Â ¾çÀÌ ¸ðÀÚ¶ó »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return -1;
	}

	return needSP;
}

struct TGachaItems {
	short itemCount;
	char szName[ITEM_NAME_MAX_LEN + 1];
};

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (GetLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛÀÇ ·¹º§ Á¦ÇÑº¸´Ù ·¹º§ÀÌ ³·½À´Ï´Ù."));
				return false;
			}
			break;

		case LIMIT_REAL_TIME_START_FIRST_USE:
			iLimitRealtimeStartFirstUseFlagIndex = i;
			break;

		case LIMIT_TIMER_BASED_ON_WEAR:
			//iLimitTimerBasedOnWearFlagIndex = i;
			break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if (CArenaManager::instance().IsLimitedItem(GetMapIndex(), item->GetVnum()) == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, "Affects are not loaded yet!");
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you have no equipped belt.");
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you don't upgrade your belt.");
			return false;
		}
	}
	// @fixme141 END

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

#ifdef __ITEM_SHOP__
			if (item->GetSocket(0) > 0)
				duration += item->GetSocket(0);
#endif
			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

	switch (item->GetType())
	{
	case ITEM_HAIR:
		return ItemProcess_Hair(item, wDestCell);
#ifdef ENABLE_NEW_PET_SYSTEM
	case ITEM_NEW_PET:
	case ITEM_NEW_PET_EQ:
#ifdef ENABLE_EXTRABONUS_SYSTEM
		if (!item->IsEquipped()) {
			EquipItem(item);
			CheckPetExtraBonus(item, 0);
		}
		else {
			UnequipItem(item);
			CheckPetExtraBonus(item, 1);
		}
		break;
#else
		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;
#endif
#endif
	case ITEM_POLYMORPH:
		return ItemProcess_Polymorph(item);

	case ITEM_QUEST:
#ifdef ENABLE_QUEST_DND_EVENT
		if (IS_SET(item->GetFlag(), ITEM_FLAG_APPLICABLE))
		{
			LPITEM item2;

			if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			quest::CQuestManager::instance().DND(GetPlayerID(), item, item2, false);
			return true;
		}
#endif

#ifdef __AUTO_QUQUE_ATTACK__
		if (item->GetVnum() == 61400 || item->GetVnum() == 61401 || item->GetVnum() == 61402 || item->GetVnum() == 61403)
		{
			if (item->isLocked() || item->IsExchanging())
				return false;
			else if (FindAffect(AFFECT_AUTO_METIN_FARM)) {
				ChatPacket(CHAT_TYPE_INFO, "Masz ju¿ aktywnego VIPA.");
				return false;
			}
			ChatPacket(CHAT_TYPE_INFO, "Kolejkowanie (VIP) dodane.");
			AddAffect(AFFECT_AUTO_METIN_FARM, 0, 0, AFF_NONE, item->GetValue(0), 0, false);
			item->SetCount(item->GetCount() - 1);
			return true;
		}
#endif


		if (GetArena() != NULL || IsObserverMode() == true)
		{
			if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
		}

		if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
		{
			if (item->GetSIGVnum() == 0)
			{
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
			}
			else
			{
				quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
			}
		}
		break;

	case ITEM_CAMPFIRE:
	{
		float fx, fy;
		GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

		LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX() + fx), (long)(GetY() + fy));

		if (!tree)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸ð´ÚºÒÀ» ÇÇ¿ï ¼ö ¾ø´Â ÁöÁ¡ÀÔ´Ï´Ù."));
			return false;
		}

		if (tree->IsAttr((long)(GetX() + fx), (long)(GetY() + fy), ATTR_WATER))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹° ¼Ó¿¡ ¸ð´ÚºÒÀ» ÇÇ¿ï ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

		LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX() + fx), (long)(GetY() + fy), 0, false, number(0, 359));

		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = campfire;

		campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

		item->SetCount(item->GetCount() - 1);
	}
	break;

	case ITEM_UNIQUE:
	{
		switch (item->GetSubType())
		{
		case USE_ABILITY_UP:
		{
			switch (item->GetValue(0))
			{
			case APPLY_MOV_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_STR:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEX:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CON:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_INT:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CAST_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_RESIST_MAGIC:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
					item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEF_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
					item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;
			}
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		item->SetCount(item->GetCount() - 1);
		break;

		default:
		{
			if (item->GetSubType() == USE_SPECIAL)
			{
				sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

				switch (item->GetVnum())
				{
				case 71049:
					if (g_bEnableBootaryCheck)
					{
						if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
						{
							UseSilkBotary();
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³ÀÎ »óÁ¡À» ¿­ ¼ö ¾ø´Â Áö¿ªÀÔ´Ï´Ù"));
						}
					}
					else
					{
						UseSilkBotary();
					}
					break;
				}
			}
			else
			{
				if (!item->IsEquipped())
					EquipItem(item);
				else
					UnequipItem(item);
			}
		}
		break;
		}
	}
	break;

	case ITEM_COSTUME:
	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_ROD:
	case ITEM_RING:
	case ITEM_BELT:
#ifdef ENABLE_ARTEFAKT_SYSTEM
	case ITEM_ARTEFAKT:
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	case ITEM_MOUNT_EQUIPMENT:
#endif
		// MINING
	case ITEM_PICK:
		// END_OF_MINING

		if (item->GetType() == ITEM_WEAPON 
		|| (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_BODY) 
		|| (item->GetType() == ITEM_COSTUME && (item->GetSubType() == COSTUME_ACCE || item->GetSubType() == COSTUME_BODY || item->GetSubType() == COSTUME_HAIR)))
		{
			int iPulse = thecore_pulse();
			if (iPulse - GetEquipItemsTime() < PASSES_PER_SEC(3))
			{
				int time = (3 - (iPulse - GetEquipItemsTime()) / PASSES_PER_SEC(1));
				ChatPacket(CHAT_TYPE_INFO, "Poczekaj zanim to zrobisz!");
				return false;
			}

			m_iEquipItemsCount++;
			if (get_global_time() < m_iEquipItemsLastTime)
			{
				if (GetEquipItemsCount() > 5)
				{
					SetEquipItemsTime();
					m_iEquipItemsCount = 0;
				}
			}
			else
				m_iEquipItemsCount = 0;

			m_iEquipItemsLastTime = get_global_time() + 2;
		}

		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;

#ifdef ENABLE_SYSTEM_RUNE
	case ITEM_RUNE:
	case ITEM_RUNE_RED:
	case ITEM_RUNE_BLUE:
	case ITEM_RUNE_GREEN:
	case ITEM_RUNE_YELLOW:
	case ITEM_RUNE_BLACK:
#ifdef ENABLE_EXTRABONUS_SYSTEM
		if (!item->IsEquipped()) {
			EquipItem(item);
			CheckRuneExtraBonus(item, 0);
		}
		else {
			UnequipItem(item);
			CheckRuneExtraBonus(item, 1);
		}
		break;
#else
		if (!item->IsEquipped()) {
			EquipItem(item);
		}
		else {
			UnequipItem(item);
		}
		break;
#endif
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	case ITEM_TITLE:
	{
		int title_id = item->GetValue(0);
		quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
		if (pPC->IsRunning())
			return false;
		if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
		{
			ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
			return false;
		}
		if (!CanWarp())
		{
			ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
			return false;
		}
		if (m_pTitle[title_id].active != 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "Posiadasz ju¿ ten tytu³");
			return false;
		}
		m_pTitle[title_id].active = 1;
		item->SetCount(item->GetCount() - 1);
		TitleSendGeneralInfo();
		ChatPacket(CHAT_TYPE_INFO, "Tytu³ zosta³ dodany do okna tytu³ów!");
	}
	break;
#endif

#ifdef ENABLE_PET_SYSTEM_EX
	case ITEM_PET:
		switch (item->GetSubType())
		{
		case PET_UPBRINGING: //1
		case PET_PAY: //12
		{
			auto mobVnum = item->GetValue(0);
			if (!mobVnum)
			{
				ChatPacket(CHAT_TYPE_INFO, "Item Pet %d with no mob in value0", item->GetVnum());
				return false;
			}
			auto* petSystem = GetPetSystem();
			if (!petSystem)
				return false;

			if (petSystem->CountSummoned())
				petSystem->UnsummonAll();
			else
			{
				// summon
				constexpr bool bFromFar = false;
				const auto* pkMob = CMobManager::instance().Get(mobVnum);
				const auto* petName = (pkMob) ? pkMob->GetLocaleName() : "";
				petSystem->Summon(mobVnum, item, petName, bFromFar);
				// spawn effect
				constexpr const char* spawn_effect_file_name = "d:\\ymir work\\effect\\etc\\appear_die\\npc2_appear.mse";
				auto* petActor = petSystem->GetByVnum(mobVnum);
				if (petActor && petActor->GetCharacter() && item->GetSubType() == PET_PAY)
					petActor->GetCharacter()->SpecificEffectPacket(spawn_effect_file_name);
			}
		}
		break;

		case PET_EGG: //0
		case PET_BAG: //2
		case PET_FEEDSTUFF: //3
		case PET_SKILL: //4
		case PET_SKILL_DEL_BOOK: //5
		case PET_NAME_CHANGE: //6
		case PET_EXPFOOD: //7
		case PET_SKILL_ALL_DEL_BOOK: //8
		case PET_EXPFOOD_PER: //9
		case PET_ATTR_DETERMINE: //10
		case PET_ATTR_CHANGE: //11
		case PET_PRIMIUM_FEEDSTUFF: //13
			break;
		}
		break;
#endif
	case ITEM_DS:
	{
		if (!item->IsEquipped())
			return false;
		return DSManager::instance().PullOut(this, NPOS, item);
		break;
	}
	case ITEM_SPECIAL_DS:
		if (!item->IsEquipped())
			EquipItem(item);
		else
			UnequipItem(item);
		break;

#ifdef __ENABLE_TOGGLE_ITEMS__
	case ITEM_TOGGLE:
	{
		if (!CanHandleItem())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot actually use it."));
			return false;
		}

		if (item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "[TEST] Deactivating Toggle Item");
			}

			item->DeactivateToggleItem();
			return true;
		}

		if (item->GetCount() > 1)
		{
			int pos = GetEmptyInventory(item->GetSize());

			if (-1 == pos)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
				return false;
			}

			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
			if (!item2)
				return false;

			item->CopyAttributeTo(item2);
			item->CopySocketTo(item2);
			item->SetCount(item->GetCount() - 1);
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));
			item = item2;
		}

		const int32_t toggleType = item->GetValue(TOGGLE_TYPE_VALUE);
		if (FindActiveToggleItem(toggleType, item))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already activated toggle item."));
			return false;
		}

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_INFO, "[TEST] Activating Toggle Item");
		}

		item->ActivateToggleItem(true);
		return true;
	}
	break;
#endif

	case ITEM_FISH:
	{
		if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
			return false;
		}
#ifdef ENABLE_NEWSTUFF
		else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
			return false;
		}
#endif

		if (item->GetSubType() == FISH_ALIVE)
			fishing::UseFish(this, item);
	}
	break;

	case ITEM_TREASURE_BOX:
	{
		return false;

	}
	break;

	case ITEM_TREASURE_KEY:
	{
		LPITEM item2;

		if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
			return false;

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			return false;

		if (item2->GetType() != ITEM_TREASURE_BOX)
		{
			ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¿­¼è·Î ¿©´Â ¹°°ÇÀÌ ¾Æ´Ñ°Í °°´Ù."));
			return false;
		}

		if (item->GetValue(0) == item2->GetValue(0))
		{
			DWORD dwBoxVnum = item2->GetVnum();
			std::vector <DWORD> dwVnums;
			std::vector <DWORD> dwCounts;
			std::vector <LPITEM> item_gets(0);
			int count = 0;

			if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
			{
				ITEM_MANAGER::instance().RemoveItem(item);
				ITEM_MANAGER::instance().RemoveItem(item2);

				for (int i = 0; i < count; i++) {
					switch (dwVnums[i])
					{
					case CSpecialItemGroup::GOLD:
						#if defined(__CHATTING_WINDOW_RENEWAL__)
						ChatPacket(CHAT_TYPE_MONEY_INFO, LC_TEXT("µ· %d ³ÉÀ» È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
						#else
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ· %d ³ÉÀ» È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
						#endif
						break;
					case CSpecialItemGroup::EXP:
						#if defined(__CHATTING_WINDOW_RENEWAL__)
						ChatPacket(CHAT_TYPE_EXP_INFO, LC_TEXT("»óÀÚ¿¡¼­ ºÎÅÍ ½ÅºñÇÑ ºûÀÌ ³ª¿É´Ï´Ù."));
						ChatPacket(CHAT_TYPE_EXP_INFO, LC_TEXT("%dÀÇ °æÇèÄ¡¸¦ È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
						#else
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ºÎÅÍ ½ÅºñÇÑ ºûÀÌ ³ª¿É´Ï´Ù."));
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÀÇ °æÇèÄ¡¸¦ È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
						#endif
						break;
					case CSpecialItemGroup::MOB:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ¸ó½ºÅÍ°¡ ³ªÅ¸³µ½À´Ï´Ù!"));
						break;
					case CSpecialItemGroup::SLOW:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â »¡°£ ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ ¿òÁ÷ÀÌ´Â ¼Óµµ°¡ ´À·ÁÁ³½À´Ï´Ù!"));
						break;
					case CSpecialItemGroup::DRAIN_HP:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ°¡ °©ÀÚ±â Æø¹ßÇÏ¿´½À´Ï´Ù! »ý¸í·ÂÀÌ °¨¼ÒÇß½À´Ï´Ù."));
						break;
					case CSpecialItemGroup::POISON:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â ³ì»ö ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ µ¶ÀÌ ¿Â¸öÀ¸·Î ÆÛÁý´Ï´Ù!"));
						break;
#ifdef ENABLE_WOLFMAN_CHARACTER
					case CSpecialItemGroup::BLEEDING:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â ³ì»ö ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ µ¶ÀÌ ¿Â¸öÀ¸·Î ÆÛÁý´Ï´Ù!"));
						break;
#endif
					case CSpecialItemGroup::MOB_GROUP:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ¸ó½ºÅÍ°¡ ³ªÅ¸³µ½À´Ï´Ù!"));
						break;
					default:
						if (item_gets[i])
						{
							#if defined(__CHATTING_WINDOW_RENEWAL__)
							if (dwCounts[i] > 1)
								ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ %d °³ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName(), dwCounts[i]);
							else
								ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName());
							#else
							if (dwCounts[i] > 1)
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ %d °³ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName(), dwCounts[i]);
							else
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName());
							#endif
						}
					}
				}
			}
			else
			{
				ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¿­¼è°¡ ¸ÂÁö ¾Ê´Â °Í °°´Ù."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¿­¼è°¡ ¸ÂÁö ¾Ê´Â °Í °°´Ù."));
			return false;
		}
	}
	break;

	case ITEM_GIFTBOX:
	{
		OpenChestItem(item);
	}
	break;

	case ITEM_SKILLFORGET:
	{
		if (!item->GetSocket(0))
		{
			ITEM_MANAGER::instance().RemoveItem(item);
			return false;
		}

		DWORD dwVnum = item->GetSocket(0);

		if (SkillLevelDown(dwVnum))
		{
			ITEM_MANAGER::instance().RemoveItem(item);
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("½ºÅ³ ·¹º§À» ³»¸®´Âµ¥ ¼º°øÇÏ¿´½À´Ï´Ù."));
		}
		else
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("½ºÅ³ ·¹º§À» ³»¸± ¼ö ¾ø½À´Ï´Ù."));
	}
	break;

	case ITEM_SKILLBOOK:
	{
		if (IsPolymorphed())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
		DWORD dwVnum = 0;

		if (item->GetVnum() == 50300)
		{
			dwVnum = item->GetSocket(0);
		}
		else
		{
			dwVnum = item->GetValue(0);
		}

		if (0 == dwVnum)
		{
			ITEM_MANAGER::instance().RemoveItem(item);

			return false;
		}

		if (true == LearnSkillByBook(dwVnum))
		{
#ifdef ENABLE_BOOKS_STACKFIX
			item->SetCount(item->GetCount() - 1);
#else
			ITEM_MANAGER::instance().RemoveItem(item);
#endif

			int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

			if (distribution_test_server)
				iReadDelay /= 3;

			SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
		}
	}
	break;

	case ITEM_USE:
	{
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		switch(item->GetVnum())
		{
			case 80030:
			case 80031:
			case 80032:
			case 80034:
			case 80035:
			case 80036:
			{
				if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
				{
					ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
					return false;
				}
				
				int pkt_osiag = item->GetValue(0);
				PointChange(POINT_PKT_OSIAG, pkt_osiag );
				
				ChatPacket(CHAT_TYPE_INFO, "[Kupon PO] Otrzyma³eœ %d PO.", pkt_osiag);
				item->SetCount(item->GetCount() - 1);
				break;
			}
		}
#endif
		switch(item->GetVnum())
		{
			case 80037:
			case 80038:
			case 80039:
			case 80040:
			{
				if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
				{
					ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
					return false;
				}
				
				int SM = item->GetValue(0);
				if (SM == 0)
					return false;

				const auto dwCash = GetCash();
				SetCash(dwCash+SM);
				
				ChatPacket(CHAT_TYPE_INFO, "[Kupon SM] Otrzyma³eœ %d SM.", SM);
				item->SetCount(item->GetCount() - 1);
				break;
			}
		}
		switch (item->GetVnum())
		{
			case 80033:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
						return false;
					}
					int Exp = item->GetValue(0);
					if (PetGetLevel() >= 40 && PetGetLevel() < 70) {
						if (GetWear(WEAR_NEW_PET)) {
							PetSetExpPercent(Exp);
							item->SetCount(item->GetCount() - 1);
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, "Aby u¿yæ tego przedmiotu musisz mieæ przywo³anego zwierzaka!");
							return false;
						}
					} else {
						ChatPacket(CHAT_TYPE_INFO, "Tej fiolki mo¿na u¿yæ tylko od 40 do 70 poziomu.");
						return false;
					}
					break;
				}
			case 80056:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
						return false;
					}
					int Exp = item->GetValue(0);
					if (PetGetLevel() >= 70 && PetGetLevel() < 110) {
						if (GetWear(WEAR_NEW_PET)) {
							PetSetExpPercent(Exp);
							item->SetCount(item->GetCount() - 1);
						}
						else {
							ChatPacket(CHAT_TYPE_INFO, "Aby u¿yæ tego przedmiotu musisz mieæ przywo³anego zwierzaka!");
							return false;
						}
					} else {
						ChatPacket(CHAT_TYPE_INFO, "Tej fiolki mo¿na u¿yæ tylko od 70 do 110 poziomu.");
						return false;
					}
					break;
				}
		}
#ifdef ENABLE_COLLECT_WINDOW
		switch(item->GetVnum())
		{
			case 72348:
			case 72349:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
						return false;
					}
					int val = item->GetValue(0);
					int type = item->GetValue(1);
					if (type == 0) {
						time_t item_delay = GetQuestFlag("biologist.item_delay");
						time_t current_time = get_global_time();
						time_t remaining_time = item_delay - current_time;

						time_t reduced_time = remaining_time - (remaining_time * val / 100);

						if (reduced_time > 10) {
							SetQuestFlag("biologist.item_delay", current_time + reduced_time);
							
							ChatPacket(CHAT_TYPE_COMMAND, "UpdateTime 0 %d", reduced_time);
							item->SetCount(item->GetCount() - 1);

							ChatPacket(CHAT_TYPE_INFO, "Czas oczekiwania na oddanie przedmiotu zosta³ pomyœlnie skrócony.");
							ConvertTime(reduced_time);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Czas oczekiwania zosta³ ju¿ skrócony maksymalnie.");
							return false;
						}
					} else if (type == 1) {
						int collector_state = GetQuestFlag("collector.state");

						std::string delay_flag_key = "collector.item_delay" + std::to_string(collector_state);

						time_t item_delay = GetQuestFlag(delay_flag_key);
						time_t current_time = get_global_time();
						time_t remaining_time = item_delay - current_time;

						time_t reduced_time = remaining_time - (remaining_time * val / 100); 

						if (reduced_time > 10) {
							SetQuestFlag(delay_flag_key, current_time + reduced_time);
							
							ChatPacket(CHAT_TYPE_COMMAND, "UpdateTime 1 %d", reduced_time);
							item->SetCount(item->GetCount() - 1);

							ChatPacket(CHAT_TYPE_INFO, "Czas oczekiwania na oddanie przedmiotu zosta³ pomyœlnie skrócony.");
							ConvertTime(reduced_time);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Czas oczekiwania zosta³ ju¿ skrócony maksymalnie.");
							return false;
						}
					}
					break;
				}

			case 71036:
			case 71037:
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
						return false;
					}
					int val = item->GetValue(0);
					int type = item->GetValue(1);
					int max_increase = item->GetValue(2);
					if (type == 0) {
						int take_chance = GetQuestFlag("biologist.take_chance");
						if (take_chance == max_increase || take_chance > max_increase) {
							ChatPacket(CHAT_TYPE_INFO, "Szansa na oddanie zosta³a ju¿ zwiêkszona maksymalnie!");
							return false;
						}
						SetQuestFlag("biologist.take_chance", take_chance+val);
						item->SetCount(item->GetCount()-1);

						ChatPacket(CHAT_TYPE_INFO, "Szansa na oddanie zosta³a zwiêkszona o 25%");

						int new_chance = GetQuestFlag("biologist.take_chance");

						ChatPacket(CHAT_TYPE_INFO, "Aktualna szansa: %d", new_chance);
						ChatPacket(CHAT_TYPE_COMMAND, "UpdateChance 0 %d", new_chance);
					} else if (type == 1) {
						int take_chance = GetQuestFlag("collector.take_chance");
						if (take_chance == max_increase || take_chance > max_increase) {
							ChatPacket(CHAT_TYPE_INFO, "Szansa na oddanie zosta³a ju¿ zwiêkszona maksymalnie!");
							return false;
						}
						SetQuestFlag("collector.take_chance", take_chance+val);
						item->SetCount(item->GetCount()-1);

						ChatPacket(CHAT_TYPE_INFO, "Szansa na oddanie zosta³a zwiêkszona o 25%");

						int new_chance = GetQuestFlag("collector.take_chance");
						
						ChatPacket(CHAT_TYPE_INFO, "Aktualna szansa: %d", new_chance);
						ChatPacket(CHAT_TYPE_COMMAND, "UpdateChance 1 %d", new_chance);
					}
					break;
				}
		}
#endif
		switch(item->GetVnum())
		{
// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 			case 80044:
// 				{
// 					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
// 					{
// 						ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna!");
// 						return false;
// 					}
					
// 					int Exp = 100000000;
// 					if (GetWear(WEAR_COSTUME_MOUNT)) {
// 						if (GetMountLevel() == 50) {
// 							ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz u¿yæ tego przedmiotu - Wierzchowiec posiada maksymalny poziom.");
// 							return false;
// 						}
// 						if (IsRidingMount()) {
// 							ChatPacket(CHAT_TYPE_INFO, "Musisz zsi¹œæ wierzchowca!");
// 							return false;
// 						}
// 						MountSetExp(Exp);
// 						item->SetCount(item->GetCount() - 1);
// 					}
// 					else {
// 						ChatPacket(CHAT_TYPE_INFO, "Aby u¿yæ tego przedmiotu musisz mieæ przywo³anego wierzchowca!");
// 						return false;
// 					}
// 					break;
// 				}
// #endif
		}
#ifdef ENABLE_INSTANT_INGOT_OPEN
		if (item->GetVnum() == 80005 || item->GetVnum() == 80006 || item->GetVnum() == 80007 || item->GetVnum() == 80008 || item->GetVnum() == 80009) {
			OpenIngotItem(item);
		}
#endif
		if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
		{
			if (test_server)
				sys_log(0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

			int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
			int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int apply_value = item->GetValue(2);
			int apply_duration = item->GetValue(1);

			switch (item->GetSubType())
			{
			case USE_ABILITY_UP:
				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì È¿°ú°¡ °É·Á ÀÖ½À´Ï´Ù."));
					return false;
				}

				{
					switch (item->GetValue(0))
					{
					case APPLY_MOV_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_ATT_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_CRITICAL_PCT:
					case APPLY_PENETRATE_PCT:
					case APPLY_STR:

					case APPLY_DEX:
					case APPLY_CON:
					case APPLY_INT:
					case APPLY_CAST_SPEED:
					case APPLY_RESIST_MAGIC:
					case APPLY_ATT_GRADE_BONUS:
					case APPLY_DEF_GRADE_BONUS:
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
						break;
					}
				}

				if (GetDungeon())
					GetDungeon()->UsePotion(this);

				if (GetWarMap())
					GetWarMap()->UsePotion(this, item);

				item->SetCount(item->GetCount() - 1);
				break;

			case USE_AFFECT:
			{
				if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì È¿°ú°¡ °É·Á ÀÖ½À´Ï´Ù."));
				}
				else
				{
					AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case USE_POTION_NODELAY:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
						return false;
					}

					switch (item->GetVnum())
					{
					case 70020:
					case 71018:
					case 71019:
					case 71020:
						if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
						{
							if (m_nPotionLimit <= 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë Á¦ÇÑ·®À» ÃÊ°úÇÏ¿´½À´Ï´Ù."));
								return false;
							}
						}
						break;

					default:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
						return false;
						break;
					}
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#endif
				bool used = false;

				if (item->GetValue(0) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(1) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (item->GetValue(3) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(4) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (used)
				{
					if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
					{
						if (test_server)
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿ùº´ ¶Ç´Â Á¾ÀÚ ¸¦ »ç¿ëÇÏ¿´½À´Ï´Ù"));
						SetUseSeedOrMoonBottleTime();
					}
					if (GetDungeon())
						GetDungeon()->UsePotion(this);

					if (GetWarMap())
						GetWarMap()->UsePotion(this, item);

					m_nPotionLimit--;

					//RESTRICT_USE_SEED_OR_MOONBOTTLE
					item->SetCount(item->GetCount() - 1);
					//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
				}
#ifdef ENABLE_NEWSTUFF
				if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Blogo, std::chrono::milliseconds(1000)))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Blogo));
					return false;
				}
#endif
			}
			break;
			}

			return true;
		}

		if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#endif
		}

		if (test_server)
		{
			sys_log(0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
		}

		switch (item->GetSubType())
		{
		case USE_TIME_CHARGE_PER:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret;
				char buf[128];
				if (item->GetVnum() == DRAGON_HEART_VNUM)
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
				}
				else
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				}
				if (ret > 0)
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%ld}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÃÊ ¸¸Å­ ÃæÀüµÇ¾ú½À´Ï´Ù."), ret);
					item->SetCount(item->GetCount() - 1);
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					return true;
				}
				else
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "No change by item{VN:%d SOC%d:%ld}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÃæÀüÇÒ ¼ö ¾ø½À´Ï´Ù."));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
				return false;
		}
		break;
		case USE_TIME_CHARGE_FIX:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				char buf[128];
				if (ret)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÃÊ ¸¸Å­ ÃæÀüµÇ¾ú½À´Ï´Ù."), ret);
					sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					item->SetCount(item->GetCount() - 1);
					return true;
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÃæÀüÇÒ ¼ö ¾ø½À´Ï´Ù."));
					sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
				return false;
		}
		break;

		case USE_SPECIAL:
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			if (item->GetVnum() == 71999)
			{
				if (GetBuffNPCSystem()->IsActive()) {
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_HAVE_A_ACTIVE_SHAMAN"));
					return false;
				}

				if (item->GetSocket(0) == 0) {
					SetBuffSealVID(item->GetCell());

					TPacketGCBuffNPCAction packet;
					packet.bHeader = HEADER_GC_BUFF_NPC_ACTION;
					packet.bAction = 0;
					packet.dValue0 = 0;
					packet.dValue1 = 0;
					GetDesc()->Packet(&packet, sizeof(packet));
				}
				else
				{
					DBManager::instance().DirectQuery("UPDATE srv1_player.player_buff_npc SET player_name='%s', player_id=%d WHERE id=%d ", GetName(), GetPlayerID(), item->GetSocket(0));
					item->RemoveFromCharacter();
					GetBuffNPCSystem()->LoadBuffNPC();
					SetQuestFlag("buff_npc.is_summon", 1);
					GetBuffNPCSystem()->Summon();
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_ACTIVATE_SEAL"));
				}
			}
#endif

			switch (item->GetVnum())
			{
			case ITEM_NOG_POCKET:
			{
				if (FindAffect(AFFECT_NOG_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì È¿°ú°¡ °É·Á ÀÖ½À´Ï´Ù."));
					return false;
				}
				long time = item->GetValue(0);
				long bonus_dungeon = item->GetValue(1);
				long srednia = item->GetValue(2);
				AddAffect(AFFECT_NOG_ABILITY, POINT_ATTBONUS_DUNGEON, bonus_dungeon, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_NOG_ABILITY, POINT_NORMAL_HIT_DAMAGE_BONUS, srednia, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_RAMADAN_CANDY:
			{
				// @fixme147 BEGIN
				if (FindAffect(AFFECT_RAMADAN_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì È¿°ú°¡ °É·Á ÀÖ½À´Ï´Ù."));
					return false;
				}
				// @fixme147 END
				long time = item->GetValue(0);
				long moveSpeedPer = item->GetValue(1);
				long attPer = item->GetValue(2);
				long expPer = item->GetValue(3);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;
			case ITEM_MARRIAGE_RING:
			{
				marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
				if (pMarriage)
				{
					if (pMarriage->ch1 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
							break;
						}
					}

					if (pMarriage->ch2 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
							break;
						}
					}

					int consumeSP = CalculateConsumeSP(this);

					if (consumeSP < 0)
						return false;

					PointChange(POINT_SP, -consumeSP, false);

					WarpToPID(pMarriage->GetOther(GetPlayerID()));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°áÈ¥ »óÅÂ°¡ ¾Æ´Ï¸é °áÈ¥¹ÝÁö¸¦ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
			}
			break;

#ifdef ENABLE_VS_PELERYNKA_VIP
			case UNIQUE_ITEM_CAPE_OF_COURAGE:
			case 70057:
			case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
				AggregateMonster(0);
				break;
				
			case 70069:
				AggregateMonster(1);
				break;
#else
			case UNIQUE_ITEM_CAPE_OF_COURAGE:
			case 70057:
			case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
				AggregateMonster();
				// item->SetCount(item->GetCount()-1);
				break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			case ACCE_REVERSAL_VNUM_1:
			case ACCE_REVERSAL_VNUM_2:
			{
				LPITEM item2;
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (!CleanAcceAttr(item, item2))
					return false;
				item->SetCount(item->GetCount()-1);
				break;
			}
			break;
#endif

#ifdef ENABLE_VIP_ITEMS
			case VIP_PD_1_DAY:
			case VIP_PD_3_DAY:
			case VIP_PD_7_DAY:
			case VIP_PD_14_DAY:
			case VIP_PD_30_DAY:
				if (GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "VIP jest ju¿ aktywny.");
					return false;
				}
				else
				{
					if (item->GetValue(0) > 0) {
						ITEM_MANAGER::instance().RemoveItem(item);
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET vip_pd_expire = DATE_ADD(NOW(), INTERVAL '%d' DAY) WHERE id = '%d';", item->GetValue(0), GetDesc()->GetAccountTable().id));

						ChatPacket(CHAT_TYPE_INFO, "VIP zosta³ aktywowany. Zaloguj siê ponownie.");
					} else
						ChatPacket(CHAT_TYPE_INFO, "SYS: Czas dla VIP Boxa nie jest ustawiony w bazie danych.");
				}
				break;
			case VIP_GLOVE_1_DAY:
			case VIP_GLOVE_3_DAY:
			case VIP_GLOVE_7_DAY:
			case VIP_GLOVE_14_DAY:
			case VIP_GLOVE_30_DAY:
				if (GetPremiumRemainSeconds(PREMIUM_ITEM) > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "VIP jest ju¿ aktywny.");
					return false;
				}
				else 
				{
					if (item->GetValue(0) > 0) {
						ITEM_MANAGER::instance().RemoveItem(item);
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET vip_glove_expire = DATE_ADD(NOW(), INTERVAL '%d' DAY) WHERE id = '%d';", item->GetValue(0), GetDesc()->GetAccountTable().id));
						
						ChatPacket(CHAT_TYPE_INFO, "VIP zosta³ aktywowany. Zaloguj siê ponownie.");
					} else
						ChatPacket(CHAT_TYPE_INFO, "SYS: Czas dla VIP Boxa nie jest ustawiony w bazie danych.");
				}
				break;
			case VIP_AUTOPODNOSZENIE_1_DAY:
			case VIP_AUTOPODNOSZENIE_3_DAY:
			case VIP_AUTOPODNOSZENIE_7_DAY:
			case VIP_AUTOPODNOSZENIE_14_DAY:
			case VIP_AUTOPODNOSZENIE_30_DAY:
				if (GetPremiumRemainSeconds(PREMIUM_AUTOLOOT) > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, "VIP jest ju¿ aktywny.");
					return false;
				}
				else 
				{
					if (item->GetValue(0) > 0) {
						ITEM_MANAGER::instance().RemoveItem(item);
						std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE srv1_account.account SET pickup_expire = DATE_ADD(NOW(), INTERVAL '%d' DAY) WHERE id = '%d';", item->GetValue(0), GetDesc()->GetAccountTable().id));
						
						ChatPacket(CHAT_TYPE_INFO, "VIP zosta³ aktywowany. Zaloguj siê ponownie.");
					} else
						ChatPacket(CHAT_TYPE_INFO, "SYS: Czas dla VIP Boxa nie jest ustawiony w bazie danych.");
				}
				break;
#endif

			case UNIQUE_ITEM_WHITE_FLAG:
				ForgetMyAttacker();
				item->SetCount(item->GetCount() - 1);
				break;

			case UNIQUE_ITEM_TREASURE_BOX:
				break;

			case 30093:
			case 30094:
			case 30095:
			case 30096:

			{
				const int MAX_BAG_INFO = 26;
				static struct LuckyBagInfo
				{
					DWORD count;
					int prob;
					DWORD vnum;
				} b1[MAX_BAG_INFO] =
				{
					{ 1000,	302,	1 },
					{ 10,	150,	27002 },
					{ 10,	75,	27003 },
					{ 10,	100,	27005 },
					{ 10,	50,	27006 },
					{ 10,	80,	27001 },
					{ 10,	50,	27002 },
					{ 10,	80,	27004 },
					{ 10,	50,	27005 },
					{ 1,	10,	50300 },
					{ 1,	6,	92 },
					{ 1,	2,	132 },
					{ 1,	6,	1052 },
					{ 1,	2,	1092 },
					{ 1,	6,	2082 },
					{ 1,	2,	2122 },
					{ 1,	6,	3082 },
					{ 1,	2,	3122 },
					{ 1,	6,	5052 },
					{ 1,	2,	5082 },
					{ 1,	6,	7082 },
					{ 1,	2,	7122 },
					{ 1,	1,	11282 },
					{ 1,	1,	11482 },
					{ 1,	1,	11682 },
					{ 1,	1,	11882 },
				};

				LuckyBagInfo* bi = NULL;
				bi = b1;

				int pct = number(1, 1000);

				int i;
				for (i = 0; i < MAX_BAG_INFO; i++)
				{
					if (pct <= bi[i].prob)
						break;
					pct -= bi[i].prob;
				}
				if (i >= MAX_BAG_INFO)
					return false;

				if (bi[i].vnum == 50300)
				{
					GiveRandomSkillBook();
				}
				else if (bi[i].vnum == 1)
				{
					PointChange(POINT_GOLD, 1000, true);
				}
				else
				{
					AutoGiveItem(bi[i].vnum, bi[i].count);
				}
				ITEM_MANAGER::instance().RemoveItem(item);
			}
			break;

			case 50004:
			{
				if (item->GetSocket(0))
				{
					item->SetSocket(0, item->GetSocket(0) + 1);
				}
				else
				{
					int iMapIndex = GetMapIndex();

					PIXEL_POSITION pos;

					if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
					{
						item->SetSocket(0, 1);
						item->SetSocket(1, pos.x);
						item->SetSocket(2, pos.y);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ °÷¿¡¼± ÀÌº¥Æ®¿ë °¨Áö±â°¡ µ¿ÀÛÇÏÁö ¾Ê´Â°Í °°½À´Ï´Ù."));
						return false;
					}
				}

				int dist = 0;
				float distance = (DISTANCE_SQRT(GetX() - item->GetSocket(1), GetY() - item->GetSocket(2)));

				if (distance < 1000.0f)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌº¥Æ®¿ë °¨Áö±â°¡ ½Åºñ·Î¿î ºûÀ» ³»¸ç »ç¶óÁý´Ï´Ù."));

					struct TEventStoneInfo
					{
						DWORD dwVnum;
						int count;
						int prob;
					};
					const int EVENT_STONE_MAX_INFO = 15;
					TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  8 },
						{ 27004, 10,  6 },
						{ 27002, 10, 12 },
						{ 27005, 10, 12 },
						{ 27100,  1,  9 },
						{ 27103,  1,  9 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 12 },

						{ 25040,  1,  4 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1,  0 },

						{ 50001,  1,  0 },
						{ 50003,  1,  1 },
					};
					TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  1 },
						{ 27004, 10,  1 },
						{ 27004, 10,  9 },
						{ 27005, 10,  9 },
						{ 27100,  1,  5 },
						{ 27103,  1,  5 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 14 },

						{ 25040,  1,  5 },

						{ 27410,  1,  5 },
						{ 27600,  1,  5 },
						{ 25100,  1,  5 },

						{ 50001,  1,  0 },
						{ 50003,  1,  5 },

					};
					TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  0 },
						{ 27004, 10,  0 },
						{ 27002, 10,  0 },
						{ 27005, 10,  0 },
						{ 27100,  1,  0 },
						{ 27103,  1,  0 },
						{ 27101,  1,  0 },
						{ 27104,  1,  0 },
						{ 27999,  1, 25 },

						{ 25040,  1,  0 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1, 15 },

						{ 50001,  1, 10 },
						{ 50003,  1, 50 },

					};

					{
						TEventStoneInfo* info;
						if (item->GetSocket(0) <= 4)
							info = info_4;
						else if (item->GetSocket(0) <= 7)
							info = info_7;
						else
							info = info_10;

						int prob = number(1, 100);

						for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
						{
							if (!info[i].prob)
								continue;

							if (prob <= info[i].prob)
							{
								if (info[i].dwVnum == 50001)
								{
									DWORD* pdw = M2_NEW DWORD[2];

									pdw[0] = info[i].dwVnum;
									pdw[1] = info[i].count;

									DBManager::instance().ReturnQuery(QID_LOTTO, GetPlayerID(), pdw,
										"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
										get_table_postfix(), GetPlayerID());
								}
								else
									AutoGiveItem(info[i].dwVnum, info[i].count);

								break;
							}
							prob -= info[i].prob;
						}
					}

					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

					if (len < 0 || len >= (int)sizeof(chatbuf))
						len = sizeof(chatbuf) - 1;

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header = HEADER_GC_CHAT;
					pack_chat.size = sizeof(TPacketGCChat) + len;
					pack_chat.type = CHAT_TYPE_COMMAND;
					pack_chat.id = 0;
					pack_chat.bEmpire = GetDesc()->GetEmpire();
					//pack_chat.id	= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());

					ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
					return true;
				}
				else if (distance < 20000)
					dist = 1;
				else if (distance < 70000)
					dist = 2;
				else
					dist = 3;

				const int STONE_DETECT_MAX_TRY = 10;
				if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌº¥Æ®¿ë °¨Áö±â°¡ ÈçÀûµµ ¾øÀÌ »ç¶óÁý´Ï´Ù."));
					ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
					AutoGiveItem(27002);
					return true;
				}

				if (dist)
				{
					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf),
						"StoneDetect %u %d %d",
						(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

					if (len < 0 || len >= (int)sizeof(chatbuf))
						len = sizeof(chatbuf) - 1;

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header = HEADER_GC_CHAT;
					pack_chat.size = sizeof(TPacketGCChat) + len;
					pack_chat.type = CHAT_TYPE_COMMAND;
					pack_chat.id = 0;
					pack_chat.bEmpire = GetDesc()->GetEmpire();
					//pack_chat.id		= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());
				}

			}
			break;

			case 27989:
			case 76006:
			{
				LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

				if (pMap != NULL)
				{
					item->SetSocket(0, item->GetSocket(0) + 1);

					FFindStone f;

					// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
					pMap->for_each(f);

					if (f.m_mapStone.size() > 0)
					{
						std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

						DWORD max = UINT_MAX;
						LPCHARACTER pTarget = stone->second;

						while (stone != f.m_mapStone.end())
						{
							DWORD dist = (DWORD)DISTANCE_SQRT(GetX() - stone->second->GetX(), GetY() - stone->second->GetY());

							if (dist != 0 && max > dist)
							{
								max = dist;
								pTarget = stone->second;
							}
							stone++;
						}

						if (pTarget != NULL)
						{
							int val = 3;

							if (max < 10000) val = 2;
							else if (max < 70000) val = 1;

							ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
								(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°¨Áö±â¸¦ ÀÛ¿ëÇÏ¿´À¸³ª °¨ÁöµÇ´Â ¿µ¼®ÀÌ ¾ø½À´Ï´Ù."));
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°¨Áö±â¸¦ ÀÛ¿ëÇÏ¿´À¸³ª °¨ÁöµÇ´Â ¿µ¼®ÀÌ ¾ø½À´Ï´Ù."));
					}

					if (item->GetSocket(0) >= 6)
					{
						ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
						ITEM_MANAGER::instance().RemoveItem(item);
					}
				}
				break;
			}
			break;

			case 27996:
				item->SetCount(item->GetCount() - 1);
				AttackedByPoison(NULL); // @warme008
				break;

			case 27987:

			{
				item->SetCount(item->GetCount() - 1);

				int r = number(1, 100);

				if (r <= 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á¶°³¿¡¼­ µ¹Á¶°¢ÀÌ ³ª¿Ô½À´Ï´Ù."));
					AutoGiveItem(27990);
				}
				else
				{
					const int prob_table_gb2312[] =
					{
						95, 97, 99
					};

					const int* prob_table = prob_table_gb2312;

					if (r <= prob_table[0])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á¶°³°¡ ÈçÀûµµ ¾øÀÌ »ç¶óÁý´Ï´Ù."));
					}
					else if (r <= prob_table[1])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á¶°³¿¡¼­ ¹éÁøÁÖ°¡ ³ª¿Ô½À´Ï´Ù."));
						AutoGiveItem(27992);
					}
					else if (r <= prob_table[2])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á¶°³¿¡¼­ Ã»ÁøÁÖ°¡ ³ª¿Ô½À´Ï´Ù."));
						AutoGiveItem(27993);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ö°ï¿½ ï¿½ï¿½ï¿½Ô½ï¿½ï¿½Ï´ï¿½."));
						AutoGiveItem(27994);
					}
				}
			}
			break;

			case 71013:
				CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50100:
			case 50101:
			case 50102:
			case 50103:
			case 50104:
			case 50105:
			case 50106:
				CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50200:
			{
				if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
				{
#ifdef __PREMIUM_PRIVATE_SHOP__
					if (IsPrivateShopOwner())
					{
						if (GetPrivateShopTable()->llGold > 0 || GetPrivateShopTable()->dwCheque > 0)
						{
							OpenPrivateShopPanel();
							return true;
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
							return false;
						}
					}

					OpenPrivateShopPanel();
#else
					__OpenPrivateShop();
#endif
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YYou cannot open a private shop in this map."));
				}
			}
			break;

#ifdef __PREMIUM_PRIVATE_SHOP__
			case 71221:
			{
				if (IsPrivateShopOwner())
				{
					if (GetPrivateShopTable()->llGold > 0 || GetPrivateShopTable()->dwCheque > 0)
					{
						OpenPrivateShopPanel();
						return true;
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
						return false;
					}
				}

				OpenPrivateShopPanel();
				ChatPacket(CHAT_TYPE_COMMAND, "SetPrivateShopPremiumBuild");
			} break;

			case 60004:
			{
				OpenShopSearch(MODE_LOOKING);
			} break;

			case 60005:
			case 60006:
			case 60007:
			{
				OpenShopSearch(MODE_TRADING);
			} break;
#endif

			case fishing::FISH_MIND_PILL_VNUM:
				AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20 * 60, 0, true);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50301:
			case 50302:
			case 50303:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â ´É·ÂÀ» ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				int lv = GetSkillLevel(SKILL_LEADERSHIP);

				if (lv < item->GetValue(0))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ Ã¥Àº ³Ê¹« ¾î·Á¿ö ÀÌÇØÇÏ±â°¡ Èûµì´Ï´Ù."));
					return false;
				}

				if (lv >= item->GetValue(1))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ Ã¥Àº ¾Æ¹«¸® ºÁµµ µµ¿òÀÌ µÉ °Í °°Áö ¾Ê½À´Ï´Ù."));
					return false;
				}

				if (LearnSkillByBook(SKILL_LEADERSHIP))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50304:
			case 50305:
			case 50306:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("·¹º§ 30ÀÌ µÇ±â Àü¿¡´Â ½ÀµæÇÒ ¼ö ÀÖÀ» °Í °°Áö ¾Ê½À´Ï´Ù."));
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("·¹º§ 50ÀÌ µÇ±â Àü¿¡´Â ½ÀµæÇÒ ¼ö ÀÖÀ» °Í °°Áö ¾Ê½À´Ï´Ù."));
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) >= 2)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿¬°è±â´Â ´õÀÌ»ó ¼ö·ÃÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				int iPct = item->GetValue(0);

				if (LearnSkillByBook(SKILL_COMBO, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
				}
			}
			break;
			case 50311:
			case 50312:
			case 50313:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);
				if (GetSkillLevel(dwSkillVnum) >= 20 || dwSkillVnum - SKILL_LANGUAGE1 + 1 == GetEmpire())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ¿Ïº®ÇÏ°Ô ¾Ë¾ÆµéÀ» ¼ö ÀÖ´Â ¾ð¾îÀÌ´Ù."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50061:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 10)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó ¼ö·ÃÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50495:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;
				}
				
				int iSkillID = GetQuestFlag("buff.trainskill_vnum");
				if (iSkillID < 0 && iSkillID > 2) {
					ChatPacket(CHAT_TYPE_INFO, "Wybierz umiejêtnoœæ, któr¹ chcesz trenowaæ.");
					return false;
				}

				int iSkillLevel = GetBuffNPCSystem()->GetSkillLevel(iSkillID);
				int iSkillGrade = GetBuffNPCSystem()->GetSkillGrade(iSkillLevel);

				if (iSkillID == -1 || iSkillGrade == -1 || iSkillGrade != 2)
					return false;

				char flag_next_read[128+1];
				memset(flag_next_read, 0, sizeof(flag_next_read));
				snprintf(flag_next_read, sizeof(flag_next_read), "buff_npc_learn2.%u.next_read", iSkillID);

				int iNextReadLastTime = GetQuestFlag(flag_next_read);
				if(get_global_time() < iNextReadLastTime)
				{
					if (FindAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÁÖ¾È¼ú¼­¸¦ ÅëÇØ ÁÖÈ­ÀÔ¸¶¿¡¼­ ºüÁ®³ª¿Ô½À´Ï´Ù."));
					}
					else 
					{
						int sec_new = iNextReadLastTime-get_global_time();
						ConvertTime(sec_new);
						return false;
					}
				}

				int percent = item->GetValue(5);

				if (FindAffect(AFFECT_SKILE_BUFF_BOOK_BONUS))
					RemoveAffect(AFFECT_SKILE_BUFF_BOOK_BONUS);
					percent += 20;

				item->SetCount(item->GetCount() - 1);
				if(number(1, 100) < percent)
				{
					DWORD nextTime = get_global_time() + item->GetValue(4);
					SetQuestFlag(flag_next_read, nextTime);

					// m_dwSkill[iSkillID] = this->m_dwSkill[iSkillID] + 1;
					GetBuffNPCSystem()->SetSkill(iSkillID, iSkillLevel + 1);

					ChatPacket(CHAT_TYPE_INFO, "Trening zakoñczony pomyœlnie.");
						
					ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", 
					GetBuffNPCSystem()->GetSkillLevel(0), 
					GetBuffNPCSystem()->GetSkillLevel(1), 
					GetBuffNPCSystem()->GetSkillLevel(2)
					);

					if (test_server) {
						sys_log(0, "TrainBuffSkill_50495: percent: %d, skillVnum: %d iSkillGrade: %d", percent, iSkillID, iSkillGrade);
					}
				}
				else {
					ChatPacket(CHAT_TYPE_INFO, "Trening umiejêtnoœci nie powiód³ siê :(");
					return false;
				}
			}
			break;

			case 50511:
			case 50494:
			case 50496:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				int iSkillID = item->GetValue(0);
				int iSkillLevel = GetBuffNPCSystem()->GetSkillLevel(item->GetValue(1));
				int iFakeSkillID = item->GetValue(1);
				int iSkillGrade = GetBuffNPCSystem()->GetSkillGrade(iSkillLevel);

				if(iSkillID == -1 || iSkillGrade == -1)
					return false;
					
				if(iSkillGrade != 1) {
					if (iSkillGrade > 1)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ½ºÅ³Àº Ã¥À¸·Î ´õÀÌ»ó ¼ö·ÃÇÒ ¼ö ¾ø½À´Ï´Ù."));
					else
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ½ºÅ³Àº ¾ÆÁ÷ Ã¥À¸·Î ¼ö·ÃÇÒ °æÁö¿¡ ÀÌ¸£Áö ¾Ê¾Ò½À´Ï´Ù."));
					return false;
				}

				char flag_next_read[128+1];
				memset(flag_next_read, 0, sizeof(flag_next_read));
				snprintf(flag_next_read, sizeof(flag_next_read), "buff_npc_learn.%u.next_read", iSkillID);
				char flag_read_count[128+1];
				memset(flag_read_count, 0, sizeof(flag_read_count));
				snprintf(flag_read_count, sizeof(flag_read_count), "buff_npc_learn.%u.read_count", iSkillID);

				int iNextReadLastTime = GetQuestFlag(flag_next_read);
				if(get_global_time() < iNextReadLastTime)
				{
					if (FindAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_BUFF_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÁÖ¾È¼ú¼­¸¦ ÅëÇØ ÁÖÈ­ÀÔ¸¶¿¡¼­ ºüÁ®³ª¿Ô½À´Ï´Ù."));
					}
					else 
					{
						int sec_new = iNextReadLastTime-get_global_time();
						ConvertTime(sec_new);
						return false;
					}
				}

				int iReadCount = GetQuestFlag(flag_read_count);
					
				int need_bookcount = 1;
				int percent = item->GetValue(5);

				if (FindAffect(AFFECT_SKILE_BUFF_BOOK_BONUS))
					RemoveAffect(AFFECT_SKILE_BUFF_BOOK_BONUS);
					percent += 30;

				item->SetCount(item->GetCount() - 1);
				if(number(1, 100) < percent)
				{
					
					DWORD nextTime = get_global_time() + item->GetValue(4);
					SetQuestFlag(flag_next_read, nextTime);

					if (iReadCount >= need_bookcount)
					{
						// m_dwSkill[iSkillID] = this->m_dwSkill[iSkillID] + 1;
						GetBuffNPCSystem()->SetSkill(iFakeSkillID, iSkillLevel + 1);
						SetQuestFlag(flag_read_count, 0);

						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¥À¸·Î ´õ ³ôÀº °æÁöÀÇ ¼ö·ÃÀ» ¼º°øÀûÀ¸·Î ³¡³»¼Ì½À´Ï´Ù."));
						
						ChatPacket(CHAT_TYPE_COMMAND, "BuffNPCSkillInfo %d %d %d", 
						GetBuffNPCSystem()->GetSkillLevel(0), 
						GetBuffNPCSystem()->GetSkillLevel(1), 
						GetBuffNPCSystem()->GetSkillLevel(2)
						);

						if (test_server) {
							sys_log(0, "TrainBuffSkill: percent: %d, skillVnum: %d, iSkillGrade: %d", percent, iSkillID, iSkillGrade);
						}
					}
					else
					{
						SetQuestFlag(flag_read_count, iReadCount + 1);

						switch (number(1, 3))
						{
							case 1:
								ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¾î´ÀÁ¤µµ ÀÌ ±â¼ú¿¡ ´ëÇØ ÀÌÇØ°¡ µÇ¾úÁö¸¸ Á¶±Ý ºÎÁ·ÇÑµí ÇÑµ¥.."));
								break;
											
							case 2:
								ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("µåµð¾î ³¡ÀÌ º¸ÀÌ´Â °Ç°¡...  ÀÌ ±â¼úÀº ÀÌÇØÇÏ±â°¡ ³Ê¹« Èûµé¾î.."));
								break;

							case 3:
							default:
								ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¿­½ÉÈ÷ ÇÏ´Â ¹è¿òÀ» °¡Áö´Â °Í¸¸ÀÌ ±â¼úÀ» ¹è¿ï¼ö ÀÖ´Â À¯ÀÏÇÑ ±æÀÌ´Ù.."));
								break;
						}

						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ±ÇÀ» ´õ ÀÐ¾î¾ß ¼ö·ÃÀ» ¿Ï·á ÇÒ ¼ö ÀÖ½À´Ï´Ù."), need_bookcount - iReadCount);
						break;
					}
				} else {
					ChatPacket(CHAT_TYPE_INFO, "Trening umiejêtnoœci nie powiód³ siê :(");
					return false;
				}
			}
			break;


			case 50314: case 50315: case 50316:
			case 50323: case 50324:
			case 50325: case 50326:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz zmieniæ wartoœci, nie da siê przekszta³ciæ.");
					return false;
				}

				int iSkillLevelLowLimit = item->GetValue(0);
				int iSkillLevelHighLimit = item->GetValue(1);
				int iPct = MINMAX(0, item->GetValue(2), 100);
				int iLevelLimit = item->GetValue(3);
				DWORD dwSkillVnum = 0;

				switch (item->GetVnum())
				{
				case 50314: case 50315: case 50316:
					dwSkillVnum = SKILL_POLYMORPH;
					break;

				case 50323: case 50324:
					dwSkillVnum = SKILL_ADD_HP;
					break;

				case 50325: case 50326:
					dwSkillVnum = SKILL_RESIST_PENETRATE;
					break;
					


				default:
					return false;
				}

				if (0 == dwSkillVnum)
					return false;

				if (GetLevel() < iLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Twój poziom jest zbyt niski!");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "Opanowa³eœ ju¿ t¹ umiejêtnoœæ");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie jest ³atwo zrozumieæ t¹ ksi¹¿kê...");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿na siê wiêcej nauczyæ z tej ksi¹¿ki.");
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = item->GetValue(4);
					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

#ifdef ENABLE_NEW_PET_SYSTEM_BOOK
			case 90501:
			case 90502:
			case 90503:
			case 90504:
			case 90505:
			case 90506:
			case 90507:
			case 90508:
			case 90509:
			case 90510:
			case 90511:
			{
				// Dodanie wspomagania dla peta
				auto bSkillGrade = item->GetValue(2);
				auto rBookConfig = item->GetValue(0);
				
				if (NewPetSystemHelper::GetSkillLevel(this, rBookConfig) >= NewPetSystemHelper::PET_MAX_SKILL_LV)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_MAX_LEVEL"));
					return false;
				}
				if (NewPetSystemHelper::GetSkillGrade(NewPetSystemHelper::GetSkillLevel(this, rBookConfig)) != bSkillGrade)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_GRADE_MISMATCH"));
					return false;
				}
				if (NewPetSystemHelper::GetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig)) > get_global_time())
				{
					if (FindAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY);
					}
					else
					{
						int sec_new = (NewPetSystemHelper::GetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig))-get_global_time());
						ConvertTime(sec_new);
						return false; // nie dopuszczam do ponownego uzycia
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, "<Test Server> Hm cos tu nie gra, jak to widzisz to nie usunal sie Affect czytania ksiegi.");
						}
						else
						{
							return false; // nie dopuszczam do ponownego uzycia
						}
					}
				}
				BYTE bChance = item->GetValue(1);
				if (FindAffect(AFFECT_SKILE_PET_BOOK_BONUS))
				{
					bChance += 40;
					RemoveAffect(AFFECT_SKILE_PET_BOOK_BONUS);
				}

				item->SetCount(item->GetCount() - 1);

				time_t ttDelay = item->GetValue(3);

				if (bChance >= number(1, 100))
				{
					int iRes = NewPetSystemHelper::TrainSkill(this, rBookConfig);
					NewPetSystemHelper::SetSkillDelay(this, rBookConfig, NewPetSystemHelper::GetSkillLevel(this, rBookConfig), ttDelay);

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_SUCCESSFUL"));

					if (test_server) {
						sys_log(0, "TrainPetSkill: bChance: %d, skillVnum: %d", bChance, rBookConfig);
					}

					if (iRes > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_BOOK_REMAIN %d"), iRes);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_FAIL"));
				}

				return true;
			}
			case 90500:
			{
				// Dodanie wspomagania dla peta
				auto bSkillGrade = item->GetValue(2);
				auto vnum = GetQuestFlag("pet.trainskill_vnum");
				
				if (vnum == 0) {
					ChatPacket(CHAT_TYPE_INFO, "Wybierz umiejêtnoœæ, któr¹ chcesz trenowaæ.");
					return false;
				}

				if (NewPetSystemHelper::GetSkillLevel(this, vnum) >= NewPetSystemHelper::PET_MAX_SKILL_LV)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_MAX_LEVEL"));
					return false;
				}
				if (NewPetSystemHelper::GetSkillGrade(NewPetSystemHelper::GetSkillLevel(this, vnum)) != bSkillGrade)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_GRADE_MISMATCH"));
					return false;
				}
				if (NewPetSystemHelper::GetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum)) > get_global_time())
				{
					if (FindAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILE_PET_NO_BOOK_DELAY);
					}
					else
					{
						int sec_new = (NewPetSystemHelper::GetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum))-get_global_time());
						ConvertTime(sec_new);
						return false; // nie dopuszczam do ponownego uzycia
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, "<Test Server> Hm cos tu nie gra, jak to widzisz to nie usunal sie Affect czytania ksiegi.");
						}
						else
						{
							return false; // nie dopuszczam do ponownego uzycia
						}
					}
				}
				BYTE bChance = item->GetValue(1);
				if (FindAffect(AFFECT_SKILE_PET_BOOK_BONUS))
				{
					bChance += 20;
					RemoveAffect(AFFECT_SKILE_PET_BOOK_BONUS);
				}

				item->SetCount(item->GetCount() - 1);

				time_t ttDelay = item->GetValue(3);

				if (bChance >= number(1, 100))
				{
					int iRes = NewPetSystemHelper::TrainSkill(this, vnum);
					NewPetSystemHelper::SetSkillDelay(this, vnum, NewPetSystemHelper::GetSkillLevel(this, vnum), ttDelay);

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_SUCCESSFUL"));

					if (test_server) {
						sys_log(0, "TrainPetSkill - 90500: bChance: %d, skillVnum: %d", bChance, vnum);
					}

					if (iRes > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_BOOK_REMAIN %d"), iRes);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEW_PET_SYSTEM_SKILL_TRAINING_FAIL"));
				}

				return true;
			}
#endif

#ifdef ENABLE_SKILE_PASYWNE
			case 50335: case 50336: //SKILL_HUMAN_BONUS
			case 50337: case 50338: //SKILL_MONSTER_BONUS
			case 50339: case 50340: //SKILL_STONE_BONUS
			case 50341: case 50342: //SKILL_DUNG_BONUS
			case 50343: case 50344: //SKILL_BOSS_BONUS
			case 50345: case 50346: //SKILL_EXP_BONUS
			case 50347: case 50348: //SKILL_ODP_UM
			case 50349: case 50350: //SKILL_SREDNIE_BONUS
			case 50351: case 50352: //SKILL_STAT_BONUS
			case 50353: case 50354: //SKILL_HP_BONUS
			case 50355: case 50356: //SKILL_DO_PZ_BONUS
			case 50357: case 50358:
			case 50359: case 50360:
			case 50361: case 50362:
			case 50363: case 50364:
			case 50365: case 50366: 
			case 50367:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz zmieniæ wartoœci, nie da siê przekszta³ciæ.");
					return false;
				}

				int iSkillLevelLowLimit = item->GetValue(0);
				int iSkillLevelHighLimit = item->GetValue(1);
				int iPct = MINMAX(0, item->GetValue(2), 100);
				int iLevelLimit = item->GetValue(3);
				DWORD dwSkillVnum = 0;

				switch (item->GetVnum())
				{
					case 50335: case 50336: case 50337:  //SKILL_HUMAN_BONUS
						dwSkillVnum = SKILL_HUMAN_BONUS;
							break;
					case 50338: case 50339: case 50340: //SKILL_MONSTER_BONUS
						dwSkillVnum = SKILL_MONSTER_BONUS;
							break;
					case 50341: case 50342: case 50343: //SKILL_STONE_BONUS
						dwSkillVnum = SKILL_STONE_BONUS;
							break;
					case 50344: case 50345: case 50346: //SKILL_DUNG_BONUS
						dwSkillVnum = SKILL_DUNG_BONUS;
							break;
					case 50347: case 50348: case 50349://SKILL_BOSS_BONUS
						dwSkillVnum = SKILL_BOSS_BONUS;
							break;
					case 50350: case 50351: case 50352: //SKILL_EXP_BONUS
						dwSkillVnum = SKILL_EXP_BONUS;
							break;
					case 50353: case 50354: case 50355: //SKILL_ODP_UM
						dwSkillVnum = SKILL_ODP_UM;
							break;
					case 50356: case 50357: case 50358: //SKILL_SREDNIE_BONUS
						dwSkillVnum = SKILL_SREDNIE_BONUS;
							break;
					case 50359: case 50360: case 50361: //SKILL_STAT_BONUS
						dwSkillVnum = SKILL_STAT_BONUS;
							break;
					case 50362: case 50363: case 50364: //SKILL_HP_BONUS
						dwSkillVnum = SKILL_HP_BONUS;
							break;
					case 50365: case 50366: case 50367: //SKILL_DO_PZ_BONUS
						dwSkillVnum = SKILL_DO_PZ_BONUS;
							break;

					default:
						return false;
				}

				if (0 == dwSkillVnum)
					return false;

				if (GetLevel() < iLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Twój poziom jest zbyt niski!");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, "Opanowa³eœ ju¿ t¹ umiejêtnoœæ!");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie jest ³atwo zrozumieæ t¹ ksi¹¿kê...");
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿na siê wiêcej nauczyæ z tej ksi¹¿ki.");
					return false;
				}
				
				if (LearnSkillByNewBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}

				// if (GetQuestFlag("skile_pasywne.pasywne_egzo") > 0)
				// {
					// if (LearnSkillByNewBook(dwSkillVnum, iPct))
					// {
						// SetSkillLevel(dwSkillVnum, m_pSkillLevels[dwSkillVnum].bLevel);
						// SetQuestFlag("skile_pasywne.pasywne_egzo", 0);
// #ifdef ENABLE_BOOKS_STACKFIX
						// item->SetCount(item->GetCount() - 1);
// #else
						// ITEM_MANAGER::instance().RemoveItem(item);
// #endif
					// }
				// }
				// else
				// {
						// ChatPacket(CHAT_TYPE_INFO, "Musisz u??Zwoju Egzorcyzmu(P).");
						// return false;
				// }

			}
			break;
#endif
			case 50902:
			case 50903:
			case 50904:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_CREATE;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó ¼ö·ÃÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
					}
				}
				else
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
					}
				}
			}
			break;

			// MINING
			case ITEM_MINING_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_MINING;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó ¼ö·ÃÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;
			// END_OF_MINING

			case ITEM_HORSE_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯½ÅÁß¿¡´Â Ã¥À» ÀÐÀ»¼ö ¾ø½À´Ï´Ù."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_HORSE;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ ½Â¸¶ ½ºÅ³À» ¼ö·ÃÇÒ ¼ö ÀÖ´Â ·¹º§ÀÌ ¾Æ´Õ´Ï´Ù."));
					return false;
				}

				if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
				{
					if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
					{
						RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÁÖ¾È¼ú¼­¸¦ ÅëÇØ ÁÖÈ­ÀÔ¸¶¿¡¼­ ºüÁ®³ª¿Ô½À´Ï´Ù."));
					}
					else
					{
						SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
						return false;
					}
				}

				if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
					GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
					GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó ½Â¸¶ ¼ö·Ã¼­¸¦ ÀÐÀ» ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (number(1, 100) <= iPct)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("½Â¸¶ ¼ö·Ã¼­¸¦ ÀÐ¾î ½Â¸¶ ½ºÅ³ Æ÷ÀÎÆ®¸¦ ¾ò¾ú½À´Ï´Ù."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾òÀº Æ÷ÀÎÆ®·Î´Â ½Â¸¶ ½ºÅ³ÀÇ ·¹º§À» ¿Ã¸± ¼ö ÀÖ½À´Ï´Ù."));
					PointChange(POINT_HORSE_SKILL, 1);

					int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					if (distribution_test_server) iReadDelay /= 3;

					if (!test_server)
						SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("½Â¸¶ ¼ö·Ã¼­ ÀÌÇØ¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
				}
#ifdef ENABLE_BOOKS_STACKFIX
				item->SetCount(item->GetCount() - 1);
#else
				ITEM_MANAGER::instance().RemoveItem(item);
#endif
			}
			break;

			case 70102:
			case 70103:
			{
				if (GetAlignment() >= 0)
					return false;

				int delta = MIN(-GetAlignment(), item->GetValue(0));

				sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

				UpdateAlignment(delta);
				item->SetCount(item->GetCount() - 1);

				if (delta / 10 > 0)
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¸¶À½ÀÌ ¸¼¾ÆÁö´Â±º. °¡½¿À» Áþ´©¸£´ø ¹«¾ð°¡°¡ Á» °¡º­¿öÁø ´À³¦ÀÌ¾ß."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼±¾ÇÄ¡°¡ %d Áõ°¡ÇÏ¿´½À´Ï´Ù."), delta / 10);
				}
			}
			break;

			case 71107:
			case 39032: // @fixme169 mythical peach alternative vnum
			{
				int val = item->GetValue(0);
				int interval = item->GetValue(1);
				quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
				if (!pPC) // @fixme169 missing check
					return false;
				if (GetAlignment() == 200000)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼±¾ÇÄ¡¸¦ ´õ ÀÌ»ó ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (200000 - GetAlignment() < val * 10)
				{
					val = (200000 - GetAlignment()) / 10;
				}

				int old_alignment = GetAlignment() / 10;

				UpdateAlignment(val * 10);

				item->SetCount(item->GetCount() - 1);

				ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¸¶À½ÀÌ ¸¼¾ÆÁö´Â±º. °¡½¿À» Áþ´©¸£´ø ¹«¾ð°¡°¡ Á» °¡º­¿öÁø ´À³¦ÀÌ¾ß."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼±¾ÇÄ¡°¡ %d Áõ°¡ÇÏ¿´½À´Ï´Ù."), val);

				char buf[256 + 1];
				snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment() / 10);
				LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
			}
			break;
			
			case 80050: // Jab?o 
			{
				int val = item->GetValue(0);
				quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
				if (!pPC) // @fixme169 missing check
					return false;

				if (GetAlignment() >= 50000)
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}

				int old_alignment = GetAlignment();

				UpdateAlignment(val);

				item->SetCount(item->GetCount() - 1);

				ChatPacket(CHAT_TYPE_TALKING, "Twój umys³ zosta³ oczyszczony. Skumulowa³eœ energiê Chi.");
				ChatPacket(CHAT_TYPE_INFO, "Przywrócono %d punktów Rangi..", val);

				char buf[256 + 1];
				snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
				LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
			}
			break;
			case 80051: // gruszka
			{
				if (GetAlignment() > 49999 && GetAlignment() < 100000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Zwiêkszono iloœæ rangi o %d ", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}
			}
			break;
			
			case 80052: // winogron
			{
				if (GetAlignment() > 99999 && GetAlignment() < 200000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Zwiêkszono iloœæ rangi o %d ", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}
			}
			break;
			
			case 80053: // Arbuz
			{
				if (GetAlignment() > 199999 && GetAlignment() < 300000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Zwiêkszono iloœæ rangi o %d ", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}
			}
			break;
			
			case 80054: // Melon
			{
				if (GetAlignment() > 299999 && GetAlignment() < 400000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Zwiêkszono iloœæ rangi o %d ", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}
			}
			break;
			
			case 80055: // Truskawka
			{
				if (GetAlignment() > 399999 && GetAlignment() < 500000)
				{
					int val = item->GetValue(0);
					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
					if (!pPC) // @fixme169 missing check
						return false;

					int old_alignment = GetAlignment();

					UpdateAlignment(val);

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Zwiêkszono iloœæ rangi o %d ", val);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%d %lld", old_alignment, GetAlignment());
					LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
				} 
				else 
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz ju¿ zwiêkszyæ swojej Rangi.");
					return false;
				}
			}
			break;

			case 71109:
			case 72719:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetSocketCount() == 0)
					return false;

				switch (item2->GetType())
				{
				case ITEM_WEAPON:
					break;
				case ITEM_ARMOR:
					switch (item2->GetSubType())
					{
					case ARMOR_EAR:
					case ARMOR_WRIST:
					case ARMOR_NECK:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»©³¾ ¿µ¼®ÀÌ ¾ø½À´Ï´Ù"));
						return false;
					}
					break;

				default:
					return false;
				}

				std::stack<long> socket;

				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					socket.push(item2->GetSocket(i));

				int idx = ITEM_SOCKET_MAX_NUM - 1;

				while (socket.size() > 0)
				{
					if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
						break;

					idx--;
					socket.pop();
				}

				if (socket.size() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»©³¾ ¿µ¼®ÀÌ ¾ø½À´Ï´Ù"));
					return false;
				}

				LPITEM pItemReward = AutoGiveItem(socket.top());

				if (pItemReward != NULL)
				{
					item2->SetSocket(idx, 1);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
						item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
					LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case 70201:
			case 70202:
			case 70203:
			case 70204:
			case 70205:
			case 70206:
			{
				// NEW_HAIR_STYLE_ADD
				if (GetPart(PART_HAIR) >= 1001)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÇöÀç Çì¾î½ºÅ¸ÀÏ¿¡¼­´Â ¿°»ö°ú Å»»öÀÌ ºÒ°¡´ÉÇÕ´Ï´Ù."));
				}
				// END_NEW_HAIR_STYLE_ADD
				else
				{
					quest::CQuestManager& q = quest::CQuestManager::instance();
					quest::PC* pPC = q.GetPC(GetPlayerID());

					if (pPC)
					{
						int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

						if (last_dye_level == 0 ||
							last_dye_level + 3 <= GetLevel() ||
							item->GetVnum() == 70201)
						{
							SetPart(PART_HAIR, item->GetVnum() - 70201);

							if (item->GetVnum() == 70201)
								pPC->SetFlag("dyeing_hair.last_dye_level", 0);
							else
								pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

							item->SetCount(item->GetCount() - 1);
							UpdatePacket();
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ·¹º§ÀÌ µÇ¾î¾ß ´Ù½Ã ¿°»öÇÏ½Ç ¼ö ÀÖ½À´Ï´Ù."), last_dye_level + 3);
						}
					}
				}
			}
			break;

			case ITEM_NEW_YEAR_GREETING_VNUM:
			{
				DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets;
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						if (dwVnums[i] == CSpecialItemGroup::GOLD)
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ· %d ³ÉÀ» È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
					}

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case ITEM_VALENTINE_ROSE:
			case ITEM_VALENTINE_CHOCOLATE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE == GET_SEX(this))) ||
					((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE == GET_SEX(this))))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºº°ÀÌ ¸ÂÁö¾Ê¾Æ ÀÌ ¾ÆÀÌÅÛÀ» ¿­ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_WHITEDAY_CANDY:
			case ITEM_WHITEDAY_ROSE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE == GET_SEX(this))) ||
					((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE == GET_SEX(this))))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºº°ÀÌ ¸ÂÁö¾Ê¾Æ ÀÌ ¾ÆÀÌÅÛÀ» ¿­ ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50011:
			{
				DWORD dwBoxVnum = 50011;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						char buf[50 + 1];
						snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
						LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

						//ITEM_MANAGER::instance().RemoveItem(item);
						item->SetCount(item->GetCount() - 1);

						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ· %d ³ÉÀ» È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
							break;

						case CSpecialItemGroup::EXP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ºÎÅÍ ½ÅºñÇÑ ºûÀÌ ³ª¿É´Ï´Ù."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÀÇ °æÇèÄ¡¸¦ È¹µæÇß½À´Ï´Ù."), dwCounts[i]);
							break;

						case CSpecialItemGroup::MOB:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ¸ó½ºÅÍ°¡ ³ªÅ¸³µ½À´Ï´Ù!"));
							break;

						case CSpecialItemGroup::SLOW:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â »¡°£ ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ ¿òÁ÷ÀÌ´Â ¼Óµµ°¡ ´À·ÁÁ³½À´Ï´Ù!"));
							break;

						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ°¡ °©ÀÚ±â Æø¹ßÇÏ¿´½À´Ï´Ù! »ý¸í·ÂÀÌ °¨¼ÒÇß½À´Ï´Ù."));
							break;

						case CSpecialItemGroup::POISON:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â ³ì»ö ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ µ¶ÀÌ ¿Â¸öÀ¸·Î ÆÛÁý´Ï´Ù!"));
							break;
#ifdef ENABLE_WOLFMAN_CHARACTER
						case CSpecialItemGroup::BLEEDING:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ³ª¿Â ³ì»ö ¿¬±â¸¦ µéÀÌ¸¶½ÃÀÚ µ¶ÀÌ ¿Â¸öÀ¸·Î ÆÛÁý´Ï´Ù!"));
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ ¸ó½ºÅÍ°¡ ³ªÅ¸³µ½À´Ï´Ù!"));
							break;

						default:
							if (item_gets[i])
							{
								#if defined(__CHATTING_WINDOW_RENEWAL__)
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ %d °³ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_ITEM_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName());
								#else
								if (dwCounts[i] > 1)
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ %d °³ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName(), dwCounts[i]);
								else
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»óÀÚ¿¡¼­ %s °¡ ³ª¿Ô½À´Ï´Ù."), item_gets[i]->GetName());
								#endif
							}
							break;
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("¾Æ¹«°Íµµ ¾òÀ» ¼ö ¾ø¾ú½À´Ï´Ù."));
					return false;
				}
			}
			break;

			case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
			{
				//PointChange(POINT_GOLD, -iCost);
				PointChange(POINT_STAT_RESET_COUNT, 1);
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50107:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#endif

				EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50108:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#endif

				EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_WONSO_BEAN_VNUM:
				PointChange(POINT_HP, GetMaxHP() - GetHP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_SUGAR_VNUM:
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_FRUIT_VNUM:
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());
				item->SetCount(item->GetCount() - 1);
				break;

			case 90008: // VCARD
			case 90009: // VCARD
				VCardUse(this, this, item);
				break;

			case ITEM_ELK_VNUM:
			{
				int iGold = item->GetSocket(0);
				ITEM_MANAGER::instance().RemoveItem(item);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ· %d ³ÉÀ» È¹µæÇß½À´Ï´Ù."), iGold);
				PointChange(POINT_GOLD, iGold);
			}
			break;
			

			case 70021:
			{
				int HealPrice = quest::CQuestManager::instance().GetEventFlag("MonarchHealGold");
				if (HealPrice == 0)
					HealPrice = 2000000;

				if (CMonarch::instance().HealMyEmpire(this, HealPrice))
				{
					char szNotice[256];
					snprintf(szNotice, sizeof(szNotice), LC_TEXT("±ºÁÖÀÇ Ãàº¹À¸·Î ÀÌÁö¿ª %s À¯Àú´Â HP,SP°¡ ¸ðµÎ Ã¤¿öÁý´Ï´Ù."), EMPIRE_NAME(GetEmpire()));
					SendNoticeMap(szNotice, GetMapIndex(), false);

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖÀÇ Ãàº¹À» »ç¿ëÇÏ¿´½À´Ï´Ù."));
				}
			}
			break;

			case 27995:
			{
			}
			break;

			case 71092:
			{
				if (m_pkChrTarget != NULL)
				{
					if (m_pkChrTarget->IsPolymorphed())
					{
						m_pkChrTarget->SetPolymorph(0);
						m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
					}
				}
				else
				{
					if (IsPolymorphed())
					{
						SetPolymorph(0);
						RemoveAffect(AFFECT_POLYMORPH);
					}
				}
			}
			break;

			case 71051:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
					return false;

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
				if (item2->AddRareAttribute2())
#else
				if (item2->AddRareAttribute())
#endif
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼º°øÀûÀ¸·Î ¼Ó¼ºÀÌ Ãß°¡ µÇ¾ú½À´Ï´Ù"));

					int iAddedIdx = item2->GetRareAttrCount() + 4;
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					LogManager::instance().ItemLog(
						GetPlayerID(),
						item2->GetAttributeType(iAddedIdx),
						item2->GetAttributeValue(iAddedIdx),
						item->GetID(),
						"ADD_RARE_ATTR",
						buf,
						GetDesc()->GetHostName(),
						item->GetOriginalVnum());

					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´õ ÀÌ»ó ÀÌ ¾ÆÀÌÅÛÀ¸·Î ¼Ó¼ºÀ» Ãß°¡ÇÒ ¼ö ¾ø½À´Ï´Ù"));
				}
			}
			break;

			case 71052:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

#ifdef ENABLE_ITEM_RARE_ATTR_LEVEL_PCT
				if (item2->ChangeRareAttribute2())
#else
				if (item2->ChangeRareAttribute())
#endif
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯°æ ½ÃÅ³ ¼Ó¼ºÀÌ ¾ø½À´Ï´Ù"));
				}
			}
			break;

			case ITEM_AUTO_HP_RECOVERY_S:
			case ITEM_AUTO_HP_RECOVERY_M:
			case ITEM_AUTO_HP_RECOVERY_L:
			case ITEM_AUTO_HP_RECOVERY_X:
			case ITEM_AUTO_SP_RECOVERY_S:
			case ITEM_AUTO_SP_RECOVERY_M:
			case ITEM_AUTO_SP_RECOVERY_L:
			case ITEM_AUTO_SP_RECOVERY_X:

			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
			{
#ifdef ENABLE_NEWSTUFF
				if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Potions, std::chrono::milliseconds(1500)))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Potions));
					return false;
				}
#endif
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}
#ifdef ENABLE_NEWSTUFF
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
					return false;
				}
#endif

				EAffectTypes type = AFFECT_NONE;
				bool isSpecialPotion = false;

				switch (item->GetVnum())
				{
				case ITEM_AUTO_HP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_HP_RECOVERY_S:
				case ITEM_AUTO_HP_RECOVERY_M:
				case ITEM_AUTO_HP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
					type = AFFECT_AUTO_HP_RECOVERY;
					break;

				case ITEM_AUTO_SP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_SP_RECOVERY_S:
				case ITEM_AUTO_SP_RECOVERY_M:
				case ITEM_AUTO_SP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
					type = AFFECT_AUTO_SP_RECOVERY;
					break;
				}

				if (AFFECT_NONE == type)
					break;

				if (item->GetCount() > 1)
				{
					int pos = GetEmptyInventory(item->GetSize());

					if (-1 == pos)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇ°¿¡ ºó °ø°£ÀÌ ¾ø½À´Ï´Ù."));
						break;
					}

					item->SetCount(item->GetCount() - 1);

					LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
					item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

					if (item->GetSocket(1) != 0)
					{
						item2->SetSocket(1, item->GetSocket(1));
					}

					item = item2;
				}

				CAffect* pAffect = FindAffect(type);

				if (NULL == pAffect)
				{
					EPointTypes bonus = POINT_NONE;

					if (true == isSpecialPotion)
					{
						if (type == AFFECT_AUTO_HP_RECOVERY)
						{
							bonus = POINT_MAX_HP_PCT;
						}
						else if (type == AFFECT_AUTO_SP_RECOVERY)
						{
							bonus = POINT_MAX_SP_PCT;
						}
					}

					AddAffect(type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

					item->Lock(true);
					item->SetSocket(0, true);

					AutoRecoveryItemProcess(type);
				}
				else
				{
					if (item->GetID() == pAffect->dwFlag)
					{
						RemoveAffect(pAffect);

						item->Lock(false);
						item->SetSocket(0, false);
					}
					else
					{
						LPITEM old = FindItemByID(pAffect->dwFlag);

						if (NULL != old)
						{
							old->Lock(false);
							old->SetSocket(0, false);
						}

						RemoveAffect(pAffect);

						EPointTypes bonus = POINT_NONE;

						if (true == isSpecialPotion)
						{
							if (type == AFFECT_AUTO_HP_RECOVERY)
							{
								bonus = POINT_MAX_HP_PCT;
							}
							else if (type == AFFECT_AUTO_SP_RECOVERY)
							{
								bonus = POINT_MAX_SP_PCT;
							}
						}

						AddAffect(type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

						item->Lock(true);
						item->SetSocket(0, true);

						AutoRecoveryItemProcess(type);
					}
				}
			}
			break;
			}
			break;

		case USE_CLEAR:
		{
			switch (item->GetVnum())
			{
#ifdef ENABLE_WOLFMAN_CHARACTER
			case 27124: // Bandage
				RemoveBleeding();
				break;
#endif
			case 27874: // Grilled Perch
			default:
				RemoveBadAffect();
				break;
			}
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_INVISIBILITY:
		{
			if (item->GetVnum() == 70026)
			{
				quest::CQuestManager& q = quest::CQuestManager::instance();
				quest::PC* pPC = q.GetPC(GetPlayerID());

				if (pPC != NULL)
				{
					int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

					if (get_global_time() - last_use_time < 10 * 60)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
						return false;
					}

					pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
				}
			}

			AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_POTION_NODELAY:
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				switch (item->GetVnum())
				{
				case 70020:
				case 71018:
				case 71019:
				case 71020:
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë Á¦ÇÑ·®À» ÃÊ°úÇÏ¿´½À´Ï´Ù."));
							return false;
						}
					}
					break;

				default:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#endif

			bool used = false;

			if (item->GetValue(0) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_HPUP_RED);
					used = TRUE;
				}
			}

			if (item->GetValue(1) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (item->GetValue(3) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
					EffectPacket(SE_HPUP_RED);
					used = TRUE;
				}
			}

			if (item->GetValue(4) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (used)
			{
				if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
				{
					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿ùº´ ¶Ç´Â Á¾ÀÚ ¸¦ »ç¿ëÇÏ¿´½À´Ï´Ù"));
					SetUseSeedOrMoonBottleTime();
				}
				if (GetDungeon())
					GetDungeon()->UsePotion(this);

				if (GetWarMap())
					GetWarMap()->UsePotion(this, item);

				m_nPotionLimit--;

				//RESTRICT_USE_SEED_OR_MOONBOTTLE
				item->SetCount(item->GetCount() - 1);
				//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
			}
#ifdef ENABLE_NEWSTUFF
			if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::Blogo, std::chrono::milliseconds(1000)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::Blogo));
				return false;
			}
#endif
		}
		break;

		case USE_POTION:
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}

				switch (item->GetVnum())
				{
				case 27001:
				case 27002:
				case 27003:
				case 27004:
				case 27005:
				case 27006:
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë Á¦ÇÑ·®À» ÃÊ°úÇÏ¿´½À´Ï´Ù."));
							return false;
						}
					}
					break;

				default:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
					return false;
				}
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#endif

			if (item->GetValue(1) != 0)
			{
				if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
				{
					return false;
				}

				PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_SPUP_BLUE);
			}

			if (item->GetValue(0) != 0)
			{
				if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
				{
					return false;
				}

				PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_HPUP_RED);
			}

			if (GetDungeon())
				GetDungeon()->UsePotion(this);

			if (GetWarMap())
				GetWarMap()->UsePotion(this, item);

			// item->SetCount(item->GetCount() - 1);
			m_nPotionLimit--;
			break;

		case USE_POTION_CONTINUE:
		{
			if (item->GetValue(0) != 0)
			{
				AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
			}
			else if (item->GetValue(1) != 0)
			{
				AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
			}
			else
				return false;
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		item->SetCount(item->GetCount() - 1);
		break;

		case USE_ABILITY_UP:
		{
#ifdef USE_FISH_SYSTEM
			if (item->GetVnum() > FISH_ITEMID_START-1 && item->GetVnum() < FISH_ITEMID_END+1) {
				DWORD pointid = (aApplyInfo[item->GetValue(0)].bPointType);
				if (FindAffect(AFFECT_BLEND_FISH, pointid)) {
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				} else {
					AddAffect(AFFECT_BLEND_FISH, pointid, item->GetValue(2), 0, item->GetValue(1), 0, false);
						item->SetCount(item->GetCount() - 1);
				}
			}
#endif
			switch (item->GetValue(0))
			{
				case APPLY_MOV_SPEED:
					AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
					EffectPacket(SE_DXUP_PURPLE);
#endif
					break;

				case APPLY_ATT_SPEED:
					AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
					EffectPacket(SE_SPEEDUP_GREEN);
#endif
					break;
			}
		}

		if (GetDungeon())
			GetDungeon()->UsePotion(this);

		if (GetWarMap())
			GetWarMap()->UsePotion(this, item);

		// item->SetCount(item->GetCount() - 1);
		break;

		case USE_TALISMAN:
		{
			const int TOWN_PORTAL = 1;
			const int MEMORY_PORTAL = 2;

			if (GetMapIndex() == 200 || GetMapIndex() == 113)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÇöÀç À§Ä¡¿¡¼­ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}

			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#ifdef ENABLE_NEWSTUFF
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·Ã Áß¿¡´Â ÀÌ¿ëÇÒ ¼ö ¾ø´Â ¹°Ç°ÀÔ´Ï´Ù."));
				return false;
			}
#endif

			if (m_pkWarpEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌµ¿ÇÒ ÁØºñ°¡ µÇ¾îÀÖÀ½À¸·Î ±ÍÈ¯ºÎ¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù"));
				return false;
			}

			// CONSUME_LIFE_WHEN_USE_WARP_ITEM
			int consumeLife = CalculateConsume(this);

			if (consumeLife < 0)
				return false;
			// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

			if (item->GetValue(0) == TOWN_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (!GetDungeon())
						if (!GiveRecallItem(item))
							return false;

					PIXEL_POSITION posWarp;

					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
					{
						// CONSUME_LIFE_WHEN_USE_WARP_ITEM
						PointChange(POINT_HP, -consumeLife, false);
						// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

						WarpSet(posWarp.x, posWarp.y);
					}
					else
					{
						sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
					}
				}
				else
				{
					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿ø·¡ À§Ä¡·Î º¹±Í"));

					ProcessRecallItem(item);
				}
			}
			else if (item->GetValue(0) == MEMORY_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (GetDungeon())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´øÀü ¾È¿¡¼­´Â %s%s »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."),
							item->GetName(),
							"");
						return false;
					}

					if (!GiveRecallItem(item))
						return false;
				}
				else
				{
					// CONSUME_LIFE_WHEN_USE_WARP_ITEM
					PointChange(POINT_HP, -consumeLife, false);
					// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

					ProcessRecallItem(item);
				}
			}
		}
		break;

		case USE_TUNING:
		case USE_DETACHMENT:
		{
			LPITEM item2;

			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 ¿µ¼®Àº ÀÌ ¾ÆÀÌÅÛÀ¸·Î °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù"));
				return false;
			}

			if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
			{
				if (item->GetVnum() == 71056)
				{
					RefineItem(item, item2);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¿µ¼®Àº ÀÌ ¾ÆÀÌÅÛÀ¸·Î °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù"));
				}
			}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0 && item->GetVnum() != 51003 || item->GetVnum() == ACCE_REVERSAL_VNUM_1 || item->GetVnum() == ACCE_REVERSAL_VNUM_2)
			{
				if (!CleanAcceAttr(item, item2))
					return false;
					
				item->SetCount(item->GetCount()-1);
				return true;
			}
#endif
			else
			{
				RefineItem(item, item2);
			}
		}
		break;

		case USE_CHANGE_COSTUME_ATTR:
		case USE_RESET_COSTUME_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_COSTUME != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			if (item2->GetAttributeSetIndex() == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
				return false;
			}

			if (item2->GetAttributeCount() == 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯°æÇÒ ¼Ó¼ºÀÌ ¾ø½À´Ï´Ù."));
				return false;
			}

			switch (item->GetSubType())
			{
			case USE_CHANGE_COSTUME_ATTR:
				item2->ChangeAttribute();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
				}
				break;
			case USE_RESET_COSTUME_ATTR:
				item2->ClearAttribute();
				item2->AlterToMagicItem();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
				}
				break;
			}

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÏ¿´½À´Ï´Ù."));

			item->SetCount(item->GetCount() - 1);
			break;
		}

		//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
		case USE_PUT_INTO_BELT_SOCKET:
		case USE_PUT_INTO_RING_SOCKET:
		case USE_PUT_INTO_ACCESSORY_SOCKET:
		case USE_ADD_ACCESSORY_SOCKET:
		case USE_CLEAN_SOCKET:
		case USE_CHANGE_ATTRIBUTE:
		case USE_CHANGE_ATTRIBUTE2:
		case USE_ADD_ATTRIBUTE:
		case USE_ADD_ATTRIBUTE2:
#ifdef ENABLE_SPECIAL_BONUS
		case USE_ADD_ATTRIBUTE_SPECIAL_1:
#endif
		case USE_ADD_IS_BONUS:
		case USE_ADD_NEW_BONUS:
		case USE_ADD_EXTRA_BONUS:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				return false;

			if (item2->GetVnum() == 53012) {  // blokada dodawania do startowego peta
				return false;
			}
			// kostiumy - tylko dodania pvp i exp // blokada innych
			if (item2->GetVnum() >= 41001 && item2->GetVnum() <= 41999) {
				if (item->GetVnum() != 33052 && item->GetVnum() != 33053) {
					return false;
				}
			}
			// fryzury - tylko dodania pvp i exp // blokada innych
			if (item2->GetVnum() >= 45001 && item2->GetVnum() <= 45999) {
				if (item->GetVnum() != 33054 && item->GetVnum() != 33055) {
					return false;
				}
			}
			// pasy - tylko dodania i zmianki dla pasa // blokada innych
			if (item2->GetType() == ITEM_BELT) {
				if (item->GetVnum() != 79010 && item->GetVnum() != 79011) {
					return false;
				}
			}
			// relawice - tylko dodania i zmianki dla rekawic, extrabonus // blokada innych
			if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
				if (item->GetVnum() != 79012 && item->GetVnum() != 79013 && item->GetVnum() != 34000) {
					return false;
				}
			}
			// extra bonus
			if (item2->GetVnum() >= 85001 && item2->GetVnum() <= 85024 ||
				item2->GetVnum() == 80109 || item2->GetVnum() == 80119 || item2->GetVnum() == 80129 ||
				item2->GetVnum() == 55045 || item2->GetVnum() == 55055 || item2->GetVnum() == 55065 || 
				item2->GetVnum() == 55075 || item2->GetVnum() == 55085 || item2->GetVnum() == 55095
			) {
				if (item->GetVnum() != 34000) {
					return false;
				}
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			// if (ITEM_COSTUME == item2->GetType())
			// {
				// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ï¿½Ó¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô´Ï´ï¿½."));
				// return false;
			// }

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				return false;

			switch (item->GetSubType())
			{
			case USE_CLEAN_SOCKET:
			{
				int i;
#ifdef ENABLE_EXTENDED_SOCKETS
				for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
				{
					if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
						break;
				}

#ifdef ENABLE_EXTENDED_SOCKETS
				if (i == ITEM_STONES_MAX_NUM)
#else
				if (i == ITEM_SOCKET_MAX_NUM)
#endif
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã»¼ÒÇÒ ¼®ÀÌ ¹ÚÇôÀÖÁö ¾Ê½À´Ï´Ù."));
					return false;
				}

				int j = 0;

#ifdef ENABLE_EXTENDED_SOCKETS
				for (i = 0; i < ITEM_STONES_MAX_NUM; ++i)
#else
				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
#endif
				{
					if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
						item2->SetSocket(j++, item2->GetSocket(i));
				}

#ifdef ENABLE_EXTENDED_SOCKETS
				for (; j < ITEM_STONES_MAX_NUM; ++j)
#else
				for (; j < ITEM_SOCKET_MAX_NUM; ++j)
#endif
				{
					if (item2->GetSocket(j) > 0)
						item2->SetSocket(j, 1);
				}

				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
				}

				item->SetCount(item->GetCount() - 1);

			}
			break;

			case USE_CHANGE_ATTRIBUTE:
			case USE_CHANGE_ATTRIBUTE2: // @fixme123
				if (item->GetVnum() >= 33050 && item->GetVnum() <= 33059) {
					return false;
				}

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

				if (item2->GetAttributeCount() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("º¯°æÇÒ ¼Ó¼ºÀÌ ¾ø½À´Ï´Ù."));
					return false;
				}
				if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
				{
					DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

					if (pPC)
					{
						DWORD dwNowSec = get_global_time();

						DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

						if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» ¹Ù²ÛÁö %dºÐ ÀÌ³»¿¡´Â ´Ù½Ã º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù.(%d ºÐ ³²À½)"),
								dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
							return false;
						}

						pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
					}
				}

				if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 0, 30, 40, 3
					};

					item2->ChangeAttribute(aiChangeProb);
				}
				else if (item->GetVnum() == 76014)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 10, 50, 39, 1
					};

					item2->ChangeAttribute(aiChangeProb);
				}
				else
				{
					if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
					{
						if ((item2->GetType() == ITEM_WEAPON)
							|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
						{
							bool bCanUse = true;
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
							{
								if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
								{
									bCanUse = false;
									break;
								}
							}
							if (false == bCanUse)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Àû¿ë ·¹º§º¸´Ù ³ô¾Æ »ç¿ëÀÌ ºÒ°¡´ÉÇÕ´Ï´Ù."));
								break;
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹«±â¿Í °©¿Ê¿¡¸¸ »ç¿ë °¡´ÉÇÕ´Ï´Ù."));
							break;
						}
					}
					item2->ChangeAttribute();
				}

				// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ï¿½Ó¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½ï¿½ï¿½ï¿½Ï´ï¿½."));
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
				}

				// item->SetCount(item->GetCount() - 1);
				break;

			case USE_ADD_ATTRIBUTE:
				if (ITEM_COSTUME == item2->GetType())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼Ó¼ºÀ» º¯°æÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
					return false;
				}

				// else if ((item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR && item2->GetVnum() < 45007) || (item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR && item2->GetVnum() > 45999))
				if (item->IsPremiumBonusItem() && !item2->bonusOnlyByPremiumItem())
					return false;
				
				if (item2->GetAttributeSetIndex() == -1)
				{
					return false;
				}

				if (item2->GetAttributeCount() < 5)
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					//if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
					for (int i =0 ; i<5; i++)
					{	
						item2->AddAttribute();

						int iAddedIdx = item2->GetAttributeCount() - 1;
						LogManager::instance().ItemLog(
								GetPlayerID(),
								item2->GetAttributeType(iAddedIdx),
								item2->GetAttributeValue(iAddedIdx),
								item->GetID(),
								"ADD_ATTRIBUTE_SUCCESS",
								buf,
								GetDesc()->GetHostName(),
								item->GetOriginalVnum()); // mzoe bd dzialc xd 
					}
					//xitem->SetCount(item->GetCount() - 1);
				}
				break;

#ifdef ENABLE_SPECIAL_BONUS
			case USE_ADD_ATTRIBUTE_SPECIAL_1:
			{
				const int CHANCE_ADDING = 25; // szansa na wejscie

				if (item2->GetAttributeCount() < 5)
				{
					ChatPacket(CHAT_TYPE_INFO, "Dodaj do tego przedmiotu najpierw 5 podstawowych bonusow!");
					return false;
				}

				if ((item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0) || 
					(item2->GetVnum() >= 0 && item2->GetVnum() <= 0)) // Zbroje
				{
					
					if (item->GetVnum() != 70602)
						return false;
					
					if (item2->GetAttributeValue(5) >= 15)
					{
						ChatPacket(CHAT_TYPE_INFO, "Przedmiot ma maksymalny wklad!");
						return false;
					}

					if (number(1, 100) > CHANCE_ADDING)
					{
						ChatPacket(CHAT_TYPE_INFO, "Dodawanie bonusu zakonczone niepowodzeniem!");
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "Dodawanie bonusu zakonczone powodzeniem!");
						item2->SetForceAttribute(5, 105, item2->GetAttributeValue(5)+1);
					}
					item->SetCount(item->GetCount() - 1);

				}
				else if ((item2->GetVnum() >= 180 && item2->GetVnum() <= 189) || 
					(item2->GetVnum() >= 190 && item2->GetVnum() <= 199) || 
					(item2->GetVnum() >= 1130 && item2->GetVnum() <= 1139) || 
					(item2->GetVnum() >= 3160 && item2->GetVnum() <= 3169) || 
					(item2->GetVnum() >= 2170 && item2->GetVnum() <= 2179)  || 
					(item2->GetVnum() >= 5120 && item2->GetVnum() <= 5129)) // bronie
				{
					if (item->GetVnum() != 70603)
						return false;

					if (item2->GetAttributeValue(5) >= 15)
					{
						ChatPacket(CHAT_TYPE_INFO, "Przedmiot ma maksymalny wklad!");
						return false;
					}

					BYTE value = item2->GetAttributeValue(5);

					value += 1;

					if (number(1, 100) > CHANCE_ADDING)
					{
						ChatPacket(CHAT_TYPE_INFO, "Dodawanie bonusu zakonczone niepowodzeniem!");
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, "Dodawanie bonusu zakonczone powodzeniem!");
						item2->SetForceAttribute(5, 63, value);
					}
				
					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, "Nie mozesz uzyc tego z tym przedmiotem!");
					return false;
				}
			}
			break;
#endif
			case USE_ADD_NEW_BONUS:
			{
#ifdef ENABLE_BELT_ATTRIBUTES
				if (item2->GetType() == ITEM_BELT) {
					if (item->GetVnum() == 79010) {
						if (item2->GetAttributeCount() < 4) {
							if (number(1, 100) > 80) {
								item2->AddBeltAttribute();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Dodawanie bonusu zakonczone powodzeniem!"));
							} else {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Dodawanie bonusu zakonczone niepowodzeniem!"));
							}
							
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Przedmiot ma maksymalny wklad!"));
						}
					} if (item->GetVnum() == 79011) {
						if (item2->GetAttributeCount() > 0) {
							item2->ChangeBeltAttribute();
							ChatPacket(CHAT_TYPE_INFO, "Zmiana bonusu zakonczona powodzeniem!");
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Przedmiot nie ma bonusów, które móg³byœ zmieniæ.");
						}
					}
				}
#endif
#ifdef ENABLE_GLOVE_SYSTEM
				if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
					if (item->GetVnum() == 79013) {
						if (item2->GetAttributeCount() < 5) {
							if (number(1, 100) <= 50) {
								item2->AddGloveAttribute();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Dodawanie bonusu zakonczone powodzeniem!"));
								if (item2->GetSocket(0) > 0) {
									for (int i = 3; i < 5; i++) {
										item2->SetForceAttribute(i, item2->GetAttributeType(i), item2->GetAttributeValue(i) + (item2->GetAttributeValue(i)*20)/100);
									}
								}
							} else {
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Dodawanie bonusu zakonczone niepowodzeniem!"));
							}
							item->SetCount(item->GetCount() - 1);
						} else {
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Przedmiot ma maksymalny wklad!"));
							return false;
						}
					} if (item->GetVnum() == 79012) {
						if (item2->GetSocket(0) == 0) {
							if (item2->GetAttributeCount() > 3) {
								item2->ChangeGloveAttribute();
								ChatPacket(CHAT_TYPE_INFO, "Zmiana bonusu zakonczona powodzeniem!");
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "Nie masz bonusów, które móg³byœ zmieniæ.");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz zmieniaæ bonusów jeœli rêkawica jest wzmocniona.");
							return false;
						}
					}
				}
#endif
			}
			break;

			case USE_ADD_EXTRA_BONUS:
			{
				if (item->GetVnum() == 34000) {
					if (item2->GetVnum() >= 85001 && item2->GetVnum() <= 85024) {
						if (item2->GetSocket(0) > 29) {
							if (item2->GetSocket(2) == 0) {
								item2->SetSocket(0, 45);
								item2->SetSocket(2, 1);
								ChatPacket(CHAT_TYPE_INFO, "Szarfa zosta³a wzmocniona!");
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "Szarfa zosta³a ju¿ wzmocniona!");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Szarfa ma za ma³y poziom Absorpcji. Wymagany: 30%");
							return false;
						}
					}
					if (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_GLOVE) {
						if (item2->GetVnum() == 23005) {
							if (item2->GetAttributeCount() > 4) {
								if (item2->GetSocket(0) == 0) {
									item2->SetSocket(0, 1);
									item->SetCount(item->GetCount() - 1);
									for (int i = 3; i < 5; i++) {
										item2->SetForceAttribute(i, item2->GetAttributeType(i), item2->GetAttributeValue(i) + (item2->GetAttributeValue(i)*20)/100);
									}
									ChatPacket(CHAT_TYPE_INFO, "Rêkawice zosta³y wzmocnione!");
								} else {
									ChatPacket(CHAT_TYPE_INFO, "Rêkawice zosta³y ju¿ wzmocnione!");
									return false;
								}
							} else {
								ChatPacket(CHAT_TYPE_INFO, "Rêkawice musz¹ mieæ 5 bonusów!");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Mo¿na u¿yæ tylko na rêkawicach +5");
							return false;
						}
					}
					if (item2->GetType() == ITEM_NEW_PET_EQ) {
						if (item2->GetVnum() == 80109 || item2->GetVnum() == 80119 || item2->GetVnum() == 80129) {
							if (item2->GetSocket(0) == 0) {
								item2->SetSocket(0, 1);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, "Przedmiot zosta³ wzmocnione!");
							} else {
								ChatPacket(CHAT_TYPE_INFO, "Przedmiot zosta³ ju¿ wzmocniony!");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Mo¿na u¿yæ tylko na przedmiotach +9");
							return false;
						}
					}
					if (item2->GetType() == ITEM_RUNE || item2->GetType() == ITEM_RUNE_RED || item2->GetType() == ITEM_RUNE_BLUE || item2->GetType() == ITEM_RUNE_GREEN || item2->GetType() == ITEM_RUNE_YELLOW || item2->GetType() == ITEM_RUNE_BLACK) {
						if (item2->GetVnum() == 55045 || item2->GetVnum() == 55055 || item2->GetVnum() == 55065 || item2->GetVnum() == 55075 || item2->GetVnum() == 55085 || item2->GetVnum() == 55095) {
							if (item2->GetSocket(0) == 0) {
								item2->SetSocket(0, 1);
								ChatPacket(CHAT_TYPE_INFO, "Runa zosta³a wzmocniona!");
								item->SetCount(item->GetCount() - 1);
							} else {
								ChatPacket(CHAT_TYPE_INFO, "Runa zosta³a ju¿ wzmocniona!");
								return false;
							}
						} else {
							ChatPacket(CHAT_TYPE_INFO, "Mo¿na u¿yæ tylko na runach +5");
							return false;
						}
					}
				}
			}
			break;

			case USE_ADD_IS_BONUS:
			{
				if (item2->GetVnum() == 53012 || item2->GetVnum() == 45924 || item2->GetVnum() == 45925 || item2->GetVnum() == 41924 || item2->GetVnum() == 41925) {
					return false;
				}

				if (CItemVnumHelper::IsBlockedCostumes(item2->GetVnum())) return false;

				LPITEM item2;
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
					return false;

				if (!item2->IsItemShopBonusExpireItem())
				{
					ChatPacket(CHAT_TYPE_INFO, "ITEMSHOP_TIMED_BONUSES_ATTR_WRONG_TYPE");
					return false;
				}

				if (item2->GetSocket(2) != 0 && (item->GetVnum() == item2->GetSocket(2)))
				{
					ChatPacket(CHAT_TYPE_INFO, "Ten przedmiot ma ju¿ bonus.");
					return false;
				}

				if (item->GetValue(0) == COSTUME_BODY && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_BODY))
				{
					ChatPacket(CHAT_TYPE_INFO, "Z³y przedmiot.");
					return false;
				}

				if (item->GetValue(0) == COSTUME_HAIR && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_HAIR))
				{
					ChatPacket(CHAT_TYPE_INFO, "Z³y przedmiot.");
					return false;
				}

				if (item->GetValue(0) == COSTUME_WEAPON && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_WEAPON))
				{
					ChatPacket(CHAT_TYPE_INFO, "Z³y przedmiot.");
					return false;
				}

				if (item->GetValue(0) == COSTUME_STOLE && !(item2->GetType() == ITEM_COSTUME && item2->GetSubType() == COSTUME_STOLE))
				{
					ChatPacket(CHAT_TYPE_INFO, "Z³y przedmiot.");
					return false;
				}

				if (item->GetValue(0) == 10 && !(item2->GetType() == ITEM_NEW_PET))
				{
					ChatPacket(CHAT_TYPE_INFO, "Z³y przedmiot.");
					return false;
				}

				item->SetCount(item->GetCount() - 1);
				item2->SetSocket(1, get_global_time() + item->GetValue(2));
				item2->SetSocket(2, item->GetVnum());
				item2->StartItemshopBonusTimeExpireEvent();
			}
			break;

			case USE_ADD_ACCESSORY_SOCKET:
			{
				char buf[21];
				snprintf(buf, sizeof(buf), "%u", item2->GetID());

				if (item2->IsAccessoryForSocket())
				{
					if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
					{
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= 50)
#else
						if (1)
#endif
						{
							item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÄÏÀÌ ¼º°øÀûÀ¸·Î Ãß°¡µÇ¾ú½À´Ï´Ù."));
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÄÏ Ãß°¡¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾×¼¼¼­¸®¿¡´Â ´õÀÌ»ó ¼ÒÄÏÀ» Ãß°¡ÇÒ °ø°£ÀÌ ¾ø½À´Ï´Ù."));
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀ¸·Î ¼ÒÄÏÀ» Ãß°¡ÇÒ ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
				}
			}
			break;

			case USE_PUT_INTO_BELT_SOCKET:
			case USE_PUT_INTO_ACCESSORY_SOCKET:
				if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
					{
						if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
						{
							item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀåÂø¿¡ ¼º°øÇÏ¿´½À´Ï´Ù."));
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀåÂø¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						if (item2->GetAccessorySocketMaxGrade() == 0)
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸ÕÀú ´ÙÀÌ¾Æ¸óµå·Î ¾Ç¼¼¼­¸®¿¡ ¼ÒÄÏÀ» Ãß°¡ÇØ¾ßÇÕ´Ï´Ù."));
						else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾×¼¼¼­¸®¿¡´Â ´õÀÌ»ó ÀåÂøÇÒ ¼ÒÄÏÀÌ ¾ø½À´Ï´Ù."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ÙÀÌ¾Æ¸óµå·Î ¼ÒÄÏÀ» Ãß°¡ÇØ¾ßÇÕ´Ï´Ù."));
						}
						else
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾×¼¼¼­¸®¿¡´Â ´õÀÌ»ó º¸¼®À» ÀåÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀ» ÀåÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
				}
				break;
			}
			if (item2->IsEquipped())
			{
				BuffOnAttr_AddBuffsFromItem(item2);
			}
		}
		break;
		//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

		case USE_BAIT:
		{
			if (m_pkFishingEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³¬½Ã Áß¿¡ ¹Ì³¢¸¦ °¥¾Æ³¢¿ï ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}

			LPITEM weapon = GetWear(WEAR_WEAPON);

			if (!weapon || weapon->GetType() != ITEM_ROD)
				return false;

			if (weapon->GetSocket(2))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ²ÈÇôÀÖ´ø ¹Ì³¢¸¦ »©°í %s¸¦ ³¢¿ó´Ï´Ù."), item->GetName());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³¬½Ã´ë¿¡ %s¸¦ ¹Ì³¢·Î ³¢¿ó´Ï´Ù."), item->GetName());
			}

			weapon->SetSocket(2, item->GetValue(0));
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_MOVE:
		case USE_TREASURE_BOX:
		case USE_MONEYBAG:
			break;

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
		case USE_COLLECTION_SCROLL:
		{
			if (!CanHandleItem() || item->IsExchanging())
			{
				return false;
			}

			if (GetQuestFlag("collection.percent_up"))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already increased your perecent."));
				return false;
			}

			SetQuestFlag("collection.percent_up", 1);
			ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionIncrease %d", 1);
			item->SetCount(item->GetCount() - 1);
		}
		break;
#endif

		case USE_AFFECT:
		{
	        const auto affectType = item->GetValue(0);
	        const auto pointType = aApplyInfo[item->GetValue(1)].bPointType;
	        const auto affectValue = item->GetValue(2);

	        const auto affectDuration = item->GetValue(3);

#ifdef __PREMIUM_PRIVATE_SHOP__
			if (affectType == AFFECT_PREMIUM_PRIVATE_SHOP)
			{
				if (SetPremiumPrivateShopBonus(affectDuration))
					item->SetCount(item->GetCount() - 1);
				return true;
			}
#endif

			if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì È¿°ú°¡ °É·Á ÀÖ½À´Ï´Ù."));
			}
			else
			{
				AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
				if (item->GetVnum() == 39024 || item->GetVnum() == 39025 || item->GetVnum() == 71031)
				{
				}
				else {
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;

		case USE_CREATE_STONE:
			AutoGiveItem(number(28000, 28013));
			item->SetCount(item->GetCount() - 1);
			break;

		case USE_RECIPE:
		{
			LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
			DWORD dwSourceCount1 = item->GetValue(2);

			LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
			DWORD dwSourceCount2 = item->GetValue(4);

			if (dwSourceCount1 != 0)
			{
				if (pSource1 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹°¾à Á¶ÇÕÀ» À§ÇÑ Àç·á°¡ ºÎÁ·ÇÕ´Ï´Ù."));
					return false;
				}
			}

			if (dwSourceCount2 != 0)
			{
				if (pSource2 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹°¾à Á¶ÇÕÀ» À§ÇÑ Àç·á°¡ ºÎÁ·ÇÕ´Ï´Ù."));
					return false;
				}
			}

			if (pSource1 != NULL)
			{
				if (pSource1->GetCount() < dwSourceCount1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Àç·á(%s)°¡ ºÎÁ·ÇÕ´Ï´Ù."), pSource1->GetName());
					return false;
				}

				pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
			}

			if (pSource2 != NULL)
			{
				if (pSource2->GetCount() < dwSourceCount2)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Àç·á(%s)°¡ ºÎÁ·ÇÕ´Ï´Ù."), pSource2->GetName());
					return false;
				}

				pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
			}

			LPITEM pBottle = FindSpecifyItem(50901);

			if (!pBottle || pBottle->GetCount() < 1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ºó º´ÀÌ ¸ðÀÚ¸¨´Ï´Ù."));
				return false;
			}

			pBottle->SetCount(pBottle->GetCount() - 1);

			if (number(1, 100) > item->GetValue(5))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹°¾à Á¦Á¶¿¡ ½ÇÆÐÇß½À´Ï´Ù."));
				return false;
			}

			AutoGiveItem(item->GetValue(0));
		}
		break;
		}
	}
	break;

	case ITEM_METIN:
	{
		LPITEM item2;

		if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			return false;

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			return false;

		if (item2->GetType() == ITEM_PICK) return false;
		if (item2->GetType() == ITEM_ROD) return false;

		int i;

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			DWORD dwVnum;

			if ((dwVnum = item2->GetSocket(i)) <= 2)
				continue;

			TItemTable* p = ITEM_MANAGER::instance().GetTable(dwVnum);

			if (!p)
				continue;

			if (item->GetValue(5) == p->alValues[5])
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°°Àº Á¾·ùÀÇ ¸ÞÆ¾¼®Àº ¿©·¯°³ ºÎÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
		}

		if (item2->GetType() == ITEM_ARMOR)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¸ÞÆ¾¼®Àº Àåºñ¿¡ ºÎÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
		}
		else if (item2->GetType() == ITEM_WEAPON)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¸ÞÆ¾¼®Àº ¹«±â¿¡ ºÎÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ºÎÂøÇÒ ¼ö ÀÖ´Â ½½·ÔÀÌ ¾ø½À´Ï´Ù."));
			return false;
		}

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
			{
        		int successRate = 30;
				
				if (item->GetVnum() >= 28930 && item->GetVnum() <= 28943)
				{
					successRate = 100;
				}

#ifdef ENABLE_ADDSTONE_FAILURE
        		if (number(1, 100) <= successRate)
#else
        		if (1)
#endif
        		{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸ÞÆ¾¼® ºÎÂø¿¡ ¼º°øÇÏ¿´½À´Ï´Ù."));
					item2->SetSocket(i, item->GetVnum());
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸ÞÆ¾¼® ºÎÂø¿¡ ½ÇÆÐÇÏ¿´½À´Ï´Ù."));
#ifndef ENABLE_NEW_STONE_DETACH
					item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
#endif
				}

				LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
				item->SetCount(item->GetCount() - 1);
				// ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
				break;
			}

		if (i == ITEM_SOCKET_MAX_NUM)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ºÎÂøÇÒ ¼ö ÀÖ´Â ½½·ÔÀÌ ¾ø½À´Ï´Ù."));
	}
	break;

	case ITEM_AUTOUSE:
	case ITEM_MATERIAL:
	case ITEM_SPECIAL:
	case ITEM_TOOL:
	case ITEM_LOTTERY:
		break;

	case ITEM_TOTEM:
	{
		if (!item->IsEquipped())
			EquipItem(item);
	}
	break;

#ifdef ENABLE_EXTENDED_BLEND
	case ITEM_BLEND:
		// sys_log(0,"ITEM_BLEND!!");
		if (Blend_Item_find(item->GetVnum()))
		{
			int 	affect_type = GetAffectType(item);
			int		apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int		apply_value = item->GetSocket(0);
			bool	active_blend = (item->GetSocket(1) == 1) ? true : false;
			int		apply_duration = item->GetSocket(2);

			// This is unnecessary if you aren't going to use both items(I mean use only the blends for an example).
			if (!active_blend && (((affect_type == AFFECT_BLEND || affect_type == AFFECT_BLEND_EX) && (FindAffect(AFFECT_BLEND, apply_type) || FindAffect(AFFECT_BLEND_EX, apply_type)))
				|| ((affect_type == AFFECT_WATER && (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, apply_type) || FindAffect(AFFECT_WATER, apply_type))))
				|| ((affect_type == AFFECT_MALL_EX && (FindAffect(AFFECT_MALL, apply_type) || FindAffect(AFFECT_MALL_EX, apply_type)))))
				)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				return false;
			}

			if ((affect_type == AFFECT_BLEND_EX || affect_type == AFFECT_WATER || affect_type == AFFECT_MALL_EX))
			{
				if (active_blend)
				{
					RemoveAffect(FindAffect(affect_type, apply_type));
					item->Lock(false);
					item->SetSocket(1, 0);
				}
				else
				{
					AddAffect(affect_type, apply_type, apply_value, 0, INFINITE_AFFECT_DURATION, 0, false);
					item->Lock(true);
					item->SetSocket(1, 1);
				}
			}
			else
			{
				AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
				if (item->GetVnum() == 50821 || item->GetVnum() == 50822 || item->GetVnum() == 50823
				|| item->GetVnum() == 50824 || item->GetVnum() == 50825 || item->GetVnum() == 50826) {
					return false;
				} else {
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;
#else
	// Your old case here
	case ITEM_BLEND:

		sys_log(0, "ITEM_BLEND!!");
		if (Blend_Item_find(item->GetVnum()))
		{
			int		affect_type = AFFECT_BLEND;
			int		apply_type = aApplyInfo[item->GetSocket(0)].bPointType;
			int		apply_value = item->GetSocket(1);
			int		apply_duration = item->GetSocket(2);

			if (FindAffect(affect_type, apply_type))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			}
			else
			{
				// if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
				// {
					// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				// }
				// else
				// {
				AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
				item->SetCount(item->GetCount() - 1);
				// }
			}
		}
		break;
#endif
	case ITEM_EXTRACT:
	{
		LPITEM pDestItem = GetItem(DestCell);
		if (NULL == pDestItem)
		{
			return false;
		}
		switch (item->GetSubType())
		{
		case EXTRACT_DRAGON_SOUL:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
			}
			return false;
		case EXTRACT_DRAGON_HEART:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
			}
			return false;
		default:
			return false;
		}
	}
	break;
	
	case ITEM_GACHA:
		{
			OpenChestItem(item);
		}
		break;

	case ITEM_NONE:
		sys_err("Item type NONE %s", item->GetName());
		break;

	default:
		sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
		return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	//We don't want to use it if we are dragging it over another item of the same type...
	CItem* destItem = GetItem(DestCell);
	if (destItem && item != destItem && destItem->IsStackable() && !IS_SET(destItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && destItem->GetVnum() == item->GetVnum())
	{
		if (MoveItem(Cell, DestCell, 0))
			return false;
	}

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot remove item from switchbot. Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
	// else
	// {
		// if(item->GetSubType() != USE_CHANGE_ATTRIBUTE)
		// {
			// if (thecore_pulse() > use_item_anti_flood_pulse() + PASSES_PER_SEC(1))
			// {
				// set_use_item_anti_flood_count(0);
				// set_use_item_anti_flood_pulse(thecore_pulse());
			// }

			// if (increase_use_item_anti_flood_count() >= 10)
			// {
				// GetDesc()->DelayedDisconnect(0);
				// return false;
			// }
		// }
	// }
#endif
	if (Cell.IsBuffEquipmentPosition())
	{
	}
	else
		
		if (!item->CanUsedBy(this))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁ÷ÀÌ ¸ÂÁö¾Ê¾Æ ÀÌ ¾ÆÀÌÅÛÀ» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºº°ÀÌ ¸ÂÁö¾Ê¾Æ ÀÌ ¾ÆÀÌÅÛÀ» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (Cell.IsBuffEquipmentPosition())
	{
		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use items while editing your personal shop."));
		return false;
	}
#endif

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ï°Å¸® ÀüÅõ Âü°¡Áß¿¡´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
		int iPulse = thecore_pulse();

		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¢°í¸¦ ¿¬ÈÄ %dÃÊ ÀÌ³»¿¡´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ¸¦ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);

			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			return false;
		}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
#else
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
#endif
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡Ã¢,Ã¢°í µîÀ» ¿¬ »óÅÂ¿¡¼­´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ ¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

		//PREVENT_REFINE_HACK

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ °³·®ÈÄ %dÃÊ ÀÌ³»¿¡´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ¸¦ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK

		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°³ÀÎ»óÁ¡ »ç¿ëÈÄ %dÃÊ ÀÌ³»¿¡´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ¸¦ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}

			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x, 2) + pow((float)y, 2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌµ¿ µÇ¾îÁú À§Ä¡¿Í ³Ê¹« °¡±î¿ö ±ÍÈ¯ºÎ¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant, nDist);
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE

		if (iPulse - GetExchangeTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡ ÈÄ %dÃÊ ÀÌ³»¿¡´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎµîÀ» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."), g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049))
	{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || GetOfflineShopOwner())
#else
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
#endif
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡Ã¢,Ã¢°í µîÀ» ¿¬ »óÅÂ¿¡¼­´Â º¸µû¸®,ºñ´Üº¸µû¸®¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

	}

	//END_PREVENT_TRADE_WINDOW

#ifdef ENABLE_EXTENDED_BLEND
	// Again
	// This is unnecessary if you aren't going to use both items(I mean use only the blends for an example).
	if (item->GetType() == ITEM_USE && (item->GetSubType() == USE_AFFECT || item->GetSubType() == USE_ABILITY_UP))
	{
		int apply_type;
		if ((item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT))
			apply_type = aApplyInfo[item->GetValue(1)].bPointType;
		else
			apply_type = aApplyInfo[item->GetValue(0)].bPointType;

		if ((item->IsWaterItem() && FindAffect(AFFECT_WATER, apply_type)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			return false;
		}

		if ((item->IsDragonGodItem() && FindAffect(AFFECT_MALL_EX, apply_type)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			return false;
		}
	}
#endif

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int)sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);
		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
		return UseItemEx(item, DestCell);
}

#ifdef __EXTENDED_ITEM_COUNT__
bool CHARACTER::DropItem(TItemPos Cell, int bCount)
#else
bool CHARACTER::DropItem(TItemPos Cell, BYTE bCount)
#endif
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°­È­Ã¢À» ¿¬ »óÅÂ¿¡¼­´Â ¾ÆÀÌÅÛÀ» ¿Å±æ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}
#ifdef ENABLE_NEWSTUFF
	if (g_ItemDropTimeLimitValue && !PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::ItemDrop, std::chrono::milliseconds(g_ItemDropTimeLimitValue)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ °ñµå¸¦ ¹ö¸± ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}
#endif
	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹ö¸± ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù."));
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
#ifdef __EXTENDED_ITEM_COUNT__
		bCount = (int)item->GetCount();
#else
		bCount = (BYTE)item->GetCount();
#endif

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Wyrzucony przedmiot zniknie za 10 sekund."));
#ifdef ENABLE_NEWSTUFF
		pkItemToDrop->StartDestroyEvent(10);
#else
		pkItemToDrop->StartDestroyEvent();
#endif

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}

bool CHARACTER::DropGold(int gold)
{
	return false;
}

#ifdef ENABLE_CHEQUE_SYSTEM
bool CHARACTER::DropCheque(long long cheque)
{
	if (cheque <= 0 || (long long)cheque > GetCheque())
		return false;

	if (!CanHandleItem())
		return false;

	if (0 != 30)
	{
		if (get_dword_time() < m_dwLastChequeDropTime + 30)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ °ñµå¸¦ ¹ö¸± ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
	}

	m_dwLastChequeDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem(80020, cheque);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			PointChange(POINT_CHEQUE, -cheque, true);

			if (cheque > 1000)
				LogManager::instance().CharLog(this, cheque, "DROP_CHEQUE", "");

			if (false == LC_IsBrazil())
			{
				item->StartDestroyEvent(150);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¶³¾îÁø ¾ÆÀÌÅÛÀº %lldºÐ ÈÄ »ç¶óÁý´Ï´Ù."), 150 / 60);
			}
			else
			{
				item->StartDestroyEvent(60);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¶³¾îÁø ¾ÆÀÌÅÛÀº %lldºÐ ÈÄ »ç¶óÁý´Ï´Ù."), 1);
			}
		}

		Save();
		return true;
	}

	return false;
}
#endif

#ifdef __EXTENDED_ITEM_COUNT__
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, int count)
#else
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, BYTE count)
#endif
{
	{
        TItemPos tempSrc = Cell;
        TItemPos tempDst = DestCell;
        if (tempSrc.window_type == EQUIPMENT)
            tempSrc.window_type = INVENTORY;

        if (tempDst.window_type == EQUIPMENT)
            tempDst.window_type = INVENTORY;

        if (tempSrc == tempDst)
            return false;
    }
	
	if (Cell == DestCell) // @fixme196
		return false;

	if (!IsValidItemPosition(Cell))
		return false;

	LPITEM item = NULL;
	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

// #ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	// if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_AND_EQUIP_SLOT_MAX && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		// return false;
// #else
	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;
// #endif

	if (true == item->isLocked())
		return false;

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°­È­Ã¢À» ¿¬ »óÅÂ¿¡¼­´Â ¾ÆÀÌÅÛÀ» ¿Å±æ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move items while your personal shop window is open."));
		return false;
	}
#endif

	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº º§Æ® ÀÎº¥Åä¸®·Î ¿Å±æ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (DestCell.IsBuffEquipmentPosition())
	{
		if (DestCell.IsBuffEquipmentPosition()) {
			if (!EquipBuffItem(DestCell.cell, item)) {
				return false;
			}
		}
		else if (Cell.IsBuffEquipmentPosition() && INVENTORY == DestCell.window_type && DestCell.cell >= INVENTORY_MAX_NUM) {
			if (!UnequipBuffItem(Cell.cell, item)) {
				return false;
			}
		}
	}
#endif
#ifdef ENABLE_SWITCHBOT
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move active switchbot item."));
		return false;
	}

	if ((DestCell.IsSwitchbotPosition() && item->IsEquipped()) || (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition()))
	{
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item type for switchbot."));
		return false;
	}

	if (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot equip items directly from switchbot."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && Cell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move equipped items to switchbot."));
		return false;
	}
#endif


#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	if (DestCell.IsSkillBookInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (DestCell.IsUpgradeItemsInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (DestCell.IsStoneInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (DestCell.IsBoxInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (DestCell.IsEfsunInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (DestCell.IsCicekInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if ((Cell.IsSkillBookInventoryPosition() && !DestCell.IsSkillBookInventoryPosition() && !DestCell.IsDefaultInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsUpgradeItemsInventoryPosition() && !DestCell.IsUpgradeItemsInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsStoneInventoryPosition() && !DestCell.IsStoneInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsBoxInventoryPosition() && !DestCell.IsBoxInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsEfsunInventoryPosition() && !DestCell.IsEfsunInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsCicekInventoryPosition() && !DestCell.IsCicekInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
		return false;
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsSkillBookInventoryPosition())
	{
		if (!item->IsSkillBook())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsUpgradeItemsInventoryPosition())
	{
		if (!item->IsUpgradeItem())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsStoneInventoryPosition())
	{
		if (!item->IsStone())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsBoxInventoryPosition())
	{
		if (!item->IsBox())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsEfsunInventoryPosition())
	{
		if (!item->IsEfsun())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsCicekInventoryPosition())
	{
		if (!item->IsCicek())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz przenieœæ tego przedmiotu do tego ekwipunku.");
			return false;
		}
	}
#endif
#ifdef __RENEWAL_MOUNT__
	if (DestCell.IsDefaultInventoryPosition() && Cell.IsEquipPosition())
	{
		if (item->IsCostumeMountItem() && GetHorse())
		{
			HorseSummon(false);
		}
	}
#endif
	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì Àåºñ¸¦ Âø¿ëÇÏ°í ÀÖ½À´Ï´Ù."));

			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
			!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
			item2->GetVnum() == item->GetVnum())
		{
			// for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i) {
			// 	if (item2->GetSocket(i) != item->GetSocket(i)) {
			// 		if (item2->GetType() != ITEM_GACHA && item->GetType() != ITEM_GACHA) {
			// 			return false;
			// 		}
			// 	}
			// }

			if (count == 0)
#ifdef __EXTENDED_ITEM_COUNT__
				count = (int)item->GetCount();
#else
				count = (BYTE)item->GetCount();
#endif

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			// if (item->GetType() == ITEM_GACHA && item2->GetType() == ITEM_GACHA) {
			// 	int newSocketValue = MIN(g_bItemCountLimit - item2->GetSocket(0), item->GetSocket(0));
			// 	item->SetSocket(0, item->GetSocket(0) - newSocketValue);
			// 	item2->SetSocket(0, item2->GetSocket(0) + newSocketValue);
			// }

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif
			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
				DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);

			char szBuf[51 + 1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item)
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center)
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, iMoney, true);

					if (iMoney > 1000)
					{
						LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", ""));
					}
				}
		}
	};

#ifdef ENABLE_CHEQUE_SYSTEM
	struct FChequeDistributor
	{
		int total;
		LPCHARACTER c;
		int x, y;
		int iCheque;

		FChequeDistributor(LPCHARACTER center, int iCheque)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iCheque(iCheque)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_CHEQUE, iCheque, true);

					if (iCheque > 1000)
						LogManager::instance().CharLog(ch, iCheque, "GET_CHEQUE", "");
				}
		}
	};
#endif

#ifdef ENABLE_PUNKTY_OSIAGNIEC
	struct FPktOsiagDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iPktOsiag;

		FPktOsiagDistributor(LPCHARACTER center, int iPktOsiag)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iPktOsiag(iPktOsiag)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_PKT_OSIAG, iPktOsiag, true);

					if (iPktOsiag > 1000)
						LogManager::instance().CharLog(ch, iPktOsiag, "GET_PktOsiag", "");
				}
		}
	};
#endif

}

#ifdef ENABLE_LONG_LONG
void CHARACTER::GiveGold(long long iAmount)
#else
void CHARACTER::GiveGold(int iAmount)
#endif
{
	if (iAmount <= 0)
		return;

#ifdef ENABLE_LONG_LONG
	sys_log(0, "GIVE_GOLD: %s %lld", GetName(), iAmount);
#else
	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

#ifdef ENABLE_LONG_LONG
		long long dwTotal = iAmount;
		long long dwMyAmount = dwTotal;
#else
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, dwMyAmount, true);
		if (dwMyAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", ""));
		}
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);
		if (iAmount > 1000)
		{
			LOG_LEVEL_CHECK(LOG_LEVEL_MAX, LogManager::instance().CharLog(this, iAmount, "GET_GOLD", ""));
		}
	}
}

#ifdef ENABLE_CHEQUE_SYSTEM
void CHARACTER::GiveCheque(long long iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_CHEQUE: %s %lld", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FChequeDistributor funcChequeDist(this, dwShare);

			pParty->ForEachOnlineMember(funcChequeDist);
		}

		PointChange(POINT_CHEQUE, dwMyAmount, true);

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_CHEQUE", "");
	}
	else
	{
		PointChange(POINT_CHEQUE, iAmount, true);

		if (LC_IsBrazil() == true)
		{
			if (iAmount >= 213)
				LogManager::instance().CharLog(this, iAmount, "GET_CHEQUE", "");
		}
		else
		{
			if (iAmount > 1000)
				LogManager::instance().CharLog(this, iAmount, "GET_CHEQUE", "");
		}
	}
}
#endif

#ifdef ENABLE_STONE_POINT_SYSTEM
void CHARACTER::GivePktOsiag(int iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_PktOsiag: %s %lld", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FPktOsiagDistributor funcPktOsiagDist(this, dwShare);

			pParty->ForEachOnlineMember(funcPktOsiagDist);
		}

		PointChange(POINT_PKT_OSIAG, dwMyAmount, true);

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_PktOsiag", "");
	}
	else
	{
		PointChange(POINT_PKT_OSIAG, iAmount, true);

		if (LC_IsBrazil() == true)
		{
			if (iAmount >= 213)
				LogManager::instance().CharLog(this, iAmount, "GET_PktOsiag", "");
		}
		else
		{
			if (iAmount > 1000)
				LogManager::instance().CharLog(this, iAmount, "GET_PktOsiag", "");
		}
	}
}
#endif

bool CHARACTER::PickupItem(DWORD dwVID, bool isPremiumPickUp)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this) || isPremiumPickUp)
	{
		// @fixme150 BEGIN
		if (item->GetType() == ITEM_QUEST && !isPremiumPickUp)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
				return false;
			}
		}
		// @fixme150 END

		if (item->IsOwnership(this))
		{
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM(item);

				Save();
			}

#ifdef ENABLE_CHEQUE_SYSTEM
			else if (item->GetType() == ITEM_CHEQUE)
			{
				if (item->GetCount() + GetCheque() > CHEQUE_MAX - 1)
					return false;
				GiveCheque(item->GetCount());
				item->RemoveFromGround();
				M2_DESTROY_ITEM(item);
				Save();
			}
#endif

			else
			{
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
#ifdef __EXTENDED_ITEM_COUNT__
					int bCount = item->GetCount();
#else
					BYTE bCount = item->GetCount();
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
					{
						LPITEM item2 = GetInventoryItem(i);

						if (!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									if (item2->GetType() != ITEM_GACHA)
										break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;

#ifdef __EXTENDED_ITEM_COUNT__
							int bCount2 = MIN(g_bItemCountLimit - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

							if (item2->GetType() == ITEM_GACHA && item->GetType() == ITEM_GACHA)
								item2->SetSocket(0, item2->GetSocket(0) + item->GetSocket(0));

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
								#if defined(__CHATTING_WINDOW_RENEWAL__)
								ChatPacket(CHAT_TYPE_ITEM_INFO, "Otrzyma³eœ: (%ux) %s", bCount2, item->GetName());
								#else
								ChatPacket(CHAT_TYPE_INFO, "Otrzyma³eœ: (%ux) %s", bCount2, item->GetName());
								#endif
								// ChatPacket(CHAT_TYPE_INFO, "Otrzyma?ï¿½: (%ux) %s", bCount2, item->GetName());
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem(GetPlayerID(), item2);
								return true;
							}
						}
					}

					item->SetCount(bCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
						return false;
					}
				}
				else
				{
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
						return false;
					}
				}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
				if (!IsWpadanie()) {
					if (item->IsSkillBook())
					{
						if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
					else if (item->IsUpgradeItem())
					{
						if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
					else if (item->IsStone())
					{
						if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
					else if (item->IsBox())
					{
						if ((iEmptyCell = GetEmptyBoxInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptyBoxInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
					else if (item->IsEfsun())
					{
						if ((iEmptyCell = GetEmptyEfsunInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptyEfsunInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
					else if (item->IsCicek())
					{
						if ((iEmptyCell = GetEmptyCicekInventory(item->GetSize())) != -1)
						{
							if ((iEmptyCell = GetEmptyCicekInventory(item->GetSize())) == -1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
								return false;
							}
						}
					}
				}
				else {
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
						return false;
					}
				}
#endif

				item->RemoveFromGround();

				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
				if (!IsWpadanie()) {
					if (item->IsSkillBook())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
					else if (item->IsUpgradeItem())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
					else if (item->IsStone())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
					else if (item->IsBox())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
					else if (item->IsEfsun())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
					else if (item->IsCicek())
						item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				} else {
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				}
#endif

				char szHint[32 + 1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
				#if defined(__CHATTING_WINDOW_RENEWAL__)
				ChatPacket(CHAT_TYPE_ITEM_INFO, "Otrzyma³eœ: (%dx) %s", item->GetCount(), item->GetName());
				#else
				ChatPacket(CHAT_TYPE_INFO, "Otrzyma³eœ: (%dx) %s", item->GetCount(), item->GetName());
				#endif
				// ChatPacket(CHAT_TYPE_INFO, "Otrzyma?ï¿½: (%dx) %s ", item->GetCount(), item->GetName());

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem(GetPlayerID(), item);
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			ChatPacket(CHAT_TYPE_INFO, "test = 1");
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);

			GetParty()->ForEachOnlineMember(funcFindOwnership);

			LPCHARACTER owner = funcFindOwnership.owner;
			// @fixme115
			if (!owner)
				return false;

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
						return false;
					}
				}
			}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (item->IsSkillBook())
			{
				if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
			else if (item->IsUpgradeItem())
			{
				if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
			else if (item->IsStone())
			{
				if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
			else if (item->IsBox())
			{
				if ((iEmptyCell = GetEmptyBoxInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
			else if (item->IsEfsun())
			{
				if ((iEmptyCell = GetEmptyEfsunInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
			else if (item->IsCicek())
			{
				if ((iEmptyCell = GetEmptyCicekInventory(item->GetSize())) == -1)
				{
					sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
					return false;
				}
			}
#endif
			else
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÁöÇÏ°í ÀÖ´Â ¾ÆÀÌÅÛÀÌ ³Ê¹« ¸¹½À´Ï´Ù."));
						return false;
					}
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			else if (item->IsSkillBook())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsUpgradeItem())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsStone())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsBox())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsEfsun())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsCicek())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
#endif
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

			char szHint[32 + 1];
			snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog(owner, item, "GET", szHint);

			if (owner == this)
				ChatPacket(CHAT_TYPE_INFO, "Otrzyma³eœ: (%dx) %s ", item->GetCount(), item->GetName());

			else
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s ´ÔÀ¸·ÎºÎÅÍ %s"), GetName(), item->GetName());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ Àü´Þ: %s ´Ô¿¡°Ô %s"), owner->GetName(), item->GetName());
			}

			if (item->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem(owner->GetPlayerID(), item);

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(WORD bCell, WORD bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);

	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (bCell == bDestCell)
		return false;

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;

	if (item1 == item2)
	{
		sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
		return false;
	}

	if (!IsEmptyItemGrid(TItemPos(INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
				return false;
		}

		if (bEquipCell != item1->FindEquipCell(this))
			return false;

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
		else
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
			return false;
		}
	}
#endif

	if (false == CanUnequipNow(item))
		return false;

#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem() && GetHorse()){
		HorseSummon(false);
	}
#endif

	int pos;
	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
		pos = GetEmptySkillBookInventory(item->GetSize());
	else if (item->IsUpgradeItem())
		pos = GetEmptyUpgradeItemsInventory(item->GetSize());
	else if (item->IsStone())
		pos = GetEmptyStoneInventory(item->GetSize());
	else if (item->IsBox())
		pos = GetEmptyBoxInventory(item->GetSize());
	else if (item->IsEfsun())
		pos = GetEmptyEfsunInventory(item->GetSize());
	else if (item->IsCicek())
		pos = GetEmptyCicekInventory(item->GetSize());
#endif
	else
		pos = GetEmptyInventory(item->GetSize());

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
		item->AddToCharacter(this, TItemPos(SKILL_BOOK_INVENTORY, pos));
	else if (item->IsUpgradeItem())
		item->AddToCharacter(this, TItemPos(UPGRADE_ITEMS_INVENTORY, pos));
	else if (item->IsStone())
		item->AddToCharacter(this, TItemPos(STONE_INVENTORY, pos));
	else if (item->IsBox())
		item->AddToCharacter(this, TItemPos(BOX_INVENTORY, pos));
	else if (item->IsEfsun())
		item->AddToCharacter(this, TItemPos(EFSUN_INVENTORY, pos));
	else if (item->IsCicek())
		item->AddToCharacter(this, TItemPos(CICEK_INVENTORY, pos));
#endif
	else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));

	CheckMaximumPoints();

	return true;
}

//

//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	if (item->GetCount() > 1) 
	{
		ChatPacket(CHAT_TYPE_INFO, "Musisz rozdzieliæ przedmiot przed za³o¿eniem!");
		return false;
	}

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» Åº »óÅÂ¿¡¼­ ¿¹º¹À» ÀÔÀ» ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	// #poly_equip_unequip
	// if (iWearCell != WEAR_ARROW && IsPolymorphed())
	// {
	// 	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â Âø¿ëÁßÀÎ Àåºñ¸¦ º¯°æÇÒ ¼ö ¾ø½À´Ï´Ù."));
	// 	return false;
	// }

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºº°ÀÌ ¸ÂÁö¾Ê¾Æ ÀÌ ¾ÆÀÌÅÛÀ» »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return false;
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW
		&& (dwCurTime - GetLastAttackTime() <= 500 || dwCurTime - m_dwLastSkillTime <= 250))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°¡¸¸È÷ ÀÖÀ» ¶§¸¸ Âø¿ëÇÒ ¼ö ÀÖ½À´Ï´Ù."));
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip the costume weapon. Wrong equipped weapon."));
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if (GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "ÀÌ¹Ì °°Àº Á¾·ùÀÇ ¿ëÈ¥¼®À» Âø¿ëÇÏ°í ÀÖ½À´Ï´Ù.");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}

	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			if (0 == item->GetSocket(1))
			{
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;
#ifdef __ITEM_SHOP__
				if (item->GetSocket(0) > 0)
					duration += item->GetSocket(0);
#endif
				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

#ifdef ENABLE_SYSTEM_RUNE
		// WHITE
		else if (true == CItemVnumHelper::IsRunaWhite(dwVnum))
		{
			this->EffectPacket(SE_RUNA_WHITE_EFFECT);
		}
		// RED
		else if (true == CItemVnumHelper::IsRunaRed(dwVnum))
		{
			this->EffectPacket(SE_RUNA_RED_EFFECT);
		}
		// BLUE
		else if (true == CItemVnumHelper::IsRunaBlue(dwVnum))
		{
			this->EffectPacket(SE_RUNA_BLUE_EFFECT);
		}
		// GREEN
		else if (true == CItemVnumHelper::IsRunaGreen(dwVnum))
		{
			this->EffectPacket(SE_RUNA_GREEN_EFFECT);
		}
		// BLACK
		else if (true == CItemVnumHelper::IsRunaBlack(dwVnum))
		{
			this->EffectPacket(SE_RUNA_BLACK_EFFECT);
		}
		// YELLOW
		else if (true == CItemVnumHelper::IsRunaYellow(dwVnum))
		{
			this->EffectPacket(SE_RUNA_YELLOW_EFFECT);
		}
#endif


		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std);
				}
			}
		}
		else if (
			(ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
			|| (ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			|| (ITEM_COSTUME == item->GetType() && COSTUME_MOUNT == item->GetSubType())
#endif
		)
		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif
#ifdef ENABLE_STOLE_COSTUME
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_STOLE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif

		// if (item->IsNewMountItem()) // @fixme152
		// 	quest::CQuestManager::instance().SIGUse(GetPlayerID(), quest::QUEST_NO_NPC, item, false);
	}

#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem())
	{
		if (!PulseManager::Instance().IncreaseCount(GetPlayerID(), ePulse::RideMount, std::chrono::milliseconds(1500), 2)) {
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz robiæ tego tak szybko.");
			return false;
		}

		LPITEM costumeItem = GetWear(WEAR_COSTUME_MOUNT);
		if (!costumeItem)
		{
			StopRiding();
			HorseSummon(false);
		}
		else
		{
			StopRiding();
			if (GetHorse())
				HorseSummon(false);
			HorseSummon(true, false, costumeItem->GetValue(1));
			// StartRiding();
		}
	}
#endif

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if (0 == bOldValue)
	{
		CBuffOnAttributes* pBuff = NULL;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
			{
				static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
				static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			case POINT_COSTUME_ATTR_BONUS:
			{
				static BYTE abSlot[] = {
					WEAR_COSTUME_BODY,
					WEAR_COSTUME_HAIR,
					#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
					WEAR_COSTUME_WEAPON,
					#endif
					#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					WEAR_COSTUME_MOUNT,
					#endif
					#ifdef ENABLE_NEW_MOUNT_SYSTEM
					WEAR_COSTUME_MOUNT,
					#endif
				};
				static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			default:
				break;
			}
			m_map_buff_on_attrs.emplace(bType, pBuff);

		}
		else
			pBuff = it->second;
		if (pBuff != NULL)
			pBuff->On(bNewValue);
	}
	else
	{
		assert(m_map_buff_on_attrs.end() != it);
		it->second->ChangeBuffValue(bNewValue);
	}
}

LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	return NULL;
}

int CHARACTER::CountSpecifyItem(DWORD vnum, int iExceptionCell) const
{
	int	count = 0;
	LPITEM item;

	// for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	// {
		// item = GetInventoryItem(i);
		// if (NULL != item && item->GetVnum() == vnum)
		// {
			// if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			// {
				// continue;
			// }
			// else
			// {
				// count += item->GetCount();
			// }
		// }
	// }
	
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if(i == iExceptionCell)
			continue;

		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
	{
		item = GetItem(TItemPos(INVENTORY, i));
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count, int iExceptionCell)
{
	if (0 == count)
		return;

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if(i == iExceptionCell)
			continue;

		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int	count = 0;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
}

void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err("item %d 's owner exists!", item->GetID());
		return;
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
	{
		cell = GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		cell = GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		cell = GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsBox())
	{
		cell = GetEmptyBoxInventory(item->GetSize());
	}
	else if (item->IsEfsun())
	{
		cell = GetEmptyEfsunInventory(item->GetSize());
	}
	else if (item->IsCicek())
	{
		cell = GetEmptyCicekInventory(item->GetSize());
	}
#endif
	else
	{
		cell = GetEmptyInventory(item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsStone())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsBox())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsEfsun())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		else if (item->IsCicek())
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

#ifdef __EXTENDED_ITEM_COUNT__
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, int bCount, int iRarePct, bool bMsg)
#else
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
#endif
{
	TItemTable* p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#else
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					// if (bMsg)
					// 	#if defined(__CHATTING_WINDOW_RENEWAL__)
					// 	ChatPacket(CHAT_TYPE_ITEM_INFO, "Otrzyma?œ: (%ux) %s", bCount2, item->GetName());
					// 	#else
					// 	ChatPacket(CHAT_TYPE_INFO, "Otrzyma?œ: (%ux) %s", bCount2, item->GetName());
					// 	#endif
						// ChatPacket(CHAT_TYPE_INFO, "Otrzyma?œ: (%ux) %s", bCount2, item->GetName());

					return item;
				}
			}
		}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}

		for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}

		for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}

		for (int i = BOX_INVENTORY_SLOT_START; i < BOX_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}

		for (int i = EFSUN_INVENTORY_SLOT_START; i < EFSUN_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}

		for (int i = CICEK_INVENTORY_SLOT_START; i < CICEK_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetItem(TItemPos(INVENTORY, i));

			if (!item)
				continue;

#ifdef ENABLE_SORT_INVENTORY_ITEMS
			if (item->GetOriginalVnum() == dwItemVnum && FN_check_item_socket(item))
#else
			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
#endif
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef __EXTENDED_ITEM_COUNT__
				int bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(g_bItemCountLimit - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÀÌÅÛ È¹µæ: %s"), item->GetName());

					return item;
				}
			}
		}
#endif
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
						inv_item->GetSocket(1) == item->GetSocket(1) &&
						inv_item->GetSocket(2) == item->GetSocket(2) &&
						inv_item->GetCount() < g_bItemCountLimit)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						M2_DESTROY_ITEM(item);
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	else if (item->IsSkillBook())
	{
		iEmptyCell = GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		iEmptyCell = GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsBox())
	{
		iEmptyCell = GetEmptyBoxInventory(item->GetSize());
	}
	else if (item->IsEfsun())
	{
		iEmptyCell = GetEmptyEfsunInventory(item->GetSize());
	}
	else if (item->IsCicek())
	{
		iEmptyCell = GetEmptyCicekInventory(item->GetSize());
	}
#endif
	else
		iEmptyCell = GetEmptyInventory(item->GetSize());

	if (iEmptyCell != -1)
	{
		if (bMsg)
			#if defined(__CHATTING_WINDOW_RENEWAL__)
			ChatPacket(CHAT_TYPE_ITEM_INFO, "Otrzyma³eœ: (%dx) %s ", item->GetCount(), item->GetName());
			#else
			ChatPacket(CHAT_TYPE_INFO,"Otrzyma³eœ: (%dx) %s ", item->GetCount(), item->GetName());
			#endif
			// ChatPacket(CHAT_TYPE_INFO, "Otrzyma?œ: (%dx) %s ", item->GetCount(), item->GetName());

		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsStone())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsBox())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsEfsun())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		else if (item->IsCicek())
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef ENABLE_NEWSTUFF
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH &&
			(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			return true;
		break;

	case fishing::FISHER_MOB:
		if (item->GetType() == ITEM_ROD)
			return true;
		break;

		// BUILDING_NPC
	case BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		if (item->GetType() == ITEM_WEAPON &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;

	case BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		if (item->GetType() == ITEM_ARMOR &&
			(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;

	case BLACKSMITH_ACCESSORY_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetType() == ITEM_ARMOR &&
			!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
			item->GetRefinedVnum())
			return true;
		else
			return false;
		break;
		// END_OF_BUILDING_NPC

	case BLACKSMITH_MOB:
		if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
		{
			return true;
		}
		else
		{
			return false;
		}

	case BLACKSMITH2_MOB:
		if (item->GetRefineSet() >= 500)
		{
			return true;
		}
		else
		{
			return false;
		}

	case ALCHEMIST_MOB:
		if (item->GetRefinedVnum())
			return true;
		break;

	case 20101:
	case 20102:
	case 20103:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Áö ¾ÊÀº ¸»¿¡°Ô ¼±ÃÊ¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Àº ¸»¿¡°Ô »ç·á¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20104:
	case 20105:
	case 20106:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Áö ¾ÊÀº ¸»¿¡°Ô ¼±ÃÊ¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Àº ¸»¿¡°Ô »ç·á¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20107:
	case 20108:
	case 20109:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Áö ¾ÊÀº ¸»¿¡°Ô ¼±ÃÊ¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Á×Àº ¸»¿¡°Ô »ç·á¸¦ ¸ÔÀÏ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			return false;
		}
		break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			fishing::Grill(from, item);
		else
		{
			// TAKE_ITEM_BUG_FIX
			from->SetQuestNPCID(GetVID());
			// END_OF_TAKE_ITEM_BUG_FIX
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		}
		break;

		// DEVILTOWER_NPC
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		}
		break;
		// END_OF_DEVILTOWER_NPC

	case BLACKSMITH_MOB:
	case BLACKSMITH2_MOB:
	case BLACKSMITH_WEAPON_MOB:
	case BLACKSMITH_ARMOR_MOB:
	case BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum())
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ ¾ÆÀÌÅÛÀº °³·®ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		}
		break;

	case 20101:
	case 20102:
	case 20103:
	case 20104:
	case 20105:
	case 20106:
	case 20107:
	case 20108:
	case 20109:
		if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
			item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
			item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			from->ReviveHorse();
			item->SetCount(item->GetCount() - 1);
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡°Ô ¼±ÃÊ¸¦ ÁÖ¾ú½À´Ï´Ù."));
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
			item->GetVnum() == ITEM_HORSE_FOOD_2 ||
			item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			from->FeedHorse();
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡°Ô »ç·á¸¦ ÁÖ¾ú½À´Ï´Ù."));
			item->SetCount(item->GetCount() - 1);
			EffectPacket(SE_HPUP_RED);
		}
		break;

	default:
		sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
		from->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}
#endif

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}
#endif

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC(NULL);
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
                                            std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count, bool sendMessage)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP() * dwCount / 100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP() - 1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;
	#ifdef ENABLE_WOLFMAN_CHARACTER
			case CSpecialItemGroup::BLEEDING:
				{
					AttackedByBleeding(NULL);
					bSuccess = true;
				}
				break;
	#endif
			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, sendMessage);

					if (item_get)
					{
						bSuccess = true;
					}
				}
				break;
		}

		if (bSuccess)
		{
			dwItemVnums.emplace_back(dwVnum);
			dwItemCounts.emplace_back(dwCount);
			item_gets.emplace_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ ÀÌ ¸Ó¸®¸¦ »ç¿ëÇÒ ¼ö ¾ø´Â ·¹º§ÀÔ´Ï´Ù."));
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
	case JOB_WARRIOR:
		hair -= 72000;
		break;

	case JOB_ASSASSIN:
		hair -= 71250;
		break;

	case JOB_SURA:
		hair -= 70500;
		break;

	case JOB_SHAMAN:
		hair -= 69750;
		break;
#ifdef ENABLE_WOLFMAN_CHARACTER
	case JOB_WOLFMAN:
		break;
#endif
	default:
		return false;
		break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ¿ÀÏÇÑ ¸Ó¸® ½ºÅ¸ÀÏ·Î´Â ±³Ã¼ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD


static int POLYMORPH_AFFECT[1][40 + 1] = {
	{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10/*20*/,11,11,12,12,13,13,14,14,15,15/*30*/,16,17,18,19,20,21,22,23,24,25,25 },
};

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		SetPolymorph(0);
		RemoveAffect(AFFECT_POLYMORPH);
		return false;
	}
	if (IsHorseRiding())
		StopRiding();

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "To jest z³y przedmiot na handel.");
		item->SetCount(item->GetCount() - 1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, "To jest z³y przedmiot na handel.");
		item->SetCount(item->GetCount() - 1);
		return false;
	}

	switch (item->GetVnum())
	{
	case 70104:
	case 70105:
	case 70106:
	case 70107:
	case 71093:
	{

		sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);


		int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
		if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz siê przemieniæ w postaæ, która ma wy¿szy poziom od Ciebie.");
			return false;
		}

		int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40 * 50));
		iDuration *= 60;

		DWORD dwBonus = 0;

		if (true == LC_IsYMIR() || true == LC_IsKorea())
		{
			dwBonus = GetSkillLevel(POLYMORPH_SKILL_ID) + 60;
		}
		else
		{
			dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID) / 40) * 100;
		}

		AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

		AddAffect(AFFECT_POLYMORPH, POINT_MOV_SPEED, 200, AFF_POLYMORPH, iDuration, 0, false);

		DWORD skillLevel = GetSkillLevel(POLYMORPH_SKILL_ID);
		AddAffect(AFFECT_POLYMORPH, POINT_NORMAL_HIT_DAMAGE_BONUS, POLYMORPH_AFFECT[0][skillLevel], AFF_POLYMORPH, iDuration, 0, false);

		if (item->GetVnum() == 70105) {
			AddAffect(AFFECT_POLYMORPH, POINT_ATTBONUS_MONSTER, 10, AFF_POLYMORPH, iDuration, 0, false);
			AddAffect(AFFECT_POLYMORPH, POINT_ATTBONUS_BOSS, 10, AFF_POLYMORPH, iDuration, 0, false);
		}
		else if (item->GetVnum() == 70106) {
			AddAffect(AFFECT_POLYMORPH, POINT_ATTBONUS_MONSTER, 20, AFF_POLYMORPH, iDuration, 0, false);
			AddAffect(AFFECT_POLYMORPH, POINT_ATTBONUS_BOSS, 15, AFF_POLYMORPH, iDuration, 0, false);
			AddAffect(AFFECT_POLYMORPH, POINT_ATTBONUS_LEGENDA, 5, AFF_POLYMORPH, iDuration, 0, false);
		}

		item->SetCount(item->GetCount() - 1);
	}
	break;

	case 50322:
	{
		sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

		if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
		{
			CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
		}
		else
		{
		}
	}
	break;

	default:
		sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
		return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)	return false;
	if (GetShop())		return false;
	if (GetMyShop())	return false;
	if (m_bUnderRefine)	return false;
	if (IsWarping())	return false;
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (GetOfflineShop()) return false;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (IsEditingPrivateShop() || IsShopSearch() || GetMyPrivateShop()) return false;
#endif

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM Unique3 = GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	LPITEM MountCostume = GetWear(WEAR_COSTUME_MOUNT);
#endif

	if (NULL != Unique1)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
		{
			return UnequipItem(Unique1);
		}
	}

	if (NULL != Unique2)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
		{
			return UnequipItem(Unique2);
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (NULL != Unique3)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
		{
			return UnequipItem(Unique3);
		}
	}
#endif

#ifdef ENABLE_NEW_MOUNT_SYSTEM
	if (MountCostume)
		return UnequipItem(MountCostume);
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i = 0; i < sizeof(stunSkills) / sizeof(DWORD); ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef ENABLE_NEWSTUFF
				&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
				)
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					// if (avail > amount)
					// {
					// 	const int pct_of_used = amount_of_used * 100 / amount_of_full;
					// 	const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

					// 	bool bLog = false;

					// 	if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
					// 		bLog = true;
					// 	pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
					// }
					// else
					// {
					// 	amount = avail;

					// 	ITEM_MANAGER::instance().RemoveItem(pItem);
					// }

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange(POINT_HP_RECOVERY, amount);
						EffectPacket(SE_AUTO_HPUP);
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange(POINT_SP_RECOVERY, amount);
						EffectPacket(SE_AUTO_SPUP);
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect(const_cast<CAffect*>(pAffect));
			}
		}
		else
		{
			RemoveAffect(const_cast<CAffect*>(pAffect));
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	case BUFF_EQUIPMENT:
		return cell < (BUFF_WINDOW_SLOT_MAX_NUM);
#endif
#ifdef ENABLE_SWITCHBOT
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif
	default:
		return false;
	}
}

#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}

bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
	case JOB_WARRIOR:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
			return false;
		break;

	case JOB_ASSASSIN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
			return false;
		break;

	case JOB_SHAMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
			return false;
		break;

	case JOB_SURA:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
			return false;
		break;
#ifdef ENABLE_WOLFMAN_CHARACTER
	case JOB_WOLFMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
			return false;
		break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (GetLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("·¹º§ÀÌ ³·¾Æ Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			break;

		case LIMIT_STR:
			if (GetPoint(POINT_ST) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±Ù·ÂÀÌ ³·¾Æ Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			break;

		case LIMIT_INT:
			if (GetPoint(POINT_IQ) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Áö´ÉÀÌ ³·¾Æ Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			break;

		case LIMIT_DEX:
			if (GetPoint(POINT_DX) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¹ÎÃ¸ÀÌ ³·¾Æ Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			break;

		case LIMIT_CON:
			if (GetPoint(POINT_HT) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ã¼·ÂÀÌ ³·¾Æ Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return false;
			}
			break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) ||
			(GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item))
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			|| (GetWear(WEAR_COSTUME_MOUNT) && GetWear(WEAR_COSTUME_MOUNT)->IsSameSpecialGroup(item))
#endif
			)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°°Àº Á¾·ùÀÇ À¯´ÏÅ© ¾ÆÀÌÅÛ µÎ °³¸¦ µ¿½Ã¿¡ ÀåÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°áÈ¥ÇÏÁö ¾ÊÀº »óÅÂ¿¡¼­ ¿¹¹°À» Âø¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}

	}

	if (item->GetType() == ITEM_RING)
	{
		if ((GetWear(WEAR_RING1) && GetWear(WEAR_RING1)->IsRingSameGroup(item)) || (GetWear(WEAR_RING1) && GetWear(WEAR_RING1)->IsSameSubType(item)) 
		|| (GetWear(WEAR_RING2) && GetWear(WEAR_RING2)->IsRingSameGroup(item)) || (GetWear(WEAR_RING2) && GetWear(WEAR_RING2)->IsSameSubType(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°°Àº Á¾·ùÀÇ À¯´ÏÅ© ¾ÆÀÌÅÛ µÎ °³¸¦ µ¿½Ã¿¡ ÀåÂøÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return false;
		}
	}

#ifdef ENABLE_NEW_PET_SYSTEM
	// if (item->GetType() == ITEM_NEW_PET && GetWear(WEAR_NEW_PET))
	// {
	// 	return false;
	// }

	if (item->GetType() == ITEM_NEW_PET_EQ)
	{
		// if ((GetWear(WEAR_NEW_PET_EQ0) && GetWear(WEAR_NEW_PET_EQ0)->IsSameSpecialGroup(item)) ||
		// 	(GetWear(WEAR_NEW_PET_EQ1) && GetWear(WEAR_NEW_PET_EQ1)->IsSameSpecialGroup(item)) ||
		// 	(GetWear(WEAR_NEW_PET_EQ2) && GetWear(WEAR_NEW_PET_EQ2)->IsSameSpecialGroup(item)))
		if ((GetWear(WEAR_NEW_PET_EQ0) && GetWear(WEAR_NEW_PET_EQ0)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ1) && GetWear(WEAR_NEW_PET_EQ1)->IsSameSubType(item)) ||
			(GetWear(WEAR_NEW_PET_EQ2) && GetWear(WEAR_NEW_PET_EQ2)->IsSameSubType(item)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do it.."));
			return false;
		}
	}
#endif
	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "ï¿½ï¿½Æ® ï¿½Îºï¿½ï¿½ä¸®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½.");

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		else if (item->IsSkillBook())
			pos = GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			pos = GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			pos = GetEmptyStoneInventory(item->GetSize());
		else if (item->IsBox())
			pos = GetEmptyBoxInventory(item->GetSize());
		else if (item->IsEfsun())
			pos = GetEmptyEfsunInventory(item->GetSize());
		else if (item->IsCicek())
			pos = GetEmptyCicekInventory(item->GetSize());
#endif
		else
			pos = GetEmptyInventory(item->GetSize());

		VERIFY_MSG(-1 == pos, "ï¿½ï¿½ï¿½ï¿½Ç°ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½.");
	}
	
#ifdef __RENEWAL_MOUNT__
	if (item->IsCostumeMountItem())
	{
		if(IsRiding())
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz zdj¹æ wierzchowca bêd¹c na nim.");
			return false;
		}
	}
#endif

	if (item->GetType() == ITEM_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}

	return true;
}

#ifdef ENABLE_EXTENDED_BLEND
int CHARACTER::GetAffectType(LPITEM item)
{
	switch (item->GetValue(1))
	{
	case 1: return AFFECT_BLEND_EX;
	case 2: return AFFECT_WATER;
	case 3: return AFFECT_MALL_EX;
	default: return AFFECT_BLEND;
	}
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
static bool FN_check_item_sex_buff(DWORD dwSex, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE)) {
		if (0 == dwSex)
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) {
		if (1 == dwSex)
			return false;
	}

	return true;
}

static bool FN_check_item_level_buff(DWORD dwLevel, LPITEM item)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (dwLevel < limitValue)
				return false;
			break;
		}
	}
	return true;
}

LPITEM CHARACTER::GetBuffWear(WORD bCell) const
{
	if (bCell >= BUFF_WINDOW_SLOT_MAX_NUM)
	{
		sys_err("CHARACTER::GetBuffWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pBuffEquipmentItem[bCell];
}

bool CHARACTER::IsBuffEquipEmpty()
{
	for (int i = 0; i < 3; ++i) {
		if (m_pointsInstant.pBuffEquipmentItem[i])
			return false;
	}
	return true;
}

bool CHARACTER::CheckBuffEquipmentPositionAvailable(int iWearCell)
{
	if (iWearCell < 0) {
		return false;
	}
	return false;
}

LPITEM CHARACTER::GetBuffEquipmentItem(WORD wCell) const
{
	return GetItem(TItemPos(BUFF_EQUIPMENT, wCell));
}

bool CHARACTER::IsBuffEquipUniqueItem(DWORD dwItemVnum) const
{
	LPITEM item = GetBuffWear(BUFF_WEAR_UNIQUE);

	if (item && item->GetVnum() == dwItemVnum)
		return true;

	return false;
}

bool CHARACTER::EquipBuffItem(BYTE cell, LPITEM item)
{
	if (item->IsExchanging())
		return false;

	int iWearCell = item->FindBuffEquipCell(this);

	if (iWearCell != cell) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_PUT_ITEM_HERE"));
		return false;
	}

	if (!GetBuffNPCSystem()->IsActive()) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_YOU_DONT_HAVE_ACTIVE_SHAMAN"));
		return false;
	}

	if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_SHAMAN"));
		return false;
	}

	if (false == FN_check_item_sex_buff(GetBuffNPCSystem()->GetSex(), item)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_THIS_ITEM_IS_NOT_FOR_YOU_GENDER"));
		return false;
	}

	if (GetBuffNPCSystem() != NULL) {
		if (GetBuffNPCSystem()->IsSummoned()) {
			item->StartUniqueExpireEvent();
		}
	}

	return true;
}

bool CHARACTER::UnequipBuffItem(BYTE cell, LPITEM item)
{
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ASLAN_BUFF_NPC_CANNOT_REMOVE_ITEM_FROM_SLOT"));
		return false;
	}

	item->StopUniqueExpireEvent();

	return true;
}
#endif

#ifdef ENABLE_INSTANT_INGOT_OPEN
bool CHARACTER::OpenIngotItem(TItemPos pos, int bOpenCount)
{
	LPITEM item;
	if (!CanHandleItem())
		return false;
	
	if (!IsValidItemPosition(pos) || !(item = GetItem(pos)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (IsStun())
		return false;
	
	return OpenIngotItem(item, bOpenCount);
}

bool CHARACTER::OpenIngotItem(LPITEM item, int bOpenCount)
{
	if (!item)
		return false;

	if (!CanHandleItem())
		return false;

	if (item->IsExchanging())
		return false;

	if (IsStun())
		return false;

	if (item->isLocked())
		return false;

	int itemCount = item->GetCount();
	long long loopCount = MINMAX(1, MIN(bOpenCount, itemCount), 500);
	DWORD itemVnum = item->GetVnum();

	if (loopCount > itemCount || loopCount < 1) {
		ChatPacket(CHAT_TYPE_INFO, "invalid loopCount: %d (itemCount: %d)", loopCount, itemCount);
		sys_err("invalid loopCount: %d (itemCount: %d)", loopCount, itemCount);
		return false;
	}
	long long goldToAdd = 0;

	switch (itemVnum)
	{
		case 80005:
			goldToAdd = loopCount * 500000;
			break;

		case 80006:
			goldToAdd = loopCount * 1000000;
			break;

		case 80007:
			goldToAdd = loopCount * 2000000;
			break;

		case 80008:
			goldToAdd = loopCount * 10000000;
			break;

		case 80009:
			goldToAdd = loopCount * 5000000;
			break;

		default:
			ChatPacket(CHAT_TYPE_INFO, "error: case = default");
			return false;
	}
	item->Lock(true);
	ChatPacket(CHAT_TYPE_INFO, "%s - x%d", item->GetName(), loopCount);
	ChatPacket(CHAT_TYPE_INFO, "Otrzyma³eœ: %lld Yang", goldToAdd);
	item->SetCount(itemCount - loopCount);
	PointChange(POINT_GOLD, goldToAdd);
	item->Lock(false);

	return true;
}
#endif

#ifdef __ENABLE_TOGGLE_ITEMS__
CItem* CHARACTER::FindActiveToggleItem(int32_t toggleType, CItem* pExcept /* = nullptr */)
{
	if (!GetDesc())
		return nullptr;

	for (int32_t i = 0; i < INVENTORY_MAX_NUM; i++)
	{
		CItem* pItem = GetInventoryItem(i);
		if (!pItem)
			continue;

		if (pItem == pExcept)
			continue;

		if (!pItem->IsToggleItem())
			continue;

		if (!pItem->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
			continue;

		if (pItem->GetValue(TOGGLE_TYPE_VALUE) != toggleType)
			continue;

		return pItem;
	}

	return nullptr;
}
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
