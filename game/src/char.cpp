#include "stdafx.h"

#include "../../common/VnumHelper.h"
#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "horsename_manager.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "skill_power.h"
#include "buff_on_attributes.h"
#include "skill.h"
#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif
#ifdef ENABLE_NEWSTUFF
#include "../../common/PulseManager.h"
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
#include "buff_npc_system.h"
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
#include "PetSystem.h"
#endif
#ifdef __ITEM_SHOP__
#include "itemshop_manager.h"
#endif
#ifdef __TOMBOLA__
#include "tombola_manager.h"
#endif
#include "DragonSoul.h"
#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offline_shop.h"
	#include "offlineshop_manager.h"
	#include "offlineshop_config.h"
extern bool offlineshop_map_allow_find(int mapIndex);
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
#include "MountSystem.h"
#endif

#include "../../common/CommonDefines.h"

#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#include "private_shop_util.h"
#endif

extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
			return false;
	}
	return true;
}

#ifdef NEW_ICEDAMAGE_SYSTEM
const DWORD CHARACTER::GetNoDamageRaceFlag()
{
	return m_dwNDRFlag;
}

void CHARACTER::SetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag>=MAIN_RACE_MAX_NUM) return;
	if (IS_SET(m_dwNDRFlag, 1<<dwRaceFlag)) return;
	SET_BIT(m_dwNDRFlag, 1<<dwRaceFlag);
}

void CHARACTER::UnsetNoDamageRaceFlag(DWORD dwRaceFlag)
{
	if (dwRaceFlag>=MAIN_RACE_MAX_NUM) return;
	if (!IS_SET(m_dwNDRFlag, 1<<dwRaceFlag)) return;
	REMOVE_BIT(m_dwNDRFlag, 1<<dwRaceFlag);
}

void CHARACTER::ResetNoDamageRaceFlag()
{
	m_dwNDRFlag = 0;
}

const std::set<DWORD> & CHARACTER::GetNoDamageAffectFlag()
{
	return m_setNDAFlag;
}

void CHARACTER::SetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.emplace(dwAffectFlag);
}

void CHARACTER::UnsetNoDamageAffectFlag(DWORD dwAffectFlag)
{
	m_setNDAFlag.erase(dwAffectFlag);
}

void CHARACTER::ResetNoDamageAffectFlag()
{
	m_setNDAFlag.clear();
}
#endif

// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const {
	LPCHARACTER p = NULL;
	if (is_pc) {
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	} else {
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character) {
	if (character == NULL) {
		Reset();
		return *this;
	}
	if (character->IsPC()) {
		is_pc = true;
		id = character->GetPlayerID();
	} else {
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

#if defined(__DUNGEON_INFO_SYSTEM__)
	m_bDungeonInfoOpen = false;
	m_pkDungeonInfoReloadEvent = NULL;
#endif

	m_fSyncTime = get_float_time()-3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;

	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData		= NULL;
	m_pkMobInst		= NULL;

	m_pkShop		= NULL;
	m_pkChrShopOwner	= NULL;
	m_pkMyShop		= NULL;
	m_pkExchange	= NULL;
	m_pkParty		= NULL;
	m_pkPartyRequestEvent = NULL;

	m_pGuild = NULL;

	m_pkChrTarget = NULL;

	m_pkMuyeongEvent = NULL;

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_pkBleedingEvent = NULL;
#endif
	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent	= NULL;
	m_speed_hack_count	= 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

#ifdef ENABLE_LRN_BATTLE_PASS
	m_pkBattlePass = NULL;
#endif

	m_points = {};
	m_pointsInstant = {};

	m_bCharType = CHAR_TYPE_MONSTER;

#ifdef ENABLE_SYSTEMY_KARTY_OKEY
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));
#endif

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;
#if defined(BL_OFFLINE_MESSAGE)
	dwLastOfflinePMTime = 0;
#endif
	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;
	m_iMoveWaitTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;
#ifdef WEEKLY_RANK_BYLUZER
	m_CooldownWeekly = 0;
	m_WeeklyLoadCount = 0;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	m_pTitle = NULL;
	m_CooldownTitle = 0;
#endif
	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time()-180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;
#ifdef ENABLE_WOLFMAN_CHARACTER
	m_bHasBled = false;
#endif
	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	dwLastSuccessShootTime = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_dwQuestItemVID = 0;
#ifdef ENABLE_QUEST_DND_EVENT
	m_dwQuestDNDItemVID = 0;
#endif

	m_szMobileAuth[0] = '\0';

	m_dwUnderGuildWarInfoMessageTime = get_dword_time()-60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;
	m_bMountCounter = 0;

	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW

	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	m_iSafeboxLoadTime = 0;
	m_iMoveWaitTime = 0;

	m_iMyShopTime = 0;

	InitMC();

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;

#ifdef ENABLE_CHEQUE_SYSTEM
	m_dwLastChequeDropTime = 0;
#endif

	m_dwMountTime = 0;
#ifdef __RENEWAL_MOUNT__
	m_bIsHorse = false;
#endif
	m_bIsLoadedAffect = false;
	cannot_dead = false;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	m_buffNPCSystem = 0;
	m_bIsBuffNPC = false;
	m_buffSkillRecoverHPValue = 0;
#endif

#ifdef __PET_SYSTEM__
	m_petSystem = 0;
	m_bIsPet = false;
#endif
#ifdef __ITEM_SHOP__
	m_iLastItemShopTime = 0;
	m_bHasWheelData = false;
#endif

#ifdef __TOMBOLA__
	m_pkTombolaRewardEvent = nullptr;
#endif
#ifdef NEW_ICEDAMAGE_SYSTEM
	m_dwNDRFlag = 0;
	m_setNDAFlag.clear();
#endif

#ifdef ENABLE_SWITCHBOT
	use_item_anti_flood_count_ = 0;
	use_item_anti_flood_pulse_ = 0;
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	m_mountSystem = 0;
	m_bIsMount = false;
#endif
#ifdef ENABLE_ANTI_PACKET_FLOOD
	analyze_protect = 0;
	analyze_protect_count = 0;
#endif
	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
	m_bHideAcceCostume = false;
	m_bHideWeaponCostume = false;
	m_bHideAuraCostume = false;
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	m_pkOfflineShop = NULL;
	m_pkChrOfflineShopOwner = NULL;
	m_pkOfflineShopUpdateEvent = NULL;
	m_offline_vip = 30001;
	m_offline_norm = 30000;//Do not touch	
#endif	

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_bAcceCombination	= false;
	m_bAcceAbsorption	= false;
#endif
#ifdef ENABLE_AURA_SYSTEM
	m_bAuraRefine = false;
	m_bAuraAbsorption = false;
#endif
#ifdef ENABLE_REAL_TIME_REGEN
	m_wRealTimeRegenNum = 0;
#endif
	m_iEquipItemsTime = 0;
	m_iEquipItemsCount = 0;
	m_iEquipItemsLastTime = 0;
	m_iPlayerDamageTime = 0;
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	m_LegendLastDamageInfo = 0;
#endif

	skipUpdatePacket_ = false;

#ifdef __PREMIUM_PRIVATE_SHOP__
	m_pPrivateShop = nullptr;
	m_pMyPrivateShop = nullptr;
	m_dwPrivateShopOwner = 0;
	m_bIsEditingPrivateShop = false;
	memset(&m_privateShopTable, 0, sizeof(TPrivateShop));

	m_bShopSearchMode = MODE_NONE;

	m_dwCurrentPrivateShopTime = 0;

	m_tLastPrivateShopModify		= 0;
	m_tLastPrivateShopWithdraw		= 0;
	m_tLastPrivateShopClose			= 0;
	m_tLastPrivateShopBuild			= 0;
	m_tLastPrivateShopBuy			= 0;
	m_tLastPrivateShopSearch		= 0;
	m_tLastPrivateShopStateChange	= 0;
#endif

}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128+1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
		m_stName = c_pszName;
}

void CHARACTER::Destroy()
{
	CloseMyShop();

	if (m_pkRegen)
	{
		if (m_pkDungeon) {
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
				--m_pkRegen->count;
			}
		} else {
			// Is this really safe? NO IT ISNT! F*CK THAT SH!T!
			if (is_valid_regen(m_pkRegen))
				--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;

		m_mountSystem = 0;
		m_bIsMount = false;
	}

	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
	HorseSummon(false);
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
	SetBattlePass(NULL);
#endif

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (m_buffNPCSystem)
	{
		m_buffNPCSystem->Destroy();
		delete m_buffNPCSystem;

		m_buffNPCSystem = 0;
		m_bIsBuffNPC = false;
	}
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
//		BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL);
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
#ifdef ENABLE_WOLFMAN_CHARACTER
	event_cancel(&m_pkBleedingEvent);
#endif
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	//DELAYED_WARP
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	//END_DELAYED_WARP

	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING
#if defined(__DUNGEON_INFO_SYSTEM__)
	StopDungeonInfoReloadEvent();
#endif

#ifdef __TOMBOLA__
	if (m_pkTombolaRewardEvent)
	{
		tombola_reward_event_info* info = dynamic_cast<tombola_reward_event_info*>(m_pkTombolaRewardEvent->info);
		M2_DESTROY_ITEM(info->item);
		event_cancel(&m_pkTombolaRewardEvent);
	}
#endif

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}
	
#ifdef TITLE_SYSTEM_BYLUZER
	if (m_pTitle)
	{
		M2_DELETE_ARRAY(m_pTitle);
		m_pTitle = NULL;
	}
#endif

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
}

#ifdef __PET_SYSTEM__
bool CHARACTER::SummonedPet()
{
	if (m_petSystem->CountSummoned() > 0)
		return true;
	else
		return false;
}
#endif

const char * CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
}
#ifdef __EXTENDED_ITEM_COUNT__
void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, int bItemCount)
#else
void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, BYTE bItemCount)
#endif
{
	if (!CanHandleItem()) // @fixme149
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� �ŷ���(â��,��ȯ,����)���� ���λ����� ����� �� �����ϴ�."));
		return;
	}

#ifndef ENABLE_OPEN_SHOP_WITH_ARMOR
	if (GetPart(PART_MAIN) > 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ����� ���� ������ �� �� �ֽ��ϴ�."));
		return;
	}
#endif

	if (GetMyShop())
	{
		CloseMyShop();
		return;
	}

	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

#ifdef ENABLE_LONG_LONG
	long long nTotalMoney = 0;

#ifdef ENABLE_CHEQUE_SYSTEM
	long long nTotalCheque = 0;
#endif

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<long long>((pTable+n)->price);
#ifdef ENABLE_CHEQUE_SYSTEM
		nTotalCheque += static_cast<long long>((pTable + n)->cheque);
#endif
	}

	nTotalMoney += static_cast<long long>(GetGold());
#ifdef ENABLE_CHEQUE_SYSTEM
	nTotalCheque += static_cast<long long>(GetCheque());
#endif
#else
	int64_t nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable + n)->price);
	}

	nTotalMoney += static_cast<int64_t>(GetGold());
#endif

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("20�� ���� �ʰ��Ͽ� ������ ������ �����ϴ�"));
		return;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if (CHEQUE_MAX <= nTotalCheque)
	{
		sys_err("[OVERFLOW_CHEQUE] Overflow (CHEQUE_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't create a shop with more than %d won."), CHEQUE_MAX-1);
		return;
	}
#endif

	char szSign[SHOP_SIGN_MAX_LEN+1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��Ӿ ��� ���Ե� ���� �̸����� ������ �� �� �����ϴ�."));
		return;
	}

	// MYSHOP_PRICE_LIST
	std::map<DWORD, DWORD> itemkind;
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
#ifdef __EXTENDED_ITEM_COUNT__
	for (int i = 0; i < bItemCount; ++i)
#else
	for (BYTE i = 0; i < bItemCount; ++i)
#endif
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����ȭ �������� ���λ������� �Ǹ��� �� �����ϴ�."));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������� �������� ���λ������� �Ǹ��� �� �����ϴ�."));
				return;
			}

			if (true == pkItem->isLocked())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������� �������� ���λ������� �Ǹ��� �� �����ϴ�."));
				return;
			}

			// MYSHOP_PRICE_LIST
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.emplace((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST

	if (CountSpecifyItem(71049)) {
		// @fixme403 BEGIN
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		size_t idx=0;
		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
			header.aPriceInfo[idx].dwPrice = it->second;
			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
		// @fixme403 END
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return;

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon( false, true );
	}
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

	SetPolymorph(30000, true);
}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';

		PacketAround(&p, sizeof(p));
#ifdef ENABLE_WOLFMAN_CHARACTER
		SetPolymorph(m_points.job, true);
		// SetPolymorph(0, true);
#else
		SetPolymorph(GetJob(), true);
#endif
	}
}

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc   = bFunc;
	pack.bArg    = bArg;
	pack.dwVID   = dwVID;
	pack.dwTime  = dwTime ? dwTime : get_dword_time();
	pack.bRot    = bRot;
	pack.lX		= x;
	pack.lY		= y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster() )
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

#ifdef ENABLE_SKILE_PASYWNE
void CHARACTER::LoadPasywkiBonus()
{
	if (GetSkillLevel(SKILL_HUMAN_BONUS) == 40 && 
	GetSkillLevel(SKILL_MONSTER_BONUS) == 40 && 
	GetSkillLevel(SKILL_STONE_BONUS) == 40 && 
	GetSkillLevel(SKILL_DUNG_BONUS) == 40 && 
	GetSkillLevel(SKILL_BOSS_BONUS) == 40 && 
	GetSkillLevel(SKILL_EXP_BONUS) == 40 && 
	GetSkillLevel(SKILL_ODP_UM) == 40 && 
	GetSkillLevel(SKILL_SREDNIE_BONUS) == 40 && 
	GetSkillLevel(SKILL_STAT_BONUS) == 40 && 
	GetSkillLevel(SKILL_HP_BONUS) == 40 && 
	GetSkillLevel(SKILL_DO_PZ_BONUS) == 40) {
		if (!FindAffect(AFFECT_PASYWKI_EXTRABONUS)) {
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ATTBONUS_HUMAN, 9, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ATTBONUS_MONSTER, 9, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ATTBONUS_STONE, 4.5, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ATTBONUS_DUNGEON, 4.5, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ATTBONUS_BOSS, 4.5, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_MALL_EXPBONUS, 6, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_SKILL_DEFEND_BONUS, 3, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_NORMAL_HIT_DAMAGE_BONUS, 6, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_HT, 4, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_IQ, 4, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_ST, 4, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_DX, 4, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_MAX_HP, 900, AFF_NONE, 60*60*60*365, 0, false);
			AddAffect(AFFECT_PASYWKI_EXTRABONUS, POINT_STEAL_HP, 3, AFF_NONE, 60*60*60*365, 0, false);
		}
	}
}

void CHARACTER::LoadPasywki()
{
	CSkillProto* pkSk;
	int add_skill = 0;
	{
		pkSk = CSkillManager::instance().Get(SKILL_HUMAN_BONUS);   // Silny na ludzi
		
		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_HUMAN_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_ATTBONUS_HUMAN,add_skill);
		}
	}
	{
		pkSk = CSkillManager::instance().Get(SKILL_MONSTER_BONUS);   // Silny na potwory

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_MONSTER_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_ATTBONUS_MONSTER,add_skill);
		}
	}

	{
		pkSk = CSkillManager::instance().Get(SKILL_STONE_BONUS);   // Silny na metiny

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_STONE_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_ATTBONUS_STONE,add_skill);
		}
	}
	
	{
		pkSk = CSkillManager::instance().Get(SKILL_DUNG_BONUS);   // Silny na dung

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_DUNG_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_ATTBONUS_DUNGEON,add_skill);
		}
	}

	{
		pkSk = CSkillManager::instance().Get(SKILL_BOSS_BONUS);   // Silny na bossy

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_BOSS_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_ATTBONUS_BOSS,add_skill);
		}
	}
	
	{
		pkSk = CSkillManager::instance().Get(SKILL_EXP_BONUS);   // Do�wiadczenie

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_EXP_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_MALL_EXPBONUS,add_skill);
		}
	}
	
	{
		pkSk = CSkillManager::instance().Get(SKILL_ODP_UM);   // Odporno�?na umiej?no�?

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ODP_UM) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_SKILL_DEFEND_BONUS,add_skill);
		}
	}
	
	{
		pkSk = CSkillManager::instance().Get(SKILL_SREDNIE_BONUS);   // �rednie

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_SREDNIE_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_NORMAL_HIT_DAMAGE_BONUS,add_skill);
		}
	}
	

	{
		pkSk = CSkillManager::instance().Get(SKILL_STAT_BONUS);   // Statystyki

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_STAT_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_HT,add_skill);
			PointChange(POINT_IQ,add_skill);
			PointChange(POINT_ST,add_skill);
			PointChange(POINT_DX,add_skill);
		}
	}
	{
		pkSk = CSkillManager::instance().Get(SKILL_HP_BONUS);   // hp

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_HP_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_MAX_HP,add_skill);
		}
	}
	{
		pkSk = CSkillManager::instance().Get(SKILL_DO_PZ_BONUS);   // do p?

		if (NULL != pkSk)
		{
			pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_DO_PZ_BONUS) / 100.0f);

			add_skill = static_cast<int>(pkSk->kPointPoly.Eval());
			PointChange(POINT_STEAL_HP,add_skill);
		}
	}
}
#endif

void CHARACTER::ConvertTime(DWORD sec_new)
{
    long hour = sec_new / 3600;
    sec_new -= hour * 3600;
    long min = (sec_new / 60);
    sec_new -= min * 60;
    char buf[128+1];

    if (hour > 0 && min > 0)
        snprintf(buf, sizeof(buf), LC_TEXT("%d h %d m %d s"), hour, min, sec_new);
    else if (hour > 0 && min == 0)
        snprintf(buf, sizeof(buf), LC_TEXT("%d h %d s"), hour, sec_new);
    else if (hour == 0 && min > 0)
        snprintf(buf, sizeof(buf), LC_TEXT("%d m %d s"), min, sec_new);
    else
        snprintf(buf, sizeof(buf), LC_TEXT("%d s"), sec_new);

    ChatPacket(CHAT_TYPE_INFO, buf);
}

#ifdef ENABLE_NEW_PET_SYSTEM
void CHARACTER::SendPetLevelUpEffect(int vid, int type, int value, int amount)
{
	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = vid;
	pack.type = type;
	pack.value = value;
	pack.amount = amount;
	PacketAround(&pack, sizeof(pack));
}
#endif

// #define ENABLE_SHOWNPCLEVEL
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{
	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	LPCHARACTER ch = (LPCHARACTER) entity;
	ch->SendGuildName(GetGuild());

	TPacketGCCharacterAdd pack;

	pack.header		= HEADER_GC_CHARACTER_ADD;
	pack.dwVID		= m_vid;
	pack.bType		= GetCharType();
	pack.angle		= GetRotation();
	pack.x		= GetX();
	pack.y		= GetY();
	pack.z		= GetZ();
	pack.wRaceNum	= GetRaceNum();
	if (IsPet())
	{
		pack.bMovingSpeed	= 150;
	}
	else
	{
		pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (IsPC() == true || m_bCharType == CHAR_TYPE_NPC || m_bCharType == CHAR_TYPE_HORSE || IsBuffNPC() == true)
#else
	if (IsPC() == true || m_bCharType == CHAR_TYPE_NPC)
#endif
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
		addPacket.awPart[CHR_EQUIPPART_EMBLEMAT] = GetPart(PART_EMBLEMAT);
#endif
#ifdef ENABLE_AURA_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		if (IsBuffNPC() == true) {
			addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetBuffWearWeapon();
			addPacket.awPart[CHR_EQUIPPART_HAIR] = GetBuffWearHead();
			addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetBuffWearArmor();
		}
#endif
		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
#ifdef ENABLE_QUIVER_SYSTEM
		addPacket.dwArrow = (IsPC()&&GetWear(WEAR_ARROW)!=NULL)?GetWear(WEAR_ARROW)->GetOriginalVnum():0;
#endif
		addPacket.bEmpire = m_bEmpire;

#if defined(ENABLE_SHOWNPCLEVEL) && defined (ENABLE_NEW_PET_SYSTEM)
		if (1 || IsPet())
#else
#ifdef ENABLE_NEW_PET_SYSTEM
		if (IsPC() == true || IsPet())
#else
		if (IsPC() == true)
#endif // ENABLE_NEW_PET_SYSTEM

#endif //ENABLE_SHOWNPCLEVEL && ENABLE_NEW_PET_SYSTEM
		{
			addPacket.dwLevel = GetLevel();
		}
		else
		{
			addPacket.dwLevel = 0;
		}

		if (false)
		{
			LPCHARACTER ch = (LPCHARACTER) entity;

			if (GetEmpire() == ch->GetEmpire() || ch->GetGMLevel() > GM_PLAYER || m_bCharType == CHAR_TYPE_NPC || IsHorse())
			{
				goto show_all_info;
			}
			else
			{
				memset(addPacket.name, 0, CHARACTER_NAME_MAX_LEN);
				addPacket.dwGuildID = 0;
#ifdef TITLE_SYSTEM_BYLUZER
				addPacket.title_active = 99;
#endif
				addPacket.sAlignment = 0;
			}
		}
		else
		{
		show_all_info:
			strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

			if (GetGuild() != NULL)
			{
				addPacket.dwGuildID = GetGuild()->GetID();
			}
			else
			{
				addPacket.dwGuildID = 0;
			}

			addPacket.sAlignment = m_iAlignment;
#ifdef TITLE_SYSTEM_BYLUZER
			BYTE active_title_now = 99;
			if (m_pTitle)
			{
				for (int i = 0; i < TITLE_MAX_NUM; i++)
				{
					if (m_pTitle[i].active == 2)
					{
						active_title_now = i;
						break;
					}
				}
			}
			addPacket.title_active = active_title_now;
#endif
		}

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (GetMyShop())
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));

		d->Packet(&p, sizeof(TPacketGCShopSign));
	}
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if ((IsOfflineShopNPC() && GetRaceNum() >= 30000) || (GetRaceNum() <= 30007))
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
		p.dwVID = GetVID();
		if (m_stOfflineShopSign.empty())
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT sign FROM srv1_player.offline_shop_npc WHERE owner_id = %u", GetOfflineShopRealOwner()));
			if (pMsg->Get()->uiNumRows > 0)
			{				
				MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
				strlcpy(p.szSign, row[0], sizeof(p.szSign));				
			}
		}
		else
		{
			strlcpy(p.szSign, m_stOfflineShopSign.c_str(), sizeof(p.szSign));
		}
		
		d->Packet(&p, sizeof(TPacketGCShopSign));
	}
#endif
	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
				GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (IsSkipUpdatePacket() || !GetSectree())
		return;

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	pack.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	pack.awPart[CHR_EQUIPPART_EMBLEMAT] = GetPart(PART_EMBLEMAT);
#endif
#ifdef ENABLE_AURA_SYSTEM
	pack.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (IsBuffNPC() == true) {
		pack.awPart[CHR_EQUIPPART_WEAPON] = GetBuffWearWeapon();
		pack.awPart[CHR_EQUIPPART_HAIR] = GetBuffWearHead();
		pack.awPart[CHR_EQUIPPART_ARMOR] = GetBuffWearArmor();
	}
#endif
	pack.bMovingSpeed	= GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed	= GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag	= m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwGuildID	= 0;
	pack.sAlignment	= m_iAlignment;
	pack.dwLevel = GetLevel();
	pack.bPKMode	= m_bPKMode;

	if (GetGuild())
		pack.dwGuildID = GetGuild()->GetID();
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE active_title_now = 99;
	if (m_pTitle)
	{
		for (int i = 0; i < TITLE_MAX_NUM; i++)
		{
			if (m_pTitle[i].active == 2)
			{
				active_title_now = i;
				break;
			}
		}
	}
	pack.title_active = active_title_now;
#endif


	pack.dwMountVnum	= GetMountVnum();
#ifdef ENABLE_QUIVER_SYSTEM
	pack.dwArrow = GetWear(WEAR_ARROW) != NULL ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;
	pack2.dwLevel = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] �ο�� ����");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] ���� ����");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id			= m_dwPlayerID;
	tab.voice		= GetPoint(POINT_VOICE);
	tab.level		= GetLevel();
	tab.level_step	= GetPoint(POINT_LEVEL_STEP);
	tab.exp			= GetExp();
	tab.gold		= GetGold();
#ifdef ENABLE_CHEQUE_SYSTEM
	tab.cheque 		= GetCheque();
#endif
	tab.job			= m_points.job;
	tab.part_base	= m_pointsInstant.bBasePart;
	tab.skill_group	= m_points.skill_group;
#ifdef WEEKLY_RANK_BYLUZER
	tab.w_monsters = GetWeekly(1);
	tab.w_bosses = GetWeekly(2);
	tab.w_metins = GetWeekly(3);
	tab.w_dungs = GetWeekly(4);
	tab.w_ds = GetWeekly(5);
	tab.w_refine = GetWeekly(6);
	tab.w_sm = GetWeekly(7);
	tab.weekly_season = GetWeekly(8);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	tab.pkt_osiag = GetPktOsiag();
