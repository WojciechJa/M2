#ifndef __INC_TABLES_H__
#define __INC_TABLES_H__

#include <memory>

#include "length.h"
#include "item_length.h"
#include "CommonDefines.h"
#ifdef __PREMIUM_PRIVATE_SHOP__
#include <boost/functional/hash.hpp>
#endif

typedef	DWORD IDENT;

enum
{
	HEADER_GD_LOGIN				= 1,
	HEADER_GD_LOGOUT			= 2,

	HEADER_GD_PLAYER_LOAD		= 3,
	HEADER_GD_PLAYER_SAVE		= 4,
	HEADER_GD_PLAYER_CREATE		= 5,
	HEADER_GD_PLAYER_DELETE		= 6,

	HEADER_GD_LOGIN_KEY			= 7,
	// 8 empty
	HEADER_GD_BOOT				= 9,
	HEADER_GD_PLAYER_COUNT		= 10,
	HEADER_GD_QUEST_SAVE		= 11,
	HEADER_GD_SAFEBOX_LOAD		= 12,
	HEADER_GD_SAFEBOX_SAVE		= 13,
	HEADER_GD_SAFEBOX_CHANGE_SIZE	= 14,
	HEADER_GD_EMPIRE_SELECT		= 15,

	HEADER_GD_SAFEBOX_CHANGE_PASSWORD		= 16,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD_SECOND	= 17, // Not really a packet, used internal
	HEADER_GD_DIRECT_ENTER		= 18,

	HEADER_GD_GUILD_SKILL_UPDATE	= 19,
	HEADER_GD_GUILD_EXP_UPDATE		= 20,
	HEADER_GD_GUILD_ADD_MEMBER		= 21,
	HEADER_GD_GUILD_REMOVE_MEMBER	= 22,
	HEADER_GD_GUILD_CHANGE_GRADE	= 23,
	HEADER_GD_GUILD_CHANGE_MEMBER_DATA	= 24,
	HEADER_GD_GUILD_DISBAND		= 25,
	HEADER_GD_GUILD_WAR			= 26,
	HEADER_GD_GUILD_WAR_SCORE		= 27,
	HEADER_GD_GUILD_CREATE		= 28,

	HEADER_GD_ITEM_SAVE			= 30,
	HEADER_GD_ITEM_DESTROY		= 31,

	HEADER_GD_ADD_AFFECT		= 32,
	HEADER_GD_REMOVE_AFFECT		= 33,

	HEADER_GD_HIGHSCORE_REGISTER	= 34,
	HEADER_GD_ITEM_FLUSH		= 35,

	HEADER_GD_PARTY_CREATE		= 36,
	HEADER_GD_PARTY_DELETE		= 37,
	HEADER_GD_PARTY_ADD			= 38,
	HEADER_GD_PARTY_REMOVE		= 39,
	HEADER_GD_PARTY_STATE_CHANGE	= 40,
	HEADER_GD_PARTY_HEAL_USE		= 41,

	HEADER_GD_FLUSH_CACHE		= 42,
	HEADER_GD_RELOAD_PROTO		= 43,

	HEADER_GD_CHANGE_NAME		= 44,
	HEADER_GD_SMS				= 45,

	HEADER_GD_GUILD_CHANGE_LADDER_POINT	= 46,
	HEADER_GD_GUILD_USE_SKILL		= 47,

	HEADER_GD_REQUEST_EMPIRE_PRIV	= 48,
	HEADER_GD_REQUEST_GUILD_PRIV	= 49,

	HEADER_GD_MONEY_LOG				= 50,

	HEADER_GD_GUILD_DEPOSIT_MONEY				= 51,
	HEADER_GD_GUILD_WITHDRAW_MONEY				= 52,
	HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY	= 53,

	HEADER_GD_REQUEST_CHARACTER_PRIV	= 54,

	HEADER_GD_SET_EVENT_FLAG			= 55,

	HEADER_GD_PARTY_SET_MEMBER_LEVEL	= 56,

	HEADER_GD_GUILD_WAR_BET		= 57,

	HEADER_GD_CREATE_OBJECT		= 60,
	HEADER_GD_DELETE_OBJECT		= 61,
	HEADER_GD_UPDATE_LAND		= 62,

	HEADER_GD_MARRIAGE_ADD		= 70,
	HEADER_GD_MARRIAGE_UPDATE	= 71,
	HEADER_GD_MARRIAGE_REMOVE	= 72,

	HEADER_GD_WEDDING_REQUEST	= 73,
	HEADER_GD_WEDDING_READY		= 74,
	HEADER_GD_WEDDING_END		= 75,

#ifdef ENABLE_LRN_BATTLE_PASS
	HEADER_GD_BATTLE_PASS_QUEST_SAVE = 80,
	HEADER_GD_BATTLE_PASS_REWARD_SAVE = 81,
	HEADER_GD_BATTLE_PASS_INFO_SAVE = 82,
#endif

	HEADER_GD_AUTH_LOGIN		= 100,
	HEADER_GD_LOGIN_BY_KEY		= 101,
	HEADER_GD_BILLING_EXPIRE	= 104,
	HEADER_GD_VCARD				= 105,
	HEADER_GD_BILLING_CHECK		= 106,
	HEADER_GD_MALL_LOAD			= 107,

	HEADER_GD_MYSHOP_PRICELIST_UPDATE	= 108,
	HEADER_GD_MYSHOP_PRICELIST_REQ		= 109,

	HEADER_GD_BLOCK_CHAT				= 110,

	HEADER_GD_RELOAD_ADMIN			= 115,
	HEADER_GD_BREAK_MARRIAGE		= 116,
	HEADER_GD_ELECT_MONARCH			= 117,
	HEADER_GD_CANDIDACY				= 118,
	HEADER_GD_ADD_MONARCH_MONEY		= 119,
	HEADER_GD_TAKE_MONARCH_MONEY	= 120,
	HEADER_GD_COME_TO_VOTE			= 121,
	HEADER_GD_RMCANDIDACY			= 122,
	HEADER_GD_SETMONARCH			= 123,
	HEADER_GD_RMMONARCH				= 124,
	HEADER_GD_DEC_MONARCH_MONEY		= 125,

	HEADER_GD_CHANGE_MONARCH_LORD	= 126,
	HEADER_GD_BLOCK_COUNTRY_IP		= 127,
	HEADER_GD_BLOCK_EXCEPTION		= 128,

	HEADER_GD_REQ_CHANGE_GUILD_MASTER	= 129,

	HEADER_GD_REQ_SPARE_ITEM_ID_RANGE	= 130,

	HEADER_GD_UPDATE_HORSE_NAME		= 131,
	HEADER_GD_REQ_HORSE_NAME		= 132,

	HEADER_GD_DC					= 133,

	HEADER_GD_VALID_LOGOUT			= 134,

	HEADER_GD_REQUEST_CHARGE_CASH	= 137,

	HEADER_GD_DELETE_AWARDID		= 138,	// delete gift notify icon

