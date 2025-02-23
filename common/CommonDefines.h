#ifndef __INC_METIN2_COMMON_DEFINES_H__
#define __INC_METIN2_COMMON_DEFINES_H__
#pragma once


#define CHECK_CLIENT_VERSION					//-> Check client version
#ifdef CHECK_CLIENT_VERSION
#	define CURRENT_VERSION 15
#endif


/// Version 1.1
#define __CANNOT_DEAL_DAMAGE_TO_PLAYERS_ON_POLYMOPH__
#define __RESET_PLAYER_DAMAGE__
#define __NEW_DROP_SYSTEM__

#define __PREMIUM_PRIVATE_SHOP__
#ifdef __PREMIUM_PRIVATE_SHOP__
	//#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	#define ENABLE_CHEQUE_SYSTEM
	//#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	//#define ENABLE_PRIVATE_SHOP_SPECIAL_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_INV
	// #define ENABLE_PRIVATE_SHOP_EXTEND_SPECIAL_INV
	// #define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	// #define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	// #define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	// #define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
	// #define ENABLE_PRIVATE_SHOP_SOCKET5
#endif

//////////////////////////////////////////////////////////////////////////
// ### Standard Features ###
#define _IMPROVED_PACKET_ENCRYPTION_
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	#define USE_IMPROVED_PACKET_DECRYPTED_BUFFER
#endif

#define __PET_SYSTEM__
#define __UDP_BLOCK__

#define __QUEST_RENEWAL__ // // Kategoria Misji
#ifdef __QUEST_RENEWAL__
	#define _QR_MS_ // Marty Sama
#endif						

// ### END Standard Features ###
//////////////////////////////////////////////////////////////////////////

//Dodatki
#define ENABLE_DROP_INFO_PCT						// procent na drop w wiki dropu
#define ENABLE_BELT_ATTRIBUTES						// Atrybuty w pasach - 28.07.24
#define ENABLE_GLOVE_ATTRIBUTES						// System atrybytów w rêkawicach.	27.09.24

#define ENABLE_ALIGN_RENEWAL						// System Rang z Bonusami
#define ENABLE_EXTENDED_BLEND						// Nowe Rosy
#define __BL_MOVE_CHANNEL__							// Zmiana Kana³ów
#define ENABLE_REAL_TIME_REGEN						// Respy
#define ENABLE_POLY_SHOP							// Sklep Poli
#define ENABLE_DROP_INFO							// Wikipedia Dropu
#define ENABLE_LRN_LOCATION_SYSTEM					// Zapis Lokalizacji
//#define ENABLE_OFFLINE_SHOP_SYSTEM					// Sklepy Offline
#define WEEKLY_RANK_BYLUZER							// Ranking Tygodniowy			03.03.2023
#define TITLE_SYSTEM_BYLUZER						// System Tytu³ów				04.03.2023
#define ENABLE_SWITCHBOT							// SwitchBot					01.04.2023
#define ENABLE_RESP_SYSTEM							// Teleport na bossy i metki	24.04.2023

#define ENABLE_SPLIT_INVENTORY_SYSTEM				// Dodatkowy Ekwipunek			13.05.2023

#define ENABLE_CUBE_RENEWAL_WORLDARD				// System Cube
#define ENABLE_CUBE_ATTR_SOCKET						// 18.05.2023

