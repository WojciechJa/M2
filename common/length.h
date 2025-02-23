#ifndef __INC_METIN_II_LENGTH_H__
#define __INC_METIN_II_LENGTH_H__

#include "CommonDefines.h"

#define WORD_MAX 0xffff
enum EMisc
{
	MAX_HOST_LENGTH			= 15,
	IP_ADDRESS_LENGTH		= 15,
	LOGIN_MAX_LEN			= 30,
	PASSWD_MAX_LEN			= 16,
#ifdef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT		= 5,
#else
	PLAYER_PER_ACCOUNT		= 4,
#endif
	ACCOUNT_STATUS_MAX_LEN	= 8,
	CHARACTER_NAME_MAX_LEN	= 24,
	SHOP_SIGN_MAX_LEN		= 32,
	INVENTORY_PAGE_COLUMN	= 5, // 5 (default)
	INVENTORY_PAGE_ROW		= 9, // 9 (default)
	INVENTORY_PAGE_SIZE		= INVENTORY_PAGE_COLUMN*INVENTORY_PAGE_ROW,
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	INVENTORY_PAGE_COUNT	= 4, // 2 (default)
#else
	INVENTORY_PAGE_COUNT	= 2, // 2 (default)
#endif
	INVENTORY_MAX_NUM		= INVENTORY_PAGE_SIZE*INVENTORY_PAGE_COUNT, // 90 (default)
	ABILITY_MAX_NUM			= 50,
	EMPIRE_MAX_NUM			= 4,
	BANWORD_MAX_LEN			= 24,
	SMS_MAX_LEN				= 80,
	MOBILE_MAX_LEN			= 32,
	SOCIAL_ID_MAX_LEN		= 18,

	GUILD_NAME_MAX_LEN		= 12,
	
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	MAX_LOCATION_POSITION = 25,
	MAX_LOCATION_CHAR_STRING = 2048,
#endif

	SHOP_HOST_ITEM_MAX_NUM	= 40,
	SHOP_GUEST_ITEM_MAX_NUM = 18,
	
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	OFFLINE_SHOP_HOST_ITEM_MAX_NUM = 120,
#endif

	SHOP_PRICELIST_MAX_NUM	= 40,

	CHAT_MAX_LEN			= 512,

	QUICKSLOT_MAX_NUM		= 36,

	JOURNAL_MAX_NUM			= 2,

	QUERY_MAX_LEN			= 8192,

	FILE_MAX_LEN			= 128,

	PLAYER_EXP_TABLE_MAX	= 200,
	PLAYER_MAX_LEVEL_CONST	= 250,

	GUILD_MAX_LEVEL			= 20,
	MOB_MAX_LEVEL			= 100,

	ATTRIBUTE_MAX_VALUE		= 20,
	CHARACTER_PATH_MAX_NUM	= 64,
	SKILL_MAX_NUM			= 255,
	SKILLBOOK_DELAY_MIN		= 64800,
	SKILLBOOK_DELAY_MAX		= 108000,
	SKILL_MAX_LEVEL			= 40,

	APPLY_NAME_MAX_LEN		= 32,
	EVENT_FLAG_NAME_MAX_LEN = 32,

	MOB_SKILL_MAX_NUM		= 5,
#ifdef TITLE_SYSTEM_BYLUZER
	TITLE_MAX_NUM			= 21,
#endif
    POINT_MAX_NUM = 255,
	DRAGON_SOUL_BOX_SIZE = 32,
	DRAGON_SOUL_BOX_COLUMN_NUM = 8,
	DRAGON_SOUL_BOX_ROW_NUM = DRAGON_SOUL_BOX_SIZE / DRAGON_SOUL_BOX_COLUMN_NUM,
	DRAGON_SOUL_REFINE_GRID_SIZE = 15,
	MAX_AMOUNT_OF_MALL_BONUS	= 20,

	WEAR_MAX_NUM				= 54,

#ifdef ENABLE_LONG_LONG
	GOLD_MAX = 1000000000000001LL,
#else
	// LIMIT_GOLD
	GOLD_MAX = 2000000000,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	CHEQUE_MAX = 1000000000000001LL,
	TAX_MUL = 3,
	CHEQUE_NAME_VALUE = 1000000000,
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	PO_MAX = 999999,
#endif
	//END_LIMIT_GOLD

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	SKILL_BOOK_INVENTORY_MAX_NUM	= 135,
	UPGRADE_ITEMS_INVENTORY_MAX_NUM	= 135,
	STONE_INVENTORY_MAX_NUM	= 135,
	BOX_INVENTORY_MAX_NUM	= 135,
	EFSUN_INVENTORY_MAX_NUM	= 135,
	CICEK_INVENTORY_MAX_NUM	= 135,
#endif

	BELT_INVENTORY_SLOT_WIDTH = 4,
	BELT_INVENTORY_SLOT_HEIGHT= 4,

