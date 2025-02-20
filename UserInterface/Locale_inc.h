#pragma once

#define CHECK_CLIENT_VERSION

#define ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	#define ENABLE_PRIVATE_SHOP_PREMIUM_TIME
	//#define ENABLE_PRIVATE_SHOP_BUNDLE_REQ
	#define ENABLE_PRIVATE_SHOP_CHEQUE
	#define ENABLE_PRIVATE_SHOP_LIMITED_DISTANCE_RENDERING
	#define ENABLE_PRIVATE_SHOP_BUILD_LIMITATIONS
	// #define ENABLE_PRIVATE_SHOP_LOCKED_SLOTS
	// #define ENABLE_PRIVATE_SHOP_PRICE_SORT
	#define ENABLE_PRIVATE_SHOP_TIME_SORT
	//#define ENABLE_PRIVATE_SHOP_DIFFERENT_PRICE_RESTRICTION
	//#define ENABLE_PRIVATE_SHOP_CHANGE_LOOK
	//#define ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
	// #define ENABLE_PRIVATE_SHOP_APPLY_RANDOM
#endif

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////


//////////////////TESTYYYYYYYY?????????????????????????????????? widaæ?
#define ENABLE_DROP_INFO_PCT						// Procent na drop w wiki dropu
#define ENABLE_BELT_ATTRIBUTES						// Bonowanie pasow w switchbocie
#define ENABLE_GLOVE_ATTRIBUTES						// Bonowanie rekawic w switchbocie

///NOWE
#define ENABLE_QUEST_RENEWAL // Quest page renewal
#define ENABLE_DROP_INFO							// Wikipedia Dropu
#define ENABLE_LRN_LOCATION_SYSTEM					// Zapis Lokalizacji
#define ENABLE_ALIGN_RENEWAL						// System Rang z Bonusami
#define ENABLE_SHINING_SYSTEM						// System Efektów
#define ENABLE_EXTENDED_BLEND						// Nieskoñczone dopalacze
#define ENABLE_NEW_PET_SYSTEM						// System Petów by Zoom
#define __BL_MOVE_CHANNEL__							// Zmiana kana³ów
//#define ENABLE_OFFLINE_SHOP_SYSTEM					// Sklepy Offline by Ups..
#define ENABLE_LONG_LONG							// Limit Yang (Long Long)
#define ENABLE_ITEMSHOP								// ItemShop in game by Filip

#define ENABLE_TOMBOLA								// Ko³o losu
#ifdef ENABLE_TOMBOLA
//#define ENABLE_TOMBOLA_PERCENTAGE
#endif

#define FAST_ITEMS_PICKUP
#define __ENABLE_TOGGLE_ITEMS__
#define ITEM_CHECKINOUT_UPDATE

#define WEEKLY_RANK_BYLUZER							// Tygodniowy Ranking				- 03.03.2023
#define TITLE_SYSTEM_BYLUZER						// System Tytu³ów					- 04.03.2023
#define ENABLE_SWITCHBOT							// SwitchBot						- 04.04.2023

