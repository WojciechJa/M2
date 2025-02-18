#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../libthecore/include/xmd5.h"
#endif
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	#include "CollectionsSystem.hpp"
#endif
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "monarch.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_manager.h"
#endif
#include "../../common/VnumHelper.h"

#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
#include "DragonSoul.h"
#endif

#include "target.h"

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
ACMD(do_buy_battlepass)
{
	if (!ch)
		return;

	auto* pBattlePass = ch->GetBattlePass();
    if (!pBattlePass) {
        sys_err("BattlePass object is null %s", ch->GetName());
        return;
    }

	if (pBattlePass->GetPremium() != 0) {
        ch->ChatPacket(CHAT_TYPE_INFO, "Masz ju¿ BattlePass Premium!");
        return;
    }

    const auto dwPrice = 500;
    const auto dwCash = ch->GetCash();

    if (dwCash < dwPrice) {
        ch->ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o SM!");
        return;
    }

    ch->SetCash(dwCash - dwPrice);
    pBattlePass->SetPremium();
    ch->ChatPacket(CHAT_TYPE_INFO, "BattlePass Premium zosta³ aktywowany.");
}
#endif

#ifdef ENABLE_DRAGON_SOUL_TIME_RENEWAL
ACMD(do_renew_alchemy)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::RenewDS, std::chrono::milliseconds(30000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::RenewDS));
		return;
	}
#endif

	int price = 200000000;
	if (ch->GetGold() < price) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o Won/Yang.");
		return;
	}

	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item != 0 || NULL) {
			item->SetSocket(0, 86400);
		}
	}
	ch->ChatPacket(CHAT_TYPE_INFO, "Czas alchemii zosta³ przed³u¿ony.");
	ch->PointChange(POINT_GOLD, -price);
}
#endif

ACMD(do_dungeons) {
	const DWORD dungeonIdx = ch->GetMapIndex();
	const DWORD mapIdx = dungeonIdx / 10000;

	std::unordered_map<int, std::tuple<int, int, int, int, const char*>> dungeonData = {
		{ 17, {200040, 17, 9472, 6144, "dungeon_wisdom.join_wisdom_dungeon"} },
		{ 21, {200040, 21, 7680, 6144, "dt_dungeon.join_dt_dungeon"} },
		{ 13, {200041, 13, 8448, 5376, "wukong_dungeon.join_wukong_dungeon"} },
		{ 14, {200042, 14, 8448, 5888, "scorpion_dungeon.join_scorpion_dungeon"} },
		{ 16, {200043, 16, 8960, 6144, "arctic_dungeon.join_arctic_dungeon"} },
		{ 18, {200044, 18, 9728, 5376, "hellsgate_dungeon.join_hellsgate_dungeon"} },
		{ 12, {200045, 12, 7680, 5376, "sanctuary_dungeon.join_sanctuary_dungeon"} },
		{ 15, {200046, 15, 9368, 5615, "widmo_dungeon.join_widmo_dungeon"} },
		{ 24, {200047, 24, 9368, 7152, "widmo2_dungeon.join_widmo2_dungeon"} }
	};
	
	auto it = dungeonData.find(mapIdx);
	if (it == dungeonData.end()) {
		ch->ChatPacket(CHAT_TYPE_INFO, "Nie znajdujesz siê na dungeonie!");
		return;
	}

	int requiredItem, dungeonId, x, y;
	const char* questFlag;
	std::tie(requiredItem, dungeonId, x, y, questFlag) = it->second;

	if (ch->GetDungeon()->GetFlag("can_rejoin") > 0) {
		if (ch->GetParty()) {
			if (ch->GetParty()->GetNearMemberCount()) {
				if (ch->GetParty()->GetLeader() && ch->GetParty()->GetLeader()->CountSpecifyItem(requiredItem) >= ch->GetParty()->GetMemberCount()) {
					ch->GetParty()->GetLeader()->RemoveSpecifyItem(requiredItem, ch->GetParty()->GetMemberCount());
				} else {
					ch->ChatPacket(CHAT_TYPE_INFO, "Lider nie posiada w ekwipunku wymaganej liczby przepustek.");
					return;
				}
			} else {
				ch->ChatPacket(CHAT_TYPE_INFO, "Osoba z grupy musi byæ obok ciebie!");
				return;
			}
		} else {
			if (ch->CountSpecifyItem(requiredItem) >= 1) {
				ch->RemoveSpecifyItem(requiredItem, 1);
			} else {
				ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz przepustki!");
				return;
			}
		}

		LPDUNGEON pDungeon = CDungeonManager::instance().Create(dungeonId);
		if (!pDungeon)
			return;

		pDungeon->JumpAll(dungeonIdx, x, y);
		ch->SetQuestFlag(questFlag, 1);
	} else {
		ch->ChatPacket(CHAT_TYPE_INFO, "Nie skoñczy³eœ jeszcze wyprawy!");
		return;
	}
}

ACMD(do_toggle_antyexp)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ChangeEXP, std::chrono::milliseconds(20000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ChangeEXP));
        return;
    }

	if (ch->GetLevel() < 30)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Tej opcji mo¿esz u¿yæ od 30 poziomu postaci!");
		return;
	}

	if (!ch->GetDesc())
	{
		return;
	}

	TPacketGCAntyExp pack;
	pack.bHeader = HEADER_GC_ANTYEXP;

	if (ch->GetQuestFlag("antyexp.status") == 1)
	{
		ch->SetQuestFlag("antyexp.status", 0);
		ch->SetToggleAntyExp(false);
		pack.status = 0;
		ch->ChatPacket(CHAT_TYPE_INFO, "Zdobywanie doœwiadczenia: W³¹czone");
	}
	else
	{
		ch->SetQuestFlag("antyexp.status", 1);
		ch->SetToggleAntyExp(true);
		pack.status = 1;
		ch->ChatPacket(CHAT_TYPE_INFO, "Zdobywanie doœwiadczenia: Wy³¹czone");
	}

	ch->GetDesc()->Packet(&pack, sizeof(TPacketGCAntyExp));
}

//vasto split_items
ACMD(do_split_items) //SPLIT ITEMS
{
	if (!ch)
		return;
	
	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));
	
	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Wrong command use.");
		return;
	}
	
	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Zamknij wszystkie okna i odczekaj kilka sekund!");
		return;
	}
	
	WORD count = 0; //zamien se to na bajta jak nie masz zwiekszonych slotow.
	WORD cell = 0;
	WORD destCell = 0;
	
	str_to_number(cell, arg1);
	str_to_number(count, arg2);
	str_to_number(destCell, arg3);

	LPITEM item = ch->GetInventoryItem(cell);
	if (item != NULL)
	{
		WORD itemCount = item->GetCount();
		while (itemCount > 0)
		{
		#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
			if (item->IsSkillBook()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptySkillBookInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else if (item->IsUpgradeItem()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyUpgradeItemsInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else if (item->IsStone()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyStoneInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else if (item->IsBox()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyBoxInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else if (item->IsEfsun()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyEfsunInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else if (item->IsCicek()) {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyCicekInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
			else {
				if (count > itemCount)
					count = itemCount;

				int iEmptyPosition = ch->GetEmptyInventoryFromIndex(destCell, item->GetSize());
				if (iEmptyPosition == -1)
					return;
				
				itemCount -= count;
				ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
			}
		#else
			if (count > itemCount)
				count = itemCount;

			int iEmptyPosition = ch->GetEmptyInventoryFromIndex(destCell, item->GetSize());
			if (iEmptyPosition == -1)
				return;
			
			itemCount -= count;
			ch->MoveItem(TItemPos(INVENTORY, cell), TItemPos(INVENTORY, iEmptyPosition), count);
		#endif
		}
	}
}

#ifdef ENABLE_VS_SHOP_SEARCH
ACMD(do_search_in_shops)
{
	char arg1[256], arg2[256], arg3[256], arg4[256];
	four_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3), arg4, sizeof(arg4));
	// if (!*arg1 || !*arg2 || !*arg3)
	// {
	// 	ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: search_in_shops <search_type> <type> <subtype>");
	// 	return;
	// }
	
	DWORD search_type, vnum, type, subtype;
	str_to_number(search_type, arg1);
	str_to_number(vnum, arg2);
	str_to_number(type, arg3);
	str_to_number(subtype, arg4);

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ShopSearch, std::chrono::milliseconds(3000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ShopSearch));
		return;
	}
#endif
	COfflineShopManager::Instance().ClearShopSearch(ch);

	std::string vnumString = "0";

    COfflineShopManager::Instance().SearchInShops(ch, search_type, vnum, vnumString, type, subtype);
}
#endif

#ifdef ENABLE_PROMO_CODE_SYSTEM
ACMD(do_promo_code_system)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if (!ch || arg1[0] == 0)
		return;
	
	const std::string& strArg1 = std::string(arg1);
	
	if (strArg1 == "check_code")
	{
		if (ch->GetQuestFlag("promo_code.cooltime") && get_global_time() < ch->GetQuestFlag("promo_code.cooltime"))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Do ponownego sprawdzenia kodu promocyjnego pozostalo: %d sek.", (ch->GetQuestFlag("promo_code.cooltime") - get_global_time()) % 180);
			return;
		}
	
		if (strlen(arg2) > 10)
			return;
		
		if (ch->check_PromoCodeSystem(arg2))
		{
#ifdef ENABLE_PROMO_CODE_ONE_USE_PER_ACCOUNT
			if (ch->restriction_PromoCodeSystem(arg2))
				ch->reward_PromoCodeSystem(arg2);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "Na tym koncie zostal juz wykorzystany ten kod promocyjny!");
#else
			ch->reward_PromoCodeSystem(arg2);
#endif
		}
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Podany przez Ciebie kod promocyjny nie istnieje!");
		
		ch->SetQuestFlag("promo_code.cooltime", get_global_time() + 5);
	}
}