	BELT_INVENTORY_SLOT_COUNT = BELT_INVENTORY_SLOT_WIDTH * BELT_INVENTORY_SLOT_HEIGHT,
	
#ifdef ENABLE_RESP_SYSTEM
	RESP_TELEPORT_COOLDOWN = 1,
#endif

};

#ifdef ENABLE_SWITCHBOT

enum SwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	//1 = Switching Item
	//2 = Yang
	SWITCHBOT_PRICE_TYPE = 2,
	//Amount Swtichting Items the Yang-Price
	SWITCHBOT_PRICE_AMOUNT = 25000,
	SWITCHBOT_ZMIANKA_VNUM = 79011,
	SWITCHBOT_ZMIANKA_GLOVE_VNUM = 79012,
};

const DWORD c_arSwitchingItems[3] =
{
	39028,
	71084,
	76014,
};
#endif

enum EWearPositions
{
	WEAR_BODY,		// 0
	WEAR_HEAD,		// 1
	WEAR_FOOTS,		// 2
	WEAR_WRIST,		// 3
	WEAR_WEAPON,	// 4
	WEAR_NECK,		// 5
	WEAR_EAR,		// 6
	WEAR_UNIQUE1,	// 7
	WEAR_UNIQUE2,	// 8
	WEAR_ARROW,		// 9
	WEAR_SHIELD,	// 10
    WEAR_ABILITY1,  // 11
    WEAR_ABILITY2,  // 12
    WEAR_ABILITY3,  // 13
    WEAR_ABILITY4,  // 14
    WEAR_ABILITY5,  // 15
    WEAR_ABILITY6,  // 16
    WEAR_ABILITY7,  // 17
    WEAR_ABILITY8,  // 18
	WEAR_COSTUME_BODY,	// 19
	WEAR_COSTUME_HAIR,	// 20
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	WEAR_COSTUME_MOUNT, // 21
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	WEAR_COSTUME_MOUNT, // 21
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	WEAR_COSTUME_ACCE, // 22
#endif
#ifdef ENABLE_STOLE_COSTUME
	WEAR_COSTUME_STOLE, //23
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	WEAR_COSTUME_WEAPON, //24
#endif
#ifdef ENABLE_AURA_SYSTEM
	WEAR_COSTUME_AURA,		// 24
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	WEAR_COSTUME_EMBLEMAT,
#endif
	WEAR_RING1,				//25
	WEAR_RING2,
	WEAR_BELT,

#ifdef ENABLE_NEW_PET_SYSTEM
	WEAR_NEW_PET,
	WEAR_NEW_PET_EQ0,
	WEAR_NEW_PET_EQ1,
	WEAR_NEW_PET_EQ2,
#endif
#ifdef ENABLE_GLOVE_SYSTEM
	WEAR_GLOVE,
#endif
#ifdef ENABLE_SYSTEM_RUNE
	WEAR_RUNE, // 33
	WEAR_RUNE_RED,
	WEAR_RUNE_BLUE,
	WEAR_RUNE_GREEN,
	WEAR_RUNE_YELLOW,
	WEAR_RUNE_BLACK, //38
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
	WEAR_ARTEFAKT1,
	WEAR_ARTEFAKT2,
	WEAR_ARTEFAKT3,
	WEAR_ARTEFAKT4,
	WEAR_ARTEFAKT5,
	WEAR_ARTEFAKT6,
	WEAR_ARTEFAKT7,
	WEAR_ARTEFAKT8,
	WEAR_ARTEFAKT9,
	WEAR_ARTEFAKT10,
	WEAR_ARTEFAKT11,
	WEAR_ARTEFAKT12,
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	WEAR_MOUNT_EQUIPMENT,
#endif
	WEAR_MAX = 54
};

enum EDragonSoulDeckType
{
	DRAGON_SOUL_DECK_0,
	DRAGON_SOUL_DECK_1,
	DRAGON_SOUL_DECK_MAX_NUM = 2,

	DRAGON_SOUL_DECK_RESERVED_MAX_NUM = 3,
};

#ifdef ENABLE_SYSTEM_RUNE
#ifdef ENABLE_EXTRABONUS_SYSTEM
enum ERuneConfig
{
	RUNE_EXTRABONUS_VALUE = 20,
	RUNE_AFFECT_START = 700 // 700 + item.GetType() = 740,741,742 itd.. // char_item.cpp
};
#endif
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
#ifdef ENABLE_EXTRABONUS_SYSTEM
enum EPetConfig
{
	PET_EXTRABONUS_VALUE = 20,
	PET_AFFECT_START = 500 // + item.getvalue(0)
};
#endif
#endif

enum ESex
{
	SEX_MALE,
	SEX_FEMALE
};

enum EDirection
{
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_MAX_NUM
};

