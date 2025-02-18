#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);

	public:
		CItem(DWORD dwVnum);
		virtual ~CItem();

		int			GetLevelLimit();
		int			GetRealUseLimit();

		bool		CheckItemUseLevel(int nLevel);

		long		FindApplyValue(BYTE bApplyType);

		bool		IsStackable()		{ return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false; }

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow(BYTE b)	{ m_bWindow = b; }
		BYTE		GetWindow()		{ return m_bWindow; }

		void		SetID(DWORD id)		{ m_dwID = id;	}
		DWORD		GetID()			{ return m_dwID; }

		void			SetProto(const TItemTable * table);
		TItemTable const *	GetProto()	{ return m_pProto; }

#ifdef ENABLE_LONG_LONG
		long long		GetGold();
		long long		GetShopBuyPrice();
#else
		int		GetGold();
		int		GetShopBuyPrice();
#endif
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
		const char * GetName();
#else
		const char * GetName()			{ return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif
		const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
		BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

		void		SetFlag(long flag)	{ m_lFlag = flag;	}
		long		GetFlag()		{ return m_lFlag;	}

		void		AddFlag(long bit);
		void		RemoveFlag(long bit);

		DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
		DWORD		GetAntiFlag()		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
		DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		BYTE		GetSpecialInvType()	{ return m_pProto ? m_pProto->SpecialInvType : 0; }
#endif

		void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
		DWORD		GetVID()		{ return m_dwVID;	}

		bool		SetCount(DWORD count);
		DWORD		GetCount();

		DWORD		GetVnum() const		{ return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;	}
		DWORD		GetOriginalVnum() const		{ return m_dwVnum;	}
		BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
		BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
		BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
		long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}

		long		GetValue(DWORD idx);

		void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
		WORD		GetCell()				{ return m_wCell;	}

		LPITEM		RemoveFromCharacter();
		bool		AddToCharacter(LPCHARACTER ch, TItemPos Cell);
		LPCHARACTER	GetOwner()		{ return m_pOwner; }

		LPITEM		RemoveFromGround();
		bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

		int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM 
		int			FindBuffEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
#endif
		bool		IsEquipped() const		{ return m_bEquipped;	}
#ifdef ENABLE_NEW_PET_SYSTEM
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell, bool is_item_load = false);
#else
		bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
#endif
		bool		IsEquipable() const;

		bool		CanUsedBy(LPCHARACTER ch);

		bool		DistanceValid(LPCHARACTER ch);

		void		UpdatePacket();
		void		UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use * packet);

		void		SetExchanging(bool isOn = true);
		bool		IsExchanging()		{ return m_bExchanging;	}

		bool		IsTwohanded();

		bool		IsPolymorphItem();

		void		ModifyPoints(bool bAdd);

		bool		CreateSocket(BYTE bSlot, BYTE bGold);
		const long *	GetSockets()		{ return &m_alSockets[0];	}
		long		GetSocket(int i)	{ return m_alSockets[i];	}

		void		SetSockets(const long * al);
		void		SetSocket(int i, long v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	}
		const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

#if defined(__ITEM_APPLY_RANDOM__)
		const TPlayerItemAttribute* GetRandomApplies() { return m_aApplyRandom; }
		const TPlayerItemAttribute& GetRandomApply(BYTE i) { return m_aApplyRandom[i]; }
		TPlayerItemAttribute* GetNextRandomApplies();

		void SetRandomApplies(const TPlayerItemAttribute* c_pApplyRandom);

		BYTE GetRandomApplyType(BYTE i) { return m_aApplyRandom[i].bType; }
		short GetRandomApplyValue(BYTE i) { return m_aApplyRandom[i].sValue; }
		BYTE GetRandomApplyPath(BYTE i) { return m_aApplyRandom[i].bPath; }
