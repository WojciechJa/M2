#pragma once

#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"

enum
{
	HEADER_CG_HANDSHAKE				= 0xff,
	HEADER_CG_PONG					= 0xfe,
	HEADER_CG_TIME_SYNC				= 0xfc,
	HEADER_CG_KEY_AGREEMENT			= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_

	HEADER_CG_LOGIN					= 1,
	HEADER_CG_ATTACK				= 2,
	HEADER_CG_CHAT					= 3,
	HEADER_CG_CHARACTER_CREATE		= 4,
	HEADER_CG_CHARACTER_DELETE		= 5,
	HEADER_CG_CHARACTER_SELECT		= 6,
	HEADER_CG_MOVE					= 7,
	HEADER_CG_SYNC_POSITION			= 8,
	HEADER_CG_ENTERGAME				= 10,

	HEADER_CG_ITEM_USE				= 11,
	HEADER_CG_ITEM_DROP				= 12,
	HEADER_CG_ITEM_MOVE				= 13,
	HEADER_CG_ITEM_PICKUP			= 15,

	HEADER_CG_QUICKSLOT_ADD			= 16,
	HEADER_CG_QUICKSLOT_DEL			= 17,
	HEADER_CG_QUICKSLOT_SWAP		= 18,
	HEADER_CG_WHISPER				= 19,
	HEADER_CG_ITEM_DROP2			= 20,
#ifdef ENABLE_INSTANT_CHEST_OPEN
	HEADER_CG_CHEST_OPEN			= 22,
#endif
#ifdef ENABLE_INSTANT_INGOT_OPEN
	HEADER_CG_INGOT_OPEN			= 243,
#endif
	HEADER_CG_ON_CLICK				= 26,
	HEADER_CG_EXCHANGE				= 27,
	HEADER_CG_CHARACTER_POSITION	= 28,
	HEADER_CG_SCRIPT_ANSWER			= 29,
	HEADER_CG_QUEST_INPUT_STRING	= 30,
	HEADER_CG_QUEST_CONFIRM			= 31,

	HEADER_CG_SHOP					= 50,
	HEADER_CG_FLY_TARGETING			= 51,
	HEADER_CG_USE_SKILL				= 52,
	HEADER_CG_ADD_FLY_TARGETING		= 53,
	HEADER_CG_SHOOT					= 54,
	HEADER_CG_MYSHOP				= 55,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_CG_OFFLINE_SHOP			= 56,
	HEADER_CG_MY_OFFLINE_SHOP		= 57,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	HEADER_CG_WON_EXCHANGE			= 222,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM		= 60,
	HEADER_CG_TARGET			 	= 61,

	HEADER_CG_TEXT					= 64,
	HEADER_CG_WARP					= 65,
	HEADER_CG_SCRIPT_BUTTON			= 66,
	HEADER_CG_MESSENGER				= 67,

	HEADER_CG_MALL_CHECKOUT			= 69,
	HEADER_CG_SAFEBOX_CHECKIN		= 70,
	HEADER_CG_SAFEBOX_CHECKOUT		= 71,

	HEADER_CG_PARTY_INVITE			= 72,
	HEADER_CG_PARTY_INVITE_ANSWER	= 73,
	HEADER_CG_PARTY_REMOVE			= 74,
	HEADER_CG_PARTY_SET_STATE		= 75,
	HEADER_CG_PARTY_USE_SKILL		= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE		= 77,
	HEADER_CG_PARTY_PARAMETER		= 78,

	HEADER_CG_GUILD					= 80,
	HEADER_CG_ANSWER_MAKE_GUILD		= 81,

	HEADER_CG_FISHING				= 82,

	HEADER_CG_ITEM_GIVE				= 83,

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_CG_BUFF_NPC_CREATE		= 186,
	HEADER_CG_BUFF_NPC_ACTION		= 187,
#endif

	HEADER_CG_EMPIRE				= 90,

	HEADER_CG_REFINE				= 96,

	HEADER_CG_MARK_LOGIN			= 100,
	HEADER_CG_MARK_CRCLIST			= 101,
	HEADER_CG_MARK_UPLOAD			= 102,
	HEADER_CG_MARK_IDXLIST			= 104,

	HEADER_CG_HACK					= 105,
	HEADER_CG_CHANGE_NAME			= 106,
	HEADER_CG_LOGIN2				= 109,
	HEADER_CG_DUNGEON				= 110,
	HEADER_CG_LOGIN3				= 111,

	HEADER_CG_GUILD_SYMBOL_UPLOAD	= 112,
	HEADER_CG_SYMBOL_CRC			= 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM	= 114,
	// END_OF_SCRIPT_SELECT_ITEM

#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_CG_TITLE_ACTIVE			= 129,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_CG_WEEKLY_RANK			= 130,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT				= 171,
#endif
	HEADER_CG_DRAGON_SOUL_REFINE	= 205,
#ifdef ENABLE_LRN_BATTLE_PASS
	HEADER_CG_BATTLE_PASS_COLLECT	= 207,
#endif
	HEADER_CG_STATE_CHECKER			= 206,
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_CG_CUBE_RENEWAL 			= 220,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_CG_PET_ACTION			= 223,
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	HEADER_CG_SEND_LOCATION_MANAGER	= 215,
#endif
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_CG_MOVE_CHANNEL			= 240,
#endif
#ifdef ENABLE_AURA_SYSTEM
	HEADER_CG_AURA					= 232,
#endif
	HEADER_CG_GIVE_ITEMS			= 234,
// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 	HEADER_CG_MOUNT_NPC_ACTION		= 233,
// #endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_CG_PRIVATE_SHOP,
#endif

	HEADER_CG_CLIENT_VERSION		= 0xfd,
	HEADER_CG_CLIENT_VERSION2		= 0xf1,

	/********************************************************/
	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_TIME_SYNC							= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
	HEADER_GC_BINDUDP							= 0xfe,
	HEADER_GC_HANDSHAKE							= 0xff,

	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_MOVE								= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION						= 5,

	HEADER_GC_LOGIN_SUCCESS						= 6,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT				= 32,
	HEADER_GC_LOGIN_FAILURE						= 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS			= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE			= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS			= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID	= 11,

	HEADER_GC_ATTACK							= 12,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,

	HEADER_GC_MAIN_CHARACTER_OLD				= 15,
	HEADER_GC_CHARACTER_POINTS					= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE			= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE					= 19,

	HEADER_GC_ITEM_DEL							= 20,
	HEADER_GC_ITEM_SET							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	HEADER_GC_ITEM_UPDATE						= 25,

	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,

