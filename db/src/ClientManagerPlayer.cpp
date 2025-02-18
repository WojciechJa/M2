#include "stdafx.h"

#include "ClientManager.h"

#include "Main.h"
#include "QID.h"
#include "ItemAwardManager.h"
#include "HB.h"
#include "Cache.h"
#include "../../common/length.h"
#include "../../common/CommonDefines.h"
extern bool g_bHotBackup;

extern std::string g_stLocale;
extern int g_test_server;
extern int g_log;

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "PrivateShopUtils.h"
#endif


//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!! IMPORTANT !!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Check all SELECT syntax on item table before change this function!!!
//
bool CreateItemTableFromRes(MYSQL_RES * res, std::vector<TPlayerItem> * pVec, DWORD dwPID)
{
	if (!res)
	{
		pVec->clear();
		return true;
	}

	int rows;

	if ((rows = mysql_num_rows(res)) <= 0)
	{
		pVec->clear();
		return true;
	}

	pVec->resize(rows);

	for (int i = 0; i < rows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		TPlayerItem & item = pVec->at(i);

		int cur = 0;

		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		str_to_number(item.id, row[cur++]);
		str_to_number(item.window, row[cur++]);
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);
		str_to_number(item.alSockets[0], row[cur++]);
		str_to_number(item.alSockets[1], row[cur++]);
		str_to_number(item.alSockets[2], row[cur++]);
#ifdef ENABLE_EXTENDED_SOCKETS
		str_to_number(item.alSockets[3], row[cur++]);
		str_to_number(item.alSockets[4], row[cur++]);
		str_to_number(item.alSockets[5], row[cur++]);		
#endif
#if defined(__ITEM_APPLY_RANDOM__)
		for (int j = 0; j < ITEM_APPLY_MAX_NUM; j++)
		{
			str_to_number(item.aApplyRandom[j].bType, row[cur++]);
			str_to_number(item.aApplyRandom[j].sValue, row[cur++]);
			str_to_number(item.aApplyRandom[j].bPath, row[cur++]);
		}
#endif
		for (int j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
		{
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}

		item.owner		= dwPID;
	}

	return true;
}

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab)
{
	size_t queryLen;

	queryLen = snprintf(pszQuery, querySize,
			"UPDATE player%s SET "
			"job = %d, "
			"voice = %d, "
			"dir = %d, "
			"x = %d, "
			"y = %d, "
			"z = %d, "
			"map_index = %d, "
			"exit_x = %ld, "
			"exit_y = %ld, "
			"exit_map_index = %ld, "
			"hp = %d, "
			"mp = %d, "
			"stamina = %d, "
			"random_hp = %d, "
			"random_sp = %d, "
			"playtime = %d, "
			"level = %d, "
			"level_step = %d, "
			"st = %d, "
			"ht = %d, "
			"dx = %d, "
			"iq = %d, "
#ifdef ENABLE_LONG_LONG
			"gold = %lld, "
#else
			"gold = %d, "
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
			"cheque = %lld, "
#endif
			"exp = %u, "
			"stat_point = %d, "
			"skill_point = %d, "
			"sub_skill_point = %d, "
			"stat_reset_count = %d, "
			"ip = '%s', "
			"part_main = %d, "
			"part_hair = %d, "
			#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"part_acce = %d, "
			#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
			"part_emblemat = %d, "
#endif
			"last_play = NOW(), "
			"skill_group = %d, "
			"alignment = %ld, "
			"horse_level = %d, "
			"horse_riding = %d, "
			"horse_hp = %d, "
			"horse_hp_droptime = %u, "
			"horse_stamina = %d, "
			"horse_skill_point = %d, "
#ifdef WEEKLY_RANK_BYLUZER
			"w_monsters = %d, "
			"w_bosses = %d, "
			"w_metins = %d, "
			"w_dungs = %d, "
			"w_ds = %d, "
			"w_refine = %d, "
			"w_sm = %d, "
			"weekly_season = %d, "
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
			"pkt_osiag = %d, "
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
			"private_shop_unlocked_slot = %u, "
#endif
			,
		GetTablePostfix(),
		pkTab->job,
		pkTab->voice,
		pkTab->dir,
		pkTab->x,
		pkTab->y,
		pkTab->z,
		pkTab->lMapIndex,
		pkTab->lExitX,
		pkTab->lExitY,
		pkTab->lExitMapIndex,
		pkTab->hp,
		pkTab->sp,
		pkTab->stamina,
		pkTab->sRandomHP,
		pkTab->sRandomSP,
		pkTab->playtime,
		pkTab->level,
		pkTab->level_step,
		pkTab->st,
		pkTab->ht,
		pkTab->dx,
		pkTab->iq,
		pkTab->gold,
#ifdef ENABLE_CHEQUE_SYSTEM
		pkTab->cheque,
#endif
		pkTab->exp,
		pkTab->stat_point,
		pkTab->skill_point,
		pkTab->sub_skill_point,
		pkTab->stat_reset_count,
		pkTab->ip,
		pkTab->parts[PART_MAIN],
		pkTab->parts[PART_HAIR],
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		pkTab->parts[PART_ACCE],
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
		pkTab->parts[PART_EMBLEMAT],
#endif
		pkTab->skill_group,
		pkTab->lAlignment,
		pkTab->horse.bLevel,
		pkTab->horse.bRiding,
		pkTab->horse.sHealth,
		pkTab->horse.dwHorseHealthDropTime,
		pkTab->horse.sStamina,
		pkTab->horse_skill_point
#ifdef WEEKLY_RANK_BYLUZER
		, pkTab->w_monsters
		, pkTab->w_bosses
		, pkTab->w_metins
		, pkTab->w_dungs
		, pkTab->w_ds
		, pkTab->w_refine
		, pkTab->w_sm
		, pkTab->weekly_season
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		, pkTab->pkt_osiag
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
		, pkTab->wPrivateShopUnlockedSlot
#endif
		);

	static char text[8192 + 1];

	CDBManager::instance().EscapeString(text, pkTab->skills, sizeof(pkTab->skills));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "skill_level = '%s', ", text);

	CDBManager::instance().EscapeString(text, pkTab->quickslot, sizeof(pkTab->quickslot));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "quickslot = '%s' ", text);

#ifdef TITLE_SYSTEM_BYLUZER
	CDBManager::instance().EscapeString(text, pkTab->titles, sizeof(pkTab->titles));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, ", titles_active = '%s' ", text);
#endif

	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, " WHERE id=%d", pkTab->id);
	return queryLen;
}

CPlayerTableCache * CClientManager::GetPlayerCache(DWORD id)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(id);

	if (it == m_map_playerCache.end())
		return NULL;

	TPlayerTable* pTable = it->second->Get(false);
	pTable->logoff_interval = GetCurrentTime() - it->second->GetLastUpdateTime();
	return it->second;
}

void CClientManager::PutPlayerCache(TPlayerTable * pNew)
{
	CPlayerTableCache * c;

	c = GetPlayerCache(pNew->id);

	if (!c)
	{
		c = new CPlayerTableCache;
		m_map_playerCache.emplace(pNew->id, c);
	}

	if (g_bHotBackup)
		PlayerHB::instance().Put(pNew->id);

	c->Put(pNew);
}

/*
 * PLAYER LOAD
 */