	HEADER_GD_UPDATE_CHANNELSTATUS	= 139,
	HEADER_GD_REQUEST_CHANNELSTATUS	= 140,
#ifdef __ITEM_SHOP__
	HEADER_GD_REFRESH_ITEMSHOP = 141,
    HEADER_GD_ITEMSHOP_SAVE_PREMIUM = 142,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_GD_UPDATE_PET_NAME = 150,
	HEADER_GD_REQ_PET_NAME = 151,
#endif
#if defined(BL_OFFLINE_MESSAGE)
	HEADER_GD_REQUEST_OFFLINE_MESSAGES = 154,
	HEADER_GD_SEND_OFFLINE_MESSAGE = 155,
#endif
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_GD_MOVE_CHANNEL = 240,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_GD_WEEKLY_REWARDS = 153,
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
	HEADER_GD_LOCATION_SYSTEM_GET_DATA = 160,
	HEADER_GD_LOCATION_SYSTEM_MANAGER_DATA = 161,
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GD_PRIVATE_SHOP,
#endif

	HEADER_GD_SETUP			= 0xff,

	///////////////////////////////////////////////
	HEADER_DG_NOTICE			= 1,

	HEADER_DG_LOGIN_SUCCESS			= 30,
	HEADER_DG_LOGIN_NOT_EXIST		= 31,
	HEADER_DG_LOGIN_WRONG_PASSWD	= 33,
	HEADER_DG_LOGIN_ALREADY			= 34,

	HEADER_DG_PLAYER_LOAD_SUCCESS	= 35,
	HEADER_DG_PLAYER_LOAD_FAILED	= 36,
	HEADER_DG_PLAYER_CREATE_SUCCESS	= 37,
	HEADER_DG_PLAYER_CREATE_ALREADY	= 38,
	HEADER_DG_PLAYER_CREATE_FAILED	= 39,
	HEADER_DG_PLAYER_DELETE_SUCCESS	= 40,
	HEADER_DG_PLAYER_DELETE_FAILED	= 41,

	HEADER_DG_ITEM_LOAD			= 42,

	HEADER_DG_BOOT				= 43,
	HEADER_DG_QUEST_LOAD		= 44,

	HEADER_DG_SAFEBOX_LOAD					= 45,
	HEADER_DG_SAFEBOX_CHANGE_SIZE			= 46,
	HEADER_DG_SAFEBOX_WRONG_PASSWORD		= 47,
	HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER = 48,

	HEADER_DG_EMPIRE_SELECT		= 49,

	HEADER_DG_AFFECT_LOAD		= 50,
	HEADER_DG_MALL_LOAD			= 51,

	HEADER_DG_DIRECT_ENTER		= 55,

	HEADER_DG_GUILD_SKILL_UPDATE	= 56,
	HEADER_DG_GUILD_SKILL_RECHARGE	= 57,
	HEADER_DG_GUILD_EXP_UPDATE		= 58,

	HEADER_DG_PARTY_CREATE		= 59,
	HEADER_DG_PARTY_DELETE		= 60,
	HEADER_DG_PARTY_ADD			= 61,
	HEADER_DG_PARTY_REMOVE		= 62,
	HEADER_DG_PARTY_STATE_CHANGE	= 63,
	HEADER_DG_PARTY_HEAL_USE		= 64,
	HEADER_DG_PARTY_SET_MEMBER_LEVEL	= 65,

	HEADER_DG_TIME			= 90,
	HEADER_DG_ITEM_ID_RANGE		= 91,

	HEADER_DG_GUILD_ADD_MEMBER		= 92,
	HEADER_DG_GUILD_REMOVE_MEMBER	= 93,
	HEADER_DG_GUILD_CHANGE_GRADE	= 94,
	HEADER_DG_GUILD_CHANGE_MEMBER_DATA	= 95,
	HEADER_DG_GUILD_DISBAND		= 96,
	HEADER_DG_GUILD_WAR			= 97,
	HEADER_DG_GUILD_WAR_SCORE		= 98,
	HEADER_DG_GUILD_TIME_UPDATE		= 99,
	HEADER_DG_GUILD_LOAD		= 100,
	HEADER_DG_GUILD_LADDER		= 101,
	HEADER_DG_GUILD_SKILL_USABLE_CHANGE	= 102,
	HEADER_DG_GUILD_MONEY_CHANGE	= 103,
	HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE	= 104,

	HEADER_DG_SET_EVENT_FLAG		= 105,

	HEADER_DG_GUILD_WAR_RESERVE_ADD	= 106,
	HEADER_DG_GUILD_WAR_RESERVE_DEL	= 107,
	HEADER_DG_GUILD_WAR_BET		= 108,

	HEADER_DG_RELOAD_PROTO		= 120,
	HEADER_DG_CHANGE_NAME		= 121,

	HEADER_DG_AUTH_LOGIN		= 122,

	HEADER_DG_CHANGE_EMPIRE_PRIV	= 124,
	HEADER_DG_CHANGE_GUILD_PRIV		= 125,

	HEADER_DG_MONEY_LOG			= 126,

	HEADER_DG_CHANGE_CHARACTER_PRIV	= 127,

	HEADER_DG_BILLING_REPAIR		= 128,
	HEADER_DG_BILLING_EXPIRE		= 129,
	HEADER_DG_BILLING_LOGIN		= 130,
	HEADER_DG_VCARD			= 131,
	HEADER_DG_BILLING_CHECK		= 132,

	HEADER_DG_CREATE_OBJECT		= 140,
	HEADER_DG_DELETE_OBJECT		= 141,
	HEADER_DG_UPDATE_LAND		= 142,

	HEADER_DG_MARRIAGE_ADD		= 150,
	HEADER_DG_MARRIAGE_UPDATE		= 151,
	HEADER_DG_MARRIAGE_REMOVE		= 152,

	HEADER_DG_WEDDING_REQUEST		= 153,
	HEADER_DG_WEDDING_READY		= 154,
	HEADER_DG_WEDDING_START		= 155,
	HEADER_DG_WEDDING_END		= 156,

	HEADER_DG_MYSHOP_PRICELIST_RES	= 157,
	HEADER_DG_RELOAD_ADMIN = 158,
	HEADER_DG_BREAK_MARRIAGE = 159,
	HEADER_DG_ELECT_MONARCH			= 160,
	HEADER_DG_CANDIDACY				= 161,
	HEADER_DG_ADD_MONARCH_MONEY		= 162,
	HEADER_DG_TAKE_MONARCH_MONEY	= 163,
	HEADER_DG_COME_TO_VOTE			= 164,
	HEADER_DG_RMCANDIDACY			= 165,
	HEADER_DG_SETMONARCH			= 166,
	HEADER_DG_RMMONARCH			= 167,
	HEADER_DG_DEC_MONARCH_MONEY = 168,

	HEADER_DG_CHANGE_MONARCH_LORD_ACK = 169,
	HEADER_DG_UPDATE_MONARCH_INFO	= 170,
	HEADER_DG_BLOCK_COUNTRY_IP		= 171,
	HEADER_DG_BLOCK_EXCEPTION		= 172,

	HEADER_DG_ACK_CHANGE_GUILD_MASTER = 173,

	HEADER_DG_ACK_SPARE_ITEM_ID_RANGE = 174,

	HEADER_DG_UPDATE_HORSE_NAME 	= 175,
	HEADER_DG_ACK_HORSE_NAME		= 176,

	HEADER_DG_NEED_LOGIN_LOG		= 177,

