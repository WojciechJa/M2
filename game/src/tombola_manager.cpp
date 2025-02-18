#include "stdafx.h"

#ifdef __TOMBOLA__
#include "char.h"
#include "db.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "item_manager.h"
#include <random>
#include "itemshop_manager.h"
#include "tombola_manager.h"

typedef struct STombolaSetting
{
	float fSpinSpeed;
	int iBlinkCount;
	int iMinSpin;
	int iMaxSpin;
	int iSlowMin;
	int iSlowMax;
} TTombolaSetting;

static const TTombolaSetting tombolaSetting = {
	0.058, 6, 1, 3, 4, 10
};

CTombola::CTombola()
{

}

CTombola::~CTombola()
{
	Initialize();
}

void CTombola::Initialize()
{
	if (!m_map_pkItem.empty())
	{
		for (auto& vec : m_map_pkItem)
			vec.second.pkItem.clear();

		m_map_pkItem.clear();
	}

	sys_log(0, "CTombola::Initialize(): Fetch data from database");
	const char* szQuery = "SELECT `id`, `price`, `price_type`, `main_item_vnum`, `main_item_count`, `main_item_chance` FROM `itemshop_wheel_group` ";

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg->uiSQLErrno != 0)
	{
		sys_err("Failed to load tombola items, error code %d", pMsg->uiSQLErrno);
		return;
	}

	if (pMsg->Get())
	{
		while (const auto row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			auto col = 0;

			BYTE byGroup = atoi(row[col++]);

			if (m_map_pkItem.find(byGroup) == m_map_pkItem.end())
			{
				CTombola::TTombolaGroup pkGroup;
				pkGroup.dwPrice = atoi(row[col++]);
				std::string szType = row[col++];
				if (szType == "yang")
					pkGroup.dwPriceType = TOMBOLA_PRICE_YANG;
				else if (szType == "cash")
					pkGroup.dwPriceType = TOMBOLA_PRICE_CASH;
				else
					pkGroup.dwPriceType = atoi(szType.c_str());

				pkGroup.pkMainItem.dwVnum = atoi(row[col++]);
				pkGroup.pkMainItem.dwCount = atoi(row[col++]);
				pkGroup.pkMainItem.dwChance = atoi(row[col++]);

				m_map_pkItem.insert(std::make_pair(byGroup, pkGroup));

				sys_log(0, "TOMBOLA_GROUP index [%d], price [%d], price_type [%s]", byGroup, pkGroup.dwPrice, szType.c_str());
			}

			char szQuery2[128];
			snprintf(szQuery2, sizeof(szQuery2), "SELECT `item_vnum`, `item_count`, `item_chance` FROM `itemshop_wheel_item` WHERE `group` = %d ", byGroup);

			std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery(szQuery2));
			if (pMsg2->uiSQLErrno != 0)
			{
				sys_err("Failed to load tombola items, error code %d", pMsg2->uiSQLErrno);
				return;
			}

			if (pMsg2->Get())
			{
				int k = 0;
				while (const auto row2 = mysql_fetch_row(pMsg2->Get()->pSQLResult))
				{
					CTombola::TTombolaItem item;
					item.dwVnum = atoi(row2[0]);
					item.dwCount = atoi(row2[1]);
					item.dwChance = atoi(row2[2]);

					sys_log(0, "        %d %d %d %d", k++, item.dwVnum, item.dwCount, item.dwChance);
					m_map_pkItem[byGroup].pkItem.push_back(item);
				}
			}
		}
	}

	return;
}

CTombola::TTombolaVec CTombola::CreatePrize(BYTE byStage)
{
	static std::random_device rd;
	auto& vecPrize = m_map_pkItem[byStage].pkItem;
	std::shuffle(vecPrize.begin(), vecPrize.end(), rd);

	auto result_vec = CTombola::TTombolaVec(vecPrize.begin(), vecPrize.begin() + 16);
	result_vec[0] = m_map_pkItem[byStage].pkMainItem;

	return result_vec;
}

void CTombola::RequestOpen(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC())
		return;

	if (!ch->m_bHasWheelData)
	{
		for (const auto& group : m_map_pkItem)
		{
			TEMP_BUFFER buf;

			DWORD dwGroup = group.first;
			DWORD dwPrice = group.second.dwPrice;
			DWORD dwPriceType = group.second.dwPriceType;
			DWORD dwVectorSize = group.second.pkItem.size() + 1;
			const auto pkMainItem = group.second.pkMainItem;

			buf.write(&dwGroup, sizeof(DWORD));
			buf.write(&dwPrice, sizeof(DWORD));
			buf.write(&dwPriceType, sizeof(DWORD));
			buf.write(&dwVectorSize, sizeof(DWORD));

			buf.write(&pkMainItem, sizeof(TTombolaItem));

			for (auto& it : group.second.pkItem)
				buf.write(&it, sizeof(TTombolaItem));

			ClientPacket(TOMBOLA_GC_SUBHEADER_ITEM, buf.read_peek(), buf.size(), ch);
		}
		ch->m_bHasWheelData = true;
	}

	ClientPacket(TOMBOLA_GC_SUBHEADER_OPEN, nullptr, 0, ch);
	sys_log(0, "Tombola opened");
}