#define __BL_MOVE_CHANNEL__							// Szybka Zmiana Channeli		22.05.2023
#define ENABLE_CHEQUE_SYSTEM
#define ENABLE_SHOP_USE_CHEQUE
#define __REMOVE_ITEM__								// Kosz na Przedmioty by Filip	18.06.2023
#define ENABLE_STOLE_COSTUME
#define ENABLE_GLOVE_SYSTEM							// Rekawice						18.07.2023
#define ENABLE_SYSTEM_RUNE							// System Run					19.07.2023
#define ENABLE_AFFECT_POLYMORPH_REMOVE				// Usuwanie Polimorfii			20.07.2023
#define ENABLE_SKILL_AFFECT_REMOVE					// Usuwanie Affectów i skili	20.07.2023
#define ENABLE_ARTEFAKT_SYSTEM						// System Artefaktów			25.07.2023
#define __DUNGEON_INFO_SYSTEM__						// Dungeon Info					29.07.2023
#define ENABLE_SYSTEMY_KARTY_OKEY					// Gra Karty Okey
#define ENABLE_MINIMAP_DUNGEONINFO					//
#define ENABLE_SORT_INVENTORY_ITEMS
#define __EXTENDED_ITEM_COUNT__						// Zdjêty limit stack do 5000
#define ENABLE_PUNKTY_OSIAGNIEC						// System Osi¹gniêæ
#define ENABLE_SPECIAL_BONUS						// Dodatkowy Bonus w Broni i Zbroi
#define ENABLE_AURA_SYSTEM
#define ENABLE_NEW_STONE_DETACH
#define ENABLE_HIDE_COSTUME_SYSTEM
#define ENABLE_COLLECT_WINDOW						// Kolekcjoner w GUI.
#define ENABLE_COSTUME_EMBLEMAT						// Emblematy
#define ENABLE_PROMO_CODE_SYSTEM
#define ENABLE_SKILE_PASYWNE						// Skile Pasywne
#define __CHATTING_WINDOW_RENEWAL__ 				// Chat system
#define ENABLE_INSTANT_CHEST_OPEN					// Szybkie otwieranie szkat.
#ifdef ENABLE_INSTANT_CHEST_OPEN
#	define	INSTANT_OPEN_COUNT	1000
#endif
#ifdef ENABLE_PROMO_CODE_SYSTEM						// Kod promocyjny
	#define ENABLE_PROMO_CODE_ONE_USE_PER_ACCOUNT
#endif
#define ENABLE_EXTENDED_ITEMNAME_ON_GROUND			// Extended itemname on ground
//#define __BL_PARTY_POSITION__						// Party member position on minimap
//#define ENABLE_ZLOMIARZ_WINDOW					// Zlomiarz
#define __ENABLE_COLLECTIONS_SYSTEM__				// Kolekcjoner
#define ENABLE_LRN_BATTLE_PASS						// Battlepass
#define ENABLE_EVENT_MANAGER						// Okno eventow
#define ENABLE_DUNGEON_RETURN						// Powrot na dunga
#define ENABLE_VIP_ITEMS							// Itemy VIP
#define ENABLE_INSTANT_INGOT_OPEN					// Sztabki bez laga
#define ENABLE_AREZZO_CHEST_DROP					// Drop szkat arezzo z mobow.
#define ENABLE_DRAGON_SOUL_TIME_RENEWAL				// Odnawianie czasu alchemii
#define BL_OFFLINE_MESSAGE							// Wiadomoœci offline
#define USE_FISH_SYSTEM								// Rybki z bonusami
#define ENABLE_ODLAMKI_SYSTEM						// Wymiana kd na odlamki
#define ENABLE_EXTRABONUS_SYSTEM					// Wzmocnienia przedmiotow
//#define ENABLE_SKILL_SELECT_FEATURE
#define ENABLE_GOTO_LAG_FIX							// goto lag fix - tpmetkibossy
#define WJ_ENABLE_TRADABLE_ICON						// slot marketing
#define RENEWAL_DEAD_PACKET
#define __AUTO_QUQUE_ATTACK__
#define __VIEW_TARGET_PLAYER_HP__
#define __VIEW_TARGET_DECIMAL_HP__
#define ENABLE_ENTITY_PRELOADING
//#define ENABLE_MESSENGER_TEAM
#define ENABLE_ITEMSHOP_TIMED_BONUSES


//#define ENABLE_KILL_NOTICE							// Info na czacie o zbiciu bossa
//#define ENABLE_VS_SHOP_SEARCH						// Wyszukiwarka sklepy offline
#define ENABLE_VS_PELERYNKA_VIP						// Pelerynki vip
#define ENABLE_VS_COSTUME_BONUSES					// Bonusy czasowe w kostiumach


#define ENABLE_HWID_SYSTEM							//-> x
#ifdef ENABLE_HWID_SYSTEM
enum eHWIDSettings {
	CPU_ID_MAX_LEN 									= 128,	//	128
	HDD_MODEL_MAX_LEN 								= 128,	//	128
	MACHINE_GUID_MAX_LEN 							= 128,	//	128
	MAC_ADDR_MAX_LEN 								= 128,	//	128
	HDD_SERIAL_MAX_LEN 								= 128,	//	128
};
#endif
#define ENABLE_ANTY_CHEAT							//-> Antycheat
#define __AUTH_QUEUEING__
#define TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#	define LEGEND_DAMAGE_BOARD_MAX_ENTRIES	15
#endif
#define __ENABLE_TOGGLE_ITEMS__
#define ITEM_CHECKINOUT_UPDATE
#define ENABLE_QUIVER_SYSTEM


//#define /* @author: Owsap */ __ITEM_APPLY_RANDOM__ // Apply Random Individual Attributes

