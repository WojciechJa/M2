#pragma once

#include "../../common/tables.h"
#include "../../common/length.h"

class CBattlePassManager
{
	public:
		CBattlePassManager(const LPCHARACTER pkChar); ~CBattlePassManager();
		
		int16_t BATTLE_PASS_MAX_LEVEL = 0;
		
		struct SQuest
		{
			int32_t	questIndex;
			int32_t	questGroup;
			int32_t	questCount;
			int32_t	questCountMax;
			int8_t	questStatus;
		};
		
		struct SReward
		{
			int16_t	rewardLevel;
			int8_t	rewardStatus[2];
		};
		
		struct SInfo
		{
			int16_t	level;
			int32_t exp;
			int16_t premium;
		};
		
		std::vector<SQuest> vQuest;
		std::vector<SReward> vReward;
		std::vector<SInfo> vInfo;

	public:
		static void		Create(const LPCHARACTER pkChar);
		
		// void			AnalyzeQuestSignal(EBattlePassTypes signal);
		
		int32_t			GetExp(int16_t level);
		int16_t			GetMaxLevel();
		
		void			SetExp(int32_t value);
		void			SetReward(int16_t reward_level, int8_t reward_type);

		bool			GetPremium() const { return vInfo[0].premium; }
		void			SetPremium() { vInfo[0].premium = 1; }

		void			SetQuest();
		void			SetLevel();
		
		void			SendReward();
		void			SendQuest();
		void			SendLevel();
		
		void			SendQuestData();
		void			SendRewardData();
		
		void			SendQuestUpdate(int32_t index, int32_t count, int8_t status);
		
		void			InsertQuest(int32_t index, int32_t count, int8_t status);
		void			InsertReward(int16_t level, int8_t status_free, int8_t status_premium);
		void			InsertInfo(int16_t level, int32_t exp, int16_t premium);
		
		// void			RefreshReward();
		void			CollectReward(const char* pData);
		bool			CanCollectReward(int16_t reward_level, int8_t reward_type);
		
		void			Save();
		void			Clear();
		
	private:
		LPCHARACTER		m_pkChar;
		int32_t			expNext;
};