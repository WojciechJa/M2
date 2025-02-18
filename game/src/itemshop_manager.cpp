#include "stdafx.h"

#ifdef __ITEM_SHOP__
#include "desc_client.h"
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "item.h"
#include "log.h"
#include "config.h"
#include "constants.h"
#include "db.h"
#include <ctime>
#include "buffer_manager.h"
#include "desc_manager.h"
#include "itemshop_manager.h"

CItemShopManager::CItemShopManager()
{
	m_expireTime = ITEMSHOP_STATS_FLUSH_TIME;
	m_lastFlushTime = time(nullptr);
	m_bNeedFlush = false;
	m_nextUpdate = time(nullptr);
	m_bNeedUpdate = false;
	m_ttDataTime = 0;
}

CItemShopManager::~CItemShopManager()
{
	Destroy();
}

void CItemShopManager::Destroy()
{
	FlushStatistics();

	for (const auto& it : m_vec_itemShopItems)
		delete it;
	
	m_vec_itemShopItems.clear();
	m_vec_pkEditors.clear();	
	m_set_pkCurrentViewer.clear();
	m_vec_itemShopBuyStats.clear();
	m_vec_itemShopBuyStatsSave.clear();

	m_expireTime = 0;
	m_lastFlushTime = 0;
	m_bNeedFlush = false;
	m_ttDataTime = 0;
}