#define WJ_SPLIT_INVENTORY_SYSTEM                   // Dodatkowy Ekwipunek				- 13.05.2023
#define ENABLE_CUBE_RENEWAL_WORLDARD				// Cube System						- 18.05.2023
#define ENABLE_REFINE_RENEWAL						// Auto Refine System				- 18.05.2023
#define ENABLE_FOV_OPTION							// Zmiana FOV						- 19.05.2023
#define ENABLE_CHEQUE_SYSTEM						// System Wonów
//#define ENABLE_DYNAMIC_FONTS						// Dynamiczna zmiana czcionki		- 18.06.2023
#define ENABLE_DOG_MODE								// Dynamiczna zmiana mobów na psy	- 18.06.2023
#define ENABLE_REMOVE_ITEM							// Kosz na Przedmioty by Filip		- 18.06.2023 
#define ENABLE_STOLE_COSTUME					
#define ENABLE_GLOVE_SYSTEM							// Rekawice							- 18.07.2023
#define ENABLE_SYSTEM_RUNE							// System Run						- 19.07.2023
#define ENABLE_AFFECT_POLYMORPH_REMOVE			
#define ENABLE_SKILL_AFFECT_REMOVE
#define ENABLE_ARTEFAKT_SYSTEM						//artefakty
#define ENABLE_DUNGEON_INFO_SYSTEM					// Dungeon Info						- 29.07.2023
#define __BL_CHEST_DROP_INFO__						//podglad dropu szkat
#define ENABLE_MINIMAP_DUNGEONINFO
#define RENEWAL_RENDERING_RECT_BOX
#define ENABLE_EXTENDED_ITEM_COUNT
#define ENABLE_PUNKTY_OSIAGNIEC						//po
#define ENABLE_SPECIAL_BONUS						//Dodatkowy Bonus w Broni i Zbroi
#define ENABLE_AURA_SYSTEM							//szaty aury
#define ENABLE_NEW_STONE_DETACH						//Okno wyciagania kd
#define ENABLE_HIDE_COSTUME_SYSTEM					//Ukrywanie kostiumow
#define ENABLE_COLLECT_WINDOW						//System kolekcjonera w GUI.
#define ENABLE_COSTUME_EMBLEMAT						//emblematy
#define ENABLE_SKILE_PASYWNE						// Skile pasywne
#define HIDE_EMPIRE_FLAG							//Ukrywanie flag królestwa dla petów, sklepów offline
#define ENABLE_CHATTING_WINDOW_RENEWAL				// Chat system
#define ENABLE_INSTANT_CHEST_OPEN					// Szybkie otwieranie szkat
#define BL_OFFLINE_MESSAGE							// Wiadomoœci offline
#define ENABLE_EXTENDED_ITEMNAME_ON_GROUND			// Extended itemname ground
//#define __BL_PARTY_POSITION__						// Party member position on minimap
#define ENABLE_COLLECTIONS_SYSTEM					// Kolekcjoner
#define WJ_MULTI_TEXTLINE							// ? multitextline
#define ENABLE_LRN_BATTLE_PASS						// Battlepass
#define ENABLE_EVENT_MANAGER						// Okno Eventow
#define __BACK_DUNGEON__							// Powrot na dungi
#define ENABLE_INSTANT_INGOT_OPEN					// Uzywanie sztabek bez laga
#define USE_FISH_SYSTEM								// Rybki z bonusami
#define ENABLE_ODLAMKI_SYSTEM						// Wymiana kd na odlamki
#define __BL_SMOOTH_SCROLL__						// smooth scroll
#define ENABLE_KEYCHANGE_SYSTEM						// ustawienia klawiszy
#define ENABLE_EXTRABONUS_SYSTEM					// wzmocnienia
#define ENABLE_QUIVER_SYSTEM						// kolczany ninja
#define ENABLE_SKILL_TABLE_RENEW					// Enable synchronization skill_proto -> skilltable
#define __BL_HYPERLINK_ITEM_ICON__



//#define ENABLE_VS_SHOP_SEARCH						// Wyszukiwarka sklepy offline
#define ENABLE_VS_COSTUME_BONUSES					// bonusy czasowe w kostiumach

#define WJ_ENABLE_TRADABLE_ICON
#define RENEWAL_DEAD_PACKET
#define __AUTO_QUQUE_ATTACK__
#define ENABLE_VIEW_TARGET_PLAYER_HP
#define ENABLE_VIEW_TARGET_DECIMAL_HP
#define ENABLE_ENTITY_PRELOADING
//#define ENABLE_MESSENGER_TEAM

#define ENABLE_HWID_SYSTEM							// hwid bany
#define TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
#	define LEGEND_DAMAGE_BOARD_MAX_ENTRIES	15
#endif

//OPTYMALIZACJE
//#define ENABLE_LOADING_PERFORMANCE			// Client loading performance ( Boost Loading Time )
//#define __LOADING_PERFORMANCE_CHECK__
#define ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
//#define ENABLE_LOADING_PERFORMANCE
//#define IMPROVE_ENTITY_FREEZE
//#define MEMORYLEAK_FIX_OWSAP