#endif
	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	// if (dwPlayedTime > 60000)
	// {
		// if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		// {
			// if (GetRealAlignment() < 0)
			// {
				// if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					// UpdateAlignment(120 * (dwPlayedTime / 60000));
				// else
					// UpdateAlignment(60 * (dwPlayedTime / 60000));
			// }
			// else
				// UpdateAlignment(5 * (dwPlayedTime / 60000));
		// }

		// SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		// ResetPlayTime(dwPlayedTime % 60000);
	// }

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex	= GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex	= tab.lMapIndex;
		tab.lExitX		= tab.x;
		tab.lExitY		= tab.y;
	}
	else
	{
		tab.lExitMapIndex	= m_lExitMapIndex;
		tab.lExitX		= m_posExit.x;
		tab.lExitY		= m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	if (m_PlayerSlots)
	{
		for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
			tab.quickslot[i] = m_PlayerSlots->pQuickslot[i];
	}

	if (m_stMobile.length() && !*m_szMobileAuth)
		strlcpy(tab.szMobile, m_stMobile.c_str(), sizeof(tab.szMobile));

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES
#ifdef TITLE_SYSTEM_BYLUZER
	thecore_memcpy(tab.titles, m_pTitle, sizeof(TPlayerTitle) * TITLE_MAX_NUM);
#endif
	tab.horse = GetHorseData();
#ifdef __PREMIUM_PRIVATE_SHOP__
	tab.wPrivateShopUnlockedSlot = GetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT);
#endif
}

void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

#ifdef ENABLE_LRN_BATTLE_PASS
	if (auto pBattlePass = GetBattlePass(); pBattlePass)
		pBattlePass->Save();
#endif

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );
#ifdef ENABLE_NEW_PET_SYSTEM
	RemoveAffect(AFFECT_NEW_PET);
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	RemoveAffect(AFFECT_TITLES);
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	RemoveAffect(AFFECT_NEW_MOUNT);
#endif
#ifdef ENABLE_SKILE_PASYWNE
	if (FindAffect(AFFECT_PASYWKI_EXTRABONUS)) {
		RemoveAffect(AFFECT_PASYWKI_EXTRABONUS);
	}
#endif
#ifdef ENABLE_RESP_SYSTEM
	CRespManager::instance().LogoutFromMap(this);
#endif
#ifdef __ITEM_SHOP__
	CItemShopManager::instance().RemoveViewer(this);
    m_bHasWheelData = false;
#endif

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}
	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (GetOfflineShop())
	{
		GetOfflineShop()->RemoveGuest(this);
		SetOfflineShop(NULL);
	}
#endif


#ifdef __PREMIUM_PRIVATE_SHOP__
	if (GetViewingPrivateShop())
	{
		GetViewingPrivateShop()->RemoveShopViewer(this);
		SetViewingPrivateShop(nullptr);
	}

	//	@note: DB does not get notified about characters disconnect when a player is going back to
	//			the select character window.
	#ifdef ENABLE_PRIVATE_SHOP_PREMIUM_TIME
		if (IsPrivateShopOwner() && strcmp(c_pszReason, "timed_event - SCMD_PHASE_SELECT") == 0)
		{
			// Update db
			BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_LOGOUT;
			DWORD dwPID = GetPlayerID();

			db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, 0, sizeof(BYTE) + sizeof(DWORD));
			db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
			db_clientdesc->Packet(&dwPID, sizeof(DWORD));
		}
	#endif

	if (m_dwCurrentPrivateShopTime > 0)
	{
		AddAffect(AFFECT_PREMIUM_PRIVATE_SHOP, POINT_NONE, 0, 0, m_dwCurrentPrivateShopTime, 0, true);
		m_aiPremiumTimes[PREMIUM_PRIVATE_SHOP] = m_dwCurrentPrivateShopTime + time(0);

		BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_PREMIUM_TIME_UPDATE;

		TPacketGDPrivateShopPremiumTimeUpdate packet{};
		packet.dwAID = GetDesc()->GetAccountTable().id;
		packet.dwPID = GetPlayerID();
		packet.tPremiumTime = m_dwCurrentPrivateShopTime;

		db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, 0, sizeof(BYTE) + sizeof(TPacketGDPrivateShopPremiumTimeUpdate));
		db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
		db_clientdesc->Packet(&packet, sizeof(TPacketGDPrivateShopPremiumTimeUpdate));
	}
#endif

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);
#ifdef ENABLE_DUNGEON_RETURN
	if (GetDungeon())
		GetDungeon()->UpdateCoords(this);
#endif
	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));

#ifdef ENABLE_CHEQUE_SYSTEM
	char buf[51];
	snprintf(buf, sizeof(buf), "%s %lld %lld %d %ld %lld",
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), GetCheque(), g_bChannel, GetMapIndex(), GetAlignment());
#endif

	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	LogManager::instance().CharLog(this, 0, "LOGOUT", "");

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

	if (GetParty())
		GetParty()->Unlink(this);

	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}

	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;
	m_bSkipSave = true;

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
#endif

	if (GetDesc())
		GetDesc()->BindCharacter(NULL);

	M2_DESTROY_CHARACTER(this);
}

bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup(
			#ifdef ENABLE_GOTO_LAG_FIX
			IsPC()
			#endif
		);
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "	  in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string	name;
	float		vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap 	gs_bgmInfoMap;
static bool		gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	// SUPPORT_BGM
	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
		//if (m_stMobile.length())
		//		ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
	// END_OF_SUPPORT_BGM
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));

		if (m_stMobile.length())
			ChatPacket(CHAT_TYPE_COMMAND, "sms");
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL]		= GetLevel();
	pack.points[POINT_EXP]		= GetExp();
	pack.points[POINT_NEXT_EXP]		= GetNextExp();
	pack.points[POINT_HP]		= GetHP();
	pack.points[POINT_MAX_HP]		= GetMaxHP();
	pack.points[POINT_SP]		= GetSP();
	pack.points[POINT_MAX_SP]		= GetMaxSP();
	pack.points[POINT_GOLD]		= GetGold();
	pack.points[POINT_STAMINA]		= GetStamina();
	pack.points[POINT_MAX_STAMINA]	= GetMaxStamina();
#ifdef WEEKLY_RANK_BYLUZER
	pack.points[POINT_WEEKLY1] = GetWeekly(1);
	pack.points[POINT_WEEKLY2] = GetWeekly(2);
	pack.points[POINT_WEEKLY3] = GetWeekly(3);
	pack.points[POINT_WEEKLY4] = GetWeekly(4);
	pack.points[POINT_WEEKLY5] = GetWeekly(5);
	pack.points[POINT_WEEKLY6] = GetWeekly(6);
	pack.points[POINT_WEEKLY7] = GetWeekly(7);
	pack.points[POINT_WEEKLY_SEASON] = GetWeekly(8);
#endif

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);

#ifdef ENABLE_CHEQUE_SYSTEM
	pack.points[POINT_CHEQUE] = GetCheque();
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	pack.points[POINT_PKT_OSIAG] = GetPktOsiag();
#endif
	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.job = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.job = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.job = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.job = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.job = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.job = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.job = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.job = MAIN_RACE_SHAMAN_M;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
			m_points.job = MAIN_RACE_WOLFMAN_M;
			break;
#endif
		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

DWORD CHARACTER::GetRaceNum() const // @fixme501
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

#define ENABLE_GM_FLAG_IF_TEST_SERVER
#define ENABLE_GM_FLAG_FOR_LOW_WIZARD
void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_PlayerSlots = std::make_unique<PlayerSlotT>(); // @fixme199
	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, t->parts[PART_ACCE]);
#endif
#ifdef ENABLE_AURA_SYSTEM
	SetPart(PART_AURA, t->parts[PART_AURA]);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	SetPart(PART_EMBLEMAT, t->parts[PART_EMBLEMAT]);
#endif
	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES
	
#ifdef TITLE_SYSTEM_BYLUZER
	if (m_pTitle)
		M2_DELETE_ARRAY(m_pTitle);

	m_pTitle = M2_NEW TPlayerTitle[TITLE_MAX_NUM];
	thecore_memcpy(m_pTitle, t->titles, sizeof(TPlayerTitle) * TITLE_MAX_NUM);
#endif

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#ifdef ENABLE_CHEQUE_SYSTEM
	SetCheque(t->cheque);
#endif
	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);
#ifdef WEEKLY_RANK_BYLUZER
	SetWeekly(1, t->w_monsters);
	SetWeekly(2, t->w_bosses);
	SetWeekly(3, t->w_metins);
	SetWeekly(4, t->w_dungs);
	SetWeekly(5, t->w_ds);
	SetWeekly(6, t->w_refine);
	SetWeekly(7, t->w_sm);
	SetWeekly(8, t->weekly_season);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	SetPktOsiag(t->pkt_osiag);
#endif
	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

#ifndef ENABLE_GM_FLAG_IF_TEST_SERVER
	if (!test_server)
#endif
	{
#ifdef ENABLE_GM_FLAG_FOR_LOW_WIZARD
		if (GetGMLevel() > GM_PLAYER)
#else
		if (GetGMLevel() > GM_LOW_WIZARD)
#endif
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	m_stMobile = t->szMobile;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

#ifdef __PREMIUM_PRIVATE_SHOP__
	m_dwCurrentPrivateShopTime = m_aiPremiumTimes[PREMIUM_PRIVATE_SHOP] - time(0);
#endif

	m_dwLogOffInterval = t->logoff_interval;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#ifdef __PET_SYSTEM__
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;
	}

	m_mountSystem = M2_NEW CMountSystem(this);
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	if (m_buffNPCSystem)
	{
		m_buffNPCSystem->Destroy();
		delete m_buffNPCSystem;
	}

	m_buffNPCSystem = M2_NEW CBuffNPCActor(this);
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SetOfflineShopVID(COfflineShopManager::instance().FindMyOfflineShop(GetPlayerID()));
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
	if (m_pkBattlePass == NULL)
		CBattlePassManager::Create(this);
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	SetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, t->wPrivateShopUnlockedSlot);
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	// if (IsStone())
	// {
	// 	DetermineDropMetinStone();
	// }

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		SetPoint(POINT_DEF_GRADE_BONUS, 6);

		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE ||
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109 ||
#ifdef __RENEWAL_MOUNT__
			GetRaceNum() == 20900 ||
			GetRaceNum() == 20901 ||
			GetRaceNum() == 20902 ||
			GetRaceNum() == 20903 ||
			GetRaceNum() == 20204 ||
			GetRaceNum() == 20905 ||
			GetRaceNum() == 20906 ||
			GetRaceNum() == 20907 ||
			GetRaceNum() == 20908 ||
			GetRaceNum() == 20909 ||
			GetRaceNum() == 20910 ||
			GetRaceNum() == 20911 ||
			GetRaceNum() == 20912 ||
			GetRaceNum() == 20913
#endif
	  )
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SetOfflineShopVID(COfflineShopManager::instance().FindMyOfflineShop(GetPlayerID()));
#endif
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f;

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT;

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
		default:
			return m_pkMobData->m_table.wAttackRange;
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	//autopotka fix
	// if (IsPolymorphed())
	// {
	// 	DWORD dwMobVnum = GetPolymorphVnum();
	// 	const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
	// 	int iAtt = 0;
	// 	int iDef = 0;

	// 	if (pMob)
	// 	{
	// 		//iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
	// 		iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2 + GetPolymorphPoint(POINT_DX) * 2;
	// 		// lev + con
	// 		iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
	// 	}

	// 	SetPoint(POINT_ATT_GRADE, iAtt);
	// 	SetPoint(POINT_DEF_GRADE, iDef);
	// 	SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
	// 	SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	// }
	if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		// ATK = 2lev + 2str

		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;

			case JOB_ASSASSIN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
#endif
			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		// ATK Setting

		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT);
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}

		if( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	SetSkipUpdatePacket(true);

	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);
#ifdef __PREMIUM_PRIVATE_SHOP__
	long lUnlockedSlots = GetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT);
#endif

#ifdef WEEKLY_RANK_BYLUZER
	long weekly1 = GetWeekly(1);
	long weekly2 = GetWeekly(2);
	long weekly3 = GetWeekly(3);
	long weekly4 = GetWeekly(4);
	long weekly5 = GetWeekly(5);
	long weekly6 = GetWeekly(6);
	long weekly7 = GetWeekly(7);
	long weekly_season = GetWeekly(8);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	long lPktOsiag = GetPktOsiag();
#endif
	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	SetPart(PART_EMBLEMAT, GetOriginalPart(PART_EMBLEMAT));
#endif
#ifdef ENABLE_AURA_SYSTEM
	SetPart(PART_AURA, GetOriginalPart(PART_AURA));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

#ifdef __PREMIUM_PRIVATE_SHOP__
	SetPoint(POINT_PRIVATE_SHOP_UNLOCKED_SLOT, lUnlockedSlots);
#endif

#ifdef WEEKLY_RANK_BYLUZER
	SetPoint(POINT_WEEKLY1, weekly1);
	SetPoint(POINT_WEEKLY2, weekly2);
	SetPoint(POINT_WEEKLY3, weekly3);
	SetPoint(POINT_WEEKLY4, weekly4);
	SetPoint(POINT_WEEKLY5, weekly5);
	SetPoint(POINT_WEEKLY6, weekly6);
	SetPoint(POINT_WEEKLY7, weekly7);
	SetPoint(POINT_WEEKLY_SEASON, weekly_season);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	SetPoint(POINT_PKT_OSIAG, lPktOsiag);
#endif
	long long iMaxHP, iMaxSP;
	int iMaxStamina;

#ifdef ENABLE_SKILE_PASYWNE
	LoadPasywki();
#endif

	if (IsPC())
	{
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
			}
		}

		SetPoint(POINT_MOV_SPEED,	150);
		SetPoint(POINT_ATT_SPEED,	150);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED,	100);
	}
	
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		if (GetMountVnum())
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
		}

	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (IsPC())
	{
		if (GetMountVnum() && !GetWear(WEAR_COSTUME_MOUNT))
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));
		}

	}
#endif

	ComputeBattlePoints();

	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP);
	}

	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP);
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);
	// @fixme118 part1
	int iCurHP = this->GetHP();
	int iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	if (IsPC())
	{
		for (int i = 0 ; i < WEAR_MAX_NUM; i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				pItem->ModifyPoints(true);
				SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
			}
		}

#ifdef ENABLE_EVENT_MANAGER
		CHARACTER_MANAGER::Instance().CheckBonusEvent(this);
#endif

#ifdef __ENABLE_TOGGLE_ITEMS__
		for (int32_t i = 0; i < INVENTORY_MAX_NUM; i++)
		{
			CItem* pItem = GetInventoryItem(i);
			if (!pItem)
				continue;
	
			if (!pItem->IsToggleItem())
				continue;
	
			if (!pItem->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
				continue;
	
			pItem->ModifyPoints(true);
		}
#endif

		if (DragonSoul_IsDeckActivated())
		{
			for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
				i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
			{
				LPITEM pItem = GetWear(i);
				if (pItem)
				{
					if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
						pItem->ModifyPoints(true);
				}
			}
		}
	}

	ComputeSkillPoints();

	RefreshAffect();

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	
	if (IsPC())
	{
		CPetSystem * pPetSystem = GetPetSystem();
		if (pPetSystem)
			pPetSystem->RefreshBuff();

		// @fixme118 part2 (after petsystem stuff)
		if (this->GetHP() != iCurHP)
			this->PointChange(POINT_HP, iCurHP-this->GetHP());
		if (this->GetSP() != iCurSP)
			this->PointChange(POINT_SP, iCurSP-this->GetSP());
	}

	SetSkipUpdatePacket(false);
	UpdatePacket();
}

void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->IsPC())
	{
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
// #ifdef ENABLE_WOLFMAN_CHARACTER
// 		if (ch->IsAffectFlag(AFF_BLEEDING))
// 			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
// #endif
		if (2493 == ch->GetMobTable().dwVnum)
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					regenPct += (val*cnt);

					break;
				}
			}

			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val*cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		ch->CheckTarget();
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			return 3;
		}
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (ch->IsAffectFlag(AFF_BLEEDING))
			return 3;
#endif
		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop())
		return false;

	if (IsDead())
		return false;

	/*
	   if (get_float_time() - m_fSyncTime < 0.2f)
	   return false;
	 */
	return true;
}

bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);
	if (!new_tree)
	{
		if (!GetDesc())
		{
			sys_err("No tree %s %ld %ld %ld", GetName(), x, y, GetMapIndex());
			Dead();
			return false;
		}
		
		x = GetX();
		y = GetY();
		new_tree = GetSectree();
		return false;
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (!IsNPC())
		{
			SECTREEID id = new_tree->GetID();
			SECTREEID old_id = GetSectree()->GetID();

			const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
			sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
		}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] ����");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;

	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "�������" : "�׳��̵�");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}

DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case WEAPON_CLAW:
				dwMode = MOTION_MODE_CLAW;
				break;
#endif
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		// sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
			(int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

bool CHARACTER::Move(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
}

long long CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
				case POINT_ST:
					if ((GetJob() == JOB_SHAMAN) || ((GetJob() == JOB_SURA) && (GetSkillGroup() == 2)))
						return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

#ifdef ENABLE_LONG_LONG
long long CHARACTER::GetPoint(BYTE type) const
#else
int CHARACTER::GetPoint(BYTE type) const
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

#ifdef ENABLE_LONG_LONG
	long long val = m_pointsInstant.points[type];
	long long max_val = INT_MAX;
#else	
	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
#endif

	switch (type)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			max_val = 50;
			break;
	}
	
	if (val > max_val)
#ifdef ENABLE_MAX_YANG
		sys_err("POINT_ERROR: %s type %d val %d (max: %lld)", GetName(), val, max_val);
#else		
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);
#endif

	return (val);
}

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
		case POINT_ATT_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 170;
			else
				limit = 250;
			break;

		case POINT_MOV_SPEED:
			min_limit = 0;

			if (IsPC())
				limit = 200;
			else
				limit = 250;
			break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			limit = 50;
			max_val = 50;
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
			limit = 20;
			max_val = 50;
			break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);

	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}

#ifdef ENABLE_LONG_LONG
void CHARACTER::SetPoint(BYTE type, long long val)
#else
void CHARACTER::SetPoint(BYTE type, int val)
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

#ifdef ENABLE_LONG_LONG
long long CHARACTER::GetAllowedGold() const
#else
INT CHARACTER::GetAllowedGold() const
#endif
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

#ifdef ENABLE_LONG_LONG
void CHARACTER::PointChange(BYTE type, long long amount, bool bAmount, bool bBroadcast)
{
	long long val = 0;
#else
void CHARACTER::PointChange(BYTE type, int amount, bool bAmount, bool bBroadcast)
{
	int val = 0;
#endif

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
		case POINT_NONE:
			return;

		case POINT_LEVEL:
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());
#ifdef ENABLE_WOLFMAN_CHARACTER
			if (GetJob() == JOB_WOLFMAN)
			{
				if ((5 <= val) && (GetSkillGroup()!=1))
				{
					ClearSkill();
					// set skill group
					SetSkillGroup(1);
					// set skill points
					SetRealPoint(POINT_SKILL, GetLevel()-1);
					SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
					PointChange(POINT_SKILL, 0);
					// update points (not required)
					// ComputePoints();
					// PointsPacket();
				}
			}
#endif
			PointChange(POINT_NEXT_EXP,	GetNextExp(), false);

			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
				{
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());
				}

				if (GetParty())
				{
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
				}
			}
			break;

		case POINT_NEXT_EXP:
			val = GetNextExp();
			bAmount = false;
			break;

		case POINT_EXP:
			{
				DWORD exp = GetExp();
				DWORD next_exp = GetNextExp();

				if (g_bChinaIntoxicationCheck)
				{
					if (IsOverTime(OT_NONE))
					{
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
					}
				}

				if ((amount < 0) && (exp < (DWORD)(-amount)))
				{
					sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
					amount = -exp;

					SetExp(exp + amount);
					val = GetExp();
				}
				else
				{
					if (gPlayerMaxLevel <= GetLevel())
						return;

					if (IsAntyExp())
						return;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "You have gained %d exp.", amount);

					DWORD iExpBalance = 0;

					if (exp + amount >= next_exp)
					{
						iExpBalance = (exp + amount) - next_exp;
						amount = next_exp - exp;

						SetExp(0);
						exp = next_exp;
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}

					DWORD q = DWORD(next_exp / 4.0f);
					int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

					if (iLevStep >= 4)
					{
						sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
						iLevStep = 4;
					}

					if (exp >= next_exp && iLevStep < 4)
					{
						for (int i = 0; i < 4 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 3 && iLevStep < 3)
					{
						for (int i = 0; i < 3 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 2 && iLevStep < 2)
					{
						for (int i = 0; i < 2 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q && iLevStep < 1)
						PointChange(POINT_LEVEL_STEP, 1);

					if (iExpBalance)
					{
						PointChange(POINT_EXP, iExpBalance);
					}

					val = GetExp();
				}
			}
			break;

		case POINT_LEVEL_STEP:
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						if ((GetLevel() <= g_iStatusPointGetLevelLimit) &&
							(GetLevel() <= gPlayerMaxLevel) ) // @fixme104
							PointChange(POINT_STAT, 1);
						break;

					case 4:
						{
							int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
							int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

							m_points.iRandomHP += iHP;
							m_points.iRandomSP += iSP;

							if (GetSkillGroup())
							{
								if (GetLevel() >= 5)
									PointChange(POINT_SKILL, 1);

								if (GetLevel() >= 9)
									PointChange(POINT_SUB_SKILL, 1);
							}

							PointChange(POINT_MAX_HP, iHP);
							PointChange(POINT_MAX_SP, iSP);
							PointChange(POINT_LEVEL, 1, false, true);

							val = 0;
						}
						break;
				}

// 				if (GetLevel() <= 10)
// 					AutoGiveItem(27001, 2);
// 				else if (GetLevel() <= 30)
// 					AutoGiveItem(27002, 2);
// 				else
// 				{
// 					AutoGiveItem(27002, 2);
// //					AutoGiveItem(27003, 2);
// 				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);

			break;

		case POINT_HP:
			{
				if (IsDead() || IsStun())
					return;

				int prev_hp = GetHP();

				amount = MIN(GetMaxHP() - GetHP(), amount);
				SetHP(GetHP() + amount);
				val = GetHP();

				BroadcastTargetPacket();

				if (GetParty() && IsPC() && val != prev_hp)
					GetParty()->SendPartyInfoOneToAll(this);
			}
			break;

		case POINT_SP:
			{
				if (IsDead() || IsStun())
					return;

				amount = MIN(GetMaxSP() - GetSP(), amount);
				SetSP(GetSP() + amount);
				val = GetSP();
			}
			break;

		case POINT_STAMINA:
			{
				if (IsDead() || IsStun())
					return;

				int prev_val = GetStamina();
				amount = MIN(GetMaxStamina() - GetStamina(), amount);
				SetStamina(GetStamina() + amount);
				val = GetStamina();

				if (val == 0)
				{
					// Stamina
					SetNowWalking(true);
				}
				else if (prev_val == 0)
				{
					ResetWalking();
				}

				if (amount < 0 && val != 0)
					return;
			}
			break;

		case POINT_MAX_HP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxHP(GetMaxHP() + amount);
				// ??? ?????? = (?? ??? ?????? + ???) * ????????%
				int hp = GetRealPoint(POINT_MAX_HP);
				int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
				add_hp += GetPoint(POINT_MAX_HP);
				add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

				SetMaxHP(hp + add_hp);

				val = GetMaxHP();
			}
			break;

		case POINT_MAX_SP:
			{
				SetPoint(type, GetPoint(type) + amount);

				//SetMaxSP(GetMaxSP() + amount);
				// ??? ???y? = (?? ??? ???y? + ???) * ??????y?%
				int sp = GetRealPoint(POINT_MAX_SP);
				int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
				add_sp += GetPoint(POINT_MAX_SP);
				add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

				SetMaxSP(sp + add_sp);

				val = GetMaxSP();
			}
			break;

		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;
#ifdef WEEKLY_RANK_BYLUZER
		case POINT_WEEKLY1:
		case POINT_WEEKLY2:
		case POINT_WEEKLY3:
		case POINT_WEEKLY4:
		case POINT_WEEKLY5:
		case POINT_WEEKLY6:
		case POINT_WEEKLY7:
		case POINT_WEEKLY_SEASON:
			{
				const int64_t nTotalPoint = static_cast<int64_t>(GetWeekly(type-169)) + static_cast<int64_t>(amount);
				if (2000000000 < nTotalPoint)
					return;
				int value = GetWeekly(type-169) + amount;
				SetWeekly(type-169, value);
				val = GetWeekly(type-169);
			}
			break;
