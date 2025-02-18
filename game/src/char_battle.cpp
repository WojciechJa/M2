#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "battle.h"
#include "pvp.h"
#include "skill.h"
#include "start_position.h"
#include "profiler.h"
#include "cmd.h"
#include "dungeon.h"
#include "log.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "db.h"
#include "vector.h"
#include "marriage.h"
#include "arena.h"
#include "regen.h"
#include "monarch.h"
#include "exchange.h"
#include "shop_manager.h"
#include "castle.h"
#include "ani.h"
#include "packet.h"
#include "party.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "questmanager.h"
#include "questlua.h"
#include "threeway_war.h"
#include "BlueDragon.h"
#include "DragonLair.h"
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	#include "offlineshop_manager.h"
#endif
#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#endif
#define ENABLE_EFFECT_PENETRATE
#define ENABLE_NEWEXP_CALCULATION
// #define ENABLE_NO_DAMAGE_QUEST_RUNNING

#include <array>
#include <string>
#include "../../Extern/include/boost/format.hpp"
#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop.h"
#endif

#ifdef ENABLE_ANTY_CHEAT
float CalcDistance(int a, int b)
{
	return sqrt(pow(a, 2) + pow(b, 2));
}

bool IsGoodWeaponType(BYTE weaponType)
{
	switch(weaponType)
	{
		case WEAPON_SWORD:
		case WEAPON_DAGGER:
		case WEAPON_BOW:
		case WEAPON_TWO_HANDED:
		case WEAPON_BELL:
		case WEAPON_FAN:
			return true;
			break;
		default:
			return false;
	}
}

float GetAttackRangeLimit(LPCHARACTER ch)
{
	if (!ch)
		return 0.0;

	LPITEM weapon = ch->GetWear(WEAR_WEAPON);

	if (!weapon)
		return 220.0;

	if (ch->IsRiding())
		return 450.0;
	
	if (ch->IsPolymorphed())
		return 400.0;

	if (!IsGoodWeaponType(weapon->GetSubType()))
		return 0.0;

	float rangeLimit[6] = {	// 6 bo tak
				280.0,		// WEAPON_SWORD,
				220.0,		// WEAPON_DAGGER,
				1000.0,		// WEAPON_BOW,
				430.0,		// WEAPON_TWO_HANDED,
				280.0,		// WEAPON_BELL,
				280.0,		// WEAPON_FAN,
	};
	return rangeLimit[weapon->GetSubType()];
}

int GetAttackLimit(LPCHARACTER ch)
{
    if (NULL == ch)
        return 1000;

	int maxAttackSpeed = 70; // NIE RUSZAC ! ! !
	int curAttackSpeed = MIN(ch->GetPoint(POINT_ATT_SPEED), 170) - 100;
	
	int iMaxLimit[][10][10][10] = // limit dla ataku z minimaln? szybko?ci? ataku
	{
		// BEZ BRONI
		{
			{{1000},},		//wszystkie klasy
		},
		// NORMALNIE
		{
			// WOJOWNIK
			{{380,700},},		//miecz, 2reczna
			// NINJA
			{{380,1340,500},},	//sztylety, luk, miecz
			// SURA
			{{450},},			//miecz
			// SZAMAN
			{{520,520},},		//dzwon, wachlarz
			//LIKAN
			{{440},},//szpony
		},
		// WIERZCHOWIEC
		{
			// WOJOWNIK
			{{440,440},},		//miecz, 2reczna
			// NINJA
			{{480,1350,360},},	//sztylety, luk, miecz
			// SURA
			{{450},},			//miecz
			// SZAMAN
			{{500,720},},		//dzwon, wachlarz
			//LIKAN
			{{440},},//szpony
		},
		// POLI
		{
			{{800},},			//wszystkie klasy
		},
	};	
	int iMinLimit[][10][10][10] = // limit dla ataku z maksymaln? szybko?ci? ataku
	{
		// BEZ BRONI
		{
			{{460},},			//wszystkie klasy
		},
		// NORMALNIE
		{
			// WOJOWNIK
			{{245,245},},		//miecz, 2reczna
			// NINJA
			{{235,560,315},},	//sztylety, luk, miecz
			// SURA
			{{260},},			//miecz
			// SZAMAN
			{{260,260},},		//dzwon, wachlarz
			// LIKAN
			{{235},},//SZPONY
		},
		// WIERZCHOWIEC
		{
			// WOJOWNIK
			{{240,240},},		//miecz, 2reczna
			// NINJA
			{{300,760,238},},	//sztylety, luk, miecz
			// SURA
			{{240},},			//miecz
			// SZAMAN
			{{240,380},},		//dzwon, wachlarz
			// LIKAN
			{{240},},//SZPONY
		},
		// POLI
		{
			{{450},},//wszystkie_klasy
		},
	};

	LPITEM pkWeapon = ch->GetWear(WEAR_WEAPON);
	int character = ch->GetJob(), type_weapon = 0;

	int limitMin = 0, limitMax = 0;

	if (ch->IsPolymorphed())
	{
		// limit_as=tab[3][0][0][0];
		limitMin = iMinLimit[3][0][0][0];
		limitMax = iMaxLimit[3][0][0][0];
	}
	else
	{
		if (pkWeapon!=NULL)
		{
			BYTE item_type = pkWeapon->GetSubType();
			BYTE tab_type[10]={0, 0, 1, 1, 0, 1, 0, 0};
			type_weapon=tab_type[item_type];

			if(character == JOB_ASSASSIN && item_type == WEAPON_SWORD)
				type_weapon=2;

			// ch->IsRiding() ? limit_as=tab[2][character][0][type_weapon] : limit_as=tab[1][character][0][type_weapon];
			ch->IsRiding() ? limitMin = iMinLimit[2][character][0][type_weapon] : limitMin = iMinLimit[1][character][0][type_weapon];
			ch->IsRiding() ? limitMax = iMaxLimit[2][character][0][type_weapon] : limitMax = iMaxLimit[1][character][0][type_weapon];
		}
		else
		{
			// limit_as=tab[0][0][0][0];
			limitMin = iMinLimit[0][0][0][0];
			limitMax = iMaxLimit[0][0][0][0];
		}
	}

	int diference = limitMax - limitMin;
	int realLimit = limitMax - static_cast <int> (diference*((float)curAttackSpeed/maxAttackSpeed));

	if(realLimit <= 0)
		return 1000;

	return realLimit;
}
#endif

static DWORD __GetPartyExpNP(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
		return 14000;
	return party_exp_distribute_table[level];
}

static int __GetExpLossPerc(const DWORD level)
{
	if (!level || level > PLAYER_EXP_TABLE_MAX)
		return 1;
	return aiExpLossPercents[level];
}

DWORD AdjustExpByLevel(const LPCHARACTER ch, const DWORD exp)
{
	if (PLAYER_MAX_LEVEL_CONST < ch->GetLevel())
	{
		double ret = 0.95;
		double factor = 0.1;

		for (ssize_t i=0 ; i < ch->GetLevel()-100 ; ++i)
		{
			if ( (i%10) == 0)
				factor /= 2.0;

			ret *= 1.0 - factor;
		}

		ret = ret * static_cast<double>(exp);

		if (ret < 1.0)
			return 1;

		return static_cast<DWORD>(ret);
	}

	return exp;
}

bool CHARACTER::CanBeginFight() const
{
	if (!CanMove())
		return false;

	return m_pointsInstant.position == POS_STANDING && !IsDead() && !IsStun();
}

void CHARACTER::BeginFight(LPCHARACTER pkVictim)
{
	SetVictim(pkVictim);
	SetPosition(POS_FIGHTING);
	SetNextStatePulse(1);
}

bool CHARACTER::CanFight() const
{
	return m_pointsInstant.position >= POS_FIGHTING ? true : false;
}

void CHARACTER::CreateFly(BYTE bType, LPCHARACTER pkVictim)
{
	TPacketGCCreateFly packFly;

	packFly.bHeader         = HEADER_GC_CREATE_FLY;
	packFly.bType           = bType;
	packFly.dwStartVID      = GetVID();
	packFly.dwEndVID        = pkVictim->GetVID();

	PacketAround(&packFly, sizeof(TPacketGCCreateFly));
}

void CHARACTER::DistributeSP(LPCHARACTER pkKiller, int iMethod)
{
	if (pkKiller->GetSP() >= pkKiller->GetMaxSP())
		return;

	bool bAttacking = (get_dword_time() - GetLastAttackTime()) < 3000;
	bool bMoving = (get_dword_time() - GetLastMoveTime()) < 3000;

	if (iMethod == 1)
	{
		int num = number(0, 3);

		if (!num)
		{
			int iLvDelta = GetLevel() - pkKiller->GetLevel();
			int iAmount = 0;

			if (iLvDelta >= 5)
				iAmount = 10;
			else if (iLvDelta >= 0)
				iAmount = 6;
			else if (iLvDelta >= -3)
				iAmount = 2;

			if (iAmount != 0)
			{
				iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;

				if (iAmount >= 11)
					CreateFly(FLY_SP_BIG, pkKiller);
				else if (iAmount >= 7)
					CreateFly(FLY_SP_MEDIUM, pkKiller);
				else
					CreateFly(FLY_SP_SMALL, pkKiller);

				pkKiller->PointChange(POINT_SP, iAmount);
			}
		}
	}
	else
	{
		if (pkKiller->GetJob() == JOB_SHAMAN || (pkKiller->GetJob() == JOB_SURA && pkKiller->GetSkillGroup() == 2))
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + GetMaxSP() / 100;
			else if (bMoving)
				iAmount = 3 + GetMaxSP() * 2 / 100;
			else
				iAmount = 10 + GetMaxSP() * 3 / 100;

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
		else
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + pkKiller->GetMaxSP() / 200;
			else if (bMoving)
				iAmount = 2 + pkKiller->GetMaxSP() / 100;
			else
			{
				if (pkKiller->GetHP() < pkKiller->GetMaxHP())
					iAmount = 2 + (pkKiller->GetMaxSP() / 100);
				else
					iAmount = 9 + (pkKiller->GetMaxSP() / 100);
			}

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
	}
}

bool CHARACTER::Attack(LPCHARACTER pkVictim, BYTE bType)
{
	if (test_server)
		sys_log(0, "[TEST_SERVER] Attack : %s type %d, MobBattleType %d", GetName(), bType, !GetMobBattleType() ? 0 : GetMobAttackRange());

	if (!CanMove())
		return false;

	// CASTLE
	if (IS_CASTLE_MAP(GetMapIndex()) && false == castle_can_attack(this, pkVictim))
		return false;
	// CASTLE

	// @fixme131
	if (!battle_is_attackable(this, pkVictim))
		return false;

	DWORD dwCurrentTime = get_dword_time();

	if (IsPC())
	{
		if (IS_SPEED_HACK(this, pkVictim, dwCurrentTime))
			return false;

		if (bType == 0 && dwCurrentTime < GetSkipComboAttackByTime())
			return false;
	}
	else
	{
		MonsterChat(MONSTER_CHAT_ATTACK);
	}

	pkVictim->SetSyncOwner(this);

	if (pkVictim->CanBeginFight())
		pkVictim->BeginFight(this);

	int iRet;

	if (bType == 0)
	{
#ifdef ENABLE_ANTY_CHEAT
		if (IsPC())
		{
			// RANGE LIMITER
			int distanceX = std::abs(lastAttackPosX - pkVictim->GetX());
			int32_t distanceY = std::abs(lastAttackPosY - pkVictim->GetY());
			int32_t distance = CalcDistance(distanceX, distanceY);
			float rangeLimit = GetAttackRangeLimit(this);
			
			if ( distance > rangeLimit )
			{
				if (test_server)
					ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> attack range limit: %d // distance: %d", rangeLimit, distance);
				return false;
			}
			// RANGE LIMITER

			int limmit = GetAttackLimit(this) + 200;
			if (!(GetFuncComboLimiterTime() + limmit >= dwCurrentTime))
			{
				if (dwCurrentTime - 5000 > GetFuncComboLimiterTime())
				{
					if (toDisconnectBfuncCombo == false)
					{
						if (test_server)
							ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> Combo limit time: %d // attack range limit: %d // distance: %d", dwCurrentTime - 5000 > GetFuncComboLimiterTime(), rangeLimit, distance);
						toDisconnectBfuncCombo = true;
						LogManager::instance().HackLog("WAIT_HACK_L", this);
						ChatPacket(CHAT_TYPE_COMMAND, "quit");
						Disconnect("Shutdown(WAIT_HACK_L)");
					}
				}
				return false;
			}
		}
#endif
		switch (GetMobBattleType())
		{
			case BATTLE_TYPE_MELEE:
			case BATTLE_TYPE_POWER:
			case BATTLE_TYPE_TANKER:
			case BATTLE_TYPE_SUPER_POWER:
			case BATTLE_TYPE_SUPER_TANKER:
				iRet = battle_melee_attack(this, pkVictim);
				break;

			case BATTLE_TYPE_RANGE:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(0) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			case BATTLE_TYPE_MAGIC:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(1) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			default:
				sys_err("Unhandled battle type %d", GetMobBattleType());
				iRet = BATTLE_NONE;
				break;
		}
	}
	else
	{
		if (IsPC() == true)
		{
			if (dwCurrentTime - m_dwLastSkillTime > 1500)
			{
				sys_log(1, "HACK: Too long skill using term. Name(%s) PID(%u) delta(%u)",
						GetName(), GetPlayerID(), (dwCurrentTime - m_dwLastSkillTime));
				return false;
			}
		}

		sys_log(1, "Attack call ComputeSkill %d %s", bType, pkVictim?pkVictim->GetName():"");
		iRet = ComputeSkill(bType, pkVictim);
	}

	if (iRet == BATTLE_DAMAGE || iRet == BATTLE_DEAD)
	{
		OnMove(true);
		pkVictim->OnMove();

		// only pc sets victim null. For npc, state machine will reset this.
		if (BATTLE_DEAD == iRet && IsPC())
			SetVictim(NULL);

		return true;
	}

	return false;
}

void CHARACTER::DeathPenalty(BYTE bTown)
{
	sys_log(1, "DEATH_PERNALY_CHECK(%s) town(%d)", GetName(), bTown);

	Cube_close(this);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CloseAcce();
#endif
#ifdef ENABLE_AURA_SYSTEM
	CloseAura();
#endif

	if (GetLevel() < 10)
	{
		sys_log(0, "NO_DEATH_PENALTY_LESS_LV10(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����� ��ȣ�� ����ġ�� �������� �ʾҽ��ϴ�."));
		return;
	}

   	if (number(0, 2))
	{
		sys_log(0, "NO_DEATH_PENALTY_LUCK(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����� ��ȣ�� ����ġ�� �������� �ʾҽ��ϴ�."));
		return;
	}

	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY))
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

		// NO_DEATH_PENALTY_BUG_FIX
		if (!bTown)
		{
			if (FindAffect(AFFECT_NO_DEATH_PENALTY))
			{
				sys_log(0, "NO_DEATH_PENALTY_AFFECT(%s)", GetName());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����� ��ȣ�� ����ġ�� �������� �ʾҽ��ϴ�."));
				RemoveAffect(AFFECT_NO_DEATH_PENALTY);
				return;
			}
		}
		// END_OF_NO_DEATH_PENALTY_BUG_FIX

		int iLoss = ((GetNextExp() * __GetExpLossPerc(GetLevel())) / 100);

		iLoss = MIN(800000, iLoss);

		if (bTown)
			iLoss = 0;

		if (IsEquipUniqueItem(UNIQUE_ITEM_TEARDROP_OF_GODNESS))
			iLoss /= 2;

		sys_log(0, "DEATH_PENALTY(%s) EXP_LOSS: %d percent %d%%", GetName(), iLoss, __GetExpLossPerc(GetLevel()));

		PointChange(POINT_EXP, -iLoss, true);
	}
}