void CClientManager::QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket * packet)
{
	CPlayerTableCache * c;
	TPlayerTable * pTab;

	CLoginData * pLoginData = GetLoginDataByAID(packet->account_id);

	if (pLoginData)
	{
#ifdef __PREMIUM_PRIVATE_SHOP__
		pLoginData->SetLastPlayerHandle(dwHandle);
#endif

		for (int n = 0; n < PLAYER_PER_ACCOUNT; ++n)
			if (pLoginData->GetAccountRef().players[n].dwID != 0)
				DeleteLogoutPlayer(pLoginData->GetAccountRef().players[n].dwID);
	}
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	CLocationSystemCache* locationSystemCache = GetLocationSystemCache(packet->player_id);
	
	if (!locationSystemCache)
	{
		for (int n = 0; n < PLAYER_PER_ACCOUNT; n++)
		{
			if (pLoginData->GetAccountRef().players[n].dwID == packet->player_id)
				CreateLocationSystemCache(packet->player_id);
		}
		
		UpdateLocationSystemCache(packet->player_id);
	}
#endif
#ifdef ENABLE_LRN_BATTLE_PASS
	char queryBattlePassCheck[QUERY_MAX_LEN];
	
	snprintf(queryBattlePassCheck, sizeof(queryBattlePassCheck), "SELECT * FROM player_battle_pass%s WHERE pid = %d", GetTablePostfix(), packet->player_id);
	auto pMsgBattlePassCheck = CDBManager::instance().DirectQuery(queryBattlePassCheck);
	
	if (pMsgBattlePassCheck->Get()->uiNumRows == 0)
	{
		char queryBattlePassAdd[QUERY_MAX_LEN];
		
		snprintf(queryBattlePassAdd, sizeof(queryBattlePassAdd), "INSERT INTO player_battle_pass%s (pid, level, exp, premium) VALUES (%d, %d, %d, %d)", GetTablePostfix(), packet->player_id, 1, 0, 0);
		std::unique_ptr<SQLMsg> pBattlePassAdd(CDBManager::instance().DirectQuery(queryBattlePassAdd));
	}
#endif
	//----------------------------------------------------------------

	// ---------------------------------------------------------------

	//----------------------------------

	//----------------------------------
	if ((c = GetPlayerCache(packet->player_id)))
	{
		CLoginData * pkLD = GetLoginDataByAID(packet->account_id);

		if (!pkLD || pkLD->IsPlay())
		{
			sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
			peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, dwHandle, 0);
			return;
		}

		pTab = c->Get();

		pkLD->SetPlay(true);
		SendLoginToBilling(pkLD, true);
		thecore_memcpy(pTab->aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(pTab->aiPremiumTimes));

		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, dwHandle, sizeof(TPlayerTable));
		peer->Encode(pTab, sizeof(TPlayerTable));

		if (packet->player_id != pkLD->GetLastPlayerID())
		{
			TPacketNeedLoginLogInfo logInfo;
			logInfo.dwPlayerID = packet->player_id;

			pkLD->SetLastPlayerID( packet->player_id );

			peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, dwHandle, sizeof(TPacketNeedLoginLogInfo) );
			peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
		}

		char szQuery[1024] = { 0, };

		TItemCacheSet * pSet = GetItemCacheSet(pTab->id);

#ifdef ENABLE_LONG_LONG
	#ifdef ENABLE_CHEQUE_SYSTEM
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %lld cheque %d ", pTab->name, pTab->id, pTab->gold, pTab->cheque);
	#else
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %lld ", pTab->name, pTab->id, pTab->gold);
	#endif
#else
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %d ", pTab->name, pTab->id, pTab->gold);
#endif

		//--------------------------------------------

		//--------------------------------------------

		/////////////////////////////////////////////

		/////////////////////////////////////////////
		if (pSet)
		{
			static std::vector<TPlayerItem> s_items;
			s_items.resize(pSet->size());

			DWORD dwCount = 0;
			TItemCacheSet::iterator it = pSet->begin();

			while (it != pSet->end())
			{
				CItemCache * c = *it++;
				TPlayerItem * p = c->Get();

				if (p->vnum)
					thecore_memcpy(&s_items[dwCount++], p, sizeof(TPlayerItem));
			}

			if (g_test_server)
				sys_log(0, "ITEM_CACHE: HIT! %s count: %u", pTab->name, dwCount);

			peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
			peer->EncodeDWORD(dwCount);

			if (dwCount)
				peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

			// Quest
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d AND lValue<>0",
					GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle,0,packet->account_id));

			// Affect
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d",
					GetTablePostfix(), pTab->id);
			// @fixme402 ClientHandleInfo+pTab->id
			CDBManager::instance().ReturnQuery(szQuery, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
		
#ifdef ENABLE_LRN_BATTLE_PASS
			// BattlePass
			snprintf(szQuery, sizeof(szQuery),
				"SELECT pid, qidx, count, status FROM quest_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), pTab->id);
				
			CDBManager::instance().ReturnQuery(szQuery, QID_BATTLE_PASS_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, 0, packet->account_id));
			
			snprintf(szQuery, sizeof(szQuery),
				"SELECT pid, level, status_free, status_premium FROM reward_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), pTab->id);
				
			CDBManager::instance().ReturnQuery(szQuery, QID_BATTLE_PASS_REWARD, peer->GetHandle(), new ClientHandleInfo(dwHandle, 0, packet->account_id));
			
			snprintf(szQuery, sizeof(szQuery),
				"SELECT pid, level, exp, premium FROM player_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), pTab->id);
				
			CDBManager::instance().ReturnQuery(szQuery, QID_BATTLE_PASS_INFO, peer->GetHandle(), new ClientHandleInfo(dwHandle, 0, packet->account_id));
#endif
		}
		/////////////////////////////////////////////

		/////////////////////////////////////////////
		else
		{
			snprintf(szQuery, sizeof(szQuery), 
				"SELECT "
				"id,"
				"window+0,"
				"pos,"
				"count,"
				"vnum,"
				"socket0,socket1,socket2,"
#ifdef ENABLE_EXTENDED_SOCKETS
				"socket3,socket4,socket5,"
#endif
#if defined(__ITEM_APPLY_RANDOM__)
				"apply_type0, apply_value0, apply_path0,"
				"apply_type1, apply_value1, apply_path1,"
				"apply_type2, apply_value2, apply_path2,"
				"apply_type3, apply_value3, apply_path3,"
#endif
				"attrtype0,attrvalue0,"
				"attrtype1,attrvalue1,"
				"attrtype2,attrvalue2,"
				"attrtype3,attrvalue3,"
				"attrtype4,attrvalue4,"
				"attrtype5,attrvalue5,"
				"attrtype6,attrvalue6 "
				"FROM item%s "
#ifdef ENABLE_SWITCHBOT
				// "WHERE owner_id=%d AND (window < %d or window = %d or window = %d) ",
				// aslan buff
				"WHERE owner_id=%d AND (window < %d or window = %d or window = %d or window = %d) ",
#else
				"WHERE owner_id=%d AND (window < %d or window = %d) ",
#endif
				GetTablePostfix(), 
				pTab->id, 
				SAFEBOX, 
				DRAGON_SOUL_INVENTORY,
				BUFF_EQUIPMENT,
#ifdef ENABLE_SWITCHBOT
				SWITCHBOT
#endif
			);

			CDBManager::instance().ReturnQuery(szQuery,
					QID_ITEM,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID, szName, szState, lValue FROM quest%s WHERE dwPID=%d",
					GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery,
					QID_QUEST,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost FROM affect%s WHERE dwPID=%d",
					GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery,
					QID_AFFECT,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));

#ifdef ENABLE_LRN_BATTLE_PASS
			CDBManager::instance().ReturnQuery(szQuery,
					QID_BATTLE_PASS_QUEST,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));
					
			CDBManager::instance().ReturnQuery(szQuery,
					QID_BATTLE_PASS_REWARD,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));
					
			CDBManager::instance().ReturnQuery(szQuery,
					QID_BATTLE_PASS_INFO,
					peer->GetHandle(),
					new ClientHandleInfo(dwHandle, pTab->id));