	HEADER_GC_QUICKSLOT_ADD						= 28,
	HEADER_GC_QUICKSLOT_DEL						= 29,
	HEADER_GC_QUICKSLOT_SWAP					= 30,

	HEADER_GC_ITEM_OWNERSHIP					= 31,

	HEADER_GC_WHISPER							= 34,

	HEADER_GC_MOTION							= 36,
	HEADER_GC_PARTS								= 37,

	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,

	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP                               = 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,

	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_GC_OFFLINE_SHOP						= 47,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 48,
	HEADER_GC_GOLD								= 49,
	HEADER_GC_OFFLINE_SHOP_LOG					= 52,
#endif
#ifdef ENABLE_AURA_SYSTEM
	HEADER_GC_AURA								= 157,
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_GC_BUFF_NPC_ACTION = 188,
	HEADER_GC_BUFF_NPC_USE_SKILL = 189,
#endif

	HEADER_GC_ANTYEXP 							= 58,
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	HEADER_GC_WPADANIE							= 59,
#endif

	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET			 				= 63,

	HEADER_GC_WARP								= 65,

	HEADER_GC_ADD_FLY_TARGETING					= 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_LEVEL_OLD					= 72,
	HEADER_GC_SKILL_LEVEL						= 76,

	HEADER_GC_MESSENGER							= 74,
	HEADER_GC_GUILD								= 75,

	HEADER_GC_PARTY_INVITE						= 77,
	HEADER_GC_PARTY_ADD							= 78,
	HEADER_GC_PARTY_UPDATE						= 79,
	HEADER_GC_PARTY_REMOVE						= 80,
	HEADER_GC_QUEST_INFO						= 81,
	HEADER_GC_REQUEST_MAKE_GUILD				= 82,
	HEADER_GC_PARTY_PARAMETER					= 83,

	HEADER_GC_SAFEBOX_SET						= 85,
	HEADER_GC_SAFEBOX_DEL						= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD			= 87,
	HEADER_GC_SAFEBOX_SIZE						= 88,

	HEADER_GC_FISHING							= 89,

	HEADER_GC_EMPIRE							= 90,

	HEADER_GC_PARTY_LINK						= 91,
	HEADER_GC_PARTY_UNLINK						= 92,

	HEADER_GC_REFINE_INFORMATION_OLD			= 95,

	HEADER_GC_VIEW_EQUIP						= 99,

	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_IDXLIST						= 102,
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	HEADER_GC_MINIMAP_DUNGEONINFO				= 103,
#endif
	HEADER_GC_TIME								= 106,
	HEADER_GC_CHANGE_NAME						= 107,

	HEADER_GC_DUNGEON							= 110,

	HEADER_GC_WALK_MODE							= 111,
	HEADER_GC_SKILL_GROUP						= 112,
	HEADER_GC_MAIN_CHARACTER					= 113,

	HEADER_GC_SEPCIAL_EFFECT					= 114,

	HEADER_GC_NPC_POSITION						= 115,

	HEADER_GC_LOGIN_KEY							= 118,
	HEADER_GC_REFINE_INFORMATION				= 119,
	HEADER_GC_CHANNEL							= 121,

	HEADER_GC_TARGET_UPDATE						= 123,
	HEADER_GC_TARGET_DELETE						= 124,
	HEADER_GC_TARGET_CREATE						= 125,

	HEADER_GC_AFFECT_ADD						= 126,
	HEADER_GC_AFFECT_REMOVE						= 127,

	HEADER_GC_MALL_OPEN							= 122,
	HEADER_GC_MALL_SET							= 128,
	HEADER_GC_MALL_DEL							= 129,

	HEADER_GC_LAND_LIST							= 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,

	HEADER_GC_SYMBOL_DATA						= 133,

	// MINING
	HEADER_GC_DIG_MOTION						= 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
	// END_OF_SUPPORT_BGM

	HEADER_GC_AUTH_SUCCESS						= 150,

	HEADER_GC_PANAMA_PACK						= 151,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS					= 152,
	HEADER_GC_HYBRIDCRYPT_SDB					= 153, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT
#ifdef ENABLE_COLLECT_WINDOW
	HEADER_GC_COLLECT							= 155,
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_GC_TITLE_INFO = 163,
	HEADER_GC_TITLE_ACTIVE = 164,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_GC_WEEKLY_RANK				= 165,
	HEADER_GC_WEEKLY_PAGE				= 166,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT						= 171,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_MANAGER 				= 172,
#endif
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	HEADER_GC_LEGEND_DAMAGE_DATA				= 173,
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
	HEADER_GC_SEND_LOCATION_DATA				= 180,
#endif
	// ROULETTE
	HEADER_GC_ROULETTE							= 200,
	// END_ROULETTE
#ifdef ENABLE_LRN_BATTLE_PASS
	HEADER_GC_BATTLE_PASS_QUEST				= 184,
	HEADER_GC_BATTLE_PASS_QUEST_DATA		= 181,
	
	HEADER_GC_BATTLE_PASS_REWARD			= 182,
	HEADER_GC_BATTLE_PASS_REWARD_DATA		= 183,
#endif
	HEADER_GC_SPECIFIC_EFFECT					= 208,

	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
#ifdef ENABLE_ENTITY_PRELOADING
	HEADER_GC_PRELOAD_ENTITIES 					= 217,
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
	HEADER_GC_CUBE_RENEWAL 						= 221,
#endif
	HEADER_GC_GIVE_ITEMS						= 234,
// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 	HEADER_GC_MOUNT_NPC_ACTION 					= 235,
// #endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GC_PRIVATE_SHOP,
#endif

	/////////////////////////////////////////////////////////////////////////////

	HEADER_GG_LOGIN				= 1,
	HEADER_GG_LOGOUT				= 2,
	HEADER_GG_RELAY				= 3,
	HEADER_GG_NOTICE				= 4,
	HEADER_GG_SHUTDOWN				= 5,
	HEADER_GG_GUILD				= 6,
	HEADER_GG_DISCONNECT			= 7,
	HEADER_GG_SHOUT				= 8,
	HEADER_GG_SETUP				= 9,
	HEADER_GG_MESSENGER_ADD                     = 10,
	HEADER_GG_MESSENGER_REMOVE                  = 11,
	HEADER_GG_FIND_POSITION			= 12,
	HEADER_GG_WARP_CHARACTER			= 13,
	HEADER_GG_MESSENGER_MOBILE			= 14,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX		= 15,
	HEADER_GG_TRANSFER				= 16,
	HEADER_GG_XMAS_WARP_SANTA			= 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY		= 18,
	HEADER_GG_RELOAD_CRC_LIST			= 19,
	HEADER_GG_LOGIN_PING			= 20,
	HEADER_GG_CHECK_CLIENT_VERSION		= 21,
	HEADER_GG_BLOCK_CHAT			= 22,