#endif
		case POINT_GOLD:
			{
#ifdef ENABLE_LONG_LONG
				const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(amount);
#else
				const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(amount);
#endif
 				if (GOLD_MAX <= nTotalMoney)
 				{
#ifdef ENABLE_LONG_LONG
 					sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
#else
 					sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %d id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
#endif
 					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
 					return;
 				}

				if (g_bChinaIntoxicationCheck && amount > 0)
				{
					if (IsOverTime(OT_NONE))
					{
						sys_log(1, "<GOLD_LOG> %s = NONE", GetName());
					}
					else if (IsOverTime(OT_3HOUR))
					{
						amount = (amount / 2);
						sys_log(1, "<GOLD_LOG> %s = 3HOUR", GetName());
					}
					else if (IsOverTime(OT_5HOUR))
					{
						amount = 0;
						sys_log(1, "<GOLD_LOG> %s = 5HOUR", GetName());
					}
				}
				SetGold(GetGold() + amount);
				val = GetGold();
			}
			break;
#ifdef ENABLE_CHEQUE_SYSTEM
		case POINT_CHEQUE:
		{
			const int64_t nTotalCheque = static_cast<int64_t>(GetCheque()) + static_cast<int64_t>(amount);

			if (CHEQUE_MAX <= nTotalCheque)
			{
				sys_err("[OVERFLOW_CHEQUE] OriCheque %d AddedCheque %lld id %u Name %s ", GetCheque(), amount, GetPlayerID(), GetName());
				LogManager::instance().CharLog(this, GetCheque() + amount, "OVERFLOW_CHEQUE", "");
				return;
			}

			SetCheque(GetCheque() + amount);
			val = GetCheque();
		}
		break;
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		case POINT_PKT_OSIAG:
			{
				const int64_t nTotalPktOsiag = static_cast<int64_t>(GetPktOsiag()) + static_cast<long long>(amount);

				if (PO_MAX <= nTotalPktOsiag)
				{
					sys_err("[OVERFLOW_PKT_OSIAG] OriPktOsiag %d AddedPktOsiag %lld id %u Name %s ", GetPktOsiag(), amount, GetPlayerID(), GetName());
					LogManager::instance().CharLog(this, GetPktOsiag() + amount, "OVERFLOW_PKT_OSIAG", "");
					return;
				}

				SetPktOsiag(GetPktOsiag() + amount);
				val = GetPktOsiag();
			}
			break;
#endif

		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			SetRealPoint(type, val);
			break;

		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN:	// 42
		case POINT_ATTBONUS_ANIMAL:	// 43
		case POINT_ATTBONUS_ORC:	// 44
		case POINT_ATTBONUS_MILGYO:	// 45
		case POINT_ATTBONUS_UNDEAD:	// 46
		case POINT_ATTBONUS_DEVIL:	// 47

		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:
#endif

		case POINT_POISON_PCT:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:
#endif
		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP:		// 48
		case POINT_STEAL_SP:		// 49

		case POINT_MANA_BURN_PCT:	// 50
		case POINT_DAMAGE_SP_RECOVER:	// 51
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL:
		case POINT_RESIST_FAN:
		case POINT_RESIST_BOW:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:
#endif
		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE:	// 67
		case POINT_REFLECT_CURSE:	// 68
		case POINT_POISON_REDUCE:	// 69
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_REDUCE:
#endif
		case POINT_KILL_SP_RECOVER:	// 70
		case POINT_KILL_HP_RECOVERY:	// 75
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case POINT_ACCEDRAIN_RATE:
#endif
#ifdef ENABLE_GLOVE_SYSTEM
		case POINT_RANDOM:
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:
#endif
		case POINT_ENCHANT_ELECT:
		case POINT_ENCHANT_FIRE:
		case POINT_ENCHANT_ICE:
		case POINT_ENCHANT_WIND:
		case POINT_ENCHANT_EARTH:
		case POINT_ENCHANT_DARK:
#ifdef __PREMIUM_PRIVATE_SHOP__
		case POINT_PRIVATE_SHOP_UNLOCKED_SLOT:
#endif
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR :
		case POINT_RESIST_ASSASSIN :
		case POINT_RESIST_SURA :
		case POINT_RESIST_SHAMAN :
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN :
#endif
		case POINT_RESIST_HUMAN:
		case POINT_ATTBONUS_BOSS:
		case POINT_ATTBONUS_WLADCA:
		case POINT_ATTBONUS_STONE:
		case POINT_ATTBONUS_DUNGEON:
		case POINT_RESIST_BOSS:
		case POINT_RESIST_WLADCA:
		case POINT_ATTBONUS_LEGENDA:
		case POINT_RESIST_MONSTER:
		case POINT_DMG_BONUS:
		case POINT_FINAL_DMG_BONUS:

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
			if (GetPoint(type) + amount > 250)
			{
				sys_err("MALL_BONUS exceeded over 250!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 250 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_RAMADAN_CANDY_BONUS_EXP:
			SetPoint(type, amount);
			val = GetPoint(type);
			break;

		case POINT_EXP_DOUBLE_BONUS:	// 71
		case POINT_GOLD_DOUBLE_BONUS:	// 72
		case POINT_ITEM_DROP_BONUS:	// 73
		case POINT_POTION_BONUS:	// 74
			if (GetPoint(type) + amount > 250)
			{
				sys_err("BONUS exceeded over 250!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 250 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:		// 76
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN SET_BIT type(%u) amount(%d)", type, amount);
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			else
			{
				// ChatPacket(CHAT_TYPE_INFO, "IMMUNE_STUN REMOVE_BIT type(%u) amount(%d)", type, amount);
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			break;

		case POINT_IMMUNE_SLOW:		// 77
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			break;

		case POINT_IMMUNE_FALL:	// 78
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			MountVnum(val);
			break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
			{
				int old_val = GetPoint(type);
				SetPoint(type, old_val + amount);
				val = GetPoint(type);
				BuffOnAttr_ValueChange(type, old_val, val);
			}
			break;
		case POINT_STAT_BONUS:
			{
				PointChange(POINT_ST, amount);
				PointChange(POINT_DX, amount);
				PointChange(POINT_IQ, amount);
				PointChange(POINT_HT, amount);
				val = GetPoint(type);
			}
			break;

		case POINT_ATTBONUS_KLASY:
			{
				PointChange(POINT_ATTBONUS_WARRIOR, amount);
				PointChange(POINT_ATTBONUS_SURA, amount);
				PointChange(POINT_ATTBONUS_SHAMAN, amount);
				PointChange(POINT_ATTBONUS_ASSASSIN, amount);
				val = GetPoint(type);
			}
			break;

		case POINT_RESIST_KLASY:
			{
				PointChange(POINT_RESIST_WARRIOR, amount);
				PointChange(POINT_RESIST_SURA, amount);
				PointChange(POINT_RESIST_SHAMAN, amount);
				PointChange(POINT_RESIST_ASSASSIN, amount);
				val = GetPoint(type);
			}
			break;

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	if (GetDesc())
	{
		struct packet_point_change pack;

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	switch (bApplyType)
	{
		case APPLY_NONE:			// 0
#if defined(__ITEM_APPLY_RANDOM__)
		case APPLY_RANDOM:
#endif
			break;

		case APPLY_CON:
			PointChange(POINT_HT, iVal);
			PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
			break;

		case APPLY_INT:
			PointChange(POINT_IQ, iVal);
			PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
			break;

		case APPLY_SKILL:
			// SKILL_DAMAGE_BONUS
			{
				// 00000000 00000000 00000000 00000000

				// vnum     ^ add       change
				BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
				int iAdd = iVal & 0x00800000;
				int iChange = iVal & 0x007fffff;

				sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

				if (0 == iAdd)
					iChange = -iChange;

				boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

				if (iter == m_SkillDamageBonus.end())
					m_SkillDamageBonus.emplace(bSkillVnum, iChange);
				else
					iter->second += iChange;
			}
			// END_OF_SKILL_DAMAGE_BONUS
			break;

		case APPLY_MAX_HP:
		case APPLY_MAX_HP_PCT:
			{
				int i = GetMaxHP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxHP() / (float)i;
				PointChange(POINT_HP, GetHP() * fRatio - GetHP());
			}
			break;

		case APPLY_MAX_SP:
		case APPLY_MAX_SP_PCT:
			{
				int i = GetMaxSP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxSP() / (float)i;
				PointChange(POINT_SP, GetSP() * fRatio - GetSP());
			}
			break;

		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_ATT_SPEED:
		case APPLY_MOV_SPEED:
		case APPLY_CAST_SPEED:
		case APPLY_HP_REGEN:
		case APPLY_SP_REGEN:
		case APPLY_POISON_PCT:
		case APPLY_STUN_PCT:
		case APPLY_SLOW_PCT:
		case APPLY_CRITICAL_PCT:
		case APPLY_PENETRATE_PCT:
		case APPLY_ATTBONUS_HUMAN:
		case APPLY_ATTBONUS_ANIMAL:
		case APPLY_ATTBONUS_ORC:
		case APPLY_ATTBONUS_MILGYO:
		case APPLY_ATTBONUS_UNDEAD:
		case APPLY_ATTBONUS_DEVIL:
		case APPLY_ATTBONUS_WARRIOR:	// 59
		case APPLY_ATTBONUS_ASSASSIN:	// 60
		case APPLY_ATTBONUS_SURA:	// 61
		case APPLY_ATTBONUS_SHAMAN:	// 62
		case APPLY_ATTBONUS_MONSTER:	// 63
		case APPLY_STEAL_HP:
		case APPLY_STEAL_SP:
		case APPLY_MANA_BURN_PCT:
		case APPLY_DAMAGE_SP_RECOVER:
		case APPLY_BLOCK:
		case APPLY_DODGE:
		case APPLY_RESIST_SWORD:
		case APPLY_RESIST_TWOHAND:
		case APPLY_RESIST_DAGGER:
		case APPLY_RESIST_BELL:
		case APPLY_RESIST_FAN:
		case APPLY_RESIST_BOW:
		case APPLY_RESIST_FIRE:
		case APPLY_RESIST_ELEC:
		case APPLY_RESIST_MAGIC:
		case APPLY_RESIST_WIND:
		case APPLY_RESIST_ICE:
		case APPLY_RESIST_EARTH:
		case APPLY_RESIST_DARK:
		case APPLY_REFLECT_MELEE:
		case APPLY_REFLECT_CURSE:
		case APPLY_ANTI_CRITICAL_PCT:
		case APPLY_ANTI_PENETRATE_PCT:
		case APPLY_POISON_REDUCE:
		case APPLY_KILL_SP_RECOVER:
		case APPLY_EXP_DOUBLE_BONUS:
		case APPLY_GOLD_DOUBLE_BONUS:
		case APPLY_ITEM_DROP_BONUS:
		case APPLY_POTION_BONUS:
		case APPLY_KILL_HP_RECOVER:
		case APPLY_IMMUNE_STUN:
		case APPLY_IMMUNE_SLOW:
		case APPLY_IMMUNE_FALL:
		case APPLY_BOW_DISTANCE:
		case APPLY_ATT_GRADE_BONUS:
		case APPLY_DEF_GRADE_BONUS:
		case APPLY_MAGIC_ATT_GRADE:
		case APPLY_MAGIC_DEF_GRADE:
		case APPLY_CURSE_PCT:
		case APPLY_MAX_STAMINA:
		case APPLY_MALL_ATTBONUS:
		case APPLY_MALL_DEFBONUS:
		case APPLY_MALL_EXPBONUS:
		case APPLY_MALL_ITEMBONUS:
		case APPLY_MALL_GOLDBONUS:
		case APPLY_SKILL_DAMAGE_BONUS:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:

			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:
		case APPLY_NORMAL_HIT_DEFEND_BONUS:
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_RESIST_WARRIOR :
		case APPLY_RESIST_ASSASSIN :
		case APPLY_RESIST_SURA :
		case APPLY_RESIST_SHAMAN :
		case APPLY_ENERGY:					// 82
		case APPLY_DEF_GRADE:				// 83
		case APPLY_COSTUME_ATTR_BONUS:		// 84
		case APPLY_MAGIC_ATTBONUS_PER:		// 85
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:			// 86
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case APPLY_ACCEDRAIN_RATE:			//97
#endif
		case APPLY_ENCHANT_ELECT:
		case APPLY_ENCHANT_FIRE:
		case APPLY_ENCHANT_ICE:
		case APPLY_ENCHANT_WIND:
		case APPLY_ENCHANT_EARTH:
		case APPLY_ENCHANT_DARK:
		case APPLY_RESIST_HUMAN:
		case APPLY_ATTBONUS_BOSS:
		case APPLY_ATTBONUS_WLADCA:
		case APPLY_ATTBONUS_STONE:
		case APPLY_ATTBONUS_DUNGEON:
		case APPLY_RESIST_BOSS:
		case APPLY_RESIST_WLADCA:
		case APPLY_RESIST_MONSTER:
		case APPLY_STAT_BONUS:
		case APPLY_ATTBONUS_KLASY:
		case APPLY_RESIST_KLASY:
		case APPLY_DMG_BONUS:
		case APPLY_FINAL_DMG_BONUS:
		case APPLY_ATTBONUS_LEGENDA:
			PointChange(aApplyInfo[bApplyType].bPointType, iVal);
			break;

		default:
			sys_err("Unknown apply type %d name %s", bApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = (DWORD)GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char * format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size      = sizeof(struct packet_chat) + len;
	pack_chat.type      = type;
	pack_chat.id        = 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ä���� �ߴ��Ͽ����ϴ�."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	// @fixme128
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��̸� �����ϼ���."));
		return;
	}

	int count = number(5, 10);
	
// #ifdef TITLE_SYSTEM_BYLUZER
// 	if (m_pTitle[15].active == 2)
// 		count = round(count*112/100);
// 	else if (m_pTitle[16].active == 2)
// 		count = round(count*108/100);
// 	else if (m_pTitle[17].active == 2)
// 		count = round(count*105/100);
// #endif

	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ø� �� �� �ִ� ���� �ƴմϴ�"));
			return;
		}
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ô븦 ���� �ϼ���."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�̳��� ���� ���� �ּ���."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

#ifdef DISABLE_ANTI_FISHBOT
			if (info)
				fishing::Take(info, this);
#else
			if (info)
			{
				int iTries = GetQuestFlag("fishing.antibot") - 1;
				SetQuestFlag("fishing.antibot", iTries);

				if (iTries != 0 && test_server)
					ChatPacket(CHAT_TYPE_INFO, "(fishing) iTries = %d", iTries);
				else if (iTries == 0)
					m_pkFishingEvent = fishing::CreateFishingPullEvent(m_pkFishingEvent, this);

				if (info->step == 0 && iTries < 0)
				{
					fishing::Take(info, this);
					event_cancel(&m_pkFishingEvent);
				}

				return;
			}
#endif
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���ô밡 �ƴ� �������� ���ø� �� �� �����ϴ�!"));
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("�������·ξ����");
}

void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}


#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void CHARACTER::SendGold()
{
	TPacketGold pack;
	memset(&pack, 0, sizeof(TPacketGold));
	pack.header=HEADER_GC_GOLD;
	pack.vid=GetVID();
	pack.gold=GetGold();
	GetDesc()->Packet(&pack, sizeof(pack));
}

bool IsWrongChar(char c) {
	return !isalnum(c) && c != ' ' && c != '+';
}

bool IsSpaceChar(char c) {
	return c == ' ';
}

void CHARACTER::SetOfflineShop(LPOFFLINESHOP pkOfflineShop)
{
	if ((m_pkOfflineShop = pkOfflineShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_OFFLINE_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_OFFLINE_SHOP);
		SetOfflineShopOwner(NULL);
	}
}

void CHARACTER::OpenMyOfflineShop(const char * c_pszSign, TShopItemTable * pTable, int bItemCount)
{
	quest::PC * pPC = quest::CQuestManager::Instance().GetPC(GetPlayerID());

	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

	if (GetShop() || GetOfflineShop())
		return;
	
    if (IsPC())
    {
        if (((unsigned)GetMapIndex() < 0) || std::size_t ((unsigned)GetMapIndex() >= (sizeof(g_shopCount) / sizeof(g_shopCount[0]))))
        {
            sys_err("Unknown mapindex(%u) (or dungeon) for %u, name %s", GetMapIndex(), GetPlayerID(), GetName());
            ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU_CANT_OPEN_SHOP_AT_THIS_LOCATION"));
            return;
        }
        else
        {
            if ((g_shopLimit[GetMapIndex()] > 0) && (g_shopCount[GetMapIndex()] >= g_shopLimit[GetMapIndex()]))
            {
                ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LIMIT_OFFLINE_SHOP_ON_THIS_MAP"), g_shopLimit[GetMapIndex()]);
                return;
            }
        }
    }
	
	if (!LC_IsBrazil())
	{
		int64_t nTotalMoney = 0;
		for (int n = 0; n < bItemCount; ++n)
			nTotalMoney += (pTable + n)->price;

	}

	bool bPriceNegative = false;
	for (int n = 0; n < bItemCount; ++n)
	{
		if ((pTable + n)->price < 0)
		{
			bPriceNegative = true;
		}
	}

	for (int n = 0; n < bItemCount; ++n)
	{
		if ((pTable + n)->cheque < 0)
		{
			bPriceNegative = true;
		}
	}

	if (bPriceNegative == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NEGATIVE_PRICE"));
		return;
	}
	
	// Check Game admin can not open offline shop
	{
		if (!test_server)
		{
			if (GetGMLevel() > GM_LOW_WIZARD && GetGMLevel() < GM_IMPLEMENTOR)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ADMINISTRATOR"));
				return;
			}
		}
	}
	// End Of Check Game admin can not open offline shop
	
	// Check player is open offline shop or not
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		
		int bResult = 0;
		str_to_number(bResult, row[0]);
		
		if (bResult)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ALREDY_OPEN"));
			return;
		}
	}
	// End Of check player is open offline shop or not
	
	// Check if player has current items in database.
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM %soffline_shop_item WHERE owner_id = %u", get_table_postfix(), GetPlayerID()));
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		
		int bResult = 0;
		str_to_number(bResult, row[0]);
		
		if (bResult)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_BACK_ITEM"));
			COfflineShopManager::instance().Giveback2(this);
			return;
		}
	}
	// End of check if player has current items in database

	// Time Mechanism
	int iTime = 168 * 60 * 60;
	long long shopCost = g_dwNeedMoney_01;
	int shopCostStatus = g_bNeedMoney_01;
	bool g_bNeedItem = g_bNeedItem_01;
	int g_iItemVnum = g_iItemVnum_01;
	BYTE g_bItemCount = g_bItemCount_01;

	// Check map allows for Offline Shop
	if (g_bOfflineShopMapAllowLimit)
	{
		if (!offlineshop_map_allow_find(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("YOU_CANT_OPEN_SHOP_AT_THIS_LOCATION"));
			return;
		}
	}
	// End Of Check map allows for offline shop

	if (g_bNeedItem && shopCostStatus)
	{
		BYTE bCount = quest::CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem(g_iItemVnum);
		if (bCount < g_bItemCount)
		{
			LPITEM item = ITEM_MANAGER::instance().CreateItem(g_iItemVnum);
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aby otworzyc sklep na taki okres czasu potrzebujesz %s (x%d)!"), item->GetName(), g_bItemCount);
			return;
		} 
		else if (GetGold() < (long long) shopCost)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aby otworzyc sklep na taki okres czasu potrzebujesz %lld yang."), shopCost);
			return;
		}
		else {
			LPITEM item = FindSpecifyItem(g_iItemVnum);
			if (!item)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ERROR"));
				return;
			}
			RemoveSpecifyItem(g_iItemVnum, g_bItemCount);
			PointChange(POINT_GOLD, -static_cast<long long>(shopCost), false);
		}
	}
	else
	{
		if (g_bNeedItem)
		{
			BYTE bCount = quest::CQuestManager::instance().GetCurrentCharacterPtr()->CountSpecifyItem(g_iItemVnum);
			if (bCount < g_bItemCount)
			{
				LPITEM item = ITEM_MANAGER::instance().CreateItem(g_iItemVnum);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aby otworzyc sklep na taki okres czasu potrzebujesz %s (x%d)!"), item->GetName(), g_bItemCount);
				return;
			} else {
				LPITEM item = FindSpecifyItem(g_iItemVnum);
				if (!item)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ERROR"));
					return;
				}
				RemoveSpecifyItem(g_iItemVnum, g_bItemCount);	 
			}
		}

		if (shopCostStatus)
		{
			if (GetGold() < (long long) shopCost)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aby otworzyc sklep na taki okres czasu potrzebujesz %lld yang."), shopCost);
				return;
			} else {
				PointChange(POINT_GOLD, -static_cast<long long>(shopCost), false);
			}
		}
	}

	std::string shopSign = c_pszSign;
	std::string shopSignNoSpace = c_pszSign;

	shopSign.erase(std::remove_if(shopSign.begin(), shopSign.end(), IsWrongChar), shopSign.end());
	
	shopSignNoSpace.erase(std::remove_if(shopSignNoSpace.begin(), shopSignNoSpace.end(), IsSpaceChar), shopSignNoSpace.end());

	if (shopSign.length() < 2 || shopSignNoSpace.length() < 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_SHORT_NAME"));
		return;
	}

	if (CBanwordManager::instance().CheckString(shopSign.c_str(), shopSign.length() + 1)) // Check for banned words
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_BAD_NAME"));
		return;
	}
	
	m_stOfflineShopSign = shopSign;
	m_offline_norm = 30000;
	LPCHARACTER npc = CHARACTER_MANAGER::instance().SpawnMob(m_offline_norm, GetMapIndex(), GetX(), GetY(), GetZ(), false, -1, false, true, GetPlayerID());
	
	if (!npc)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ERROR"));
		return;
	}
	
	std::map<DWORD, DWORD> itemkind;
	std::set<TItemPos> cont;

	for (int i = 0; i < bItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MY_OFFLINE_SHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ANTIFLAG"));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				return;
			}

			if (true == pkItem->isLocked())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_LOCKED_ITEM"));
				return;
			}

			itemkind[pkItem->GetVnum()] = (pTable + i)->price + (pTable + i)->cheque / pkItem->GetCount();

			char szColumns[QUERY_MAX_LEN], szValues[QUERY_MAX_LEN];

			int iLen = snprintf(szColumns, sizeof(szColumns), "id,owner_id,pos,count,price_gold,price_cheque,vnum,type,subtype"
			);
			
			int iUpdateLen = snprintf(szValues, sizeof(szValues),
																"%u, "
																"%u, "
																"%d, "
																"%u, "
																"%lld, "
																"%lld, "
																"%u, "
																"%d, "
																"%d"
																, pkItem->GetID(), GetPlayerID(), (pTable + i)->display_pos, pkItem->GetCount(), (pTable + i)->price, (pTable + i)->cheque, pkItem->GetVnum(), pkItem->GetType(), pkItem->GetSubType()
			);
			
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",socket0,socket1,socket2");
			iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%ld,%ld,%ld", pkItem->GetSocket(0), pkItem->GetSocket(1), pkItem->GetSocket(2));
			
#ifdef USE_LENTS_SHOULDER_SASH
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3, attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6, applytype0, applyvalue0, applytype1, applyvalue1, applytype2, applyvalue2, applytype3, applyvalue3, applytype4, applyvalue4, applytype5, applyvalue5, applytype6, applyvalue6, applytype7, applyvalue7");
			iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
				pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0),
				pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1),
				pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2),
				pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3),
				pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4),
				pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5),
				pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6),
				pkItem->GetAttributeType(7), pkItem->GetAttributeValue(7),
				pkItem->GetAttributeType(8), pkItem->GetAttributeValue(8),
				pkItem->GetAttributeType(9), pkItem->GetAttributeValue(9),
				pkItem->GetAttributeType(10), pkItem->GetAttributeValue(10),
				pkItem->GetAttributeType(11), pkItem->GetAttributeValue(11),
				pkItem->GetAttributeType(12), pkItem->GetAttributeValue(12),
				pkItem->GetAttributeType(13), pkItem->GetAttributeValue(13),
				pkItem->GetAttributeType(14), pkItem->GetAttributeValue(14));
#else
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ",attrtype0,attrvalue0,attrtype1,attrvalue1,attrtype2,attrvalue2,attrtype3,attrvalue3,attrtype4,attrvalue4,attrtype5,attrvalue5,attrtype6,attrvalue6");
			iUpdateLen += snprintf(szValues + iUpdateLen, sizeof(szValues) - iUpdateLen, ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				pkItem->GetAttributeType(0), pkItem->GetAttributeValue(0),
				pkItem->GetAttributeType(1), pkItem->GetAttributeValue(1),
				pkItem->GetAttributeType(2), pkItem->GetAttributeValue(2),
				pkItem->GetAttributeType(3), pkItem->GetAttributeValue(3),
				pkItem->GetAttributeType(4), pkItem->GetAttributeValue(4),
				pkItem->GetAttributeType(5), pkItem->GetAttributeValue(5),
				pkItem->GetAttributeType(6), pkItem->GetAttributeValue(6));