#endif
		}
		//ljw
		//return;
	}
	//----------------------------------

	//----------------------------------
	else
	{
		sys_log(0, "[PLAYER_LOAD] Load from PlayerDB pid[%d]", packet->player_id);

		char queryStr[QUERY_MAX_LEN];

		//--------------------------------------------------------------

		//--------------------------------------------------------------
		snprintf(queryStr, sizeof(queryStr),
				"SELECT "
				"id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
#ifdef ENABLE_CHEQUE_SYSTEM
				"gold,cheque,level,level_step,st,ht,dx,iq,exp,"
#else
				"gold,level,level_step,st,ht,dx,iq,exp,"
#endif
				"stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,"
				#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				"part_acce, "
				#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
				"part_emblemat, "
#endif
				"skill_level,quickslot,skill_group,alignment,mobile,horse_level,horse_riding,horse_hp,horse_hp_droptime,horse_stamina,"
				"UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(last_play),horse_skill_point"
#ifdef WEEKLY_RANK_BYLUZER
				",w_monsters,w_bosses,w_metins,w_dungs,w_ds,w_refine,w_sm,weekly_season"
#endif
#ifdef TITLE_SYSTEM_BYLUZER
				",titles_active"
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
				", pkt_osiag "
#endif
			#ifdef __PREMIUM_PRIVATE_SHOP__
				", private_shop_unlocked_slot "
#endif
				" FROM player%s WHERE id=%d",
				GetTablePostfix(), packet->player_id);

		ClientHandleInfo * pkInfo = new ClientHandleInfo(dwHandle, packet->player_id);
		pkInfo->account_id = packet->account_id;
		CDBManager::instance().ReturnQuery(queryStr, QID_PLAYER, peer->GetHandle(), pkInfo);

		snprintf(queryStr, sizeof(queryStr),
			"SELECT "
			"id,"
			"window+0,"
			"pos,"
			"count,"
			"vnum,"
			"socket0,socket1,socket2,"
#ifdef ENABLE_EXTENDED_SOCKETS
			"socket3,socket4,socket5,"
#endif
#if defined(__ITEM_APPLY_RANDOM__)
			"apply_type0, apply_value0, apply_path0,"
			"apply_type1, apply_value1, apply_path1,"
			"apply_type2, apply_value2, apply_path2,"
			"apply_type3, apply_value3, apply_path3,"
#endif
			"attrtype0,attrvalue0,"
			"attrtype1,attrvalue1,"
			"attrtype2,attrvalue2,"
			"attrtype3,attrvalue3,"
			"attrtype4,attrvalue4,"
			"attrtype5,attrvalue5,"
			"attrtype6,attrvalue6 "
			"FROM item%s "
#ifdef ENABLE_SWITCHBOT
			// "WHERE owner_id=%d AND (window < %d or window = %d or window = %d) ",
			// aslanbuff
			"WHERE owner_id=%d AND (window < %d or window = %d or window = %d or window = %d) ",
#else
			"WHERE owner_id=%d AND (window < %d or window = %d) ",
#endif
			GetTablePostfix(),
			packet->player_id,
			SAFEBOX,
			DRAGON_SOUL_INVENTORY,
			BUFF_EQUIPMENT,
#ifdef ENABLE_SWITCHBOT
			SWITCHBOT
#endif
		);
		CDBManager::instance().ReturnQuery(queryStr, QID_ITEM, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));

		//--------------------------------------------------------------

		//--------------------------------------------------------------
		snprintf(queryStr, sizeof(queryStr),
				"SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d",
				GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id,packet->account_id));

		//--------------------------------------------------------------

		//--------------------------------------------------------------
		snprintf(queryStr, sizeof(queryStr),
				"SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d",
				GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	
#ifdef ENABLE_LRN_BATTLE_PASS
		//--------------------------------------------------------------
		// BATTLE_PASS
		//--------------------------------------------------------------
		snprintf(queryStr, sizeof(queryStr),
				"SELECT pid, qidx, count, status FROM quest_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), packet->player_id);
		
		CDBManager::instance().ReturnQuery(queryStr, QID_BATTLE_PASS_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id, packet->account_id));
		
		snprintf(queryStr, sizeof(queryStr),
				"SELECT pid, level, status_free, status_premium FROM reward_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), packet->player_id);
		
		CDBManager::instance().ReturnQuery(queryStr, QID_BATTLE_PASS_REWARD, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id, packet->account_id));
		
		snprintf(queryStr, sizeof(queryStr),
				"SELECT pid, level, exp, premium FROM player_battle_pass%s WHERE pid = %d",
				GetTablePostfix(), packet->player_id);
		
		CDBManager::instance().ReturnQuery(queryStr, QID_BATTLE_PASS_INFO, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id, packet->account_id));
#endif
	}

#ifdef __PREMIUM_PRIVATE_SHOP__
	LPPRIVATE_SHOP pPrivateShop = GetPrivateShop(packet->player_id);
	if (pPrivateShop)
	{
		// Update peer of the owner & end despawn event in case it was running
		pPrivateShop->BindOwnerPeerHandle(peer->GetHandle());
		pPrivateShop->SetOwnerHandle(dwHandle);

#ifdef ENABLE_PRIVATE_SHOP_PREMIUM_TIME
		PrivateShopEndPremiumEvent(packet->player_id);
#endif
	}

	if (CPrivateShopCache* c = GetPrivateShopCache(packet->player_id))
	{
		TPrivateShop* pTab = c->Get();

		// If dwVnum does not exist, it means it has been marked for delete
		if (pTab->dwVnum)
		{
			// Send private shop table to the owner
			BYTE bSubHeader = PRIVATE_SHOP_DG_SUBHEADER_LOAD;
			peer->EncodeHeader(HEADER_DG_PRIVATE_SHOP, dwHandle, sizeof(BYTE) + sizeof(TPrivateShop));
			peer->Encode(&bSubHeader, sizeof(BYTE));
			peer->Encode(pTab, sizeof(TPrivateShop));

			// Item Cache
			static std::vector<TPlayerPrivateShopItem> s_vec_privateShopItem;
			WORD wCount = 0;

			TPrivateShopItemCacheSet* pItemSet = GetPrivateShopItemCacheSet(packet->player_id);
			if (pItemSet)
			{
				s_vec_privateShopItem.resize(pItemSet->size());
				auto it = pItemSet->begin();

				while (it != pItemSet->end())
				{
					CPrivateShopItemCache* c = *it++;
					TPlayerPrivateShopItem* p = c->Get();

					if (p->dwVnum)
						thecore_memcpy(&s_vec_privateShopItem[wCount++], p, sizeof(TPlayerPrivateShopItem));
					else
						s_vec_privateShopItem.pop_back();
				}

				BYTE bSubHeader = PRIVATE_SHOP_DG_SUBHEADER_ITEM_LOAD;
				peer->EncodeHeader(HEADER_DG_PRIVATE_SHOP, dwHandle, sizeof(BYTE) + sizeof(WORD) + sizeof(TPlayerPrivateShopItem) * wCount);
				peer->EncodeBYTE(bSubHeader);
				peer->EncodeWORD(wCount);

				if (wCount)
					peer->Encode(&s_vec_privateShopItem[0], sizeof(TPlayerPrivateShopItem) * wCount);
			}

			// Sale Cache
			static std::vector<TPrivateShopSale> s_vec_privateShopSale;
			wCount = 0;

			TPrivateShopSaleCacheSet* pSaleSet = GetPrivateShopSaleCacheSet(packet->player_id);
			if (pSaleSet)
			{
				s_vec_privateShopSale.resize(pSaleSet->size());
				auto it = pSaleSet->begin();

				while (it != pSaleSet->end())
				{
					CPrivateShopSaleCache* c = *it++;
					TPrivateShopSale* p = c->Get();

					if (p->dwID)
						thecore_memcpy(&s_vec_privateShopSale[wCount++], p, sizeof(TPrivateShopSale));
				}

				if (wCount > 250)
					wCount = 250;

				// Order by time to show the most recent sales
				std::sort(s_vec_privateShopSale.begin(), s_vec_privateShopSale.end(), [](const TPrivateShopSale& a, const TPrivateShopSale& b) { return a.tTime > b.tTime; });

				BYTE bSubHeader = PRIVATE_SHOP_DG_SUBHEADER_SALE_LOAD;
				peer->EncodeHeader(HEADER_DG_PRIVATE_SHOP, dwHandle, sizeof(BYTE) + sizeof(WORD) + sizeof(TPrivateShopSale) * wCount);
				peer->EncodeBYTE(bSubHeader);
				peer->EncodeWORD(wCount);

				if (wCount)
					peer->Encode(&s_vec_privateShopSale[0], sizeof(TPrivateShopSale) * wCount);
			}

			if (!pPrivateShop || (pPrivateShop && pPrivateShop->GetState() == STATE_UNAVAILABLE))
			{
				pPrivateShop = PrivateShopCreate(pTab, s_vec_privateShopItem);

				if (pPrivateShop)
				{
					pPrivateShop->BindOwnerPeerHandle(peer->GetHandle());
					pPrivateShop->SetOwnerHandle(dwHandle);

					PrivateShopSpawn(pTab->dwOwner);
				}
				else
				{
					sys_err("Cannot spawn private shop %u", packet->player_id);
				}
			}
		}
	}
	else
	{
		CDBManager::instance().ReturnQuery(GetPrivateShopItemQuery(packet->player_id),
			QID_PRIVATE_SHOP_ITEM,
			peer->GetHandle(),
			new ClientHandleInfo(dwHandle, packet->player_id));

		CDBManager::instance().ReturnQuery(GetPrivateShopItemSaleQuery(packet->player_id),
			QID_PRIVATE_SHOP_SALE,
			peer->GetHandle(),
			new ClientHandleInfo(dwHandle, packet->player_id));

		CDBManager::instance().ReturnQuery(GetPrivateShopQuery(packet->player_id),
			QID_PRIVATE_SHOP,
			peer->GetHandle(),
			new ClientHandleInfo(dwHandle, packet->player_id));
	}