void CItemShopManager::Initialize(TItemShopRefreshPacket* pItem)
{
	if (pItem)
		sys_log(0, "CItemShopManager::Initialize with item id %d, category %d, flag %d", pItem->id, pItem->category, pItem->flag);
	else
		sys_log(0, "CItemShopManager::Initialize with null item");

	// create vector with old items for popup info
	std::vector<TItemShopItem*> vec_oldItem = m_vec_itemShopItems;
	std::vector<TItemShopItem*> vec_newItem;

	auto clear_vector = [](std::vector<TItemShopItem*>& vec) mutable
	{
		for (auto it : vec)
			delete it;
	};

	// if pItem is nullptr we clear all the data, reload happens
	if (!pItem)
	{		
		if (!m_vec_itemShopItems.empty())
			m_vec_itemShopItems.clear();

		if (!m_vec_pkEditors.empty())
			m_vec_pkEditors.clear();

		if (!m_map_itemShopCategory.empty())
			m_map_itemShopCategory.clear();
	}

	{
		// load category
		// execute query
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT `id`, `name` FROM `itemshop_category%s`", get_table_postfix()));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to load itemshop category, error code %d", pMsg->uiSQLErrno);
			clear_vector(vec_oldItem);
			return;
		}

		// iterate all data we get from database
		if (pMsg->Get())
		{
			while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
			{
				// prepare pointer, copy and add to map
				BYTE byIndex = std::atoi(row[0]);
				std::string stName = row[1];

				sys_log(0, "ITEMSHOP category index [%d] name [%s]", byIndex, row[1]);

				m_map_itemShopCategory.insert(std::make_pair(byIndex, stName));
			}
		}
	}

	// let's create query for items
	char szQuery[512 + 128] = "SELECT `id`, `category`, `price`, `vnum`, `count`, `bonus_type`-1, ";
	auto len = strlen(szQuery);

	for (auto i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		len += snprintf(szQuery + len, sizeof(szQuery) - len,
			"`socket%d`, ", i);

	for (auto i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
		len += snprintf(szQuery + len, sizeof(szQuery) - len,
			"`attrtype%d`, `attrvalue%d`, ", i, i);

	len += snprintf(szQuery + len, sizeof(szQuery) - len,
		"`is_vip`, `fixed_count`, `additional_time`, UNIX_TIMESTAMP(`add_time`), `discount`, UNIX_TIMESTAMP(`discount_time`), " 
		"UNIX_TIMESTAMP(`limited_time`), `limited_count`, `limited_buy_count`, `limited_buy_time` "
		"FROM `itemshop_items%s` ", get_table_postfix());

	// when we add item from another game get only that certain item
	if (pItem)
		len += snprintf(szQuery + len, sizeof(szQuery) - len,
			"WHERE `id` = %d AND `category` = %d ", pItem->id, pItem->category);

	{
		// execute query
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to load itemshop items, error code %d", pMsg->uiSQLErrno);
			clear_vector(vec_oldItem);
			return;
		}

		// iterate all data we get from database
		if (pMsg->Get())
		{
			while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
			{
				// prepare pointers
				auto col = 0;
				auto item = new TItemShopItem;

				// copy data to item
				str_to_number(item->id, row[col++]);
				str_to_number(item->category, row[col++]);
				str_to_number(item->price, row[col++]);
				str_to_number(item->vnum, row[col++]);
				str_to_number(item->count, row[col++]);

				str_to_number(item->bonus_type, row[col++]);

				if (item->bonus_type >= BONUS_MAX_NUM)
				{
					sys_err("Wrong bonus type %d [BONUS_MAX_NUM %d]", item->bonus_type, BONUS_MAX_NUM);
					delete item;
					continue;
				}

				for (auto i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
					str_to_number(item->alSockets[i], row[col++]);

				for (auto i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
				{
					str_to_number(item->aAttr[i].bType, row[col++]);
					str_to_number(item->aAttr[i].sValue, row[col++]);
				}

				str_to_number(item->is_vip, row[col++]);
				str_to_number(item->fixed_count, row[col++]);
				str_to_number(item->additional_time, row[col++]);
				str_to_number(item->add_time, row[col++]);
				str_to_number(item->discount, row[col++]);
				str_to_number(item->discount_time, row[col++]);

				str_to_number(item->limited_time, row[col++]);
				str_to_number(item->limited_count, row[col++]);
				str_to_number(item->limited_buy_count, row[col++]);

				str_to_number(item->limited_buy_time, row[col++]);

				// check if correct premium
				if (item->is_vip)
				{
					switch (item->vnum)
					{
					case PREMIUM_EXP:
					case PREMIUM_ITEM:
					case PREMIUM_SAFEBOX:
					case PREMIUM_AUTOLOOT:
					case PREMIUM_FISH_MIND:
					case PREMIUM_MARRIAGE_FAST:
					case PREMIUM_GOLD:
						break;
					default:
						sys_err("Unknown premium type %d", item->vnum);
						delete item;
						continue;
					}
				}
				// check if correct item
				else
				{
					const auto tItem = ITEM_MANAGER::instance().GetTable(item->vnum);
					if (!tItem)
					{
						sys_err("Cannot create item id [%d], category [%d], vnum [%d], count [%d], price [%d] ");
						delete item;
						continue;
					}
				}

				sys_log(0, "ITEMSHOP item id [%d], category [%d], vnum [%d], count [%d], price [%d], is vip [%d]", item->id, item->category, item->vnum, item->count, item->price, item->is_vip);

				time_t time = std::time(nullptr);

				// when we get item from other game, update vector with items and send data to all viewers
				if (pItem)
				{
					switch (pItem->flag)
					{
					case EDITOR_FLAG_ADD:
					{
						SendItem(item, ITEMSHOP_SUBHEADER_GC_ITEM);
						m_vec_itemShopItems.push_back(item);

						// send popup dialog to all connected players
						TItemShopPopupPacket pack{};
						pack.type = ITEMSHOP_POPUP_NEW_ITEM;

						if (item->discount && item->discount_time >= time)
							pack.type = ITEMSHOP_POPUP_ITEM_PROMO;

						if (item->limited_count || item->limited_time)
							pack.type = ITEMSHOP_POPUP_ITEM_LIMITED;

						pack.id = pItem->id;

						ClientPacket(ITEMSHOP_SUBHEADER_GC_POPUP, &pack, sizeof(TItemShopPopupPacket), nullptr, true);
					}
					break;
					case EDITOR_FLAG_EDIT:
					{
						SendItem(item, ITEMSHOP_SUBHEADER_GC_UPDATE);
						std::replace_if(m_vec_itemShopItems.begin(), m_vec_itemShopItems.end(),
							[&](TItemShopItem* it) -> bool
							{
								return it->id == pItem->id && it->category == pItem->category;
							},
							item);
					}
					break;
					case EDITOR_FLAG_DELETE:
					{
						TItemShopItemDeletePacket pack;
						pack.id = pItem->id;
						pack.category = pItem->category;

						ClientPacket(ITEMSHOP_SUBHEADER_GC_DELETE, &pack, sizeof(TItemShopItemDeletePacket));

						const auto it = std::remove_if(m_vec_itemShopItems.begin(), m_vec_itemShopItems.end(), [&](TItemShopItem* it) -> bool {
							if (it->id == pItem->id && it->category == pItem->category) {
								delete it;
								return true;
							}
							return false;
							});
						m_vec_itemShopItems.erase(it, m_vec_itemShopItems.end());

					}
					break;
					default:
						sys_err("unknown refresh flag %d", pItem->flag);
						break;
					}
				}
				else
				{
					m_vec_itemShopItems.push_back(item);
				}
			}
		}
	}

	m_ttDataTime = time(nullptr);

	if (pItem)
	{
		clear_vector(vec_oldItem);
		return;
	}

	{
		// load editors on normal initialization
		// execute query
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT `name` FROM `itemshop_editors%s` ", get_table_postfix()));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to load itemshop editors, error code %d", pMsg->uiSQLErrno);
			clear_vector(vec_oldItem);
			return;
		}

		// iterate all data we get from database
		if (pMsg->Get())
		{
			while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
			{
				sys_log(0, "ITEMSHOP editor name [%s]", row[0]);

				// prepare pointer, copy and add to vector
				std::string szName = row[0];
				std::for_each(szName.begin(), szName.end(), [](char& c) { c = tolower(c); });

				m_vec_pkEditors.push_back(szName);
			}
		}
	}
	for (const auto& viewer : m_set_pkCurrentViewer)
	{
		RemoveViewer(viewer);

		if (!viewer || !viewer->IsPC())
			continue;

		OpenItemShop(viewer, 0, true);
		viewer->m_bHasWheelData = false;
		CTombola::instance().RequestOpen(viewer);
	}

	InitializeStatistics();

	for (const auto& new_item : m_vec_itemShopItems)
	{
		bool add = true;
		for (const auto& old_item : vec_oldItem)
		{
			if (old_item->id == new_item->id)
			{
				if (old_item->discount == new_item->discount && old_item->discount_time == new_item->discount_time
					&& old_item->limited_count == new_item->limited_count && old_item->limited_time == new_item->limited_time)
				{
					add = false;
					break;
				}
			}
		}

		if (add)
			vec_newItem.push_back(new_item);
	}

	if (!vec_newItem.empty())
	{
		time_t time = std::time(nullptr);

		// send popup dialog to all connected players
		TItemShopPopupPacket pack{};
		pack.type = ITEMSHOP_POPUP_NEW_ITEM;

		// iterate newly added items
		for (const auto& item : vec_newItem)
		{
			sys_log(0, "item id %d, discount %d, discount_time %d, limited_count %d, limited_time %d", item->id, item->discount, item->discount_time, item->limited_count, item->limited_time);

			if (item->discount && item->discount_time >= time)
				pack.type = ITEMSHOP_POPUP_ITEM_PROMO;

			if (item->limited_count || item->limited_time)
				pack.type = ITEMSHOP_POPUP_ITEM_LIMITED;

			pack.id = item->id;
			pack.category = item->category;
		}

		if (vec_newItem.size() > 1)
			pack.id = 0;

		ClientPacket(ITEMSHOP_SUBHEADER_GC_POPUP, &pack, sizeof(TItemShopPopupPacket), nullptr, true);
	}

	clear_vector(vec_oldItem);
}