ACMD(do_promo_code_open)
{
	if (!ch)
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCodeWindow");
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
ACMD(do_choose_quest)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!ch || !*arg1 && !*arg2)
		return;

#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::ChangeQuest, std::chrono::milliseconds(1000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::ChangeQuest));
		return;
	}
#endif

	BYTE id = 0;
	BYTE id1 = 0;
	str_to_number(id, arg1);
	str_to_number(id1, arg2);

	ch->SetQuestFlag("collector.state", id);
	// ch->ChatPacket(CHAT_TYPE_INFO, "%d %d %d", ch->GetQuestFlag("collector.state"), id, id1);

	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), id1, 0);
}
ACMD(do_open_collect_window)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::CollectWindow, std::chrono::milliseconds(700))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::CollectWindow));
		return;
	}
#endif
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCollectWindow");
	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), 13, 2);
}
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
ACMD(do_hide_costume)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::HideCostume, std::chrono::milliseconds(15000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::HideCostume));
		return;
	}
#endif
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
	else if (bPartPos == 3)
		ch->SetAcceCostumeHidden(hidden);
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
	else if (bPartPos == 5)
		ch->SetAuraCostumeHidden(hidden);
	else if (bPartPos == 6)
		ch->SetStoleCostumeHidden(hidden);
	else
		return;

	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_NEW_STONE_DETACH
ACMD(do_detach_stone)
{
	if (!ch)
	{
		return;
	}

	if (ch->IsObserverMode() || ch->GetExchange() || ch->IsCubeOpen() || ch->IsOpenSafebox() || ch->GetMyShop() || ch->GetShopOwner())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_WINDOWS_FIRST"));
		return;
	}

	const char* line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	BYTE socketIdx;
	WORD scrollSlotPos, targetSlotPos;

	str_to_number(socketIdx, arg1);
	str_to_number(scrollSlotPos, arg2);
	str_to_number(targetSlotPos, arg3);

	const DWORD SCROLL_VNUM = 25100;
	const DWORD ITEM_BROKEN_METIN_VNUM = 28960;

	LPITEM scroll = ch->GetItem(TItemPos(INVENTORY, scrollSlotPos));

	if (!scroll || scroll->GetVnum() != SCROLL_VNUM)
	{
		return;
	}

	LPITEM item = ch->GetItem(TItemPos(INVENTORY, targetSlotPos));

	if (!item || (item->GetType() != ITEM_WEAPON && (item->GetType() != ITEM_ARMOR && item->GetSubType() != ARMOR_BODY)) || item->IsEquipped())
	{
		return;
	}

	long socket = item->GetSocket(socketIdx);

	if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
	{
		char hint[64];
		snprintf(hint, sizeof(hint), "USE_DETACHMENT, STONE: %ld", socket);
		LogManager::instance().ItemLog(ch, item, hint, item->GetName());

		item->SetSocket(socketIdx, 1);
		ch->AutoGiveItem(socket);
		scroll->SetCount(scroll->GetCount() - 1);
	}
}
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
ACMD(do_clear_shopsearch)
{
	int iFloodResult = ch->GetQuestFlag("clear_search.flood_check");
	if (iFloodResult){
		if (get_global_time() < iFloodResult + 3) {
			return;
		}
	}

	COfflineShopManager::Instance().ClearShopSearch(ch);
}
#endif

#ifdef TITLE_SYSTEM_BYLUZER
ACMD(do_detach_title)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE id = 0;
	str_to_number(id, arg1);

	ch->DetachTitle(id);
}
#endif

#ifdef ENABLE_SORT_INVENTORY_ITEMS
ACMD (do_sort_inventory)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::SortInventory, std::chrono::milliseconds(10000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::SortInventory));
        return;
    }
#endif

    if (ch->IsDead() || ch->GetExchange() || ch->GetShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
    {
        ch->ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz tego teraz zrobiæ, spróbuj ponownie póŸniej.");
        return;
    }
   
    for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
    {
        LPITEM item = ch->GetInventoryItem(i);
       
        if(!item)
            continue;
       
        if(item->isLocked())
            continue;
       
        if(item->GetCount() == g_bItemCountLimit)
            continue;
       
        if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            for (int j = i; j < INVENTORY_MAX_NUM; ++j)
            {
                LPITEM item2 = ch->GetInventoryItem(j);
               
                if(!item2)
                    continue;
               
                if(item2->isLocked())
                    continue;
   
                if (item2->GetVnum() == item->GetVnum())
                {
                    bool bStopSockets = false;
                   
				   	// if (item->GetType() != ITEM_GACHA && item2->GetType() != ITEM_GACHA) {
					// 	for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
					// 	{
					// 		if (item2->GetSocket(k) != item->GetSocket(k))
					// 		{
					// 			bStopSockets = true;
					// 			break;
					// 		}
					// 	}
					// }
                   
                    if(bStopSockets)
                        continue;
#ifdef __EXTENDED_ITEM_COUNT__
                    int bAddCount = MIN(g_bItemCountLimit - item->GetCount(), item2->GetCount());
#else
					BYTE bAddCount = MIN(g_bItemCountLimit - item->GetCount(), item2->GetCount());
#endif
					// if (item->GetType() == ITEM_GACHA && item2->GetType() == ITEM_GACHA) {
					// 	int newSocketValue = MIN(g_bItemCountLimit - item->GetSocket(0), item2->GetSocket(0));
					// 	item->SetSocket(0, item->GetSocket(0) + newSocketValue);
					// 	item2->SetSocket(0, item2->GetSocket(0) - newSocketValue);
					// }

                    item->SetCount(item->GetCount() + bAddCount);
                    item2->SetCount(item2->GetCount() - bAddCount);
                   
                    continue;
                }
            }
        }
    }
   
   	ch->ChatPacket(CHAT_TYPE_INFO, "Przedmioty zosta³y po³¹czone.");
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_sort_special_inventory)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE type = 0;
	str_to_number(type, arg1);

	ch->SortSpecialInventoryItems(type);
}
#endif
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
ACMD(do_bezposrednie_wpadanie_test)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::Wpadanie, std::chrono::milliseconds(20000)))
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::Wpadanie));
        return;
    }

	TPacketGCWpadanie pack;
	pack.bHeader = HEADER_GC_WPADANIE;
	if (ch->IsWpadanie())
	{
		ch->SetToggleWpadanie(false);
		pack.status = 0;
		ch->ChatPacket(CHAT_TYPE_INFO, "Wpadanie przedmiotów do Dodatkowego Ekwipunku: W³¹czone");
	}
	else
	{
		ch->SetToggleWpadanie(true);
		pack.status = 1;
		ch->ChatPacket(CHAT_TYPE_INFO, "Wpadanie przedmiotów do Dodatkowego Ekwipunku: Wy³¹czone");
	}
	ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWpadanie));
}
#endif

ACMD(do_user_horse_ride)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì Å»°ÍÀ» ÀÌ¿ëÁßÀÔ´Ï´Ù."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» µ¹·Áº¸³Â½À´Ï´Ù."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡¼­ ¸ÕÀú ³»·Á¾ß ÇÕ´Ï´Ù."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
	}
}

ACMD(do_user_horse_feed)
{
#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» Åº »óÅÂ¿¡¼­´Â ¸ÔÀÌ¸¦ ÁÙ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;
	// BYTE bLocale = ch->GetLanguage();

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		// ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»¿¡°Ô %s%s ÁÖ¾ú½À´Ï´Ù."),
		// 	LC_ITEM_NAME(dwFood, bLocale),
		// 	g_iUseLocale ? "" : under_han(LC_ITEM_NAME(dwFood, bLocale)) ? LC_TEXT("À»") : LC_TEXT("¸¦"));
	}
	else
	{
		// ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¾ÆÀÌÅÛÀÌ ÇÊ¿äÇÕ´Ï´Ù"), LC_ITEM_NAME(dwFood, bLocale));
	}
}