#endif
}

void CClientManager::ItemAward(CPeer * peer,char* login)
{
	char login_t[LOGIN_MAX_LEN + 1] = "";
	strlcpy(login_t,login,LOGIN_MAX_LEN + 1);
	std::set<TItemAward *> * pSet = ItemAwardManager::instance().GetByLogin(login_t);
	if(pSet == NULL)
		return;
	typeof(pSet->begin()) it = pSet->begin();
	while(it != pSet->end() )
	{
		TItemAward * pItemAward = *(it++);
		char* whyStr = pItemAward->szWhy;
		char cmdStr[100] = "";
		strcpy(cmdStr,whyStr);
		char command[20] = "";
		// @fixme203 directly GetCommand instead of strcpy
		GetCommand(cmdStr, command);
		if( !(strcmp(command,"GIFT") ))
		{
			TPacketItemAwardInfromer giftData;
			strcpy(giftData.login, pItemAward->szLogin);
			strcpy(giftData.command, command);
			giftData.vnum = pItemAward->dwVnum;
			ForwardPacket(HEADER_DG_ITEMAWARD_INFORMER,&giftData,sizeof(TPacketItemAwardInfromer));
		}
	}
}
char* CClientManager::GetCommand(char* str, char* command) // @fixme203
{
	char* tok;

	if( str[0] == '[' )
	{
		tok = strtok(str,"]");
		strcat(command,&tok[1]);
	}

	return command;
}

bool CreatePlayerTableFromRes(MYSQL_RES * res, TPlayerTable * pkTab)
{
	if (mysql_num_rows(res) == 0)
		return false;

	memset(pkTab, 0, sizeof(TPlayerTable));

	MYSQL_ROW row = mysql_fetch_row(res);

	int	col = 0;

	// "id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
	// "gold,level,level_step,st,ht,dx,iq,exp,"
	// "stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,"
	// "skill_level,quickslot,skill_group,alignment,mobile,horse_level,horse_riding,horse_hp,horse_stamina FROM player%s WHERE id=%d",
	str_to_number(pkTab->id, row[col++]);
	strlcpy(pkTab->name, row[col++], sizeof(pkTab->name));
	str_to_number(pkTab->job, row[col++]);
	str_to_number(pkTab->voice, row[col++]);
	str_to_number(pkTab->dir, row[col++]);
	str_to_number(pkTab->x, row[col++]);
	str_to_number(pkTab->y, row[col++]);
	str_to_number(pkTab->z, row[col++]);
	str_to_number(pkTab->lMapIndex, row[col++]);
	str_to_number(pkTab->lExitX, row[col++]);
	str_to_number(pkTab->lExitY, row[col++]);
	str_to_number(pkTab->lExitMapIndex,  row[col++]);
	str_to_number(pkTab->hp, row[col++]);
	str_to_number(pkTab->sp, row[col++]);
	str_to_number(pkTab->stamina, row[col++]);
	str_to_number(pkTab->sRandomHP, row[col++]);
	str_to_number(pkTab->sRandomSP, row[col++]);
	str_to_number(pkTab->playtime, row[col++]);
	str_to_number(pkTab->gold, row[col++]);
#ifdef ENABLE_CHEQUE_SYSTEM
	str_to_number(pkTab->cheque, row[col++]);
#endif
	str_to_number(pkTab->level, row[col++]);
	str_to_number(pkTab->level_step, row[col++]);
	str_to_number(pkTab->st, row[col++]);
	str_to_number(pkTab->ht, row[col++]);
	str_to_number(pkTab->dx, row[col++]);
	str_to_number(pkTab->iq, row[col++]);
	str_to_number(pkTab->exp, row[col++]);
	str_to_number(pkTab->stat_point, row[col++]);
	str_to_number(pkTab->skill_point, row[col++]);
	str_to_number(pkTab->sub_skill_point, row[col++]);
	str_to_number(pkTab->stat_reset_count, row[col++]);
	str_to_number(pkTab->part_base, row[col++]);
	str_to_number(pkTab->parts[PART_HAIR], row[col++]);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	str_to_number(pkTab->parts[PART_ACCE], row[col++]);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	str_to_number(pkTab->parts[PART_EMBLEMAT], row[col++]);
#endif
	if (row[col])
		thecore_memcpy(pkTab->skills, row[col], sizeof(pkTab->skills));
	else
		memset(&pkTab->skills, 0, sizeof(pkTab->skills));

	col++;

	if (row[col])
		thecore_memcpy(pkTab->quickslot, row[col], sizeof(pkTab->quickslot));
	else
		memset(pkTab->quickslot, 0, sizeof(pkTab->quickslot));

	col++;

	str_to_number(pkTab->skill_group, row[col++]);
	str_to_number(pkTab->lAlignment, row[col++]);

	if (row[col])
	{
		strlcpy(pkTab->szMobile, row[col], sizeof(pkTab->szMobile));
	}

	col++;

	str_to_number(pkTab->horse.bLevel, row[col++]);
	str_to_number(pkTab->horse.bRiding, row[col++]);
	str_to_number(pkTab->horse.sHealth, row[col++]);
	str_to_number(pkTab->horse.dwHorseHealthDropTime, row[col++]);
	str_to_number(pkTab->horse.sStamina, row[col++]);
	str_to_number(pkTab->logoff_interval, row[col++]);
	str_to_number(pkTab->horse_skill_point, row[col++]);
#ifdef WEEKLY_RANK_BYLUZER
	str_to_number(pkTab->w_monsters, row[col++]);
	str_to_number(pkTab->w_bosses, row[col++]);
	str_to_number(pkTab->w_metins, row[col++]);
	str_to_number(pkTab->w_dungs, row[col++]);
	str_to_number(pkTab->w_ds, row[col++]);
	str_to_number(pkTab->w_refine, row[col++]);
	str_to_number(pkTab->w_sm, row[col++]);
	str_to_number(pkTab->weekly_season, row[col++]);
#endif

#ifdef TITLE_SYSTEM_BYLUZER
	if (row[col])
		thecore_memcpy(pkTab->titles, row[col], sizeof(pkTab->titles));
	else
		memset(&pkTab->titles, 0, sizeof(pkTab->titles));
	col++;
#endif

#ifdef ENABLE_PUNKTY_OSIAGNIEC
	str_to_number(pkTab->pkt_osiag, row[col++]);
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	str_to_number(pkTab->wPrivateShopUnlockedSlot, row[col++]);
#endif
	// reset sub_skill_point
	{
		pkTab->skills[123].bLevel = 0; // SKILL_CREATE

		if (pkTab->level > 9)
		{
			int max_point = pkTab->level - 9;

			int skill_point =
				MIN(20, pkTab->skills[121].bLevel) +
				MIN(20, pkTab->skills[124].bLevel) +
				MIN(10, pkTab->skills[131].bLevel) +
				MIN(20, pkTab->skills[141].bLevel) +
				MIN(20, pkTab->skills[142].bLevel);

			pkTab->sub_skill_point = max_point - skill_point;
		}
		else
			pkTab->sub_skill_point = 0;
	}

	return true;
}