bool CHARACTER::IsStun() const
{
	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN))
		return true;

	return false;
}

EVENTFUNC(StunEvent)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "StunEvent> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}
	ch->m_pkStunEvent = NULL;
	ch->Dead();
	return 0;
}

void CHARACTER::Stun(bool bJustDie)
{
	if (IsStun())
		return;

	if (IsDead())
		return;

	if (!IsPC() && m_pkParty)
	{
		m_pkParty->SendMessage(this, PM_ATTACKED_BY, 0, 0);
	}

	sys_log(1, "%s: Stun %p", GetName(), this);

	PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
	PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
	
	CloseMyShop();

	event_cancel(&m_pkRecoveryEvent); // ?? ???? ???.
	
	if(bJustDie)
	{
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);
		Dead();
	}
	else
	{
		TPacketGCStun pack;
		pack.header	= HEADER_GC_STUN;
		pack.vid	= m_vid;
		PacketAround(&pack, sizeof(pack));
	
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);
	
		if (m_pkStunEvent)
			return;
	
		char_event_info* info = AllocEventInfo<char_event_info>();
	
		info->ch = this;
	
		m_pkStunEvent = event_create(StunEvent, info, PASSES_PER_SEC(3));
	}
}

EVENTINFO(SCharDeadEventInfo)
{
	bool isPC;
	uint32_t dwID;

	SCharDeadEventInfo()
	: isPC(0)
	, dwID(0)
	{
	}
};

EVENTFUNC(dead_event)
{
	const SCharDeadEventInfo* info = dynamic_cast<SCharDeadEventInfo*>(event->info);

	if ( info == NULL )
	{
		sys_err( "dead_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = NULL;

	if (true == info->isPC)
	{
		ch = CHARACTER_MANAGER::instance().FindByPID( info->dwID );
	}
	else
	{
		ch = CHARACTER_MANAGER::instance().Find( info->dwID );
	}

	if (NULL == ch)
	{
		sys_err("DEAD_EVENT: cannot find char pointer with %s id(%d)", info->isPC ? "PC" : "MOB", info->dwID );
		return 0;
	}

	ch->m_pkDeadEvent = NULL;

	if (ch->GetDesc())
	{
		ch->GetDesc()->SetPhase(PHASE_GAME);

		ch->SetPosition(POS_STANDING);

		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
			if (ch->GetDungeon())
			{
				ch->WarpSet(1075900, 267800);
			} else {
				ch->WarpSet(pos.x, pos.y);
			}
		else
		{
			sys_err("cannot find spawn position (name %s)", ch->GetName());
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}

		ch->PointChange(POINT_HP, (ch->GetMaxHP() / 2) - ch->GetHP(), true);

		ch->DeathPenalty(0);

		ch->StartRecoveryEvent();

		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
	}
	else
	{
		if (ch->IsMonster() == true)
		{
			if (ch->IsRevive() == false && ch->HasReviverInParty() == true)
			{
				ch->SetPosition(POS_STANDING);
				ch->SetHP(ch->GetMaxHP());

				ch->ViewReencode();

				ch->SetAggressive();
				ch->SetRevive(true);

				return 0;
			}
		}

		M2_DESTROY_CHARACTER(ch);
	}

	return 0;
}

bool CHARACTER::IsDead() const
{
	if (m_pointsInstant.position == POS_DEAD)
		return true;

	return false;
}

#define GetGoldMultipler() (distribution_test_server ? 3 : 1)

void CHARACTER::RewardGold(LPCHARACTER pkAttacker)
{
	// ADD_PREMIUM
	bool isAutoLoot =
		(pkAttacker->GetPremiumRemainSeconds(PREMIUM_AUTOLOOT) > 0 ||
		 pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_AUTOLOOT))
		? true : false;
	// END_OF_ADD_PREMIUM

	PIXEL_POSITION pos = GetXYZ(); // @fixme194 GetMovablePosition is useless here

	int iTotalGold = 0;
	int iGoldPercent = MobRankStats[GetMobRank()].iGoldPercent;

	if (pkAttacker->IsPC())
		iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;

#ifdef ENABLE_EVENT_MANAGER
	if (pkAttacker->IsPC())
	{
		const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire());
		if(event != NULL)
			iGoldPercent = iGoldPercent * (100 + (event->value[0]+CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP))) / 100;
		else
			iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
	}
#else
	if (pkAttacker->IsPC())
		iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;
#endif

	iGoldPercent = iGoldPercent * CHARACTER_MANAGER::instance().GetMobGoldDropRate(pkAttacker) / 100;

	// ADD_PREMIUM
	if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0 ||
			pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_LUCKY_GOLD))
		iGoldPercent += iGoldPercent;
	// END_OF_ADD_PREMIUM

	if (iGoldPercent > 100)
		iGoldPercent = 100;

	int iPercent;

	if (GetMobRank() >= MOB_RANK_BOSS)
		iPercent = ((iGoldPercent * PERCENT_LVDELTA_BOSS(pkAttacker->GetLevel(), GetLevel())) / 100);
	else
		iPercent = ((iGoldPercent * PERCENT_LVDELTA(pkAttacker->GetLevel(), GetLevel())) / 100);

	if (number(1, 100) > iPercent)
		return;

	int iGoldMultipler = GetGoldMultipler();

	if (1 == number(1, 50000))
		iGoldMultipler *= 10;
	else if (1 == number(1, 10000))
		iGoldMultipler *= 5;

	if (pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
			iGoldMultipler *= 2;

	if (test_server)
		pkAttacker->ChatPacket(CHAT_TYPE_PARTY, "gold_mul %d rate %d", iGoldMultipler, CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker));

	LPITEM item;

	int iGold10DropPct = 100;
	// iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));

#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(YANG_DROP_EVENT, pkAttacker->GetEmpire());
	if(event != NULL)
		iGold10DropPct = (iGold10DropPct * 100) / (100 + event->value[0] + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
	else
		iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#else
	iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#endif

	if (GetMobRank() >= MOB_RANK_BOSS && !IsStone() && GetMobTable().dwGoldMax != 0)
	{
		if (1 == number(1, iGold10DropPct))
			iGoldMultipler *= 10;

		int iSplitCount = number(25, 35);

		for (int i = 0; i < iSplitCount; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax) / iSplitCount;
			if (test_server)
				sys_log(0, "iGold %d", iGold);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
				continue;

			if (test_server)
			{
				sys_log(0, "Drop Moeny MobGoldAmountRate %d %d", CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker), iGoldMultipler);
				sys_log(0, "Drop Money gold %d GoldMin %d GoldMax %d", iGold, GetMobTable().dwGoldMax, GetMobTable().dwGoldMax);
			}

			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + ((number(-14, 14) + number(-14, 14)) * 23);
				pos.y = GetY() + ((number(-14, 14) + number(-14, 14)) * 23);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}

	else if (1 == number(1, iGold10DropPct))
	{
		for (int i = 0; i < 10; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
				continue;

			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + (number(-7, 7) * 20);
				pos.y = GetY() + (number(-7, 7) * 20);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}
	else
	{
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
		iGold *= iGoldMultipler;

		int iSplitCount;

		if (iGold >= 3)
			iSplitCount = number(1, 3);
		else if (GetMobRank() >= MOB_RANK_BOSS)
		{
			iSplitCount = number(3, 10);

			if ((iGold / iSplitCount) == 0)
				iSplitCount = 1;
		}
		else
			iSplitCount = 1;

		if (iGold != 0)
		{
			iTotalGold += iGold; // Total gold

			for (int i = 0; i < iSplitCount; ++i)
			{
				if (isAutoLoot)
				{
					pkAttacker->GiveGold(iGold / iSplitCount);
				}
				else if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
				{
					pos.x = GetX() + (number(-7, 7) * 20);
					pos.y = GetY() + (number(-7, 7) * 20);

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER, GetRaceNum(), iTotalGold);
}

#ifdef ENABLE_KILL_NOTICE
void CHARACTER::CreateKillNotice(const CMob* pkMob, LPCHARACTER pkAttacker) {
	if (!pkMob || !pkAttacker || GetDungeon()) {
        return;
    }

    const char* attackerName = pkAttacker->GetName();
    int attackerLevel = pkAttacker->GetLevel();
    const char* mobName = pkMob->m_table.szLocaleName;
    const char* gender = (SEX_MALE == GET_SEX(pkAttacker)) ? "pokona? : "pokona?";

	char buf[128];
	snprintf(buf, sizeof(buf), "[CH%d]|cFFf7dd83 %s|r |cFFc5ff8c(Lv. %d)|r %s:|cFFf7b983 %s|r!", g_bChannel, attackerName, attackerLevel, gender, mobName);

    BroadcastNotice(buf);
}
#endif

typedef struct {
	LPITEM item;
	bool bAutoPickup;
} item_and_pickup_info;

void CHARACTER::Reward(bool bItemDrop)
{
	if (GetRaceNum() == 5001)
	{
		// @fixme194 BEGIN GetMovablePosition should not return
		PIXEL_POSITION pos = GetXYZ();
		SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos);
		// @fixme194 END

		LPITEM item;
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(NULL) / 100;
		iGold *= GetGoldMultipler();
		int iSplitCount = number(25, 35);

		sys_log(0, "WAEGU Dead gold %d split %d", iGold, iSplitCount);

		for (int i = 1; i <= iSplitCount; ++i)
		{
			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
			{
				if (i != 0)
				{
					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;

					pos.x += GetX();
					pos.y += GetY();
				}

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();
			}
		}
		return;
	}

   	LPCHARACTER pkAttacker = DistributeExp();

#ifdef ENABLE_KILL_EVENT_FIX
	if (!pkAttacker && !(pkAttacker = GetMostAttacked()))
		return;
#else
	if (!pkAttacker)
		return;
#endif

	if (pkAttacker->IsPC())
	{
		// if ((GetLevel() - pkAttacker->GetLevel()) >= -10)
		// {
			// if (pkAttacker->GetRealAlignment() < 0)
			// {
				// if (pkAttacker->IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_KILL))
					// pkAttacker->UpdateAlignment(14);
				// else
					// pkAttacker->UpdateAlignment(7);
			// }
			// else
				// pkAttacker->UpdateAlignment(2);
		// }

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());

#ifdef ENABLE_KILL_NOTICE
		if (!pkAttacker->GetDungeon()) {
			if (IsBoss() || IsWladca() || IsElitka() || IsLegenda()) {
				const CMob* pkMob = CMobManager::instance().Get(GetRaceNum());
				if (pkMob) {
					CreateKillNotice(pkMob, pkAttacker);
				}
			}
		}
		//}
#endif

		if (!number(0, 9))
		{
			if (pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY))
			{
				int iHP = pkAttacker->GetMaxHP() * pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY) / 100;
				pkAttacker->PointChange(POINT_HP, iHP);
				CreateFly(FLY_HP_SMALL, pkAttacker);
			}

			if (pkAttacker->GetPoint(POINT_KILL_SP_RECOVER))
			{
				int iSP = pkAttacker->GetMaxSP() * pkAttacker->GetPoint(POINT_KILL_SP_RECOVER) / 100;
				pkAttacker->PointChange(POINT_SP, iSP);
				CreateFly(FLY_SP_SMALL, pkAttacker);
			}
		}
	}

	if (!bItemDrop)
		return;

	// @fixme194 BEGIN GetMovablePosition should not return
	PIXEL_POSITION pos = GetXYZ();
	SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos);
	// @fixme194 END

	if (test_server)
		sys_log(0, "Drop money : Attacker %s", pkAttacker->GetName());
	RewardGold(pkAttacker);

	long lMapIndex = GetMapIndex();
	long mX = GetX();
	long mY = GetY();

	static std::vector<LPITEM> s_vec_item;
	static std::vector<LPITEM> s_vec_item_auto_pickup;
	s_vec_item.clear();
	s_vec_item_auto_pickup.clear();
	bool bCreateDropItem = ITEM_MANAGER::instance().CreateDropItem(this, pkAttacker, s_vec_item);

	LPITEM item;
	if (bCreateDropItem)
	{
		if (s_vec_item.size() == 0 && s_vec_item_auto_pickup.size() == 0);
		else if (s_vec_item.size() + s_vec_item_auto_pickup.size() == 1)
		{
			bool bAutoPickup = s_vec_item.size() == 0;
			if (bAutoPickup)
			{
				item = s_vec_item_auto_pickup[0];
				sys_log(0, "DROP_ITEM: %s %d %d from %s is_auto_pickup %d", item->GetName(), pos.x, pos.y, GetName(), bAutoPickup);
				if (item->GetVnum() == 50128)
				{
					pkAttacker->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
				}
				pkAttacker->AutoGiveItem(item, true);
			}
			else
			{
				item = s_vec_item[0];
				sys_log(0, "DROP_ITEM: %s %d %d from %s is_auto_pickup %d", item->GetName(), pos.x, pos.y, GetName(), bAutoPickup);
			}

			if (!bAutoPickup)
			{
				item->AddToGround(lMapIndex, pos);

				item->SetOwnership(pkAttacker);

				item->StartDestroyEvent(60);

				pos.x = number(-7, 7) * 20;
				pos.y = number(-7, 7) * 20;
				pos.x += mX;
				pos.y += mY;

				if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0)
					pkAttacker->PickupItem(item->GetVID(), true);
			}
		}
		else
		{
#ifdef __NEW_DROP_SYSTEM__

			std::random_device random_device;
			std::mt19937 random_gen(random_device());

			auto GetRandomUint64 = [&](std::uint64_t min, std::uint64_t max)
			{
				std::uniform_int_distribution<std::uint64_t> uid(min, max);
				return uid(random_gen);
			};

			// copy all items into one vector
			static std::vector<item_and_pickup_info> s_vec_full_item;

			s_vec_full_item.clear();
			s_vec_full_item.resize(s_vec_item.size() + s_vec_item_auto_pickup.size());
			
			for(int i = 0; i < s_vec_item.size(); ++i)
			{
				s_vec_full_item[ i ].item = s_vec_item[ i ];
				s_vec_full_item[ i ].bAutoPickup = false;
			}

			for(int i = 0; i < s_vec_item_auto_pickup.size(); ++i)
			{
				s_vec_full_item[ s_vec_item.size() + i ].item = s_vec_item_auto_pickup[ i ];
				s_vec_full_item[ s_vec_item.size() + i ].bAutoPickup = true;
			}

			int iItemIdx = s_vec_full_item.size() - 1;

			std::uint64_t totalDamage = 0;
			std::uint64_t highestDamage = 0;

			std::vector<std::pair<LPCHARACTER, int>> v_players{ };

			// Add players to list
			for(auto& damageIt : m_map_kDamage)
			{
				int damageDone = damageIt.second.iTotalDamage;

#ifdef __FAKE_PC__
				playerDamage += damageIt.second.iTotalFakePCDamage;
#endif

				if(damageDone <= 0)
					continue;

				LPCHARACTER lpCharacter = CHARACTER_MANAGER::instance().Find(damageIt.first);
			
				if(!lpCharacter)
					continue;

				if(damageDone > highestDamage)
					highestDamage = damageDone;

				totalDamage += damageDone;

				v_players.push_back(std::make_pair(lpCharacter, damageDone));
			}

			std::uint64_t damageTreshold = ( highestDamage * 20 ) / 100;

			sys_log(!test_server, "----------------------------------------------------------------");

			for(auto & player : v_players)
			{
				float damagePercent = ( float( player.second ) / totalDamage ) * 100.f;
				sys_log(!test_server, "NEW_DROP_SYSTEM: Player %s done %.1f%% of damage to the mob.", player.first->GetName(), damagePercent);
			}

			// Used only for logging, can remove after
			float damageTresholdPerc = ( float( damageTreshold ) / totalDamage ) * 100.f;

			sys_log(!test_server, "NEW_DROP_SYSTEM: Minimum damage to get drop: %d (~%.2f%% of total damage)", damageTreshold, damageTresholdPerc);

			v_players.erase( std::remove_if(v_players.begin(), v_players.end(), [&](const std::pair<LPCHARACTER,int>& playerInfo)
			{
				bool remove = ( playerInfo.second < damageTreshold );

				if(remove)
				{
					sys_log(!test_server, "NEW_DROP_SYSTEM: Removed %s from getting drop, too low damage.", playerInfo.first->GetName(), playerInfo.second);
					totalDamage -= playerInfo.second;
				}

				return remove;
			}), v_players.end());

			// Sort by damage
			std::sort(v_players.begin(), v_players.end(), [](const std::pair<LPCHARACTER, int>& player_a, const std::pair<LPCHARACTER, int>& player_b)
			{
				return player_a.second > player_b.second;
			});

			// Create ticket vector
			// LPCHARACTER:range{min,max}
			std::vector<std::pair<LPCHARACTER, std::pair<std::uint64_t, std::uint64_t>>> v_tickets{ };

			std::uint64_t range_start = 1;

			for(auto & player : v_players)
			{
				std::uint64_t start = range_start;
				std::uint64_t end = range_start + player.second - 1;

				v_tickets.push_back(std::make_pair(player.first, std::make_pair(start, end)));

				range_start = end + 1;
			}

			// Used only for logging
			std::vector<std::pair<LPCHARACTER, int>> v_itemdropcount;

			if(!v_tickets.empty())
			{
				while(iItemIdx >= 0)
				{
					item_and_pickup_info& r_info = s_vec_full_item[ iItemIdx-- ];
					item = r_info.item;

					if(!item)
						continue;

					item->AddToGround(lMapIndex, pos);

					std::uint64_t winning_number = GetRandomUint64(1, totalDamage);

					auto winner = std::find_if(v_tickets.begin(), v_tickets.end(), [winning_number](const std::pair<LPCHARACTER, std::pair<std::uint64_t, std::uint64_t>>& ticket_entry)
					{
						return winning_number >= ticket_entry.second.first && winning_number <= ticket_entry.second.second;
					});

					LPCHARACTER ch = ( *winner ).first;
					 
					// LOGGING
					auto it = std::find_if(v_itemdropcount.begin(), v_itemdropcount.end(), [&](const std::pair<LPCHARACTER,int>& itemDropLog)
					{
						return itemDropLog.first == ch;
					});

					if(it != v_itemdropcount.end())
						( *it ).second++;
					else v_itemdropcount.push_back(std::make_pair(ch, 1));
					// ---

#ifndef NO_PARTY_DROP
					if(ch->GetParty())
						ch = ch->GetParty()->GetNextOwnership(ch, mX, mY);
#endif
					item->SetOwnership(ch);

					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += mX;
					pos.y += mY;

					if(item->GetVnum() == 50128)
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
					}

					if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_PICKUP) > 0)
						pkAttacker->PickupItem(item->GetVID(), true);
				}
			}

			for(auto & itemDropLog : v_itemdropcount)
				sys_log(!test_server, "NEW_DROP_SYSTEM: Player %s dropped %d items.", itemDropLog.first->GetName(), itemDropLog.second);

			sys_log(!test_server, "----------------------------------------------------------------");

			v_players.clear();
			v_tickets.clear();
			v_itemdropcount.clear();