#endif
			
			char szInsertQuery[QUERY_MAX_LEN];
			snprintf(szInsertQuery, sizeof(szInsertQuery), "INSERT INTO %soffline_shop_item (%s) VALUES (%s)", get_table_postfix(), szColumns, szValues);
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szInsertQuery));
			ITEM_MANAGER::instance().RemoveItem(pkItem);
		}

		cont.insert((pTable + i)->pos);
	}

	npc->SetOfflineShopTimer(iTime);
	npc->StartOfflineShopUpdateEvent();

	// Insert
	char szSign[SHOP_SIGN_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szSign, sizeof(szSign), shopSign.c_str(), shopSign.size());
	
	char szQuery2[512];
	snprintf(szQuery2,sizeof(szQuery2),"INSERT INTO %soffline_shop_npc(owner_id, sign, name, time, x, y, z, mapIndex, channel, npc_decoration) VALUES(%u, '%s', '%s', %d, %ld, %ld, %ld, %ld, %d, %d)", get_table_postfix(),GetPlayerID(), szSign,GetName(), iTime,GetX(),GetY(),GetZ(),GetMapIndex(), g_bChannel, m_offline_norm);
	DBManager::Instance().DirectQuery(szQuery2);

	// Create Offline Shop
	LPOFFLINESHOP pkOfflineShop = COfflineShopManager::instance().CreateOfflineShop(npc, GetPlayerID());
	
	if (!pkOfflineShop)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_ERROR"));
		return;
	}
	
	npc->SetOfflineShop(pkOfflineShop);
	npc->SetOfflineShopChannel(g_bChannel);
	// End Of Create Offline Shop
	
	// Set Name
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	snprintf(szName, sizeof(szName), "%s", GetName());
	npc->SetName(szName);
	// End Of Set Name
	
	SetOfflineShopVID(npc->GetVID());
	npc->Show(GetMapIndex(), GetX(), GetY(), GetZ(), true);
	
	// Send Offline Shop Sign to Everyone
	TPacketGCShopSign p;
	p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
	p.dwVID = npc->GetVID();
	strlcpy(p.szSign, shopSign.c_str(), sizeof(p.szSign));
	
	PacketAround(&p, sizeof(TPacketGCShopSign));
	// End Of Send Offline Shop Sign to Everyone

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LIMITED_TIME_SHOP"));
	++g_shopCount[GetMapIndex()];
}

EVENTFUNC(offline_shop_update_event)
{
	char_event_info* pInfo = dynamic_cast<char_event_info*>(event->info);

	if (pInfo == NULL)
	{
		sys_err("offline_shop_update_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER npc = pInfo->ch;

	if (npc == NULL)
		return 0;
	
	int remain_time = npc->GetOfflineShopTimer() - processing_time / passes_per_sec;
	if (remain_time <= 0)
	{
		--g_shopCount[npc->GetMapIndex()];
		sys_log(0, "OFFLINE SHOP EXPIRED : expired %s", npc->GetName());
		LPCHARACTER pChar = CHARACTER_MANAGER::instance().Find(npc->GetOfflineShopRealOwner());			
		if (pChar)
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SHOP_OFFLINE_END_TIME"));			
			COfflineShopManager::instance().Giveback(pChar);
		}	
		COfflineShopManager::instance().DestroyOfflineShop(NULL, npc->GetVID());
		DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), npc->GetOfflineShopRealOwner());
		M2_DESTROY_CHARACTER(npc);
		return 0;
	}

	// If the variable is reach to maximum, start to save it
	if (npc->GetOfflineShopSaveTime() >= g_bOfflineShopSaveTime)
	{
		DBManager::instance().DirectQuery("UPDATE %soffline_shop_npc SET time = %d WHERE owner_id = %u", get_table_postfix(), npc->GetOfflineShopTimer(), npc->GetOfflineShopRealOwner());
		npc->SetOfflineShopTimer(1);
	}

	npc->SetOfflineShopTimer(remain_time);
	npc->SetOfflineShopSaveTime(npc->GetOfflineShopSaveTime() + 1);
	return PASSES_PER_SEC(MIN(60, remain_time));
}

void CHARACTER::StartOfflineShopUpdateEvent()
{
	if (m_pkOfflineShopUpdateEvent)
		return;

	if (IsPC() || IsMonster())
		return;

	char_event_info * pInfo = AllocEventInfo<char_event_info>();
	pInfo->ch = this;
	m_pkOfflineShopUpdateEvent = event_create(offline_shop_update_event, pInfo, PASSES_PER_SEC(60));
}


void CHARACTER::StopOfflineShopUpdateEvent()
{
	m_pointsInstant.bSaveTime = 0;
	m_pointsInstant.leftTime = 0;
	event_cancel(&m_pkOfflineShopUpdateEvent);
}

void CHARACTER::SetOfflineShopSign(const char * c_szSign)
{
	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_szSign, sizeof(szSign));
	m_stOfflineShopSign = szSign;
}

void CHARACTER::DestroyOfflineShop()
{
	DBManager::instance().DirectQuery("DELETE FROM %soffline_shop_npc WHERE owner_id = %u", get_table_postfix(), GetOfflineShopRealOwner());
	LPOFFLINESHOP pkOfflineShop = GetOfflineShop();
	if (pkOfflineShop)
		pkOfflineShop->RemoveAllGuest();
	M2_DESTROY_CHARACTER(this);	
}
#endif

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, DWORD wVal) // @fixme502
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

DWORD CHARACTER::GetPart(BYTE bPartPos) const // @fixme502
{
	assert(bPartPos < PART_MAX_NUM);
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
			return pArmor->GetVnum();
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
		return 0;
#ifdef ENABLE_STOLE_COSTUME
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_STOLE) && IsStoleCostumeHidden() == true)
	{
		LPITEM pAcce = GetWear(WEAR_COSTUME_ACCE);
		if (pAcce && IsAcceCostumeHidden() == false)
		{
			DWORD toSetValue = pAcce->GetVnum();

			toSetValue -= 85000;
			if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
				toSetValue += 1000;
			
			return toSetValue;
		}
		else
		{
			return 0;
		}
	}
#endif
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true)
		{
		LPITEM pAcce = GetWear(WEAR_COSTUME_STOLE);
		if (pAcce && IsStoleCostumeHidden() == false)
		{
			DWORD toSetValue = pAcce->GetVnum();
			toSetValue -= 85000;
			if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
				toSetValue += 1000;
			
			return toSetValue;
		}
		else
		{
			return 0;
		}
	}
	else if (bPartPos == PART_AURA && GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden() == true)
		return 0;
	else if (bPartPos == PART_WEAPON && GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
			return pWeapon->GetVnum();
		else
			return 0;
	}
#endif
	return m_pointsInstant.parts[bPartPos];
}

DWORD CHARACTER::GetOriginalPart(BYTE bPartPos) const // @fixme502
{
	switch (bPartPos)
	{
		case PART_MAIN:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
				if (const LPITEM pArmor = GetWear(WEAR_BODY))
					return pArmor->GetVnum();
#endif
			if (!IsPC())
				return GetPart(PART_MAIN);
			else
				return m_pointsInstant.bBasePart;

		case PART_HAIR:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
				return 0;
#endif
			return GetPart(PART_HAIR);

#ifdef ENABLE_COSTUME_EMBLEMAT
		case PART_EMBLEMAT:
			return GetPart(PART_EMBLEMAT);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case PART_ACCE:
			{
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
#ifdef ENABLE_STOLE_COSTUME
				if (GetWear(WEAR_COSTUME_STOLE) && IsStoleCostumeHidden() == true)
				{
					LPITEM pAcce = GetWear(WEAR_COSTUME_ACCE);
					if (pAcce && IsAcceCostumeHidden() == false)
					{
						DWORD toSetValue = pAcce->GetVnum();
						toSetValue -= 85000;
						if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
							toSetValue += 1000;
						
						return toSetValue;
					}
					else
						return 0;
				}
#endif
			if (GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true)
				{
					LPITEM pAcce = GetWear(WEAR_COSTUME_STOLE);
					if (pAcce && IsStoleCostumeHidden() == false)
					{
						DWORD toSetValue = pAcce->GetVnum();
						toSetValue -= 85000;
						if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
							toSetValue += 1000;
						
						return toSetValue;
					}
					else
						return 0;
				}
#else
				if (GetWear(WEAR_COSTUME_ACCE))
					return 0;
#endif
				return GetPart(PART_ACCE);
			}
#endif
#ifdef ENABLE_AURA_SYSTEM
		case PART_AURA:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden() == true)
				return 0;
#endif
			return GetPart(PART_AURA);
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		case PART_WEAPON:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
				if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
					return pWeapon->GetVnum();
#endif
			return GetPart(PART_WEAPON);
#endif

		default:
			return 0;
	}
}

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
void CHARACTER::SetBodyCostumeHidden(bool hidden)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
}

void CHARACTER::SetHairCostumeHidden(bool hidden)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CHARACTER::SetAcceCostumeHidden(bool hidden)
{
	m_bHideAcceCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAcceCostumeHidden %d", m_bHideAcceCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_acce", m_bHideAcceCostume ? 1 : 0);
}
#endif

#ifdef ENABLE_STOLE_COSTUME
void CHARACTER::SetStoleCostumeHidden(bool hidden)
{
	m_bHideStoleCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetStoleCostumeHidden %d", m_bHideStoleCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_stole", m_bHideStoleCostume ? 1 : 0);
}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
void CHARACTER::SetWeaponCostumeHidden(bool hidden)
{
	m_bHideWeaponCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetWeaponCostumeHidden %d", m_bHideWeaponCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_weapon", m_bHideWeaponCostume ? 1 : 0);
}
#endif

#ifdef ENABLE_AURA_SYSTEM
void CHARACTER::SetAuraCostumeHidden(bool hidden)
{
	m_bHideAuraCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAuraCostumeHidden %d", m_bHideAuraCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_aura", m_bHideAuraCostume ? 1 : 0);
}
#endif
#endif

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER

	if (ch) // @fixme131
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.emplace_back(this);

			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	if (m_pkDungeon && IsPC() && !pkParty)
        SetDungeon(NULL);
	
	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;
	DWORD	dwLeaderPID;

	TPartyJoinEventInfo()
	: dwGuestPID( 0 )
	, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("��Ƽ���� ���� ���°� �ƴ϶� ��û�� �� �� �����ϴ�."));
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
			return false;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �ٸ� ������ ��Ƽ�� �̷� �� �����ϴ�."));
			return false;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ �ʴ븦 �� �� �����ϴ�."));
			return false;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ��忡�� ��Ƽ �ʴ븦 �� �� �����ϴ�."));
			return false;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> -30 ~ +30 ���� �̳��� ���游 �ʴ��� �� �ֽ��ϴ�."));
			return false;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ְ� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return false;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� ���� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return false;

		case PERR_ALREADYJOIN:
			return false;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �� �̻� ��Ƽ���� �ʴ��� �� �����ϴ�."));
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s �Կ��� ��Ƽ���� ��û�� �߽��ϴ�."), leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ��Ƽ�� �������� �ʽ��ϴ�."));
	else
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
			case PERR_NONE: 		member->PartyJoin(this); return;
			case PERR_SERVER:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�.")); break;
			case PERR_DUNGEON:		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ �ʴ븦 �� �� �����ϴ�.")); break;
			case PERR_OBSERVER: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ��忡�� ��Ƽ �ʴ븦 �� �� �����ϴ�.")); break;
			case PERR_LVBOUNDARY:	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> -30 ~ +30 ���� �̳��� ���游 �ʴ��� �� �ֽ��ϴ�.")); break;
			case PERR_LOWLEVEL: 	member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ְ� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�.")); break;
			case PERR_HILEVEL: 		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� ���� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�.")); break;
			case PERR_ALREADYJOIN: 	break;
			case PERR_PARTYISFULL: {
									   ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �� �̻� ��Ƽ���� �ʴ��� �� �����ϴ�."));
									   member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ο������� �ʰ��Ͽ� ��Ƽ�� ������ �� �����ϴ�."));
									   break;
								   }
			default: sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ���� �ʴ��� �� �ִ� ������ �����ϴ�."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> %s ���� ��Ƽ �ź� �����Դϴ�."), pchInvitee->GetName());
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
			return;

		case PERR_DIFFEMPIRE:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �ٸ� ������ ��Ƽ�� �̷� �� �����ϴ�."));
			return;

		case PERR_DUNGEON:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ �ʴ븦 �� �� �����ϴ�."));
			return;

		case PERR_OBSERVER:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ��忡�� ��Ƽ �ʴ븦 �� �� �����ϴ�."));
			return;

		case PERR_LVBOUNDARY:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> -30 ~ +30 ���� �̳��� ���游 �ʴ��� �� �ֽ��ϴ�."));
			return;

		case PERR_LOWLEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ְ� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return;

		case PERR_HILEVEL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� ���� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return;

		case PERR_ALREADYJOIN:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �̹� %s���� ��Ƽ�� ���� �ֽ��ϴ�."), pchInvitee->GetName());
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �� �̻� ��Ƽ���� �ʴ��� �� �����ϴ�."));
			return;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.emplace(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10)));

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ���� �ʴ��� �� �ִ� ������ �����ϴ�."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ������ ��Ƽ ���� ó���� �� �� �����ϴ�."));
			return;

		case PERR_DUNGEON:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� �ȿ����� ��Ƽ �ʴ뿡 ���� �� �����ϴ�."));
			return;

		case PERR_OBSERVER:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ���� ��忡�� ��Ƽ �ʴ븦 �� �� �����ϴ�."));
			return;

		case PERR_LVBOUNDARY:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> -30 ~ +30 ���� �̳��� ���游 �ʴ��� �� �ֽ��ϴ�."));
			return;

		case PERR_LOWLEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ְ� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return;

		case PERR_HILEVEL:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� ���� ���� ���� 30������ ���� �ʴ��� �� �����ϴ�."));
			return;

		case PERR_ALREADYJOIN:
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ �ʴ뿡 ���� �� �����ϴ�."));
			return;

		case PERR_PARTYISFULL:
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> �� �̻� ��Ƽ���� �ʴ��� �� �����ϴ�."));
			pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> ��Ƽ�� �ο������� �ʰ��Ͽ� ��Ƽ�� ������ �� �����ϴ�."));
			return;

		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> %s���� ��Ƽ �ʴ븦 �����ϼ̽��ϴ�."), pchInvitee->GetName());
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> %s���� ��Ƽ�� �����ϼ̽��ϴ�."), GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<��Ƽ> %s���� ��Ƽ�� �����ϼ̽��ϴ�."), pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int	level_limit = 30;
	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
   	{
	   	if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
		else if (pchLeader->GetParty()->IsPartyInAnyDungeon()) // @fixme301
			return PERR_DUNGEON;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon) {
		return;
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

#ifdef RENEWAL_DEAD_PACKET
#include "threeway_war.h"
DWORD CHARACTER::CalculateDeadTime(BYTE type)
{
	//generated from do_restart(cmd_general.cpp)
	if (!m_pkDeadEvent)
		return 0;
	int iTimeToDead = (event_time(m_pkDeadEvent) / passes_per_sec);
	if (type == REVIVE_TYPE_AUTO_TOWN)
		return iTimeToDead-7;
	if (!test_server && type == REVIVE_TYPE_HERE && (!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG)) {
		if (IsHack(false) && !CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			return iTimeToDead - (180 - g_nPortalLimitTime);
		if (iTimeToDead > 170)
			return iTimeToDead - 170;
	}
	if (IsHack(false) && ((!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) || !CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex())))
		return iTimeToDead - (180 - g_nPortalLimitTime);
	if (iTimeToDead > 173) 
		return iTimeToDead - 173;
	return 0;
}
#endif


void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL) {
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
#ifdef ENABLE_RESP_SYSTEM
	CRespManager::instance().RegisterMob(this);
#endif
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*if (IsAffectFlag(AFF_JEONSIN))
		  RemoveAffect(SKILL_JEONSINBANGEO);*/
	}

	/*if (IsAffectFlag(AFF_GUNGON))
	  RemoveAffect(SKILL_GUNGON);*/

	// MINING
	mining_cancel();
	// END_OF_MINING

	SECTREE * sectree = GetSectree();
	if (sectree && sectree->IsAttr(GetX(), GetY(), ATTR_BANPK) && IsMonster() && !InDungeon())
		M2_DESTROY_CHARACTER(this);
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

	{
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	{
		if (pkChrCauser->GetExchange())
		{
			sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) return;

				//PREVENT_TRADE_WINDOW
				if (pkChrCauser == this)
				{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen() || GetOfflineShopOwner())
#else
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen())	
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� �ŷ���(â��,��ȯ,����)���� ���λ����� ����� �� �����ϴ�."));
						return;
					}
				}
				else
				{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen() || pkChrCauser->GetOfflineShopOwner())
#else
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen())
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ٸ� �ŷ���(â��,��ȯ,����)���� ���λ����� ����� �� �����ϴ�."));
						return;
					}

					//if ((GetExchange() || IsOpenSafebox() || GetShopOwner()))
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen() || GetOfflineShopOwner()))
#else
					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen()))
#endif
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ٸ� �ŷ��� �ϰ� �ִ� ���Դϴ�."));
						return;
					}
				}
				//END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
				return;
			}

			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (IsOfflineShopNPC())
	{
		pkChrCauser->SetOfflineShopOwner(this);
		CTargetManager::instance().DeleteTarget(pkChrCauser->GetPlayerID(), 9999, GetName());
		GetOfflineShop()->AddGuest(pkChrCauser, this);
		return;
	}
#endif
	if (g_bChinaIntoxicationCheck)
	{
		if (pkChrCauser->IsOverTime(OT_3HOUR))
		{
			sys_log(0, "Play OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 3);
			return;
		}
		else if (pkChrCauser->IsOverTime(OT_5HOUR))
		{
			sys_log(0, "Play OverTime : name = %s, hour = %d)", pkChrCauser->GetName(), 5);
			return;
		}
	}

	pkChrCauser->SetQuestNPCID(GetVID());

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}

	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			//sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
			//			pkChrCauser->GetName(),
			//			GetName(),
			//			pkChrCauser->GetPlayerID());
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}
}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
	    m_pointsInstant.gm_level =  gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
	}
	else
	{
	    m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
		return true;
	if (test_server)
		return true;
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.emplace(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	// p.bHPPercent = 0;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
	
	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;
	
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
		return;
	
	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
	
	m_pkChrTarget = pkChrTarget;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);
		p.dwVID	= m_pkChrTarget->GetVID();
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			// p.bHPPercent = 0;
			p.iMinHP = 0;
			p.iMaxHP = 0;
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			// p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
		}
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 || m_pkChrTarget->GetRaceNum() == 20102 || m_pkChrTarget->GetRaceNum() == 20103 || m_pkChrTarget->GetRaceNum() == 20104 || m_pkChrTarget->GetRaceNum() == 20105 || m_pkChrTarget->GetRaceNum() == 20106 || m_pkChrTarget->GetRaceNum() == 20107 || m_pkChrTarget->GetRaceNum() == 20108 || m_pkChrTarget->GetRaceNum() == 20109)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();
				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();
					if (iHorseMaxHealth)
					{
						// p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);
						p.iMinHP = 100;
						p.iMaxHP = 100;
					}
					else
					{
						// p.bHPPercent = 100;
						p.iMinHP = 100;
						p.iMaxHP = 100;
					}
				}
				else
				{
					// p.bHPPercent = 100;
					p.iMinHP = 100;
					p.iMaxHP = 100;
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					// p.bHPPercent = 0;
					p.iMinHP = 0;
					p.iMaxHP = 0;
				}
				else
				{
					// p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		// p.bHPPercent = 0;
		p.iMinHP = 0;
		p.iMaxHP = 0;
	}
	
	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;
	
	TPacketGCTarget p;
	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();
	if (GetMaxHP() <= 0)
	{
		// p.bHPPercent = 0;
		p.iMinHP = 0;
		p.iMaxHP = 0;
	}
	else
	{
		// p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
		// p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
	}
	
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();
	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;
		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}
		
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

#if defined(__BL_MOVE_CHANNEL__)
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex, long lCustomAddr, WORD wCustomPort)
#else
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
#endif
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	//Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		//do not send SDB files if char is in the same map
		if( lCurMapIndex != lMapIndex )
		{
			const TMapRegion * rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap( GetDesc(), rMapRgn->strMapName.c_str() );
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
#if defined(__BL_MOVE_CHANNEL__)
	p.lAddr	= lCustomAddr ? lCustomAddr : lAddr;
	p.wPort	= wCustomPort ? wCustomPort : wPort;
#else
	p.lAddr = lAddr;
	p.wPort = wPort;
#endif
#ifdef ENABLE_NEWSTUFF
	if (!g_stProxyIP.empty())
		p.lAddr = inet_addr(g_stProxyIP.c_str());
#endif

#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

#define ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
#ifdef ENABLE_GOHOME_IF_MAP_NOT_ALLOWED
		GoHome();
#else
		GetDesc()->SetPhase(PHASE_CLOSE);
#endif
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	/*
	   float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	   float fx, fy;
	   GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	   x = GetX() + (int) fx;
	   y = GetY() + (int) fy;
	 */
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p �����ϰ� ���ư���! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC())
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
				{
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC())
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000)
			{
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}

	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet() && false == IsHorse())
	{
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX())*(x-GetX())+((double)y - GetY())*(y-GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �߸��� ��ȣ�� �Է��ϼ̽��ϴ�."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â�� �̹� �����ֽ��ϴ�."));
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime()  < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> â�� ������ 10�� �ȿ��� �� �� �����ϴ�."));
		return;
	}
	// else if (GetDistanceFromSafeboxOpen() > 1000)
	// {
	// 	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<â��> �Ÿ��� �־ â�� �� �� �����ϴ�."));
	// 	return;
	// }
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
			item->SetAttributes(pItems->aAttr);

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
			{
				item->OnAfterCreatedItem(); // @fixme306
				item->SetSkipSave(false);
			}
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
			item->SetAttributes(pItems->aAttr);

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	if (GetMaxHP() <= 0) // @fixme136
		out.percent_hp	= 0;
	else
		out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role		= GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

#if defined(__BL_PARTY_POSITION__)
	out.x = GetX();
	out.y = GetY();