void CClientManager::RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID)
{
	CQueryInfo * qi = (CQueryInfo *) pMsg->pvUserData;
	std::unique_ptr<ClientHandleInfo> info((ClientHandleInfo *) qi->pvData);

	MYSQL_RES * pSQLResult = pMsg->Get()->pSQLResult;
	if (!pSQLResult)
	{
		sys_err("null MYSQL_RES QID %u", dwQID);
		return;
	}

	switch (dwQID)
	{
		case QID_PLAYER:
			sys_log(0, "QID_PLAYER %u %u", info->dwHandle, info->player_id);
			RESULT_PLAYER_LOAD(peer, pSQLResult, info.get());

			break;

		case QID_ITEM:
			sys_log(0, "QID_ITEM %u", info->dwHandle);
			RESULT_ITEM_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;

		case QID_QUEST:
			{
				sys_log(0, "QID_QUEST %u", info->dwHandle);
				RESULT_QUEST_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);

				ClientHandleInfo*  temp1 = info.get();
				if (temp1 == NULL)
					break;

				CLoginData* pLoginData1 = GetLoginDataByAID(temp1->account_id);

				if( pLoginData1->GetAccountRef().login == NULL)
					break;
				if( pLoginData1 == NULL )
					break;
				sys_log(0,"info of pLoginData1 before call ItemAwardfunction %d",pLoginData1);
				ItemAward(peer,pLoginData1->GetAccountRef().login);
			}
			break;

		case QID_AFFECT:
			sys_log(0, "QID_AFFECT %u", info->dwHandle);
			// @fixme402 RESULT_AFFECT_LOAD+info->player_id
			RESULT_AFFECT_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;
			/*
			   case QID_PLAYER_ITEM_QUEST_AFFECT:
			   sys_log(0, "QID_PLAYER_ITEM_QUEST_AFFECT %u", info->dwHandle);
			   RESULT_PLAYER_LOAD(peer, pSQLResult, info->dwHandle);

			   if (!pMsg->Next())
			   {
			   sys_err("RESULT_COMPOSITE_PLAYER: QID_PLAYER_ITEM_QUEST_AFFECT: ITEM FAILED");
			   return;
			   }

			   case QID_ITEM_QUEST_AFFECT:
			   sys_log(0, "QID_ITEM_QUEST_AFFECT %u", info->dwHandle);
			   RESULT_ITEM_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);

			   if (!pMsg->Next())
			   {
			   sys_err("RESULT_COMPOSITE_PLAYER: QID_PLAYER_ITEM_QUEST_AFFECT: QUEST FAILED");
			   return;
			   }

			   case QID_QUEST_AFFECT:
			   sys_log(0, "QID_QUEST_AFFECT %u", info->dwHandle);
			   RESULT_QUEST_LOAD(peer, pSQLResult, info->dwHandle);

			   if (!pMsg->Next())
			   sys_err("RESULT_COMPOSITE_PLAYER: QID_PLAYER_ITEM_QUEST_AFFECT: AFFECT FAILED");
			   else
			   RESULT_AFFECT_LOAD(peer, pSQLResult, info->dwHandle);

			   break;
			   */
#ifdef ENABLE_LRN_BATTLE_PASS
		case QID_BATTLE_PASS_QUEST:
		{
			sys_log(0, "QID_BATTLE_PASS_QUEST %u", info->dwHandle);
			RESULT_BATTLE_PASS_QUEST_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		}
		break;
		
		case QID_BATTLE_PASS_REWARD:
		{
			sys_log(0, "QID_BATTLE_PASS_REWARD %u", info->dwHandle);
			RESULT_BATTLE_PASS_REWARD_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		}
		break;
		
		case QID_BATTLE_PASS_INFO:
		{
			sys_log(0, "QID_BATTLE_PASS_INFO %u", info->dwHandle);
			RESULT_BATTLE_PASS_INFO_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		}
		break;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
		case QID_PRIVATE_SHOP:
		{
			sys_log(0, "QID_PRIVATE_SHOP %u", info->dwHandle);
			RESULT_PRIVATE_SHOP_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		} break;

		case QID_PRIVATE_SHOP_ITEM:
		{
			sys_log(0, "QID_PRIVATE_SHOP_ITEM %u", info->dwHandle);
			RESULT_PRIVATE_SHOP_ITEM_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		} break;

		case QID_PRIVATE_SHOP_SALE:
		{
			sys_log(0, "QID_PRIVATE_SHOP_SALE %u", info->dwHandle);
			RESULT_PRIVATE_SHOP_SALE_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
		} break;
#endif
	}
}

void CClientManager::RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo)
{
	TPlayerTable tab;

	if (!CreatePlayerTableFromRes(pRes, &tab))
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	CLoginData * pkLD = GetLoginDataByAID(pkInfo->account_id);

	if (!pkLD || pkLD->IsPlay())
	{
		sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	pkLD->SetPlay(true);
	SendLoginToBilling(pkLD, true);
	thecore_memcpy(tab.aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(tab.aiPremiumTimes));

	peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, pkInfo->dwHandle, sizeof(TPlayerTable));
	peer->Encode(&tab, sizeof(TPlayerTable));

	if (tab.id != pkLD->GetLastPlayerID())
	{
		TPacketNeedLoginLogInfo logInfo;
		logInfo.dwPlayerID = tab.id;

		pkLD->SetLastPlayerID( tab.id );

		peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, pkInfo->dwHandle, sizeof(TPacketNeedLoginLogInfo) );
		peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
	}
}

void CClientManager::RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID)
{
	static std::vector<TPlayerItem> s_items;

	CreateItemTableFromRes(pRes, &s_items, dwPID);
	DWORD dwCount = s_items.size();

	peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
	peer->EncodeDWORD(dwCount);

	CreateItemCacheSet(dwPID);

	// ITEM_LOAD_LOG_ATTACH_PID
	sys_log(0, "ITEM_LOAD: count %u pid %u", dwCount, dwPID);
	// END_OF_ITEM_LOAD_LOG_ATTACH_PID

	if (dwCount)
	{
		peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

		for (DWORD i = 0; i < dwCount; ++i)
			PutItemCache(&s_items[i], true);
	}
}

// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
void CClientManager::RESULT_AFFECT_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0)
	{
		// @fixme402 begin
		static DWORD dwPID;
		static DWORD dwCount = 0; //1;
		static TPacketAffectElement paeTable = {0};

		dwPID = dwRealPID;
		sys_log(0, "AFFECT_LOAD: count %u PID %u RealPID %u", dwCount, dwPID, dwRealPID);

		peer->EncodeHeader(HEADER_DG_AFFECT_LOAD, dwHandle, sizeof(DWORD) + sizeof(DWORD) + sizeof(TPacketAffectElement) * dwCount);
		peer->Encode(&dwPID, sizeof(DWORD));
		peer->Encode(&dwCount, sizeof(DWORD));
		peer->Encode(&paeTable, sizeof(TPacketAffectElement) * dwCount);
		// @fixme402 end
		return;
	}

	static std::vector<TPacketAffectElement> s_elements;
	s_elements.resize(iNumRows);

	DWORD dwPID = 0;

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TPacketAffectElement & r = s_elements[i];
		row = mysql_fetch_row(pRes);

		if (dwPID == 0)
			str_to_number(dwPID, row[0]);

		str_to_number(r.dwType, row[1]);
		str_to_number(r.bApplyOn, row[2]);
		str_to_number(r.lApplyValue, row[3]);
		str_to_number(r.dwFlag, row[4]);
		str_to_number(r.lDuration, row[5]);
		str_to_number(r.lSPCost, row[6]);
	}

	sys_log(0, "AFFECT_LOAD: count %d PID %u", s_elements.size(), dwPID);

	DWORD dwCount = s_elements.size();

	peer->EncodeHeader(HEADER_DG_AFFECT_LOAD, dwHandle, sizeof(DWORD) + sizeof(DWORD) + sizeof(TPacketAffectElement) * dwCount);
	peer->Encode(&dwPID, sizeof(DWORD));
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_elements[0], sizeof(TPacketAffectElement) * dwCount);
}