#else
			// copy all items into one vector
			static std::vector<item_and_pickup_info> s_vec_full_item;
			s_vec_full_item.clear();

			s_vec_full_item.resize(s_vec_item.size() + s_vec_item_auto_pickup.size());
			for (int i = 0; i < s_vec_item.size(); ++i)
			{
				s_vec_full_item[i].item = s_vec_item[i];
				s_vec_full_item[i].bAutoPickup = false;
			}
			for (int i = 0; i < s_vec_item_auto_pickup.size(); ++i)
			{
				s_vec_full_item[s_vec_item.size() + i].item = s_vec_item_auto_pickup[i];
				s_vec_full_item[s_vec_item.size() + i].bAutoPickup = true;
			}
			
			int iItemIdx = s_vec_full_item.size() - 1;

			std::priority_queue<std::pair<int, LPCHARACTER> > pq;

			std::uint64_t total_dam = 0;

			for (TDamageMap::iterator it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
			{
				int iDamage = it->second.iTotalDamage;
#ifdef __FAKE_PC__
				iDamage += it->second.iTotalFakePCDamage;
#endif
				if (iDamage > 0)
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

					if (ch)
					{
						pq.push(std::make_pair(iDamage, ch));
						total_dam += iDamage;
					}
				}
			}

			std::vector<LPCHARACTER> v;

			while (!pq.empty() && pq.top().first * 10 >= total_dam)
			{
				v.push_back(pq.top().second);
				pq.pop();
			}

			if (!v.empty())
			{
				std::vector<LPCHARACTER>::iterator it = v.begin();

				while (iItemIdx >= 0)
				{
					item_and_pickup_info& r_info = s_vec_full_item[iItemIdx--];
					item = r_info.item;

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					item->AddToGround(lMapIndex, pos);

					LPCHARACTER ch = *it;

#ifndef NO_PARTY_DROP
					if (ch->GetParty())
						ch = ch->GetParty()->GetNextOwnership(ch, mX, mY);
#endif
					++it;

					if (it == v.end())
						it = v.begin();

					item->SetOwnership(ch);

					item->StartDestroyEvent();

					pos.x = random_number(-7, 7) * 20;
					pos.y = random_number(-7, 7) * 20;
					pos.x += mX;
					pos.y += mY;
					
					if (item->GetVnum() == 50128)
					{
						ch->SetItemDropQuest(item->GetVnum());
						ch->ChatPacket(CHAT_TYPE_COMMAND, "ITEM_QUEST_DROP");
					}

					sys_log(0, "DROP_ITEM: %s %dx %d %d by %s", item->GetName(), item->GetCount(), pos.x, pos.y, mName);
				}
			}
#endif
		}
	}

	m_map_kDamage.clear();
}

struct TItemDropPenalty
{
	int iInventoryPct;		// Range: 1 ~ 1000
	int iInventoryQty;		// Range: --
	int iEquipmentPct;		// Range: 1 ~ 100
	int iEquipmentQty;		// Range: --
};

TItemDropPenalty aItemDropPenalty_kor[9] =
{
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{  25,   1,  5,  1 },
	{  50,   2, 10,  1 },
	{  75,   4, 15,  1 },
	{ 100,   8, 20,  1 },
};

void CHARACTER::ItemDropPenalty(LPCHARACTER pkKiller)
{
	if (GetMyShop())
		return;

	if (GetLevel() < 50)
		return;

	struct TItemDropPenalty * table = &aItemDropPenalty_kor[0];

	if (GetLevel() < 10)
		return;

	long long iAlignIndex;

#ifdef ENABLE_ALIGN_RENEWAL
	if (GetRealAlignment() >= 500000)			
		iAlignIndex = 0;
	else if (GetRealAlignment() >= 450000)		
		iAlignIndex = 1;
	else if (GetRealAlignment() >= 350000)		
		iAlignIndex = 2;
	else if (GetRealAlignment() >= 250000)		
		iAlignIndex = 3;
	else if (GetRealAlignment() >= 150000)		
		iAlignIndex = 4;
	else if (GetRealAlignment() >= 50000)		
		iAlignIndex = 5;
	else if (GetRealAlignment() >= 25000)		
		iAlignIndex = 6;
	else if (GetRealAlignment() >= 0)
		iAlignIndex = 7;
	else if (GetRealAlignment() > -25000)
		iAlignIndex = 8;
	else if (GetRealAlignment() > -50000)
		iAlignIndex = 9;
	else if (GetRealAlignment() > -100000)
		iAlignIndex = 10;
	else
		iAlignIndex = 11;
#else
	if (GetRealAlignment() >= 12000)
		iAlignIndex = 0;
	else if (GetRealAlignment() >= 8000)
		iAlignIndex = 1;
	else if (GetRealAlignment() >= 4000)
		iAlignIndex = 2;
	else if (GetRealAlignment() >= 1000)
		iAlignIndex = 3;
	else if (GetRealAlignment() >= 0)
		iAlignIndex = 4;
	else if (GetRealAlignment() > -4000)
		iAlignIndex = 5;
	else if (GetRealAlignment() > -8000)
		iAlignIndex = 6;
	else if (GetRealAlignment() > -12000)
		iAlignIndex = 7;
	else
		iAlignIndex = 8;
#endif

	// std::vector<std::pair<LPITEM, int> > vec_item;
	// LPITEM pkItem;
	// int	i;
	// bool isDropAllEquipments = false;

	// TItemDropPenalty & r = table[iAlignIndex];
	// sys_log(0, "%s align %d inven_pct %d equip_pct %d", GetName(), iAlignIndex, r.iInventoryPct, r.iEquipmentPct);

	// bool bDropInventory = r.iInventoryPct >= number(1, 1000);
	// bool bDropEquipment = r.iEquipmentPct >= number(1, 100);
	// bool bDropAntiDropUniqueItem = false;

	// if ((bDropInventory || bDropEquipment) && IsEquipUniqueItem(UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY))
	// {
		// bDropInventory = false;
		// bDropEquipment = false;
		// bDropAntiDropUniqueItem = true;
	// }

	// @fixme198 BEGIN
	// if (bDropInventory || bDropEquipment) {
		// if (pkKiller)
			// pkKiller->SetExchangeTime();
		// SetExchangeTime();
	// }
	// @fixme198 END

	// if (bDropInventory) // Drop Inventory
	// {
		// std::vector<BYTE> vec_bSlots;

		// for (i = 0; i < INVENTORY_MAX_NUM; ++i)
			// if (GetInventoryItem(i))
				// vec_bSlots.emplace_back(i);

		// if (!vec_bSlots.empty())
		// {
			// random_shuffle(vec_bSlots.begin(), vec_bSlots.end());

			// int iQty = MIN(vec_bSlots.size(), r.iInventoryQty);

			// if (iQty)
				// iQty = number(1, iQty);

			// for (i = 0; i < iQty; ++i)
			// {
				// pkItem = GetInventoryItem(vec_bSlots[i]);

				// if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					// continue;

				// SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 255);
				// vec_item.emplace_back(pkItem->RemoveFromCharacter(), INVENTORY);
			// }
		// }
		// else if (iAlignIndex == 8)
			// isDropAllEquipments = true;
	// }

	// if (bDropEquipment) // Drop Equipment
	// {
		// std::vector<BYTE> vec_bSlots;

		// for (i = 0; i < WEAR_MAX_NUM; ++i)
			// if (GetWear(i))
				// vec_bSlots.emplace_back(i);

		// if (!vec_bSlots.empty())
		// {
			// random_shuffle(vec_bSlots.begin(), vec_bSlots.end());
			// int iQty;

			// if (isDropAllEquipments)
				// iQty = vec_bSlots.size();
			// else
				// iQty = MIN(vec_bSlots.size(), number(1, r.iEquipmentQty));

			// if (iQty)
				// iQty = number(1, iQty);

			// for (i = 0; i < iQty; ++i)
			// {
				// pkItem = GetWear(vec_bSlots[i]);

				// if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					// continue;

				// SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 255);
				// vec_item.emplace_back(pkItem->RemoveFromCharacter(), EQUIPMENT);
			// }
		// }
	// }

	// if (bDropAntiDropUniqueItem)
	// {
		// LPITEM pkItem;

		// pkItem = GetWear(WEAR_UNIQUE1);

		// if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		// {
			// SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE1, 255);
			// vec_item.emplace_back(pkItem->RemoveFromCharacter(), EQUIPMENT);
		// }

		// pkItem = GetWear(WEAR_UNIQUE2);

		// if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		// {
			// SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE2, 255);
			// vec_item.emplace_back(pkItem->RemoveFromCharacter(), EQUIPMENT);
		// }
	// }

	// {
		// PIXEL_POSITION pos;
		// pos.x = GetX();
		// pos.y = GetY();

		// unsigned int i;

		// for (i = 0; i < vec_item.size(); ++i)
		// {
			// LPITEM item = vec_item[i].first;
			// int window = vec_item[i].second;

			// item->AddToGround(GetMapIndex(), pos);
			// item->StartDestroyEvent();

			// sys_log(0, "DROP_ITEM_PK: %s %d %d from %s", item->GetName(), pos.x, pos.y, GetName());
			// LogManager::instance().ItemLog(this, item, "DEAD_DROP", (window == INVENTORY) ? "INVENTORY" : ((window == EQUIPMENT) ? "EQUIPMENT" : ""));

			// pos.x = GetX() + number(-7, 7) * 20;
			// pos.y = GetY() + number(-7, 7) * 20;
		// }
	// }
}

class FPartyAlignmentCompute
{
	public:
		FPartyAlignmentCompute(int iAmount, int x, int y)
		{
			m_iAmount = iAmount;
			m_iCount = 0;
			m_iStep = 0;
			m_iKillerX = x;
			m_iKillerY = y;
		}

		void operator () (LPCHARACTER pkChr)
		{
			if (DISTANCE_APPROX(pkChr->GetX() - m_iKillerX, pkChr->GetY() - m_iKillerY) < PARTY_DEFAULT_RANGE)
			{
				if (m_iStep == 0)
				{
					++m_iCount;
				}
				else
				{
					pkChr->UpdateAlignment(m_iAmount / m_iCount);
				}
			}
		}