#ifdef ENABLE_ODLAMKI_SYSTEM
ACMD(do_odlamki)
{
	LPCHARACTER	npc;
	npc = ch->GetQuestNPC();

	if (npc)
	{
		if (NULL == ch || NULL == npc || !npc)
		{
			return;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOdlamki");
	}
	else {
		return;
	}
}
#endif

// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// ACMD(do_horseshoe)
// {

// 	const char *line;
// 	char arg1[256], arg2[256], arg3[256];
// 	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
// 	one_argument(line, arg3, sizeof(arg3));

// 	if (0 == arg1[0])
// 	{
// 		return;
// 	}

// 	switch (LOWER(arg1[0]))
// 	{
// 		case 'o':	// open
// 			ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenHorseShoe");
// 			break;

// 		case 'b':
// 			LPCHARACTER	npc;
// 			npc = ch->GetQuestNPC();

// 			if (npc)
// 			{
// 				if (NULL == ch || NULL == npc || !npc)
// 				{
// 					return;
// 				}
				
// 				LPITEM item = ITEM_MANAGER::instance().CreateItem(80069, 1, 0, true);

// 				int REQUIRED_ITEM1 = 71129;
// 				int REQUIRED_ITEM2 = 71122;
// 				int REQUIRED_ITEM1_COUNT = 1;
// 				int REQUIRED_ITEM2_COUNT = 1;
// 				int price_cheque = 5000;
// 				int price_po = 30000;
				
// 				if (price_cheque == 0 || price_po == 0)
// 				{
// 					ch->ChatPacket(CHAT_TYPE_INFO, "ERROR: price_cheque or price_po == 0");
// 					return;
// 				}
				
// 				if (ch->GetCheque() < price_cheque)
// 				{
// 					ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganej liczby Won!");
// 					return;
// 				}

// 				if (ch->GetPktOsiag() < price_po)
// 				{
// 					ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganej liczby PO!");
// 					return;
// 				}

// 				if (item)
// 				{
// 					int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

// 					if (iEmptyPos != -1)
// 					{
// 						if (ch->CountSpecifyItem(REQUIRED_ITEM1) >= REQUIRED_ITEM1_COUNT && ch->CountSpecifyItem(REQUIRED_ITEM2) >= REQUIRED_ITEM2_COUNT) {
// 							ch->RemoveSpecifyItem(REQUIRED_ITEM1, REQUIRED_ITEM1_COUNT);
// 							ch->RemoveSpecifyItem(REQUIRED_ITEM2, REQUIRED_ITEM2_COUNT);
// 							ch->PointChange(POINT_CHEQUE, -price_cheque);
// 							ch->PointChange(POINT_PKT_OSIAG, -price_po);
// 							item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
// 							break;
// 						}
// 						else {
// 							ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganych przedmiotów!");
// 							return;
// 						}
// 						break;
// 					}
// 					else
// 					{
// 						M2_DESTROY_ITEM(item);
// 						ch->ChatPacket(CHAT_TYPE_INFO, "Masz za ma³o miejsca w ekwipunku!");
// 						break;
// 					}
// 				}
// 				break;
// 			}
// 			else {
// 				return;
// 			}
// 	}
// }
// #endif

#ifdef ENABLE_POLY_SHOP
ACMD(do_buy_marble)
{
	if (ch->IsObserverMode() || ch->GetExchange())
		return;

	char arg1[256], arg2[256], arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: buy_marble_item <mobVnum> <itemCount> <marmurGrade>");
		return;
	}

	DWORD mobVnum, marmurGrade;
	int itemCount;
	long long price = 0;

	str_to_number(mobVnum, arg1);
	str_to_number(itemCount, arg2);
	str_to_number(marmurGrade, arg3);

	if (!mobVnum || mobVnum <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong mobVnum.");
		return;
	}

	if (marmurGrade > 2 || marmurGrade < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong marmurGrade (must be 0, 1, or 2).");
		return;
	}

	if (itemCount < 1 || itemCount > 20)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "wrong count.");
		return;
	}

	switch (marmurGrade)
	{
		case 0:
			price = (mobVnum == 552) ? 30000000 * itemCount : 50000000 * itemCount;
			break;

		case 1:
			price = 1 * itemCount;
			break;

		case 2:
			price = 3 * itemCount;
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "wrong marmurGrade.");
			return;
	}

	switch (mobVnum)
	{
		case 552:
		case 636:
		case 2001:
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "marble not exist.");
			return;
	}

	if ((marmurGrade == 1 && ch->GetCheque() < price) || 
	    (marmurGrade == 2 && ch->GetCheque() < price) ||
	    (marmurGrade != 1 && marmurGrade != 2 && ch->GetGold() < price))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Nie masz wystarczaj¹co du¿o Yang/Won.");
		return;
	}

	int GRADE_VNUM[3] = {70104, 70105, 70106};
	int REQUIRED_ITEMS[3] = {71001, 71094, 50513};
	int REQUIRED_ITEMS_COUNT[2] = {3, 10};

	LPITEM item = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[0], itemCount, 0, true);
	LPITEM item_grade1 = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[1], itemCount, 0, true);
	LPITEM item_grade2 = ITEM_MANAGER::instance().CreateItem(GRADE_VNUM[2], itemCount, 0, true);

	if (item || item_grade1 || item_grade2)
	{
		int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

		if (iEmptyPos != -1)
		{
			switch (marmurGrade)
			{
				case 0:
					ch->PointChange(POINT_GOLD, -price);
					item->SetSocket(0, mobVnum);
					item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					break;

				case 1:
					if (ch->CountSpecifyItem(REQUIRED_ITEMS[0]) >= REQUIRED_ITEMS_COUNT[0] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[1]) >= REQUIRED_ITEMS_COUNT[0] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[2]) >= REQUIRED_ITEMS_COUNT[0] * itemCount)
					{
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[0], REQUIRED_ITEMS_COUNT[0] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[1], REQUIRED_ITEMS_COUNT[0] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[2], REQUIRED_ITEMS_COUNT[0] * itemCount);

						ch->PointChange(POINT_CHEQUE, -price);
						item_grade1->SetSocket(0, mobVnum);
						item_grade1->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganych przedmiotów!");
					}
					break;

				case 2:
					if (ch->CountSpecifyItem(REQUIRED_ITEMS[0]) >= REQUIRED_ITEMS_COUNT[1] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[1]) >= REQUIRED_ITEMS_COUNT[1] * itemCount &&
						ch->CountSpecifyItem(REQUIRED_ITEMS[2]) >= REQUIRED_ITEMS_COUNT[1] * itemCount)
					{
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[0], REQUIRED_ITEMS_COUNT[1] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[1], REQUIRED_ITEMS_COUNT[1] * itemCount);
						ch->RemoveSpecifyItem(REQUIRED_ITEMS[2], REQUIRED_ITEMS_COUNT[1] * itemCount);

						ch->PointChange(POINT_CHEQUE, -price);
						item_grade2->SetSocket(0, mobVnum);
						item_grade2->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganych przedmiotów!");
					}
					break;

				default:
					ch->ChatPacket(CHAT_TYPE_INFO, "MARMUR_GRADE: ERROR");
					break;
			}
		}
		else
		{
			M2_DESTROY_ITEM(item);
			ch->ChatPacket(CHAT_TYPE_INFO, "Brak miejsca w ekwipunku!");
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Item creation failed.");
	}
}
#endif

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
	: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("¼Ë´Ù¿îÀÌ %dÃÊ ³²¾Ò½À´Ï´Ù."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("%dÃÊ ÈÄ °ÔÀÓÀÌ ¼Ë´Ù¿î µË´Ï´Ù."), iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}
	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d) // @fixme197
					d->DelayedDisconnect(1);
				break;

			case SCMD_PHASE_SELECT:
				ch->Disconnect("timed_event - SCMD_PHASE_SELECT");
				if (d)
					d->SetPhase(PHASE_SELECT);
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%dÃÊ ³²¾Ò½À´Ï´Ù."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ãë¼Ò µÇ¾ú½À´Ï´Ù."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("·Î±×ÀÎ È­¸éÀ¸·Î µ¹¾Æ °©´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°ÔÀÓÀ» Á¾·á ÇÕ´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ä³¸¯ÅÍ¸¦ ÀüÈ¯ ÇÕ´Ï´Ù. Àá½Ã¸¸ ±â´Ù¸®¼¼¿ä."));
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
		false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
	   	(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch		= ch;
				info->subcmd		= subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}
#define eFRS_HERESEC	170
			if (iTimeToDead > eFRS_HERESEC)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - eFRS_HERESEC);
				return;
			}
		}
	}

	//PREVENT_HACK

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
			   	false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%dÃÊ ³²À½)"), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