TItemShopItem* CItemShopManager::FindItemByID(DWORD id, DWORD category, BYTE bonus_type)
{
	const auto item = std::find_if(m_vec_itemShopItems.begin(), m_vec_itemShopItems.end(), 
		[&](TItemShopItem* it) -> bool
		{
			if (bonus_type > BONUS_NONE)
				return it->id == id && it->category == category && it->bonus_type == bonus_type;
			else
				return it->id == id && it->category == category;
		}
	);

	return (item != m_vec_itemShopItems.end()) ? (*item) : nullptr;
}

void CItemShopManager::OpenItemShop(LPCHARACTER ch, time_t time, bool reload)
{
	// check if player is editor, send data
	BYTE isEditor = IsEditor(ch) ? 1 : 0;
	ClientPacket(ITEMSHOP_SUBHEADER_GC_EDITOR, &isEditor, sizeof(BYTE), ch);

	// check if player already opened shop, if not, send all items
	if (!IsViewer(ch) && m_ttDataTime > time)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_CLEAR, nullptr, 0, ch);

		if (!m_map_itemShopCategory.empty())
		{
			for (const auto& it : m_map_itemShopCategory)
			{
				TItemShopCategoryPacket pack{};
				pack.index = it.first;
				strncpy(pack.szName, it.second.c_str(), CATEGORY_NAME_MAX_NUM + 1);

				ClientPacket(ITEMSHOP_SUBHEADER_GC_CATEGORY, &pack, sizeof(TItemShopCategoryPacket), ch);
			}
		}

		time_t time = std::time(nullptr);

		if (!m_vec_itemShopItems.empty())
		{
			TEMP_BUFFER buf (1024 * 1024);
			for (const auto& item : m_vec_itemShopItems)
			{
				// skip items with limited time expired and sold out
				if (!reload && ((item->limited_time && time > item->limited_time)
					|| (item->limited_count && item->limited_buy_count >= item->limited_count)))
				{
					TItemShopItem it{};
					it.id = item->id;
					it.category = item->category;

					ManageItem(nullptr, EDITOR_FLAG_DELETE, it, true);
					continue;
				}

				buf.write(item, sizeof(TItemShopItem));
			}

			ClientPacket(ITEMSHOP_SUBHEADER_GC_ITEM, buf.read_peek(), buf.size(), ch);
		}
	}

	for (const auto& it : m_vec_itemShopBuyStats)
		ClientPacket(ITEMSHOP_SUBHEADER_GC_BUY_STATS, &it, sizeof(TItemShopBuyStats), ch);

	TItemShopOpenPacket p{};
	p.ttDataTime = m_ttDataTime;
	// send coins
	SendCoins(ch);
	// send open packet
	if (!reload)
		ClientPacket(ITEMSHOP_SUBHEADER_GC_OPEN, &p, sizeof(TItemShopOpenPacket), ch);
	// add player to viewer list
	AddViewer(ch);
}

void CItemShopManager::SendItem(TItemShopItem* item, BYTE header, LPCHARACTER ch)
{
	ClientPacket(header, item, sizeof(TItemShopItem), ch);
}

void CItemShopManager::SendError(BYTE byError, LPCHARACTER ch)
{
	ClientPacket(ITEMSHOP_SUBHEADER_GC_ERROR, &byError, sizeof(BYTE), ch);
}

void CItemShopManager::SendCoins(LPCHARACTER ch)
{
	auto dwCash = ch->GetCash();
	ClientPacket(ITEMSHOP_SUBHEADER_GC_COINS, &dwCash, sizeof(DWORD), ch);
}

