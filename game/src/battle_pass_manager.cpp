#include "stdafx.h"
#include "battle_pass.h"
#include "battle_pass_manager.h"

#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "desc_client.h"
#include "packet.h"
#include "buffer_manager.h"

#include <algorithm>

CBattlePassManager::~CBattlePassManager()
{
}

CBattlePassManager::CBattlePassManager(const LPCHARACTER pkChar) : m_pkChar(pkChar)
{
	if (m_pkChar == NULL)
		return;
	
	if (const LPDESC pkCharDesc = m_pkChar->GetDesc(); !pkCharDesc)
		return;
}

void CBattlePassManager::Create(const LPCHARACTER pkChar)
{
	if (pkChar == NULL)
		return;
	
	pkChar->SetBattlePass(new CBattlePassManager(pkChar));
}

// void CBattlePassManager::AnalyzeQuestSignal(EBattlePassTypes signal)
// {
// 	switch (signal)
// 	{
// 		case BATTLE_PASS_MONSTER:
// 		case BATTLE_PASS_BOSS:
// 		case BATTLE_PASS_STONE:
// 		case BATTLE_PASS_DUNGEON:
// 		case BATTLE_PASS_CHAT:
// 		case BATTLE_PASS_REFINE:
// 		{
// 			std::for_each(vQuest.begin(), vQuest.end(), [&](auto& quest) {
// 				if (quest.questGroup == signal && !quest.questStatus)
// 				{
// 					if (quest.questCount + 1 == quest.questCountMax)
// 					{
// 						SetExp(quest.questIndex);
// 						quest.questStatus = 1;
// 					}
// 					if (quest.questStatus == 2)
// 					{
// 						quest.questCount = 0;
// 						quest.questStatus = 0;
// 					}
					
// 					quest.questCount++;
// 					SendQuestUpdate(quest.questIndex, quest.questCount, quest.questStatus);
// 				}
// 			});
// 		}
// 		break;
		
// 		default:
// 		{
// 			sys_err("Unknown BattlePass signal !!");
// 			return;
// 		}
// 	}
// }

int16_t CBattlePassManager::GetMaxLevel()
{
	const auto& battlePass = CBattlePass::instance();
	const auto& level = battlePass.BattlePassMaxLevel;
	
	return level;
}

int32_t CBattlePassManager::GetExp(int16_t level)
{
	if (GetMaxLevel() == level)
		return 0;
	
	const auto& battlePass = CBattlePass::instance();
	const auto& info = battlePass.vLevel;
	
	auto it = std::find_if(info.begin(), info.end(), [&](const auto& data) { return data.level == level; });
	
	if (it != info.end())
		return it->exp;

	return 0;
}

void CBattlePassManager::SetExp(int32_t value)
{
	// const auto& battlePass = CBattlePass::instance();
	// const auto& quest = battlePass.vQuest;
	
	// auto it = std::find_if(quest.begin(), quest.end(), [index](const auto& element) { return element.questIndex == index; });
	// if (it != quest.end())
	// {
	const int32_t currentExp = vInfo[0].exp;
	const int32_t neededExp = expNext;
	const int32_t totalExp = currentExp + value;
	
	if (totalExp >= neededExp)
	{
		int32_t expDifference = totalExp - neededExp;
		
		if (expDifference > 0)
		{
			while (expNext <= expDifference)
			{
				expDifference -= expNext;
				SetLevel();
			}
			
			vInfo[0].exp = expDifference;
		}
		else
		{
			vInfo[0].exp = 0;
			SetLevel();
		}
	}
	else
		vInfo[0].exp = totalExp;
	
	SendLevel();
	// }
}

void CBattlePassManager::SetReward(int16_t reward_level, int8_t reward_type)
{
	const auto& battlePass = CBattlePass::instance();
	const auto& reward = battlePass.vReward;
	
	auto it = std::find_if(reward.begin(), reward.end(), [&](const auto& data) { return data.rewardLevel == reward_level; });
	if (it != reward.end())
		m_pkChar->AutoGiveItem(it->rewardItem[reward_type], it->rewardCount[reward_type]);
}

void CBattlePassManager::SetQuest()
{
	SQuest data;
	const auto& battlePass = CBattlePass::instance();
	const auto& quest = battlePass.vQuest;
	
	for (const auto& element : quest)
	{
		data = {};
		
		data.questIndex = element.questIndex;
		data.questGroup = element.questGroup;
		data.questCount = 0;
		data.questCountMax = element.questCount;
		data.questStatus = 0;
		
		vQuest.push_back(data);
	}
}