		int m_iAmount;
		int m_iCount;
		int m_iStep;

		int m_iKillerX;
		int m_iKillerY;
};

void CHARACTER::Dead(LPCHARACTER pkKiller, bool bImmediateDead)
{
	if (IsDead())
		return;
#ifndef DISABLE_STOP_RIDING_WHEN_DIE
	{
		if (IsHorseRiding())
		{
			StopRiding();
		}
		else if (GetMountVnum())
		{
			RemoveAffect(AFFECT_MOUNT_BONUS);
			m_dwMountVnum = 0;
			UnEquipSpecialRideUniqueItem();

			UpdatePacket();
		}

	}
#endif

	if (!pkKiller && m_dwKillerPID)
		pkKiller = CHARACTER_MANAGER::instance().FindByPID(m_dwKillerPID);

	m_dwKillerPID = 0;

	bool isAgreedPVP = false;
	bool isUnderGuildWar = false;
	bool isDuel = false;
	bool isForked = false;

	if (pkKiller && pkKiller->IsPC())
	{
		if (pkKiller->m_pkChrTarget == this)
			pkKiller->SetTarget(NULL);

		if (!IsPC() && pkKiller->GetDungeon())
			pkKiller->GetDungeon()->IncKillCount(pkKiller, this);

		isAgreedPVP = CPVPManager::instance().Dead(this, pkKiller->GetPlayerID());
		isDuel = CArenaManager::instance().OnDead(pkKiller, this);

#ifdef ENABLE_LRN_BATTLE_PASS
		if (!IsPC() && pkKiller->IsPC())
		{
			if (auto pBattlePass = pkKiller->GetBattlePass(); pBattlePass) {
				if (IsBoss() || IsWladca()) {
					pBattlePass->SetExp(2);
				} else if (IsStone()) {
					if (!pkKiller->GetDungeon()) {
						pBattlePass->SetExp(1);
					}
				}
			}
				// pBattlePass->AnalyzeQuestSignal(IsStone() ? BATTLE_PASS_STONE : (GetMobRank() >= MOB_RANK_BOSS) ? BATTLE_PASS_BOSS : BATTLE_PASS_MONSTER);
		}
#endif

		if (IsPC())
		{
			CGuild * g1 = GetGuild();
			CGuild * g2 = pkKiller->GetGuild();

			if (g1 && g2)
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);
		}
	}

#ifdef ENABLE_QUEST_DIE_EVENT
	if (IsPC())
	{
		if (pkKiller)
			SetQuestNPCID(pkKiller->GetVID());
		// quest::CQuestManager::instance().Die(GetPlayerID(), quest::QUEST_NO_NPC);
		quest::CQuestManager::instance().Die(GetPlayerID(), (pkKiller)?pkKiller->GetRaceNum():quest::QUEST_NO_NPC);
	}
#endif

	//CHECK_FORKEDROAD_WAR
	if (IsPC())
	{
		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
			isForked = true;
	}
	//END_CHECK_FORKEDROAD_WAR

	if (pkKiller &&
			!isAgreedPVP &&
			!isUnderGuildWar &&
			IsPC() &&
			!isDuel &&
			!isForked &&
			!IS_CASTLE_MAP(GetMapIndex()))
	{
		if (GetGMLevel() == GM_PLAYER || test_server)
		{
			ItemDropPenalty(pkKiller);
		}
	}

	// CASTLE_SIEGE
	if (IS_CASTLE_MAP(GetMapIndex()))
	{
		if (CASTLE_FROG_VNUM == GetRaceNum())
			castle_frog_die(this, pkKiller);
		else if (castle_is_guard_vnum(GetRaceNum()))
			castle_guard_die(this, pkKiller);
		else if (castle_is_tower_vnum(GetRaceNum()))
			castle_tower_die(this, pkKiller);
	}
	// CASTLE_SIEGE

	if (true == isForked)
	{
		CThreeWayWar::instance().onDead( this, pkKiller );
	}

	SetPosition(POS_DEAD);
	ClearAffect(true);

	if (pkKiller && IsPC())
	{
		if (!pkKiller->IsPC())
		{
			if (!isForked)
			{
				sys_log(1, "DEAD: %s %p WITH PENALTY", GetName(), this);
				SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
				LogManager::instance().CharLog(this, pkKiller->GetRaceNum(), "DEAD_BY_NPC", pkKiller->GetName());
			}
		}
		else
		{
			sys_log(1, "DEAD_BY_PC: %s %p KILLER %s %p", GetName(), this, pkKiller->GetName(), get_pointer(pkKiller));
			REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
			if (GetEmpire() != pkKiller->GetEmpire())
			{
				int iEP = MIN(GetPoint(POINT_EMPIRE_POINT), pkKiller->GetPoint(POINT_EMPIRE_POINT));

				PointChange(POINT_EMPIRE_POINT, -(iEP / 10));
				pkKiller->PointChange(POINT_EMPIRE_POINT, iEP / 5);

				if (GetPoint(POINT_EMPIRE_POINT) < 10)
				{
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %lld %d %s %d %lld %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
			else
			{
				// if (!isAgreedPVP && !isUnderGuildWar && !IsKillerMode() && GetAlignment() >= 0 && !isDuel && !isForked)
				// {
					// int iNoPenaltyProb = 0;

					// if (pkKiller->GetAlignment() >= 0)	// 1/3 percent down
						// iNoPenaltyProb = 33;
					// else				// 4/5 percent down
						// iNoPenaltyProb = 20;

					// if (number(1, 100) < iNoPenaltyProb)
						// pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("����� ��ȣ�� �������� �������� �ʾҽ��ϴ�."));
					// else
					// {
						// if (pkKiller->GetParty())
						// {
							// FPartyAlignmentCompute f(-20000, pkKiller->GetX(), pkKiller->GetY());
							// pkKiller->GetParty()->ForEachOnlineMember(f);

							// if (f.m_iCount == 0)
								// pkKiller->UpdateAlignment(-20000);
							// else
							// {
								// sys_log(0, "ALIGNMENT PARTY count %d amount %d", f.m_iCount, f.m_iAmount);

								// f.m_iStep = 1;
								// pkKiller->GetParty()->ForEachOnlineMember(f);
							// }
						// }
						// else
							// pkKiller->UpdateAlignment(-20000);
					// }
				// }

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %lld %d %s %d %lld %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
		}
	}
	else
	{
		sys_log(1, "DEAD: %s %p", GetName(), this);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
	}

	ClearSync();

	//sys_log(1, "stun cancel %s[%d]", GetName(), (DWORD)GetVID());
	event_cancel(&m_pkStunEvent);

	if (IsPC())
	{
		m_dwLastDeadTime = get_dword_time();
		SetKillerMode(false);
		GetDesc()->SetPhase(PHASE_DEAD);
	}
	else
	{
#ifdef ENABLE_RESP_SYSTEM
		CRespManager::instance().KillMob(this);
#endif
		if (!IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD))
		{
			if (!(pkKiller && pkKiller->IsPC() && pkKiller->GetGuild() && pkKiller->GetGuild()->UnderAnyWar(GUILD_WAR_TYPE_FIELD)))
			{
				if (GetMobTable().dwResurrectionVnum)
				{
					// DUNGEON_MONSTER_REBIRTH_BUG_FIX
					LPCHARACTER chResurrect = CHARACTER_MANAGER::instance().SpawnMob(GetMobTable().dwResurrectionVnum, GetMapIndex(), GetX(), GetY(), GetZ(), true, (int) GetRotation());
					if (GetDungeon() && chResurrect)
					{
						chResurrect->SetDungeon(GetDungeon());
					}
					// END_OF_DUNGEON_MONSTER_REBIRTH_BUG_FIX

					Reward(false);
				}
				else if (IsRevive() == true)
				{
					Reward(false);
				}
				else
				{
					Reward(true); // Drops gold, item, etc..
				}
			}
			else
			{
				if (pkKiller->m_dwUnderGuildWarInfoMessageTime < get_dword_time())
				{
					pkKiller->m_dwUnderGuildWarInfoMessageTime = get_dword_time() + 60000;
					pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<���> ������߿��� ��ɿ� ���� ������ �����ϴ�."));
				}
			}
		}
	}
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	if (!IsPC() && (pkKiller && pkKiller->IsPC()))
	{
		if (GetMobTable().sPktOsiag > 0) {
			short pkt_osiag = GetMobTable().sPktOsiag;
			pkKiller->PointChange(POINT_PKT_OSIAG, pkt_osiag, true);
		}
	}
#endif

	// BOSS_KILL_LOG
	if (GetMobRank() >= MOB_RANK_BOSS && pkKiller && pkKiller->IsPC())
	{
		char buf[51];
		snprintf(buf, sizeof(buf), "%d %ld", g_bChannel, pkKiller->GetMapIndex());
		if (IsStone())
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "STONE_KILL", buf);
		else
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "BOSS_KILL", buf);
	}
	// END_OF_BOSS_KILL_LOG

#ifndef RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));
#endif

	REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	if (GetDesc() != NULL) {
		itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

		while (it != m_list_pkAffect.end())
			SendAffectAddPacket(GetDesc(), *it++);
	}

	if (isDuel == false)
	{
		if (m_pkDeadEvent)
		{
			sys_log(1, "DEAD_EVENT_CANCEL: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			event_cancel(&m_pkDeadEvent);
		}

		if (IsStone())
			ClearStone();

		if (GetDungeon())
		{
			GetDungeon()->DeadCharacter(this);
		}

		SCharDeadEventInfo* pEventInfo = AllocEventInfo<SCharDeadEventInfo>();

		if (IsPC())
		{
			pEventInfo->isPC = true;
			pEventInfo->dwID = this->GetPlayerID();

			m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(180));
		}
		else
		{
			pEventInfo->isPC = false;
			pEventInfo->dwID = this->GetVID();

			if (IsRevive() == false && HasReviverInParty() == true)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(3));
			}
			else
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(10));
			}
		}

		sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

#ifdef RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	for (BYTE i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
		pack.t_d[i] = CalculateDeadTime(i);
	PacketAround(&pack, sizeof(pack));
#endif

	if (IsCubeOpen())
		Cube_close(this);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (IsPC())
		CloseAcce();
#endif

#ifdef ENABLE_AURA_SYSTEM
	if (IsPC())
		CloseAura();
#endif

	CShopManager::instance().StopShopping(this);
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	COfflineShopManager::instance().StopShopping(this);
#endif
	CloseMyShop();
	CloseSafebox();

#ifdef __PREMIUM_PRIVATE_SHOP__
	ClosePrivateShopPanel(true);
	CloseShopSearch();
	if (GetViewingPrivateShop())
		GetViewingPrivateShop()->RemoveShopViewer(this);
#endif

	if (IsMonster() && 2493 == GetMobTable().dwVnum)
	{
		if (pkKiller && pkKiller->GetGuild())
			CDragonLairManager::instance().OnDragonDead(this, pkKiller->GetGuild()->GetID());
		else
			sys_err("DragonLair: Dragon killed by nobody");
	}
#ifdef WEEKLY_RANK_BYLUZER
	if (IsMonster() && pkKiller && pkKiller->IsPC())
	{
		if (GetMobTable().dwVnum == 2092 || GetMobTable().dwVnum == 1093)
		{
			pkKiller->PointChange(POINT_WEEKLY4, 1);
		}
		else if (GetMobTable().dwVnum == 9682 || GetMobTable().dwVnum == 9694)
		{
			pkKiller->PointChange(POINT_WEEKLY4, 2);
		}
		else if (GetMobTable().dwVnum == 9712 || GetMobTable().dwVnum == 29754)
		{
			pkKiller->PointChange(POINT_WEEKLY4, 3);
		}
		else if (GetMobTable().dwVnum == 16103)
		{
			pkKiller->PointChange(POINT_WEEKLY4, 4);
		}
		else if (GetMobRank() >= MOB_RANK_BOSS)
		{
			if (!pkKiller->InDungeon()) {
				if (GetMobTable().dwVnum == 691 || GetMobTable().dwVnum == 2091 || GetMobTable().dwVnum == 3913 || GetMobTable().dwVnum == 1901) {
					pkKiller->PointChange(POINT_WEEKLY2, 1);
				} else if (GetMobTable().dwVnum == 3190 || GetMobTable().dwVnum == 3191 || GetMobTable().dwVnum == 8213 || GetMobTable().dwVnum == 8214) {
					pkKiller->PointChange(POINT_WEEKLY2, 2);
				} else if (GetMobTable().dwVnum == 6775 || GetMobTable().dwVnum == 6789 || GetMobTable().dwVnum == 34600 || GetMobTable().dwVnum == 34601) {
					pkKiller->PointChange(POINT_WEEKLY2, 3);
				} else if (GetMobTable().dwVnum == 3390 || GetMobTable().dwVnum == 3391) {
					pkKiller->PointChange(POINT_WEEKLY2, 4);
				}
			}
		}
		else
		{
			if (GetLevel() > (pkKiller->GetLevel()+25) || GetLevel() < (pkKiller->GetLevel()-25))
				return;
				
			pkKiller->PointChange(POINT_WEEKLY1, 1);
		}
		pkKiller->CheckWeekly();
	}
	else if (IsStone() && pkKiller && pkKiller->IsPC())
	{
		if (!pkKiller->InDungeon() && GetLevel() > 30)
		{
			if (GetLevel() > (pkKiller->GetLevel()+25) || GetLevel() < (pkKiller->GetLevel()-25))//+-20lv
				return;

			if (GetMobTable().dwVnum == 8008 || GetMobTable().dwVnum == 8009 || GetMobTable().dwVnum == 2095 || GetMobTable().dwVnum == 8013 || GetMobTable().dwVnum == 8027) {
				pkKiller->PointChange(POINT_WEEKLY3, 1);
			} else if (GetMobTable().dwVnum == 8052 || GetMobTable().dwVnum == 8053 || GetMobTable().dwVnum == 8210 || GetMobTable().dwVnum == 8207) {
				pkKiller->PointChange(POINT_WEEKLY3, 2);
			} else if (GetMobTable().dwVnum == 6798 || GetMobTable().dwVnum == 8203 || GetMobTable().dwVnum == 6801 || GetMobTable().dwVnum == 6799) {
				pkKiller->PointChange(POINT_WEEKLY3, 3);
			} else if (GetMobTable().dwVnum == 6802 || GetMobTable().dwVnum == 6803) {
				pkKiller->PointChange(POINT_WEEKLY3, 4);
			}
			pkKiller->CheckWeekly();
		}
	}
#endif
}

struct FuncSetLastAttacked
{
	FuncSetLastAttacked(DWORD dwTime) : m_dwTime(dwTime)
	{
	}

	void operator () (LPCHARACTER ch)
	{
		ch->SetLastAttacked(m_dwTime);
	}

	DWORD m_dwTime;
};

void CHARACTER::SetLastAttacked(DWORD dwTime)
{
	assert(m_pkMobInst != NULL);

	m_pkMobInst->m_dwLastAttackedTime = dwTime;
	m_pkMobInst->m_posLastAttacked = GetXYZ();
}