#define ABILITY_MAX_LEVEL	10

enum EAbilityDifficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERY_HARD,
	DIFFICULTY_NUM_TYPES
};

enum EAbilityCategory
{
	CATEGORY_PHYSICAL,
	CATEGORY_MENTAL,
	CATEGORY_ATTRIBUTE,
	CATEGORY_NUM_TYPES
};

enum EJobs
{
	JOB_WARRIOR,
	JOB_ASSASSIN,
	JOB_SURA,
	JOB_SHAMAN,
#ifdef ENABLE_WOLFMAN_CHARACTER
	JOB_WOLFMAN,
#endif
	JOB_MAX_NUM
};

enum ESkillGroups
{
	SKILL_GROUP_MAX_NUM = 2,
};

enum ERaceFlags
{
	RACE_FLAG_ANIMAL	= (1 << 0),
	RACE_FLAG_UNDEAD	= (1 << 1),
	RACE_FLAG_DEVIL		= (1 << 2),
	RACE_FLAG_HUMAN		= (1 << 3),
	RACE_FLAG_ORC		= (1 << 4),
	RACE_FLAG_MILGYO	= (1 << 5),
	RACE_FLAG_INSECT	= (1 << 6),
	RACE_FLAG_FIRE		= (1 << 7),
	RACE_FLAG_ICE		= (1 << 8),
	RACE_FLAG_DESERT	= (1 << 9),
	RACE_FLAG_TREE		= (1 << 10),
	RACE_FLAG_ATT_ELEC	= (1 << 11),
	RACE_FLAG_ATT_FIRE	= (1 << 12),
	RACE_FLAG_ATT_ICE	= (1 << 13),
	RACE_FLAG_ATT_WIND	= (1 << 14),
	RACE_FLAG_ATT_EARTH	= (1 << 15),
	RACE_FLAG_ATT_DARK	= (1 << 16),
	RACE_FLAG_BOSS		= (1 << 17),
	RACE_FLAG_WLADCA	= (1 << 18),
	RACE_FLAG_STONE		= (1 << 19),
	RACE_FLAG_DUNGEON	= (1 << 20),
	RACE_FLAG_LEGENDA	= (1 << 21),
};

enum ELoads
{
	LOAD_NONE,
	LOAD_LIGHT,
	LOAD_NORMAL,
	LOAD_HEAVY,
	LOAD_MASSIVE
};

enum
{
	QUICKSLOT_TYPE_NONE,
	QUICKSLOT_TYPE_ITEM,
	QUICKSLOT_TYPE_SKILL,
	QUICKSLOT_TYPE_COMMAND,
	QUICKSLOT_TYPE_MAX_NUM,
};

#ifdef __PREMIUM_PRIVATE_SHOP__
enum EPrivateShop
{
	INVENTORY_PAGE_MAX_NUM			= INVENTORY_PAGE_COUNT,
	INVENTORY_PAGE_WIDTH			= 5,
	INVENTORY_PAGE_HEIGHT			= 9,
	INVENTORY_PAGE_ITEM_MAX_NUM		= INVENTORY_PAGE_WIDTH * INVENTORY_PAGE_HEIGHT,

	PRIVATE_SHOP_PAGE_MAX_NUM		= 2,
	PRIVATE_SHOP_WIDTH				= 8,
	PRIVATE_SHOP_HEIGHT				= 8,
	PRIVATE_SHOP_PAGE_ITEM_MAX_NUM	= PRIVATE_SHOP_WIDTH * PRIVATE_SHOP_HEIGHT,
	PRIVATE_SHOP_HOST_ITEM_MAX_NUM	= (PRIVATE_SHOP_WIDTH * PRIVATE_SHOP_HEIGHT) * PRIVATE_SHOP_PAGE_MAX_NUM,
	PRIVATE_SHOP_LOCKED_SLOT_MAX_NUM = PRIVATE_SHOP_HOST_ITEM_MAX_NUM / 2,
	PRIVATE_SHOP_SLOT_UNLOCK_ITEM = 72357,
	PRIVATE_SHOP_MAX_PREMIUM_TIME = 60 * 60 * 24 * 7,
	SELECTED_ITEM_MAX_NUM = 10,

	VALID_SALE_DAY_INTERVAL					= 30,
	VALID_MARKET_PRICE_DAY_INTERVAL			= 3,
	MARKET_ITEM_PRICE_UPDATE_SEC_INTERVAL	= 3600,

	TITLE_MAX_LEN = 32,
	TITLE_MIN_LEN = 3,
};

enum EShopSearchMode
{
	MODE_NONE,
	MODE_LOOKING,
	MODE_TRADING,
};

enum EPrivateShopState
{
	STATE_UNAVAILABLE,
	STATE_CLOSED,
	STATE_OPEN,
	STATE_MODIFY
};