#define eFRS_TOWNSEC	173
		if (iTimeToDead > eFRS_TOWNSEC)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ ¸¶À»¿¡¼­ Àç½ÃÀÛ ÇÒ ¼ö ¾ø½À´Ï´Ù. (%d ÃÊ ³²À½)"), iTimeToDead - eFRS_TOWNSEC);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	//FORKED_LOAD

	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				ch->CheckMount();
#endif
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}

			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ºÁö¿¡¼­ ºÎÈ° ±âÈ¸¸¦ ¸ðµÎ ÀÒ¾ú½À´Ï´Ù! ¸¶À»·Î ÀÌµ¿ÇÕ´Ï´Ù!"));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
				ch->CheckMount();
#endif

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					ch->CheckMount();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
					ch->CheckMount();
#endif
					break;
			}

			return;
		}
	}
	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				if (ch->GetDungeon())
				{
					ch->WarpSet(1075900, 267800);
				} else {
					ch->WarpSet(pos.x, pos.y);
				}
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			ch->CheckMount();
#endif
			break;
	}
}

#ifdef ENABLE_SKILL_SELECT_FEATURE
ACMD(do_selectskill_open)
{
	if (!ch)
		return;
	
	if (ch->GetSkillGroup())
		return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "selectskill_open");
}

ACMD(do_selectskill_select)
{
	if (!ch)
		return;
	
	if (ch->GetSkillGroup())
		return;
	
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;
	
	BYTE skill_group = 0;
	str_to_number(skill_group, arg1);
	if (skill_group)
		ch->SetSkillGroup(skill_group);
		ch->ClearSkill();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("skill_path_selected"));
		ch->UpdatePacket();
}
#endif

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_val)
{
	char	arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int val = 0;
	str_to_number(val, arg2);
	
	if (!*arg1 || val <= 0)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Nie mo¿esz zmieniæ swojego stanu, gdy jesteœ przemieniony.");
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;
	
	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;
	
	if (val > ch->GetPoint(POINT_STAT))
	{
		val = ch->GetPoint(POINT_STAT);
	}
	
	if (ch->GetRealPoint(idx) + val > MAX_STAT)
	{
		val = MAX_STAT - ch->GetRealPoint(idx);
	}

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + val);
	ch->SetPoint(idx, ch->GetPoint(idx) + val);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -val);
	ch->ComputePoints();
}


ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â ´É·ÂÀ» ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
			return;

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
			return;

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
			return;

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
			return;

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
		return;

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µÐ°© Áß¿¡´Â ´É·ÂÀ» ¿Ã¸± ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -1);
	ch->ComputePoints();
}

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ó´ë¹æÀÌ ´ë·ÃÁßÀÔ´Ï´Ù."));
		return;
	}

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå¿¡ ¼ÓÇØÀÖÁö ¾Ê½À´Ï´Ù."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµå ½ºÅ³ ·¹º§À» º¯°æÇÒ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

//
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Àß¸øµÈ ¾ÏÈ£¸¦ ÀÔ·ÂÇÏ¼Ì½À´Ï´Ù."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í°¡ ÀÌ¹Ì ¿­·ÁÀÖ½À´Ï´Ù."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Ã¢°í> Ã¢°í¸¦ ´ÝÀºÁö 10ÃÊ ¾È¿¡´Â ¿­ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ¼­¹ö ¹®Á¦·Î ÆÄÆ¼ °ü·Ã Ã³¸®¸¦ ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ´øÀü ¾È¿¡¼­´Â ÆÄÆ¼¿¡¼­ ³ª°¥ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<ÆÄÆ¼> ÆÄÆ¼¿¡¼­ ³ª°¡¼Ì½À´Ï´Ù."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
ACMD(do_remove_polymorph)
{
	if (!ch)
		return;

	if (!ch->IsPolymorphed())
		return;

	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}
#endif

ACMD(do_remove_skill_affect)
{

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);
	
	CAffect* pAffect = ch->FindAffect(affect);
	
	if (affect == 3 || affect == 4 || affect == 19 || affect == 34
	|| affect == 49 || affect == 63 || affect == 64 || affect == 65
	|| affect == 79 || affect == 94 || affect == 95 || affect == 96
	|| affect == 110 || affect == 111 || affect == 510)

		if (pAffect)
			ch->RemoveAffect(affect);
	
}

// #ifdef ENABLE_SKILL_AFFECT_REMOVE
// ACMD(do_remove_skill_affect)
// {
	// char arg1[256];
	// one_argument(argument, arg1, sizeof(arg1));

	// if (!*arg1)
		// return;

	// DWORD affectvnum = 0;
	// str_to_number(affectvnum, arg1);

	// if (!ch->IsAffectFlag(affectvnum))
		// return;

	// if (affectvnum != AFF_JEONGWIHON && affectvnum != AFF_GEOMGYEONG 
		// && affectvnum != AFF_CHEONGEUN && affectvnum != AFF_GYEONGGONG 
		// && affectvnum != AFF_JEUNGRYEOK && affectvnum != AFF_GWIGUM
		// && affectvnum != AFF_TERROR && affectvnum != AFF_JUMAGAP
		// && affectvnum != AFF_MUYEONG && affectvnum != AFF_MANASHIELD
		// && affectvnum != AFF_HOSIN && affectvnum != AFF_BOHO
		// && affectvnum != AFF_GICHEON && affectvnum != AFF_KWAESOK
		// && affectvnum != AFF_JEUNGRYEOK && affectvnum != AFFECT_POLYMORPH
		// && affectvnum != AFFECT_MALL_EX && affectvnum != AFFECT_WATER
		// && affectvnum != AFFECT_BLEND_EX
// #ifdef ENABLE_WOLFMAN_CHARACTER
		// && affectvnum != AFF_BLUE_POSSESSION
// #endif
	// )
		// return;

	// CAffect* pAffect = ch->FindAffectByFlag(affectvnum);
	// if (!pAffect)
		// return;

	// ch->RemoveAffect(pAffect);
// }
// #endif

ACMD(do_war)
{
	CGuild * g = ch->GetGuild();

	if (!g)
		return;

	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ´Ù¸¥ ÀüÀï¿¡ ÂüÀü Áß ÀÔ´Ï´Ù."));
		return;
	}

	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
			type = GUILD_WAR_TYPE_FIELD;
	}

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀü¿¡ ´ëÇÑ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
		return;
	}

	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·± ±æµå°¡ ¾ø½À´Ï´Ù."));
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµå°¡ ÀÌ¹Ì ÀüÀï Áß ÀÔ´Ï´Ù."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> Àüºñ°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ Àüºñ°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ¼±ÀüÆ÷°í ÁßÀÎ ±æµåÀÔ´Ï´Ù."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµå°¡ ÀÌ¹Ì ÀüÀï Áß ÀÔ´Ï´Ù."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ÀüÀïÀÌ ¿¹¾àµÈ ±æµå ÀÔ´Ï´Ù."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ÀÌ¹Ì ÀüÀï ÁßÀÎ ±æµåÀÔ´Ï´Ù."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ·¹´õ Á¡¼ö°¡ ¸ðÀÚ¶ó¼­ ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀüÀ» ÇÏ±â À§ÇØ¼± ÃÖ¼ÒÇÑ %d¸íÀÌ ÀÖ¾î¾ß ÇÕ´Ï´Ù."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ·¹´õ Á¡¼ö°¡ ¸ðÀÚ¶ó¼­ ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµå¿ø ¼ö°¡ ºÎÁ·ÇÏ¿© ±æµåÀüÀ» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµåÀåÀÌ Á¢¼ÓÁßÀÌ ¾Æ´Õ´Ï´Ù."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> »ó´ë¹æ ±æµåÀÇ ±æµåÀåÀÌ Á¢¼ÓÁßÀÌ ¾Æ´Õ´Ï´Ù."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±æµåÀü¿¡ ´ëÇÑ ±ÇÇÑÀÌ ¾ø½À´Ï´Ù."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<±æµå> ±×·± ±æµå°¡ ¾ø½À´Ï´Ù."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀ¸·Î ºÎÅÍ Ä£±¸ µî·ÏÀ» °ÅºÎ ´çÇß½À´Ï´Ù."), ch->GetName());
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("ÀÎº¥Åä¸®°¡ ²Ë Â÷¼­ ³»¸± ¼ö ¾ø½À´Ï´Ù."));
	}
}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ë·ÃÀå¿¡¼­ »ç¿ëÇÏ½Ç ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÌ¹Ì ÆÄÆ¼¿¡ ¼ÓÇØ ÀÖÀ¸¹Ç·Î °¡ÀÔ½ÅÃ»À» ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