void CHARACTER::SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag)
{
	if (IsPC() == true || (pAttacker->IsPC() == true && pAttacker->GetTarget() == this))
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));

		damageInfo.header = HEADER_GC_DAMAGE_INFO;
		damageInfo.dwVID = (DWORD)GetVID();
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}

		if (pAttacker->GetDesc() != NULL)
		{
			pAttacker->GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}
	}
}

//
// Arguments
//
// Return value
//    true		: dead
//    false		: not dead yet
//
bool CHARACTER::Damage(LPCHARACTER pAttacker, int dam, EDamageType type) // returns true if dead
{
	if (pAttacker) {
		LPITEM weapon;
		if (pAttacker->IsPolymorphed())
		if (!(weapon = pAttacker->GetWear(WEAR_WEAPON))) {
			pAttacker->ChatPacket(CHAT_TYPE_INFO, "Za��?bro?");
			return false;
		}

		// legenda
		if (GetRaceNum() == 6791) {
			if (pAttacker->GetLevel() > 149) {
				return false;
			}
		}
	// STATEK WIDMO
		if ((GetRaceNum() == 16155) || (GetRaceNum() == 16177)) {
			if (!pAttacker->IsPolymorphed()) {
				return false;
			}
		}
		if ((GetRaceNum() >= 16163 && GetRaceNum() <= 16167) || (GetRaceNum() >= 16185 && GetRaceNum() <= 16189)) {
			if (!pAttacker->IsRiding()) {
				return false;
			}
		}
		if (GetRaceNum() == 16156 || 
			(GetRaceNum() >= 16168 && GetRaceNum() <= 16173) || 
			GetRaceNum() == 16178 || 
			(GetRaceNum() >= 16190 && GetRaceNum() <= 16195)) {
			if (pAttacker->IsPolymorphed() || pAttacker->IsRiding()) {
				return false;
			}
		}
	}

	if ((GetRaceNum() == 16159) || (GetRaceNum() == 16181)) {
		if (type != DAMAGE_TYPE_MELEE && type != DAMAGE_TYPE_RANGE && type != DAMAGE_TYPE_MAGIC) {
			return false;
		}
	}
//
	
#ifdef ENABLE_NEWSTUFF
	if (pAttacker && IsStone() && pAttacker->IsPC())
	{
		if (GetEmpire() && GetEmpire() == pAttacker->GetEmpire())
		{
			SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
			return false;
		}
	}
#endif

	if (pAttacker && DAMAGE_TYPE_MAGIC == type)
		dam = (int)((float)dam * (100 + (pAttacker->GetPoint(POINT_MAGIC_ATT_BONUS_PER) + pAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100.f + 0.5f);

	if (type != DAMAGE_TYPE_NORMAL && type != DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (IsAffectFlag(AFF_TERROR))
		{
			int pct = GetSkillPower(SKILL_TERROR) / 400;

			if (number(1, 100) <= pct)
				return false;
		}
	}

	// blokada tp po otrzymaniu dmg od gracza
	if (pAttacker && pAttacker->IsPC() && this->IsPC())
		this->SetPlayerDamageTime();

#ifdef ENABLE_NO_DAMAGE_QUEST_RUNNING
	if (pAttacker && pAttacker->IsPC())
	{
		auto * pPC = quest::CQuestManager::instance().GetPCForce(pAttacker->GetPlayerID());
		if (pPC->IsRunning())
		{
			if (test_server)
			{
				auto & mgr = quest::CQuestManager::instance();
				sys_err("QUEST There's suspended quest state for %s, can't run new quest state (quest: %s pc: %s)",
						pAttacker->GetName(),
						pPC->GetCurrentQuestName().c_str(),
						mgr.GetCurrentCharacterPtr() ? mgr.GetCurrentCharacterPtr()->GetName() : "<none>");
			}
			pAttacker->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't deal damage while running quests"));
			return false;
		}
	}
#endif

	if (GetRaceNum() == 5001)
	{
		bool bDropMoney = false;

		int iPercent = 0; // @fixme136
		if (GetMaxHP() >= 0)
			iPercent = (GetHP() * 100) / GetMaxHP();

		if (iPercent <= 10 && GetMaxSP() < 5)
		{
			SetMaxSP(5);
			bDropMoney = true;
		}
		else if (iPercent <= 20 && GetMaxSP() < 4)
		{
			SetMaxSP(4);
			bDropMoney = true;
		}
		else if (iPercent <= 40 && GetMaxSP() < 3)
		{
			SetMaxSP(3);
			bDropMoney = true;
		}
		else if (iPercent <= 60 && GetMaxSP() < 2)
		{
			SetMaxSP(2);
			bDropMoney = true;
		}
		else if (iPercent <= 80 && GetMaxSP() < 1)
		{
			SetMaxSP(1);
			bDropMoney = true;
		}

		if (bDropMoney)
		{
			DWORD dwGold = 1000;
			int iSplitCount = number(10, 13);

			sys_log(0, "WAEGU DropGoldOnHit %d times", GetMaxSP());

			for (int i = 1; i <= iSplitCount; ++i)
			{
				PIXEL_POSITION pos;
				LPITEM item;

				if ((item = ITEM_MANAGER::instance().CreateItem(1, dwGold / iSplitCount)))
				{
					if (i != 0)
					{
						pos.x = (number(-14, 14) + number(-14, 14)) * 20;
						pos.y = (number(-14, 14) + number(-14, 14)) * 20;

						pos.x += GetX();
						pos.y += GetY();
					}

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	int iCurHP = GetHP();
	int iCurSP = GetSP();

	bool IsCritical = false;
	bool IsPenetrate = false;
	bool IsDeathBlow = false;

	if (type == DAMAGE_TYPE_MELEE || type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC)
	{
		if (pAttacker)
		{
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC())
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{
				if (iCriticalPct >= 10)
					iCriticalPct = 5 + (iCriticalPct - 10) / 4;
				else
					iCriticalPct /= 2;

				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}

			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			if (iPenetratePct)
			{
				{
					CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

					if (NULL != pkSk)
					{
						pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

						iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
					}
				}

				if (iPenetratePct >= 10)
				{
					iPenetratePct = 5 + (iPenetratePct - 10) / 4;
				}
				else
				{
					iPenetratePct /= 2;
				}

				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� �߰� ������ %d"), GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);

					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
#ifdef ENABLE_EFFECT_PENETRATE
					EffectPacket(SE_PENETRATE);
#endif
				}
			}
		}
	}

	else if (type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE)
	{
#ifdef ENABLE_ANTY_CHEAT
		if (pAttacker && pAttacker->IsPC())
		{
			boost::unordered_map<VID, DWORD>::iterator it = simpleAttackRateCounterMap.find(pAttacker->GetVID());
			DWORD current_time = get_dword_time();
			if (it != simpleAttackRateCounterMap.end())
			{
				DWORD msSinceLastAttack = current_time - it->second;
				DWORD limit = GetAttackLimit(pAttacker);

				if (msSinceLastAttack < limit) 
				{
					if (test_server)
						pAttacker->ChatPacket(CHAT_TYPE_PARTY, "<ANTYCHEAT> msSinceLastAttack=%d", msSinceLastAttack);
					return false;
				}
				else
				{
					simpleAttackRateCounterMap[pAttacker->GetVID()] = current_time; 
				}
			}
			else
			{
				simpleAttackRateCounterMap.emplace(pAttacker->GetVID(), current_time);
			}
		}
#endif
		if (type == DAMAGE_TYPE_NORMAL)
		{
			if (GetPoint(POINT_BLOCK) && number(1, 100) <= GetPoint(POINT_BLOCK))
			{
				if (test_server)
				{
					pAttacker->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ��! (%d%%)"), GetName(), GetPoint(POINT_BLOCK));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ��! (%d%%)"), GetName(), GetPoint(POINT_BLOCK));
				}

				SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
				return false;
			}
		}
		else if (type == DAMAGE_TYPE_NORMAL_RANGE)
		{
			if (GetPoint(POINT_DODGE) && number(1, 100) <= GetPoint(POINT_DODGE))
			{
				if (test_server)
				{
					pAttacker->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ȸ��! (%d%%)"), GetName(), GetPoint(POINT_DODGE));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ȸ��! (%d%%)"), GetName(), GetPoint(POINT_DODGE));
				}

				SendDamagePacket(pAttacker, 0, DAMAGE_DODGE);
				return false;
			}
		}

		if (IsAffectFlag(AFF_JEONGWIHON))
			dam = (int) (dam * (100 + GetSkillPower(SKILL_JEONGWI) * 25 / 100) / 100);

		if (IsAffectFlag(AFF_TERROR))
			dam = (int) (dam * (95 - GetSkillPower(SKILL_TERROR) / 5) / 100);

		if (IsAffectFlag(AFF_HOSIN))
			dam = dam * (100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE)) / 100;
		
		if (pAttacker)
		{
			if (type == DAMAGE_TYPE_NORMAL)
			{
				if (GetPoint(POINT_REFLECT_MELEE))
				{
					int reflectDamage = dam * GetPoint(POINT_REFLECT_MELEE) / 100;

					if (pAttacker->IsImmune(IMMUNE_REFLECT))
						reflectDamage = int(reflectDamage / 3.0f + 0.5f);

					pAttacker->Damage(this, reflectDamage, DAMAGE_TYPE_SPECIAL);
				}
			}

			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC())
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);
				}
			}

			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC())
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			{
				CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

				if (NULL != pkSk)
				{
					pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

					iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
				}
			}

			if (iPenetratePct)
			{
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� �߰� ������ %d"), GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);
					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;
#ifdef ENABLE_EFFECT_PENETRATE
					EffectPacket(SE_PENETRATE);
#endif
				}
			}

			if (pAttacker->GetPoint(POINT_STEAL_HP))
			{
				int pct = 1;

				if (number(1, 10) <= pct)
				{
					int iHP = MIN(dam, MAX(0, iCurHP)) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;

					if (iHP > 0 && GetHP() >= iHP)
					{
						CreateFly(FLY_HP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_HP, iHP);
						PointChange(POINT_HP, -iHP);
					}
				}
			}

			if (pAttacker->GetPoint(POINT_STEAL_SP))
			{
				int pct = 1;

				if (number(1, 10) <= pct)
				{
					int iCur;

					if (IsPC())
						iCur = iCurSP;
					else
						iCur = iCurHP;

					int iSP = MIN(dam, MAX(0, iCur)) * pAttacker->GetPoint(POINT_STEAL_SP) / 100;

					if (iSP > 0 && iCur >= iSP)
					{
						CreateFly(FLY_SP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_SP, iSP);

						if (IsPC())
							PointChange(POINT_SP, -iSP);
					}
				}
			}

			if (pAttacker->GetPoint(POINT_STEAL_GOLD))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_STEAL_GOLD))
				{
					int iAmount = number(1, GetLevel());
					pAttacker->PointChange(POINT_GOLD, iAmount);
					DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 1, iAmount);
				}
			}

			if (pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP>=0)?MIN(dam, iCurHP):dam) * pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_HP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_HP, i);
				}
			}

			if (pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP>=0)?MIN(dam, iCurHP):dam) * pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_SP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_SP, i);
				}
			}

			if (pAttacker->GetPoint(POINT_MANA_BURN_PCT))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_MANA_BURN_PCT))
					PointChange(POINT_SP, -50);
			}
		}
	}

	switch (type)
	{
		case DAMAGE_TYPE_NORMAL:
		case DAMAGE_TYPE_NORMAL_RANGE:
			if (pAttacker)
				if (pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS))
					dam = dam * (100 + pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS)) / 100;

			dam = dam * (100 - MIN(99, GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS))) / 100;
			break;

		case DAMAGE_TYPE_MELEE:
		case DAMAGE_TYPE_RANGE:
		case DAMAGE_TYPE_FIRE:
		case DAMAGE_TYPE_ICE:
		case DAMAGE_TYPE_ELEC:
		case DAMAGE_TYPE_MAGIC:
			if (pAttacker)
				if (pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS))
					dam = dam * (100 + pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS)) / 100;

			dam = dam * (100 - MIN(99, GetPoint(POINT_SKILL_DEFEND_BONUS))) / 100;
			break;

		default:
			break;
	}

	if (IsAffectFlag(AFF_MANASHIELD))
	{
		int iDamageSPPart = dam / 3;
		int iDamageToSP = iDamageSPPart * GetPoint(POINT_MANASHIELD) / 100;
		int iSP = GetSP();

		if (iDamageToSP <= iSP)
		{
			PointChange(POINT_SP, -iDamageToSP);
			dam -= iDamageSPPart;
		}
		else
		{
			PointChange(POINT_SP, -GetSP());
			dam -= iSP * 100 / MAX(GetPoint(POINT_MANASHIELD), 1);
		}
	}

	if (GetPoint(POINT_MALL_DEFBONUS) > 0)
	{
		int dec_dam = MIN(200, dam * GetPoint(POINT_MALL_DEFBONUS) / 100);
		dam -= dec_dam;
	}

	if (pAttacker)
	{
		if (pAttacker->GetPoint(POINT_MALL_ATTBONUS) > 0)
		{
			int add_dam = MIN(300, dam * pAttacker->GetLimitPoint(POINT_MALL_ATTBONUS) / 100);
			dam += add_dam;
		}

		if (pAttacker->IsPC() && IsPC())
		{
			// if (type == DAMAGE_TYPE_NORMAL)
			// {
			// 	pAttacker->ChatPacket(CHAT_TYPE_INFO, "test");
			// 	pAttacker->ChatPacket(CHAT_TYPE_INFO, "damage: %d", dam / 8);
			// }

			int iEmpire = pAttacker->GetEmpire();
			long lMapIndex = pAttacker->GetMapIndex();
			int iMapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex);

			if (iEmpire && iMapEmpire && iEmpire != iMapEmpire)
			{
				dam = dam * 9 / 10;
			}
			
			if (!IsPC() && GetMonsterDrainSPPoint())
			{
				int iDrain = GetMonsterDrainSPPoint();

				if (iDrain <= pAttacker->GetSP())
					pAttacker->PointChange(POINT_SP, -iDrain);
				else
				{
					int iSP = pAttacker->GetSP();
					pAttacker->PointChange(POINT_SP, -iSP);
				}
			}

		}
		else if (pAttacker->IsGuardNPC())
		{
			SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD);
			Stun();
			return true;
		}

		if (pAttacker->IsPC() && CMonarch::instance().IsPowerUp(pAttacker->GetEmpire()))
		{
			dam += dam / 10;
		}

		if (IsPC() && CMonarch::instance().IsDefenceUp(GetEmpire()))
		{
			dam -= dam / 10;
		}
	}

	if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		return false;

	if (!IsPC())
	{
		if (m_pkParty && m_pkParty->GetLeader())
			m_pkParty->GetLeader()->SetLastAttacked(get_dword_time());
		else
			SetLastAttacked(get_dword_time());

		MonsterChat(MONSTER_CHAT_ATTACKED);
	}

	if (IsStun())
	{
		Dead(pAttacker);
		return true;
	}

	if (IsDead())
		return true;

	/*/
	if (type == DAMAGE_TYPE_POISON)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP() - 1;
		}
	}
#ifdef ENABLE_WOLFMAN_CHARACTER
	else if (type == DAMAGE_TYPE_BLEEDING)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP();
		}
	}
#endif
	/*/
	// ------------------------

	// -----------------------
	// if (pAttacker && pAttacker->IsPC())
	// {
	// 	int iDmgPct = CHARACTER_MANAGER::instance().GetUserDamageRate(pAttacker);
	// 	dam = dam * iDmgPct / 100;
	// }
	if (IsMonster() && IsStoneSkinner())
	{
		
		if (GetHPPct() < GetMobTable().bStoneSkinPoint) {
			dam /= 2;
		}
	}

	if (pAttacker)
	{
		if (pAttacker->IsMonster() && pAttacker->IsDeathBlower())
		{
			if (pAttacker->IsDeathBlow())
			{
				if (number(JOB_WARRIOR, JOB_MAX_NUM-1) == GetJob()) // @fixme192 (1, 4)
				{
					IsDeathBlow = true;
					dam = dam * 4;
				}
			}
		}

		if (GetRaceNum() == 2493)
			dam = BlueDragon_Damage(this, pAttacker, dam);

		if (pAttacker->GetPoint(POINT_FINAL_DMG_BONUS) > 0)
		{
			int add_dam = ((dam * (100 + pAttacker->GetPoint(POINT_FINAL_DMG_BONUS))) / 100) - dam;
			dam += add_dam;
		}

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		if (IsLegenda())
		{
			int iPulse = thecore_pulse();
			if (!(iPulse - GetLegendLastDamageInfo() < PASSES_PER_SEC(1)))
			{
				SetLegendLastDamageInfo();

				for (auto it = m_map_kDamageLegend.begin(); it != m_map_kDamageLegend.end(); )
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->second.dwVID);
					if (!ch)
						it = m_map_kDamageLegend.erase(it);
					else
						++it;
				}

				std::vector<std::pair<std::string, TBattleInfoLegend>> vec(m_map_kDamageLegend.begin(), m_map_kDamageLegend.end());

				std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
					return a.second.iTotalDamage > b.second.iTotalDamage;
					});

				TPacketGCSendLegendDamage damagePacket;
				damagePacket.bHeader = HEADER_GC_LEGEND_DAMAGE_DATA;
				damagePacket.dwBossVID = GetVID();
				(GetVictim()) ? strncpy(damagePacket.cTarget, GetVictim()->GetName(), sizeof(damagePacket.cTarget)) : strncpy(damagePacket.cTarget, "", sizeof(damagePacket.cTarget));
				
				int lRecord = 0;
				for (const auto& pair : vec)
				{
					damagePacket.legendDamageData[lRecord] = TPacketGCTableLegendDamage();
					strncpy(damagePacket.legendDamageData[lRecord].cName, pair.second.szName, sizeof(damagePacket.legendDamageData[lRecord].cName));
					damagePacket.legendDamageData[lRecord].iLevel = pair.second.iLevel;
					damagePacket.legendDamageData[lRecord].iRace = pair.second.iRace;
					damagePacket.legendDamageData[lRecord].iEmpire = pair.second.iEmpire;
					damagePacket.legendDamageData[lRecord].iDamage = pair.second.iTotalDamage;

					++lRecord;
				}

				if (lRecord < LEGEND_DAMAGE_BOARD_MAX_ENTRIES)
				{
					while (lRecord < LEGEND_DAMAGE_BOARD_MAX_ENTRIES)
					{
						damagePacket.legendDamageData[lRecord] = TPacketGCTableLegendDamage();
						strncpy(damagePacket.legendDamageData[lRecord].cName, "", sizeof(damagePacket.legendDamageData[lRecord].cName));
						damagePacket.legendDamageData[lRecord].iLevel = 0;
						damagePacket.legendDamageData[lRecord].iRace = 0;
						damagePacket.legendDamageData[lRecord].iEmpire = 0;
						damagePacket.legendDamageData[lRecord].iDamage = 0;
						lRecord++;
					}
				}

				PacketAround(&damagePacket, sizeof(damagePacket), this);
			}
		}