void CItemShopManager::BuyItem(LPCHARACTER ch, DWORD id, DWORD category, int count, BYTE bonus_type)
{
	// check if player already opened shop
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) tried to buy item id[%d] category[%d]",
			ch->GetPlayerID(), ch->GetName(), id, category);
		return;
	}

	// check if category is ok
	if (category > ITEMSHOP_CATEGORY_MAX_NUM)
	{
		SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
		return;
	}

	// get item and see if it's actually there
	auto* item = FindItemByID(id, category, bonus_type);
	if (item == nullptr)
	{
		SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
		return;
	}
	
	// check if count is ok
	if (count > ITEM_MAX_COUNT)
	{
		SendError(EItemShopError::ITEMSHOP_ERROR_COUNT, ch);
		return;
	}

	TItemTable* pTable = nullptr;

	if (!item->is_vip)
	{
		// check if item exist thus game can create it
		pTable = ITEM_MANAGER::instance().GetTable(item->vnum);

		if (!pTable)
		{
			SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
			return;
		}
	}

	// prepare important data for count, price and additional time for costumes
	const bool bFixedCount = item->fixed_count;
	const DWORD dwAddTime = item->additional_time;
	const int dwItemCount = (bFixedCount) ? item->count : count;
	auto dwPrice = (bFixedCount) ? item->price : dwItemCount * item->price;
	auto dwPriceCopy = dwPrice;

	//prepare time pointer
	time_t time = std::time(nullptr);

	// for the limited time and/or count items
	DWORD dwLeftLimitedCount = 0;
	DWORD dwLimitedBuyCount = 0;
	if (item->limited_count)
	{
		// check if item is limited time offer
		if (item->limited_time && time > item->limited_time)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP LIMITED TIME END"));

			// delete item if expired
			TItemShopItem it{};
			it.id = item->id;
			it.category = item->category;

			ManageItem(nullptr, EDITOR_FLAG_DELETE, it, true);

			return;
		}

		//prepare query
		char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),
			"SELECT `limited_buy_count` FROM `itemshop_items%s` WHERE `id` = %d AND `category` = %d ",
			get_table_postfix(), item->id, item->category);

		// get actuall buy count at real time
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to get itemshop limited item count, error code %d", pMsg->uiSQLErrno);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP UNKNOWN ERROR %d"), pMsg->uiSQLErrno);
			return;
		}
		const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		if (row)
		{
			str_to_number(dwLimitedBuyCount, row[0]);
			dwLeftLimitedCount = item->limited_count - dwLimitedBuyCount;
		}
		else
		{
			sys_err("Failed to get itemshop limited item count, error code %d", 90);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP UNKNOWN ERROR %d"), 90);
			return;
		}

		// check if item is out of stock
		if (dwLeftLimitedCount >= dwItemCount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP LIMITED ITEM SUCESS"));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP LIMITED ITEM FAIL"));
			return;
		}
	}
	else
	{
		if (item->limited_time)
		{
			// check item limited time offer
			if (time > item->limited_time)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP LIMITED TIME END"));

				// delete item if expired
				TItemShopItem it{};
				it.id = item->id;
				it.category = item->category;

				ManageItem(nullptr, EDITOR_FLAG_DELETE, it, true);

				return;
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP LIMITED ITEM SUCESS"));
			}
		}
	}

	//check if item has discount
	if (item->discount)
	{
		if (item->discount_time >= time)
		{
			// if item still has discount reduce price by certain percent
			dwPrice *= static_cast<float>(100 - item->discount) / 100.0;
		}
		else
		{
			// if discount is over set it to 0

			//prepare query
			char szQuery[128];
			snprintf(szQuery, sizeof(szQuery),
				"UPDATE `itemshop_items%s` SET `discount` = 0, `discount_time` = 0 WHERE `id` = %d AND `category` = %d",
				get_table_postfix(), item->id, item->category);

			// execute query
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			if (pMsg->uiSQLErrno != 0)
			{
				sys_err("Failed to delete itemshop item discount, error code %d", pMsg->uiSQLErrno);
				return;
			}
		}
	}
	dwPrice = MAX((DWORD)1, dwPrice);

	//don't use discount for cash coupons!!!
	if (item->vnum > 80014 && item->vnum < 80017)
	{
		if (dwPriceCopy > dwPrice)
		{
			dwPrice = dwPriceCopy;
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP CASH COUPONS CAN'T HAVE DISCOUNT"));
		}
	}

	// check if player has enough cash
	const auto dwCash = ch->GetCash();
	if (dwCash < dwPrice)
	{
		SendError(EItemShopError::ITEMSHOP_ERROR_NOT_ENOUGH_COINS, ch);
		return;
	}

	if (item->limited_buy_time)
	{
		char szQuery[256];
		snprintf(szQuery, sizeof(szQuery),
			"SELECT `locked` FROM `itemshop_buylimit%s` WHERE `id` = %d AND `player_id` = %d",
			get_table_postfix(), item->id, ch->GetPlayerID());

		// execute query
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to delete itemshop item discount, error code %d", pMsg->uiSQLErrno);
			return;
		}

		int time_locked = 0;

		if (pMsg->Get())
		{

			while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
			{
				str_to_number(time_locked, row[0]);
				break;
			}
		}

		if (time_locked > time)
		{
			char szLockTime[128 + 1];
			FormatTime(time_locked - time, szLockTime, sizeof(szLockTime));

			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP YOU CANT BUY ITEM FOR ANOTHER %s"), szLockTime);
			return;
		}
		else
		{
			char szQuery2[256];
			snprintf(szQuery2, sizeof(szQuery2),
				"REPLACE INTO `itemshop_buylimit%s` (`id`, `player_id`, `locked`) VALUES (%d, %d, %d)",
				get_table_postfix(), item->id, ch->GetPlayerID(), time + item->limited_buy_time);

			// execute query
			std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery(szQuery2));
			if (pMsg2->uiSQLErrno != 0)
			{
				sys_err("Failed to set itemshop item lock, error code %d", pMsg2->uiSQLErrno);
				return;
			}
		}
	}

	// check if item is vip packet
	if (item->is_vip)
	{
		// prepare all important pointers
		DWORD dwIndex = item->vnum;
		DWORD dwPremiumTime = item->additional_time;
		DWORD dwPrintTime = 0;

		int* tPremium = ch->GetPremium();
		int iAddTime = 0;
		bool bExtend = false;
		int iPremiumTime = abs(tPremium[dwIndex] - time);

		// if player doesn't have premium or it's expired
		if (time >= tPremium[dwIndex])
		{
			dwPrintTime = dwPremiumTime;
			iAddTime = time + dwPremiumTime;
		}
		// if player has active premium packet
		else
		{
			dwPrintTime = abs(tPremium[dwIndex] - time)  + dwPremiumTime;
			iAddTime = tPremium[dwIndex] + dwPremiumTime;
			bExtend = true;
		}

		// add affect and save time to array
		ch->AddAffect(AFFECT_PREMIUM_START + dwIndex, POINT_NONE, 0, 0, dwPrintTime, 0, true);
		tPremium[dwIndex] = iAddTime;
		sys_log(0, "PREMIUM: %s type %d %d sec", ch->GetName(), dwIndex, dwPrintTime);

		// print reward chat packet
		char szTimeLeft[128 + 1];
		FormatTime(dwPrintTime, szTimeLeft, sizeof(szTimeLeft));

		std::string stPremiumName;
		switch (dwIndex)
		{
			case PREMIUM_EXP:
				stPremiumName = "PREMIUM_EXP";
				break;
			case PREMIUM_ITEM:
				stPremiumName = "PREMIUM_ITEM";
				break;
			case PREMIUM_SAFEBOX:
				stPremiumName = "PREMIUM_SAFEBOX";
				break;
			case PREMIUM_AUTOLOOT:
				stPremiumName = "PREMIUM_AUTOLOOT";
				break;
			case PREMIUM_FISH_MIND:
				stPremiumName = "PREMIUM_FISH_MIND";
				break;
			case PREMIUM_MARRIAGE_FAST:
				stPremiumName = "PREMIUM_MARRIAGE_FAST";
				break;
			case PREMIUM_GOLD:
				stPremiumName = "PREMIUM_GOLD";
				break;
			default:
				stPremiumName = "PREMIUM_ERROR";
				break;
		}
		
		if (bExtend)
		{
			char szPremiumTime[128 + 1];
			FormatTime(iPremiumTime, szPremiumTime, sizeof(szPremiumTime));
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP ALREADY HAD PACKET FOR %s"), szPremiumTime);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP EXTENDED %s FOR %s"), LC_TEXT(stPremiumName.c_str()), szTimeLeft);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP BOUGHT %s FOR %s"), LC_TEXT(stPremiumName.c_str()), szTimeLeft);
		}

		// save data in db
		TItemShopSavePremiumPacket p;
		p.account_id = ch->GetDesc()->GetAccountTable().id;
		p.premium = dwIndex;
		p.time = iAddTime;

		db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_SAVE_PREMIUM, 0, &p, sizeof(TItemShopSavePremiumPacket));

	}
	else
	{
		// create item
		auto pkItem = ITEM_MANAGER::instance().CreateItem(item->vnum, dwItemCount, 0, false);
		if (!pkItem)
		{
			sys_err("failed creating itemshop item for player %s", ch->GetName());
			SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
			return;
		}

		/*
			!!! check if player has space for an item !!!
			!!! don't touch it without my assistance !!!
			!!! this may cause error that lets player to buy for free !!!
		*/
		TItemPos tCell;
		if (pkItem->IsDragonSoul())
		{
			tCell.cell = ch->GetEmptyDragonSoulInventory(pkItem);
			tCell.window_type = DRAGON_SOUL_INVENTORY;
		}
		else
		{
			tCell.cell = ch->GetEmptyInventory(pkItem->GetSize());
			tCell.window_type = INVENTORY;
		}

		// if doesn't destroy item in order to prevent leaks
		if (tCell.cell == -1)
		{
			M2_DESTROY_ITEM(pkItem);
			SendError(EItemShopError::ITEMSHOP_ERROR_INVENTORY_FULL, ch);
			return;
		}

		// set sockets if they are set
		auto i = 0;
		for (const auto lSocket : item->alSockets)
		{
			if (lSocket)
				pkItem->SetSocket(i, lSocket, false);
			i++;
		}

		// set attributes if they are set
		i = 0;
		for (const auto& aAttr : item->aAttr)
		{
			if (aAttr.bType && aAttr.sValue)
				pkItem->SetForceAttribute(i, aAttr.bType, aAttr.sValue);
			i++;
		}

		// if item has additional wear time, let's set it
		if (dwAddTime)
		{
			if (pkItem->IsRealTimeItem())
				pkItem->SetSocket(0, static_cast<long>(time + dwAddTime), false);
			else if (pTable->cLimitRealTimeFirstUseIndex != -1)
				pkItem->SetSocket(0, dwAddTime, false);
			else if (pTable->cLimitTimerBasedOnWearIndex != -1)
				pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, dwAddTime, false);
			sys_log(0, "additional time: %d, socket: %d", dwAddTime, pkItem->GetSocket(0));
		}

		ch->AutoGiveItem(pkItem, true);
		// log purchase
		LogManager::instance().ItemShopBuyLog(item->vnum, dwItemCount, item->id, item->category, ch->GetDesc()->GetAccountTable().id, dwPrice);

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP ITEM %s x %d BOUGHT"), pkItem->GetName(), pkItem->GetCount());

		// save limited item count
		if (item->limited_count)
		{
			//prepare query
			char szQuery[128];
			snprintf(szQuery, sizeof(szQuery),
				"UPDATE `itemshop_items%s` SET `limited_buy_count` = `limited_buy_count` + %d WHERE `id` = %d AND `category` = %d",
				get_table_postfix(), dwItemCount, item->id, item->category);

			// execute query
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			if (pMsg->uiSQLErrno != 0)
			{
				sys_err("Failed to update itemshop item limited count, error code %d", pMsg->uiSQLErrno);
				return;
			}

			if (!m_bNeedUpdate)
			{
				if (m_nextUpdate < time)
				{
					m_nextUpdate = time + ITEMSHOP_LIMITED_ITEM_REFRESH_TIME;
					m_bNeedUpdate = true;
				}
			}

			TItemShopBuyStats tStats;
			tStats.dwID = item->id;
			tStats.dwCategory = item->category;
			tStats.dwCount = dwLimitedBuyCount + dwItemCount;

			ClientPacket(ITEMSHOP_SUBHEADER_GC_LIMITED_COUNT, &tStats, sizeof(TItemShopBuyStats), ch);

			pkItem->SetSocket(0, dwLimitedBuyCount + dwItemCount, false);
			pkItem->SetSocket(1, item->limited_count, false);
			pkItem->SetSocket(2, 255, false);
		}

		// save buy statistics
		TItemShopBuyStats tStats(id, category);
		const auto find = std::find_if(m_vec_itemShopBuyStatsSave.begin(), m_vec_itemShopBuyStatsSave.end(),
			[&](const TItemShopBuyStats it) -> bool
			{
				return it.dwID == id && it.dwCategory == category;
			}
		);

		if (find == m_vec_itemShopBuyStatsSave.end())
			m_vec_itemShopBuyStatsSave.push_back(tStats);
		else
			(*find).dwCount++;

		tStats.dwCount++;
		m_bNeedFlush = true;