ACMD(do_monarch_warpto)
{
	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ë °¡´ÉÇÑ ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_WARP))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_WARP));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, WarpPrice);
		return;
	}

	int x = 0, y = 0;
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë¹ý: warpto <character name>"));
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("Å¸Á¦±¹ À¯Àú¿¡°Ô´Â ÀÌµ¿ÇÒ¼ö ¾ø½À´Ï´Ù"));
				return;
			}

			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç À¯Àú´Â %d Ã¤³Î¿¡ ÀÖ½À´Ï´Ù. (ÇöÀç Ã¤³Î %d)"), pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}

			PIXEL_POSITION pos;

			if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
			else
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, "You warp to (%d, %d)", pos.x, pos.y);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¿¡°Ô·Î ÀÌµ¿ÇÕ´Ï´Ù"), arg1);
				ch->WarpSet(pos.x, pos.y);

				CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

				ch->SetMC(CHARACTER::MI_WARP);
			}
		}
		else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
		}

		return;
	}
	else
	{
		if (tch->GetEmpire() != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Å¸Á¦±¹ À¯Àú¿¡°Ô´Â ÀÌµ¿ÇÒ¼ö ¾ø½À´Ï´Ù"));
			return;
		}
		if (!IsMonarchWarpZone(tch->GetMapIndex()))
		{
			ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
			return;
		}
		x = tch->GetX();
		y = tch->GetY();
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ¿¡°Ô·Î ÀÌµ¿ÇÕ´Ï´Ù"), arg1);
	ch->WarpSet(x, y);
	ch->Stop();

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_WARP);
}

ACMD(do_monarch_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("»ç¿ë¹ý: transfer <name>"));
		return;
	}

	if (!CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ë °¡´ÉÇÑ ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	if (!ch->IsMCOK(CHARACTER::MI_TRANSFER))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_TRANSFER));
		return;
	}

	const int WarpPrice = 10000;

	if (!CMonarch::instance().IsMoneyOk(WarpPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, WarpPrice);
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bEmpire != ch->GetEmpire())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ Á¦±¹ À¯Àú´Â ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀº %d Ã¤³Î¿¡ Á¢¼Ó Áß ÀÔ´Ï´Ù. (ÇöÀç Ã¤³Î: %d)"), arg1, pkCCI->bChannel, g_bChannel);
				return;
			}
			if (!IsMonarchWarpZone(pkCCI->lMapIndex))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}
			if (!IsMonarchWarpZone(ch->GetMapIndex()))
			{
				ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ´ÔÀ» ¼ÒÈ¯ÇÏ¿´½À´Ï´Ù."), arg1);

			CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

			ch->SetMC(CHARACTER::MI_TRANSFER);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÔ·ÂÇÏ½Å ÀÌ¸§À» °¡Áø »ç¿ëÀÚ°¡ ¾ø½À´Ï´Ù."));
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÚ½ÅÀ» ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	if (tch->GetEmpire() != ch->GetEmpire())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´Ù¸¥ Á¦±¹ À¯Àú´Â ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	if (!IsMonarchWarpZone(tch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}
	if (!IsMonarchWarpZone(ch->GetMapIndex()))
	{
		ch->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("ÇØ´ç Áö¿ªÀ¸·Î ¼ÒÈ¯ÇÒ ¼ö ¾ø½À´Ï´Ù."));
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());

	CMonarch::instance().SendtoDBDecMoney(WarpPrice, ch->GetEmpire(), ch);

	ch->SetMC(CHARACTER::MI_TRANSFER);
}

ACMD(do_monarch_info)
{
	if (CMonarch::instance().IsMonarch(ch->GetPlayerID(), ch->GetEmpire()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("³ªÀÇ ±ºÁÖ Á¤º¸"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			if (n == ch->GetEmpire())
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  º¸À¯±Ý¾× %lld "), EMPIRE_NAME(n), p->name[n], p->money[n]);
			else
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ Á¤º¸"));
		TMonarchInfo * p = CMonarch::instance().GetMonarch();
		for (int n = 1; n < 4; ++n)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[%s±ºÁÖ] : %s  "), EMPIRE_NAME(n), p->name[n]);

		}
	}
}

ACMD(do_elect)
{
	db_clientdesc->DBPacketHeader(HEADER_GD_COME_TO_VOTE, ch->GetDesc()->GetHandle(), 0);
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

ACMD(do_monarch_tax)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: monarch_tax <1-50>");
		return;
	}

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ëÇÒ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	int tax = 0;
	str_to_number(tax,  arg1);

	if (tax < 1 || tax > 50)
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1-50 »çÀÌÀÇ ¼öÄ¡¸¦ ¼±ÅÃÇØÁÖ¼¼¿ä"));

	quest::CQuestManager::instance().SetEventFlag("trade_tax", tax);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼¼±ÝÀÌ %d %·Î ¼³Á¤µÇ¾ú½À´Ï´Ù"));

	char szMsg[1024];

	snprintf(szMsg, sizeof(szMsg), "±ºÁÖÀÇ ¸íÀ¸·Î ¼¼±ÝÀÌ %d %% ·Î º¯°æµÇ¾ú½À´Ï´Ù", tax);
	BroadcastNotice(szMsg);

	snprintf(szMsg, sizeof(szMsg), "¾ÕÀ¸·Î´Â °Å·¡ ±Ý¾×ÀÇ %d %% °¡ ±¹°í·Î µé¾î°¡°ÔµË´Ï´Ù.", tax);
	BroadcastNotice(szMsg);

	ch->SetMC(CHARACTER::MI_TAX);
}

static const DWORD cs_dwMonarchMobVnums[] =
{
	191,
	192,
	193,
	194,
	391,
	392,
	393,
	394,
	491,
	492,
	493,
	494,
	591,
	691,
	791,
	1304,
	1901,
	2091,
	2191,
	2206,
	0,
};

ACMD(do_monarch_mob)
{
	char arg1[256];
	LPCHARACTER	tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (!ch->IsMonarch())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±ºÁÖ¸¸ÀÌ »ç¿ëÇÒ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mmob <mob name>");
		return;
	}

#ifdef ENABLE_MONARCH_MOB_CMD_MAP_CHECK // @warme006
	BYTE pcEmpire = ch->GetEmpire();
	BYTE mapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(ch->GetMapIndex());
	if (mapEmpire != pcEmpire && mapEmpire != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÀÚ±¹ ¿µÅä¿¡¼­¸¸ »ç¿ëÇÒ ¼ö ÀÖ´Â ±â´ÉÀÔ´Ï´Ù"));
		return;
	}
#endif

	const int SummonPrice = 5000000;

	if (!ch->IsMCOK(CHARACTER::MI_SUMMON))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ÃÊ°£ ÄðÅ¸ÀÓÀÌ Àû¿ëÁßÀÔ´Ï´Ù."), ch->GetMCLTime(CHARACTER::MI_SUMMON));
		return;
	}

	if (!CMonarch::instance().IsMoneyOk(SummonPrice, ch->GetEmpire()))
	{
		int NationMoney = CMonarch::instance().GetMoney(ch->GetEmpire());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("±¹°í¿¡ µ·ÀÌ ºÎÁ·ÇÕ´Ï´Ù. ÇöÀç : %u ÇÊ¿ä±Ý¾× : %u"), NationMoney, SummonPrice);
		return;
	}

	const CMob * pkMob;
	DWORD vnum = 0;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	DWORD count;

	for (count = 0; cs_dwMonarchMobVnums[count] != 0; ++count)
		if (cs_dwMonarchMobVnums[count] == vnum)
			break;

	if (0 == cs_dwMonarchMobVnums[count])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¼ÒÈ¯ÇÒ¼ö ¾ø´Â ¸ó½ºÅÍ ÀÔ´Ï´Ù. ¼ÒÈ¯°¡´ÉÇÑ ¸ó½ºÅÍ´Â È¨ÆäÀÌÁö¸¦ ÂüÁ¶ÇÏ¼¼¿ä"));
		return;
	}

	tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE,
			true);

	if (tch)
	{
		CMonarch::instance().SendtoDBDecMoney(SummonPrice, ch->GetEmpire(), ch);

		ch->SetMC(CHARACTER::MI_SUMMON);
	}
}

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:	return LC_TEXT("ÃÖ´ë »ý¸í·Â +%d");
		case POINT_MAX_SP:	return LC_TEXT("ÃÖ´ë Á¤½Å·Â +%d");
		case POINT_HT:		return LC_TEXT("Ã¼·Â +%d");
		case POINT_IQ:		return LC_TEXT("Áö´É +%d");
		case POINT_ST:		return LC_TEXT("±Ù·Â +%d");
		case POINT_DX:		return LC_TEXT("¹ÎÃ¸ +%d");
		case POINT_ATT_SPEED:	return LC_TEXT("°ø°Ý¼Óµµ +%d");
		case POINT_MOV_SPEED:	return LC_TEXT("ÀÌµ¿¼Óµµ %d");
		case POINT_CASTING_SPEED:	return LC_TEXT("ÄðÅ¸ÀÓ -%d");
		case POINT_HP_REGEN:	return LC_TEXT("»ý¸í·Â È¸º¹ +%d");
		case POINT_SP_REGEN:	return LC_TEXT("Á¤½Å·Â È¸º¹ +%d");
		case POINT_POISON_PCT:	return LC_TEXT("µ¶°ø°Ý %d");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:	return LC_TEXT("µ¶°ø°Ý %d");