#endif

		BYTE damageFlag = 0;

		if (type == DAMAGE_TYPE_POISON)
			damageFlag = DAMAGE_POISON;
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
		else if (type == DAMAGE_TYPE_BLEEDING)
			damageFlag = DAMAGE_BLEEDING;
#elif defined(ENABLE_WOLFMAN_CHARACTER) && defined(USE_MOB_BLEEDING_AS_POISON)
		else if (type == DAMAGE_TYPE_BLEEDING)
			damageFlag = DAMAGE_POISON;
#endif
		else
			damageFlag = DAMAGE_NORMAL;

		if (IsCritical == true)
			damageFlag |= DAMAGE_CRITICAL;

		if (IsPenetrate == true)
			damageFlag |= DAMAGE_PENETRATE;

		float damMul = this->GetDamMul();
		float tempDam = dam;
		dam = tempDam * damMul + 0.5f;

		if (pAttacker)
			SendDamagePacket(pAttacker, dam, damageFlag);

		if (test_server)
		{
			int iTmpPercent = 0; // @fixme136
			if (GetMaxHP() >= 0)
				iTmpPercent = (GetHP() * 100) / GetMaxHP();

			if(pAttacker)
			{
				pAttacker->ChatPacket(CHAT_TYPE_INFO, "-> %s, DAM %d HP %lld(%d%%) %s%s",
						GetName(),
						dam,
						GetHP(),
						iTmpPercent,
						IsCritical ? "crit " : "",
						IsPenetrate ? "pene " : "",
						IsDeathBlow ? "deathblow " : "");
			}

			ChatPacket(CHAT_TYPE_PARTY, "<- %s, DAM %d HP %lld(%d%%) %s%s",
					pAttacker ? pAttacker->GetName() : 0,
					dam,
					GetHP(),
					iTmpPercent,
					IsCritical ? "crit " : "",
					IsPenetrate ? "pene " : "",
					IsDeathBlow ? "deathblow " : "");
		}

		if (m_bDetailLog)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s[%d]�� ���� ��ġ: %d %d"), pAttacker->GetName(), (DWORD) pAttacker->GetVID(), pAttacker->GetX(), pAttacker->GetY());
		}
	}

	if (!cannot_dead)
	{
#if defined(__DUNGEON_INFO_SYSTEM__)
		if (pAttacker)
			pAttacker->SetLastDamage(dam);
#endif

		if (GetHP() - dam <= 0) // @fixme137
			dam = GetHP();

		PointChange(POINT_HP, -dam, false);
	}

	if (pAttacker && dam > 0 && IsNPC())
	{
		TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

		if (it == m_map_kDamage.end())
		{
			m_map_kDamage.emplace(pAttacker->GetVID(), TBattleInfo(dam, 0));
			it = m_map_kDamage.find(pAttacker->GetVID());
		}
		else
		{
			it->second.iTotalDamage += dam;
		}

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
		TDamageMapLegend::iterator itL = m_map_kDamageLegend.find(pAttacker->GetName());
		if (itL == m_map_kDamageLegend.end())
		{
			m_map_kDamageLegend.insert(TDamageMapLegend::value_type(pAttacker->GetName(), TBattleInfoLegend(pAttacker->GetVID(), pAttacker->GetName(), pAttacker->GetLevel(), pAttacker->GetRaceNum(), pAttacker->GetEmpire(), dam, 0)));
			itL = m_map_kDamageLegend.find(pAttacker->GetName());
		}
		else
		{
			itL->second.iTotalDamage += dam;
		}
#endif

		StartRecoveryEvent();
		UpdateAggrPointEx(pAttacker, type, dam, it->second);
	}

	if (GetHP() <= 0)
	{
		if (pAttacker && !pAttacker->IsNPC())
			m_dwKillerPID = pAttacker->GetPlayerID();
		else
			m_dwKillerPID = 0;

		Stun(true);
	}
	
	return false;
}

void CHARACTER::DistributeHP(LPCHARACTER pkKiller)
{
	if (pkKiller->GetDungeon())
		return;
}

static bool CanGetExp(DWORD pLevel, DWORD pMapIndex)
{
	switch (pMapIndex)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		{
			if (pLevel >= 30)
				return false;
		}
		break;
		
		case 6:
		{
			if (pLevel >= 40)
				return false;
		}
		break;
		
		case 7:
		case 22:
		case 23:
		{
			if (pLevel >= 75)
				return false;
		}
		break;
		
		case 8:
		{
			if (pLevel >= 100)
				return false;
		}
		break;
		
		case 9:
		{
			if (pLevel >= 125)
				return false;
		}
		break;
		
		case 10:
		{
			if (pLevel >= 150)
				return false;
		}
		break;
		
		case 20:
		{
			if (pLevel >= 175)
				return false;
		}
		break;
	}

	return true;
}

#ifdef ENABLE_NEWEXP_CALCULATION
//#define NEW_GET_LVDELTA(me, victim) aiPercentByDeltaLev[MINMAX(0, (victim + 30) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define NEW_GET_LVDELTA(me, victim)	100
typedef long double rate_t;
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	if (test_server && iExp < 0)
	{
		to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) overflow", iExp);
		return;
	}
	
	if (!CanGetExp(to->GetLevel(), to->GetMapIndex()))
	{
		if (test_server) to->ChatPacket(CHAT_TYPE_INFO, "<test_server> blokada expa - zbyt wysoki lvl");
		return;
	}
	
	// decrease/increase exp based on player<>mob level
	rate_t lvFactor = static_cast<rate_t>(NEW_GET_LVDELTA(to->GetLevel(), from->GetLevel())) / 100.0L;
	iExp *= lvFactor;

#ifdef ENABLE_NEW_PET_SYSTEM
	if (to->PetGetLevel() < 40) {
		to->PetSetExp(iExp);
	}
#endif

	// start calculating rate exp bonus
	int iBaseExp = iExp;

	rate_t rateFactor = 100;
	if (distribution_test_server)
		rateFactor *= 3;

	rateFactor += CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT);
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
		rateFactor += 20;
	if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
		rateFactor += 20;
	if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
		if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			rateFactor += 30;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
		rateFactor += 50;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP2))
		rateFactor += 30;
	if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP3))
		rateFactor += 20;

	switch (to->GetMountVnum())
	{
		case 20110:
		case 20111:
		case 20112:
		case 20113:
			if (to->IsEquipUniqueItem(71115) || to->IsEquipUniqueItem(71117) || to->IsEquipUniqueItem(71119) ||
					to->IsEquipUniqueItem(71121) )
			{
				rateFactor += 10;
			}
			break;

		case 20114:
		case 20120:
		case 20121:
		case 20122:
		case 20123:
		case 20124:
		case 20125:
			rateFactor += 30;
			break;
	}

	if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		rateFactor += 50;
	if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP))
		rateFactor += 50;
	rateFactor += to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS);
	rateFactor += to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP);
	rateFactor += to->GetPoint(POINT_MALL_EXPBONUS);
	rateFactor += to->GetPoint(POINT_EXP);
	// useless (never used except for china intoxication) = always 100
	rateFactor = rateFactor * static_cast<rate_t>(CHARACTER_MANAGER::instance().GetMobExpRate(to))/100.0L;
	// fix underflow formula
	iExp = std::max<int>(0, iExp);
	rateFactor = std::max<rate_t>(100.0L, rateFactor);
	// apply calculated rate bonus
	iExp *= (rateFactor/100.0L);
	auto iOldExp = iExp;
	// you can get at maximum only 10% of the total required exp at once (so, you need to kill at least 10 mobs to level up) (useless)
	iExp = MIN(to->GetNextExp() / 10, iExp);
	// it recalculate the given exp if the player level is greater than the exp_table size (useless)
	iExp = AdjustExpByLevel(to, iExp);
	if (test_server)
		to->ChatPacket(CHAT_TYPE_INFO, "base_exp(%d) * rate(%Lf) = exp(%d) => exp+minGNE+adjust(%d)", iBaseExp, rateFactor/100.0L, iOldExp, iExp);
	// set
	to->PointChange(POINT_EXP, iExp, true);
	from->CreateFly(FLY_EXP, to);

	// marriage
	{
		LPCHARACTER you = to->GetMarryPartner();
		if (you)
		{
			// sometimes, this overflows
			DWORD dwUpdatePoint = (2000.0L/to->GetLevel()/to->GetLevel()/3)*iExp;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint *= 3;

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}
}
#else
static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	iExp = CALCULATE_VALUE_LVDELTA(to->GetLevel(), from->GetLevel(), iExp);

	if (distribution_test_server)
		iExp *= 3;

	int iBaseExp = iExp;

	// iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;

#ifdef ENABLE_EVENT_MANAGER
	const auto event = CHARACTER_MANAGER::Instance().CheckEventIsActive(EXP_EVENT, to->GetEmpire());
	if(event != 0)
		iExp = iExp * (100 + (event->value[0] + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT))) / 100;
	else
		iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#else
	iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
#endif

	{
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
			iExp += iExp * 20 /100;

		if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
			iExp += iExp * 20 / 100;

		if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
				iExp += iExp * 30 / 100;

		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
			iExp += iExp * 50 / 100;
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP2))
			iExp += iExp * 30 / 100;
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP3))
			iExp += iExp * 20 / 100;

		switch (to->GetMountVnum())
		{
			case 20110:
			case 20111:
			case 20112:
			case 20113:
				if (to->IsEquipUniqueItem(71115) || to->IsEquipUniqueItem(71117) || to->IsEquipUniqueItem(71119) ||
						to->IsEquipUniqueItem(71121) )
				{
					iExp += iExp * 10 / 100;
				}
				break;

			case 20114:
			case 20120:
			case 20121:
			case 20122:
			case 20123:
			case 20124:
			case 20125:

				iExp += iExp * 30 / 100;
				break;
		}
	}

	{
		if (to->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		{
			iExp += (iExp * 50 / 100);
		}

		if (to->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_EXP) == true)
		{
			iExp += (iExp * 50 / 100);
		}

		iExp += iExp * to->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_EXP_BONUS) / 100;
	}

	iExp += (iExp * to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP)/100);
	iExp += (iExp * to->GetPoint(POINT_MALL_EXPBONUS)/100);
	iExp += (iExp * to->GetPoint(POINT_EXP)/100);

	if (test_server)
	{
		sys_log(0, "Bonus Exp : Ramadan Candy: %d MallExp: %d PointExp: %d",
				to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP),
				to->GetPoint(POINT_MALL_EXPBONUS),
				to->GetPoint(POINT_EXP)
			   );
	}

	iExp = iExp * CHARACTER_MANAGER::instance().GetMobExpRate(to) / 100;

	iExp = MIN(to->GetNextExp() / 10, iExp);

	if (test_server)
	{
		if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && iBaseExp>0)
			to->ChatPacket(CHAT_TYPE_INFO, "exp bonus %d%%", (iExp-iBaseExp)*100/iBaseExp);
		to->ChatPacket(CHAT_TYPE_INFO, "exp(%d) base_exp(%d)", iExp, iBaseExp);
	}

	iExp = AdjustExpByLevel(to, iExp);

	to->PointChange(POINT_EXP, iExp, true);
	from->CreateFly(FLY_EXP, to);

	{
		LPCHARACTER you = to->GetMarryPartner();

		if (you)
		{
			DWORD dwUpdatePoint = 2000*iExp/to->GetLevel()/to->GetLevel()/3;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint = (DWORD)(dwUpdatePoint * 3);

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}
}
#endif