#endif

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
				GetPlayerID(),
				NULL,
				"SELECT size FROM safebox%s WHERE account_id = %u",
				get_table_postfix(),
				GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("�ȴ´�");
			else
				MonsterLog("�ڴ�");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking?"walking.":"running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	// @fixme104
	PointChange(POINT_STAT, (MINMAX(1, iLv, g_iStatusPointGetLevelLimit) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		extern const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
		DWORD dwSkillVnum = 0;
		// 50% of getting random books or getting one of the same player's race
		if (!number(0, 1))
			dwSkillVnum = GetRandomSkillVnum(GetJob());
		else
			dwSkillVnum = GetRandomSkillVnum();
		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const std::string& sFileName) {
    TPacketGCSpecificEffect p = {};

    p.header = HEADER_GC_SPECIFIC_EFFECT;
    p.vid = GetVID();
    strlcpy(p.effect_file, sFileName.c_str(), MAX_EFFECT_FILE_NAME);

    PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
		return;

	char sbuf[256+1];

	if (IsMonster())
	{
		if (number(0, 60))
			return;

		snprintf(sbuf, sizeof(sbuf),
				"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
				GetRaceNum(), GetRaceNum(), bMonsterChatType*3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
			return;

		if (IsGuardNPC())
		{
			if (number(0, 6))
				return;
		}
		else
		{
			if (number(0, 30))
				return;
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);

	if (text.empty())
		return;

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size	= sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_dwQuestItemVID = (item) ? item->GetVID() : 0;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_dwQuestItemVID = 0;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	if (!m_dwQuestItemVID)
		return nullptr;
	return ITEM_MANAGER::Instance().FindByVID(m_dwQuestItemVID);
}

#ifdef ENABLE_QUEST_DND_EVENT
void CHARACTER::SetQuestDNDItemPtr(LPITEM item)
{
	m_dwQuestDNDItemVID = (item) ? item->GetVID() : 0;
}

void CHARACTER::ClearQuestDNDItemPtr()
{
	m_dwQuestDNDItemVID = 0;
}

LPITEM CHARACTER::GetQuestDNDItemPtr() const
{
	if (!m_dwQuestDNDItemVID)
		return nullptr;
	return ITEM_MANAGER::Instance().FindByVID(m_dwQuestDNDItemVID);
}
#endif

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
#ifdef ENABLE_WOLFMAN_CHARACTER
	if (dwRaceNum < MAIN_RACE_MAX_NUM)
#else
	if (dwRaceNum < JOB_MAX_NUM)
#endif
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	// 29.08.2019 - Fixing quest handle. In case of any loop/inappropriate use of function, script might be presented to random player.
	quest::CQuestManager& q = quest::CQuestManager::instance();

	int lFlagValue = 0;
	uint32_t dwPID = q.GetCurrentCharacterPtr() ? q.GetCurrentCharacterPtr()->GetPlayerID() : 0;

	quest::PC* pPC = q.GetPC(GetPlayerID());

	if (pPC)
		lFlagValue = pPC->GetFlag(flag);

	// Restoring proper pointer
	if (dwPID)
		q.GetPC(dwPID);

	return lFlagValue;
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();

	// 29.08.2019 - Fixing quest handle. In case of any loop/inappropriate use of function, script might be presented to random player.
	uint32_t dwPID = q.GetCurrentCharacterPtr() ? q.GetCurrentCharacterPtr()->GetPlayerID() : 0;

	quest::PC* pPC = q.GetPC(GetPlayerID());

	if (pPC)
		pPC->SetFlag(flag, value);

	// Restoring proper pointer
	if (dwPID)
		q.GetPC(dwPID);
}

// void CHARACTER::DetermineDropMetinStone()
// {
// #ifdef ENABLE_NEWSTUFF
// 	if (g_NoDropMetinStone)
// 	{
// 		m_dwDropMetinStone = 0;
// 		return;
// 	}
// #endif

// 	static const DWORD c_adwMetin[] =
// 	{
// #if defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_WOLFMAN_STONES)
// 		28012,
// #endif
// 		28030,
// 		28031,
// 		28032,
// 		28033,
// 		28034,
// 		28035,
// 		28036,
// 		28037,
// 		28038,
// 		28039,
// 		28040,
// 		28041,
// 		28042,
// 		28043,
// #if defined(ENABLE_MAGIC_REDUCTION_SYSTEM) && defined(USE_MAGIC_REDUCTION_STONES)
// 		28044,
// 		28045,
// #endif
// 	};
// 	DWORD stone_num = GetRaceNum();
// 	int idx = std::lower_bound(aStoneDrop, aStoneDrop+STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
// 	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
// 	{
// 		m_dwDropMetinStone = 0;
// 	}
// 	else
// 	{
// 		const SStoneDropInfo & info = aStoneDrop[idx];
// 		m_bDropMetinStonePct = info.iDropPct;
// 		{
// 			m_dwDropMetinStone = c_adwMetin[number(0, sizeof(c_adwMetin)/sizeof(DWORD) - 1)];
// 			int iGradePct = number(1, 100);
// 			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel ++)
// 			{
// 				int iLevelGradePortion = info.iLevelPct[iStoneLevel];
// 				if (iGradePct <= iLevelGradePortion)
// 				{
// 					break;
// 				}
// 				else
// 				{
// 					iGradePct -= iLevelGradePortion;
// 					m_dwDropMetinStone += 100;
// 				}
// 			}
// 		}
// 	}
// }

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid    = GetVID();
	for (int i = 0; i<WEAR_MAX_NUM; i++)
	{
		LPITEM item = GetWear(i);
		if (item)
		{
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
		}
		else
		{
			p.equips[i].vnum = 0;
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return ((iLeaderShip >= 20) || ((iLeaderShip >= 12) && ((m_dwLastDeadTime + 180) > get_dword_time())));
}

// #define ENABLE_MOUNT_ENTITY_REFRESH
void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;
	if ((m_dwMountVnum != 0)&&(vnum!=0)) //@fixme108 set recursively to 0 for eventuality
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
#ifdef ENABLE_MOUNT_ENTITY_REFRESH
	// EncodeRemovePacket(this); // commented, otherwise it may warp you back
#endif
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

#ifdef ENABLE_MOUNT_ENTITY_REFRESH
		if (entity->IsType(ENTITY_CHARACTER))
		{
			EncodeRemovePacket(entity);
			if (!m_bIsObserver)
				EncodeInsertPacket(entity);

			if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
		}
		else
			EncodeInsertPacket(entity);
#else
		EncodeInsertPacket(entity);
#endif
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace {
	class FuncCheckWarp
	{
		public:
			FuncCheckWarp(LPCHARACTER pkWarp)
			{
				m_lTargetY = 0;
				m_lTargetX = 0;

				m_lX = pkWarp->GetX();
				m_lY = pkWarp->GetY();

				m_bInvalid = false;
				m_bEmpire = pkWarp->GetEmpire();

				char szTmp[64];

				if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
				{
					if (number(1, 100) < 5)
						sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

					m_bInvalid = true;

					return;
				}

				m_lTargetX *= 100;
				m_lTargetY *= 100;

				m_bUseWarp = true;

				if (pkWarp->IsGoto())
				{
					LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
					m_lTargetX += pkSectreeMap->m_setting.iBaseX;
					m_lTargetY += pkSectreeMap->m_setting.iBaseY;
					m_bUseWarp = false;
				}
			}

			bool Valid()
			{
				return !m_bInvalid;
			}

			void operator () (LPENTITY ent)
			{
				if (!Valid())
					return;

				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

				if (iDist > 300)
					return;

				if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
					return;

				if (pkChr->IsHack())
					return;

				if (!pkChr->CanHandleItem(false, true))
					return;

				if (m_bUseWarp)
					pkChr->WarpSet(m_lTargetX, m_lTargetY);
				else
				{
					pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
					pkChr->Stop();
				}
			}

			bool m_bInvalid;
			bool m_bUseWarp;

			long m_lX;
			long m_lY;
			long m_lTargetX;
			long m_lTargetY;

			BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}

void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ִ� ������ ������ �� �����ϴ�."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ִ� ������ ������ �� �����ϴ�."));
			return false;
		}
	}
	else
	{
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �¶��� ���°� �ƴմϴ�."));
			return false;
		}

		if (pcci->bChannel != g_bChannel)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ %d ä�ο� �ֽ��ϴ�. (���� ä�� %d)"), pcci->bChannel, g_bChannel);
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ִ� ������ ������ �� �����ϴ�."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �ִ� ������ ������ �� �����ϴ�."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("â�� ���� %d�� �̳����� �ٸ������� �̵��Ҽ� �����ϴ�."), limittime);

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true;
	}
	
#ifdef __TOMBOLA__
	if (m_pkTombolaRewardEvent)
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this while wheel is spinning."));

		return true;
	}
#endif

	if (bCheckShopOwner)
	{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || GetOfflineShopOwner()) 
#else
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
#endif
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ�â,â�� ���� �� ���¿����� �ٸ������� �̵�,���� �Ҽ� �����ϴ�"));

			return true;
		}
	}
	else
	{
		if (GetExchange() || GetMyShop() || IsOpenSafebox() || IsCubeOpen())
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ�â,â�� ���� �� ���¿����� �ٸ������� �̵�,���� �Ҽ� �����ϴ�"));

			return true;
		}
	}

	//PREVENT_PORTAL_AFTER_EXCHANGE
	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ� �� %d�� �̳����� �ٸ��������� �̵� �� �� �����ϴ�."), limittime );
		return true;
	}
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	//PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�ŷ� �� %d�� �̳����� �ٸ��������� �̵� �� �� �����ϴ�."), limittime);
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ ������ %d�� �̳����� ��ȯ��,��ȯ���θ� ����� �� �����ϴ�."), limittime);
		return true;
	}
	//END_PREVENT_ITEM_COPY

	return false;
}

BOOL CHARACTER::IsMonarch() const
{
	//MONARCH_LIMIT
	if (CMonarch::instance().IsMonarch(GetPlayerID(), GetEmpire()))
		return true;

	return false;

	//END_MONARCH_LIMIT
}
void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//
// Monarch
//
void CHARACTER::InitMC()
{
	for (int n = 0; n < MI_MAX; ++n)
	{
		m_dwMonarchCooltime[n] = thecore_pulse();
	}

	m_dwMonarchCooltimelimit[MI_HEAL] = PASSES_PER_SEC(MC_HEAL);
	m_dwMonarchCooltimelimit[MI_WARP] = PASSES_PER_SEC(MC_WARP);
	m_dwMonarchCooltimelimit[MI_TRANSFER] = PASSES_PER_SEC(MC_TRANSFER);
	m_dwMonarchCooltimelimit[MI_TAX] = PASSES_PER_SEC(MC_TAX);
	m_dwMonarchCooltimelimit[MI_SUMMON] = PASSES_PER_SEC(MC_SUMMON);

	m_dwMonarchCooltime[MI_HEAL] -= PASSES_PER_SEC(GetMCL(MI_HEAL));
	m_dwMonarchCooltime[MI_WARP] -= PASSES_PER_SEC(GetMCL(MI_WARP));
	m_dwMonarchCooltime[MI_TRANSFER] -= PASSES_PER_SEC(GetMCL(MI_TRANSFER));
	m_dwMonarchCooltime[MI_TAX] -= PASSES_PER_SEC(GetMCL(MI_TAX));
	m_dwMonarchCooltime[MI_SUMMON] -= PASSES_PER_SEC(GetMCL(MI_SUMMON));
}

DWORD CHARACTER::GetMC(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltime[e];
}

void CHARACTER::SetMC(enum MONARCH_INDEX e)
{
	m_dwMonarchCooltime[e] = thecore_pulse();
}

bool CHARACTER::IsMCOK(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if ((iPulse -  GetMC(e)) <  GetMCL(e))
	{
		if (test_server)
			sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

		return false;
	}

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return true;
}

DWORD CHARACTER::GetMCL(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltimelimit[e];
}

DWORD CHARACTER::GetMCLTime(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return  (GetMCL(e)) / passes_per_sec   -  (iPulse - GetMC(e)) / passes_per_sec;
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60*5);
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
#ifdef ENABLE_WOLFMAN_CHARACTER
		case MAIN_RACE_WOLFMAN_M:
#endif
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	// if (GetMaxHP() <= 0) // @fixme136
	// 	return 0;
	// return (GetHP() * 100) / GetMaxHP();

	if ((int64_t) GetMaxHP() <= 0)
		return 0;
	return ((int64_t) GetHP() * 100) / (int64_t) GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		if (g_bEnableSpeedHackCrash)
		{
			// close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60));
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC	*desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.emplace(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

BYTE CHARACTER::GetMountCounter() const
{
	return m_bMountCounter;
}

void CHARACTER::ResetMountCounter()
{
	m_bMountCounter = 0;
}

BYTE CHARACTER::IncreaseMountCounter()
{
	return ++m_bMountCounter;
}

bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen() || GetOfflineShopOwner())
#else
	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())	
#endif		
		return false;

	if ((iPulse - GetPlayerDamageTime()) < PASSES_PER_SEC(3))
		return false;

#ifdef __PREMIUM_PRIVATE_SHOP__
	if (GetViewingPrivateShop() || IsEditingPrivateShop() || IsShopSearch())
		return false;
#endif

	return true;
}

bool CHARACTER::IsInSafezone() const
{
	LPSECTREE sectree = GetSectree();
	return (sectree && sectree->IsAttr(GetX(), GetY(), ATTR_BANPK));
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_MAX_LEVEL_CONST < GetLevel())
		return 2500000000u;
	else
		return exp_table[GetLevel()];
}

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}

#ifdef WEEKLY_RANK_BYLUZER
bool CHARACTER::InDungeon() const
{
	int map_table[9] = { 12, 13, 14, 15, 16, 17, 18, 21, 24 };
	for (BYTE i = 0; i < 9; ++i)
	{
		if (GetMapIndex() >= map_table[i] * 10000 && GetMapIndex() < (map_table[i] + 1) * 10000)
			return true;
	}
	return false;
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
std::vector<LPITEM> CHARACTER::GetAcceMaterials()
{
	if (!m_PlayerSlots)
		return std::vector<LPITEM>{nullptr, nullptr};
	return std::vector<LPITEM>{ITEM_MANAGER::instance().Find(m_PlayerSlots->pAcceMaterials[0].id), ITEM_MANAGER::instance().Find(m_PlayerSlots->pAcceMaterials[1].id)};
}

const TItemPosEx* CHARACTER::GetAcceMaterialsInfo()
{
	if (!m_PlayerSlots)
		return nullptr;
	return m_PlayerSlots->pAcceMaterials;
}

void CHARACTER::SetAcceMaterial(int pos, LPITEM ptr)
{
	if (!m_PlayerSlots)
		return;

	if (pos < 0 || pos >= ACCE_WINDOW_MAX_MATERIALS)
		return;
	if (!ptr)
		m_PlayerSlots->pAcceMaterials[pos] = {};
	else
	{
		m_PlayerSlots->pAcceMaterials[pos].id = ptr->GetID();
		m_PlayerSlots->pAcceMaterials[pos].pos.cell = ptr->GetCell();
		m_PlayerSlots->pAcceMaterials[pos].pos.window_type = ptr->GetWindow();
	}
}

void CHARACTER::OpenAcce(bool bCombination)
{
	if (IsAcceOpened(bCombination))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The acce window it's already opened."));
		return;
	}

	if (bCombination)
	{
		if (m_bAcceAbsorption)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the acce absorption window."));
			return;
		}

		m_bAcceCombination = true;
	}
	else
	{
		if (m_bAcceCombination)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the acce combine window."));
			return;
		}

		m_bAcceAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.dwCheque = 0;
	sPacket.bChance = 0;
	int i;
	for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
	 	sPacket.materials[i].vnum = 0;
	 	sPacket.materials[i].count = 0;
	}
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	ClearAcceMaterials();
}

void CHARACTER::CloseAcce()
{
	if ((!m_bAcceCombination) && (!m_bAcceAbsorption))
		return;

	bool bWindow = m_bAcceCombination;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.dwCheque = 0;
	sPacket.bChance = 0;
	int i;
	for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	if (bWindow)
		m_bAcceCombination = false;
	else
		m_bAcceAbsorption = false;

	ClearAcceMaterials();
}

void CHARACTER::ClearAcceMaterials()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
		SetAcceMaterial(i, nullptr);
	}
}

bool CHARACTER::AcceIsSameGrade(long lGrade)
{
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0])
		return false;
	return pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD) == lGrade;
}

BYTE CHARACTER::GetAcceCombineChance(long lGrade)
{
	auto pkItemMaterial = GetAcceMaterials();
	BYTE bChance = 0;
	if ((pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET)) > 18) {
		bChance = ACCE_COMBINE_GRADE_5;
	} else {
		switch (lGrade)
		{
			case 2:
			{
				bChance = ACCE_COMBINE_GRADE_2;
			}
			break;
			case 3:
			{
				bChance = ACCE_COMBINE_GRADE_3;
			}
			break;
			case 4:
			{
				bChance = ACCE_COMBINE_GRADE_4;
			}
			break;
			default:
			{
				bChance = ACCE_COMBINE_GRADE_1;
			}
			break;
		}
	}

	return bChance;
}

long long CHARACTER::GetAcceCombineCheque(long lGrade)
{
	long long dwCheque = 0;
	auto pkItemMaterial = GetAcceMaterials();
	if ((pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET)) > 18) {
		dwCheque = ACCE_GRADE_5_PRICE;
	}
	else {
		switch (lGrade)
		{
			case 4:
			{
				dwCheque = ACCE_GRADE_4_PRICE;
			}
			break;
			case 5:
			{
				dwCheque = ACCE_GRADE_5_PRICE;
			}
			break;
		}
	}
	return dwCheque;
}

long long CHARACTER::GetAcceCombinePrice(long lGrade)
{
	long long dwPrice = 0;
	switch (lGrade)
	{
		case 1:
		{
			dwPrice = ACCE_GRADE_1_PRICE;
		}
		break;
		case 2:
		{
			dwPrice = ACCE_GRADE_2_PRICE;
		}
		break;
		case 3:
		{
			dwPrice = ACCE_GRADE_3_PRICE;
		}
		break;
	}
	return dwPrice;
}

void CHARACTER::SetAcceCombineMaterials(long lGrade)
{
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i) {
		taccematerials.vnum[i] = 0;
		taccematerials.count[i] = 0;
	}

	auto pkItemMaterial = GetAcceMaterials();
	if ((pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET)) > 18) {
		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i) {
			taccematerials.vnum[0] = ACCE_MATERIAL_VNUM_5;
			taccematerials.count[0] = ACCE_MATERIAL_COUNT_5;
		}
	} 
	else {
		switch (lGrade)
		{
			case 1:
			{
				taccematerials.vnum[0] = ACCE_MATERIAL_VNUM_1;
				taccematerials.count[0] = ACCE_MATERIAL_COUNT_1;
				break;
			}
			case 2:
			{
				taccematerials.vnum[0] = ACCE_MATERIAL_VNUM_2;
				taccematerials.count[0] = ACCE_MATERIAL_COUNT_2;
				break;
			}

			case 3:
			{
				taccematerials.vnum[0] = ACCE_MATERIAL_VNUM_3;
				taccematerials.count[0] = ACCE_MATERIAL_COUNT_3;
				break;
			}

			case 4:
			{
				taccematerials.vnum[0] = ACCE_MATERIAL_VNUM_4;
				taccematerials.count[0] = ACCE_MATERIAL_COUNT_4;
				break;
			}
		}
	}
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	auto pkItemMaterial = GetAcceMaterials();

	if (m_bAcceCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + ACCE_GRADE_4_ABS_RANGE > ACCE_GRADE_4_ABS_MAX ? ACCE_GRADE_4_ABS_MAX : (dwMinAbs + ACCE_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable * pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
				case 2:
				{
					dwMinAbs = ACCE_GRADE_3_ABS;
					dwMaxAbs = ACCE_GRADE_3_ABS;
				}
				break;
				case 3:
				{
					dwMinAbs = ACCE_GRADE_4_ABS_MIN;
					dwMaxAbs = ACCE_GRADE_4_ABS_MAX_COMB;
				}
				break;
				default:
				{
					dwMinAbs = ACCE_GRADE_2_ABS;
					dwMaxAbs = ACCE_GRADE_2_ABS;
				}
				break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAcceMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if (pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add locked items."));
		return;
	}
#ifdef __SOULBINDING_SYSTEM__
	else if ((pkItem->IsBind()) || (pkItem->IsUntilBind()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add binded items."));
		return;
	}
#endif
	else if ((m_bAcceCombination) && (bPos == 1) && (!AcceIsSameGrade(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD))))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can combine just accees of same grade."));
		return;
	}
	else if ((m_bAcceCombination) && (pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_GRADE_4_ABS_MAX))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This acce got already maximum absorption chance."));
		return;
	}
	else if ((bPos == 1) && (m_bAcceAbsorption))
	{
		if (pkItem->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can absorb just the bonuses from armors and weapons."));
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if ((pkItem->GetSocket(ACCE_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	auto pkItemMaterial = GetAcceMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

	if (pkItemMaterial[bPos])
		return;

	SetAcceMaterial(bPos, pkItem);
	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	SetAcceCombineMaterials(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = GetAcceCombinePrice(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));
	sPacket.dwCheque = GetAcceCombineCheque(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));
	sPacket.bChance = GetAcceCombineChance(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));

	int i;
	for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
		sPacket.materials[i].vnum = taccematerials.vnum[i];
		sPacket.materials[i].count = taccematerials.count[i];
	}

	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

void CHARACTER::RemoveAcceMaterial(BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	auto pkItemMaterial = GetAcceMaterials();

	long long dwPrice = 0;
	long long dwCheque = 0;
	BYTE bChance = 0;

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
			SetAcceMaterial(bPos, nullptr);
		}

		if (pkItemMaterial[0]) {
			dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
			dwCheque = GetAcceCombineCheque(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
			bChance = GetAcceCombineChance(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
			SetAcceCombineMaterials(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
		}
	}
	else
		ClearAcceMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = dwPrice;
	sPacket.dwCheque = dwCheque;
	sPacket.bChance = bChance;

	int i;
	for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
		sPacket.materials[i].vnum = 0;
		sPacket.materials[i].count = 0;
	}
	
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

BYTE CHARACTER::CanRefineAcceMaterials()
{
	BYTE bReturn = 0;
	if (!GetDesc())
		return bReturn;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		return bReturn;

	auto materialInfo = GetAcceMaterialsInfo();
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0] || !pkItemMaterial[1])
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial null");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetOwner()!=this || pkItemMaterial[1]->GetOwner() != this)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial different ownership");
		return bReturn;
	}
	else if (pkItemMaterial[0]->IsEquipped() || pkItemMaterial[1]->IsEquipped())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial equipped");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetWindow() != INVENTORY || pkItemMaterial[1]->GetWindow() != INVENTORY)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial not in INVENTORY");
		return bReturn;
	}
	else if (!materialInfo[0].id || !materialInfo[1].id)
	{
		sys_err("CanRefineAcceMaterials: materialInfo id 0");
		return bReturn;
	}
	else if (materialInfo[0].pos.cell != pkItemMaterial[0]->GetCell() || materialInfo[1].pos.cell != pkItemMaterial[1]->GetCell())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong cell");
		return bReturn;
	}
	else if (materialInfo[0].pos.window_type != pkItemMaterial[0]->GetWindow() || materialInfo[1].pos.window_type != pkItemMaterial[1]->GetWindow())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong window_type");
		return bReturn;
	}

	if (m_bAcceCombination)
	{
		if (!AcceIsSameGrade(pkItemMaterial[1]->GetValue(ACCE_GRADE_VALUE_FIELD)))
		{
			sys_err("CanRefineAcceMaterials: pkItemMaterial different acce grade");
			return bReturn;
		}

		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_ACCE))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAcceAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_ACCE))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}
	return bReturn;
}

void CHARACTER::RefineAcceMaterials()
{
	BYTE bCan = CanRefineAcceMaterials();
	if (bCan == 0)
		return;

	auto pkItemMaterial = GetAcceMaterials();

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
#ifdef ENABLE_LONG_LONG
	long long dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
#else
	DWORD dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
#endif

	long long dwCheque = GetAcceCombineCheque(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));

	BYTE bChance = GetAcceCombineChance(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));

	int i;

	if (bCan == 1)
	{
		long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);

		if (GetGold() < dwPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
			return;
		}

		if (GetCheque() < dwCheque)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Cheque."));
			return;
		}
		switch (lVal)
		{
			case 1:
			case 2:
			case 3:
			{
				if (CountSpecifyItem(taccematerials.vnum[0]) >= taccematerials.count[0]) {
					RemoveSpecifyItem(taccematerials.vnum[0], taccematerials.count[0]);
				} else {
					ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganych przedmiot?.");
					return;
				}
			}
			break;
			case 4:
			{
				if (CountSpecifyItem(taccematerials.vnum[0]) >= taccematerials.count[0] &&
					CountSpecifyItem(taccematerials.vnum[1]) >= taccematerials.count[1])
				{
					RemoveSpecifyItem(taccematerials.vnum[0], taccematerials.count[0]);
					RemoveSpecifyItem(taccematerials.vnum[1], taccematerials.count[1]);
				} else {
					ChatPacket(CHAT_TYPE_INFO, "Nie posiadasz wymaganych przedmiot?.");
					return;
				}
			}
			break;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= bChance ? true : false);
		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}

			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));
			pkItem->SetSocket(ACCE_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET));

			PointChange(POINT_GOLD, -dwPrice);
			PointChange(POINT_CHEQUE, -dwCheque);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");

			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), dwAbs);
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));

			EffectPacket(SE_EFFECT_ACCE_SUCCEDED);
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);

			ClearAcceMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			PointChange(POINT_CHEQUE, -dwCheque);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d%"), pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed."));

			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);

			pkItemMaterial[1] = NULL;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.dwCheque = dwCheque;
		sPacket.bChance = bChance;
		for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
			sPacket.materials[i].vnum = 0;
			sPacket.materials[i].count = 0;
		}
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		#ifdef USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}
		#endif
		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success."));
		EffectPacket(SE_EFFECT_ACCE_SUCCEDED);

		ClearAcceMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.dwCheque = dwCheque;
		sPacket.bChance = bChance;
		for (i = 0; i < ACCE_MATERIALS_COUNT; ++i) {
		 	sPacket.materials[i].vnum = 0;
		 	sPacket.materials[i].count = 0;
		}
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
}