#endif
		case POINT_STUN_PCT:	return LC_TEXT("½ºÅÏ +%d");
		case POINT_SLOW_PCT:	return LC_TEXT("½½·Î¿ì +%d");
		case POINT_CRITICAL_PCT:	return LC_TEXT("%d%% È®·ü·Î Ä¡¸íÅ¸ °ø°Ý");
		case POINT_RESIST_CRITICAL:	return LC_TEXT("»ó´ëÀÇ Ä¡¸íÅ¸ È®·ü %d%% °¨¼Ò");
		case POINT_PENETRATE_PCT:	return LC_TEXT("%d%% È®·ü·Î °üÅë °ø°Ý");
		case POINT_RESIST_PENETRATE: return LC_TEXT("»ó´ëÀÇ °üÅë °ø°Ý È®·ü %d%% °¨¼Ò");
		case POINT_ATTBONUS_HUMAN:	return LC_TEXT("ÀÎ°£·ù ¸ó½ºÅÍ Å¸°ÝÄ¡ +%d%%");
		case POINT_ATTBONUS_ANIMAL:	return LC_TEXT("µ¿¹°·ù ¸ó½ºÅÍ Å¸°ÝÄ¡ +%d%%");
		case POINT_ATTBONUS_ORC:	return LC_TEXT("¿õ±ÍÁ· Å¸°ÝÄ¡ +%d%%");
		case POINT_ATTBONUS_MILGYO:	return LC_TEXT("¹Ð±³·ù Å¸°ÝÄ¡ +%d%%");
		case POINT_ATTBONUS_UNDEAD:	return LC_TEXT("½ÃÃ¼·ù Å¸°ÝÄ¡ +%d%%");
		case POINT_ATTBONUS_DEVIL:	return LC_TEXT("¾Ç¸¶·ù Å¸°ÝÄ¡ +%d%%");
		case POINT_STEAL_HP:		return LC_TEXT("Å¸°ÝÄ¡ %d%% ¸¦ »ý¸í·ÂÀ¸·Î Èí¼ö");
		case POINT_STEAL_SP:		return LC_TEXT("Å¸·ÂÄ¡ %d%% ¸¦ Á¤½Å·ÂÀ¸·Î Èí¼ö");
		case POINT_MANA_BURN_PCT:	return LC_TEXT("%d%% È®·ü·Î Å¸°Ý½Ã »ó´ë Àü½Å·Â ¼Ò¸ð");
		case POINT_DAMAGE_SP_RECOVER:	return LC_TEXT("%d%% È®·ü·Î ÇÇÇØ½Ã Á¤½Å·Â È¸º¹");
		case POINT_BLOCK:			return LC_TEXT("¹°¸®Å¸°Ý½Ã ºí·° È®·ü %d%%");
		case POINT_DODGE:			return LC_TEXT("È° °ø°Ý È¸ÇÇ È®·ü %d%%");
		case POINT_RESIST_SWORD:	return LC_TEXT("ÇÑ¼Õ°Ë ¹æ¾î %d%%");
		case POINT_RESIST_TWOHAND:	return LC_TEXT("¾ç¼Õ°Ë ¹æ¾î %d%%");
		case POINT_RESIST_DAGGER:	return LC_TEXT("µÎ¼Õ°Ë ¹æ¾î %d%%");
		case POINT_RESIST_BELL:		return LC_TEXT("¹æ¿ï ¹æ¾î %d%%");
		case POINT_RESIST_FAN:		return LC_TEXT("ºÎÃ¤ ¹æ¾î %d%%");
		case POINT_RESIST_BOW:		return LC_TEXT("È°°ø°Ý ÀúÇ× %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:		return LC_TEXT("µÎ¼Õ°Ë ¹æ¾î %d%%");
#endif
		case POINT_RESIST_FIRE:		return LC_TEXT("È­¿° ÀúÇ× %d%%");
		case POINT_RESIST_ELEC:		return LC_TEXT("Àü±â ÀúÇ× %d%%");
		case POINT_RESIST_MAGIC:	return LC_TEXT("¸¶¹ý ÀúÇ× %d%%");
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:	return LC_TEXT("¸¶¹ý ÀúÇ× %d%%");
#endif
		case POINT_RESIST_WIND:		return LC_TEXT("¹Ù¶÷ ÀúÇ× %d%%");
		case POINT_RESIST_ICE:		return LC_TEXT("³Ã±â ÀúÇ× %d%%");
		case POINT_RESIST_EARTH:	return LC_TEXT("´ëÁö ÀúÇ× %d%%");
		case POINT_RESIST_DARK:		return LC_TEXT("¾îµÒ ÀúÇ× %d%%");
		case POINT_REFLECT_MELEE:	return LC_TEXT("Á÷Á¢ Å¸°ÝÄ¡ ¹Ý»ç È®·ü : %d%%");
		case POINT_REFLECT_CURSE:	return LC_TEXT("ÀúÁÖ µÇµ¹¸®±â È®·ü %d%%");
		case POINT_POISON_REDUCE:	return LC_TEXT("µ¶ ÀúÇ× %d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:	return LC_TEXT("µ¶ ÀúÇ× %d%%");
#endif
		case POINT_KILL_SP_RECOVER:	return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã Á¤½Å·Â È¸º¹");
		case POINT_EXP_DOUBLE_BONUS:	return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã °æÇèÄ¡ Ãß°¡ »ó½Â");
		case POINT_GOLD_DOUBLE_BONUS:	return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã µ· 2¹è µå·Ó");
		case POINT_ITEM_DROP_BONUS:	return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã ¾ÆÀÌÅÛ 2¹è µå·Ó");
		case POINT_POTION_BONUS:	return LC_TEXT("¹°¾à »ç¿ë½Ã %d%% ¼º´É Áõ°¡");
		case POINT_KILL_HP_RECOVERY:	return LC_TEXT("%d%% È®·ü·Î ÀûÅðÄ¡½Ã »ý¸í·Â È¸º¹");
		case POINT_ATT_GRADE_BONUS:	return LC_TEXT("°ø°Ý·Â +%d");
		case POINT_DEF_GRADE_BONUS:	return LC_TEXT("¹æ¾î·Â +%d");
		case POINT_MAGIC_ATT_GRADE:	return LC_TEXT("¸¶¹ý °ø°Ý·Â +%d");
		case POINT_MAGIC_DEF_GRADE:	return LC_TEXT("¸¶¹ý ¹æ¾î·Â +%d");
		case POINT_MAX_STAMINA:	return LC_TEXT("ÃÖ´ë Áö±¸·Â +%d");
		case POINT_ATTBONUS_WARRIOR:	return LC_TEXT("¹«»ç¿¡°Ô °­ÇÔ +%d%%");
		case POINT_ATTBONUS_ASSASSIN:	return LC_TEXT("ÀÚ°´¿¡°Ô °­ÇÔ +%d%%");
		case POINT_ATTBONUS_SURA:		return LC_TEXT("¼ö¶ó¿¡°Ô °­ÇÔ +%d%%");
		case POINT_ATTBONUS_SHAMAN:		return LC_TEXT("¹«´ç¿¡°Ô °­ÇÔ +%d%%");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:	return LC_TEXT("¹«´ç¿¡°Ô °­ÇÔ +%d%%");
#endif
		case POINT_ATTBONUS_MONSTER:	return LC_TEXT("¸ó½ºÅÍ¿¡°Ô °­ÇÔ +%d%%");
		case POINT_MALL_ATTBONUS:		return LC_TEXT("°ø°Ý·Â +%d%%");
		case POINT_MALL_DEFBONUS:		return LC_TEXT("¹æ¾î·Â +%d%%");
		case POINT_MALL_EXPBONUS:		return LC_TEXT("°æÇèÄ¡ %d%%");
		case POINT_MALL_ITEMBONUS:		return LC_TEXT("¾ÆÀÌÅÛ µå·ÓÀ² %d¹è"); // @fixme180 float to int
		case POINT_MALL_GOLDBONUS:		return LC_TEXT("µ· µå·ÓÀ² %d¹è"); // @fixme180 float to int
		case POINT_MAX_HP_PCT:			return LC_TEXT("ÃÖ´ë »ý¸í·Â +%d%%");
		case POINT_MAX_SP_PCT:			return LC_TEXT("ÃÖ´ë Á¤½Å·Â +%d%%");
		case POINT_SKILL_DAMAGE_BONUS:	return LC_TEXT("½ºÅ³ µ¥¹ÌÁö %d%%");
		case POINT_NORMAL_HIT_DAMAGE_BONUS:	return LC_TEXT("ÆòÅ¸ µ¥¹ÌÁö %d%%");
		case POINT_SKILL_DEFEND_BONUS:		return LC_TEXT("½ºÅ³ µ¥¹ÌÁö ÀúÇ× %d%%");
		case POINT_NORMAL_HIT_DEFEND_BONUS:	return LC_TEXT("ÆòÅ¸ µ¥¹ÌÁö ÀúÇ× %d%%");
		case POINT_RESIST_WARRIOR:	return LC_TEXT("¹«»ç°ø°Ý¿¡ %d%% ÀúÇ×");
		case POINT_RESIST_ASSASSIN:	return LC_TEXT("ÀÚ°´°ø°Ý¿¡ %d%% ÀúÇ×");
		case POINT_RESIST_SURA:		return LC_TEXT("¼ö¶ó°ø°Ý¿¡ %d%% ÀúÇ×");
		case POINT_RESIST_SHAMAN:	return LC_TEXT("¹«´ç°ø°Ý¿¡ %d%% ÀúÇ×");
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:	return LC_TEXT("¹«´ç°ø°Ý¿¡ %d%% ÀúÇ×");
#endif
		default:					return "UNK_ID %d%%"; // @fixme180
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT(" (¸¸·áÀÏ : %d³â %d¿ù %dÀÏ)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
	char buf[1024]; // @warme015
	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	CItem* pMount = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CItem* pAcce = ch->GetWear(WEAR_COSTUME_ACCE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
			ch->UnequipItem(pHair);
	}

	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
			ch->UnequipItem(pBody);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_NEW_MOUNT_SYSTEM
	if (pMount)
	{
		const char* itemName = pMount->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  MOUNT : %s", itemName);

		if (pMount->IsEquipped() && arg1[0] == 'm')
			ch->UnequipItem(pMount);
	}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (pAcce)
	{
		const char* itemName = pAcce->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  ACCE : %s", itemName);

		if (pAcce->IsEquipped() && arg1[0] == 'a')
			ch->UnequipItem(pAcce);
	}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  WEAPON : %s", itemName);

		if (pWeapon->IsEquipped() && arg1[0] == 'w')
			ch->UnequipItem(pWeapon);
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
						index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
ACMD(do_cube)
{

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		default:
			return;
	}
}
#else
ACMD(do_cube)
{
	if (!ch->CanDoCube())
		return;

	sys_log(1, "CUBE COMMAND <%s>: %s", ch->GetName(), argument);
	int cube_index = 0, inven_index = 0;
	const char *line;

	char arg1[256], arg2[256], arg3[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
		// print usage
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
		return;
	}

	const std::string& strArg1 = std::string(arg1);

	// r_info (request information)

	//					    (Server -> Client) /cube r_list npcVNUM resultCOUNT 123,1/125,1/128,1/130,5

	//					   (Server -> Client) /cube m_info startIndex count 125,1|126,2|127,2|123,5&555,5&555,4/120000@125,1|126,2|127,2|123,5&555,5&555,4/120000

	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}

	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			Cube_open(ch);
			break;

		case 'c':	// close
			Cube_close(ch);
			break;

		case 'l':	// list
			Cube_show_list(ch);
			break;

		case 'a':	// add cue_index inven_index
			{
				if (0 == arg2[0] || !isdigit(*arg2) ||
					0 == arg3[0] || !isdigit(*arg3))
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
				Cube_add_item (ch, cube_index, inven_index);
			}
			break;

		case 'd':	// delete
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':	// make
			if (0 != arg2[0])
			{
				while (true == Cube_make(ch))
					sys_log(1, "cube make success");
			}
			else
				Cube_make(ch);
			break;

		default:
			return;
	}
}
#endif