#ifdef WEEKLY_RANK_BYLUZER
		if (pkItem->GetVnum() != 80037 && pkItem->GetVnum() != 80038 && pkItem->GetVnum() != 80039 && pkItem->GetVnum() != 80040)
			ch->PointChange(POINT_WEEKLY7, dwPrice);
#endif
	}

	// charge cash and send coins
	ch->SetCash(dwCash - dwPrice);
	SendCoins(ch);
}

void CItemShopManager::ManageItem(LPCHARACTER ch, BYTE flag, TItemShopItem item, bool by_system)
{
	if (!ch)
	{
		sys_err("CItemShopManager::ManageItem - LPCHARACTER ch is null!");
		return;
	}
	// check only if function executed by some player
	if (!by_system)
	{
		// check if player already opened shop
		if (!IsViewer(ch))
		{
			sys_err("no viewer character (%d %s) tried to add item vnum[%d] count[%d] price[%d] category[%d]",
				ch->GetPlayerID(), ch->GetName(), item.vnum, item.count, item.price, item.category);
			SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
			return;
		}

		// check if player is editor
		if (!IsEditor(ch))
		{
			sys_err("non editor character (%d %s) tried to add item vnum[%d] count[%d] price[%d] category[%d]",
				ch->GetPlayerID(), ch->GetName(), item.vnum, item.count, item.price, item.category);
			SendError(EItemShopError::ITEMSHOP_ERROR_NON_EDITOR, ch);
			return;
		}

		// check if category is ok
		if (item.category > ITEMSHOP_CATEGORY_MAX_NUM)
		{
			SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
			return;
		}
	}

	// prepare flag for multicore update
	auto bRefresh = false;

	// flag says game what to do with item
	// let's do appropriate things for each flag 
	switch (flag)
	{
		case EDITOR_FLAG_ADD:
		case EDITOR_FLAG_EDIT:
		{
			// check if data get from client makes sense
			if (item.vnum == 0 || item.count == 0 || item.price == 0)
			{
				SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
				return;
			}

			// check if item actually exist
			const TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item.vnum);

			if (!pTable)
			{
				SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
				return;
			}
			
			// prepare add/edit query
			char szQuery[512] = "REPLACE INTO `itemshop_items` (`id`, `category`, `price`, `vnum`, `count` ";
			auto len = strlen(szQuery);

			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
				len += snprintf(szQuery + len, sizeof(szQuery) - len,
					",`socket%d` ", i);

			for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
				len += snprintf(szQuery + len, sizeof(szQuery) - len,
					",`attrtype%d` , `attrvalue%d` ", i, i);

			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",`fixed_count`, `add_time`) ");			
			
			// when we add item we have to set id to 0 and let database set id as a incremented primary key
			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				"VALUES (%d, %d, %d, %d, %d", (flag == EDITOR_FLAG_ADD) ? 0 : item.id, item.category, item.price, item.vnum, item.count);

			for (const auto socket: item.alSockets)
				len += snprintf(szQuery + len, sizeof(szQuery) - len,
					",%ld ", socket);

			for (const auto attr: item.aAttr)
				len += snprintf(szQuery + len, sizeof(szQuery) - len,
					",%d ,%d ", attr.bType, attr.sValue);

			len += snprintf(szQuery + len, sizeof(szQuery) - len,
				",%d ,NOW()); ", item.fixed_count);

			// execute query
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			if (pMsg->uiSQLErrno != 0)
			{
				sys_err("Failed to edit itemshop item, error code %d", pMsg->uiSQLErrno);
				return;
			}

			if (!pMsg->Get() || pMsg->Get()->uiAffectedRows == 0)
			{
				sys_err("Failed to edit itemshop item, no data has changed");
				return;
			}

			// if query is executed do things to item
			// on add flag add item with id got from database to vector and send to viewers
			if (flag == EDITOR_FLAG_ADD)
			{
				item.id = pMsg->Get()->uiInsertID;
				auto* pItem = new TItemShopItem;
				std::memcpy(pItem, &item, sizeof(TItemShopItem));

				m_vec_itemShopItems.push_back(pItem);

				SendItem(pItem, ITEMSHOP_SUBHEADER_GC_ITEM);
			}
			// on edit flag edit item in vector and send to viewers
			else if (flag == EDITOR_FLAG_EDIT)
			{
				auto id = item.id;
				auto category = item.category;

				auto* pItem = new TItemShopItem;
				std::memcpy(pItem, &item, sizeof(TItemShopItem));

				std::replace_if(m_vec_itemShopItems.begin(), m_vec_itemShopItems.end(),
					[&](TItemShopItem* it) -> bool
					{
						return it->id == id && it->category == category;
					},
					pItem
				);

				SendItem(pItem, ITEMSHOP_SUBHEADER_GC_UPDATE);
			}
			// item was updated, so we have to broadcast it to other peers
			SetCooldownManage();
			bRefresh = true;
		}
			break;

		case EDITOR_FLAG_DELETE:
		{
			// check if data from client makes sense
			if (item.id == 0)
			{
				if (ch)
					SendError(EItemShopError::ITEMSHOP_ERROR_UNKNOWN_ERROR, ch);
				return;
			}

			//prepare query
			char szQuery[512];
			snprintf(szQuery, sizeof(szQuery),
				"DELETE FROM `itemshop_items%s` WHERE `id` = %d AND `category` = %d",
				get_table_postfix(), item.id, item.category
			);

			// execute query
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
			if (pMsg->uiSQLErrno != 0)
			{
				sys_err("Failed to delete itemshop item, error code %d", pMsg->uiSQLErrno);
				return;
			}

			if (!pMsg->Get() || pMsg->Get()->uiAffectedRows == 0)
			{
				sys_err("Failed to edit itemshop item, no data has changed");
				return;
			}

			// if query is executed delete item from vector and send it to viewers
			auto id = item.id;
			auto category = item.category;

			m_vec_itemShopItems.erase(std::remove_if(m_vec_itemShopItems.begin(), m_vec_itemShopItems.end(),
				[&](TItemShopItem* it) -> bool
				{
					return it->id == id && it->category == category;
				}
			), m_vec_itemShopItems.end());

			TItemShopItemDeletePacket pack;
			pack.id = item.id;
			pack.category = item.category;

			ClientPacket(ITEMSHOP_SUBHEADER_GC_DELETE, &pack, sizeof(TItemShopItemDeletePacket));

			// item was deleted, so we have to broadcast it to other peers
			bRefresh = true;
			SetCooldownManage();
		}
			break;

		default:
			break;

	}

	// if item was updated in some way, broadcast to peers
	if (bRefresh)
	{
		TItemShopRefreshPacket p;
		p.id = item.id;
		p.category = item.category;
		p.flag = flag;

		db_clientdesc->DBPacket(HEADER_GD_REFRESH_ITEMSHOP, 0, &p, sizeof(TItemShopRefreshPacket));
	}
}