void CBattlePassManager::SetLevel()
{
	vInfo[0].level++;
	expNext = GetExp(vInfo[0].level);
	
	const auto& battlePass = CBattlePass::instance();
	const auto& reward = battlePass.vReward;
	
	auto it = std::find_if(reward.begin(), reward.end(), [&](const auto& data) { return data.rewardLevel == vInfo[0].level; });
	if (it != reward.end())
	{
		SReward newReward;
		newReward.rewardLevel = it->rewardLevel;
		
		for (int i = 0; i < 2; i++)
			newReward.rewardStatus[i] = 1;
		
		vReward.push_back(newReward);
	}
	
	// RefreshReward();
	SendReward();
}

bool CBattlePassManager::CanCollectReward(int16_t reward_level, int8_t reward_type)
{
	return std::any_of(vReward.begin(), vReward.end(), [&](const auto& reward) {
		return reward.rewardLevel == reward_level && reward.rewardStatus[reward_type] == 1;
	});
}

void CBattlePassManager::InsertQuest(int32_t index, int32_t count, int8_t status)
{
	auto it = std::find_if(vQuest.begin(), vQuest.end(), [index](const SQuest& element) { return element.questIndex == index; });
	if (it != vQuest.end())
	{
		it->questCount = count;
		it->questStatus = status;
	}
}

void CBattlePassManager::InsertReward(int16_t level, int8_t status_free, int8_t status_premium)
{
	SReward data;
	
	data.rewardLevel = level;
	data.rewardStatus[0] = status_free;
	data.rewardStatus[1] = status_premium;
	
	vReward.push_back(data);
}

void CBattlePassManager::InsertInfo(int16_t level, int32_t exp, int16_t premium)
{
	if (!vInfo.empty())
		vInfo.clear();
	
	SInfo data;
	
	data.level = level;
	data.exp = exp;
	data.premium = premium;
	
	vInfo.push_back(data);
	
	expNext = GetExp(level);
}

// void CBattlePassManager::RefreshReward()
// {
// 	const auto& battlePass = CBattlePass::instance();
// 	const auto& reward = battlePass.vReward;
	
// 	auto it = std::find_if(reward.begin(), reward.end(), [&](const auto& data) { return data.rewardLevel == vInfo[0].level; });
// 	if (it != reward.end())
// 	{
// 		SReward newReward;
// 		newReward.rewardLevel = it->rewardLevel;
		
// 		for (int i = 0; i < 2; i++)
// 			newReward.rewardStatus[i] = 1;
		
// 		vReward.push_back(newReward);
// 	}
// }

void CBattlePassManager::CollectReward(const char* pData)
{
	return;
	
	const TPacketBattlePassCollect* entry = reinterpret_cast<const TPacketBattlePassCollect*>(pData);
	
	if (entry->rewardType == 1 && !vInfo[0].premium)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Do odebrania tej nagrody potrzebujesz statusu premium."));
		return;
	}
	
	if (CanCollectReward(entry->rewardLevel, entry->rewardType))
	{
		auto it = std::find_if(vReward.begin(), vReward.end(), [&](const auto& reward) { return reward.rewardLevel == entry->rewardLevel; });
		if (it != vReward.end())
		{
			it->rewardStatus[entry->rewardType] = 2;
			SendRewardData();
		}
		
		SetReward(entry->rewardLevel, entry->rewardType);
	}
}

void CBattlePassManager::SendReward()
{
	TEMP_BUFFER buffer;
	TPacketBattlePassReward packet;	
	
	const auto& battlePass = CBattlePass::instance();
	const auto& reward = battlePass.vReward;

	for (const auto& data : reward)
	{
		packet.header = HEADER_GC_BATTLE_PASS_REWARD;
		packet.rewardLevel = data.rewardLevel;
		
		for (int i = 0; i < 2; i++)
		{
			packet.rewardItem[i] = data.rewardItem[i];
			packet.rewardCount[i] = data.rewardCount[i];
		}
		
		buffer.write(&packet, sizeof(TPacketBattlePassReward));
	}

	if (m_pkChar->GetDesc() && buffer.size() != 0)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, "SERVER_BattlePassClearReward");
		m_pkChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
	}
	
	SendRewardData();
}

void CBattlePassManager::SendRewardData()
{
	TEMP_BUFFER buffer;
	TPacketBattlePassRewardData packet;
	
	for (const auto& data : vReward)
	{
		packet.header = HEADER_GC_BATTLE_PASS_REWARD_DATA;
		packet.rewardLevel = data.rewardLevel;
		
		for (int i = 0; i < 2; i++)
			packet.rewardStatus[i] = data.rewardStatus[i];
		
		buffer.write(&packet, sizeof(TPacketBattlePassRewardData));
	}
	
	if (m_pkChar->GetDesc() && buffer.size() != 0)
		m_pkChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
}