enum EPrivateShopSearchState
{
	STATE_NON_EXISTENT,
	STATE_REMOVED,
	STATE_AVAILABLE,
	STATE_RESTRICTED,
};
#endif

enum EParts
{
	PART_MAIN,
	PART_WEAPON,
	PART_HEAD,
	PART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PART_ACCE,
#endif
#ifdef ENABLE_AURA_SYSTEM
	PART_AURA,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	PART_EMBLEMAT,
#endif
	PART_MAX_NUM,
	PART_WEAPON_SUB,
};

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MONARCH_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO, //11
#endif
#if defined(__CHATTING_WINDOW_RENEWAL__)
	CHAT_TYPE_EXP_INFO,
	CHAT_TYPE_ITEM_INFO,
	CHAT_TYPE_MONEY_INFO,
#endif
	CHAT_TYPE_MAX_NUM
};

enum EWhisperType
{
	WHISPER_TYPE_NORMAL		= 0,
	WHISPER_TYPE_NOT_EXIST		= 1,
	WHISPER_TYPE_TARGET_BLOCKED	= 2,
	WHISPER_TYPE_SENDER_BLOCKED	= 3,
	WHISPER_TYPE_ERROR		= 4,
	WHISPER_TYPE_GM			= 5,
#if defined(BL_OFFLINE_MESSAGE)
	WHISPER_TYPE_OFFLINE	= 6,
#endif
	WHISPER_TYPE_SYSTEM		= 0xFF
};

enum ECharacterPosition
{
	POSITION_GENERAL,
	POSITION_BATTLE,
	POSITION_DYING,
	POSITION_SITTING_CHAIR,
	POSITION_SITTING_GROUND,
	POSITION_INTRO,
	POSITION_MAX_NUM
};

enum EGMLevels
{
	GM_PLAYER,
	GM_LOW_WIZARD,
	GM_WIZARD,
	GM_HIGH_WIZARD,
	GM_GOD,
	GM_IMPLEMENTOR,
	GM_DISABLE,
};

enum EMobRank
{
	MOB_RANK_PAWN,
	MOB_RANK_S_PAWN,
	MOB_RANK_KNIGHT,
	MOB_RANK_S_KNIGHT,
	MOB_RANK_BOSS,
	MOB_RANK_KING,
	MOB_RANK_ELITKA,
	MOB_RANK_LEGENDA,
	MOB_RANK_MAX_NUM
};

enum ECharType
{
	CHAR_TYPE_MONSTER,
	CHAR_TYPE_NPC,
	CHAR_TYPE_STONE,
	CHAR_TYPE_WARP,
	CHAR_TYPE_DOOR,
	CHAR_TYPE_BUILDING,
	CHAR_TYPE_PC,
	CHAR_TYPE_POLYMORPH_PC,
	CHAR_TYPE_HORSE,
	CHAR_TYPE_GOTO
};

enum EBattleType
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGE,
	BATTLE_TYPE_MAGIC,
	BATTLE_TYPE_SPECIAL,
	BATTLE_TYPE_POWER,
	BATTLE_TYPE_TANKER,
	BATTLE_TYPE_SUPER_POWER,
	BATTLE_TYPE_SUPER_TANKER,
	BATTLE_TYPE_MAX_NUM
};