bool CItemShopManager::IsEditor(LPCHARACTER ch) const
{
	auto vec = m_vec_pkEditors;

	std::string szName = ch->GetName();
	std::for_each(szName.begin(), szName.end(), [](char& c) { c = tolower(c); });

	return (std::find(vec.begin(), vec.end(), szName) != vec.end());
}

void CItemShopManager::ReloadItems(LPCHARACTER ch)
{
	// check if player is editor
	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) tried to reload items",
			ch->GetPlayerID(), ch->GetName());
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Reloading itemshop...");

	// broadcast reload packet to other peers
	TItemShopRefreshPacket p;
	p.id = 0;
	p.category = 0;
	p.flag = 0;

	db_clientdesc->DBPacket(HEADER_GD_REFRESH_ITEMSHOP, 0, &p, sizeof(TItemShopRefreshPacket));
}

bool CItemShopManager::IsViewer(LPCHARACTER ch)
{
	return m_set_pkCurrentViewer.find(ch) != m_set_pkCurrentViewer.end();
}

void CItemShopManager::AddViewer(LPCHARACTER ch)
{
	if (!IsViewer(ch))
		m_set_pkCurrentViewer.insert(ch);
}

void CItemShopManager::RemoveViewer(LPCHARACTER ch)
{
	if (IsViewer(ch))
		m_set_pkCurrentViewer.erase(ch);
}