bool CHARACTER::CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Close the other windows."));
		return false;
	}
	else if ((!pkItem) || (!pkTarget))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pkItem or pkTarget nullptr."));
		return false;
	}
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_ACCE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use it on non-sash items."));
		return false;
	}

	if (pkTarget->GetSocket(ACCE_ABSORBED_SOCKET) <= 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The sash item has no absorbed socket."));
		return false;
	}

	pkTarget->SetSocket(ACCE_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#ifdef ENABLE_ALIGN_RENEWAL
void CHARACTER::OnAlignUpdate(long long lOldAlignment)
{
	if (GetRealAlignment() >= 500000)
	{
		if (lOldAlignment < 500000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 100, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 100, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_FINAL_DMG_BONUS, 10, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 40000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 450000)
	{
		if (lOldAlignment < 450000 && lOldAlignment >= 500000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 75, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 75, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_FINAL_DMG_BONUS, 5, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 30000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 350000)
	{
		if (lOldAlignment < 350000 && lOldAlignment >= 450000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 50, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 50, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_FINAL_DMG_BONUS, 3, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 20000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 250000)
	{
		if (lOldAlignment < 250000 && lOldAlignment >= 350000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 35, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 35, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 15000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 150000)
	{
		if (lOldAlignment < 150000 && lOldAlignment >= 250000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 25, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 25, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 10000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 50000)
	{
		if (lOldAlignment < 50000 && lOldAlignment >= 150000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 15, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 15, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 5000, 0, 60*60*60*365, 0, true);
	}
	else if (GetRealAlignment() >= 25000)
	{
		if (lOldAlignment < 25000 && lOldAlignment >= 50000)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		AddAffect(AFFECT_ALIGN_BONUS1, POINT_ATTBONUS_MONSTER, 10, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS2, POINT_ATTBONUS_HUMAN, 10, 0, 60*60*60*365, 0, true);
		AddAffect(AFFECT_ALIGN_BONUS3, POINT_MAX_HP, 2500, 0, 60*60*60*365, 0, true);
	}
	else
	{
		if (lOldAlignment >= 20000 && GetRealAlignment() >= 0)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
		else if (lOldAlignment >= 0 && GetRealAlignment() < 0)
		{
			RemoveAffect(AFFECT_ALIGN_BONUS1);
			RemoveAffect(AFFECT_ALIGN_BONUS2);
			RemoveAffect(AFFECT_ALIGN_BONUS3);
			RemoveAffect(AFFECT_ALIGN_BONUS4);
		}
	}
}
#endif
#ifdef __ITEM_SHOP__
DWORD CHARACTER::GetCash()
{
	if (GetDesc())
	{
		char szQuery[128];
		snprintf(szQuery, sizeof(szQuery),
			"SELECT `cash` FROM `srv1_account`.`account` WHERE `id` = %d",
			GetDesc()->GetAccountTable().id
		);

		std::unique_ptr<SQLMsg> pMsg (DBManager::Instance().DirectQuery(szQuery));

		if (pMsg->uiSQLErrno != 0)
		{
			sys_err("Failed to get cash, error code %d", pMsg->uiSQLErrno);
			return 0;
		}

		if (pMsg->Get() && pMsg->Get()->pSQLResult)
		{
			DWORD dwCash = 0;
			str_to_number(dwCash, mysql_fetch_row(pMsg->Get()->pSQLResult)[0]);
			return dwCash;
		}
	}
	return 0;
}

void CHARACTER::SetCash(DWORD dwCash)
{
	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery),
		"UPDATE `srv1_account`.`account` SET `cash` = %d WHERE `id` = %d",
		dwCash, GetDesc()->GetAccountTable().id
		);

	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));

	if (pMsg->uiSQLErrno != 0)
		sys_err("Failed to set cash, error code %d", pMsg->uiSQLErrno);
}

bool CHARACTER::CanActItemShop()
{
	auto delta_time = m_iLastItemShopTime - get_global_time();
	if (delta_time > 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP COOLDOWN %lu"), delta_time);
		return false;
	}

	if (!CanHandleItem() || GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
	{
		CItemShopManager::instance().SendError(EItemShopError::ITEMSHOP_ERROR_WARP, this);
		return false;
	}

	m_iLastItemShopTime = get_global_time() + ITEMSHOP_COOLDOWN;

	return true;
}
#endif

#ifdef __TOMBOLA__
void CHARACTER::SetTombolaPrize(CTombola::TTombolaItem pkPrizeVec)
{
	SetQuestFlag("__tombola.reward_vnum", pkPrizeVec.dwVnum);
	SetQuestFlag("__tombola.reward_count", pkPrizeVec.dwCount);
}

CTombola::TTombolaItem CHARACTER::GetTombolaPrize()
{ 
	auto prize = CTombola::TTombolaItem{};
	prize.dwVnum = GetQuestFlag("__tombola.reward_vnum");
	prize.dwCount = GetQuestFlag("__tombola.reward_count");

	return prize;
}

EVENTFUNC(tombola_reward_event)
{
	auto info = dynamic_cast<CHARACTER::tombola_reward_event_info*>(event->info);
	if (!info)
	{
		sys_err("tombola_reward_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (!ch || ch->IsNPC())
		return 0;

	LPITEM pkItem = info->item;

	if (!pkItem)
		return 0;

	ch->AutoGiveItem(pkItem->GetVnum(), pkItem->GetCount(), -1, false);
	ch->ChatPacket(CHAT_TYPE_INFO, "[Ko? Losu] Wylosowa?�: %s (x%d)", pkItem->GetName(), pkItem->GetCount());

	const CTombola::TTombolaItem empty_item{};
	ch->SetTombolaPrize(empty_item);

	event_cancel(&ch->m_pkTombolaRewardEvent);

	return 0;
}

void CHARACTER::TombolaRewardEvent(int iSecond)
{
	if (m_pkTombolaRewardEvent)
		return;

	tombola_reward_event_info* info = AllocEventInfo<tombola_reward_event_info>();
	const auto item = GetTombolaPrize();

	info->ch = this;
	info->item = ITEM_MANAGER::instance().CreateItem(item.dwVnum, item.dwCount);

	m_pkTombolaRewardEvent = event_create(tombola_reward_event, info, PASSES_PER_SEC(iSecond));
}
#endif

#ifdef ENABLE_SORT_INVENTORY_ITEMS
static bool SortItems(const LPITEM & s1, const LPITEM & s2)
{
	std::string name(s1->GetName());
	std::string name2(s2->GetName());

	return name < name2;
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
void CHARACTER::SortSpecialInventoryItems(BYTE type)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::SortInventory, std::chrono::milliseconds(10000)))
    {
        ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::SortInventory));
        return;
    }
#endif

	if (IsDead() || GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo?sz tego teraz zrobi?spr?uj ponownie p?niej.");
		return;
	}

	int slot_start = 0;
	int slot_end = 0;

	switch (type) {
		default:
		case 0:
			slot_start = SKILL_BOOK_INVENTORY_SLOT_START; 
			slot_end = SKILL_BOOK_INVENTORY_SLOT_END;
			break;
		case 1:
			slot_start = UPGRADE_ITEMS_INVENTORY_SLOT_START; 
			slot_end = UPGRADE_ITEMS_INVENTORY_SLOT_END;
			break;
		case 2:
			slot_start = STONE_INVENTORY_SLOT_START;
			slot_end = STONE_INVENTORY_SLOT_END;
			break;
		case 3:
			slot_start = BOX_INVENTORY_SLOT_START; 
			slot_end = BOX_INVENTORY_SLOT_END;
			break;
		case 4:
			slot_start = EFSUN_INVENTORY_SLOT_START; 
			slot_end = EFSUN_INVENTORY_SLOT_END;
			break;
		case 5:
			slot_start = CICEK_INVENTORY_SLOT_START; 
			slot_end = CICEK_INVENTORY_SLOT_END;
			break;
		case 6:
			slot_start = SKILL_BOOK_INVENTORY_SLOT_START; 
			slot_end = CICEK_INVENTORY_SLOT_END;
			break;
	}

	for (int i = slot_start; i < slot_end; ++i) {
		LPITEM item = GetInventoryItem(i);
		if(!item)
            continue;
       
        if(item->isLocked())
            continue;
       
        if(item->GetCount() == g_bItemCountLimit)
            continue;

		if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
        {
            for (int j = i; j < slot_end; ++j)
            {
                LPITEM item2 = GetInventoryItem(j);
               
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
	ChatPacket(CHAT_TYPE_INFO, "Przedmioty zosta? po��czone.");
}
#endif
#endif

#ifdef ENABLE_AURA_SYSTEM
void CHARACTER::OpenAura(bool bCombination)
{
	if (isAuraOpened(bCombination))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The aura window it's already opened."));
		return;
	}

	if (bCombination)
	{
		if (m_bAuraAbsorption)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the aura absorption window."));
			return;
		}

		m_bAuraRefine = true;
	}
	else
	{
		if (m_bAuraRefine)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you may close the aura combine window."));
			return;
		}

		m_bAuraAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_GC_AURA;
	sPacket.subheader = AURA_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAura));

	ClearAuraMaterials();
}

void CHARACTER::CloseAura()
{
	if ((!m_bAuraRefine) && (!m_bAuraAbsorption))
		return;

	bool bWindow = (m_bAuraRefine == true ? true : false);

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_GC_AURA;
	sPacket.subheader = AURA_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAura));

	if (bWindow)
		m_bAuraRefine = false;
	else
		m_bAuraAbsorption = false;

	ClearAuraMaterials();
}

void CHARACTER::ClearAuraMaterials()
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();
	for (int i = 0; i < AURA_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
	}
}

bool CHARACTER::AuraIsSameGrade(long lGrade)
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();
	if (!pkItemMaterial[0])
		return false;

	bool bReturn = (pkItemMaterial[0]->GetValue(AURA_GRADE_VALUE_FIELD) == lGrade ? true : false);
	return bReturn;
}

DWORD CHARACTER::GetAuraCombinePrice(long lGrade)
{
	DWORD dwPrice = AURA_REFINE_PRICE;
	return dwPrice;
}

BYTE CHARACTER::CheckAuraEmptyMaterialSlot()
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();
	for (int i = 0; i < AURA_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAuraCombineResult(DWORD& dwItemVnum, DWORD& dwMinAbs, DWORD& dwMaxAbs)
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();

	if (m_bAuraRefine)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(AURA_TYPE_VALUE_FIELD);
			if (lVal == 6)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable* pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(AURA_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAuraMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= AURA_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= AURA_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME && pkItem->GetType() != COSTUME_AURA) && (bPos == 0) && (m_bAuraRefine))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME && pkItem->GetType() != COSTUME_AURA) && (bPos == 0) && (m_bAuraAbsorption))
		return;
	else if (pkItem->isLocked())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't add locked items."));
		return;
	}
#ifdef ENABLE_SOULBIND_SYSTEM
	else if (pkItem->IsBind() || pkItem->IsUntilBind())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MAILBOX_TRANSMUTATION_FX"));
		return;
	}
#endif
	else if ((m_bAuraRefine) && (pkItem->GetSocket(AURA_ABSORPTION_SOCKET) >= AURA_MAX_ABS))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This aura got already maximum absorption chance."));
		return;
	}
	else if ((bPos == 1) && (m_bAuraAbsorption))
	{
		if ((pkItem->GetType() != ITEM_ARMOR))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura You can absorb just the bonuses from armors and weapons."));
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_SHIELD) && (pkItem->GetSubType() != ARMOR_EAR) && (pkItem->GetSubType() != ARMOR_WRIST) && (pkItem->GetSubType() != ARMOR_NECK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura You can absorb just the bonuses from armors and weapons."));
			return;
		}
	}
	else if ((bPos == 1) && (m_bAuraRefine))
	{
		if ((pkItem->GetVnum() != AURA_ICE_RUNIC))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Only Refine Material Ice Runic"));
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_AURA) && (m_bAuraAbsorption))
		return;
	else if ((pkItem->GetSubType() != COSTUME_AURA) && (bPos == 0) && (m_bAuraAbsorption))
		return;
	else if ((pkItem->GetSocket(AURA_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAuraAbsorption))
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

	if (pkItemMaterial[bPos])
		return;

	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAuraCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAura sPacket;
	sPacket.header = HEADER_GC_AURA;
	sPacket.subheader = AURA_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAuraRefine == true ? true : false;
	sPacket.dwPrice = GetAuraCombinePrice(pkItem->GetValue(AURA_GRADE_VALUE_FIELD));
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAura));
}

void CHARACTER::RemoveAuraMaterial(BYTE bPos)
{
	if (bPos >= AURA_WINDOW_MAX_MATERIALS)
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();

	DWORD dwPrice = 0;

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
		}

		if (pkItemMaterial[0])
			dwPrice = GetAuraCombinePrice(pkItemMaterial[0]->GetValue(AURA_GRADE_VALUE_FIELD));
	}
	else
		ClearAuraMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAura sPacket;
	sPacket.header = HEADER_GC_AURA;
	sPacket.subheader = AURA_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAuraRefine == true ? true : false;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAura));
}

BYTE CHARACTER::CanRefineAuraMaterials()
{
	BYTE bReturn = 0;
	LPITEM* pkItemMaterial = GetAuraMaterials();
	if (m_bAuraRefine)
	{
		for (int i = 0; i < AURA_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i] != NULL)
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_AURA))
					bReturn = 1;
				else if ((pkItemMaterial[i]->GetVnum() == AURA_ICE_RUNIC))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAuraAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_AURA))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && ((pkItemMaterial[1]->GetSubType() == ARMOR_EAR) || (pkItemMaterial[1]->GetSubType() == ARMOR_WRIST) || (pkItemMaterial[1]->GetSubType() == ARMOR_NECK) || (pkItemMaterial[1]->GetSubType() == ARMOR_SHIELD)))
				bReturn = 2;
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(AURA_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}

	return bReturn;
}

void CHARACTER::RefineAuraMaterials()
{
	BYTE bCan = CanRefineAuraMaterials();
	if (bCan == 0)
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAuraMaterials();

	if (!pkItemMaterial[0] || !pkItemMaterial[1])
		return;

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAuraCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
	DWORD dwPrice = AURA_REFINE_PRICE;

	if (bCan == 1)
	{
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(AURA_TYPE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
		{
			iSuccessChance = AURA_GRADE_2_REFINE;
		}
		break;
		case 3:
		{
			iSuccessChance = AURA_GRADE_3_REFINE;
		}
		break;
		case 4:
		{
			iSuccessChance = AURA_GRADE_4_REFINE;
		}
		break;
		case 5:
		{
			iSuccessChance = AURA_GRADE_5_REFINE;
		}
		break;
		case 6:
		{
			iSuccessChance = AURA_GRADE_6_REFINE;
		}
		break;
		default:
		{
			iSuccessChance = AURA_GRADE_1_REFINE;
		}
		break;
		}

		if (GetGold() < dwPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura You don't have enough Yang."));
			return;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);
		if (pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 49 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 99 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 149 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 199 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 249)
			bSucces = true;
		int oldGradeValue = pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD);
		bool bPacket = true;
		if (bSucces)
		{
			if (pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 49 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 99 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 149 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 199 && pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) != 249)
			{
				PointChange(POINT_GOLD, -dwPrice);
				pkItemMaterial[0]->SetSocket(AURA_ABSORPTION_SOCKET, pkItemMaterial[0]->GetSocket(AURA_ABSORPTION_SOCKET) + 1);
				pkItemMaterial[0]->SetSocket(AURA_GRADE_VALUE_FIELD, pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) + 1);
				bool bDelete = false;
				if (pkItemMaterial[1]->GetCount() - 1 < 1)
					bDelete = true;
				pkItemMaterial[1]->SetCount(pkItemMaterial[1]->GetCount() - 1);
				if (bDelete == true)
					pkItemMaterial[1] = NULL;
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Success."));
			}
			else
			{
				LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
				if (!pkItem)
				{
					sys_err("%d can't be created.", dwItemVnum);
					return;
				}

				ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
				LogManager::instance().ItemLog(this, pkItem, "AURA REFINE SUCCESS", pkItem->GetName());
				pkItem->SetSocket(AURA_ABSORPTION_SOCKET, pkItem->GetSocket(AURA_ABSORPTION_SOCKET) + 1);
				pkItem->SetSocket(AURA_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(AURA_ABSORBED_SOCKET));
				pkItem->SetSocket(AURA_GRADE_VALUE_FIELD, pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) + 1);

				PointChange(POINT_GOLD, -dwPrice);
				DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

				WORD wCell = pkItemMaterial[0]->GetCell();
				ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "AURA (REFINE SUCCESS)");
				pkItemMaterial[1]->SetCount(pkItemMaterial[1]->GetCount() - 1);

				pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
				ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
				pkItem->AttrLog();

				if (lVal == 6)
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura New absorption rate: %2f%"), float(pkItem->GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Success."));

				ClearAuraMaterials();
				bPacket = false;
			}
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			pkItemMaterial[1]->SetCount(pkItemMaterial[1]->GetCount() - 1);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			if (lVal == 6)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura New absorption rate: %d%"), pkItemMaterial[0]->GetSocket(AURA_ABSORPTION_SOCKET));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Failed."));

			if (pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) == 50 || pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) == 100 || pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) == 150 || pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) == 200 || pkItemMaterial[0]->GetSocket(AURA_GRADE_VALUE_FIELD) == 250)
				ClearAuraMaterials();
			bPacket = false;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAura sPacket;
		sPacket.header = HEADER_GC_AURA;
		sPacket.subheader = AURA_SUBHEADER_GC_REFINED;
		sPacket.bWindow = m_bAuraRefine == true ? true : false;
		sPacket.dwPrice = dwPrice;
		if (pkItemMaterial[0])
		{
			if (bPacket)
				if (oldGradeValue != 49 && oldGradeValue != 99 && oldGradeValue != 149 && oldGradeValue != 199 && oldGradeValue != 249)
					sPacket.bPos = 255;
				else
					sPacket.bPos = 0;
			else
				sPacket.bPos = 0;
		}
		else
			sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAura));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(AURA_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}

		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Success."));

		ClearAuraMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAura sPacket;
		sPacket.header = HEADER_GC_AURA;
		sPacket.subheader = AURA_SUBHEADER_GC_REFINED;
		sPacket.bWindow = m_bAuraRefine == true ? true : false;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAura));
	}
}

bool CHARACTER::CleanAuraAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;
	else if ((!pkItem) || (!pkTarget))
		return false;
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_AURA))
		return false;

	if (pkTarget->GetSocket(AURA_ABSORBED_SOCKET) <= 0)
		return false;

	pkTarget->SetSocket(AURA_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	pkItem->SetCount(pkItem->GetCount() - 1);
	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#if defined(__BL_MOVE_CHANNEL__)
EVENTINFO(move_channel_info)
{
	LPCHARACTER		ch;
	int				left_second;
	long			lAddr;
	WORD			wPort;

	move_channel_info(const LPCHARACTER m_ch, const int m_sec, const long m_Addr, const WORD m_Port) :
		ch(m_ch),
		left_second(m_sec),
		lAddr(m_Addr),
		wPort(m_Port)
	{}
};

EVENTFUNC(move_channel_event)
{
	move_channel_info* info = dynamic_cast<move_channel_info*>(event->info);

	if (info == nullptr)
	{
		sys_err("<move_channel_event> <Factor> Null pointer");
		return 0;
	}

	const LPCHARACTER ch = info->ch;
	if (ch == nullptr)
		return 0;

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = nullptr;
		if (ch->CanWarp())
			ch->WarpSet(ch->GetX(), ch->GetY(), 0, info->lAddr, info->wPort);
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MOVE_CHANNEL_CANCEL"));
		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MOVE_CHANNEL_TIMER"), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::MoveChannel(const TRespondMoveChannel* p)
{
	if (p == nullptr)
		return;
	
	const LPDESC d = GetDesc();
	if (d == nullptr)
		return;

	if (p->wPort == 0 
		|| p->lAddr == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MOVE_CHANNEL_ERROR"));
		return;
	}

	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MOVE_CHANNEL_CANCEL"));
		event_cancel(&m_pkTimedEvent);
		return;
	}

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("MOVE_CHANNEL_SUCCESS"));
	if (IsHack(false, true, 10))
		return;

	m_pkTimedEvent = event_create(move_channel_event, 
		new move_channel_info(this, 3, p->lAddr, p->wPort), 1);
		//new move_channel_info(this, IsPosition(POS_FIGHTING) ? 10 : 3, p->lAddr, p->wPort), 1);
}
#endif

// LPREGEN CHARACTER::GetRegen()
// {
// 	return m_pkRegen;
// }

#ifdef ENABLE_NEW_PET_SYSTEM
void CHARACTER::SendPetInfo(bool summon)
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->SendPetInfo(summon);
		return;
	}
}

void CHARACTER::PetSkillUp(int skill_id, bool set_from_quest)
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->SetSkillUp(skill_id, set_from_quest);
		return;
	}
}

void CHARACTER::PetSetExp(int count)
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->SetExp(count);
		return;
	}
}

void CHARACTER::SummonPet()
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->SummonPet();
		return;
	}
}

void CHARACTER::AddPetAffect()
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->AddPetAffect();
		return;
	}
}

void CHARACTER::UpdatePet()
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->UpdatePet();
		return;
	}
}

bool CHARACTER::PetName(std::string name)
{
	if (CountSpecifyItem(ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_VNUM) < ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_COUNT)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need special item for change pet name.."));
		return false;
	}

	RemoveSpecifyItem(ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_VNUM, ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_COUNT);

	if (name.length() < 1 || name.length() >= CHARACTER_NAME_MAX_LEN || check_name(name.c_str()) == 0)
	{
		return false;
	}

	char nameEscaped[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(nameEscaped, sizeof(nameEscaped), name.c_str(), strlen(name.c_str()));
	CHorseNameManager::instance().UpdatePetName(GetPlayerID(), nameEscaped, true);

	return true;
}
void CHARACTER::PetSetExpPercent(int count)
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		petSystem->SetExpPercent(count);
		return;
	}
}
int CHARACTER::PetGetLevel()
{
	CPetSystem* petSystem = GetPetSystem();
	if (petSystem)
	{
		return petSystem->GetLevel();
	}
}
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
void CHARACTER::SetBattlePass(CBattlePassManager* pBattlePass)
{
	if (m_pkBattlePass)
		delete m_pkBattlePass;
	
	m_pkBattlePass = pBattlePass;
}
#endif

#ifdef WEEKLY_RANK_BYLUZER
INT CHARACTER::GetWeekly(BYTE id) const
{
	if (id == 1)
		return m_points.w_monsters;
	else if (id == 2)
		return m_points.w_bosses;
	else if (id == 3)
		return m_points.w_metins;
	else if (id == 4)
		return m_points.w_dungs;
	else if (id == 5)
		return m_points.w_ds;
	else if (id == 6)
		return m_points.w_refine;
	else if (id == 7)
		return m_points.w_sm;
	else if (id == 8)
		return m_points.weekly_season;
	else
		return 0;
}

void CHARACTER::SetWeekly(BYTE id, int value)
{
	if (id == 1)
		m_points.w_monsters = value;
	else if (id == 2)
		m_points.w_bosses = value;
	else if (id == 3)
		m_points.w_metins = value;
	else if (id == 4)
		m_points.w_dungs = value;
	else if (id == 5)
		m_points.w_ds = value;
	else if (id == 6)
		m_points.w_refine = value;
	else if (id == 7)
		m_points.w_sm = value;
	else if (id == 8)
		m_points.weekly_season = value;
	else
		return;
}

void CHARACTER::CheckWeekly()
{
	int weekly_season = m_pointsInstant.weekly_season;
	if (weekly_season == 0)
	{
		std::unique_ptr<SQLMsg> pkMsg(DBManager::instance().DirectQuery("SELECT season from weekly_season where weekly=1"));
		auto uiNumRows = pkMsg->Get()->uiNumRows;
		if (uiNumRows == 0)
			return;
		MYSQL_ROW row = mysql_fetch_row(pkMsg->Get()->pSQLResult);
		str_to_number(weekly_season, row[0]);
		
		m_pointsInstant.weekly_season = weekly_season;
	}

	if (GetWeekly(8) == weekly_season)
		return;

	for (int i = 0; i < 9; i++)
		SetWeekly(i+1, 0);

	SetWeekly(8, weekly_season);
}
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
void CHARACTER::WonExchange(BYTE bOption, WORD wValue)
{
#ifdef ENABLE_NEWSTUFF
	if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::WonExchange, std::chrono::milliseconds(3000)))
    {
        ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Odczekaj %.2f sekund."), PULSEMANAGER_CLOCK_TO_SEC2(GetPlayerID(), ePulse::WonExchange));
        return;
    }
#endif
	if (!CanWarp())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring0"));
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring1"));
		return;
	}

	WORD wVal = abs(wValue);
	if (bOption == WON_EXCHANGE_CG_SUBHEADER_SELL)
	{
		if (GetCheque() < wVal)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring2"));
			return;
		}
		if (GetGold() + static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE) > GOLD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring3"));
			return;
		}
		
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring4"));
		PointChange(POINT_GOLD, static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE), true);
		PointChange(POINT_CHEQUE, -wVal, true);
	}
	else if (bOption == WON_EXCHANGE_CG_SUBHEADER_BUY)
	{
		if (GetGold() < static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring5"));
			return;
		}
		if (GetCheque() + wVal > CHEQUE_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring6"));
			return;
		}

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring4"));
		PointChange(POINT_GOLD, -(static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE)), true);
		PointChange(POINT_CHEQUE, wVal, true);
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("localestring7"));
	}
}
#endif


#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
void CHARACTER::SendBuffNPCUseSkillPacket(int skill_vnum, int skill_level)
{
	TPacketGCBuffNPCUseSkill pack;
	pack.bHeader = HEADER_GC_BUFF_NPC_USE_SKILL;
	pack.dSkillVnum = skill_vnum;
	pack.dVid = GetVID();
	pack.dSkillLevel = skill_level;
	
	PacketView(&pack, sizeof(TPacketGCBuffNPCUseSkill), this);
}

void CHARACTER::LoadBuffNPC()
{
	if (GetBuffNPCSystem() != NULL) {
		GetBuffNPCSystem()->LoadBuffNPC();
		if (GetQuestFlag("buff_npc.is_summon") == 1)
			GetBuffNPCSystem()->Summon();
	}
}
#endif