enum EApplyTypes
{
	APPLY_NONE,			// 0
	APPLY_MAX_HP,		// 1
	APPLY_MAX_SP,		// 2
	APPLY_CON,			// 3
	APPLY_INT,			// 4
	APPLY_STR,			// 5
	APPLY_DEX,			// 6
	APPLY_ATT_SPEED,	// 7
	APPLY_MOV_SPEED,	// 8
	APPLY_CAST_SPEED,	// 9
	APPLY_HP_REGEN,		// 10
	APPLY_SP_REGEN,		// 11
	APPLY_POISON_PCT,	// 12
	APPLY_STUN_PCT,		// 13
	APPLY_SLOW_PCT,		// 14
	APPLY_CRITICAL_PCT,		// 15
	APPLY_PENETRATE_PCT,	// 16
	APPLY_ATTBONUS_HUMAN,	// 17
	APPLY_ATTBONUS_ANIMAL,	// 18
	APPLY_ATTBONUS_ORC,		// 19
	APPLY_ATTBONUS_MILGYO,	// 20
	APPLY_ATTBONUS_UNDEAD,	// 21
	APPLY_ATTBONUS_DEVIL,	// 22
	APPLY_STEAL_HP,			// 23
	APPLY_STEAL_SP,			// 24
	APPLY_MANA_BURN_PCT,	// 25
	APPLY_DAMAGE_SP_RECOVER,	// 26
	APPLY_BLOCK,			// 27
	APPLY_DODGE,			// 28
	APPLY_RESIST_SWORD,		// 29
	APPLY_RESIST_TWOHAND,	// 30
	APPLY_RESIST_DAGGER,	// 31
	APPLY_RESIST_BELL,		// 32
	APPLY_RESIST_FAN,		// 33
	APPLY_RESIST_BOW,		// 34
	APPLY_RESIST_FIRE,		// 35
	APPLY_RESIST_ELEC,		// 36
	APPLY_RESIST_MAGIC,		// 37
	APPLY_RESIST_WIND,		// 38
	APPLY_REFLECT_MELEE,	// 39
	APPLY_REFLECT_CURSE,	// 40
	APPLY_POISON_REDUCE,	// 41
	APPLY_KILL_SP_RECOVER,	// 42
	APPLY_EXP_DOUBLE_BONUS,	// 43
	APPLY_GOLD_DOUBLE_BONUS,	// 44
	APPLY_ITEM_DROP_BONUS,	// 45
	APPLY_POTION_BONUS,		// 46
	APPLY_KILL_HP_RECOVER,	// 47
	APPLY_IMMUNE_STUN,		// 48
	APPLY_IMMUNE_SLOW,		// 49
	APPLY_IMMUNE_FALL,		// 50
	APPLY_SKILL,			// 51
	APPLY_BOW_DISTANCE,		// 52
	APPLY_ATT_GRADE_BONUS,	// 53
	APPLY_DEF_GRADE_BONUS,	// 54
	APPLY_MAGIC_ATT_GRADE,	// 55
	APPLY_MAGIC_DEF_GRADE,	// 56
	APPLY_CURSE_PCT,		// 57
	APPLY_MAX_STAMINA,		// 58
	APPLY_ATTBONUS_WARRIOR,	// 59
	APPLY_ATTBONUS_ASSASSIN,	// 60
	APPLY_ATTBONUS_SURA,	// 61
	APPLY_ATTBONUS_SHAMAN,	// 62
	APPLY_ATTBONUS_MONSTER,	// 63
	APPLY_MALL_ATTBONUS,	//64
	APPLY_MALL_DEFBONUS,	//65
	APPLY_MALL_EXPBONUS,	//66
	APPLY_MALL_ITEMBONUS,	//67
	APPLY_MALL_GOLDBONUS,	//68
	APPLY_MAX_HP_PCT,	//69
	APPLY_MAX_SP_PCT,	//70
	APPLY_SKILL_DAMAGE_BONUS,	//71
	APPLY_NORMAL_HIT_DAMAGE_BONUS,	//72
	APPLY_SKILL_DEFEND_BONUS,	//73
	APPLY_NORMAL_HIT_DEFEND_BONUS,	//74
	APPLY_PC_BANG_EXP_BONUS, 	//75
	APPLY_PC_BANG_DROP_BONUS, 	//76

	APPLY_EXTRACT_HP_PCT,	//77

	APPLY_RESIST_WARRIOR,	//78
	APPLY_RESIST_ASSASSIN,	//79
	APPLY_RESIST_SURA,	//80
	APPLY_RESIST_SHAMAN,
	APPLY_ENERGY,
	APPLY_DEF_GRADE,
	APPLY_COSTUME_ATTR_BONUS,
	APPLY_MAGIC_ATTBONUS_PER,
	APPLY_MELEE_MAGIC_ATTBONUS_PER,

	APPLY_RESIST_ICE,
	APPLY_RESIST_EARTH,
	APPLY_RESIST_DARK,

	APPLY_ANTI_CRITICAL_PCT,
	APPLY_ANTI_PENETRATE_PCT,

#ifdef ENABLE_WOLFMAN_CHARACTER
	APPLY_BLEEDING_REDUCE			= 92,		//92
	APPLY_BLEEDING_PCT				= 93,		//93
	APPLY_ATTBONUS_WOLFMAN			= 94,
	APPLY_RESIST_WOLFMAN			= 95,
	APPLY_RESIST_CLAW				= 96,
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	APPLY_ACCEDRAIN_RATE			= 97,			//97
#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	APPLY_RESIST_MAGIC_REDUCTION	= 98,	//98
#endif

	APPLY_ENCHANT_ELECT				= 99,
	APPLY_ENCHANT_FIRE				= 100,
	APPLY_ENCHANT_ICE				= 101,
	APPLY_ENCHANT_WIND				= 102,
	APPLY_ENCHANT_EARTH				= 103,
	APPLY_ENCHANT_DARK				= 104,
	APPLY_RESIST_HUMAN,				// 105
	APPLY_ATTBONUS_BOSS,			// 106
	APPLY_ATTBONUS_WLADCA,			// 107
	APPLY_ATTBONUS_STONE			= 108,
	APPLY_RESIST_BOSS,				// 109
	APPLY_RESIST_WLADCA,			// 110
	APPLY_RESIST_MONSTER,			// 111
	APPLY_STAT_BONUS,				// 112
	APPLY_ATTBONUS_DUNGEON,			// 113
	APPLY_ATTBONUS_LEGENDA,			// 114
	APPLY_ATTBONUS_KLASY,			// 115
	APPLY_RESIST_KLASY,				// 116

#if defined(__ITEM_APPLY_RANDOM__)
	APPLY_RANDOM,
#endif