void CClientManager::RESULT_QUEST_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0)
	{
		DWORD dwCount = 0;
		peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD));
		peer->Encode(&dwCount, sizeof(DWORD));
		return;
	}

	static std::vector<TQuestTable> s_table;
	s_table.resize(iNumRows);

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TQuestTable & r = s_table[i];

		row = mysql_fetch_row(pRes);

		str_to_number(r.dwPID, row[0]);
		strlcpy(r.szName, row[1], sizeof(r.szName));
		strlcpy(r.szState, row[2], sizeof(r.szState));
		str_to_number(r.lValue, row[3]);
	}

	sys_log(0, "QUEST_LOAD: count %d PID %u", s_table.size(), s_table[0].dwPID);

	DWORD dwCount = s_table.size();

	peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD) + sizeof(TQuestTable) * dwCount);
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_table[0], sizeof(TQuestTable) * dwCount);
}

/*
 * PLAYER SAVE
 */
void CClientManager::QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable * pkTab)
{
	if (g_test_server)
		sys_log(0, "PLAYER_SAVE: %s", pkTab->name);

	PutPlayerCache(pkTab);
}

typedef std::map<DWORD, time_t> time_by_id_map_t;
static time_by_id_map_t s_createTimeByAccountID;

/*
 * PLAYER CREATE
 */
void CClientManager::__QUERY_PLAYER_CREATE(CPeer *peer, DWORD dwHandle, TPlayerCreatePacket* packet)
{
	char	queryStr[QUERY_MAX_LEN];
	int		queryLen;
	int		player_id;

	time_by_id_map_t::iterator it = s_createTimeByAccountID.find(packet->account_id);

	if (it != s_createTimeByAccountID.end())
	{
		time_t curtime = time(0);

		if (curtime - it->second < 30)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}
	}

	queryLen = snprintf(queryStr, sizeof(queryStr),
			"SELECT pid%u FROM player_index%s WHERE id=%d", packet->account_index + 1, GetTablePostfix(), packet->account_id);

	auto pMsg0(CDBManager::instance().DirectQuery(queryStr));
	if (pMsg0->Get()->uiNumRows != 0)
	{
		if (!pMsg0->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg0->Get()->pSQLResult);

		DWORD dwPID = 0; str_to_number(dwPID, row[0]);
		if (row[0] && dwPID > 0)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			sys_log(0, "ALREADY EXIST AccountChrIdx %d ID %d", packet->account_index, dwPID);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	if (g_stLocale == "sjis")
		snprintf(queryStr, sizeof(queryStr),
			"SELECT COUNT(*) as count FROM player%s WHERE name='%s' collate sjis_japanese_ci",
			GetTablePostfix(), packet->player_table.name);
	else
	snprintf(queryStr, sizeof(queryStr),
			"SELECT COUNT(*) as count FROM player%s WHERE name='%s'", GetTablePostfix(), packet->player_table.name);

	auto pMsg1(CDBManager::instance().DirectQuery(queryStr));
	if (pMsg1->Get()->uiNumRows)
	{
		if (!pMsg1->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg1->Get()->pSQLResult);

		if (*row[0] != '0')
		{
			sys_log(0, "ALREADY EXIST name %s, row[0] %s query %s", packet->player_table.name, row[0], queryStr);
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	queryLen = snprintf(queryStr, sizeof(queryStr),
			"INSERT INTO player%s "
			"(id, account_id, name, level, st, ht, dx, iq, "
			"job, voice, dir, x, y, z, "
			"hp, mp, random_hp, random_sp, stat_point, stamina, part_base, part_main, part_hair,"
			#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"part_acce, "
			#endif
#ifdef ENABLE_CHEQUE_SYSTEM
			" gold, cheque, playtime, "
#else
			" gold, playtime, "
#endif
#ifdef WEEKLY_RANK_BYLUZER
			"w_monsters, w_bosses, w_metins, w_dungs, w_ds, w_refine, w_sm, weekly_season, "
#endif
			"skill_level, quickslot"
#ifdef TITLE_SYSTEM_BYLUZER
			", titles_active"
#endif
			") VALUES("
			"0, %u, '%s', %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %d, %d, %d, %d, 0, "
			#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"0, "
			#endif
#ifdef ENABLE_LONG_LONG
			"%lld, "
#else
			"%d, "
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
			"%lld, "
#endif
			"0, "
#ifdef WEEKLY_RANK_BYLUZER
			"0, 0, 0, 0, 0, 0, 0, 0, "
#endif
			,
			GetTablePostfix(),
			packet->account_id, packet->player_table.name, packet->player_table.level, packet->player_table.st, packet->player_table.ht, packet->player_table.dx, packet->player_table.iq,
			packet->player_table.job, packet->player_table.voice, packet->player_table.dir, packet->player_table.x, packet->player_table.y, packet->player_table.z,
			packet->player_table.hp, packet->player_table.sp, packet->player_table.sRandomHP, packet->player_table.sRandomSP, packet->player_table.stat_point, packet->player_table.stamina, packet->player_table.part_base, packet->player_table.part_base, 
#ifdef ENABLE_CHEQUE_SYSTEM
			packet->player_table.gold, packet->player_table.cheque);
#else
			packet->player_table.gold);
#endif

#ifdef ENABLE_LONG_LONG
	#ifdef ENABLE_CHEQUE_SYSTEM
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %lld cheque %d st %d ht %d job %d",
	#else
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %lld, st %d ht %d job %d",
	#endif
#else
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %d, st %d ht %d job %d",
#endif
			packet->account_id,
			packet->player_table.name,
			packet->player_table.level,
			packet->player_table.gold,
#ifdef ENABLE_CHEQUE_SYSTEM
			packet->player_table.cheque,
#endif
			packet->player_table.st,
			packet->player_table.ht,
			packet->player_table.job);

	static char text[4096 + 1];

	CDBManager::instance().EscapeString(text, packet->player_table.skills, sizeof(packet->player_table.skills));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s', ", text);
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	CDBManager::instance().EscapeString(text, packet->player_table.quickslot, sizeof(packet->player_table.quickslot));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s',", text);

#ifdef TITLE_SYSTEM_BYLUZER
	CDBManager::instance().EscapeString(text, packet->player_table.titles, sizeof(packet->player_table.titles));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s')", text);
#endif

	auto pMsg2(CDBManager::instance().DirectQuery(queryStr));
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	if (pMsg2->Get()->uiAffectedRows <= 0)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
		sys_log(0, "ALREADY EXIST3 query: %s AffectedRows %lu", queryStr, pMsg2->Get()->uiAffectedRows);
		return;
	}

	player_id = pMsg2->Get()->uiInsertID;

	snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%d=%d WHERE id=%d",
			GetTablePostfix(), packet->account_index + 1, player_id, packet->account_id);

	auto pMsg3(CDBManager::instance().DirectQuery(queryStr));
	if (pMsg3->Get()->uiAffectedRows <= 0)
	{
		sys_err("QUERY_ERROR: %s", queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), player_id);
		CDBManager::instance().DirectQuery(queryStr);

		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	TPacketDGCreateSuccess pack;
	memset(&pack, 0, sizeof(pack));

	pack.bAccountCharacterIndex = packet->account_index;

	pack.player.dwID			= player_id;
	strlcpy(pack.player.szName, packet->player_table.name, sizeof(pack.player.szName));
	pack.player.byJob			= packet->player_table.job;
	pack.player.byLevel			= 1;
	pack.player.dwPlayMinutes	= 0;
	pack.player.byST			= packet->player_table.st;
	pack.player.byHT			= packet->player_table.ht;
	pack.player.byDX 			= packet->player_table.dx;
	pack.player.byIQ			= packet->player_table.iq;
	pack.player.wMainPart		= packet->player_table.part_base;
	pack.player.x			= packet->player_table.x;
	pack.player.y			= packet->player_table.y;

	peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_SUCCESS, dwHandle, sizeof(TPacketDGCreateSuccess));
	peer->Encode(&pack, sizeof(TPacketDGCreateSuccess));