void CBattlePassManager::SendQuest()
{
	TEMP_BUFFER buffer;
	TPacketBattlePassQuest packet;
	
	const auto& battlePass = CBattlePass::instance();
	const auto& quest = battlePass.vQuest;
	
	for (const auto& data : quest)
	{
		packet.header = HEADER_GC_BATTLE_PASS_QUEST;
		
		packet.questIndex = data.questIndex;
		packet.questGroup = data.questGroup;
		packet.questCount = data.questCount;
		packet.questExp = data.questExp;
		
		buffer.write(&packet, sizeof(TPacketBattlePassQuest));
	}

	if (m_pkChar->GetDesc() && buffer.size() != 0)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, "SERVER_BattlePassClearQuest");
		m_pkChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
	}
	
	SendQuestData();
}

void CBattlePassManager::SendQuestData()
{
	TEMP_BUFFER buffer;
	TPacketBattlePassQuestData packet;
	
	for (const auto& data : vQuest)
	{
		packet.header = HEADER_GC_BATTLE_PASS_QUEST_DATA;
		
		packet.questIndex = data.questIndex;
		packet.questCount = data.questCount;
		packet.questStatus = data.questStatus;
		
		buffer.write(&packet, sizeof(TPacketBattlePassQuestData));
	}
	
	if (m_pkChar->GetDesc() && buffer.size() != 0)
		m_pkChar->GetDesc()->Packet(buffer.read_peek(), buffer.size());
}

void CBattlePassManager::SendQuestUpdate(int32_t index, int32_t count, int8_t status)
{
	TPacketBattlePassQuestData packet
	{
		packet.header = HEADER_GC_BATTLE_PASS_QUEST_DATA,
		
		packet.questIndex = index,
		packet.questCount = count,
		packet.questStatus = status
	};
	
	if (m_pkChar->GetDesc())
		m_pkChar->GetDesc()->Packet(&packet, sizeof(packet));
}

void CBattlePassManager::SendLevel()
{
	if (!m_pkChar)
		return;
	
	if (!m_pkChar->GetDesc())
		return;
	
	if (vInfo.size() == 0)
		return;
	
	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, "SERVER_BattlePassLevel %d#%d#%d", vInfo[0].level, vInfo[0].exp, expNext);
}

void CBattlePassManager::Save()
{
	std::vector<TBattlePassQuestTable> questTable;
	std::vector<TBattlePassRewardTable> rewardTable;
	std::vector<TBattlePassInformationTable> infoTable;
	
	if (!vQuest.empty())
	{
		questTable.reserve(vQuest.size());
		
		for (const auto& quest : vQuest)
		{
			if (quest.questCount > 0)
			{
				TBattlePassQuestTable packet;
				
				packet.pid = m_pkChar->GetPlayerID();
				packet.index = quest.questIndex;
				packet.count = quest.questCount;
				packet.status = quest.questStatus;
				
				questTable.push_back(packet);
			}
		}
	}
	
	if (!vReward.empty())
	{
		rewardTable.reserve(vReward.size());
		
		for (const auto& reward : vReward)
		{
			TBattlePassRewardTable packet;
			
			packet.pid = m_pkChar->GetPlayerID();
			packet.level = reward.rewardLevel;
			packet.status_free = reward.rewardStatus[0];
			packet.status_premium = reward.rewardStatus[1];
			
			rewardTable.push_back(packet);
		}
	}
	
	if (!vInfo.empty())
	{
		infoTable.reserve(vInfo.size());
		
		for (const auto& info : vInfo)
		{
			TBattlePassInformationTable packet;
			
			packet.pid = m_pkChar->GetPlayerID();
			packet.level = info.level;
			packet.exp = info.exp;
			packet.premium = info.premium;
			
			infoTable.push_back(packet);
		}
	}
	
	if (!questTable.empty())
	{
		db_clientdesc->DBPacketHeader(HEADER_GD_BATTLE_PASS_QUEST_SAVE, 0, sizeof(TBattlePassQuestTable) * questTable.size());
		db_clientdesc->Packet(questTable.data(), sizeof(TBattlePassQuestTable) * questTable.size());
	}
	
	if (!rewardTable.empty())
	{
		db_clientdesc->DBPacketHeader(HEADER_GD_BATTLE_PASS_REWARD_SAVE, 0, sizeof(TBattlePassRewardTable) * rewardTable.size());
		db_clientdesc->Packet(rewardTable.data(), sizeof(TBattlePassRewardTable) * rewardTable.size());
	}
	
	if (!infoTable.empty())
	{
		db_clientdesc->DBPacketHeader(HEADER_GD_BATTLE_PASS_INFO_SAVE, 0, sizeof(TBattlePassInformationTable) * infoTable.size());
		db_clientdesc->Packet(infoTable.data(), sizeof(TBattlePassInformationTable) * infoTable.size());
	}
}

void CBattlePassManager::Clear()
{
	vQuest.clear();
	vReward.clear();
	
	InsertInfo(1, 0, 0);
}