	APPLY_DMG_BONUS,				// 117
	APPLY_FINAL_DMG_BONUS,			// 118

	MAX_APPLY_NUM					= 127,
};

enum EOnClickEvents
{
	ON_CLICK_NONE,
	ON_CLICK_SHOP,
	ON_CLICK_TALK,
	ON_CLICK_MAX_NUM
};

enum EOnIdleEvents
{
	ON_IDLE_NONE,
	ON_IDLE_GENERAL,
	ON_IDLE_MAX_NUM
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	BUFF_EQUIPMENT,
#endif
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	SKILL_BOOK_INVENTORY,
	UPGRADE_ITEMS_INVENTORY,
	STONE_INVENTORY,
	BOX_INVENTORY,
	EFSUN_INVENTORY,
	CICEK_INVENTORY,
#endif
	GROUND,
	WINDOW_TYPE_MAX,
};

enum EMobSizes
{
	MOBSIZE_RESERVED,
	MOBSIZE_SMALL,
	MOBSIZE_MEDIUM,
	MOBSIZE_BIG
};

enum EAIFlags
{
	AIFLAG_AGGRESSIVE	= (1 << 0),
	AIFLAG_NOMOVE	= (1 << 1),
	AIFLAG_COWARD	= (1 << 2),
	AIFLAG_NOATTACKSHINSU	= (1 << 3),
	AIFLAG_NOATTACKJINNO	= (1 << 4),
	AIFLAG_NOATTACKCHUNJO	= (1 << 5),
	AIFLAG_ATTACKMOB = (1 << 6 ),
	AIFLAG_BERSERK	= (1 << 7),
	AIFLAG_STONESKIN	= (1 << 8),
	AIFLAG_GODSPEED	= (1 << 9),
	AIFLAG_DEATHBLOW	= (1 << 10),
	AIFLAG_REVIVE		= (1 << 11),
};

enum EMobStatType
{
	MOB_STATTYPE_POWER,
	MOB_STATTYPE_TANKER,
	MOB_STATTYPE_SUPER_POWER,
	MOB_STATTYPE_SUPER_TANKER,
	MOB_STATTYPE_RANGE,
	MOB_STATTYPE_MAGIC,
	MOB_STATTYPE_MAX_NUM
};

enum EImmuneFlags
{
	IMMUNE_STUN		= (1 << 0),
	IMMUNE_SLOW		= (1 << 1),
	IMMUNE_FALL		= (1 << 2),
	IMMUNE_CURSE	= (1 << 3),
	IMMUNE_POISON	= (1 << 4),
	IMMUNE_TERROR	= (1 << 5),
	IMMUNE_REFLECT	= (1 << 6),
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	MOB_ENCHANT_BLEEDING,
#endif
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_CLAW_AS_DAGGER)
	MOB_RESIST_CLAW,
#endif
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	MOB_RESIST_BLEEDING,
#endif
	MOB_RESISTS_MAX_NUM
};

enum
{
	SKILL_ATTR_TYPE_NORMAL = 1,
	SKILL_ATTR_TYPE_MELEE,
	SKILL_ATTR_TYPE_RANGE,
	SKILL_ATTR_TYPE_MAGIC
		/*
		   SKILL_ATTR_TYPE_FIRE,
		   SKILL_ATTR_TYPE_ICE,
		   SKILL_ATTR_TYPE_ELEC,
		   SKILL_ATTR_TYPE_DARK,
		   */
};

enum
{
	SKILL_NORMAL,
	SKILL_MASTER,
	SKILL_GRAND_MASTER,
	SKILL_PERFECT_MASTER,
};

enum EGuildWarType
{
	GUILD_WAR_TYPE_FIELD,
	GUILD_WAR_TYPE_BATTLE,
	GUILD_WAR_TYPE_FLAG,
	GUILD_WAR_TYPE_MAX_NUM
};

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,
	GUILD_WAR_OVER,
	GUILD_WAR_RESERVE,

	GUILD_WAR_DURATION = 30*60,
	GUILD_WAR_WIN_POINT = 1000,
	GUILD_WAR_LADDER_HALF_PENALTY_TIME = 12*60*60,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
#ifdef ENABLE_ITEM_ATTR_COSTUME
	ATTRIBUTE_SET_COSTUME_BODY,
	ATTRIBUTE_SET_COSTUME_HAIR,
#if defined(ENABLE_ITEM_ATTR_COSTUME) && defined(ENABLE_WEAPON_COSTUME_SYSTEM)
	ATTRIBUTE_SET_COSTUME_WEAPON,