// Aslan Buff - 15.05.2023 - Start
#define ENABLE_EXTENDED_SOCKETS
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	// You can Deactivate or Activate this
	#define ASLAN_BUFF_NPC_USE_SKILL_17_TO_M
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
	#define ASLAN_BUFF_NPC_ENABLE_EQ
	// Config values ( !!! no deactivate this defines !!! ) 
	#define ASLAN_BUFF_NPC_START_INT 180
	#define ASLAN_BUFF_NPC_MAX_INT 300
	#define ASLAN_BUFF_NPC_MIN_HP_PERC_USE_SKILL_HEAL 30
	#define ASLAN_BUFF_EXP_RING_ITEM_1 72001
	#define ASLAN_BUFF_EXP_RING_ITEM_2 72002
	#define ASLAN_BUFF_EXP_RING_ITEM_3 72003
#endif
// Aslan Buff - End

#define ENABLE_NEW_PET_SYSTEM // pety
#ifdef ENABLE_NEW_PET_SYSTEM
	#define ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_VNUM (DWORD)30000 // ID itemu, ktory zostanie zabrany do zmiany nazwy peta
	#define ENABLE_NEW_PET_SYSTEM_CHANGE_NAME_ITEM_COUNT (WORD)1 // ile ma zabrac itemow- domyslnie 1
	#define ENABLE_NEW_PET_SYSTEM_BOOK // czytanie ksiêgi ale jeœli nie dzia³a jak nale¿y to polecam wy³¹czyæ dodaj¹c komentarz blokowy
#endif

//NOWE
//#define USE_LENTS_SHOULDER_SASH // W¨©?CZ JE?LI POSIADASZ SYSTEM SZARF
//

#define ENABLE_LONG_LONG							// Limit Yang
#define __ITEM_SHOP__								// ItemShop w Grze
#define __TOMBOLA__

//////////////////////////////////////////////////////////////////////////
// ### New Features ###
#define ENABLE_D_NJGUILD
#define ENABLE_FULL_NOTICE
#define ENABLE_NEWSTUFF
#define ENABLE_PORT_SECURITY
#define ENABLE_BELT_INVENTORY_EX
#define ENABLE_CMD_WARP_IN_DUNGEON
// #define ENABLE_ITEM_ATTR_COSTUME
// #define ENABLE_SEQUENCE_SYSTEM
#define ENABLE_PLAYER_PER_ACCOUNT5
//#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM

//#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_NEW_MOUNT_SYSTEM
#define __RENEWAL_MOUNT__

#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_QUEST_DIE_EVENT
#define ENABLE_QUEST_BOOT_EVENT
#define ENABLE_QUEST_DND_EVENT
//#define ENABLE_PET_SYSTEM_EX
#define ENABLE_SKILL_FLAG_PARTY
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_CHECK_SELL_PRICE

enum eCommonDefines {
	MAP_ALLOW_LIMIT = 32, // 32 default
};

//#define ENABLE_WOLFMAN_CHARACTER
//#ifdef ENABLE_WOLFMAN_CHARACTER
//#define USE_MOB_BLEEDING_AS_POISON
//#define USE_MOB_CLAW_AS_DAGGER
// #define USE_ITEM_BLEEDING_AS_POISON
// #define USE_ITEM_CLAW_AS_DAGGER
//#define USE_WOLFMAN_STONES
//#define USE_WOLFMAN_BOOKS
//#endif

// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
// #define USE_MAGIC_REDUCTION_STONES
#endif

// ### END New Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### Ex Features ###
#define DISABLE_STOP_RIDING_WHEN_DIE //	if DISABLE_TOP_RIDING_WHEN_DIE is defined, the player doesn't lose the horse after dying
#define ENABLE_ACCE_COSTUME_SYSTEM //fixed version
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS //enable only positive bonus in acce absorb
#define ENABLE_HIGHLIGHT_NEW_ITEM //if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_KILL_EVENT_FIX //if you want to fix the 0 exp problem about the when kill lua event (recommended)
// #define ENABLE_SYSLOG_PACKET_SENT // debug purposes

#define ENABLE_EXTEND_ITEM_AWARD //slight adjustement
#ifdef ENABLE_EXTEND_ITEM_AWARD
	// #define USE_ITEM_AWARD_CHECK_ATTRIBUTES //it prevents bonuses higher than item_attr lvl1-lvl5 min-max range limit
#endif

// ### END Ex Features ###
//////////////////////////////////////////////////////////////////////////

#endif
//martysama0134's ceqyqttoaf71vasf9t71218