#endif

		BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
		short		GetAttributeValue(int i){ return m_aAttr[i].sValue;	}

		void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);

		int		FindAttribute(BYTE bType);
		bool		RemoveAttributeAt(int index);
		bool		RemoveAttributeType(BYTE bType);

		bool		HasAttr(BYTE bApply);
		bool		HasRareAttr(BYTE bApply);

		void		SetDestroyEvent(LPEVENT pkEvent);
		void		StartDestroyEvent(int iSec=30);

		DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
		bool		GetSkipSave()		{ return m_bSkipSave; }

		bool		IsOwnership(LPCHARACTER ch);
		void		SetOwnership(LPCHARACTER ch, int iSec = 10);
		void		SetOwnershipEvent(LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
		void		SetLastOwnerPID(DWORD pid) { m_dwLastOwnerPID = pid; }
#endif

		int		GetAttributeSetIndex();
		void		AlterToMagicItem();
		void		AlterToSocketItem(int iSocketCount);

		WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent(LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

		void		StartRealTimeExpireEvent();
		bool		IsRealTimeItem();
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
		void		StartItemshopBonusTimeExpireEvent();
		void		SetItemshopBonusTimeExpireEvent(LPEVENT pkEvent);
		bool		IsItemShopBonusExpireItem();
#endif
		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();

		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void		ClearAttribute2();
		void 		ClearAllAttribute();
#ifdef ENABLE_VS_COSTUME_BONUSES
		void		ClearCostumeBonus();
#endif
		void		ChangeAttribute(const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute(BYTE bType, short sValue);
		
#ifdef ENABLE_BELT_ATTRIBUTES
		void		AddBeltAttribute();
		void		PutBeltAttribute(const int * aiAttrPercentTable);
		void		ChangeBeltAttribute(const int* aiChangeProb=NULL);
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
		void		AddGloveAttribute();
		bool		HasGloveAttr(BYTE bApply);
		void		PutGloveAttribute(const int * aiAttrPercentTable);
		void		ChangeGloveAttribute(const int* aiChangeProb=NULL);
		void		ClearGloveAttribute(int e);
#endif
		void		ApplyAddon(int iAddonType);

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup(const LPITEM item) const;

		bool	IsRingSameGroup(const LPITEM item) const;
		//test
		bool	IsSameSubType(const LPITEM item) const;

		// ACCESSORY_REFINE

		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade(int iGrade);
		void		SetAccessorySocketMaxGrade(int iMaxGrade);
		void		SetAccessorySocketDownGradeTime(DWORD time);

		void		AccessorySocketDegrade();

		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

		bool		CanPutInto(LPITEM item);
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo(LPITEM pItem);
		void		CopySocketTo(LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock(bool f) { m_isLocked = f; }
		bool		isLocked() const { return m_isLocked; }
#ifdef __RENEWAL_MOUNT__
		bool		IsCostumeMountItem();
#endif
	// private :
		// void		SetAttribute(int i, BYTE bType, short sValue);
	public:
		void		SetAttribute(int i, BYTE bType, short sValue);
#if defined(__ITEM_APPLY_RANDOM__)
		void		SetRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath);
#endif
		void		SetForceAttribute(int i, BYTE bType, short sValue);
#if defined(__ITEM_APPLY_RANDOM__)
		void		SetForceRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath);
#endif
#ifdef ENABLE_NEW_COSTUME_BONUS
	public:
		void ModifyCostumePoints(bool bAdd);
		void SetCostume();
		LPEVENT m_costumeTimeoutEvent;
#endif
#ifdef __ENABLE_TOGGLE_ITEMS__
	public:
		void		LoadToggleItem();
		bool		DeactivateToggleItem(bool bActionByUser = false);
		bool		ActivateToggleItem(bool bActionByUser = false);
		bool		IsToggleItem() const { return GetType() == ITEM_TOGGLE; }
		const		TItemLimit* FindLimit(uint8_t type) const;
#endif

	protected:
		bool		EquipEx(bool is_equip);
		bool		Unequip();

		void		AddAttr(BYTE bApply, BYTE bLevel);
		void		PutAttribute(const int * aiAttrPercentTable);
		void		PutAttributeWithLevel(BYTE bLevel);
#ifdef ENABLE_BELT_ATTRIBUTES
		void		PutBeltAttributeWithLevel(BYTE bLevel);
		void		AddBeltAttr(BYTE bApply, BYTE bLevel);
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
		void		PutGloveAttributeWithLevel(BYTE bLevel);
		void		AddGloveAttr(BYTE bApply, BYTE bLevel);
#endif

	public:
		bool		AddRareAttribute2(const int * aiAttrPercentTable = NULL);
		bool		ChangeRareAttribute2();
	protected:
		void		AddRareAttr(BYTE bApply, BYTE bLevel);
		void		PutRareAttribute(const int * aiAttrPercentTable);
		void		PutRareAttributeWithLevel(BYTE bLevel);

#ifdef __PREMIUM_PRIVATE_SHOP__
	public:
		void				BindPrivateShop(LPPRIVATE_SHOP pPrivateShop) { m_pPrivateShop = pPrivateShop; }
		LPPRIVATE_SHOP		GetPrivateShop() { return m_pPrivateShop; }

		void		SetGoldPrice(long long llGoldPrice) { m_llGoldPrice = llGoldPrice; }
		long long	GetGoldPrice() { return m_llGoldPrice; }

		void		SetChequePrice(DWORD dwChequePrice) { m_dwChequePrice = dwChequePrice; }
		DWORD		GetChequePrice() { return m_dwChequePrice; }

		void		SetCheckinTime(time_t tCheckin) { m_tPrivateShopCheckin = tCheckin; }
		time_t		GetCheckinTime() { return m_tPrivateShopCheckin; }
#endif

	public: // @fixme306 private -> public
		friend class CInputDB;
#ifdef __PREMIUM_PRIVATE_SHOP__
		friend class CPrivateShop;
		friend class CPrivateShopManager;
#endif
		bool		OnAfterCreatedItem();

	public:
		bool		IsPremiumBonusItem();
		bool		bonusOnlyByPremiumItem();
		bool		IsRideItem();
		bool		IsRamadanRing();

		void		ClearMountAttributeAndAffect();
		bool		IsNewMountItem();
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		bool		IsMountItem();
#endif

		void		SetMaskVnum(DWORD vnum)	{	m_dwMaskVnum = vnum; }
		DWORD		GetMaskVnum()			{	return m_dwMaskVnum; }
		bool		IsMaskedItem()	{	return m_dwMaskVnum != 0;	}

		bool		IsDragonSoul();
		int		GiveMoreTime_Per(float fPercent);
		int		GiveMoreTime_Fix(DWORD dwTime);
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		bool		IsSkillBook();
		bool		IsUpgradeItem();
		bool		IsStone();
		bool		IsBox();
		bool		IsEfsun();
		bool		IsCicek();
		DWORD		GetSpecialWindowType();
#endif
#ifdef ENABLE_EXTENDED_BLEND
		bool IsItemBlend();
		bool IsDragonGodItem();
		bool IsWaterItem();
#endif



	private:
		TItemTable const * m_pProto;

		DWORD		m_dwVnum;
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;
		DWORD		m_dwID;
		bool		m_bEquipped;
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;
		DWORD		m_dwCount;
		long		m_lFlag;
		DWORD		m_dwLastOwnerPID;

		bool		m_bExchanging;

		long		m_alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
		TPlayerItemAttribute	m_aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
		LPEVENT		m_pkRealItemShopBonusTimeExpireEvent;
#endif
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;

		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;

		DWORD		m_dwMaskVnum;
		DWORD		m_dwSIGVnum;

#ifdef __PREMIUM_PRIVATE_SHOP__
		LPPRIVATE_SHOP		m_pPrivateShop;
		long long			m_llGoldPrice;
		DWORD				m_dwChequePrice;
		time_t				m_tPrivateShopCheckin;
#endif
	public:
		void SetSIGVnum(DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	public:
		int32_t GetAcceVnum() {
			int32_t vnum = GetVnum();
			if (GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
				vnum += ACCE_EFFECT_VNUM;
			return vnum;
		}
		int32_t CalcAcceBonus(int32_t value) {
			return CalcAcceBonus(value, GetSocket(ACCE_ABSORPTION_SOCKET));
		}
		static int32_t CalcAcceBonus(int32_t value, uint32_t pct) {
			if (!pct || pct > 100)
				return 0;
			auto ret = (double)value * (double(pct) / 80.0);
			if (value > 0) {
				ret += 0.5;
				if (ret < 1)
					ret = 1;
			} else if (value < 0) {
				ret -= 0.5;
				if (ret > -1)
					ret = -1;
			}
			return (int32_t)ret;
		}
#endif
	public:
		bool GetUpdateStatus() { return m_bUpdateStatus; }
		void SetUpdateStatus(bool changing) { m_bUpdateStatus = changing; }
	private:
		bool m_bUpdateStatus;
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
	: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;

	item_vid_event_info()
	: item_vid( 0 )
	{
	}
};

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