#endif
#endif
#ifdef ENABLE_BELT_ATTRIBUTES
	ATTRIBUTE_SET_BELT,
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
	ATTRIBUTE_SET_GLOVE,
#endif

	ATTRIBUTE_SET_PET,
#ifdef ENABLE_STOLE_COSTUME
	ATTRIBUTE_SET_STOLE,
#endif

	ATTRIBUTE_SET_MAX_NUM
};

enum EPrivType
{
	PRIV_NONE,
	PRIV_ITEM_DROP,
	PRIV_GOLD_DROP,
	PRIV_GOLD10_DROP,
	PRIV_EXP_PCT,
	MAX_PRIV_NUM,
};

enum EMoneyLogType
{
	MONEY_LOG_RESERVED,
	MONEY_LOG_MONSTER,
	MONEY_LOG_SHOP,
	MONEY_LOG_REFINE,
	MONEY_LOG_QUEST,
	MONEY_LOG_GUILD,
	MONEY_LOG_MISC,
	MONEY_LOG_MONSTER_KILL,
	MONEY_LOG_DROP,
	MONEY_LOG_TYPE_MAX_NUM,
};

enum EPremiumTypes
{
	PREMIUM_EXP,
	PREMIUM_ITEM,
	PREMIUM_SAFEBOX,
	PREMIUM_AUTOLOOT,
	PREMIUM_FISH_MIND,
	PREMIUM_MARRIAGE_FAST,
	PREMIUM_GOLD,
	PREMIUM_PICKUP,
#ifdef __PREMIUM_PRIVATE_SHOP__
	PREMIUM_PRIVATE_SHOP,
#endif
	PREMIUM_MAX_NUM = 9
};

enum SPECIAL_EFFECT
{
	SE_NONE,

	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY,
	SE_LEVELUP_UNDER_15_FOR_GERMANY,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SE_EFFECT_ACCE_SUCCEDED,
	SE_EFFECT_ACCE_EQUIP,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	SE_PET_LV_UP,
#endif
#ifdef ENABLE_SYSTEM_RUNE
	SE_RUNA_WHITE_EFFECT,
	SE_RUNA_RED_EFFECT,
	SE_RUNA_BLUE_EFFECT,
	SE_RUNA_YELLOW_EFFECT,
	SE_RUNA_BLACK_EFFECT,
	SE_RUNA_GREEN_EFFECT,
#endif
};

#include "item_length.h"

enum EDragonSoulRefineWindowSize
{
	DRAGON_SOUL_REFINE_GRID_MAX = 15,
};

enum EMisc2
{
	DRAGON_SOUL_EQUIP_SLOT_START = INVENTORY_MAX_NUM + WEAR_MAX_NUM,
	DRAGON_SOUL_EQUIP_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_START + (DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM),
	DRAGON_SOUL_EQUIP_RESERVED_SLOT_END = DRAGON_SOUL_EQUIP_SLOT_END + (DS_SLOT_MAX * DRAGON_SOUL_DECK_RESERVED_MAX_NUM),

	BELT_INVENTORY_SLOT_START = DRAGON_SOUL_EQUIP_RESERVED_SLOT_END,
	BELT_INVENTORY_SLOT_END = BELT_INVENTORY_SLOT_START + BELT_INVENTORY_SLOT_COUNT,

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	SKILL_BOOK_INVENTORY_SLOT_START	= BELT_INVENTORY_SLOT_END,
	SKILL_BOOK_INVENTORY_SLOT_END = SKILL_BOOK_INVENTORY_SLOT_START + SKILL_BOOK_INVENTORY_MAX_NUM,
	
	UPGRADE_ITEMS_INVENTORY_SLOT_START = SKILL_BOOK_INVENTORY_SLOT_END,
	UPGRADE_ITEMS_INVENTORY_SLOT_END = UPGRADE_ITEMS_INVENTORY_SLOT_START + UPGRADE_ITEMS_INVENTORY_MAX_NUM,
	
	STONE_INVENTORY_SLOT_START = UPGRADE_ITEMS_INVENTORY_SLOT_END,
	STONE_INVENTORY_SLOT_END = STONE_INVENTORY_SLOT_START + STONE_INVENTORY_MAX_NUM,
	
	BOX_INVENTORY_SLOT_START = STONE_INVENTORY_SLOT_END,
	BOX_INVENTORY_SLOT_END = BOX_INVENTORY_SLOT_START + BOX_INVENTORY_MAX_NUM,
	
	EFSUN_INVENTORY_SLOT_START = BOX_INVENTORY_SLOT_END,
	EFSUN_INVENTORY_SLOT_END = EFSUN_INVENTORY_SLOT_START + EFSUN_INVENTORY_MAX_NUM,
	