	HEADER_GG_BLOCK_EXCEPTION		= 24,
	HEADER_GG_SIEGE					= 25,
	HEADER_GG_MONARCH_NOTICE		= 26,
	HEADER_GG_MONARCH_TRANSFER		= 27,

	HEADER_GG_CHECK_AWAKENESS		= 29,
#ifdef ENABLE_FULL_NOTICE
	HEADER_GG_BIG_NOTICE			= 30,
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_GG_REMOVE_OFFLINE_SHOP           = 31,
	HEADER_GG_CHANGE_OFFLINE_SHOP_TIME      = 32,
	HEADER_GG_OFFLINE_SHOP_BUY              = 33,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GG_SWITCHBOT					= 34,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_RESULT,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_UPDATE,
#endif
	HEADER_GG_RELOAD,
};

#pragma pack(1)
typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGNotice;

typedef struct SPacketGGMonarchNotice
{
	BYTE	bHeader;
	BYTE	bEmpire;
	long	lSize;
} TPacketGGMonarchNotice;

//FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE	bHeader;
	BYTE	bPass;
	BYTE	bSungzi;
} TPacketGGForkedMapInfo;
//END_FORKED_ROAD
typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE	bHeader;
	BYTE	bEmpire;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE	bHeader;
	BYTE	bChannel;
	long	lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE	bHeader;
	BYTE	bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SMessengerData
{
	char        szMobile[MOBILE_MAX_LEN + 1];
} TMessengerData;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGMessengerMobile
{
	BYTE        bHeader;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char        szMobile[MOBILE_MAX_LEN + 1];
} TPacketGGMessengerMobile;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
#ifdef ENABLE_CMD_WARP_IN_DUNGEON
	int mapIndex;
#endif
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
} TPacketGGBlockChat;

typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;

typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	char	pin[PASSWD_MAX_LEN + 1];
	DWORD	adwClientKey[4];
#ifdef CHECK_CLIENT_VERSION
	DWORD	client_version;
#endif
#ifdef ENABLE_HWID_SYSTEM
	char	cpu_id[CPU_ID_MAX_LEN + 1];
	char	hdd_model[HDD_MODEL_MAX_LEN + 1];
	char	machine_guid[MACHINE_GUID_MAX_LEN + 1];
	char	mac_addr[MAC_ADDR_MAX_LEN + 1];
	char	hdd_serial[HDD_SERIAL_MAX_LEN + 1];
#endif
} TPacketCGLogin3;

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_give_items
{
	BYTE	bHeader;
} TPacketGCGiveItem;
typedef struct packet_give_items1
{
	BYTE	bHeader;
	DWORD	itemcount;
} TPacketCGGiveItems;

typedef struct packet_received_items
{
	BYTE	bHeader;
	DWORD	item_count;
} TPacketCGReceivedItems;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE		header;
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;

typedef struct command_attack
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	long	lX;
	long	lY;
	DWORD	dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;

typedef struct command_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

#ifdef ENABLE_INSTANT_CHEST_OPEN
typedef struct OpenChestPacket
{
	BYTE header;
	TItemPos pos;
	WORD wOpenCount;
} TPacketCGOpenChest;
#endif

#ifdef ENABLE_INSTANT_INGOT_OPEN
typedef struct OpenIngotPacket
{
	BYTE header;
	TItemPos pos;
	WORD wOpenCount;
} TPacketCGOpenIngot;
#endif

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD	cheque;
#endif
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD	cheque;
#endif
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif
} TPacketCGItemDrop2;

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

typedef struct command_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE	header;
	BYTE	pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	,SHOP_SUBHEADER_CG_ADD_ITEM,
	SHOP_SUBHEADER_CG_REMOVE_ITEM,
	SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP,
	SHOP_SUBHEADER_CG_REFRESH,
	SHOP_SUBHEADER_CG_REFRESH_MONEY,
	SHOP_SUBHEADER_CG_REFRESH_CHEQUE,
	SHOP_SUBHEADER_CG_WITHDRAW_MONEY,
	SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE,
#endif
};

typedef struct command_shop_buy
{
	BYTE	count;
} TPacketCGShopBuy;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct command_offlineshop_logs_packet
{
	BYTE bHeader;
	ShopLog		log[20];
} TPacketGCOfflineShopLogs;
#endif

#if defined(__BL_MOVE_CHANNEL__)
typedef struct command_move_channel
{
	BYTE		header;
	BYTE		channel;
} TPacketCGMoveChannel;
#endif

#ifdef __EXTENDED_ITEM_COUNT__
typedef struct command_shop_sell
{
	WORD wPos;
	int sCount;
	BYTE bType;
} TPacketCGShopSell;
#else
typedef struct command_shop_sell
{
	BYTE pos;
	BYTE count;
} TPacketCGShopSell;
#endif

typedef struct command_shop
{
	BYTE	header;
#ifdef __EXTENDED_ITEM_COUNT__
	int subheader;
#else
	BYTE subheader;
#endif
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_CG_CHEQUE_ADD,
#endif
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
#ifdef ENABLE_LONG_LONG
	long long arg1;
#else
	DWORD	arg1;
#endif
	BYTE	arg2;
	TItemPos	Pos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool	SelectPosAuto;
#endif
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
#ifdef ENABLE_COLLECT_WINDOW
	BYTE		button;
#endif
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE	header;
	DWORD	addr;
	WORD	port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	BYTE		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];

	DWORD		handle;
	DWORD		random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;

typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
#ifdef ENABLE_AURA_SYSTEM
	CHR_EQUIPPART_AURA,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	CHR_EQUIPPART_EMBLEMAT,
#endif
	CHR_EQUIPPART_NUM,
};

#ifdef ENABLE_AURA_SYSTEM
enum
{
	AURA_SUBHEADER_GC_OPEN = 0,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_ADDED,
	AURA_SUBHEADER_GC_REMOVED,
	AURA_SUBHEADER_GC_REFINED,
	AURA_SUBHEADER_CG_CLOSE = 0,
	AURA_SUBHEADER_CG_ADD,
	AURA_SUBHEADER_CG_REMOVE,
	AURA_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAura
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	DWORD	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAura;
#endif

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;