#define ENABLE_FIX_MOBS_LAG
#if defined(ENABLE_FIX_MOBS_LAG)
	// -> The define ENABLE_FIX_MOBS_LAG have problems in device reseting.
	// -> With this new define all this problems are fixed.
#define FIX_MOBS_LAG_FIX
#endif

//FIXY
#define ENABLE_AFFECT_FIX
//#define TRANSPARENCY_BUILDING_FIX

// Aslan Buff - START - 15.05.2023
#define ENABLE_EXTENDED_SOCKETS
#define ENABLE_ASLAN_BUFF_NPC_SYSTEM
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	#define ASLAN_BUFF_NPC_USE_SKILL_TECH_LEVEL
#endif
// Aslan Buff - END
// 

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT
#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_LEVEL_IN_TRADE
#define ENABLE_DICE_SYSTEM
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
#define ENABLE_TEXT_LEVEL_REFRESH
#define ENABLE_USE_COSTUME_ATTR
#define ENABLE_DISCORD_RPC
#define ENABLE_PET_SYSTEM_EX
//#define ENABLE_LOCALE_EX
#define ENABLE_NO_DSS_QUALIFICATION
#define ENABLE_NO_SELL_PRICE_DIVIDED_BY_5
#define ENABLE_RESP_SYSTEM


// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
#//define ENABLE_WOLFMAN_CHARACTER
// #define ENABLE_MAGIC_REDUCTION_SYSTEM
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
// ### From GameLib ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New System Defines - Extended Version ###

// if is define ENABLE_ACCE_COSTUME_SYSTEM the players can use shoulder sash
// if you want to use object scaling function you must defined ENABLE_OBJ_SCALLING
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_OBJ_SCALLING
// #define USE_ACCE_ABSORB_WITH_NO_NEGATIVE_BONUS

// if you want use SetMouseWheelScrollEvent or you want use mouse wheel to move the scrollbar
#define ENABLE_MOUSEWHEEL_EVENT

//if you want to see highlighted a new item when dropped or when exchanged
#define ENABLE_HIGHLIGHT_NEW_ITEM

// it shows emojis in the textlines
#define ENABLE_EMOJI_SYSTEM

// effects while hidden won't show up
//#define __ENABLE_STEALTH_FIX__

// circle dots in minimap instead of squares
#define ENABLE_MINIMAP_WHITEMARK_CIRCLE

// for debug: print received packets
#define ENABLE_PRINT_RECV_PACKET_DEBUG


#define D3DLIGHT8					D3DLIGHT9
#define D3DMATERIAL8				D3DMATERIAL9
#define IDirect3DVertexBuffer8		IDirect3DVertexBuffer9
#define LPDIRECT3DTEXTURE8			LPDIRECT3DTEXTURE9
#define LPDIRECT3DSURFACE8			LPDIRECT3DSURFACE9
#define D3DVIEWPORT8				D3DVIEWPORT9
#define LPDIRECT3DDEVICE8			LPDIRECT3DDEVICE9
#define LPDIRECT3DVERTEXBUFFER8		LPDIRECT3DVERTEXBUFFER9
#define LPDIRECT3DINDEXBUFFER8		LPDIRECT3DINDEXBUFFER9
#define DXLOCKTYPE					(void**)
#define D3DVERTEXELEMENT8			D3DVERTEXELEMENT9
#define LPDIRECT3DBASETEXTURE8		LPDIRECT3DBASETEXTURE9
#define LPDIRECT3DPIXELSHADER8		LPDIRECT3DPIXELSHADER9
#define D3DADAPTER_IDENTIFIER8		D3DADAPTER_IDENTIFIER9
#define IDirect3D8					IDirect3D9
#define IDirect3DDevice8			IDirect3DDevice9
#define D3DCAPS8					D3DCAPS9
#define LPDIRECT3D8					LPDIRECT3D9
#define D3DCAPS2_CANRENDERWINDOWED	DDCAPS2_CANRENDERWINDOWED
#define IDirect3DTexture8			IDirect3DTexture9
#define Direct3DCreate8				Direct3DCreate9
#define IDirect3DSurface8			IDirect3DSurface9