	HEADER_DG_RESULT_CHARGE_CASH	= 179,
	HEADER_DG_ITEMAWARD_INFORMER	= 180,	//gift notify
	HEADER_DG_RESPOND_CHANNELSTATUS	= 181,
	HEADER_GD_REQ_USER_COUNT = 183,
	HEADER_DG_RET_USER_COUNT = 184,
#ifdef __ITEM_SHOP__
	HEADER_DG_REFRESH_ITEMSHOP = 182,
#endif
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_DG_RESPOND_MOVE_CHANNEL = 188,
#endif
#if defined(BL_OFFLINE_MESSAGE)
	HEADER_DG_RESPOND_OFFLINE_MESSAGES = 189,
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	HEADER_DG_LOCATION_SYSTEM_SEND_DATA	= 190,
	HEADER_DG_LOCATION_SYSTEM_WARP_DATA = 191,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_DG_UPDATE_PET_NAME = 232,
	HEADER_DG_ACK_PET_NAME = 233,
#endif
#ifdef ENABLE_LRN_BATTLE_PASS
	HEADER_DG_BATTLE_PASS_QUEST_LOAD	= 234,
	HEADER_DG_BATTLE_PASS_REWARD_LOAD	= 235,
	HEADER_DG_BATTLE_PASS_INFO_LOAD		= 236,
	
	HEADER_DG_BATTLE_PASS_CLEAR			= 237,
#endif
	HEADER_DG_MAP_LOCATIONS			= 0xfe,

#ifdef ENABLE_EVENT_MANAGER
	HEADER_DG_EVENT_MANAGER			= 212,
	HEADER_GD_EVENT_MANAGER			= 212,
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_DG_PRIVATE_SHOP,
#endif

	HEADER_DG_P2P					= 0xff,
};

/* ----------------------------------------------
 * table
 * ----------------------------------------------
 */

/* game Server -> DB Server */
#pragma pack(1)
enum ERequestChargeType
{
	ERequestCharge_Cash = 0,
	ERequestCharge_Mileage,
};

typedef struct SRequestChargeCash
{
	DWORD		dwAID;		// id(primary key) - Account Table
	DWORD		dwAmount;
	ERequestChargeType	eChargeType;
} TRequestChargeCash;