	float	angle;
	long	x;
	long	y;
	long	z;

	BYTE	bType;
	DWORD	wRaceNum; // @fixme501
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD   awPart[CHR_EQUIPPART_NUM]; // @fixme502
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
	long long	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	DWORD	awPart[CHR_EQUIPPART_NUM]; // @fixme502
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
	long long	sAlignment;
#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD	dwLevel;
#endif
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire;
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE        header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE	header;
#ifdef ENABLE_LONG_LONG
long long	points[POINT_MAX_NUM];
#else
	int		points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int		header;
	DWORD	dwVID;
	BYTE	type;
#ifdef ENABLE_LONG_LONG
	long long	amount;
	long long	value;
#else
	long	amount;
	long	value;
#endif
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

#ifdef RENEWAL_DEAD_PACKET
enum EReviveTypes
{
	REVIVE_TYPE_HERE,
	REVIVE_TYPE_TOWN,
	REVIVE_TYPE_AUTO_TOWN,
	REVIVE_TYPE_MAX
};
#endif
typedef struct packet_dead
{
#ifdef RENEWAL_DEAD_PACKET
	packet_dead() {	memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE	header;
	DWORD	vid;
#ifdef RENEWAL_DEAD_PACKET
	BYTE	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	int		count;
#else
	BYTE	count;
#endif
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	packet_item_ground_add()
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttrs, 0, sizeof(aAttrs));
	}
#endif

	BYTE	bHeader;
	long 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
	int		count;
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	long	alSockets[ITEM_SOCKET_MAX_NUM];//3
	TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_MAX_NUM];//7
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
};

struct packet_quickslot_del
{
	BYTE	header;
	BYTE	pos;
};

struct packet_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	pos_to;
};

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_CHEQUE,
#endif
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SHOP_SUBHEADER_GC_UPDATE_ITEM2,
	SHOP_SUBHEADER_GC_REFRESH_MONEY,
	SHOP_SUBHEADER_GC_REFRESH_CHEQUE,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_CHEQUE,
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	SHOP_SUBHEADER_GC_NOT_ENOUGH_PKT_OSIAG,
#endif
};

struct packet_shop_item
{
	DWORD       vnum;
#ifdef ENABLE_LONG_LONG
	long long price;
#else
	DWORD price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	long long cheque;
#endif
#ifdef __EXTENDED_ITEM_COUNT__
	int count;
#else
	BYTE count;
#endif
	BYTE		display_pos;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	bool	ispktosiagshop;
#endif
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD	owner_vid;
	//DWORD	IsOnline;
	BYTE	shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	BYTE			bPos;
#endif
#ifdef ENABLE_LONG_LONG
	long long		iPrice;
#else
	int				iPrice;
#endif
	long long		iPrice2;
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
#ifdef ENABLE_LONG_LONG
	long long arg1;
#else
	DWORD	arg1;	// vnum
#endif
	TItemPos	arg2;	// cell
	DWORD	arg3;	// count
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos	arg4;	// srccell
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_CHEQUE_ADD,
#endif
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_LESS_CHEQUE,
#endif
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
#ifdef ENABLE_QUEST_CATEGORY
	BYTE	quest_flag;
#endif
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	long		lX;
	long		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;

typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketGCSyncPositionElement;

typedef struct packet_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	// BYTE	bHPPercent;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	long long	iMinHP;
	long long	iMaxHP;
#endif
} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef __QUEST_RENEWAL__
	WORD c_index;
#endif
	BYTE flag;
};

enum 
{
#ifdef ENABLE_MESSENGER_TEAM
	MESSENGER_SUBHEADER_GC_TEAM_LIST,
	MESSENGER_SUBHEADER_GC_TEAM_LOGIN,
	MESSENGER_SUBHEADER_GC_TEAM_LOGOUT,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_MOBILE
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

#ifdef ENABLE_MESSENGER_TEAM
typedef struct packet_messenger_team_list_offline
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerTeamListOffline;

typedef struct packet_messenger_team_list_online
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerTeamListOnline;
#endif

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
	//DWORD account;
} TPacketCGMessengerRemove;

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
	BYTE	percent_hp;
	short	affects[7];
#if defined(__BL_PARTY_POSITION__)
	long	x;
	long	y;
#endif
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	long	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	long	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid;
	BYTE	image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
#ifdef __EXTENDED_ITEM_COUNT__
	int byItemCount;
#else
	BYTE byItemCount;
#endif
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
#ifdef __EXTENDED_ITEM_COUNT__
	int bCount;
#else
	BYTE bCount;
#endif
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	time_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	BYTE	header;
	BYTE	pos;
	BYTE	type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE	header;
	BYTE	pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE	header;
	BYTE	type;
	BYTE	pos;
	DWORD	src_vnum;
	DWORD	result_vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	int material_count;
#else
	BYTE material_count;
#endif
	int		cost;
	int		cost2;
	int		cost3;
	int		prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN+1];
	long x;
	long y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	BYTE    count;
	long    alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE  header;
	DWORD vid;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketViewEquip;

