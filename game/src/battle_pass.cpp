#include "stdafx.h"
#include "battle_pass.h"

#include "locale_service.h"
#include "group_text_parse_tree.h"
#include "char_manager.h"
#include "char.h"

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <array>
#include <algorithm>

CBattlePass::CBattlePass()
{
}

CBattlePass::~CBattlePass()
{
}

int16_t GetQuestIndexFromString(std::string szGroup)
{
	std::array<std::string, 6> groupType = {"monster", "boss", "stone", "dungeon", "chat", "refine"};
	
	auto it = std::find_if(groupType.begin(), groupType.end(), [&](const std::string& tmpString) {
		return szGroup.find(tmpString) != std::string::npos && tmpString.find(szGroup) != std::string::npos;
	});
	
	if (it != groupType.end())
		return std::distance(groupType.begin(), it);
	
	return 0;
}

void CBattlePass::Reset()
{
	const auto& rkPCMap = CHARACTER_MANAGER::Instance().GetPCMap();
	
	for (const auto& [key, value] : rkPCMap)
	{
		if (auto pBattlePass = value->GetBattlePass(); pBattlePass)
			pBattlePass->Clear();
	}
}

void CBattlePass::Initialize()
{
	InitializeQuest();
	InitializeReward();
	InitializeLevel();
}

void CBattlePass::InitializeQuest()
{
	CBattlePass::SSectionQuest section;
	
	std::string fileName = LocaleService_GetBasePath() + "/battle_pass_quest.txt";
	std::ifstream fileInput(fileName);
	std::string fileLine;
	
	if (!fileInput.is_open())
	{
		sys_err("File '%s' could not be opened !!", fileName.c_str());
		return;
	}
	
	while (std::getline(fileInput, fileLine))
	{
		if (fileLine == "section")
		{
			section = {};
			
			while (std::getline(fileInput, fileLine))
			{
				if (fileLine == "end")
				{
					vQuest.push_back(section);
					break;
				}
				else
				{
					std::istringstream iss(fileLine);
					std::string key, value;
					iss >> key >> value;
					
					if (key == "index")
						section.questIndex = std::stoi(value);
					
					else if (key == "group")
						section.questGroup = GetQuestIndexFromString(value);
						
					else if (key == "count")
						section.questCount = std::stoi(value);
						
					else if (key == "exp")
						section.questExp = std::stoi(value);
				}
			}
		}
	}
}

void CBattlePass::InitializeReward()
{
	CBattlePass::SSectionReward section;
	
	std::string fileName = LocaleService_GetBasePath() + "/battle_pass_reward.txt";
	std::ifstream fileInput(fileName);
	std::string fileLine;
	
	if (!fileInput.is_open())
	{
		sys_err("File '%s' could not be opened !!", fileName.c_str());
		return;
	}
	
	while (std::getline(fileInput, fileLine))
	{
		if (fileLine == "section")
		{
			section = {};
			
			while (std::getline(fileInput, fileLine))
			{
				if (fileLine == "end")
				{
					vReward.push_back(section);
					break;
				}
				else
				{
					std::istringstream iss(fileLine);
					std::string key, value;
					iss >> key >> value;
					
					if (key == "level")
						section.rewardLevel = std::stoi(value);
					
					else if (key == "item_free")
						section.rewardItem[0] = std::stoi(value);
					
					else if (key == "count_free")
						section.rewardCount[0] = std::stoi(value);
					
					else if (key == "item_premium")
						section.rewardItem[1] = std::stoi(value);
					
					else if (key == "count_premium")
						section.rewardCount[1] = std::stoi(value);
				}
			}
		}
	}
}

void CBattlePass::InitializeLevel()
{
	std::unique_ptr<CGroupTextParseTreeLoader> loader(new CGroupTextParseTreeLoader());
	
	std::string fileName = LocaleService_GetBasePath() + "/battle_pass_level.txt";
	std::filesystem::path path(fileName);
	
	if (!loader->Load(fileName.c_str()))
	{
		sys_err("File '%s' could not be opened !!", fileName.c_str());
		return;
	}
	else
	{
		CGroupNode* levelGroup = loader->GetGroup("main_level");
		
		if (!levelGroup)
		{
			sys_err("Group 'main_level' does not exist !!");
			return;
		}
		
		if (!levelGroup->GetRowCount())
		{
			sys_err("Group 'main_level' is empty !!");
			return;
		}
		
		for (auto i = 1; i <= levelGroup->GetRowCount(); ++i)
		{
			int32_t exp;
			const auto line = std::to_string(static_cast<int>(i));
			
			if (!levelGroup->GetValue(line, "exp", exp))
			{
				sys_err("Group 'main_level' error in line: %s", line.c_str());
				return;
			}
			
			SSectionLevel data;
			
			data.level = i;
			data.exp = exp;
			
			vLevel.push_back(data);
		}
	}
	
	BattlePassMaxLevel = vLevel.size();
}