#if defined(BL_OFFLINE_MESSAGE)
typedef struct
{
	char 	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDReadOfflineMessage;

typedef struct
{
	char	szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char	szMessage[CHAT_MAX_LEN + 1];
} TPacketDGReadOfflineMessage;

typedef struct
{
	char	szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char	szTo[CHARACTER_NAME_MAX_LEN + 1];
	char	szMessage[CHAT_MAX_LEN + 1];
} TPacketGDSendOfflineMessage;
#endif

typedef struct SSimplePlayer
{
	DWORD		dwID;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		byJob;
	BYTE		byLevel;
	DWORD		dwPlayMinutes;
	BYTE		byST, byHT, byDX, byIQ;
	DWORD		wMainPart; // @fixme502
	BYTE		bChangeName;
	DWORD		wHairPart; // @fixme502
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD		wAccePart;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	DWORD	wEmblematPart;
#endif
	BYTE		bDummy[4];
	long		x, y;
	long		lAddr;
	WORD		wPort;
	BYTE		skill_group;
} TSimplePlayer;

typedef struct SAccountTable
{
	DWORD		id;
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	char		social_id[SOCIAL_ID_MAX_LEN + 1];
	char		status[ACCOUNT_STATUS_MAX_LEN + 1];
	BYTE		bEmpire;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
} TAccountTable;

typedef struct SPacketDGCreateSuccess
{
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketDGCreateSuccess;

typedef struct TPlayerItemAttribute
{
	BYTE	bType;
	short	sValue;
#if defined(__ITEM_APPLY_RANDOM__)
	BYTE bPath;
#endif
} TPlayerItemAttribute;

typedef struct SPlayerItem
{
	DWORD	id;
	BYTE	window;
	WORD	pos;
	DWORD	count;

	DWORD	vnum;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	DWORD	owner;
} TPlayerItem;

typedef struct SQuickslot
{
	BYTE	type;
	BYTE	pos;
} TQuickslot;

typedef struct SPlayerSkill
{
	BYTE	bMasterType;
	BYTE	bLevel;
	time_t	tNextRead;
} TPlayerSkill;

#ifdef TITLE_SYSTEM_BYLUZER
typedef struct SPlayerTitle
{
	BYTE	titletype;
	BYTE	active;
} TPlayerTitle;
#endif

struct	THorseInfo
{
	BYTE	bLevel;
	BYTE	bRiding;
	short	sStamina;
	short	sHealth;
	DWORD	dwHorseHealthDropTime;
};

typedef struct SPlayerTable
{
	DWORD	id;

	char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	ip[IP_ADDRESS_LENGTH + 1];

	WORD	job;
	BYTE	voice;

	BYTE	level;
	BYTE	level_step;
	short	st, ht, dx, iq;

	DWORD	exp;
#ifdef ENABLE_LONG_LONG
	long long	gold;
#else
	INT		gold;
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
	long long	cheque;
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	INT		pkt_osiag;
#endif
	BYTE	dir;
	INT		x, y, z;
	INT		lMapIndex;

	long	lExitX, lExitY;
	long	lExitMapIndex;

	// @fixme301
	int hp;
	int sp;

	int sRandomHP;
	int sRandomSP;

	int         playtime;

	short	stat_point;
	short	skill_point;
	short	sub_skill_point;
	short	horse_skill_point;

	TPlayerSkill skills[SKILL_MAX_NUM];

	TQuickslot  quickslot[QUICKSLOT_MAX_NUM];

	BYTE	part_base;
	DWORD	parts[PART_MAX_NUM]; // @fixme502

	short	stamina;

	BYTE	skill_group;
	long	lAlignment;
	char	szMobile[MOBILE_MAX_LEN + 1];

	short	stat_reset_count;

	THorseInfo	horse;

	DWORD	logoff_interval;

#ifdef __PREMIUM_PRIVATE_SHOP__
	WORD	wPrivateShopUnlockedSlot;
#endif

	int		aiPremiumTimes[PREMIUM_MAX_NUM];
#ifdef WEEKLY_RANK_BYLUZER
	int w_monsters;
	int w_bosses;
	int w_metins;
	int w_dungs;
	int w_ds;
	int w_refine;
	int w_sm;
	int weekly_season;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	TPlayerTitle titles[TITLE_MAX_NUM];
#endif
} TPlayerTable;

#ifdef WEEKLY_RANK_BYLUZER
typedef struct GDWeeklyRewards
{
	DWORD	item_vnum;
	DWORD	account_id;
} TWeeklyRewardsGD;
#endif

typedef struct SMobSkillLevel
{
	DWORD	dwVnum;
	BYTE	bLevel;
} TMobSkillLevel;

typedef struct SEntityTable
{
	DWORD dwVnum;
} TEntityTable;

typedef struct SMobTable : public SEntityTable
{
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE	bType;			// Monster, NPC
	BYTE	bRank;			// PAWN, KNIGHT, KING
	BYTE	bBattleType;		// MELEE, etc..
	BYTE	bLevel;			// Level
	BYTE	bSize;

	DWORD	dwGoldMin;
	DWORD	dwGoldMax;
	DWORD	dwExp;
	long long	dwMaxHP;
	BYTE	bRegenCycle;
	BYTE	bRegenPercent;
	WORD	wDef;

	DWORD	dwAIFlag;
	DWORD	dwRaceFlag;
	DWORD	dwImmuneFlag;

	BYTE	bStr, bDex, bCon, bInt;
	DWORD	dwDamageRange[2];

	short	sAttackSpeed;
	short	sMovingSpeed;
	BYTE	bAggresiveHPPct;
	WORD	wAggressiveSight;
	WORD	wAttackRange;

	char	cEnchants[MOB_ENCHANTS_MAX_NUM];
	char	cResists[MOB_RESISTS_MAX_NUM];

	DWORD	dwResurrectionVnum;
	DWORD	dwDropItemVnum;

	BYTE	bMountCapacity;
	BYTE	bOnClickType;

	BYTE	bEmpire;
	char	szFolder[64 + 1];

	float	fDamMultiply;

	DWORD	dwSummonVnum;
	DWORD	dwDrainSP;
	DWORD	dwMobColor;
	DWORD	dwPolymorphItemVnum;

	TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

	BYTE	bBerserkPoint;
	BYTE	bStoneSkinPoint;
	BYTE	bGodSpeedPoint;
	BYTE	bDeathBlowPoint;
	BYTE	bRevivePoint;
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	short	sPktOsiag;
#endif
} TMobTable;

typedef struct SSkillTable
{
	DWORD	dwVnum;
	char	szName[32 + 1];
	BYTE	bType;
	BYTE	bMaxLevel;
	DWORD	dwSplashRange;

	char	szPointOn[64];
	char	szPointPoly[100 + 1];
	char	szSPCostPoly[100 + 1];
	char	szDurationPoly[100 + 1];
	char	szDurationSPCostPoly[100 + 1];
	char	szCooldownPoly[100 + 1];
	char	szMasterBonusPoly[100 + 1];
	//char	szAttackGradePoly[100 + 1];
	char	szGrandMasterAddSPCostPoly[100 + 1];
	DWORD	dwFlag;
	DWORD	dwAffectFlag;

	// Data for secondary skill
	char 	szPointOn2[64];
	char 	szPointPoly2[100 + 1];
	char 	szDurationPoly2[100 + 1];
	DWORD 	dwAffectFlag2;

	// Data for grand master point
	char 	szPointOn3[64];
	char 	szPointPoly3[100 + 1];
	char 	szDurationPoly3[100 + 1];

	BYTE	bLevelStep;
	BYTE	bLevelLimit;
	DWORD	preSkillVnum;
	BYTE	preSkillLevel;

	long	lMaxHit;
	char	szSplashAroundDamageAdjustPoly[100 + 1];

	BYTE	bSkillAttrType;

	DWORD	dwTargetRange;
} TSkillTable;

typedef struct SShopItemTable
{
	DWORD		vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif

    TItemPos	pos;
#ifdef ENABLE_LONG_LONG
	long long	price;
#else
	DWORD		price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	long long	cheque;
#endif
	BYTE		display_pos;
} TShopItemTable;

#ifdef ENABLE_VS_SHOP_SEARCH
typedef struct search_item
{
	DWORD	vnum;
} TSearchItems;
#endif

typedef struct SShopTable
{
	DWORD		dwVnum;
	DWORD		dwNPCVnum;

#ifdef __EXTENDED_ITEM_COUNT__
	int byItemCount;
#else
	BYTE byItemCount;
#endif
	TShopItemTable	items[SHOP_HOST_ITEM_MAX_NUM];
} TShopTable;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct SSOfflineShopItemTable
{
	BYTE		bDisplayPos;
	TItemPos	itemPos;
	long long	lPrice;
	long long	lPrice2;
} TOfflineShopItemTable;

typedef struct ShopLog {
	DWORD	itemVnum;
	int		itemCount;
	long long	price;
	long long	price2;
	char		date[20];
	char		action[5];
}TShopLog;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
struct AcceMaterials
{
    DWORD vnum;
    DWORD count;
};
#endif

#define QUEST_NAME_MAX_LEN	32
#define QUEST_STATE_MAX_LEN	64

typedef struct SQuestTable
{
	DWORD		dwPID;
	char		szName[QUEST_NAME_MAX_LEN + 1];
	char		szState[QUEST_STATE_MAX_LEN + 1];
	long		lValue;
} TQuestTable;

typedef struct SItemLimit
{
	BYTE	bType;
	long	lValue;
} TItemLimit;

typedef struct SItemApply
{
	BYTE	bType;
	long	lValue;
} TItemApply;

typedef struct SItemTable : public SEntityTable
{
	DWORD		dwVnumRange;
	char        szName[ITEM_NAME_MAX_LEN + 1];
	char	szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE	bType;
	BYTE	bSubType;

	BYTE        bWeight;
	BYTE	bSize;

	DWORD	dwAntiFlags;
	DWORD	dwFlags;
	DWORD	dwWearFlags;
	DWORD	dwImmuneFlag;

#ifdef ENABLE_LONG_LONG
	long long	dwGold;
	long long	dwShopBuyPrice;
#else
	DWORD		dwGold;
	DWORD		dwShopBuyPrice;
#endif

	TItemLimit	aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply	aApplies[ITEM_APPLY_MAX_NUM];
	long        alValues[ITEM_VALUES_MAX_NUM];
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD	dwRefinedVnum;
	WORD	wRefineSet;
	BYTE	bAlterToMagicItemPct;
	BYTE	bSpecular;
	BYTE	bGainSocketPct;

	short int	sAddonType;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	BYTE	SpecialInvType;
	BYTE	CanUseInSpecialInv;
#endif

	char		cLimitRealTimeFirstUseIndex;
	char		cLimitTimerBasedOnWearIndex;
} TItemTable;

struct TItemAttrTable
{
	TItemAttrTable() :
		dwApplyIndex(0),
		dwProb(0)
	{
		szApply[0] = 0;
		memset(&lValues, 0, sizeof(lValues));
		memset(&bMaxLevelBySet, 0, sizeof(bMaxLevelBySet));
	}

	char    szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD   dwApplyIndex;
	DWORD   dwProb;
	long    lValues[ITEM_ATTRIBUTE_MAX_LEVEL];
	BYTE    bMaxLevelBySet[ATTRIBUTE_SET_MAX_NUM];
};

typedef struct SConnectTable
{
	char	login[LOGIN_MAX_LEN + 1];
	IDENT	ident;
} TConnectTable;

typedef struct SLoginPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLoginPacket;

typedef struct SPlayerLoadPacket
{
	DWORD	account_id;
	DWORD	player_id;
	BYTE	account_index;
} TPlayerLoadPacket;

typedef struct SPlayerCreatePacket
{
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	DWORD		account_id;
	BYTE		account_index;
	TPlayerTable	player_table;
} TPlayerCreatePacket;

typedef struct SPlayerDeletePacket
{
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	player_id;
	BYTE	account_index;
	//char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	private_code[8];
} TPlayerDeletePacket;

typedef struct SLogoutPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLogoutPacket;

typedef struct SPlayerCountPacket
{
	DWORD	dwCount;
} TPlayerCountPacket;

#define SAFEBOX_MAX_NUM			135
#define SAFEBOX_PASSWORD_MAX_LEN	6

typedef struct SSafeboxTable
{
	DWORD	dwID;
	BYTE	bSize;
	DWORD	dwGold;
	WORD	wItemCount;
} TSafeboxTable;

typedef struct SSafeboxChangeSizePacket
{
	DWORD	dwID;
	BYTE	bSize;
} TSafeboxChangeSizePacket;

typedef struct SSafeboxLoadPacket
{
	DWORD	dwID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxLoadPacket;

#if defined(__BL_MOVE_CHANNEL__)
typedef struct SMoveChannel
{
	BYTE	bChannel;
	long	lMapIndex;
} TMoveChannel;

typedef struct SRespondMoveChannel
{
	WORD	wPort;
	long	lAddr;
} TRespondMoveChannel;
#endif

typedef struct SSafeboxChangePasswordPacket
{
	DWORD	dwID;
	char	szOldPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
	char	szNewPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxChangePasswordPacket;

typedef struct SSafeboxChangePasswordPacketAnswer
{
	BYTE	flag;
} TSafeboxChangePasswordPacketAnswer;

typedef struct SEmpireSelectPacket
{
	DWORD	dwAccountID;
	BYTE	bEmpire;
} TEmpireSelectPacket;

typedef struct SPacketGDSetup
{
	char	szPublicIP[16];	// Public IP which listen to users
	BYTE	bChannel;
	WORD	wListenPort;
	WORD	wP2PPort;
	long	alMaps[MAP_ALLOW_LIMIT];
	DWORD	dwLoginCount;
	BYTE	bAuthServer;
} TPacketGDSetup;

typedef struct SPacketDGMapLocations
{
	BYTE	bCount;
} TPacketDGMapLocations;

typedef struct SMapLocation
{
	long	alMaps[MAP_ALLOW_LIMIT];
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
} TMapLocation;

typedef struct SPacketDGP2P
{
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
	BYTE	bChannel;
} TPacketDGP2P;

typedef struct SPacketGDDirectEnter
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	BYTE	index;
} TPacketGDDirectEnter;

typedef struct SPacketDGDirectEnter
{
	TAccountTable accountTable;
	TPlayerTable playerTable;
} TPacketDGDirectEnter;

typedef struct SPacketGuildSkillUpdate
{
	DWORD guild_id;
	int amount;
	BYTE skill_levels[12];
	BYTE skill_point;
	BYTE save;
} TPacketGuildSkillUpdate;

typedef struct SPacketGuildExpUpdate
{
	DWORD guild_id;
	int amount;
} TPacketGuildExpUpdate;

typedef struct SPacketGuildChangeMemberData
{
	DWORD guild_id;
	DWORD pid;
	DWORD offer;
	BYTE level;
	BYTE grade;
} TPacketGuildChangeMemberData;

typedef struct SPacketDGLoginAlready
{
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketDGLoginAlready;

typedef struct TPacketAffectElement
{
	DWORD	dwType;
	BYTE	bApplyOn;
	long	lApplyValue;
	DWORD	dwFlag;
	long	lDuration;
	long	lSPCost;
} TPacketAffectElement;

typedef struct SPacketGDAddAffect
{
	DWORD			dwPID;
	TPacketAffectElement	elem;
} TPacketGDAddAffect;

typedef struct SPacketGDRemoveAffect
{
	DWORD	dwPID;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGDRemoveAffect;

typedef struct SPacketGDHighscore
{
	DWORD	dwPID;
	long	lValue;
	char	cDir;
	char	szBoard[21];
} TPacketGDHighscore;

typedef struct SPacketPartyCreate
{
	DWORD	dwLeaderPID;
} TPacketPartyCreate;

typedef struct SPacketPartyDelete
{
	DWORD	dwLeaderPID;
} TPacketPartyDelete;

typedef struct SPacketPartyAdd
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bState;
} TPacketPartyAdd;

typedef struct SPacketPartyRemove
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
} TPacketPartyRemove;

typedef struct SPacketPartyStateChange
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bRole;
	BYTE	bFlag;
} TPacketPartyStateChange;

typedef struct SPacketPartySetMemberLevel
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bLevel;
} TPacketPartySetMemberLevel;

typedef struct SPacketGDBoot
{
    DWORD	dwItemIDRange[2];
	char	szIP[16];
} TPacketGDBoot;

typedef struct SPacketGuild
{
	DWORD	dwGuild;
	DWORD	dwInfo;
} TPacketGuild;

typedef struct SPacketGDGuildAddMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
} TPacketGDGuildAddMember;