typedef struct SLandPacketElement
{
	DWORD	dwID;
	long	x, y;
	long	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct SPacketGCTargetCreate
{
	BYTE	bHeader;
	long	lID;
	char	szName[32+1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct SPacketGCTargetUpdate
{
	BYTE	bHeader;
	long	lID;
	long	lX, lY;
} TPacketGCTargetUpdate;

typedef struct SPacketGCTargetDelete
{
	BYTE	bHeader;
	long	lID;
} TPacketGCTargetDelete;

typedef struct SPacketGCAffectAdd
{
	BYTE		bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct SPacketGCAffectRemove
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct tag_GGSiege
{
	BYTE	bHeader;
	BYTE	bEmpire;
	BYTE	bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGMonarchTransfer
{
	BYTE	bHeader;
	DWORD	dwTargetPID;
	long	x;
	long	y;
} TPacketMonarchGGTransfer;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE	bHeader;
	char	szPackName[256];
	BYTE	abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &KeyStreamLen, 4 );

		if( KeyStreamLen > 0 )
			memcpy( m_pStream+7, pDataKeyStream, KeyStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		KeyStreamLen;
	BYTE*   pDataKeyStream;

private:
	BYTE* m_pStream;
} TPacketGCHybridCryptKeys;

typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize =  GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &iStreamLen, 4 );

		if( iStreamLen > 0 )
			memcpy( m_pStream+7, m_pDataSDBStream, iStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		iStreamLen;
	BYTE*   m_pDataSDBStream;

private:
	BYTE* m_pStream;
} TPacketGCPackageSDB;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct SPacketGGRemoveOfflineShop
{
	BYTE	bHeader;
    long    lMapIndex;
	char    szNpcName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGRemoveOfflineShop;

typedef struct SPacketGGOfflineShopBuy
{
	BYTE    bHeader;
	long long	dwMoney;
	long long	dwCheque;
	char    szBuyerName[CHARACTER_NAME_MAX_LEN + 1];        
} TPacketGGOfflineShopBuy;

typedef struct packet_offline_shop_start
{
	DWORD	owner_vid;
	DWORD	IsOnline;
	struct packet_shop_item items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCOfflineShopStart;

typedef struct packet_offline_shop_money
{
	long long	dwMoney;
	long long	dwCheque;
	long long	zarobekYang;
	long long	zarobekCheque;
} TPacketGCOfflineShopMoney;

typedef struct TPacketCGMyOfflineShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	int		bCount;
} TPacketCGMyOfflineShop;

typedef struct packet_gold_new
{
	BYTE header;
	DWORD vid;
	long long gold;
} TPacketGold;
#endif


#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef __REMOVE_ITEM__
enum ERemoveItemConfig
{
	HEADER_GC_REMOVE_ITEM = 57,
	HEADER_CG_REMOVE_ITEM = 68,
	REMOVE_ITEM_SLOT_COUNT_X = 8,
	REMOVE_ITEM_SLOT_COUNT_Y = 6,
};

typedef struct SPacketGCRemoveItem
{
	BYTE	header;
} TPacketGCRemoveItem;

typedef struct SPacketCGRemoveItem
{
	BYTE	header;
	BYTE	item_count;
} TPacketCGRemoveItem;
#endif

#ifdef ENABLE_ODLAMKI_SYSTEM
enum
{
	HEADER_CG_ODLAMKI_ITEM = 224,
	HEADER_GC_ODLAMKI_ITEM = 225,
};
typedef struct SPacketGCOdlamki
{
	BYTE	header;
} TPacketGCOdlamki;

typedef struct SPacketCGOdlamki {
	BYTE	header;
	BYTE	item_count;
} TPacketCGOdlamki;
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
enum
{
	HEADER_CG_SEARCH = 226,
};

typedef struct SPacketCGSearch {
	BYTE	header;
	BYTE	items;
} TPacketCGSearch;
#endif

#ifdef ENABLE_ENTITY_PRELOADING
/*** HEADER_GC_PRELOAD_ENTITIES ***/
typedef struct packet_preload_entities
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCPreloadEntities;
#endif

#ifdef __ITEM_SHOP__
enum
{
	ITEMSHOP_SUBHEADER_GC_ITEM,
	ITEMSHOP_SUBHEADER_GC_COINS,
	ITEMSHOP_SUBHEADER_GC_CLEAR,
	ITEMSHOP_SUBHEADER_GC_OPEN,
	ITEMSHOP_SUBHEADER_GC_EDITOR,
	ITEMSHOP_SUBHEADER_GC_UPDATE,
	ITEMSHOP_SUBHEADER_GC_DELETE,
	ITEMSHOP_SUBHEADER_GC_ERROR,
	ITEMSHOP_SUBHEADER_GC_BUY_STATS,
	ITEMSHOP_SUBHEADER_GC_LIMITED_COUNT,
	ITEMSHOP_SUBHEADER_GC_REWARD,
	ITEMSHOP_SUBHEADER_GC_REWARD_STATUS,
	ITEMSHOP_SUBHEADER_GC_POPUP,
	ITEMSHOP_SUBHEADER_GC_CATEGORY,

	ITEMSHOP_SUBHEADER_CG_OPEN = 0,
	ITEMSHOP_SUBHEADER_CG_BUY,
	ITEMSHOP_SUBHEADER_CG_ADD,

	HEADER_CG_ITEMSHOP = 34,
	HEADER_GC_ITEMSHOP = 139,

	CATEGORY_NAME_MAX_NUM = 32,
};

enum EItemShopError: BYTE
{
	ITEMSHOP_ERROR_NOT_ENOUGH_COINS,
	ITEMSHOP_ERROR_INVENTORY_FULL,
	ITEMSHOP_ERROR_WARP,
	ITEMSHOP_ERROR_NON_EDITOR,
	ITEMSHOP_ERROR_COUNT,
	ITEMSHOP_ERROR_UNKNOWN_ERROR,
};

enum EItemShopPopupType
{
	ITEMSHOP_POPUP_ITEM_PROMO,
	ITEMSHOP_POPUP_ITEM_LIMITED,
	ITEMSHOP_POPUP_NEW_ITEM,
};

typedef struct item_shop_packet
{
	BYTE	header;
	DWORD	size;
	BYTE	subheader;
} TItemShopGCPacket;

typedef struct item_shop_client_packet
{
	BYTE	header;
	BYTE	subheader;
} TItemShopCGPacket;

typedef struct SItemShopItem
{
	DWORD	vnum;
	int		count;
	DWORD	price;
	BYTE	bonus_type;
	BYTE	category;
	DWORD	id;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	BYTE	is_vip;
	BYTE	fixed_count;
	DWORD	additional_time;
	DWORD	add_time;
	BYTE	discount;
	DWORD	discount_time;
	DWORD	limited_time;
	DWORD	limited_count;
	DWORD	limited_buy_count;
	DWORD	limited_buy_time;

	SItemShopItem()
	{
		vnum = 0;
		price = 0;
		bonus_type = 0;
		count = 0;
		category = 0;
		id = 0;
		is_vip = false;
		fixed_count = false;
		additional_time = 0;

		add_time = 0;
		discount = 0;
		discount_time = 0;

		limited_time = 0;
		limited_count = 0;
		limited_buy_count = 0;

		limited_buy_time = 0;

		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}

} TItemShopItem;

typedef struct item_shop_delete_item
{
	DWORD	id;
	DWORD	category;
} TItemShopItemDeletePacket;

typedef struct item_shop_buy
{
	DWORD	id;
	DWORD	category;
	int		count;
	BYTE	bonus_type;
} TPacketCGItemShopBuy;

typedef struct item_shop_add
{
	TItemShopItem	aItemShopItem;
	BYTE	bFlag;
} TPacketCGItemShopAddItem;

typedef struct item_shop_buy_stats
{
	DWORD	dwID;
	DWORD	dwCategory;
	int		dwCount;

	item_shop_buy_stats(DWORD id, DWORD category)
	{
		dwID = id;
		dwCategory = category;
		dwCount = 1;
	}
	item_shop_buy_stats()
	{
		dwID = 0;
		dwCategory = 0;
		dwCount = 1;
	}
} TItemShopBuyStats;

typedef struct itemshop_open_packet
{
	time_t	ttDataTime;
} TItemShopOpenPacket;

typedef struct item_shop_popup_packet
{
	BYTE	type;
	DWORD	id;
	DWORD	category;
} TItemShopPopupPacket;

typedef struct item_shop_category_packet
{
	BYTE index;
	char szName[CATEGORY_NAME_MAX_NUM + 1];
} TItemShopCategoryPacket;
#endif

#ifdef __TOMBOLA__
enum
{
	HEADER_CG_TOMBOLA = 37,
	TOMBOLA_CG_SUBHEADER_START = 0,
	TOMBOLA_CG_SUBHEADER_OPEN = 1,

	HEADER_GC_TOMBOLA = 50,
	TOMBOLA_GC_SUBHEADER_ITEM = 0,
	TOMBOLA_GC_SUBHEADER_START = 1,
	TOMBOLA_GC_SUBHEADER_OPEN = 2,
};

typedef struct tombola_packet
{
	BYTE	byHeader;
	WORD	wSize;
	BYTE	bySubHeader;
} TPacketGCTombola;

typedef struct tombola_client_packet
{
	BYTE	byHeader;
	BYTE	bySubHeader;
} TPacketCGTombola;
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
typedef struct SPetAction
{
	BYTE	header;
	DWORD	skillvnum;
} TPacketPetAction;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 215,
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED,
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	long long	dwPrice;
	long long	dwCheque;
	BYTE	bChance;
	AcceMaterials	materials[2];
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;
#endif

#ifdef ENABLE_DROP_INFO
enum EDropInfoConfig
{
	HEADER_GC_DROP_INFO = 93,
	HEADER_CG_DROP_INFO = 99,
};

typedef struct SPacketDropInfoItem
{
	DWORD	dwVnum;
#ifdef __EXTENDED_ITEM_COUNT__
	int	byMinCount;
	int	byMaxCount;
#else
	BYTE	byMinCount;
	BYTE	byMaxCount;
#endif
#ifdef ENABLE_DROP_INFO_PCT
	float	pct;
#endif
} TPacketDropInfoItem;

typedef struct SPacketDropInfo
{
	BYTE	header;
	WORD	size;
	int		item_count;
	DWORD	gold_min;
	DWORD	gold_max;
	DWORD	mob_vnum;
} TPacketDropInfo;
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
typedef struct SPacketGCSendLocationData
{
	BYTE	header;
	BYTE	position[MAX_LOCATION_POSITION];
	int		map_index[MAX_LOCATION_POSITION];
	char	name[MAX_LOCATION_CHAR_STRING];
	int		posx[MAX_LOCATION_POSITION];
	int		posy[MAX_LOCATION_POSITION];
} TPacketGCSendLocationData;

typedef struct SPacketCGSendLocationManager
{
	BYTE	header;
	BYTE	action;
	BYTE	position;
	char	name[MAX_LOCATION_CHAR_STRING];
	int		posx[MAX_LOCATION_POSITION];
	int		posy[MAX_LOCATION_POSITION];
} TPacketCGSendLocationManager;
#endif

#ifdef ENABLE_COLLECT_WINDOW
typedef struct SPacketGCCollectWindow
{
	BYTE	bHeader;
	BYTE	bWindowType;
	DWORD	dwTime;
	DWORD	bCount;
	DWORD	dwItemVnum;
	DWORD	bCountTotal;
	BYTE	bChance;
	DWORD	bRenderTargetID;
	DWORD	bQuestIndex;
	DWORD	RequiredLevel;
} TPacketGCCollectWindow;
#endif

#ifdef WEEKLY_RANK_BYLUZER
typedef struct packet_send_weekly_rank
{
	int		rank;
	char	name[CHARACTER_NAME_MAX_LEN+1];
	int		points;
	int		empire;
	int		job;
} TPacketGCTableWeeklyRank;

typedef struct packet_send_weekly_rank2
{
	BYTE 			bHeader;
	TPacketGCTableWeeklyRank rankingData[10];
} TPacketGCSendWeeklyRank;

typedef struct packet_send_weekly_page
{
	BYTE bHeader;
	BYTE page;
	int season;
	bool can_select;
} TPacketGCSelectWeeklyPage;

typedef struct packet_select_weekly_rank
{
	BYTE			bHeader;
	BYTE			page;
} TPacketCGSelectWeeklyRank;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
typedef struct packet_title_active
{
	BYTE		bHeader;
	TPlayerTitle	titles[TITLE_MAX_NUM];
	BYTE		actually_active;
} TPacketGCTitleActive;

typedef struct packet_title_active_now
{
	BYTE		bHeader;
	BYTE		id;
} TPacketGCTitleActiveNow;

typedef struct packet_title_active_client
{
	BYTE		bHeader;
	int			id;
} TPacketCGTitleActive;
#endif

#ifdef ENABLE_SWITCHBOT
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;

	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0)
	{
		table = {};
	}
};

enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};
#endif


#ifdef ENABLE_RESP_SYSTEM
enum ERespConfig
{
	HEADER_GC_RESP = 158,
	HEADER_CG_RESP = 87,

	RESP_CG_SUBHEADER_FETCH_RESP = 0,
	RESP_CG_SUBHEADER_FETCH_DROP,
	RESP_CG_SUBHEADER_TELEPORT,

	RESP_GC_SUBHEADER_DATA_RESP = 0,
	RESP_GC_SUBHEADER_DATA_DROP,
	RESP_GC_SUBHEADER_DATA_MOB,
	RESP_GC_SUBHEADER_REFRESH_RESP,
};

typedef struct SPacketGCRespHeader
{
	uint8_t	header;
	uint16_t	size;
	uint8_t	subheader;
} TPacketGCRespHeader;

typedef struct SPacketCGRespHeader
{
	uint8_t	header;
	uint8_t	subheader;
} TPacketCGRespHeader;

typedef struct SPacketGCRespData
{
	size_t id;
	uint32_t x;
	uint32_t y;
	uint32_t time;
	time_t nextRespTime;
} TPacketGCRespData;

typedef struct SPacketGCRespGold
{
	uint32_t	minGold;
	uint32_t	maxGold;
} TPacketGCRespGold;

typedef struct SPacketGCRespItem
{
	uint32_t	vnum;
	uint8_t	minCount;
	uint8_t	maxCount;
} TPacketGCRespItem;

typedef struct SPacketGCMapData
{
	uint8_t	mobCount;
	uint32_t	currentMetinCount;
	uint32_t	maxMetinCount;
	uint32_t	currentBossCount;
	uint32_t	maxBossCount;
} TPacketGCMapData;

typedef struct SPacketGCRespRefresh
{
	size_t	id;
	uint32_t	vnum;
	time_t	nextRespTime;
	uint32_t	x;
	uint32_t	y;
} TPacketGCRespRefresh;
#endif

typedef struct packet_anty_exp_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCAntyExp;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
typedef struct packet_wpadanie_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCWpadanie;
#endif

// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// typedef struct SPacketCGMountNPCAction
// {
// 	BYTE	bHeader;
// 	BYTE	bAction;
// 	DWORD	dValue0;
// } TPacketCGMountNPCAction;

// typedef struct SPacketGCMountNPCAction
// {
// 	BYTE	bHeader;
// 	BYTE	bAction;
// 	DWORD	dValue0;
// } TPacketGCMountNPCAction;
// #endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
typedef struct SPacketCGBuffNPCCreate
{
	BYTE bHeader;
	BYTE	bAction;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bSex;
}TPacketCGBuffNPCCreate;

typedef struct SPacketCGBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketCGBuffNPCAction;

typedef struct SPacketGCBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketGCBuffNPCAction;

typedef struct SPacketGCBuffNPCUseSkill
{
	BYTE	bHeader;
	DWORD	dSkillVnum;
	DWORD	dVid;
	DWORD	dSkillLevel;
}TPacketGCBuffNPCUseSkill;
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
enum
{
	CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_LOADING,

	CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM,
	CUBE_RENEWAL_SUB_HEADER_CLOSE,
};

typedef struct  packet_send_cube_renewal
{
	BYTE header;
	BYTE subheader;
	DWORD index_item;
	DWORD count_item;
	DWORD index_item_improve;
}TPacketCGCubeRenewalSend;

typedef struct dates_cube_renewal
{
	DWORD npc_vnum;
	DWORD index;

	DWORD	vnum_reward;
	int		count_reward;

	bool 	item_reward_stackable;

	DWORD	vnum_material_1;
	int		count_material_1;

	DWORD	vnum_material_2;
	int		count_material_2;

	DWORD	vnum_material_3;
	int		count_material_3;

	DWORD	vnum_material_4;
	int		count_material_4;

	DWORD	vnum_material_5;
	int		count_material_5;

	DWORD	vnum_material_6;
	int		count_material_6;

	DWORD	vnum_material_7;
	int		count_material_7;

	int 	gold;
	int 	punktyos;
	int		cheque;
	int 	percent;

	char 	category[100];
}TInfoDateCubeRenewal;

typedef struct packet_receive_cube_renewal
{
	packet_receive_cube_renewal(): header(HEADER_GC_CUBE_RENEWAL)
	{}

	BYTE header;
	BYTE subheader;
	TInfoDateCubeRenewal	date_cube_renewal;
}TPacketGCCubeRenewalReceive;
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
enum EWonExchangeCGSubHeader
{
	WON_EXCHANGE_CG_SUBHEADER_SELL,
	WON_EXCHANGE_CG_SUBHEADER_BUY
};

typedef struct SPacketCGWonExchange
{
	SPacketCGWonExchange(): bHeader(HEADER_CG_WON_EXCHANGE)
	{}

	BYTE bHeader;
	BYTE bSubHeader;
	WORD wValue;
} TPacketCGWonExchange;
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
enum
{
	MINIMAP_DUNGEONINFO_SUBHEADER_STATUS,
	MINIMAP_DUNGEONINFO_SUBHEADER_STAGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE,
	MINIMAP_DUNGEONINFO_SUBHEADER_BUTTON,
	MINIMAP_DUNGEONINFO_SUBHEADER_TIMER,
};

typedef struct SPacketGCMiniMapDungeonInfo
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
	DWORD	buttonvalue;
	DWORD	timerstatus;
	DWORD	time;
	char	szNotice[256];
}TPacketGCMiniMapDungeonInfo;
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
typedef struct SPacketBattlePassQuest
{
	BYTE	header;
	
	int32_t	questIndex;
	int16_t	questGroup;
	int32_t	questCount;
	int32_t	questExp;
} TPacketBattlePassQuest;