	CICEK_INVENTORY_SLOT_START = EFSUN_INVENTORY_SLOT_END,
	CICEK_INVENTORY_SLOT_END = CICEK_INVENTORY_SLOT_START + CICEK_INVENTORY_MAX_NUM,
#endif

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	INVENTORY_AND_EQUIP_SLOT_MAX = CICEK_INVENTORY_SLOT_END,
#else
	INVENTORY_AND_EQUIP_SLOT_MAX = BELT_INVENTORY_SLOT_END,
#endif
};

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
enum EBuffWindowSlots
{
	BUFF_WEAR_WEAPON,		// 0
	BUFF_WEAR_HEAD,			// 1
	BUFF_WEAR_BODY,			// 2
	BUFF_WEAR_UNIQUE,		// 3
	BUFF_SLOT_SKILLBOOK,		// 4
	BUFF_SLOT_SKILLBOOK_COMMON_1,		// 5
	BUFF_SLOT_SKILLBOOK_COMMON_2,		// 6
	
	BUFF_SLOT_CHANGE_NAME,		// 7
	BUFF_SLOT_CHANGE_SEX,		// 8
	BUFF_SLOT_RESET_SKILL,		// 9
	
	BUFF_WINDOW_SLOT_MAX_NUM,
};
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
enum EBattlePassTypes
{
	BATTLE_PASS_MONSTER,
	BATTLE_PASS_BOSS,
	BATTLE_PASS_STONE,
	BATTLE_PASS_DUNGEON,
	BATTLE_PASS_CHAT,
	BATTLE_PASS_REFINE,
};
#endif

#pragma pack(push, 1)

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
        window_type = INVENTORY;
		cell = WORD_MAX;
    }

	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

	bool IsValidItemPosition() const
	{
		switch (window_type)
		{
		case RESERVED_WINDOW:
			return false;
		case INVENTORY:
		case EQUIPMENT:
		case BELT_INVENTORY:
			return cell < INVENTORY_AND_EQUIP_SLOT_MAX;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

		case SAFEBOX:
		case MALL:
			return false;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			return cell < (BUFF_WINDOW_SLOT_MAX_NUM);
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
		case SKILL_BOOK_INVENTORY:
		case UPGRADE_ITEMS_INVENTORY:
		case STONE_INVENTORY:
		case BOX_INVENTORY:
		case EFSUN_INVENTORY:
		case CICEK_INVENTORY:
#endif
		default:
			return false;
		}
		return false;
	}

	bool IsEquipPosition() const
	{
		return ((INVENTORY == window_type || EQUIPMENT == window_type) && cell >= INVENTORY_MAX_NUM && cell < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
			|| IsDragonSoulEquipPosition();
	}

	bool IsDragonSoulEquipPosition() const
	{
		return (window_type == INVENTORY) && (DRAGON_SOUL_EQUIP_SLOT_START <= cell) && (DRAGON_SOUL_EQUIP_SLOT_END > cell); // @fixme184
	}

	bool IsBeltInventoryPosition() const
	{
		return (BELT_INVENTORY_SLOT_START <= cell) && (BELT_INVENTORY_SLOT_END > cell);
	}

	bool IsDefaultInventoryPosition() const
	{
		return INVENTORY == window_type && cell < INVENTORY_MAX_NUM;
	}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	bool IsBuffEquipmentPosition() const
	{
		return BUFF_EQUIPMENT == window_type && cell < (BUFF_WINDOW_SLOT_MAX_NUM);
	}
#endif
#ifdef ENABLE_SWITCHBOT
	bool IsSwitchbotPosition() const
	{
		return SWITCHBOT == window_type && cell < SWITCHBOT_SLOT_COUNT;
	}
#endif
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	bool IsSkillBookInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= SKILL_BOOK_INVENTORY_SLOT_START && cell < SKILL_BOOK_INVENTORY_SLOT_END);
	}
	bool IsUpgradeItemsInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= UPGRADE_ITEMS_INVENTORY_SLOT_START && cell < UPGRADE_ITEMS_INVENTORY_SLOT_END);
	}
	bool IsStoneInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= STONE_INVENTORY_SLOT_START && cell < STONE_INVENTORY_SLOT_END);
	}
	bool IsBoxInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= BOX_INVENTORY_SLOT_START && cell < BOX_INVENTORY_SLOT_END);
	}
	bool IsEfsunInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= EFSUN_INVENTORY_SLOT_START && cell < EFSUN_INVENTORY_SLOT_END);
	}
	bool IsCicekInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= CICEK_INVENTORY_SLOT_START && cell < CICEK_INVENTORY_SLOT_END);
	}
#endif
	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}
	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;

const TItemPos NPOS (RESERVED_WINDOW, WORD_MAX);

typedef struct SItemPosEx
{
	TItemPos pos;
	int id{ 0 };
} TItemPosEx;

typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#pragma pack(pop)

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