typedef struct SPacketDGGuildMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
	BYTE	isGeneral;
	BYTE	bJob;
	BYTE	bLevel;
	DWORD	dwOffer;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGGuildMember;

typedef struct SPacketGuildWar
{
	BYTE	bType;
	BYTE	bWar;
	DWORD	dwGuildFrom;
	DWORD	dwGuildTo;
	long	lWarPrice;
	long	lInitialScore;
} TPacketGuildWar;

typedef struct SPacketGuildWarScore
{
	DWORD dwGuildGainPoint;
	DWORD dwGuildOpponent;
	long lScore;
	long lBetScore;
} TPacketGuildWarScore;

typedef struct SRefineMaterial
{
	DWORD vnum;
	int count;
} TRefineMaterial;

typedef struct SRefineTable
{
	//DWORD src_vnum;
	//DWORD result_vnum;
	DWORD id;
#ifdef __EXTENDED_ITEM_COUNT__
	int material_count;
#else
	BYTE material_count;
#endif
	int cost;
	int cost2;
	int cost3;
	int prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SBanwordTable
{
	char szWord[BANWORD_MAX_LEN + 1];
} TBanwordTable;

typedef struct SPacketGDChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDChangeName;

typedef struct SPacketDGChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGChangeName;

typedef struct SPacketGuildLadder
{
	DWORD dwGuild;
	long lLadderPoint;
	long lWin;
	long lDraw;
	long lLoss;
} TPacketGuildLadder;

typedef struct SPacketGuildLadderPoint
{
	DWORD dwGuild;
	long lChange;
} TPacketGuildLadderPoint;

typedef struct SPacketGDSMS
{
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szTo[CHARACTER_NAME_MAX_LEN + 1];
	char szMobile[MOBILE_MAX_LEN + 1];
	char szMsg[SMS_MAX_LEN + 1];
} TPacketGDSMS;

typedef struct SPacketGuildUseSkill
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	DWORD dwCooltime;
} TPacketGuildUseSkill;

typedef struct SPacketGuildSkillUsableChange
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	BYTE bUsable;
} TPacketGuildSkillUsableChange;

typedef struct SPacketGDLoginKey
{
	DWORD dwAccountID;
	DWORD dwLoginKey;
} TPacketGDLoginKey;

typedef struct SPacketGDAuthLogin
{
	DWORD	dwID;
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szSocialID[SOCIAL_ID_MAX_LEN + 1];
	DWORD	adwClientKey[4];
	BYTE	bBillType;
	DWORD	dwBillID;
	int		iPremiumTimes[PREMIUM_MAX_NUM];
} TPacketGDAuthLogin;

typedef struct SPacketGDLoginByKey
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
	char	szIP[MAX_HOST_LENGTH + 1];
} TPacketGDLoginByKey;

typedef struct SPacketGiveGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	time_t duration_sec;
} TPacketGiveGuildPriv;
typedef struct SPacketGiveEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	time_t duration_sec;
} TPacketGiveEmpirePriv;
typedef struct SPacketGiveCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
} TPacketGiveCharacterPriv;
typedef struct SPacketRemoveGuildPriv
{
	BYTE type;
	DWORD guild_id;
} TPacketRemoveGuildPriv;
typedef struct SPacketRemoveEmpirePriv
{
	BYTE type;
	BYTE empire;
} TPacketRemoveEmpirePriv;