void CItemShopManager::ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch, bool send_to_all)
{
	// prepare header
	TItemShopGCPacket header;
	header.header = HEADER_GC_ITEMSHOP;
	header.size = sizeof(TItemShopGCPacket) + size;
	header.subheader = subheader;

	// send packet to all connected desc
	if (send_to_all)
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

		for (const auto& desc : c_ref_set)
		{
			if (desc->GetCharacter())
			{
				if (size)
				{
					desc->BufferedPacket(&header, sizeof(TItemShopGCPacket));
					if (size >= 8192)
						desc->LargePacket(c_pData, size);
					else
						desc->Packet(c_pData, size);
				}
				else
				{
					desc->Packet(&header, sizeof(TItemShopGCPacket));
				}
			}
		}

		return;
	}

	// check if packet is for one character, if so, send it only to it, in other case, send to all viewers that opened itemshop
	if (ch && ch->GetDesc())
	{
		if (size)
		{
			ch->GetDesc()->BufferedPacket(&header, sizeof(TItemShopGCPacket));
			if (size >= 8192)
				ch->GetDesc()->LargePacket(c_pData, size);
			else
				ch->GetDesc()->Packet(c_pData, size);
		}
		else
		{
			ch->GetDesc()->Packet(&header, sizeof(TItemShopGCPacket));
		}
	}
	else
	{
		for (const auto& it : m_set_pkCurrentViewer)
		{
			if (it && it->GetDesc())
			{
				if (size)
				{
					it->GetDesc()->BufferedPacket(&header, sizeof(TItemShopGCPacket));
					if (size >= 8192)
						it->GetDesc()->LargePacket(c_pData, size);
					else
						it->GetDesc()->Packet(c_pData, size);
				}
				else
				{
					it->GetDesc()->BufferedPacket(&header, sizeof(TItemShopGCPacket));
					it->GetDesc()->Packet(c_pData, size);
				}
			}
			else
			{
				RemoveViewer(it);
			}
		}
	}
}