void CTombola::RequestStart(LPCHARACTER ch, BYTE byGroup)
{
	if (!ch || !ch->IsPC())
		return;

	if (m_map_pkItem.find(byGroup) == m_map_pkItem.end())
	{
		sys_err("cannot find data for group %d, map size %d", byGroup, m_map_pkItem.size());
		return;
	}
	
	int dwPrice = m_map_pkItem[byGroup].dwPrice;
	int dwPriceType = m_map_pkItem[byGroup].dwPriceType;

	switch (dwPriceType)
	{
		case TOMBOLA_PRICE_YANG:
		{
			int dwCash = ch->GetGold();

			if (dwCash < dwPrice)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough coins"));
				return;
			}
		}
			break;
		case TOMBOLA_PRICE_CASH:
		{
			int dwCash = ch->GetCash();

			if (dwCash < dwPrice)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough coins"));
				return;
			}
		}
			break;
		default:
		{
			if (ch->CountSpecifyItem(dwPriceType) < dwPrice)
			{
				TItemTable* item_table = ITEM_MANAGER::instance().GetTable(dwPriceType);
				if (!item_table)
				{
					sys_err("can't find item by vnum %d", dwPriceType);
					return;
				}

				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough %s"), item_table->szLocaleName);
				return;
			}
		}
			break;

	}

	if (ch->GetTombolaPrize().dwVnum != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have not got your reward from last tombola"));
		GiveItem(ch);
		return;
	}

	auto vecItem = CreatePrize(byGroup);

	std::vector<int> vecProbs;

	for (const auto& item : vecItem)
	{
		int prob = item.dwChance;
		if (!vecProbs.empty())
			prob += vecProbs.back();
		vecProbs.push_back(prob);
	}

	int n = number(0, vecProbs.back());
	const auto it = lower_bound(vecProbs.begin(), vecProbs.end(), n);
	const auto reward_idx = MINMAX(0, std::distance(vecProbs.begin(), it), 15);
	const auto to_pos = reward_idx + 16 * number(tombolaSetting.iMinSpin, tombolaSetting.iMaxSpin);
	const auto to_spin = number(tombolaSetting.iSlowMin, tombolaSetting.iSlowMax);
	const auto time = 0.575 * tombolaSetting.fSpinSpeed * pow(to_spin, 2) 
		+ 1.15 * tombolaSetting.fSpinSpeed * to_pos + 0.575 * tombolaSetting.fSpinSpeed;

	switch (dwPriceType)
	{
		case TOMBOLA_PRICE_YANG:
		{
			ch->PointChange(POINT_GOLD, -dwPrice, true);
		}
			break;
		case TOMBOLA_PRICE_CASH:
		{
			int dwCash = ch->GetCash();
			ch->SetCash(dwCash - dwPrice);
		}
			break;
		default:
		{
			ch->RemoveSpecifyItem(dwPriceType, dwPrice);
		}
			break;
	}
	CItemShopManager::instance().SendCoins(ch);
	ch->SetTombolaPrize(vecItem[reward_idx]);
	ch->TombolaRewardEvent(std::ceil(time));

	TEMP_BUFFER buf;

	buf.write(&reward_idx, sizeof(int));
	buf.write(&to_pos, sizeof(int));
	buf.write(&to_spin, sizeof(int));
	buf.write(&time, sizeof(double));

	for (const auto& it : vecItem)
		buf.write(&it, sizeof(TTombolaItem));

	ClientPacket(TOMBOLA_GC_SUBHEADER_START, buf.read_peek(), buf.size(), ch);
}

void CTombola::GiveItem(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC())
		return;

	const auto item = ch->GetTombolaPrize();

	if (item.dwVnum == 0)
		return;

	const auto TTable = ITEM_MANAGER::instance().GetTable(item.dwVnum);

	if (!TTable)
	{
		sys_err("cannot find item by vnum %d", item.dwVnum);
		return;
	}

	if (ch->GetEmptyInventory(TTable->bSize) == -1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your inventory is full"));
		return;
	}

	ch->AutoGiveItem(item.dwVnum, item.dwCount);

	const TTombolaItem empty_item;
	ch->SetTombolaPrize(empty_item);
}

void CTombola::ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch)
{
	TPacketGCTombola header;
	header.byHeader = HEADER_GC_TOMBOLA;
	header.wSize = sizeof(TPacketGCTombola) + size;
	header.bySubHeader = subheader;

	if (ch && ch->GetDesc())
	{
		if (size)
		{
			ch->GetDesc()->BufferedPacket(&header, sizeof(TPacketGCTombola));
			ch->GetDesc()->Packet(c_pData, size);
		}
		else
		{
			ch->GetDesc()->Packet(&header, sizeof(TPacketGCTombola));
		}
	}
}
#endif