#ifdef ENABLE_LRN_BATTLE_PASS
	char szQueryBattlePass[1024];
	snprintf(szQueryBattlePass, sizeof(szQueryBattlePass), "INSERT INTO player_battle_pass%s (pid, level, exp, premium) VALUES (%d, %d, %d, %d)", GetTablePostfix(), player_id, 1, 0, 0);
	std::unique_ptr<SQLMsg> pIns(CDBManager::instance().DirectQuery(szQueryBattlePass));
#endif

	sys_log(0, "7 name %s job %d", pack.player.szName, pack.player.byJob);

	s_createTimeByAccountID[packet->account_id] = time(0);
}

/*
 * PLAYER DELETE
 */
void CClientManager::__QUERY_PLAYER_DELETE(CPeer* peer, DWORD dwHandle, TPlayerDeletePacket* packet)
{
	if (!packet->login[0] || !packet->player_id || packet->account_index >= PLAYER_PER_ACCOUNT)
		return;

	CLoginData * ld = GetLoginDataByLogin(packet->login);

	if (!ld)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
		peer->EncodeBYTE(packet->account_index);
		return;
	}

	TAccountTable & r = ld->GetAccountRef();

	// block for japan
	if (g_stLocale != "sjis")
	{
		if (!IsChinaEventServer())
		{
			if (strlen(r.social_id) < 7 || strncmp(packet->private_code, r.social_id + strlen(r.social_id) - 7, 7))
			{
				sys_log(0, "PLAYER_DELETE FAILED len(%d)", strlen(r.social_id));
				peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
				peer->EncodeBYTE(packet->account_index);
				return;
			}

			CPlayerTableCache * pkPlayerCache = GetPlayerCache(packet->player_id);
			if (pkPlayerCache)
			{
				TPlayerTable * pTab = pkPlayerCache->Get();

				if (pTab->level >= m_iPlayerDeleteLevelLimit)
				{
					sys_log(0, "PLAYER_DELETE FAILED LEVEL %u >= DELETE LIMIT %d", pTab->level, m_iPlayerDeleteLevelLimit);
					peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
					peer->EncodeBYTE(packet->account_index);
					return;
				}

				if (pTab->level < m_iPlayerDeleteLevelLimitLower)
				{
					sys_log(0, "PLAYER_DELETE FAILED LEVEL %u < DELETE LIMIT %d", pTab->level, m_iPlayerDeleteLevelLimitLower);
					peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
					peer->EncodeBYTE(packet->account_index);
					return;
				}
			}
		}
	}

	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT p.id, p.level, p.name FROM player_index%s AS i, player%s AS p WHERE pid%u=%u AND pid%u=p.id",
			GetTablePostfix(), GetTablePostfix(), packet->account_index + 1, packet->player_id, packet->account_index + 1);

	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle, packet->player_id);
	pi->account_index = packet->account_index;

	sys_log(0, "PLAYER_DELETE TRY: %s %d pid%d", packet->login, packet->player_id, packet->account_index + 1);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_DELETE, peer->GetHandle(), pi);
}

//

//
void CClientManager::__RESULT_PLAYER_DELETE(CPeer *peer, SQLMsg* msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;

	if (msg->Get() && msg->Get()->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		DWORD dwPID = 0;
		str_to_number(dwPID, row[0]);

		int deletedLevelLimit = 0;
		str_to_number(deletedLevelLimit, row[1]);

		char szName[64];
		strlcpy(szName, row[2], sizeof(szName));

		if (deletedLevelLimit >= m_iPlayerDeleteLevelLimit && !IsChinaEventServer())
		{
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u >= DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimit);
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		if (deletedLevelLimit < m_iPlayerDeleteLevelLimitLower)
		{
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u < DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimitLower);
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		char queryStr[QUERY_MAX_LEN];

		snprintf(queryStr, sizeof(queryStr), "INSERT INTO player%s_deleted SELECT * FROM player%s WHERE id=%d",
				GetTablePostfix(), GetTablePostfix(), pi->player_id);
		auto pIns(CDBManager::instance().DirectQuery(queryStr));

		if (pIns->Get()->uiAffectedRows == 0 || pIns->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAILED %u CANNOT INSERT TO player%s_deleted", dwPID, GetTablePostfix());

			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		sys_log(0, "PLAYER_DELETE SUCCESS %u", dwPID);

		char account_index_string[16];

		snprintf(account_index_string, sizeof(account_index_string), "player_id%d", m_iPlayerIDStart + pi->account_index);

		CPlayerTableCache * pkPlayerCache = GetPlayerCache(pi->player_id);

		if (pkPlayerCache)
		{
			m_map_playerCache.erase(pi->player_id);
			delete pkPlayerCache;
		}

		TItemCacheSet * pSet = GetItemCacheSet(pi->player_id);

		if (pSet)
		{
			TItemCacheSet::iterator it = pSet->begin();

			while (it != pSet->end())
			{
				CItemCache * pkItemCache = *it++;
				DeleteItemCache(pkItemCache->Get()->id);
			}

			pSet->clear();
			delete pSet;

			m_map_pkItemCacheSetPtr.erase(pi->player_id);
		}

#ifdef __PREMIUM_PRIVATE_SHOP__
		CPrivateShopCache* pPrivateShopCache = GetPrivateShopCache(pi->player_id);
		if (pPrivateShopCache)
			DeletePrivateShopCache(pi->player_id);

		TPrivateShopItemCacheSet* pPrivateShopItemCacheSet = GetPrivateShopItemCacheSet(pi->player_id);
		if (pPrivateShopItemCacheSet)
			DeletePrivateShopItemCacheSet(pi->player_id);

		LPPRIVATE_SHOP pPrivateShop = GetPrivateShop(pi->player_id);
		if (pPrivateShop)
		{
			// Just to re-assure
			pPrivateShop->BindOwnerPeerHandle(0);

			// @note: No cache will be flushed since we already deleted it
			PrivateShopGameDespawn(pPrivateShop);
		}
#endif

		snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%u=0 WHERE pid%u=%d",
				GetTablePostfix(),
				pi->account_index + 1,
				pi->account_index + 1,
				pi->player_id);

		auto pMsg(CDBManager::instance().DirectQuery(queryStr));
		if (pMsg->Get()->uiAffectedRows == 0 || pMsg->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAIL WHEN UPDATE account table");
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().DirectQuery(queryStr);

#ifdef ENABLE_SWITCHBOT //Replace your old line with the whole ifdef/else statement
//		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (window < %d or window = %d or window = %d)", GetTablePostfix(), pi->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY, SWITCHBOT);
		// aslan buff
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (window < %d or window = %d or window = %d or window = %d)", GetTablePostfix(), pi->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY, BUFF_EQUIPMENT, SWITCHBOT);
#else
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (window < %d or window = %d)", GetTablePostfix(), pi->player_id, SAFEBOX, DRAGON_SOUL_INVENTORY);
#endif
		CDBManager::instance().DirectQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM quest%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM guild_member%s WHERE pid=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		// MYSHOP_PRICE_LIST
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM myshop_pricelist%s WHERE owner_id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);
		// END_OF_MYSHOP_PRICE_LIST

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM messenger_list%s WHERE account='%s' OR companion='%s'", GetTablePostfix(), szName, szName);
		CDBManager::instance().AsyncQuery(queryStr);

#ifdef __PREMIUM_PRIVATE_SHOP__
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM private_shop%s WHERE owner_id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM private_shop_item%s WHERE owner_id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);
#endif

		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_SUCCESS, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
	else
	{
		sys_log(0, "PLAYER_DELETE FAIL NO ROW");
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
}

void CClientManager::QUERY_ADD_AFFECT(CPeer * peer, TPacketGDAddAffect * p)
{
	char queryStr[QUERY_MAX_LEN];
	/*
	   snprintf(queryStr, sizeof(queryStr),
	   "INSERT INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
	   "VALUES(%u, %u, %u, %d, %u, %d, %d) "
	   "ON DUPLICATE KEY UPDATE lApplyValue=%d, dwFlag=%u, lDuration=%d, lSPCost=%d",
	   GetTablePostfix(),
	   p->dwPID,
	   p->elem.dwType,
	   p->elem.bApplyOn,
	   p->elem.lApplyValue,
	   p->elem.dwFlag,
	   p->elem.lDuration,
	   p->elem.lSPCost,
	   p->elem.lApplyValue,
	   p->elem.dwFlag,
	   p->elem.lDuration,
	   p->elem.lSPCost);
	   */
	snprintf(queryStr, sizeof(queryStr),
			"REPLACE INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
			"VALUES(%u, %u, %u, %ld, %u, %ld, %ld)",
			GetTablePostfix(),
			p->dwPID,
			p->elem.dwType,
			p->elem.bApplyOn,
			p->elem.lApplyValue,
			p->elem.dwFlag,
			p->elem.lDuration,
			p->elem.lSPCost);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_REMOVE_AFFECT(CPeer * peer, TPacketGDRemoveAffect * p)
{
	char queryStr[QUERY_MAX_LEN];

	snprintf(queryStr, sizeof(queryStr),
			"DELETE FROM affect%s WHERE dwPID=%u AND bType=%u AND bApplyOn=%u",
			GetTablePostfix(), p->dwPID, p->dwType, p->bApplyOn);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_HIGHSCORE_REGISTER(CPeer* peer, TPacketGDHighscore * data)
{
	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT value FROM highscore%s WHERE board='%s' AND pid = %u", GetTablePostfix(), data->szBoard, data->dwPID);

	sys_log(0, "HEADER_GD_HIGHSCORE_REGISTER: PID %u", data->dwPID);

	ClientHandleInfo * pi = new ClientHandleInfo(0);
	strlcpy(pi->login, data->szBoard, sizeof(pi->login));
	pi->account_id = (DWORD)data->lValue;
	pi->player_id = data->dwPID;
	pi->account_index = (data->cDir > 0);

	CDBManager::instance().ReturnQuery(szQuery, QID_HIGHSCORE_REGISTER, peer->GetHandle(), pi);
}

void CClientManager::RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;
	//DWORD dwHandle = pi->dwHandle;

	char szBoard[21];
	strlcpy(szBoard, pi->login, sizeof(szBoard));
	int value = (int)pi->account_id;

	SQLResult * res = msg->Get();

	if (res->uiNumRows == 0)
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
		CDBManager::instance().AsyncQuery(buf);
	}
	else
	{
		if (!res->pSQLResult)
		{
			delete pi;
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(res->pSQLResult);
		if (row && row[0])
		{
			int current_value = 0; str_to_number(current_value, row[0]);
			if (((pi->account_index)&&(current_value >= value)) || ((!pi->account_index)&&(current_value <= value)))
			{
				value = current_value;
			}
			else
			{
				char buf[256];
				snprintf(buf, sizeof(buf), "REPLACE INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
				CDBManager::instance().AsyncQuery(buf);
			}
		}
		else
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
			CDBManager::instance().AsyncQuery(buf);
		}
	}

	delete pi;
}

void CClientManager::InsertLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	if (it != m_map_logout.end())
	{
		if (g_log)
			sys_log(0, "LOGOUT: Update player time pid(%d)", pid);

		it->second->time = time(0);
		return;
	}

	TLogoutPlayer * pLogout = new TLogoutPlayer;
	pLogout->pid = pid;
	pLogout->time = time(0);
	m_map_logout.emplace(pid, pLogout);

	if (g_log)
		sys_log(0, "LOGOUT: Insert player pid(%d)", pid);
}

void CClientManager::DeleteLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	if (it != m_map_logout.end())
	{
		delete it->second;
		m_map_logout.erase(it);
	}
}