#ifdef ENABLE_SYSTEMY_KARTY_OKEY
ACMD(do_cards)
{
	const char *line;

	char arg1[256], arg2[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			if (isdigit(*arg2))
			{
				DWORD safemode;
				str_to_number(safemode, arg2);
				ch->Cards_open(safemode);
			}
			break;
		case 'p':	// open
			ch->Cards_pullout();
			break;
		case 'e':	// open
			ch->CardsEnd();
			break;
		case 'd':	// open
			if (isdigit(*arg2))
			{
				DWORD destroy_index;
				str_to_number(destroy_index, arg2);
				ch->CardsDestroy(destroy_index);
			}
			break;
		case 'a':	// open
			if (isdigit(*arg2))
			{
				DWORD accpet_index;
				str_to_number(accpet_index, arg2);
				ch->CardsAccept(accpet_index);
			}
			break;
		case 'r':	// open
			if (isdigit(*arg2))
			{
				DWORD restore_index;
				str_to_number(restore_index, arg2);
				ch->CardsRestore(restore_index);
			}
			break;
		default:
			return;
	}
}
#endif

ACMD(do_in_game_mall)
{
	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
				if (test_server == true)
				{
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
				}
				break;
		}

		char buf[512+1];
		char sas[33];
		MD5_CTX ctx;
		const char sas_key[] = "GF9001";

		snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
		MD5End(&ctx, sas);
#else
		static const char hex[] = "0123456789abcdef";
		unsigned char digest[16];
		MD5Final(digest, &ctx);
		int i;
		for (i = 0; i < 16; ++i) {
			sas[i+i] = hex[digest[i] >> 4];
			sas[i+i+1] = hex[digest[i] & 0x0f];
		}
		sas[i+i] = '\0';
#endif

		snprintf(buf, sizeof(buf), "mall http://%s/ishop?pid=%u&c=%s&sid=%d&sas=%s",
				g_strWebMallURL.c_str(), ch->GetPlayerID(), country_code, g_server_id, sas);

		ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
	}
}

ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#ifdef ENABLE_DICE_SYSTEM
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, LC_TEXT("%s´ÔÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("´ç½ÅÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), n, start, end);
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_TEXT("%s´ÔÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("´ç½ÅÀÌ ÁÖ»çÀ§¸¦ ±¼·Á %d°¡ ³ª¿Ô½À´Ï´Ù. (%d-%d)"), n, start, end);
#endif
}

#ifdef ENABLE_NEWSTUFF
ACMD(do_click_safebox)
{
	if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon() || ch->GetWarMap()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the safebox in dungeon or at war."));
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc=DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
		return;
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

#ifdef ENABLE_NEW_PET_SYSTEM
ACMD(do_pet_skillup)
{
	if (ch->IsObserverMode() || ch->GetExchange())
	{
		return;
	}

	LPITEM pet_item = ch->GetWear(WEAR_NEW_PET);
	if (!pet_item)
	{
		return;
	}

	int skill_id = 0;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	str_to_number(skill_id, arg1);
	ch->PetSkillUp(skill_id);
}

ACMD(do_pet_name)
{
	if (ch->IsObserverMode() || ch->GetExchange() || !ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need wait 10 sec.."));
		return;
	}

	char arg1[CHARACTER_NAME_MAX_LEN + 1];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	ch->PetName(arg1);
}
#endif

ACMD(do_ride)
{
	sys_log(1, "[DO_RIDE] start");
	if (ch->IsDead() || ch->IsStun())
		return;

#ifdef __RENEWAL_MOUNT__
	if (ch->MountBlockMap())
		return;
#endif

	{
		if (ch->IsHorseRiding())
		{
			sys_log(1, "[DO_RIDE] stop riding");
			ch->StopRiding();
			return;
		}

		if (ch->GetMountVnum())
		{
			sys_log(1, "[DO_RIDE] unmount");
			do_unmount(ch, NULL, 0, 0);
			return;
		}
	}

#ifdef __RENEWAL_MOUNT__
	if (ch->GetWear(WEAR_COSTUME_MOUNT))
	{
		ch->StartRiding();
		return;
	}
	else
#endif
	{
		if (ch->GetHorse() != NULL)
		{
			sys_log(1, "[DO_RIDE] start riding");
			ch->StartRiding();
			return;
		}

		for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = ch->GetInventoryItem(i);
			if (NULL == item)
				continue;
#ifdef __RENEWAL_MOUNT__
			else if (item->IsCostumeMountItem())
			{
				ch->EquipItem(item);
				return;
			}
#endif
			else if (item->IsRideItem())
			{
				ch->EquipItem(item);
				return;
			}
		}
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¸»À» ¸ÕÀú ¼ÒÈ¯ÇØÁÖ¼¼¿ä."));
}


#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
ACMD(do_open_offline_shop)
{
	// If character is dead, return false
	if (ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ISDEAD"));
		return;
	}

	// If character is exchanging with someone, return false
	if (ch->GetExchange())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_EXCHANGE"));
		return;
	}

	// If character has a private shop, return false
	if (ch->GetMyShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_PRIVATE_SHOP_IS_OPEN"));
		return;
	}

	// If character is look at one offline shop, return false
	if (ch->GetOfflineShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_OFFLINE_SHOP_IS_OPEN"));
		return;
	}

	// If cube window is open, return false
	if (ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_PRIVATE_CUBE_IS_OPEN"));
		return;
	}

	// If character is a gm and this server is not test server, return false
	if (ch->GetGMLevel() > GM_LOW_WIZARD && ch->GetGMLevel() < GM_IMPLEMENTOR)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ADMINISTRATOR"));
		return;
	}

	// If character's safebox is open, return false
	if (ch->IsOpenSafebox())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_PRIVATE_DEPO_IS_OPEN"));
		return;
	}

	// If character's shop window is open, return false
	if (ch->GetShop())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_PRIVATE_SHOP_IS_OPEN"));
		return;
	}

	// Send the command to client.
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShop");
}

