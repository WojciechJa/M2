#ifndef __INC_METIN_II_GAME_DUNGEON_H
#define __INC_METIN_II_GAME_DUNGEON_H

#include "sectree_manager.h"

class CParty;
#ifdef ENABLE_D_NJGUILD
class CGuild;
#endif

#ifdef ENABLE_DUNGEON_RETURN
	#include <unordered_set>
	
	static const BYTE DUNGEON_RETURN_TIME = 3*60; // 3 min
#endif

class CDungeon
{
	typedef TR1_NS::unordered_map<LPPARTY, int> TPartyMap;
	typedef std::map<std::string, LPCHARACTER> TUniqueMobMap;

	public:
	// <Factor> Non-persistent identifier type
	typedef uint32_t IdType;

	~CDungeon();

	// <Factor>
	IdType GetId() const { return m_id; }

	// DUNGEON_NOTICE
	void	Notice(const char* msg);
	// END_OF_DUNGEON_NOTICE

	void	JoinParty(LPPARTY pParty);
	void	QuitParty(LPPARTY pParty);

	void	Join(LPCHARACTER ch);

	void	IncMember(LPCHARACTER ch);
	void	DecMember(LPCHARACTER ch);

	// DUNGEON_KILL_ALL_BUG_FIX
	void	Purge();
	void	KillAll();
	// END_OF_DUNGEON_KILL_ALL_BUG_FIX

	void	IncMonster() { m_iMonsterCount++; sys_log(0, "MonsterCount %d", m_iMonsterCount); }
	void	DecMonster() { m_iMonsterCount--; CheckEliminated(); }
	int	CountMonster() { return m_iMonsterCount; }
	int	CountRealMonster();

	void	IncPartyMember(LPPARTY pParty, LPCHARACTER ch);
	void	DecPartyMember(LPPARTY pParty, LPCHARACTER ch);

	void	IncKillCount(LPCHARACTER pkKiller, LPCHARACTER pkVictim);
	int	GetKillMobCount();
	int	GetKillStoneCount();
	bool	IsUsePotion();
	bool	IsUseRevive();
	void	UsePotion(LPCHARACTER ch);
	void	UseRevive(LPCHARACTER ch);

	long	GetMapIndex() { return m_lMapIndex; }