extern int g_iLogoutSeconds;

void CClientManager::UpdateLogoutPlayer()
{
	time_t now = time(0);

	TLogoutPlayerMap::iterator it = m_map_logout.begin();

	while (it != m_map_logout.end())
	{
		TLogoutPlayer* pLogout = it->second;

		if (now - g_iLogoutSeconds > pLogout->time)
		{
			FlushItemCacheSet(pLogout->pid);
			FlushPlayerCacheSet(pLogout->pid);

#ifdef __PREMIUM_PRIVATE_SHOP__
			FlushPrivateShopCache(pLogout->pid);
			FlushPrivateShopItemCacheSet(pLogout->pid);
			FlushPrivateShopSaleCacheSet(pLogout->pid);

#ifdef ENABLE_PRIVATE_SHOP_PREMIUM_TIME
			LPPRIVATE_SHOP pPrivateShop = GetPrivateShop(pLogout->pid);
			if (pPrivateShop)
				PrivateShopStartPremiumEvent(pPrivateShop->GetOwner());
#endif

#endif

			delete pLogout;
			m_map_logout.erase(it++);
		}
		else
			++it;
	}
}

void CClientManager::FlushPlayerCacheSet(DWORD pid)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(pid);

	if (it != m_map_playerCache.end())
	{
		CPlayerTableCache * c = it->second;
		m_map_playerCache.erase(it);

		c->Flush();
		delete c;
	}
}

#ifdef ENABLE_LRN_BATTLE_PASS
void CClientManager::RESULT_BATTLE_PASS_QUEST_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	EncodeBattlePassData(peer, pRes, dwHandle, pid, sizeof(TBattlePassQuestTable), HEADER_DG_BATTLE_PASS_QUEST_LOAD, [](void* dataPtr, MYSQL_ROW row) {
		TBattlePassQuestTable& r = *reinterpret_cast<TBattlePassQuestTable*>(dataPtr);
		
		r.pid = atoi(row[0]);
		r.index = atoi(row[1]);
		r.count = atoi(row[2]);
		r.status = atoi(row[3]);
	});
}

void CClientManager::RESULT_BATTLE_PASS_REWARD_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	EncodeBattlePassData(peer, pRes, dwHandle, pid, sizeof(TBattlePassRewardTable), HEADER_DG_BATTLE_PASS_REWARD_LOAD, [](void* dataPtr, MYSQL_ROW row) {
		TBattlePassRewardTable& r = *reinterpret_cast<TBattlePassRewardTable*>(dataPtr);
		
		r.pid = atoi(row[0]);
		r.level = atoi(row[1]);
		r.status_free = atoi(row[2]);
		r.status_premium = atoi(row[3]);
	});
}

void CClientManager::RESULT_BATTLE_PASS_INFO_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	EncodeBattlePassData(peer, pRes, dwHandle, pid, sizeof(TBattlePassInformationTable), HEADER_DG_BATTLE_PASS_INFO_LOAD, [](void* dataPtr, MYSQL_ROW row) {
		TBattlePassInformationTable& r = *reinterpret_cast<TBattlePassInformationTable*>(dataPtr);
		
		r.pid = atoi(row[0]);
		r.level = atoi(row[1]);
		r.exp = atoi(row[2]);
		r.premium = atoi(row[3]);
	});
}

template<typename RowMapper>
void CClientManager::EncodeBattlePassData(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid, int dataSize, BYTE header, RowMapper rowMapper)
{
	const int iNumRows = mysql_num_rows(pRes);
	
	if (iNumRows == 0)
	{
		const DWORD dwCount = 0;
		
		peer->EncodeHeader(header, dwHandle, sizeof(dwCount));
		peer->Encode(&dwCount, sizeof(dwCount));
		
		return;
	}
	
	std::vector<char> buffer(iNumRows * dataSize);
	MYSQL_ROW row;
	
	for (std::size_t i = 0; i < iNumRows; ++i)
	{
		char* dataPtr = &buffer[i * dataSize];
		row = mysql_fetch_row(pRes);
		
		rowMapper(dataPtr, row);
	}
	
	const DWORD dwCount = iNumRows;
	
	peer->EncodeHeader(header, dwHandle, sizeof(DWORD) + dataSize * dwCount);
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(buffer.data(), dataSize * dwCount);
}
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