typedef struct SPacketDGChangeCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
	BYTE bLog;
} TPacketDGChangeCharacterPriv;

typedef struct SPacketDGChangeGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeGuildPriv;

typedef struct SPacketDGChangeEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeEmpirePriv;

typedef struct SPacketDGUserCount
{
	DWORD user;
} TPacketDGUserCount;

typedef struct SPacketMoneyLog
{
	BYTE type;
	DWORD vnum;
#ifdef ENABLE_LONG_LONG
	long long gold;
#else
	INT gold;
#endif
} TPacketMoneyLog;

typedef struct SPacketGDGuildMoney
{
	DWORD dwGuild;
	INT iGold;
} TPacketGDGuildMoney;

typedef struct SPacketDGGuildMoneyChange
{
	DWORD dwGuild;
	INT iTotalGold;
} TPacketDGGuildMoneyChange;

typedef struct SPacketDGGuildMoneyWithdraw
{
	DWORD dwGuild;
	INT iChangeGold;
} TPacketDGGuildMoneyWithdraw;

typedef struct SPacketGDGuildMoneyWithdrawGiveReply
{
	DWORD dwGuild;
	INT iChangeGold;
	BYTE bGiveSuccess;
} TPacketGDGuildMoneyWithdrawGiveReply;

typedef struct SPacketSetEventFlag
{
	char	szFlagName[EVENT_FLAG_NAME_MAX_LEN + 1];
	long	lValue;
} TPacketSetEventFlag;

typedef struct SPacketBillingLogin
{
	DWORD	dwLoginKey;
	BYTE	bLogin;
} TPacketBillingLogin;

typedef struct SPacketBillingRepair
{
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szHost[MAX_HOST_LENGTH + 1];
} TPacketBillingRepair;

typedef struct SPacketBillingExpire
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	BYTE	bBillType;
	DWORD	dwRemainSeconds;
} TPacketBillingExpire;

typedef struct SPacketLoginOnSetup
{
	DWORD   dwID;
	char    szLogin[LOGIN_MAX_LEN + 1];
	char    szSocialID[SOCIAL_ID_MAX_LEN + 1];
	char    szHost[MAX_HOST_LENGTH + 1];
	DWORD   dwLoginKey;
	DWORD   adwClientKey[4];
#ifdef __PREMIUM_PRIVATE_SHOP__
	DWORD	dwPID;
	DWORD	dwHandle;
	bool	bHasPrivateShop;
#endif
} TPacketLoginOnSetup;

typedef struct SPacketGDCreateObject
{
	DWORD	dwVnum;
	DWORD	dwLandID;
	INT		lMapIndex;
	INT	 	x, y;
	float	xRot;
	float	yRot;
	float	zRot;
} TPacketGDCreateObject;

typedef struct SPacketGDVCard
{
	DWORD	dwID;
	char	szSellCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szSellAccount[LOGIN_MAX_LEN + 1];
	char	szBuyCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szBuyAccount[LOGIN_MAX_LEN + 1];
} TPacketGDVCard;

typedef struct SGuildReserve
{
	DWORD       dwID;
	DWORD       dwGuildFrom;
	DWORD       dwGuildTo;
	DWORD       dwTime;
	BYTE        bType;
	long        lWarPrice;
	long        lInitialScore;
	bool        bStarted;
	DWORD	dwBetFrom;
	DWORD	dwBetTo;
	long	lPowerFrom;
	long	lPowerTo;
	long	lHandicap;
} TGuildWarReserve;

typedef struct SPacketGDGuildWarBet
{
	DWORD	dwWarID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwGold;
	DWORD	dwGuild;
} TPacketGDGuildWarBet;

// Marriage

typedef struct SPacketMarriageAdd
{
	DWORD dwPID1;
	DWORD dwPID2;
	time_t tMarryTime;
	char szName1[CHARACTER_NAME_MAX_LEN + 1];
	char szName2[CHARACTER_NAME_MAX_LEN + 1];
} TPacketMarriageAdd;

typedef struct SPacketMarriageUpdate
{
	DWORD dwPID1;
	DWORD dwPID2;
	INT  iLovePoint;
	BYTE  byMarried;
} TPacketMarriageUpdate;

typedef struct SPacketMarriageRemove
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketMarriageRemove;

typedef struct SPacketWeddingRequest
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingRequest;

typedef struct SPacketWeddingReady
{
	DWORD dwPID1;
	DWORD dwPID2;
	DWORD dwMapIndex;
} TPacketWeddingReady;

typedef struct SPacketWeddingStart
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingStart;

typedef struct SPacketWeddingEnd
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingEnd;

typedef struct SPacketMyshopPricelistHeader
{
	DWORD	dwOwnerID;
	BYTE	byCount;
} TPacketMyshopPricelistHeader;

typedef struct SItemPriceInfo
{
	DWORD	dwVnum;
	long long	dwPrice;
} TItemPriceInfo;

typedef struct SItemPriceListTable
{
	DWORD	dwOwnerID;
	BYTE	byCount;

	TItemPriceInfo	aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;

typedef struct SPacketBlockChat
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lDuration;
} TPacketBlockChat;

//ADMIN_MANAGER
typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	char m_szContactIP[16];
	char m_szServerIP[16];
	int m_Authority;
} tAdminInfo;
//END_ADMIN_MANAGER

//BOOT_LOCALIZATION
struct tLocale
{
	char szValue[32];
	char szKey[32];
};
//BOOT_LOCALIZATION

//RELOAD_ADMIN
typedef struct SPacketReloadAdmin
{
	char szIP[16];
} TPacketReloadAdmin;
//END_RELOAD_ADMIN

typedef struct TMonarchInfo
{
	DWORD pid[4];
	int64_t money[4];
	char name[4][32];
	char date[4][32];
} MonarchInfo;

typedef struct TMonarchElectionInfo
{
	DWORD pid;
	DWORD selectedpid;
	char date[32];
} MonarchElectionInfo;

typedef struct tMonarchCandidacy
{
	DWORD pid;
	char name[32];
	char date[32];
} MonarchCandidacy;

typedef struct tChangeMonarchLord
{
	BYTE bEmpire;
	DWORD dwPID;
} TPacketChangeMonarchLord;

typedef struct tChangeMonarchLordACK
{
	BYTE bEmpire;
	DWORD dwPID;
	char szName[32];
	char szDate[32];
} TPacketChangeMonarchLordACK;

// Block Country Ip
typedef struct tBlockCountryIp
{
	DWORD	ip_from;
	DWORD	ip_to;
} TPacketBlockCountryIp;

enum EBlockExceptionCommand
{
	BLOCK_EXCEPTION_CMD_ADD = 1,
	BLOCK_EXCEPTION_CMD_DEL = 2,
};

// Block Exception Account
typedef struct tBlockException
{
	BYTE	cmd;	// 1 == add, 2 == delete
	char	login[LOGIN_MAX_LEN + 1];
}TPacketBlockException;

typedef struct tChangeGuildMaster
{
	DWORD dwGuildID;
	DWORD idFrom;
	DWORD idTo;
} TPacketChangeGuildMaster;

