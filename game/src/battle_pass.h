class CBattlePass : public singleton<CBattlePass>
{
	public:
		CBattlePass(); ~CBattlePass();
		
		struct SSectionQuest
		{
			int32_t	questIndex;
			int16_t	questGroup;
			int32_t	questCount;
			int32_t	questExp;
		};
		
		struct SSectionReward
		{
			int16_t	rewardLevel;
			int32_t	rewardItem[2];
			int32_t	rewardCount[2];
		};
		
		struct SSectionLevel
		{
			int16_t level;
			int32_t exp;
		};
		
		std::vector<SSectionQuest> vQuest;
		std::vector<SSectionReward> vReward;
		std::vector<SSectionLevel> vLevel;
		
	public:
		void			Reset();
		
		void			Initialize();
		void			InitializeQuest();
		void			InitializeReward();
		void			InitializeLevel();
		
		int16_t			BattlePassMaxLevel = 0;
};