typedef struct SPacketBattlePassQuestData
{
	BYTE	header;
	
	int32_t	questIndex;
	int32_t questCount;
	int8_t	questStatus;
} TPacketBattlePassQuestData;

typedef struct SPacketBattlePassReward
{
	BYTE	header;
	
	int16_t	rewardLevel;
	int32_t	rewardItem[2];
	int32_t	rewardCount[2];
} TPacketBattlePassReward;

typedef struct SPacketBattlePassRewardData
{
	BYTE	header;
	int16_t	rewardLevel;
	int8_t	rewardStatus[2];
} TPacketBattlePassRewardData;

typedef struct SPacketBattlePassCollect
{
	BYTE	header;
	
	int16_t	rewardLevel;
	int8_t	rewardType;
} TPacketBattlePassCollect;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct SPacketGCEventManager
{
	BYTE	header;
	DWORD	size;
} TPacketGCEventManager;
#endif

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
typedef struct packet_send_legend_damage
{
	char		cName[CHARACTER_NAME_MAX_LEN + 1];
	int			iLevel;
	int			iRace;
	int			iEmpire;
	long long	iDamage;
} TPacketGCTableLegendDamage;

typedef struct packet_send_legend_damage2
{
	BYTE	bHeader;
	DWORD	dwBossVID;
	char	cTarget[CHARACTER_NAME_MAX_LEN + 1];
	TPacketGCTableLegendDamage legendDamageData[LEGEND_DAMAGE_BOARD_MAX_ENTRIES];
} TPacketGCSendLegendDamage;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
enum EPrivateShopGCSubheader
{
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE,
	SUBHEADER_GC_PRIVATE_SHOP_LOAD,
	SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_SET_SALE,
	SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE,
	SUBHEADER_GC_PRIVATE_SHOP_START,
	SUBHEADER_GC_PRIVATE_SHOP_END,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE,

	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE,

	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT,
};