ACMD(do_offline_shop_open_panel)
{
	// if (ch->GetLastTimePanelOpened() + 2000 > get_dword_time())
	// {
	// 	ch->ChatPacket(CHAT_TYPE_INFO, "You have to wait 2 seconds before opening the pannel.");
	// 	return;
	// }
	
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT time, x, y, mapIndex, channel, sign FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

	if (pMsg->Get()->uiNumRows)
	{
		DWORD index = 0, pos_x = 0, pos_y = 0, time = 0, channel = 0;
		str_to_number(time, row[0]);
		str_to_number(pos_x, row[1]);
		str_to_number(pos_y, row[2]);
		str_to_number(index, row[3]);
		str_to_number(channel, row[4]);
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopPanel %d:%d:%d:%d:%d", time, pos_x, pos_y, index, channel);
	}
	else
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopPanel 0:0:0:0:0");
	
	// ch->SetLastTimePanelOpened(get_dword_time());
}
ACMD(do_offline_shop_open_logs)
{
	if (!PulseManager::Instance().IncreaseClock(ch->GetPlayerID(), ePulse::OfflineLogs, std::chrono::milliseconds(8000))) {
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %.2f"), PULSEMANAGER_CLOCK_TO_SEC2(ch->GetPlayerID(), ePulse::OfflineLogs));
		return;
	}
	ch->SendLogs();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenOfflineShopLogs");
}
#endif

// ACMD(do_fast_opener)
// {

// 	std::vector<std::string> vecArgs;
// 	split_argument(argument, vecArgs);
// 	int itemSlot;
// 	str_to_number(itemSlot, vecArgs[1].c_str());

// 	if (!*vecArgs[1].c_str() || !isnhdigit(*vecArgs[1].c_str()))
// 	{
// 		return ;
// 	}

// 	if (!ch->CanWarp())
// 	{
// 		return ;
// 	}

// 	LPITEM item = ch->GetInventoryItem(itemSlot);

// 	for (int i = 0; i <= 200; i++)
// 	{
// 		ch->UseItem(TItemPos (INVENTORY, itemSlot));
// 	}
// }

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
ACMD(do_add_collect_item)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	char arg1[256];
	char arg2[256];
	char arg3[256];

	const char* line;
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	if (!ch->CanDoAction()) // Use if (!ch->CanWarp()) -> if u havent got CanDoAction function.
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Odczekaj chwilê!");
		return;
	}

	BYTE collectID = std::atoi(arg1);
	BYTE itemID = std::atoi(arg2);
	BYTE isAll = std::atoi(arg3);
	CSystemCollections::instance().AddItem(ch, collectID, itemID, (isAll == 1) ? true : false);
}
#endif

#ifdef ENABLE_EVENT_MANAGER
ACMD(do_event_manager)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "info")
	{
		CHARACTER_MANAGER::Instance().SendDataPlayer(ch);
	}
	else if (vecArgs[1] == "remove")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 3) { 
			
			ch->ChatPacket(CHAT_TYPE_INFO, "put the event index!!");
			return; 
		}

		BYTE removeIndex;
		str_to_number(removeIndex, vecArgs[2].c_str());

		if(CHARACTER_MANAGER::Instance().CloseEventManuel(removeIndex))
			ch->ChatPacket(CHAT_TYPE_INFO, "successfuly remove!");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "dont has any event!");
	}
	else if (vecArgs[1] == "update")
	{
		if (!ch->IsGM())
			return;
		const BYTE subHeader = EVENT_MANAGER_UPDATE;
		//db_clientdesc->DBPacketHeader(HEADER_GD_EVENT_MANAGER, 0, sizeof(BYTE));
		//db_clientdesc->Packet(&subHeader, sizeof(BYTE));
		db_clientdesc->DBPacket(HEADER_GD_EVENT_MANAGER, 0, &subHeader, sizeof(BYTE));

		ch->ChatPacket(CHAT_TYPE_INFO, "successfully update!");
	}
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__

static const std::set<int> ALLOWED_SHOP_ZONES = { 1, 2 };

bool IsAllowedShopZone(int nMapIndex)
{
    return ALLOWED_SHOP_ZONES.count(nMapIndex) > 0;
}

ACMD(do_register_off_shop)
{
    if (IsAllowedShopZone(ch->GetMapIndex()))
    {
#ifdef __PREMIUM_PRIVATE_SHOP__
		if (ch->IsPrivateShopOwner())
		{
			if (ch->GetPrivateShopTable()->llGold > 0 || ch->GetPrivateShopTable()->dwCheque > 0)
			{
				ch->OpenPrivateShopPanel();
				return;
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close your current personal shop before opening a new one."));
				return;
			}
		}

		ch->OpenPrivateShopPanel();
#else
		ch->__OpenPrivateShop();
#endif
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YYou cannot open a private shop in this map."));
	}
}
#endif

ACMD(do_refund_items)
{
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		
		int bResult = 0;
		str_to_number(bResult, row[0]);
		
		if (bResult)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_BACK_ITEM"));
			char szQuery[2048];
			snprintf(szQuery, sizeof(szQuery), "SELECT pos,count,vnum,type,subtype,"
												"socket0,socket1,socket2,socket3,socket4,socket5, attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6 FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), ch->GetPlayerID());

			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			
			if (pMsg->Get()->uiNumRows == 0)
				return;
			
			MYSQL_ROW row;
			while (NULL != (row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				int cur = 0;
				
				TPlayerItem item;
				str_to_number(item.pos, row[cur++]);
				str_to_number(item.count, row[cur++]);
				str_to_number(item.vnum, row[cur++]);

				TItemTable* proto = ITEM_MANAGER::instance().GetTable(item.vnum);

				str_to_number(proto->bType, row[cur++]);
				str_to_number(proto->bSubType, row[cur++]);
				
				// Set Sockets
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					str_to_number(item.alSockets[i], row[cur++]);
				// End Of Set Sockets

				// Set Attributes
				for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
				{
					str_to_number(item.aAttr[i].bType, row[cur++]);
					str_to_number(item.aAttr[i].sValue, row[cur++]);
				}
				// End Of Set Attributes

				LPITEM pItem = ITEM_MANAGER::instance().CreateItem(item.vnum, item.count);
				if (pItem)
				{
					int iEmptyPos = 0;
					
					if (pItem->IsDragonSoul())
						iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
		#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					else if (pItem->IsSkillBook())
						iEmptyPos = ch->GetEmptySkillBookInventory(pItem->GetSize());
					else if (pItem->IsUpgradeItem())
						iEmptyPos = ch->GetEmptyUpgradeItemsInventory(pItem->GetSize());
					else if (pItem->IsStone())
						iEmptyPos = ch->GetEmptyStoneInventory(pItem->GetSize());
					else if (pItem->IsBox())
						iEmptyPos = ch->GetEmptyBoxInventory(pItem->GetSize());
					else if (pItem->IsEfsun())
						iEmptyPos = ch->GetEmptyEfsunInventory(pItem->GetSize());
					else if (pItem->IsCicek())
						iEmptyPos = ch->GetEmptyCicekInventory(pItem->GetSize());
		#endif
					else
						iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());

					if (iEmptyPos < 0)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_EQ_FULL"));
						return;
					}
					
					pItem->SetSockets(item.alSockets);
					pItem->SetAttributes(item.aAttr);
					if (pItem->IsDragonSoul())
						pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
		#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					else if (pItem->IsSkillBook())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsUpgradeItem())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsStone())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsBox())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsEfsun())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
					else if (pItem->IsCicek())
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		#endif
					else
						pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));

					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_GIVE_BACK"), pItem->GetName());
					DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_item WHERE owner_id = %u and pos = %d", get_table_postfix(), ch->GetPlayerID(), item.pos);
				}
			}	
		}
	}
}