typedef struct tItemIDRange
{
	DWORD dwMin;
	DWORD dwMax;
	DWORD dwUsableItemIDMin;
} TItemIDRangeTable;

typedef struct tUpdateHorseName
{
	DWORD dwPlayerID;
	char szHorseName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketUpdateHorseName;

typedef struct tDC
{
	char	login[LOGIN_MAX_LEN + 1];
} TPacketDC;

typedef struct tNeedLoginLogInfo
{
	DWORD dwPlayerID;
} TPacketNeedLoginLogInfo;

typedef struct tItemAwardInformer
{
	char	login[LOGIN_MAX_LEN + 1];
	char	command[20];
	unsigned int vnum;
} TPacketItemAwardInfromer;

typedef struct tDeleteAwardID
{
	DWORD dwID;
} TPacketDeleteAwardID;

typedef struct SChannelStatus
{
	WORD nPort; // backward change for client @fixme024
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_SWITCHBOT
struct TSwitchbotAttributeAlternativeTable
{
	TPlayerItemAttribute attributes[MAX_NORM_ATTR_NUM];

	bool IsConfigured() const
	{
		for (const auto& it : attributes)
		{
			if (it.bType && it.sValue)
			{
				return true;
			}
		}

		return false;
	}
};

struct TSwitchbotTable
{
	DWORD player_id;
	bool active[SWITCHBOT_SLOT_COUNT];
	bool finished[SWITCHBOT_SLOT_COUNT];
	DWORD items[SWITCHBOT_SLOT_COUNT];
	TSwitchbotAttributeAlternativeTable alternatives[SWITCHBOT_SLOT_COUNT][SWITCHBOT_ALTERNATIVE_COUNT];

	TSwitchbotTable() : player_id(0)
	{
		memset(&items, 0, sizeof(items));
		memset(&alternatives, 0, sizeof(alternatives));
		memset(&active, false, sizeof(active));
		memset(&finished, false, sizeof(finished));
	}
};

struct TSwitchbottAttributeTable
{
	BYTE attribute_set;
	int apply_num;
	long max_value;
};
#endif

#ifdef __ITEM_SHOP__
typedef struct item_shop_refresh_packet
{
	DWORD	id;
	DWORD	category;
	BYTE	flag;
} TItemShopRefreshPacket;

typedef struct item_shop_save_premium_packet
{
	DWORD	account_id;
	BYTE	premium;
	DWORD	time;
} TItemShopSavePremiumPacket;
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
enum
{
	HEADER_LOCATION_SYSTEM_DELETE,
	HEADER_LOCATION_SYSTEM_CREATE,
	HEADER_LOCATION_SYSTEM_WARP,
};

struct TLocationSystemData
{
	DWORD		pid;
	int			map_index[MAX_LOCATION_POSITION];
	int			global_x[MAX_LOCATION_POSITION];
	int			global_y[MAX_LOCATION_POSITION];
	char		name[MAX_LOCATION_CHAR_STRING];
};

struct TLocationSystemBaseData
{
	DWORD			pid;
};

struct TLocationSystemPlayerData
{
	int			map_index[MAX_LOCATION_POSITION];
	char		name[MAX_LOCATION_CHAR_STRING];
	int			posx[MAX_LOCATION_POSITION];
	int			posy[MAX_LOCATION_POSITION];
};

struct TLocationManagerData
{
	BYTE		header;
	
	DWORD		pid;
	BYTE		position;
	int			map_index;
	int			global_x;
	int			global_y;
	char		name[MAX_LOCATION_CHAR_STRING];
};

struct TLocationSystemWarpData
{
	DWORD		pid;
	int			map_index;
	int			global_x;
	int			global_y;
};
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
typedef struct SBattlePassQuestTable
{
	int32_t		pid;
	int32_t		index;
	int32_t		count;
	int8_t		status;
} TBattlePassQuestTable;

typedef struct SBattlePassRewardTable
{
	int32_t		pid;
	int16_t		level;
	int8_t		status_free;
	int8_t		status_premium;
} TBattlePassRewardTable;

typedef struct SBattlePassInformationTable
{
	int32_t		pid;
	int16_t		level;
	int32_t		exp;
	int16_t		premium;
} TBattlePassInformationTable;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct event_struct_
{
	WORD	eventID;
	BYTE	eventIndex;
	int		startTime;
	int		endTime;
	BYTE	empireFlag;
	BYTE	channelFlag;
	DWORD	value[4];
	bool	eventStatus;
	bool	eventTypeOnlyStart;
	char	startTimeText[25];
	char	endTimeText[25];
}TEventManagerData;
enum
{
	EVENT_MANAGER_LOAD,
	EVENT_MANAGER_EVENT_STATUS,
	EVENT_MANAGER_REMOVE_EVENT,
	EVENT_MANAGER_UPDATE,

	BONUS_EVENT = 1,
	DOUBLE_BOSS_LOOT_EVENT = 2,
	DOUBLE_METIN_LOOT_EVENT = 3,
	DOUBLE_MISSION_BOOK_EVENT = 4,
	DUNGEON_COOLDOWN_EVENT = 5,
	DUNGEON_CHEST_LOOT_EVENT = 6,
	EMPIRE_WAR_EVENT = 7,
	MOONLIGHT_EVENT = 8,
	TOURNAMENT_EVENT = 9,
	WHELL_OF_FORTUNE_EVENT = 10,
	HALLOWEEN_EVENT = 11,
	NPC_SEARCH_EVENT = 12,
	EXP_EVENT = 13,
	ITEM_DROP_EVENT = 14,
	YANG_DROP_EVENT = 15,
};
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
typedef struct SPrivateShop
{
	DWORD				dwOwner;
	char				szTitle[TITLE_MAX_LEN + 1];
	char				szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE				bState;

	DWORD				dwVnum;
	BYTE				bTitleType;

	long				lX;
	long				lY;
	long				lMapIndex;
	BYTE				bChannel;
	WORD				wPort;

	long long			llGold;
	DWORD				dwCheque;
	BYTE				bPageCount;
	time_t				tPremiumTime;
	WORD				wUnlockedSlots;
} TPrivateShop;

typedef struct SItemPrice
{
	long long	llGold;
	DWORD		dwCheque;
} TItemPrice;

typedef struct SPlayerPrivateShopItem
{
	DWORD					dwID;
	WORD					wPos;
	DWORD					dwCount;

	DWORD					dwVnum;
	long					alSockets[ITEM_SOCKET_MAX_NUM];

	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	time_t					tCheckin;

	TItemPrice				TPrice;
	DWORD					dwOwner;

#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
    TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPlayerPrivateShopItem;

typedef struct SPrivateShopSale
{
	DWORD		dwID;
	DWORD		dwOwner;
	DWORD		dwCustomer;
	char		szCustomerName[CHARACTER_NAME_MAX_LEN + 1];
	time_t		tTime;
	TPlayerPrivateShopItem	TItem;

	void AssignID()
	{
		size_t seededID = 0;

		// Both time of sale and sale item's id are a unique pair
		boost::hash_combine(seededID, tTime);
		boost::hash_combine(seededID, TItem.dwID);

		dwID = seededID;
	}
} TPrivateShopSale;

typedef struct SMarketItemPrice
{
	DWORD dwVnum;
	TItemPrice TPrice;
} TMarketItemPrice;

/* Game -> Database */
enum EPrivateShopGDSubheader
{
	PRIVATE_SHOP_GD_SUBHEADER_LOGOUT,
	PRIVATE_SHOP_GD_SUBHEADER_CREATE,
	PRIVATE_SHOP_GD_SUBHEADER_CLOSE,
	PRIVATE_SHOP_GD_SUBHEADER_DELETE,
	PRIVATE_SHOP_GD_SUBHEADER_DESPAWN,
	PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_PRICE_CHANGE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_MOVE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_TITLE_CHANGE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_WARP_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_SLOT_UNLOCK_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW,
	PRIVATE_SHOP_GD_SUBHEADER_BUY,
	PRIVATE_SHOP_GD_SUBHEADER_FAILED_BUY,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_TRANSFER,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_DELETE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_EXPIRE,
	PRIVATE_SHOP_GD_SUBHEADER_PREMIUM_TIME_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_INIT,
};

typedef struct SSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} TSelectedItem;

typedef struct SPacketGDPrivateShopBuyRequest
{
	DWORD			dwCustomerPID;
	long long		llGoldBalance;
	DWORD			dwChequeBalance;
	TSelectedItem	aSelectedItems[SELECTED_ITEM_MAX_NUM];
} TPacketGDPrivateShopBuyRequest;

typedef struct SPacketGDPrivateShopItemCheckin
{
	DWORD					dwShopID;
	TPlayerPrivateShopItem	TItem;
	int						iPos;
} TPacketGDPrivateShopItemCheckin;

typedef struct SPacketGDPrivateShopItemCheckout
{
	DWORD		dwPID;
	WORD		wSrcPos;
	TItemPos	TDstPos;
	TPlayerPrivateShopItem	TItem;
} TPacketGDPrivateShopItemCheckout;

typedef struct SPacketGDPrivateShopBuy
{
	TPlayerPrivateShopItem	TItem;
	DWORD					dwCustomer;
	char					szCustomerName[CHARACTER_NAME_MAX_LEN + 1];
	time_t					tTime;
} TPacketGDPrivateShopBuy;

typedef struct SPacketGDPrivateShopFailedBuy
{
	DWORD		dwShopID;
	WORD		wPos;
} TPacketGDPrivateShopFailedBuy;

typedef struct SPacketGDPrivateShopItemDelete
{
	DWORD		dwShopID;
	DWORD		dwItemID;
} TPacketGDPrivateShopItemDelete;

typedef struct SPacketGDPrivateShopItemExpire
{
	DWORD		dwShopID;
	WORD		wPos;
} TPacketGDPrivateShopItemExpire;


typedef struct SPacketGDPrivateShopPremiumTimeUpdate
{
	DWORD		dwAID;
	DWORD		dwPID;
	time_t		tPremiumTime;
} TPacketGDPrivateShopPremiumTimeUpdate;

/* Database -> Game */
enum EPrivateShopDGSubheader
{
	PRIVATE_SHOP_DG_SUBHEADER_CREATE_RESULT,
	PRIVATE_SHOP_DG_SUBHEADER_NO_SHOP,
	PRIVATE_SHOP_DG_SUBHEADER_CLOSE_RESULT_BALANCE_AVAILABLE,
	PRIVATE_SHOP_DG_SUBHEADER_CLOSE,
	PRIVATE_SHOP_DG_SUBHEADER_SPAWN,
	PRIVATE_SHOP_DG_SUBHEADER_DESTROY,
	PRIVATE_SHOP_DG_SUBHEADER_DESPAWN,
	PRIVATE_SHOP_DG_SUBHEADER_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_SALE_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_PRICE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_MODIFY_STATE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_GOLD,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_CHEQUE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_REQUEST,
	PRIVATE_SHOP_DG_SUBHEADER_REMOVE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ADD_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_SALE_UPDATE,
	PRIVATE_SHOP_DG_SUBHEADER_STATE_UPDATE,
	PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW_RESULT_NO_BALANCE,
	PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW,
	PRIVATE_SHOP_DG_SUBHEADER_NOT_MODIFY_STATE,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_PRICE_CHANGE,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_MOVE,
	PRIVATE_SHOP_DG_SUBHEADER_CANNOT_MOVE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_REQ,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_FALSE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKOUT_REQ,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_EXPIRE,
	PRIVATE_SHOP_DG_SUBHEADER_SHOP_NOT_AVAILABLE,
	PRIVATE_SHOP_DG_SUBHEADER_TITLE_CHANGE,
	PRIVATE_SHOP_DG_SUBHEADER_WARP,
	PRIVATE_SHOP_DG_SUBHEADER_UNLOCK_SLOT_RES,
	PRIVATE_SHOP_DG_SUBHEADER_NO_AVAILABLE_SPACE,
	PRIVATE_SHOP_DG_SUBHEADER_MARKET_ITEM_PRICE_DATA_UPDATE,
};

typedef struct SPacketDGPrivateShopCreateResult
{
	TPrivateShop		privateShopTable;
	bool				bSuccess;
} TPacketDGPrivateShopCreateResult;

typedef struct SPacketDGPrivateShopBuyRequest
{
	DWORD					dwCustomerPID;
	TPlayerPrivateShopItem	arRequestedItems[SELECTED_ITEM_MAX_NUM];
} TPacketDGPrivateShopBuyRequest;

typedef struct SPacketDGPrivateShopStateUpdate
{
	DWORD		dwPID;
	BYTE		bState;
} TPacketDGPrivateShopStateUpdate;

typedef struct SPacketDGPrivateShopWithdraw
{
	long long	llGold;
	DWORD		dwCheque;
} TPacketDGPrivateShopWithdraw;

typedef struct SPacketDGPrivateShopItemCheckin
{
	DWORD		dwPID;
	TPlayerPrivateShopItem	TItem;
} TPacketDGPrivateShopItemCheckin;

typedef struct SPacketDGPrivateShopItemCheckout
{
	DWORD		dwPID;
	WORD		wSrcPos;
	TItemPos	TDstPos;
} TPacketDGPrivateShopItemCheckout;

/* Database <-> Game */
typedef struct SPacketPrivateShopItemMove
{
	DWORD		dwShopID;
	WORD		wPos;
	WORD		wChangePos;
} TPacketPrivateShopItemMove;

typedef struct SPacketPrivateShopItemPriceChange
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketPrivateShopItemPriceChange;

typedef struct SPacketPrivateShopTitleChange
{
	DWORD		dwPID;
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
} TPacketPrivateShopTitleChange;

typedef struct SPacketGDPrivateShopWarpReq
{
	DWORD		dwPID;
	DWORD		dwMapIndex;
	WORD		wListenPort;
	BYTE		bChannel;
} TPacketGDPrivateShopWarpReq;

typedef struct SPacketDGPrivateShopWarp
{
	WORD		wListenPort;
	long		lAddr;
} TPacketDGPrivateShopWarp;

typedef struct SPacketGDPrivateShopSlotUnlockReq
{
	DWORD		dwPID;
	WORD		wCount;
} TPacketGDPrivateShopSlotUnlockReq;

typedef struct SPacketGDPrivateShopSlotUnlockRes
{
	DWORD		dwShopID;
	WORD		wUnlockedSlots;
} TPacketGDPrivateShopSlotUnlockRes;
#endif

#pragma pack()
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