	void	Spawn(DWORD vnum, const char* pos);
	LPCHARACTER	SpawnMob(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER	SpawnMob_ac_dir(DWORD vnum, int x, int y, int dir = 0);
	LPCHARACTER	SpawnGroup(DWORD vnum, long x, long y, float radius, bool bAggressive=false, int count=1);

	void	SpawnNameMob(DWORD vnum, int x, int y, const char* name);
	void	SpawnGotoMob(long lFromX, long lFromY, long lToX, long lToY);

	void	SpawnRegen(const char* filename, bool bOnce = true);
	void	AddRegen(LPREGEN regen);
	void	ClearRegen();
	bool	IsValidRegen(LPREGEN regen, size_t regen_id);

	void	SetUnique(const char* key, DWORD vid);
	void	SpawnMoveUnique(const char* key, DWORD vnum, const char* pos_from, const char* pos_to);
	void	SpawnMoveGroup(DWORD vnum, const char* pos_from, const char* pos_to, int count=1);
	void	SpawnUnique(const char* key, DWORD vnum, const char* pos);
	void	SpawnStoneDoor(const char* key, const char* pos);
	void	SpawnWoodenDoor(const char* key, const char* pos);
	void	KillUnique(const std::string& key);
	void	PurgeUnique(const std::string& key);
	bool	IsUniqueDead(const std::string& key);
	float	GetUniqueHpPerc(const std::string& key);
	DWORD	GetUniqueVid(const std::string& key);

	void	DeadCharacter(LPCHARACTER ch);

	void	UniqueSetMaxHP(const std::string& key, int iMaxHP);
	void	UniqueSetHP(const std::string& key, int iHP);
	void	UniqueSetDefGrade(const std::string& key, int iGrade);

	void	SendDestPositionToParty(LPPARTY pParty, long x, long y);

	void	CheckEliminated();

	void	JumpAll(long lFromMapIndex, int x, int y);
	void	WarpAll(long lFromMapIndex, int x, int y);
	void	JumpParty(LPPARTY pParty, long lFromMapIndex, int x, int y);
#ifdef ENABLE_D_NJGUILD
	void	JumpGuild(CGuild* pGuild, long lFromMapIndex, int x, int y);
#endif

	void	ExitAll();
	void	ExitAllToStartPosition();
	void	JumpToEliminateLocation();
	void	SetExitAllAtEliminate(long time);
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	void	SetWarpAtEliminate(long time, long lMapIndex, int x, int y, const char* regen_file, DWORD bStage, DWORD bStageMax, const char* strNotice);
#else
	void	SetWarpAtEliminate(long time, long lMapIndex, int x, int y, const char* regen_file);
#endif

	int	GetFlag(std::string name);
	void	SetFlag(std::string name, int value);
	void	SetWarpLocation (long map_index, int x, int y);

	typedef std::vector <std::pair <DWORD, int> > ItemGroup;
	void	CreateItemGroup (std::string& group_name, ItemGroup& item_group);
	const ItemGroup* GetItemGroup (std::string& group_name);
	//void	InsertItemGroup (std::string& group_name, DWORD item_vnum);

	template <class Func> Func ForEachMember(Func f);

	bool IsAllPCNearTo( int x, int y, int dist );

	protected:
	CDungeon(IdType id, long lOriginalMapIndex, long lMapIndex);

	void	Initialize();
	void	CheckDestroy();

	private:
	IdType 		m_id; // <Factor>
	DWORD		m_lOrigMapIndex;
	DWORD		m_lMapIndex;

	CHARACTER_SET	    m_set_pkCharacter;
	std::map<std::string, int>  m_map_Flag;
	typedef std::map<std::string, ItemGroup> ItemGroupMap;
	ItemGroupMap m_map_ItemGroup;
	TPartyMap	m_map_pkParty;
	TAreaMap&	m_map_Area;
	TUniqueMobMap	m_map_UniqueMob;

	int		m_iMobKill;
	int		m_iStoneKill;
	bool		m_bUsePotion;
	bool		m_bUseRevive;

	int		m_iMonsterCount;

	bool		m_bExitAllAtEliminate;
	bool		m_bWarpAtEliminate;

	int		m_iWarpDelay;
	long		m_lWarpMapIndex;
	long		m_lWarpX;
	long		m_lWarpY;
	std::string	m_stRegenFile;

	std::vector<LPREGEN> m_regen;

	LPEVENT		deadEvent;
	// <Factor>
	LPEVENT exit_all_event_;
	LPEVENT jump_to_event_;
	size_t regen_id_;

	friend class CDungeonManager;
	friend EVENTFUNC(dungeon_dead_event);
	// <Factor>
	friend EVENTFUNC(dungeon_exit_all_event);
	friend EVENTFUNC(dungeon_jump_to_event);

	LPPARTY m_pParty;
#ifdef ENABLE_D_NJGUILD
	CGuild* m_pGuild;
#endif
	public :
	void SetPartyNull();
#ifdef ENABLE_DUNGEON_RETURN
	public:
		void	AddAttenderByPID(const DWORD & dwPID);
		bool	CheckAttenderByPID(const DWORD & dwPID);
		void	UpdateCoords(LPCHARACTER ch, bool bClear = false);
		std::tuple<long, long, long> GetDungeonCoords(LPCHARACTER ch);

	private:
		std::unordered_set<DWORD> s_attender_pid;
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
	public:
		void			SetMiniMapDungeonInfoStatus(DWORD bStatus);
		DWORD		GetMiniMapDungeonInfoStatus() { return m_bMiniMapDungeonInfoStatus; }
		
		void			SetMiniMapDungeonInfoStage(DWORD curStage, DWORD maxStage);
		DWORD		GetMiniMapDungeonInfoStage(int bListNum);
		
		void			SetMiniMapDungeonInfoGauge(DWORD type, DWORD value1, DWORD value2);
		DWORD		GetMiniMapDungeonInfoGauge(int bListNum);

		void			SetMiniMapDungeonInfoTimer(DWORD status, DWORD time);
		DWORD		GetMiniMapDungeonInfoTimer() { return timer_status; }
		
		void				SetMiniMapDungeonInfoNotice(const char* strNotice);
		const char*	GetMiniMapDungeonInfoNotice() const;

		void			SetMiniMapDungeonInfoButton(DWORD status);
		DWORD		GetMiniMapDungeonInfoButton() { return button_status; }

		DWORD		GetMiniMapDungeonInfoTime() { return time; }
	private:
		DWORD			m_bMiniMapDungeonInfoStatus;

		DWORD			button_status;
		DWORD			timer_status;
		DWORD			time;

		DWORD			m_iMiniMapDungeonInfoStageInfo[2];
		DWORD			m_iMiniMapDungeonInfoGaugeInfo[3];
		const char*	m_strMiniMapDungeonInfoNotice;
		
		DWORD			m_iMiniMapDungeonInfoAtEliminateStageInfo[2];
		const char*	m_strMiniMapDungeonInfoAtEliminateNotice;
#endif

	public:
		LPPARTY GetParty() const { return m_pParty; }
};

class CDungeonManager : public singleton<CDungeonManager>
{
	typedef std::map<CDungeon::IdType, LPDUNGEON> TDungeonMap;
	typedef std::map<long, LPDUNGEON> TMapDungeon;

	public:
	CDungeonManager();
	virtual ~CDungeonManager();

	LPDUNGEON	Create(long lOriginalMapIndex);
	void		Destroy(CDungeon::IdType dungeon_id);
	LPDUNGEON	Find(CDungeon::IdType dungeon_id);
	LPDUNGEON	FindByMapIndex(long lMapIndex);

	private:
	TDungeonMap	m_map_pkDungeon;
	TMapDungeon m_map_pkMapDungeon;

	// <Factor> Introduced unsigned 32-bit dungeon identifier
	CDungeon::IdType next_id_;
#ifdef ENABLE_DUNGEON_RETURN
	public:
		LPDUNGEON FindDungeonByPID(const DWORD & dwPID);
#endif
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