enum EPrivateShopCGSubheader
{
	SUBHEADER_CG_PRIVATE_SHOP_BUILD,
	SUBHEADER_CG_PRIVATE_SHOP_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN_CHECK,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_START,
	SUBHEADER_CG_PRIVATE_SHOP_END,
	SUBHEADER_CG_PRIVATE_SHOP_BUY,
	SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_CG_PRIVATE_SHOP_MODIFY,
	SUBHEADER_CG_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT,
	SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST,

	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY,

	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST,
};

typedef struct SPrivateShopItem
{
	TItemPos	TPos;
	TItemPrice	TPrice;
	WORD		wDisplayPos;
} TPrivateShopItem;

typedef struct SPrivateShopItemData
{
	DWORD					dwVnum;
	TItemPrice				TPrice;
	time_t					tCheckin;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
   TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopItemData;

typedef struct SPrivateShopSaleData
{
	char					szCustomer[CHARACTER_NAME_MAX_LEN + 1];
	time_t					tTime;
	TPrivateShopItemData	TItem;
} TPrivateShopSaleData;

typedef struct SPrivateShopSearchData
{
	DWORD					dwShopID;
	char					szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD					dwVnum;
	TItemPrice				TPrice;
	DWORD					dwCount;
	WORD					wPos;
	long					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	time_t					tCheckin;
	BYTE					bState;
	long					lMapIndex;
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
    TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopSearchData;

typedef struct SPacketCGPrivateShop
{
	BYTE	bHeader;
	BYTE	bSubHeader;
} TPacketCGPrivateShop;

typedef struct SPacketGCPrivateShopAddEntity
{
	long		lX;
	long		lY;
	long		lZ;
	DWORD		dwVID;
	DWORD		dwVnum;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		bTitleType;
	char		szTitle[TITLE_MAX_LEN + 1];
} TPacketGCPrivateShopAddEntity;

typedef struct SPacketGCPrivateShopDelEntity
{
	DWORD		dwVID;
} TPacketGCPrivateShopDelEntity;

typedef struct SPacketGCPrivateShopTitle
{
	DWORD		dwVID;
	char		szTitle[TITLE_MAX_LEN + 1];
	BYTE		bTitleType;
} TPacketGCPrivateShopTitle;

typedef struct SPacketCGPrivateShopBuild
{
	char	szTitle[TITLE_MAX_LEN + 1];
	DWORD	dwPolyVnum;
	BYTE	bTitleType;
	BYTE	bPageCount;
	WORD	wItemCount;
} TPacketCGPrivateShopBuild;

typedef struct SPacketCGPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketCGPrivateShopItemPriceChange;

typedef struct SPacketCGPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketCGPrivateShopItemMove;

typedef struct SPrivateShopSearchFilter
{
	DWORD	dwVnum;
	char	szOwnerName[CHARACTER_NAME_MAX_LEN + 1];

	int		iItemType;
	int		iItemSubType;

	int		iJob;
	int		iGender;

	WORD	wMinStack;
	WORD	wMaxStack;

	BYTE	bMinRefine;
	BYTE	bMaxRefine;

	DWORD	dwMinLevel;
	DWORD	dwMaxLevel;

	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	WORD					wSashAbsorption;
	BYTE					bAlchemyLevel;
	BYTE					bAlchemyClarity;
	int						iItemValue0;
	bool					bSearchMode;
} TPrivateShopSearchFilter;

typedef struct SPacketCGPrivateShopSearch
{
	TPrivateShopSearchFilter	Filter;
	bool						bUseFilter;
} TPacketCGPrivateShopSearch;

typedef struct SPrivateShopSearchSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} SPrivateShopSearchSelectedItem;

typedef struct SPacketCGPrivateShopSearchBuy
{
	SPrivateShopSearchSelectedItem aSelectedItems[SELECTED_ITEM_MAX_NUM];
} TPacketCGPrivateShopSearchBuy;

typedef struct SPacketGCPrivateShopSearchUpdate
{
	DWORD	dwShopID;
	int		iSpecificItemPos;
	BYTE	bState;
} TPacketGCPrivateShopSearchUpdate;

typedef struct SPacketGCPrivateShop
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCPrivateShop;

typedef struct SPacketGCPrivateShopLoad
{
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
	long long	llGold;
	DWORD		dwCheque;
	long		lX;
	long		lY;
	BYTE		bChannel;
	BYTE		bState;
	BYTE		bPageCount;
} TPacketGCPrivateShopLoad;

typedef struct SPacketGCPrivateShopOpen
{
	char					szTitle[SHOP_SIGN_MAX_LEN + 1];
	BYTE					bState;
	BYTE					bPageCount;
	WORD					wUnlockedSlots;
	DWORD					dwVID;
	TPrivateShopItemData	aItems[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCPrivateShopOpen;

typedef struct SPacketGCPrivateStateUpdate
{
	BYTE	bState;
	bool	bIsMainPlayerPrivateShop;
} TPacketGCPrivateStateUpdate;

typedef struct SPacketGCPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketGCPrivateShopItemPriceChange;

typedef struct SPacketGCPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketGCPrivateShopItemMove;

typedef struct SPacketGCPrivateShopBalanceUpdate
{
	TItemPrice	TPrice;
} TPacketGCPrivateShopBalanceUpdate;

typedef struct SPacketCGPrivateShopItemCheckin
{
	TItemPos	TSrcPos;
	long long	llGold;
	DWORD		dwCheque;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckin;

typedef struct SPacketCGPrivateShopItemCheckout
{
	WORD		wSrcPos;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckout;

typedef struct SPacketGGPrivateShopItemRemove
{
	BYTE					bHeader;
	DWORD					dwShopID;
	WORD					wPos;
} TPacketGGPrivateShopItemRemove;

typedef struct SPacketGGPrivateShopItemSearch
{
	BYTE						bHeader;
	DWORD						dwCustomerID;
	DWORD						dwCustomerPort;
	bool						bUseFilter;
	TPrivateShopSearchFilter	Filter;
} TPacketGGPrivateShopItemSearch;

typedef struct SPacketGGPrivateShopItemSearchUpdate
{
	BYTE					bHeader;
	DWORD					dwShopID;
	int						iSpecificItemPos;
	BYTE					bState;
} TPacketGGPrivateShopItemSearchUpdate;

typedef struct SPacketGGPrivateShopItemSearchResult
{
	BYTE					bHeader;
	WORD					wSize;
	DWORD					dwCustomerID;
} TPacketGGPrivateShopItemSearchResult;
#endif

enum eReloadTypes : uint8_t { RELOAD_DROPS, RELOAD_VERSION };

typedef struct SPacketGGReload {
    uint8_t header;
    uint8_t type;
} TPacketGGReload;

#pragma pack()
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