namespace NPartyExpDistribute
{
	struct FPartyTotaler
	{
		int		total;
		int		member_count;
		int		x, y;

		FPartyTotaler(LPCHARACTER center)
			: total(0), member_count(0), x(center->GetX()), y(center->GetY())
		{};

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				total += __GetPartyExpNP(ch->GetLevel());

				++member_count;
			}
		}
	};

	struct FPartyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		DWORD		_iExp;
		int		m_iMode;
		int		m_iMemberCount;

		FPartyDistributor(LPCHARACTER center, int member_count, int total, DWORD iExp, int iMode)
			: total(total), c(center), x(center->GetX()), y(center->GetY()), _iExp(iExp), m_iMode(iMode), m_iMemberCount(member_count)
			{
				if (m_iMemberCount == 0)
					m_iMemberCount = 1;
			};

		void operator () (LPCHARACTER ch)
		{
			if (!ch || (ch->GetMapIndex() != c->GetMapIndex()))
				return;

			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				DWORD iExp2 = 0;

				switch (m_iMode)
				{
					case PARTY_EXP_DISTRIBUTION_NON_PARITY:
						iExp2 = (DWORD) (_iExp * (float) __GetPartyExpNP(ch->GetLevel()) / total);
						break;

					case PARTY_EXP_DISTRIBUTION_PARITY:
						iExp2 = _iExp / m_iMemberCount;
						break;

					default:
						sys_err("Unknown party exp distribution mode %d", m_iMode);
						return;
				}

				GiveExp(c, ch, iExp2);
			}
		}
	};
}

typedef struct SDamageInfo
{
	int iDam;
	LPCHARACTER pAttacker;
	LPPARTY pParty;

	void Clear()
	{
		pAttacker = NULL;
		pParty = NULL;
	}

	inline void Distribute(LPCHARACTER ch, int iExp)
	{
		if (pAttacker)
			GiveExp(ch, pAttacker, iExp);
		else if (pParty)
		{
			NPartyExpDistribute::FPartyTotaler f(ch);
			pParty->ForEachOnlineMember(f);

			if (pParty->IsPositionNearLeader(ch))
				iExp = iExp * (100 + pParty->GetExpBonusPercent()) / 100;

			if (test_server)
			{
				if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && pParty->GetExpBonusPercent())
					pParty->ChatPacketToAllMember(CHAT_TYPE_INFO, "exp party bonus %d%%", pParty->GetExpBonusPercent());
			}

			if (pParty->GetExpCentralizeCharacter())
			{
				LPCHARACTER tch = pParty->GetExpCentralizeCharacter();

				if (DISTANCE_APPROX(ch->GetX() - tch->GetX(), ch->GetY() - tch->GetY()) <= PARTY_DEFAULT_RANGE)
				{
					int iExpCenteralize = (int) (iExp * 0.05f);
					iExp -= iExpCenteralize;

					GiveExp(ch, pParty->GetExpCentralizeCharacter(), iExpCenteralize);
				}
			}

			NPartyExpDistribute::FPartyDistributor fDist(ch, f.member_count, f.total, iExp, pParty->GetExpDistributionMode());
			pParty->ForEachOnlineMember(fDist);
		}
	}
} TDamageInfo;

#ifdef ENABLE_KILL_EVENT_FIX
LPCHARACTER CHARACTER::GetMostAttacked() {
	int iMostDam=-1;
	LPCHARACTER pkChrMostAttacked = NULL;
	auto it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end()){
		//* getting information from the iterator
		const VID & c_VID = it->first;
		const int iDam    = it->second.iTotalDamage;

		//* increasing the iterator
		++it;

		//* finding the character from his vid
		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		//* if the attacked is now offline
		if (!pAttacker)
			continue;

		//* if the attacker is not a player
		if( pAttacker->IsNPC())
			continue;

		//* if the player is too far
		if(DISTANCE_APPROX(GetX()-pAttacker->GetX(), GetY()-pAttacker->GetY())>5000)
			continue;

		if (iDam > iMostDam){
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}
	}

	return pkChrMostAttacked;
}
#endif

LPCHARACTER CHARACTER::DistributeExp()
{
	int iExpToDistribute = GetExp();

	if (iExpToDistribute <= 0)
		return NULL;

	int	iTotalDam = 0;
	LPCHARACTER pkChrMostAttacked = NULL;
	int iMostDam = 0;

	typedef std::vector<TDamageInfo> TDamageInfoTable;
	TDamageInfoTable damage_info_table;
	std::map<LPPARTY, TDamageInfo> map_party_damage;

	damage_info_table.reserve(m_map_kDamage.size());

	TDamageMap::iterator it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		int iDam = it->second.iTotalDamage;

		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker || pAttacker->IsNPC() || DISTANCE_APPROX(GetX()-pAttacker->GetX(), GetY()-pAttacker->GetY())>5000)
			continue;

		iTotalDam += iDam;
		if (!pkChrMostAttacked || iDam > iMostDam)
		{
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}

		if (pAttacker->GetParty())
		{
			std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.find(pAttacker->GetParty());
			if (it == map_party_damage.end())
			{
				TDamageInfo di;
				di.iDam = iDam;
				di.pAttacker = NULL;
				di.pParty = pAttacker->GetParty();
				map_party_damage.emplace(di.pParty, di);
			}
			else
			{
				it->second.iDam += iDam;
			}
		}
		else
		{
			TDamageInfo di;

			di.iDam = iDam;
			di.pAttacker = pAttacker;
			di.pParty = NULL;

			//sys_log(0, "__ pq_damage %s %d", pAttacker->GetName(), iDam);
			//pq_damage.push(di);
			damage_info_table.emplace_back(di);
		}
	}

	for (std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.begin(); it != map_party_damage.end(); ++it)
	{
		damage_info_table.emplace_back(it->second);
		//sys_log(0, "__ pq_damage_party [%u] %d", it->second.pParty->GetLeaderPID(), it->second.iDam);
	}

	SetExp(0);
	//m_map_kDamage.clear();

	if (iTotalDam == 0)
		return NULL;

	if (m_pkChrStone)
	{
		//sys_log(0, "__ Give half to Stone : %d", iExpToDistribute>>1);
		int iExp = iExpToDistribute >> 1;
		m_pkChrStone->SetExp(m_pkChrStone->GetExp() + iExp);
		iExpToDistribute -= iExp;
	}

	sys_log(1, "%s total exp: %d, damage_info_table.size() == %d, TotalDam %d",
			GetName(), iExpToDistribute, damage_info_table.size(), iTotalDam);
	//sys_log(1, "%s total exp: %d, pq_damage.size() == %d, TotalDam %d",
	//GetName(), iExpToDistribute, pq_damage.size(), iTotalDam);

	if (damage_info_table.empty())
		return NULL;

	DistributeHP(pkChrMostAttacked);

	{
		TDamageInfoTable::iterator di = damage_info_table.begin();
		{
			TDamageInfoTable::iterator it;

			for (it = damage_info_table.begin(); it != damage_info_table.end();++it)
			{
				if (it->iDam > di->iDam)
					di = it;
			}
		}

		int	iExp = iExpToDistribute / 5;
		iExpToDistribute -= iExp;

		float fPercent = (float) di->iDam / iTotalDam;

		if (fPercent > 1.0f)
		{
			sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di->pAttacker->GetName());
			fPercent = 1.0f;
		}

		iExp += (int) (iExpToDistribute * fPercent);

		//sys_log(0, "%s given exp percent %.1f + 20 dam %d", GetName(), fPercent * 100.0f, di.iDam);

		di->Distribute(this, iExp);

		if (fPercent == 1.0f)
			return pkChrMostAttacked;

		di->Clear();
	}

	{
		TDamageInfoTable::iterator it;

		for (it = damage_info_table.begin(); it != damage_info_table.end(); ++it)
		{
			TDamageInfo & di = *it;

			float fPercent = (float) di.iDam / iTotalDam;

			if (fPercent > 1.0f)
			{
				sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di.pAttacker->GetName());
				fPercent = 1.0f;
			}

			//sys_log(0, "%s given exp percent %.1f dam %d", GetName(), fPercent * 100.0f, di.iDam);
			di.Distribute(this, (int) (iExpToDistribute * fPercent));
		}
	}

	return pkChrMostAttacked;
}

int CHARACTER::GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount/* = 1 */)
{
	LPITEM pkBow;

	if (!(pkBow = GetWear(WEAR_WEAPON)) || pkBow->GetProto()->bSubType != WEAPON_BOW)
	{
		return 0;
	}

	LPITEM pkArrow;

	if (!(pkArrow = GetWear(WEAR_ARROW)) || pkArrow->GetType() != ITEM_WEAPON ||
			(pkArrow->GetProto()->bSubType != WEAPON_ARROW
				#ifdef ENABLE_QUIVER_SYSTEM
				&& pkArrow->GetSubType() != WEAPON_QUIVER
				#endif
			)
		)
	{
		return 0;
	}

	iArrowCount = MIN(iArrowCount, pkArrow->GetCount());
#ifdef ENABLE_QUIVER_SYSTEM
	if (pkArrow->GetSubType() == WEAPON_QUIVER && pkArrow->GetRealUseLimit()) // no arrows if expired - 1 otherwise
		iArrowCount = 1;
#endif

	*ppkBow = pkBow;
	*ppkArrow = pkArrow;

	return iArrowCount;
}

void CHARACTER::UseArrow(LPITEM pkArrow, DWORD dwArrowCount)
{
#ifdef ENABLE_QUIVER_SYSTEM
	if (pkArrow->GetSubType() == WEAPON_QUIVER)
		return;
#endif

	int iCount = pkArrow->GetCount();
	DWORD dwVnum = pkArrow->GetVnum();
	// iCount = iCount - MIN(iCount, dwArrowCount);
	pkArrow->SetCount(iCount);

	if (iCount == 0)
	{
		LPITEM pkNewArrow = FindSpecifyItem(dwVnum);

		sys_log(0, "UseArrow : FindSpecifyItem %u %p", dwVnum, get_pointer(pkNewArrow));

		if (pkNewArrow)
			EquipItem(pkNewArrow);
	}
}

void CHARACTER::SetLastSuccessShootTime(DWORD value)
{
	dwLastSuccessShootTime = value;
}

DWORD CHARACTER::GetLastSuccessShootTime() const
{
	return dwLastSuccessShootTime;
}


class CFuncShoot
{
	public:
		LPCHARACTER	m_me;
		BYTE		m_bType;
		bool		m_bSucceed;

		CFuncShoot(LPCHARACTER ch, BYTE bType) : m_me(ch), m_bType(bType), m_bSucceed(FALSE)
		{
		}

		void operator () (DWORD dwTargetVID)
		{
			if (m_bType > 1)
			{
				if (g_bSkillDisable)
					return;

				m_me->m_SkillUseInfo[m_bType].SetMainTargetVID(dwTargetVID);
			}

			LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);

			if (!pkVictim)
				return;

			if (!battle_is_attackable(m_me, pkVictim))
				return;

			if (m_me->IsNPC())
			{
				if (DISTANCE_APPROX(m_me->GetX() - pkVictim->GetX(), m_me->GetY() - pkVictim->GetY()) > 5000)
					return;
			}

			#ifdef ENABLE_SKILL_COOLDOWN_CHECK
			if (m_me->IsPC() && m_bType > 0 && m_me->SkillIsOnCooldown(m_bType))
				return;
			#endif

			LPITEM pkBow, pkArrow;

			switch (m_bType)
			{
				case 0:
					{
						int iDam = 0;

						if (m_me->IsPC())
						{
							if (m_me->GetJob() != JOB_ASSASSIN)
								return;

							if (0 == m_me->GetArrowAndBow(&pkBow, &pkArrow))
								return;

							if (m_me->GetSkillGroup() != 0)
								if (!m_me->IsNPC() && m_me->GetSkillGroup() != 2)
								{
									if (m_me->GetSP() < 5)
										return;

									m_me->PointChange(POINT_SP, -5);
								}

							iDam = CalcArrowDamage(m_me, pkVictim, pkBow, pkArrow);
							m_me->UseArrow(pkArrow, 1);

							// check speed hack
							DWORD	dwCurrentTime	= get_dword_time();
							if (IS_SPEED_HACK(m_me, pkVictim, dwCurrentTime))
								iDam	= 0;

							//m_me->ChatPacket(CHAT_TYPE_INFO, "dwCurrentTime: %d, dwLastSuccessShootTime: %d final_time: %d GET_ATTACK_SPEED: %d", dwCurrentTime, m_me->GetLastSuccessShootTime(), dwCurrentTime - m_me->GetLastSuccessShootTime(), GET_ATTACK_SPEED(m_me));
							if (m_me->IsPolymorphed()) {
								int distanceX = std::abs(m_me->lastAttackPosX - pkVictim->GetX());
								int32_t distanceY = std::abs(m_me->lastAttackPosY - pkVictim->GetY());
								int32_t distance = CalcDistance(distanceX, distanceY);
								float rangeLimit = 10;
								if (distance > rangeLimit) {
									return;
								}
							} else {
								if ((dwCurrentTime - m_me->GetLastSuccessShootTime()) < 480) {
									return;
								}
							}
						}
						else
							iDam = CalcMeleeDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

						iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BOW)) / 100;

						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_NORMAL_RANGE);

						if (iDam > 0)
							m_me->SetLastSuccessShootTime(get_dword_time());
					}
					break;

				case 1:
					{
						int iDam;

						if (m_me->IsPC())
							return;

						iDam = CalcMagicDamage(m_me, pkVictim);

						NormalAttackAffect(m_me, pkVictim);

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
						const int resist_magic = MINMAX(0, pkVictim->GetPoint(POINT_RESIST_MAGIC), 100);
						const int resist_magic_reduction = MINMAX(0, (m_me->GetJob()==JOB_SURA) ? m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION)/2 : m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION), 50);
						const int total_res_magic = MINMAX(0, resist_magic - resist_magic_reduction, 100);
						iDam = iDam * (100 - total_res_magic) / 100;
#else
						iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_MAGIC)) / 100;