#if defined(__DUNGEON_INFO_SYSTEM__)
EVENTFUNC(ReloadDungeonInfoEventFunc)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);

	if (info == NULL)
	{
		sys_err("ReloadDungeonInfoEventFunc> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->IsDungeonInfoOpen())
	{
		ch->StopDungeonInfoReloadEvent();
		return 0;
	}

	CDungeonInfoManager::Instance().SendInfo(ch);

	return PASSES_PER_SEC(DungeonInfo::MAX_REFRESH_DELAY);
}

void CHARACTER::StartDungeonInfoReloadEvent()
{
	if (m_pkDungeonInfoReloadEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkDungeonInfoReloadEvent = event_create(ReloadDungeonInfoEventFunc, info, PASSES_PER_SEC(DungeonInfo::MAX_REFRESH_DELAY));
}

void CHARACTER::StopDungeonInfoReloadEvent()
{
	event_cancel(&m_pkDungeonInfoReloadEvent);
}

bool CHARACTER::UpdateDungeonRanking(const std::string c_strQuestName)
{
    if (c_strQuestName.empty())
    {
        sys_err("UpdateDungeonRanking: Quest name is empty for player %u", GetPlayerID());
        return false;
    }

    long lMapIndex = GetMapIndex() >= 10000 ? GetMapIndex() / 10000 : GetMapIndex();

    int iEnterTime = GetQuestFlag(c_strQuestName + ".enter_time");
    if (iEnterTime <= 0)
    {
        sys_err("UpdateDungeonRanking: Invalid enter_time for quest %s, player %u", c_strQuestName.c_str(), GetPlayerID());
        return false;
    }

    int iRunTime = MAX(get_global_time() - iEnterTime, 0);
    int iDamage = MAX(GetLastDamage(), 0);

    bool bQueryUpdate = true;
    char szQuery[QUERY_MAX_LEN];

    snprintf(szQuery, sizeof(szQuery), "SELECT `finish_time`, `finish_damage` FROM dungeon_ranking%s WHERE `pid` = '%u' AND `dungeon_index` = '%ld'", get_table_postfix(), GetPlayerID(), lMapIndex);
    sys_log(0, "Executing query: %s", szQuery);

    std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
    if (!pMsg || !pMsg->Get())
    {
        sys_err("UpdateDungeonRanking: Failed to execute query: %s", szQuery);
        return false;
    }

    if (pMsg->Get()->uiNumRows <= 0)
    {
        snprintf(szQuery, sizeof(szQuery), "INSERT INTO dungeon_ranking%s (`pid`, `dungeon_index`, `finish`, `finish_time`, `finish_damage`) VALUES ('%u', '%ld', '%d', '%d', '%d')",
                 get_table_postfix(), GetPlayerID(), lMapIndex, 1, iRunTime, iDamage);
        sys_log(0, "Executing query: %s", szQuery);

        std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
        if (!pMsg || !pMsg->Get())
        {
            sys_err("UpdateDungeonRanking: Failed to execute insert query: %s", szQuery);
            return false;
        }
        bQueryUpdate = false;
    }

    MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
    if (!row)
    {
        sys_err("UpdateDungeonRanking: Failed to fetch row for query: %s", szQuery);
        return false;
    }

    int iLastRunTime = 0;
    str_to_number(iLastRunTime, row[0]);

    int iLastDamage = 0;
    str_to_number(iLastDamage, row[1]);

    std::initializer_list<const char*> c_szList = {"finish", "finish_time", "finish_damage"};
    int iRow = 0;

    for (const char* c_szRow : c_szList)
    {
        char szUpdateQuery[QUERY_MAX_LEN];
        std::string strQuestFlagName = c_strQuestName + "." + static_cast<std::string>(c_szRow);

        if (iRow == 0) // finish
        {
            SetQuestFlag(strQuestFlagName, GetQuestFlag(strQuestFlagName) + 1);

            if (bQueryUpdate)
            {
                snprintf(szUpdateQuery, sizeof(szUpdateQuery), "UPDATE dungeon_ranking%s SET `%s` = `%s` + 1 WHERE `pid` = '%u' AND `dungeon_index` = '%ld'",
                         get_table_postfix(), c_szRow, c_szRow, GetPlayerID(), lMapIndex);
            }
        }
        else if (iRow == 1) // finish_time
        {
            if (iRunTime > iLastRunTime)
                break;

            SetQuestFlag(strQuestFlagName, iRunTime);

            if (bQueryUpdate)
            {
                snprintf(szUpdateQuery, sizeof(szUpdateQuery), "UPDATE dungeon_ranking%s SET `%s` = '%d' WHERE `pid` = '%u' AND `dungeon_index` = '%ld'",
                         get_table_postfix(), c_szRow, iRunTime, GetPlayerID(), lMapIndex);
            }
        }
        else if (iRow == 2) // finish_damage
		{
			if (iDamage <= iLastDamage) // Aktualizuj tylko, je?li nowy damage jest mniejszy lub rowny
			{
				SetQuestFlag(strQuestFlagName, iDamage);

				if (bQueryUpdate)
				{
					snprintf(szUpdateQuery, sizeof(szUpdateQuery), 
							 "UPDATE dungeon_ranking%s SET `%s` = '%d' WHERE `pid` = '%u' AND `dungeon_index` = '%ld'",
							 get_table_postfix(), c_szRow, iDamage, GetPlayerID(), lMapIndex);
					sys_log(0, "Executing update query for finish_damage: %s", szUpdateQuery);

					std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szUpdateQuery));
					if (!pMsg || !pMsg->Get())
					{
						sys_err("UpdateDungeonRanking: Failed to execute update query for finish_damage: %s", szUpdateQuery);
						return false;
					}
				}
			}
			else
			{
				sys_log(0, "Damage not updated: iDamage (%d) > iLastDamage (%d)", iDamage, iLastDamage);
			}
		}

        ++iRow;

        if (bQueryUpdate)
        {
            sys_log(0, "Executing update query: %s", szUpdateQuery);
            std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szUpdateQuery));
            if (!pMsg || !pMsg->Get())
            {
                sys_err("UpdateDungeonRanking: Failed to execute update query: %s", szUpdateQuery);
                return false;
            }
        }
    }

    return true;
}
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
void CHARACTER::SendCachedMiniMapDungeonInfo()
{
	if(!GetDungeon()) { 
		sys_err("not in dungeon"); 
		return; 
	}

	if (!GetDesc()) {
		sys_err("No descriptor for character");
		return;
	}
	
	const char* szDungeonNotice = GetDungeon()->GetMiniMapDungeonInfoNotice();
	if (!szDungeonNotice) {
		sys_err("No notice for dungeon");
		return;
	}

	if (GetDungeon()->GetMiniMapDungeonInfoStatus() == 1) {
		TPacketGCMiniMapDungeonInfo pack1;
		pack1.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
		pack1.bSubHeader = MINIMAP_DUNGEONINFO_SUBHEADER_STATUS;
		pack1.dValue1 = GetDungeon()->GetMiniMapDungeonInfoStatus();
		pack1.dValue2 = 0;
		pack1.dValue3 = 0;
		strcpy(pack1.szNotice, "");

		GetDesc()->Packet(&pack1, sizeof(pack1));

		TPacketGCMiniMapDungeonInfo pack2;
		pack2.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
		pack2.bSubHeader = MINIMAP_DUNGEONINFO_SUBHEADER_STAGE;
		pack2.dValue1 = GetDungeon()->GetMiniMapDungeonInfoStage(0);
		pack2.dValue2 = GetDungeon()->GetMiniMapDungeonInfoStage(1);
		pack2.dValue3 = 0;
		strcpy(pack2.szNotice, "");

		GetDesc()->Packet(&pack2, sizeof(pack2));

		TPacketGCMiniMapDungeonInfo pack3;
		pack3.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
		pack3.bSubHeader = MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE;
		pack3.dValue1 = GetDungeon()->GetMiniMapDungeonInfoGauge(0);
		pack3.dValue2 = GetDungeon()->GetMiniMapDungeonInfoGauge(1);
		pack3.dValue3 = GetDungeon()->GetMiniMapDungeonInfoGauge(2);
		strcpy(pack3.szNotice, "");

		GetDesc()->Packet(&pack3, sizeof(pack3));

		TPacketGCMiniMapDungeonInfo pack4;
		pack4.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
		pack4.bSubHeader = MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE;
		pack4.dValue1 = 0;
		pack4.dValue2 = 0;
		pack4.dValue3 = 0;
		strcpy(pack4.szNotice, szDungeonNotice);

		GetDesc()->Packet(&pack4, sizeof(pack4));

		TPacketGCMiniMapDungeonInfo pack6;
		pack6.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
		pack6.bSubHeader = MINIMAP_DUNGEONINFO_SUBHEADER_TIMER;
		pack6.timerstatus = GetDungeon()->GetMiniMapDungeonInfoTimer();
		pack6.time = GetDungeon()->GetMiniMapDungeonInfoTime();
		GetDesc()->Packet(&pack6, sizeof(pack6));
	}
}

void CHARACTER::SendMiniMapDungeonInfo(BYTE bSubHeader)
{
	if(!GetDungeon()) { sys_err("not in dungeon"); return; }

	if (!GetDesc()) {
        sys_err("No descriptor for character");
        return;
    }
	
	switch (bSubHeader)
	{
		case MINIMAP_DUNGEONINFO_SUBHEADER_STATUS:
			TPacketGCMiniMapDungeonInfo pack1;
			pack1.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack1.bSubHeader = bSubHeader;
			pack1.dValue1 = GetDungeon()->GetMiniMapDungeonInfoStatus();
			pack1.dValue2 = 0;
			pack1.dValue3 = 0;
			strcpy(pack1.szNotice, "");

			GetDesc()->Packet(&pack1, sizeof(pack1));
			break;

		case MINIMAP_DUNGEONINFO_SUBHEADER_STAGE:
			TPacketGCMiniMapDungeonInfo pack2;
			pack2.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack2.bSubHeader = bSubHeader;
			pack2.dValue1 = GetDungeon()->GetMiniMapDungeonInfoStage(0);
			pack2.dValue2 = GetDungeon()->GetMiniMapDungeonInfoStage(1);
			pack2.dValue3 = 0;
			strcpy(pack2.szNotice, "");

			GetDesc()->Packet(&pack2, sizeof(pack2));
			break;
		
		case MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE:
			TPacketGCMiniMapDungeonInfo pack3;
			pack3.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack3.bSubHeader = bSubHeader;
			pack3.dValue1 = GetDungeon()->GetMiniMapDungeonInfoGauge(0);
			pack3.dValue2 = GetDungeon()->GetMiniMapDungeonInfoGauge(1);
			pack3.dValue3 = GetDungeon()->GetMiniMapDungeonInfoGauge(2);
			strcpy(pack3.szNotice, "");

			GetDesc()->Packet(&pack3, sizeof(pack3));
			
			break;

		case MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE:
			TPacketGCMiniMapDungeonInfo pack4;
			pack4.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack4.bSubHeader = bSubHeader;
			pack4.dValue1 = 0;
			pack4.dValue2 = 0;
			pack4.dValue3 = 0;
			strcpy(pack4.szNotice, GetDungeon()->GetMiniMapDungeonInfoNotice());

			GetDesc()->Packet(&pack4, sizeof(pack4));
			break;
			
		case MINIMAP_DUNGEONINFO_SUBHEADER_BUTTON:
			TPacketGCMiniMapDungeonInfo pack5;
			pack5.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack5.bSubHeader = bSubHeader;
			pack5.buttonvalue = GetDungeon()->GetMiniMapDungeonInfoButton();
			GetDesc()->Packet(&pack5, sizeof(pack5));
			break;

		case MINIMAP_DUNGEONINFO_SUBHEADER_TIMER:
			TPacketGCMiniMapDungeonInfo pack6;
			pack6.bHeader = HEADER_GC_MINIMAP_DUNGEONINFO;
			pack6.bSubHeader = bSubHeader;
			pack6.timerstatus = GetDungeon()->GetMiniMapDungeonInfoTimer();
			pack6.time = GetDungeon()->GetMiniMapDungeonInfoTime();
			GetDesc()->Packet(&pack6, sizeof(pack6));
			break;

		default:
			sys_err("SendMiniMapDungeonInfo: %s unknown subheader %d", bSubHeader);
			return;
	}
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
void CHARACTER::SendCollectPacket(const BYTE windowType, const DWORD time, const DWORD count, const DWORD itemVnum, const DWORD countTotal, const BYTE chance, const DWORD renderTargetID, const DWORD questindex, const DWORD reqLevel)
{
	TPacketGCCollectWindow pack;
	memset(&pack, 0, sizeof(pack));
	
	pack.bHeader = HEADER_GC_COLLECT;
	pack.bWindowType = windowType;
	pack.dwTime = time;
	pack.bCount = count;
	pack.dwItemVnum = itemVnum;
	pack.bCountTotal = countTotal;
	pack.bChance = chance;
	pack.bRenderTargetID = renderTargetID;
	pack.bQuestIndex = questindex;
	pack.RequiredLevel = reqLevel;

	if (GetDesc())
		GetDesc()->Packet(&pack, sizeof(pack));
}
#endif

#ifdef ENABLE_PROMO_CODE_SYSTEM
bool CHARACTER::check_PromoCodeSystem(const char * promo_code)
{
	char szQuery[512];
	
	snprintf(szQuery, sizeof(szQuery), "SELECT promo_code FROM srv1_player.promo_code WHERE promo_code = '%s'", promo_code);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows == 0)
		return false;
	else
		return true;
}

bool CHARACTER::restriction_PromoCodeSystem(const char * promo_code)
{
	char szQuery[512];
	
	snprintf(szQuery, sizeof(szQuery), "SELECT promo_code FROM srv1_player.promo_code_restriction WHERE account_id = '%d' and promo_code = '%s'", GetDesc()->GetAccountTable().id, promo_code);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	
	if (pMsg->Get()->uiNumRows == 0)
		return true;
	else
		return false;
}

void CHARACTER::reward_PromoCodeSystem(const char * promo_code)
{
	char szQuery[1024];
	
	snprintf(szQuery, sizeof(szQuery), "SELECT promo_code_count, reward_type, reward_vnum, reward_count, reward_min_level, reward_max_level, status FROM srv1_player.promo_code WHERE promo_code = '%s'", promo_code);
	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));
	
	if (pmsg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
		std::string reward_type = row[1];
		
		if (atoi(row[0]) == 0 || atoi(row[6]) == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, "Podany przez Ciebie kod promocyjny jest juz nieaktywny!");
			return;
		}
		
		if (atoi(row[4]) > GetLevel())
		{
			ChatPacket(CHAT_TYPE_INFO, "Podany przez Ciebie kod promocyjny mozesz wykorzystac od %d poziomu postaci.", atoi(row[4]));
			return;
		}
		
		if (atoi(row[5]) != 0 && atoi(row[5]) < GetLevel())
		{
			ChatPacket(CHAT_TYPE_INFO, "Podany przez Ciebie kod promocyjny mozesz wykorzystac do %d poziomu postaci.", atoi(row[5]));
			return;
		}
		
		give_PromoCodeSystem(reward_type, atoi(row[2]), atoi(row[3]));
		reduce_PromoCodeSystem(promo_code, atoi(row[0]));
		
#ifdef ENABLE_PROMO_CODE_ONE_USE_PER_ACCOUNT
		lock_PromoCodeSystem(promo_code);
#endif
	}
}

void CHARACTER::lock_PromoCodeSystem(const char * promo_code)
{
	char szQuery[256];
	
	snprintf(szQuery, sizeof(szQuery), "INSERT INTO srv1_player.promo_code_restriction SET account_id = '%d', promo_code = '%s'", GetDesc()->GetAccountTable().id, promo_code);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
}

void CHARACTER::reduce_PromoCodeSystem(const char * promo_code, int iCount)
{
	if (iCount == 1)
		DBManager::instance().DirectQuery("UPDATE srv1_player.promo_code SET promo_code_count = 0, status = 0 WHERE promo_code = '%s'", promo_code);
	else
		DBManager::instance().DirectQuery("UPDATE srv1_player.promo_code SET promo_code_count = promo_code_count - 1 WHERE promo_code = '%s'", promo_code);
}

void CHARACTER::give_PromoCodeSystem(const std::string& sType, int iVnum, int iCount)
{
	if (!sType.compare("item"))
	{
		LPITEM itemReward = ITEM_MANAGER::instance().CreateItem(iVnum, iCount, 0, true);
		
		if (itemReward)
		{
			int iEmptyPos = GetEmptyInventory(itemReward->GetSize());
			
			if (iEmptyPos != -1)
				itemReward->AddToCharacter(this, TItemPos(INVENTORY, iEmptyPos));
			else
			{
				PIXEL_POSITION pos;
				
				if (!SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos))
					return;
				
				pos.x = number(-10, 10) * 20;
				pos.y = number(-10, 10) * 20;
				
				pos.x += GetX();
				pos.y += GetY();
				
				itemReward->AddToGround(GetMapIndex(), pos);
				itemReward->StartDestroyEvent();
			}
			
			ChatPacket(CHAT_TYPE_INFO, "Kod promocyjny skrywal w sobie: %s w ilosci %d szt.", itemReward->GetName(), iCount);
		}
	}
	
	else if (!sType.compare("gold"))
	{
		const int64_t TotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(iCount);
		
		if (TotalMoney >= GOLD_MAX)
		{
			ChatPacket(CHAT_TYPE_INFO, "Nie mozesz uzyc tego kodu promocyjnego bo mialbys za duzo yang!");
			return;
		}
		
		PointChange(POINT_GOLD, iCount, true);
		ChatPacket(CHAT_TYPE_INFO, "Kod promocyjny skrywal w sobie: %d yang!", iCount);
	}
	
	else if (!sType.compare("coins"))
	{
		DBManager::instance().DirectQuery("UPDATE srv1_account.account SET cash=cash + %d, coins=coins + %d WHERE id = %d", iCount, iCount, GetDesc()->GetAccountTable().id);
		ChatPacket(CHAT_TYPE_INFO, "Kod promocyjny skrywal w sobie: %d smoczych monet!", iCount);
	}
	
	else if (!sType.compare("level"))
	{
		int idx = 0;
		
		while (idx < iCount)
		{
			PointChange(POINT_EXP, GetNextExp());
			idx++;
		}
		
		ChatPacket(CHAT_TYPE_INFO, "Kod promocyjny skrywal w sobie: %d dodatkowych poziomow!", iCount);
	}
}
#endif

void CHARACTER::CheckAntyExp()
{
	if (GetLevel() < 30)
	{
		SetToggleAntyExp(false);
		return;
	}

	if (!GetDesc())
	{
		return;
	}

	TPacketGCAntyExp pack;
	pack.bHeader = HEADER_GC_ANTYEXP;

	if (GetQuestFlag("antyexp.status") == 1)
	{
		pack.status = 1;
		SetToggleAntyExp(true);
		ChatPacket(CHAT_TYPE_INFO, "Tej opcji mo?sz u??od 30 poziomu postaci!");
	}
	else {
		SetToggleAntyExp(false);
		pack.status = 0;
	}

	GetDesc()->Packet(&pack, sizeof(TPacketGCAntyExp));
}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// DWORD CHARACTER::MountGetNextExp() const
// {
// 	if (IsMount()) {
// 		if (120 < GetLevel())
// 			return 2500000000;
// 		else
// 			return exp_table[GetLevel()];
// 	}
// }

// DWORD CHARACTER::MountGetNextExpByLevel(int level) const
// {
// 	if (120 < level)
// 		return 2500000000;
// 	else
// 		return exp_table[level];
// }

// void CHARACTER::SendMountLevelUpEffect(int vid, int type, int value, int amount) {
// 	struct packet_point_change pack;

// 	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
// 	pack.dwVID = vid;
// 	pack.type = type;
// 	pack.value = value;
// 	pack.amount = amount;
// 	PacketAround(&pack, sizeof(pack));
// }

void CHARACTER::MountSummon(LPITEM mountItem)
{
	if (IsPolymorphed() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, "Nie mo?sz u??b?? przemienionym.");
		return; 
	}

	if (GetMapIndex() == 113)
		return;
	
	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		return;

	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
				
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
	
	if (IsHorseRiding())
		StopRiding();
	
	if (GetHorse())
		HorseSummon(false);
	
	mountSystem->Summon(mobVnum, mountItem, false);
}

void CHARACTER::MountUnsummon(LPITEM mountItem)
{
	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
	
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
	
	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	// mountSystem->ClearInfo();
	mountSystem->Unsummon(mobVnum);
}

void CHARACTER::MountUnmount(LPITEM mountItem)
{
	CMountSystem* mountSystem = GetMountSystem();
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
	
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
	
	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);
}

// void CHARACTER::MountSetExp(int count)
// {
// 	CMountSystem* mountSystem = GetMountSystem();
// 	if (mountSystem)
// 	{
// 		mountSystem->SetExp(count);
// 		return;
// 	}
// }

void CHARACTER::CheckMount()
{
	CMountSystem* mountSystem = GetMountSystem();
	LPITEM mountItem = GetWear(WEAR_COSTUME_MOUNT);
	DWORD mobVnum = 0;
	
	if (!mountSystem || !mountItem)
		return;
	
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);

	// mountSystem->SaveSQL();
	
	if(mountSystem->CountSummoned() == 0)
	{
		mountSystem->Summon(mobVnum, mountItem, false);
	}
	// if(mountSystem->CountSummoned() != 0)
	// {
	// 		// mountSystem->Summon(mobVnum, mountItem, false);
	// 	mountSystem->Mount(mobVnum, mountItem);
	// }
}

// int CHARACTER::GetMountLevel()
// {
// 	CMountSystem* mountSystem = GetMountSystem();
// 	if (mountSystem)
// 	{
// 		return mountSystem->GetLevel();
// 	}
// 	return -1;
// }

bool CHARACTER::IsRidingMount()
{
	return (GetWear(WEAR_COSTUME_MOUNT) && FindAffect(AFFECT_MOUNT));
	// return FindAffect(AFFECT_MOUNT);
}
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
void CHARACTER::SendLogs()
{
	TPacketGCOfflineShopLogs packet;
	packet.bHeader = HEADER_GC_OFFLINE_SHOP_LOG;
	
	char szQuery[QUERY_MAX_LEN];
	
	//snprintf(szQuery, sizeof(szQuery), "SELECT action, item FROM srv1_log.offlineshop_log WHERE pid = %d DESC LIMIT 10", ch->GetPlayerID());
	int i = 0;
	// snprintf(szQuery, sizeof(szQuery), "(SELECT item, count, price_gold, price_cheque, time, action FROM srv1_log.offlineshop_log WHERE pid = %u AND action = 'SELL' LIMIT 10) UNION ALL (SELECT item, count, price_gold, price_cheque, time, action FROM srv1_log.offlineshop_log WHERE pid = %u AND action = 'BUY' LIMIT 10) ORDER BY time DESC", ch->GetPlayerID(), ch->GetPlayerID());
	snprintf(szQuery, sizeof(szQuery), "SELECT item, count, price_gold, price_cheque, time, action FROM srv1_log.offlineshop_log WHERE pid = %u ORDER BY time DESC LIMIT 20", GetPlayerID());
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));

	if (pMsg->uiSQLErrno)
		return;	

	auto uiNumRows = pMsg->Get()->uiNumRows;

	if (pMsg->Get()->uiNumRows != 0)
	{
		SQLResult *pRes = pMsg->Get();
		if (pRes->uiNumRows)
	 	{
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(pRes->pSQLResult)))
			{
				// memset(&packet.log, 0, sizeof(ShopLog)*10);
				str_to_number(packet.log[i].itemVnum, row[0]);
				str_to_number(packet.log[i].itemCount, row[1]);
				str_to_number(packet.log[i].price, row[2]);
				str_to_number(packet.log[i].price2, row[3]);
				strlcpy(packet.log[i].date, row[4], sizeof(packet.log[i].date));
				strlcpy(packet.log[i].action, row[5], sizeof(packet.log[i].action));
				++i;
			}
		}
	}

	if (uiNumRows < 20)
	{
		while (i < 20)
		{
			packet.log[i].itemVnum = 0;
			packet.log[i].itemCount = 0;
			packet.log[i].price = 0;
			packet.log[i].price2 = 0;
			strlcpy(packet.log[i].date, "0", sizeof(packet.log[i].date));
			strlcpy(packet.log[i].action, "0", sizeof(packet.log[i].action));
			i++;
		}
	}

	GetDesc()->Packet(&packet, sizeof(TPacketGCOfflineShopLogs));
}
#endif

bool CHARACTER::CanDoAction()
{
	if (!IsPC())
		return false;

	if (GetShopOwner() || GetShop() || !CanHandleItem() || !CanWarp() || GetExchange())
		return false;
	
	return true;
}

#if defined(BL_OFFLINE_MESSAGE)
void CHARACTER::SendOfflineMessage(const char* To, const char* Message)
{
	if (!GetDesc())
		return;

	if (strlen(To) < 1)
		return;

	TPacketGDSendOfflineMessage p;
	strlcpy(p.szFrom, GetName(), sizeof(p.szFrom));
	strlcpy(p.szTo, To, sizeof(p.szTo));
	strlcpy(p.szMessage, Message, sizeof(p.szMessage));
	db_clientdesc->DBPacket(HEADER_GD_SEND_OFFLINE_MESSAGE, GetDesc()->GetHandle(), &p, sizeof(p));

	SetLastOfflinePMTime();
}