void CItemShopManager::FlushStatistics()
{
	if (!m_bNeedFlush || time(0) - m_lastFlushTime <= m_expireTime)
		return;

	m_bNeedFlush = false;
	sys_log(0, "hit itemshop statistics flush with %d items", m_vec_itemShopBuyStatsSave.size());

	char szQuery[256];
	for (const auto& it : m_vec_itemShopBuyStatsSave)
	{
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO itemshop_statistics%s (`id`, `category`, `count`) "
			"VALUES (%d, %d, %d) ON DUPLICATE KEY UPDATE `count` = `count` + %d; ",
			get_table_postfix(), it.dwID, it.dwCategory, it.dwCount, it.dwCount);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to save itemshop statistics, error code %d", pMsg->uiSQLErrno);
			return;
		}
	}

	m_lastFlushTime = time(nullptr);
	m_vec_itemShopBuyStatsSave.clear();
	InitializeStatistics();
}

void CItemShopManager::InitializeStatistics()
{
	m_vec_itemShopBuyStats.clear();
	// execute query
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT `id`, `category`, `count` FROM `itemshop_statistics%s` ", get_table_postfix()));
	if (pMsg->uiSQLErrno != 0)
	{
		sys_err("Failed to load itemshop statistics, error code %d", pMsg->uiSQLErrno);
		return;
	}

	// iterate all data we get from database
	if (pMsg->Get())
	{
		while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			BYTE col = 0;
			TItemShopBuyStats tStats;
			str_to_number(tStats.dwID, row[col++]);
			str_to_number(tStats.dwCategory, row[col++]);
			str_to_number(tStats.dwCount, row[col++]);
			
			sys_log(0, "ITEMSHOP statistics id [%d], category [%d], count [%d]", tStats.dwID, tStats.dwCategory, tStats.dwCount);

			m_vec_itemShopBuyStats.push_back(tStats);
		}
	}
}

void CItemShopManager::RefreshLimitedTime()
{
	if (!m_bNeedUpdate)
		return;

	time_t time = std::time(nullptr);

	if (time >= m_nextUpdate)
	{
		m_bNeedUpdate = false;

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT `id`, `category`, `limited_buy_count` "
			"FROM `itemshop_items%s` WHERE `limited_count` != 0", get_table_postfix()));
		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to refresh limited count items, error code %d", pMsg->uiSQLErrno);
			return;
		}

		if (pMsg->Get())
		{
			while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
			{
				BYTE col = 0;
				TItemShopBuyStats tStats;
				str_to_number(tStats.dwID, row[col++]);
				str_to_number(tStats.dwCategory, row[col++]);
				str_to_number(tStats.dwCount, row[col++]);

				// update data in vector so it sends right data to client
				auto item = FindItemByID(tStats.dwID, tStats.dwCategory);
				if (item)
				{
					item->limited_buy_count = MIN(tStats.dwCount, item->limited_buy_count);

					// delete item if out of stock or expired
					if (item->limited_buy_count < 1 || (item->limited_time && time > item->limited_time))
					{
						TItemShopItem it{};
						it.id = item->id;
						it.category = item->category;

						ManageItem(nullptr, EDITOR_FLAG_DELETE, it, true);
					}
				}

				ClientPacket(ITEMSHOP_SUBHEADER_GC_LIMITED_COUNT, &tStats, sizeof(TItemShopBuyStats));
			}
		}
	}
}

int  getNumberOfDays(int month, int year)
{
	//leap year condition, if month is 2
	if (month == 2)
	{
		if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
			return 29;
		else
			return 28;
	}
	//months which has 31 days
	else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8
		|| month == 10 || month == 12)
		return 31;
	else
		return 30;
}

void CItemShopManager::FormatTime(int ms, char* buffer, size_t size)
{
	const int cseconds_in_day = 86400;
	const int cseconds_in_hour = 3600;
	const int cseconds_in_minute = 60;
	const int cseconds = 1;

	long long d = ms / cseconds_in_day;
	long h = (ms % cseconds_in_day) / cseconds_in_hour;
	long m = ((ms % cseconds_in_day) % cseconds_in_hour) / cseconds_in_minute;
	long s = (((ms % cseconds_in_day) % cseconds_in_hour) % cseconds_in_minute) / cseconds;

	size_t len = 0;

	if (d > 0)
		len += snprintf(buffer + len, size - len,
			LC_TEXT("DAY %d"), d);	
	
	if (h > 0)
		len += snprintf(buffer + len, size - len,
			LC_TEXT("HOUR %d"), h);

	if (m > 0)
		len += snprintf(buffer + len, size - len,
			LC_TEXT("MINUTE %d"), m);

	if (s > 0)
		len += snprintf(buffer + len, size - len,
			LC_TEXT("SECOND %d"), s);

	if (len == 0)
		strcpy(buffer, "UNDEFINED");
}
#endif
