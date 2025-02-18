 /*
 /\><><><><><><><><><><><><><><><><><><><><><><><><><><><>/\
 \/        |===================================|          \/
 /\        |       System: Pety Expione v1     |          /\
 \/        |    Udost�pniony przez kamil2321   |          \/
 /\        |===================================|          /\
 \/                     GG: 1889862                       \/
 /\             strona: https://kamil2321.pl              /\
 \/             Forum: https://metin2time.eu              \/
 /\                   Google: kamil2321                   /\
 \/                  YouTube: kamil2321                   \/
 /\        |===================================|          /\
 \/                  Do u�ytku w�asnego!                  \/
 /\    W razie problem�w/ poprawek itp. prosz� pisa�.     /\
 \/><><><><><><><><><><><><><><><><><><><><><><><><><><><>\/
*/
#ifndef	__HEADER_PET_SYSTEM__
#define	__HEADER_PET_SYSTEM__


class CHARACTER;

// TODO: �����μ��� �ɷ�ġ? ����� ģ�е�, ����� ��Ÿ���... ��ġ
struct SPetAbility
{
};

/**
*/
class CPetActor //: public CHARACTER
{
public:
	enum EPetOptions
	{
		EPetOption_Followable		= 1 << 0,
		EPetOption_Mountable		= 1 << 1,
		EPetOption_Summonable		= 1 << 2,
		EPetOption_Combatable		= 1 << 3,
	};


protected:
	friend class CPetSystem;

	CPetActor(LPCHARACTER owner, DWORD vnum, DWORD options = EPetOption_Followable | EPetOption_Summonable);
//	CPetActor(LPCHARACTER owner, DWORD vnum, const SPetAbility& petAbility, DWORD options = EPetOption_Followable | EPetOption_Summonable);

	virtual ~CPetActor();

	virtual bool	Update(DWORD deltaTime);

protected:
	virtual bool	_UpdateFollowAI();				///< ������ ����ٴϴ� AI ó��
	virtual bool	_UpdatAloneActionAI(float fMinDist, float fMaxDist);			///< ���� ��ó���� ȥ�� ��� AI ó��

	/// @TODO
	//virtual bool	_UpdateCombatAI();

private:
	bool Follow(float fMinDistance = 50.f);

public:
	LPCHARACTER		GetCharacter()	const					{ return m_pkChar; }
	LPCHARACTER		GetOwner()	const						{ return m_pkOwner; }
	DWORD			GetVID() const							{ return m_dwVID; }
	DWORD			GetVnum() const							{ return m_dwVnum; }

	bool			HasOption(EPetOptions option) const		{ return m_dwOptions & option; }

	void			SetName(const char* petName);
#ifdef ENABLE_NEW_PET_SYSTEM
	void            SetLevel(BYTE petLevel);
	const char *	GetName() const { return m_name.c_str(); }
#endif

	bool			Mount();
	void			Unmount();

#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD			Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar = false, BYTE level = 1);
	void			Unsummon(bool isExcept = false);
#else
	DWORD			Summon(const char* petName, LPITEM pSummonItem, bool bSpawnFar = false);
	void			Unsummon();
#endif

	bool			IsSummoned() const			{ return 0 != m_pkChar; }
	void			SetSummonItem (LPITEM pItem);
	DWORD			GetSummonItemVID () { return m_dwSummonItemVID; }
	// ���� �ִ� �Լ��� �ŵδ� �Լ�.
	// �̰� �� �����Ѱ�, ������ ����,
	// POINT_MOV_SPEED, POINT_ATT_SPEED, POINT_CAST_SPEED�� PointChange()�� �Լ��� �Ἥ ������ ���� �ҿ��� ���°�,
	// PointChange() ���Ŀ� ��𼱰� ComputePoints()�� �ϸ� �ϴ� �ʱ�ȭ�ǰ�,
	// �� �����, ComputePoints()�� �θ��� ������ Ŭ���� POINT�� ���� ������ �ʴ´ٴ� �Ŵ�.
	// �׷��� ������ �ִ� ���� ComputePoints() ���ο��� petsystem->RefreshBuff()�� �θ����� �Ͽ���,
	// ������ ���� ���� ClearBuff()�� �θ���, ComputePoints�� �ϴ� ������ �Ѵ�.
	void			GiveBuff();
	void			ClearBuff();
private:
	DWORD			m_dwVnum;
	DWORD			m_dwVID;
	DWORD			m_dwOptions;
	DWORD			m_dwLastActionTime;
	DWORD			m_dwSummonItemVID;
	DWORD			m_dwSummonItemVnum;

	short			m_originalMoveSpeed;