void CHARACTER::ReadOfflineMessages()
{
	if (!GetDesc())
		return;

	TPacketGDReadOfflineMessage p;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	db_clientdesc->DBPacket(HEADER_GD_REQUEST_OFFLINE_MESSAGES, GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#ifdef ENABLE_HWID_SYSTEM
bool CHARACTER::IsHwidBanned() const
{
	if (!IsPC())
		return false;

	MYSQL_ROW row;
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT COUNT(*) as count FROM srv1_account.banned_list WHERE name='%s';", GetName()));
		if (pMsg->Get()->uiNumRows == 0)
			return false;

		row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		if (*row[0] != '0')
			return true;
	}

	return false;
}

const char* CHARACTER::GetHwid(BYTE bComponent)
{
	if (!IsPC())
		return "";

	if (bComponent < 0)
		return "";

	char querySelect[256];
	snprintf(querySelect, sizeof(querySelect), "SELECT cpu_id, hdd_model, machine_guid, mac_addr, hdd_serial FROM srv1_account.account WHERE id = '%u'", GetDesc()->GetAccountTable().id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(querySelect));

	if (pMsg->Get()->uiNumRows == 0)
		return "";

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	std::string c_stHwid = row[bComponent];
	return c_stHwid.c_str();
}
#endif

#ifdef ENABLE_REAL_TIME_REGEN
LPREGEN CHARACTER::GetRegen()
{
	return m_pkRegen;
}
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
bool CHARACTER::BuildPrivateShop(const char* c_szTitle, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount, WORD wItemCount, TPrivateShopItem* pShopItemTable)
{
	if (!CanHandleItem())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a personal shop now."));
		return false;
	}

#ifdef ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	if (!CountSpecifyItem(50200) && !CountSpecifyItem(71221))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a personal shop without a Bundle."));
		return false;
	}

	if ((dwPolyVnum > 30000 || bTitleType > 0) && !CountSpecifyItem(71221))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Kashmir Bundle to decorate your personal shop."));
		return false;
	}
#endif

#if defined(WJ_COMBAT_ZONE)	
	if (CCombatZoneManager::Instance().IsCombatZoneMap(GetMapIndex()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cz_cannot_open_shop"));
		return false;
	}
#endif

	if (IsPrivateShopOwner())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already opened your personal shop."));
		return false;
	}

	if (GetDungeon())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a personal shop in a dungeon."));
		return false;
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());
	if (pPC->IsRunning())
		return false;

	if (wItemCount == 0)
	{
		sys_err("Item count is equal to zero for player %d", GetPlayerID());
		return false;
	}

	if (CBanwordManager::instance().CheckString(c_szTitle, strlen(c_szTitle)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please choose another title name for your personal shop."));
		return false;
	}

	if (strlen(c_szTitle) < TITLE_MIN_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The entered name is too short."));
		return false;
	}

	std::vector<TPrivateShopItem>	s_vec_shopItem;		// Container of items to be added into the shop
	std::set<TItemPos> s_set_item;						// Container for checking duplicated items

	for (BYTE i = 0; i < wItemCount; ++i, ++pShopItemTable)
	{
		if (s_set_item.find(pShopItemTable->TPos) != s_set_item.end())
		{
			sys_err("Duplicated item in private shop detected! (name: %s)", GetName());
			return false;
		}

		const LPITEM pItem = GetItem(pShopItemTable->TPos);
		if (!pItem)
		{
			sys_err("Could not find an item in position: %d", pShopItemTable->TPos.cell);
			return false;
		}

		if (pItem->GetVnum() == 50200 || pItem->GetVnum() == 71221)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot put Bundles items in a personal shop when building it."));
			return false;
		}

		const TItemTable* pItemTable = pItem->GetProto();
		if (!pItemTable)
		{
			sys_err("Could not find an item table for an item at position: %d vnum: %d", pShopItemTable->TPos.cell, pItem->GetVnum());
			return false;
		}

		if (pItemTable && (IS_SET(pItemTable->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell Item-Shop items in a personal shop."));
			return false;
		}

		if (pItem->IsEquipped())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell equipped items in a personal shop."));
			return false;
		}

		if (pItem->isLocked())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell locked items in a personal shop."));
			return false;
		}

		if (pShopItemTable->TPrice.llGold < 0)
		{
			sys_err("Player %u is trying to build private shop with negative prices", GetPlayerID());
			return false;
		}

		s_set_item.insert(pShopItemTable->TPos);

		s_vec_shopItem.push_back(*pShopItemTable);
	}

	// Set shop poly vnum to default value in case its invalid
	// When building a normal shop, poly vnum is equal to 0
	if (dwPolyVnum < 30000 || dwPolyVnum > 30008)
		dwPolyVnum = 30000;

	// Spawn the entity
	if (CPrivateShopManager::Instance().BuildPrivateShop(this, c_szTitle, dwPolyVnum, bTitleType, bPageCount, s_vec_shopItem))
	{
#ifdef ENABLE_PRIVATE_SHOP_BUNDLE_REQ
		{
			if (dwPolyVnum != 30000 || bTitleType != 0 || bPageCount == PRIVATE_SHOP_PAGE_MAX_NUM)
				RemoveSpecifyItem(71221);
			else
				RemoveSpecifyItem(50200);
		}
#endif
	}
	return true;
}

void CHARACTER::ClosePrivateShop()
{
	// Clear private shop data
	memset(&m_privateShopTable, 0, sizeof(m_privateShopTable));
	m_vec_privateShopItem.clear();

	ClosePrivateShopPanel();

	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_CLOSE;

		GetDesc()->Packet(&mainPacket, sizeof(mainPacket));
	}

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your personal shop has been closed."));
}

void CHARACTER::SetPrivateShopTable(const TPrivateShop& rPrivateShopTable)
{
	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateShopLoad);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_LOAD;

		TPacketGCPrivateShopLoad subPacket{};
		strlcpy(subPacket.szTitle, rPrivateShopTable.szTitle, sizeof(subPacket.szTitle));
		subPacket.llGold = rPrivateShopTable.llGold;
		subPacket.dwCheque = rPrivateShopTable.dwCheque;
		subPacket.lX = rPrivateShopTable.lX;
		subPacket.lY = rPrivateShopTable.lY;
		subPacket.bChannel = rPrivateShopTable.bChannel;
		subPacket.bState = rPrivateShopTable.bState;
		subPacket.bPageCount = rPrivateShopTable.bPageCount;

		GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
		GetDesc()->Packet(&subPacket, sizeof(TPacketGCPrivateShopLoad));
	}

	m_privateShopTable = rPrivateShopTable;
}

void CHARACTER::OpenPrivateShopPanel()
{
	if (GetViewingPrivateShop())
		GetViewingPrivateShop()->RemoveShopViewer(this);

	SetEditingPrivateShop(true);

	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL;

		GetDesc()->Packet(&mainPacket, sizeof(mainPacket));
	}
}

void CHARACTER::ClosePrivateShopPanel(bool bSendClient /* = false */)
{
	if (!IsEditingPrivateShop())
		return;

	SetEditingPrivateShop(false);
	SetMyShopTime();

	if (bSendClient)
	{
		if (GetDesc())
		{
			TPacketGCPrivateShop mainPacket{};
			mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
			mainPacket.wSize = sizeof(TPacketGCPrivateShop);
			mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL;

			GetDesc()->Packet(&mainPacket, sizeof(mainPacket));
		}
	}
}

long long CHARACTER::GetPrivateShopTotalGold()
{
	long long llGold = GetPrivateShopTable()->llGold;

	for (const auto& rShopItem : m_vec_privateShopItem)
		llGold += rShopItem.TPrice.llGold;

	return llGold;
}

DWORD CHARACTER::GetPrivateShopTotalCheque()
{
	long long dwCheque = GetPrivateShopTable()->dwCheque;

	for (const auto& rShopItem : m_vec_privateShopItem)
		dwCheque += rShopItem.TPrice.dwCheque;

	return dwCheque;
}

void CHARACTER::SetPrivateShopItem(const TPlayerPrivateShopItem& c_rPrivateShopItem)
{
	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPrivateShopItemData);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM;

		TPrivateShopItemData subPacket{};
		CopyItemData(c_rPrivateShopItem, subPacket);

		GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
		GetDesc()->Packet(&subPacket, sizeof(TPrivateShopItemData));
	}

	m_vec_privateShopItem.push_back(c_rPrivateShopItem);
}

bool CHARACTER::RemovePrivateShopItem(WORD wPos)
{
	for (auto it = m_vec_privateShopItem.begin(); it != m_vec_privateShopItem.end(); ++it)
	{
		const TPlayerPrivateShopItem& r_shopItem = *it;
		if (r_shopItem.wPos == wPos)
		{
			TPacketGCPrivateShop mainPacket{};
			mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
			mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(WORD);
			mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM;

			GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
			GetDesc()->Packet(&wPos, sizeof(WORD));

			m_vec_privateShopItem.erase(it);
			return true;
		}
	}

	return false;
}

const TPlayerPrivateShopItem* CHARACTER::GetPrivateShopItem(WORD wPos)
{
	for (auto it = m_vec_privateShopItem.begin(); it != m_vec_privateShopItem.end(); ++it)
	{
		const TPlayerPrivateShopItem& rShopItem = *it;
		if (rShopItem.wPos == wPos)
			return &rShopItem;
	}
	return nullptr;
}

void CHARACTER::SetPrivateShopSale(const TPrivateShopSale& c_rPrivateShopSale)
{
	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPrivateShopSaleData);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_SET_SALE;

		TPrivateShopSaleData subPacket{};
		strlcpy(subPacket.szCustomer, c_rPrivateShopSale.szCustomerName, sizeof(subPacket.szCustomer));
		subPacket.tTime = c_rPrivateShopSale.tTime;

		CopyItemData(c_rPrivateShopSale, subPacket.TItem);

		GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
		GetDesc()->Packet(&subPacket, sizeof(TPrivateShopSaleData));
	}

	m_vec_privateShopSale.push_back(c_rPrivateShopSale);
}

void CHARACTER::ChangePrivateShopItemPrice(WORD wPos, long long llGold, DWORD dwCheque)
{
	for (auto it = m_vec_privateShopItem.begin(); it != m_vec_privateShopItem.end(); ++it)
	{
		TPlayerPrivateShopItem& r_shopItem = *it;
		if (r_shopItem.wPos == wPos)
		{
			// Update prices of the item
			r_shopItem.TPrice.llGold = llGold;
			r_shopItem.TPrice.dwCheque = dwCheque;

			// Send information to client
			TPacketGCPrivateShop mainPacket{};
			mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
			mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateShopItemPriceChange);
			mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE;

			TPacketGCPrivateShopItemPriceChange subPacket{};
			subPacket.wPos = wPos;
			subPacket.TPrice.llGold = llGold;
			subPacket.TPrice.dwCheque = dwCheque;

			GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
			GetDesc()->Packet(&subPacket, sizeof(TPacketGCPrivateShopItemPriceChange));

			break;
		}
	}
}

void CHARACTER::ChangePrivateShopItemPos(WORD wPos, WORD wChangePos)
{
	for (auto it = m_vec_privateShopItem.begin(); it != m_vec_privateShopItem.end(); ++it)
	{
		TPlayerPrivateShopItem& r_shopItem = *it;
		if (r_shopItem.wPos == wPos)
		{
			// Update position of the item
			r_shopItem.wPos = wChangePos;

			// Send information to client
			TPacketGCPrivateShop mainPacket{};
			mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
			mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateShopItemMove);
			mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE;

			TPacketGCPrivateShopItemMove subPacket{};
			subPacket.wPos = wPos;
			subPacket.wChangePos = wChangePos;

			GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
			GetDesc()->Packet(&subPacket, sizeof(TPacketGCPrivateShopItemMove));

			break;
		}
	}
}

void CHARACTER::ChangePrivateShopTitle(const char* c_szTitle)
{
	strlcpy(m_privateShopTable.szTitle, c_szTitle, sizeof(m_privateShopTable.szTitle));

	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(m_privateShopTable.szTitle);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE;

		GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
		GetDesc()->Packet(&m_privateShopTable.szTitle, sizeof(m_privateShopTable.szTitle));
	}
}

EVENTINFO(private_shop_warp_info)
{
	LPCHARACTER		m_pChar;
	long			m_lAddr;
	WORD			m_wPort;
	BYTE			m_bLeftSeconds;
};

EVENTFUNC(private_shop_warp_event)
{
	private_shop_warp_info* info = dynamic_cast<private_shop_warp_info*>(event->info);

	if (!info)
	{
		sys_err("<private_shop_warp_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER pChar = info->m_pChar;
	if (!pChar)
		return 0;

	if (info->m_bLeftSeconds <= 0)
	{
		pChar->m_pkTimedEvent = nullptr;

		TPrivateShop* pTable = pChar->GetPrivateShopTable();

		if (pChar->CanWarp())
			pChar->WarpSet(pTable->lX, pTable->lY, 0, info->m_lAddr, info->m_wPort);
		else
			pChar->ChatPacket(CHAT_TYPE_INFO, "Teleport to the private shop has been canceled.");

		return 0;
	}
	else
	{
		pChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%u seconds until teleport."), info->m_bLeftSeconds);
		--info->m_bLeftSeconds;
	}

	return PASSES_PER_SEC(1);
}

void CHARACTER::WarpToPrivateShop(long lAddr, WORD wPort)
{
	if (!IsPrivateShopOwner())
		return;

	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been canceled."));
		event_cancel(&m_pkTimedEvent);
		return;
	}

	ClosePrivateShopPanel(true);

	// Workaround the CanWarp check since we just closed the UI
	m_iMyShopTime = 0;

	if (IsHack(false, true, 10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can not teleport to your private shop right now."));
		return;
	}

	private_shop_warp_info* info = AllocEventInfo<private_shop_warp_info>();
	{
		info->m_pChar = this;
		info->m_lAddr = lAddr;
		info->m_wPort = wPort;

		if (IsPosition(POS_FIGHTING))
			info->m_bLeftSeconds = 10;
		else
			info->m_bLeftSeconds = 3;
	}

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are teleporting to your private shop. Please wait."));
	m_pkTimedEvent = event_create(private_shop_warp_event, info, 1);
}

void CHARACTER::SaleUpdate(const TPrivateShopSale* c_pShopSale)
{
	const TPlayerPrivateShopItem* pShopItem = GetPrivateShopItem(c_pShopSale->TItem.wPos);
	if (pShopItem)
	{
		// Update the stash values at table
		GetPrivateShopTable()->llGold += c_pShopSale->TItem.TPrice.llGold;
		GetPrivateShopTable()->dwCheque += c_pShopSale->TItem.TPrice.dwCheque;

		// Remove the bought item
		RemovePrivateShopItem(c_pShopSale->TItem.wPos);

		// Add sale to the character's list although its not really necessary as it won't be used
		SetPrivateShopSale(*c_pShopSale);

		// Send the balance update
		if (GetDesc())
		{
			// Send a notification to the player
			TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(c_pShopSale->TItem.dwVnum);
			if (pItemTable)
			{
				char szMsg[256 + 1]{};

				int len = 0;
#ifdef ENABLE_CHEQUE_SYSTEM
				{
					len = snprintf(szMsg, sizeof(szMsg), "Vendeste x%d %s por %s Yangs e %s Wons.",
						c_pShopSale->TItem.dwCount, pItemTable->szLocaleName,
						format_number(c_pShopSale->TItem.TPrice.llGold).c_str(), format_number(c_pShopSale->TItem.TPrice.dwCheque).c_str());
				}
#else
				{
					len = snprintf(szMsg, sizeof(szMsg), "Vendeste x%d %s por %s Yangs.",
						c_pShopSale->TItem.dwCount, pItemTable->szLocaleName, format_number(c_pShopSale->TItem.TPrice.llGold).c_str());
				}
#endif
				TPacketGCWhisper packet{};

				packet.bHeader = HEADER_GC_WHISPER;
				packet.wSize = sizeof(TPacketGCWhisper) + len;
				packet.bType = EWhisperType::WHISPER_TYPE_SYSTEM;
				strlcpy(packet.szNameFrom, "Loja Privada", sizeof(packet.szNameFrom));

				TEMP_BUFFER buf;

				buf.write(&packet, sizeof(packet));
				buf.write(szMsg, len);

				GetDesc()->Packet(buf.read_peek(), buf.size());
			}

			TPacketGCPrivateShop mainPacket{};
			mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
			mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateShopBalanceUpdate);
			mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE;

			TPacketGCPrivateShopBalanceUpdate subPacket{};
			subPacket.TPrice = c_pShopSale->TItem.TPrice;

			GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
			GetDesc()->Packet(&subPacket, sizeof(TPacketGCPrivateShopBalanceUpdate));
		}
	}
}

void CHARACTER::ItemExpireUpdate(WORD wPos)
{
	const TPlayerPrivateShopItem* pShopItem = GetPrivateShopItem(wPos);

	if (!pShopItem)
	{
		sys_err("Could not find item on pos %u for player %u", wPos, GetPlayerID());
		return;
	}

	// Send a notification to the player
	TItemTable* pItemTable = ITEM_MANAGER::Instance().GetTable(pShopItem->dwVnum);
	if (pItemTable)
	{
		char szMsg[128 + 1]{};

		int len = snprintf(szMsg, sizeof(szMsg), LC_TEXT("%s foi removido da loja pois o tempo do mesmo expirou."),
			pItemTable->szLocaleName);

		TPacketGCWhisper packet{};

		packet.bHeader = HEADER_GC_WHISPER;
		packet.wSize = sizeof(TPacketGCWhisper) + len;
		packet.bType = EWhisperType::WHISPER_TYPE_SYSTEM;
		strlcpy(packet.szNameFrom, "Private Shop", sizeof(packet.szNameFrom));

		TEMP_BUFFER buf;

		buf.write(&packet, sizeof(packet));
		buf.write(szMsg, len);

		GetDesc()->Packet(buf.read_peek(), buf.size());
	}

	RemovePrivateShopItem(wPos);
}

void CHARACTER::SetPrivateShopState(BYTE bState, bool bIsMainPlayerPrivateShop)
{
	if (bIsMainPlayerPrivateShop)
		GetPrivateShopTable()->bState = bState;

	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop) + sizeof(TPacketGCPrivateStateUpdate);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE;

		TPacketGCPrivateStateUpdate subPacket{};
		subPacket.bState = bState;
		subPacket.bIsMainPlayerPrivateShop = bIsMainPlayerPrivateShop;

		GetDesc()->BufferedPacket(&mainPacket, sizeof(TPacketGCPrivateShop));
		GetDesc()->Packet(&subPacket, sizeof(TPacketGCPrivateStateUpdate));
	}
}

void CHARACTER::WithdrawPrivateShop(long long llGold, DWORD dwCheque)
{
	if (llGold != GetPrivateShopTable()->llGold || dwCheque != GetPrivateShopTable()->dwCheque)
	{
		sys_err("Withdraw values mismatch gold: %lld | %lld cheque: %u | %u for player %u", llGold, GetPrivateShopTable()->llGold, dwCheque, GetPrivateShopTable()->dwCheque, GetPlayerID());
		return;

	}
	long long llCurrentGold = static_cast<long long>(GetGold());

#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD dwCurrentCheque = GetCheque();
#endif

	if ((llCurrentGold + GetPrivateShopTable()->llGold) >= GOLD_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot collect the money, because you would then have more than 2 billion Yang."));
		return;
	}

#ifdef ENABLE_CHEQUE_SYSTEM
	if ((dwCurrentCheque + GetPrivateShopTable()->dwCheque) > CHEQUE_MAX)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot collect the money, because you would then have more than 999 Won."));
		return;
	}
#endif

	PointChange(POINT_GOLD, GetPrivateShopTable()->llGold);

#ifdef ENABLE_CHEQUE_SYSTEM
	PointChange(POINT_CHEQUE, GetPrivateShopTable()->dwCheque);
#endif

	if (GetPrivateShopTable()->llGold && GetPrivateShopTable()->dwCheque <= 0)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have withdrawn %lld Yang from your personal shop earnings."), GetPrivateShopTable()->llGold);
#ifdef ENABLE_CHEQUE_SYSTEM
	else if (GetPrivateShopTable()->dwCheque && GetPrivateShopTable()->llGold <= 0)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have withdrawn %u Won from your personal shop earnings."), GetPrivateShopTable()->dwCheque);
	else if(GetPrivateShopTable()->llGold && GetPrivateShopTable()->dwCheque)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have withdrawn %lld Yang and %u Won from your personal shop earnings."), GetPrivateShopTable()->llGold, GetPrivateShopTable()->dwCheque);
#endif

	sys_log(0, "%s PRIVATE_SHOP: WITHDRAW GOLD %lld CHEQUE %u", GetName(), GetPrivateShopTable()->llGold, GetPrivateShopTable()->dwCheque);

	char szHint[128 + 1] {};
	snprintf(szHint, sizeof(szHint), "Gold %lld Cheque %u", GetPrivateShopTable()->llGold, GetPrivateShopTable()->dwCheque);
	LogManager::Instance().CharLog(this, 0, "PRIVATE SHOP WITHDRAW", szHint);

	GetPrivateShopTable()->llGold = 0;
	GetPrivateShopTable()->dwCheque = 0;

	// Update the value on client-side
	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop);
		mainPacket.bSubHeader = SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW;

		GetDesc()->Packet(&mainPacket, sizeof(TPacketGCPrivateShop));
	}

	// Update the value on db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW;
	DWORD dwPID = GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, 0, sizeof(BYTE) + sizeof(DWORD));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&dwPID, sizeof(DWORD));
}

void CHARACTER::OpenShopSearch(BYTE bMode)
{
	if (!CheckTradeWindows(this))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade while another window is open."));
		return;
	}

	m_bShopSearchMode = bMode;

	if (GetDesc())
	{
		TPacketGCPrivateShop mainPacket{};
		mainPacket.bHeader = HEADER_GC_PRIVATE_SHOP;
		mainPacket.wSize = sizeof(TPacketGCPrivateShop);
		mainPacket.bSubHeader = (bMode == MODE_LOOKING ? SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE : SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE);

		GetDesc()->Packet(&mainPacket, sizeof(mainPacket));
	}
}

void CHARACTER::CloseShopSearch()
{
	m_bShopSearchMode = MODE_NONE;
}

bool CHARACTER::SetPremiumPrivateShopBonus(time_t tDuration, bool bIsAccumulable)
{
	if (!GetDesc())
		return false;

	int PRIVATE_SHOP_MAX_PREMIUM_TIME_VALUE = PRIVATE_SHOP_MAX_PREMIUM_TIME;

	if (bIsAccumulable)
		PRIVATE_SHOP_MAX_PREMIUM_TIME_VALUE = 60 * 60 * 8;

	CAffect* pAffect = FindAffect(AFFECT_PREMIUM_PRIVATE_SHOP);
	if (pAffect)
	{
		tDuration = std::min<time_t>(tDuration + pAffect->lDuration, PRIVATE_SHOP_MAX_PREMIUM_TIME_VALUE);

		// Return if player has reached limit moments ago
		if (PRIVATE_SHOP_MAX_PREMIUM_TIME_VALUE - pAffect->lDuration < 60)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have just recently reached maximum premium time for your personal shop. "));
			return false;
		}
	}

	// Old affect will be removed by AddAffect
	AddAffect(AFFECT_PREMIUM_PRIVATE_SHOP, POINT_NONE, 0, 0, tDuration, 0, true);

	 m_aiPremiumTimes[PREMIUM_PRIVATE_SHOP] = tDuration + time(0);
	 m_dwCurrentPrivateShopTime = tDuration;

	// Update db
	BYTE bSubHeader = PRIVATE_SHOP_GD_SUBHEADER_PREMIUM_TIME_UPDATE;

	TPacketGDPrivateShopPremiumTimeUpdate packet{};
	packet.dwAID = GetDesc()->GetAccountTable().id;
	packet.dwPID = GetPlayerID();
	packet.tPremiumTime = tDuration;

	db_clientdesc->DBPacketHeader(HEADER_GD_PRIVATE_SHOP, 0, sizeof(BYTE) + sizeof(TPacketGDPrivateShopPremiumTimeUpdate));
	db_clientdesc->Packet(&bSubHeader, sizeof(BYTE));
	db_clientdesc->Packet(&packet, sizeof(TPacketGDPrivateShopPremiumTimeUpdate));

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have increased Premium Personal Shop duration."));

	// Warn the player about time limit
	if (tDuration == PRIVATE_SHOP_MAX_PREMIUM_TIME_VALUE)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have reached maximum premium time for your personal shop. "));

	char szHint[128 + 1]{};
	snprintf(szHint, sizeof(szHint), "+%u", tDuration);
	LogManager::Instance().CharLog(this, 0, "PRIVATE SHOP PREMIUM TIME", szHint);

	return true;
}
#endif