#endif

						//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_MAGIC);

					}
					break;

				case SKILL_YEONSA:
					{
						//int iUseArrow = 2 + (m_me->GetSkillPower(SKILL_YEONSA) *6/100);
						int iUseArrow = 1;

						{
							if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
							{
								m_me->OnMove(true);
								pkVictim->OnMove();

								if (pkVictim->CanBeginFight())
									pkVictim->BeginFight(m_me);

								m_me->ComputeSkill(m_bType, pkVictim);
								m_me->UseArrow(pkArrow, iUseArrow);

								if (pkVictim->IsDead())
									break;

							}
							else
								break;
						}
					}
					break;

				case SKILL_KWANKYEOK:
					{
						int iUseArrow = 1;

						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s kwankeyok %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}
					break;

				case SKILL_GIGUNG:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s gigung %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;
				case SKILL_HWAJO:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s hwajo %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_HORSE_WILDATTACK_RANGE:
					{
						int iUseArrow = 1;
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							sys_log(0, "%s horse_wildattack %s", m_me->GetName(), pkVictim->GetName());
							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);
						}
					}

					break;

				case SKILL_PAERYONG:
				case SKILL_MARYUNG:
				case SKILL_TUSOK:
				case SKILL_BIPABU:
				case SKILL_NOEJEON:
				case SKILL_GEOMPUNG:
				case SKILL_SANGONG:
				case SKILL_MAHWAN:
				case SKILL_PABEOB:
				case SKILL_YONGBI:
				case SKILL_BYEURAK:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
					}
					break;

				case SKILL_CHAIN:
					{
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);

					}
					break;

				// case SKILL_YONGBI:
				// 	{
				// 		m_me->OnMove(true);
				// 	}
				// 	break;

				default:
					sys_err("CFuncShoot: I don't know this type [%d] of range attack.", (int) m_bType);
					break;
			}

			m_bSucceed = TRUE;
		}
};

bool CHARACTER::Shoot(BYTE bType)
{
	sys_log(1, "Shoot %s type %u flyTargets.size %zu", GetName(), bType, m_vec_dwFlyTargets.size());

	if (!CanMove())
	{
		return false;
	}

	CFuncShoot f(this, bType);

	if (m_dwFlyTargetID != 0)
	{
		f(m_dwFlyTargetID);
		m_dwFlyTargetID = 0;
	}

	f = std::for_each(m_vec_dwFlyTargets.begin(), m_vec_dwFlyTargets.end(), f);
	m_vec_dwFlyTargets.clear();

	return f.m_bSucceed;
}

void CHARACTER::FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader)
{
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);
	TPacketGCFlyTargeting pack;

	//pack.bHeader	= HEADER_GC_FLY_TARGETING;
	pack.bHeader	= (bHeader == HEADER_CG_FLY_TARGETING) ? HEADER_GC_FLY_TARGETING : HEADER_GC_ADD_FLY_TARGETING;
	pack.dwShooterVID	= GetVID();

	if (pkVictim)
	{
		pack.dwTargetVID = pkVictim->GetVID();
		pack.x = pkVictim->GetX();
		pack.y = pkVictim->GetY();

		if (bHeader == HEADER_CG_FLY_TARGETING)
			m_dwFlyTargetID = dwTargetVID;
		else
			m_vec_dwFlyTargets.emplace_back(dwTargetVID);
	}
	else
	{
		pack.dwTargetVID = 0;
		pack.x = x;
		pack.y = y;
	}

	sys_log(1, "FlyTarget %s vid %d x %d y %d", GetName(), pack.dwTargetVID, pack.x, pack.y);
	PacketAround(&pack, sizeof(pack), this);
}

LPCHARACTER CHARACTER::GetNearestVictim(LPCHARACTER pkChr)
{
	if (NULL == pkChr)
		pkChr = this;

	float fMinDist = 99999.0f;
	LPCHARACTER pkVictim = NULL;

	TDamageMap::iterator it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker)
			continue;

		if (pAttacker->IsAffectFlag(AFF_EUNHYUNG) ||
				pAttacker->IsAffectFlag(AFF_INVISIBILITY) ||
				pAttacker->IsAffectFlag(AFF_REVIVE_INVISIBLE))
			continue;

		if (pAttacker->IsDead())
			continue;

		float fDist = DISTANCE_APPROX(pAttacker->GetX() - pkChr->GetX(), pAttacker->GetY() - pkChr->GetY());

		if (fDist < fMinDist)
		{
			pkVictim = pAttacker;
			fMinDist = fDist;
		}
	}

	return pkVictim;
}

void CHARACTER::SetVictim(LPCHARACTER pkVictim)
{
	if (!pkVictim)
	{
		if (0 != (DWORD)m_kVIDVictim)
			MonsterLog("���� ����� ����");

		m_kVIDVictim.Reset();
		battle_end(this);
	}
	else
	{
		if (m_kVIDVictim != pkVictim->GetVID())
			MonsterLog("���� ����� ����: %s", pkVictim->GetName());

		m_kVIDVictim = pkVictim->GetVID();
		m_dwLastVictimSetTime = get_dword_time();
	}
}

LPCHARACTER CHARACTER::GetVictim() const
{
	return CHARACTER_MANAGER::instance().Find(m_kVIDVictim);
}

LPCHARACTER CHARACTER::GetProtege() const
{
	if (m_pkChrStone)
		return m_pkChrStone;

	if (m_pkParty)
		return m_pkParty->GetLeader();

	return NULL;
}

long long CHARACTER::GetAlignment() const
{
	return m_iAlignment;
}

long long CHARACTER::GetRealAlignment() const
{
	return m_iRealAlignment;
}

void CHARACTER::ShowAlignment(bool bShow)
{
	if (bShow)
	{
		if (m_iAlignment != m_iRealAlignment)
		{
			m_iAlignment = m_iRealAlignment;
			UpdatePacket();
		}
	}
	else
	{
		if (m_iAlignment != 0)
		{
			m_iAlignment = 0;
			UpdatePacket();
		}
	}
}

void CHARACTER::UpdateAlignment(long long iAmount)
{
	bool bShow = false;

	if (m_iAlignment == m_iRealAlignment)
		bShow = true;

	long long i = m_iAlignment;

#ifdef ENABLE_ALIGN_RENEWAL
	long long iOldAlignment = m_iRealAlignment;
	m_iRealAlignment = MINMAX(0, m_iRealAlignment + iAmount, 500000);
	OnAlignUpdate(iOldAlignment);
#else
	m_iRealAlignment = MINMAX(-200000, m_iRealAlignment + iAmount, 200000);
#endif


	if (bShow)
	{
		m_iAlignment = m_iRealAlignment;

		if (i != m_iAlignment)
			UpdatePacket();
	}
}

void CHARACTER::SetKillerMode(bool isOn)
{
	if ((isOn ? ADD_CHARACTER_STATE_KILLER : 0) == IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER))
		return;

	if (isOn)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
	else
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);

	m_iKillerModePulse = thecore_pulse();
	UpdatePacket();
	sys_log(0, "SetKillerMode Update %s[%d]", GetName(), GetPlayerID());
}

bool CHARACTER::IsKillerMode() const
{
	return IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
}

void CHARACTER::UpdateKillerMode()
{
	if (!IsKillerMode())
		return;

	if (thecore_pulse() - m_iKillerModePulse >= PASSES_PER_SEC(30))
		SetKillerMode(false);
}

void CHARACTER::SetPKMode(BYTE bPKMode)
{
	if (bPKMode >= PK_MODE_MAX_NUM)
		return;

	if (m_bPKMode == bPKMode)
		return;

	if (bPKMode == PK_MODE_GUILD && !GetGuild())
		bPKMode = PK_MODE_FREE;

	m_bPKMode = bPKMode;
	UpdatePacket();

	sys_log(0, "PK_MODE: %s %d", GetName(), m_bPKMode);
}

BYTE CHARACTER::GetPKMode() const
{
	return m_bPKMode;
}

struct FuncForgetMyAttacker
{
	LPCHARACTER m_ch;

#ifdef __RESET_PLAYER_DAMAGE__
	bool _fullClear = false;
#endif

	FuncForgetMyAttacker(LPCHARACTER ch
#ifdef __RESET_PLAYER_DAMAGE__
		, bool fullClear_ = false
#endif
	)
	{
		m_ch = ch;
#ifdef __RESET_PLAYER_DAMAGE__
		_fullClear = fullClear_;
#endif
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (ch->m_kVIDVictim == m_ch->GetVID())
				ch->SetVictim(NULL);

#ifdef __RESET_PLAYER_DAMAGE__
			if (_fullClear)
				ch->ErasePlayerDamage(m_ch);
#endif
		}
	}
};

#ifdef ENABLE_VS_PELERYNKA_VIP
struct FuncAggregateMonster
{
    LPCHARACTER m_ch;
    BYTE m_type;

    FuncAggregateMonster(LPCHARACTER ch, BYTE type = 0) : m_ch(ch), m_type(type){}
    void operator () (LPENTITY ent)
    {
        if (!ent->IsType(ENTITY_CHARACTER))
            return;

        LPCHARACTER ch = (LPCHARACTER) ent;
        if (!ch || ch->IsPC() || !ch->IsMonster() || ch->GetVictim())
            return;

        int distance = DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY());
        int maxDistance = (m_type == 0) ? 5000 : (m_type == 1) ? 7500 : 5000;

		if (number(1, 100) <= 50)
			if (distance < maxDistance && ch->CanBeginFight())
				ch->BeginFight(m_ch);
	}
};
#else
struct FuncAggregateMonster
{
	LPCHARACTER m_ch;
	FuncAggregateMonster(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim())
				return;

			if (number(1, 100) <= 50)
				if (DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < 5000)
					if (ch->CanBeginFight())
						ch->BeginFight(m_ch);
		}
	}
};
#endif

struct FuncAttractRanger
{
	LPCHARACTER m_ch;
	FuncAttractRanger(LPCHARACTER ch)
	{
		m_ch = ch;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim() && ch->GetVictim() != m_ch)
				return;
			if (ch->GetMobAttackRange() > 150)
			{
				int iNewRange = 150;//(int)(ch->GetMobAttackRange() * 0.2);
				if (iNewRange < 150)
					iNewRange = 150;

				ch->AddAffect(AFFECT_BOW_DISTANCE, POINT_BOW_DISTANCE, iNewRange - ch->GetMobAttackRange(), AFF_NONE, 3*60, 0, false);
			}
		}
	}
};

struct FuncPullMonster
{
	LPCHARACTER m_ch;
	int m_iLength;
	FuncPullMonster(LPCHARACTER ch, int iLength = 300)
	{
		m_ch = ch;
		m_iLength = iLength;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;
			//if (ch->GetVictim() && ch->GetVictim() != m_ch)
			//return;
			float fDist = DISTANCE_APPROX(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
			if (fDist > 3000 || fDist < 100)
				return;

			float fNewDist = fDist - m_iLength;
			if (fNewDist < 100)
				fNewDist = 100;

			float degree = GetDegreeFromPositionXY(ch->GetX(), ch->GetY(), m_ch->GetX(), m_ch->GetY());
			float fx;
			float fy;

			GetDeltaByDegree(degree, fDist - fNewDist, &fx, &fy);
			long tx = (long)(ch->GetX() + fx);
			long ty = (long)(ch->GetY() + fy);

			ch->Sync(tx, ty);
			ch->Goto(tx, ty);
			ch->CalculateMoveDuration();

			ch->SyncPacket();
		}
	}
};

void CHARACTER::ForgetMyAttacker(
#ifdef __RESET_PLAYER_DAMAGE__
	bool fullClear_
#endif
)
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncForgetMyAttacker f(this
#ifdef __RESET_PLAYER_DAMAGE__
			, fullClear_
#endif		
		);
		pSec->ForEachAround(f);
	}
	ReviveInvisible(5);
}

#ifdef ENABLE_VS_PELERYNKA_VIP
void CHARACTER::AggregateMonster(BYTE type)
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this, type);
		pSec->ForEachAround(f);
	}
}
#else
void CHARACTER::AggregateMonster();
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this);
		pSec->ForEachAround(f);
	}
}
#endif

void CHARACTER::AttractRanger()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAttractRanger f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::PullMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncPullMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::UpdateAggrPointEx(LPCHARACTER pAttacker, EDamageType type, int dam, CHARACTER::TBattleInfo & info)
{
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL_RANGE:
			dam = (int) (dam*1.2f);
			break;

		case DAMAGE_TYPE_RANGE:
			dam = (int) (dam*1.5f);
			break;

		case DAMAGE_TYPE_MAGIC:
			dam = (int) (dam*1.2f);
			break;

		default:
			break;
	}

	if (pAttacker == GetVictim())
		dam = (int) (dam * 1.2f);

	info.iAggro += dam;

	if (info.iAggro < 0)
		info.iAggro = 0;

	//sys_log(0, "UpdateAggrPointEx for %s by %s dam %d total %d", GetName(), pAttacker->GetName(), dam, total);
	if (GetParty() && dam > 0 && type != DAMAGE_TYPE_SPECIAL)
	{
		LPPARTY pParty = GetParty();

		int iPartyAggroDist = dam;

		if (pParty->GetLeaderPID() == GetVID())
			iPartyAggroDist /= 2;
		else
			iPartyAggroDist /= 3;

		pParty->SendMessage(this, PM_AGGRO_INCREASE, iPartyAggroDist, pAttacker->GetVID());
	}

	ChangeVictimByAggro(info.iAggro, pAttacker);
}

void CHARACTER::UpdateAggrPoint(LPCHARACTER pAttacker, EDamageType type, int dam)
{
	if (IsDead() || IsStun())
		return;

	TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

	if (it == m_map_kDamage.end())
	{
		m_map_kDamage.emplace(pAttacker->GetVID(), TBattleInfo(0, dam));
		it = m_map_kDamage.find(pAttacker->GetVID());
	}

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	if (IsLegenda())
	{
		TDamageMapLegend::iterator itL = m_map_kDamageLegend.find(pAttacker->GetName());
		if (itL == m_map_kDamageLegend.end())
		{
			m_map_kDamageLegend.insert(TDamageMapLegend::value_type(pAttacker->GetName(), TBattleInfoLegend(pAttacker->GetVID(), pAttacker->GetName(), pAttacker->GetLevel(), pAttacker->GetRaceNum(), pAttacker->GetEmpire(), 0, dam)));
			itL = m_map_kDamageLegend.find(pAttacker->GetName());
		}
	}
#endif

	UpdateAggrPointEx(pAttacker, type, dam, it->second);
}

void CHARACTER::ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim)
{
	if (get_dword_time() - m_dwLastVictimSetTime < 3000)
		return;

	if (pNewVictim == GetVictim())
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			return;
		}

		TDamageMap::iterator it;
		TDamageMap::iterator itFind = m_map_kDamage.end();

		for (it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
		{
			if (it->second.iAggro > iNewAggro)
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

				if (ch && !ch->IsDead() && DISTANCE_APPROX(ch->GetX() - GetX(), ch->GetY() - GetY()) < 5000)
				{
					itFind = it;
					iNewAggro = it->second.iAggro;
				}
			}
		}

		if (itFind != m_map_kDamage.end())
		{
			m_iMaxAggro = iNewAggro;
			SetVictim(CHARACTER_MANAGER::instance().Find(itFind->first));
			m_dwStateDuration = 1;
		}
	}
	else
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			SetVictim(pNewVictim);
			m_dwStateDuration = 1;
		}
	}
}
//martysama0134's ceqyqttoaf71vasf9t71218