	std::string		m_name;
#ifdef ENABLE_NEW_PET_SYSTEM
	BYTE            m_level;
#endif

	LPCHARACTER		m_pkChar;					// Instance of pet(CHARACTER)
	LPCHARACTER		m_pkOwner;

//	SPetAbility		m_petAbility;				// �ɷ�ġ
};

/**
*/
class CPetSystem
{
public:
	typedef	boost::unordered_map<DWORD,	CPetActor*>		TPetActorMap;		/// <VNUM, PetActor> map. (�� ĳ���Ͱ� ���� vnum�� ���� ������ ���� ���� ������..??)

public:
	CPetSystem(LPCHARACTER owner);
	virtual ~CPetSystem();

	CPetActor*	GetByVID(DWORD vid) const;
	CPetActor*	GetByVnum(DWORD vnum) const;

	bool		Update(DWORD deltaTime);
	void		Destroy();

	size_t		CountSummoned() const;			///< ���� ��ȯ��(��üȭ �� ĳ���Ͱ� �ִ�) ���� ����

public:
	void		SetUpdatePeriod(DWORD ms);

#ifdef ENABLE_NEW_PET_SYSTEM
	CPetActor*	Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, BYTE level = 1, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);
#else
	CPetActor*	Summon(DWORD mobVnum, LPITEM pSummonItem, const char* petName, bool bSpawnFar, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);
#endif

	void		Unsummon(DWORD mobVnum, bool bDeleteFromList = false);
	void		Unsummon(CPetActor* petActor, bool bDeleteFromList = false);

	// TODO: ��¥ �� �ý����� �� �� ����. (ĳ���Ͱ� ������ ���� ������ �߰��� �� �����...)
	CPetActor*	AddPet(DWORD mobVnum, const char* petName, const SPetAbility& ability, DWORD options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable | CPetActor::EPetOption_Combatable);

	void		DeletePet(DWORD mobVnum);
	void		DeletePet(CPetActor* petActor);
	void		RefreshBuff();

#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD GetPetModel() const;
	void SummonPet();
	void UpdatePet();
	void AddPetAffect();
	void SendPetInfo(bool summon = false);
	void SetExp(int count);
	bool CheckExtraBonus();
	int GetLevel() const;
	int GetExp(bool exp_need = false, bool lv_only = false) const;
	void SetSkillUp(int skill_id, bool set_from_quest = false);
	std::string GetOrgName() const;
	bool SetExpPercent(int amount);

#endif

private:
	TPetActorMap	m_petActorMap;
	LPCHARACTER		m_pkOwner;					///< �� �ý����� Owner
	DWORD			m_dwUpdatePeriod;			///< ������Ʈ �ֱ� (ms����)
	DWORD			m_dwLastUpdateTime;
	LPEVENT			m_pkPetSystemUpdateEvent;
};

/**
// Summon Pet
CPetSystem* petSystem = mainChar->GetPetSystem();
CPetActor* petActor = petSystem->Summon(~~~);

DWORD petVID = petActor->GetVID();
if (0 == petActor)
{
	ERROR_LOG(...)
};


// Unsummon Pet
petSystem->Unsummon(petVID);

// Mount Pet
petActor->Mount()..


CPetActor::Update(...)
{
	// AI : Follow, actions, etc...
}

*/

#ifdef ENABLE_NEW_PET_SYSTEM_BOOK
namespace NewPetSystemHelper
{
// Structs
struct SSkillBookInfo
{
	WORD wSkillID;
	BYTE bSkillGrade;
	BYTE bChance;
	time_t ttDelay;
};

// Variables
extern const BYTE SKILL_BONUS_VALUE;
extern const BYTE PET_MAX_SKILL_LV;

// Public functions
void AddBookRecord(const DWORD dwVnum, const WORD wSkillID, const BYTE bSkillGrade, const BYTE bChance, const time_t ttDelay);
void AddBookCount(const WORD wLevel, const WORD wCount);
bool FindBookByVnum(const DWORD dwVnum);
const SSkillBookInfo & GetBookInfo(const DWORD dwVnum);
BYTE GetSkillLevel(LPCHARACTER ch, const WORD wSkillID);
BYTE GetSkillGrade(const WORD wSkillLevel);
time_t GetSkillDelay(LPCHARACTER ch, const WORD wSkillID, const WORD wSkillLevel);
void SetSkillDelay(LPCHARACTER ch, const WORD wSkillID, const WORD wSkillLevel, const time_t ttDelay);
int TrainSkill(LPCHARACTER ch, const WORD wSkillID);
}
#endif	//ENABLE_NEW_PET_SYSTEM_BOOK

#endif	//__HEADER_PET_SYSTEM__
