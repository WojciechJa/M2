#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Column = 5;
const DWORD c_Inventory_Page_Row = 9;
const DWORD c_Inventory_Page_Size = c_Inventory_Page_Column*c_Inventory_Page_Row; // x*y
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
const DWORD c_Inventory_Page_Count = 4;
#else
const DWORD c_Inventory_Page_Count = 2;
#endif
const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_Equipment_Count = 12;
#ifdef ENABLE_ARTEFAKT_SYSTEM
const DWORD c_Artefakty_Slots = 12;
#endif

const DWORD c_Equipment_Start = c_ItemSlot_Count;

const DWORD c_Equipment_Body	= c_Equipment_Start + CItemData::WEAR_BODY;
const DWORD c_Equipment_Head	= c_Equipment_Start + CItemData::WEAR_HEAD;
const DWORD c_Equipment_Shoes	= c_Equipment_Start + CItemData::WEAR_FOOTS;
const DWORD c_Equipment_Wrist	= c_Equipment_Start + CItemData::WEAR_WRIST;
const DWORD c_Equipment_Weapon	= c_Equipment_Start + CItemData::WEAR_WEAPON;
const DWORD c_Equipment_Neck	= c_Equipment_Start + CItemData::WEAR_NECK;
const DWORD c_Equipment_Ear		= c_Equipment_Start + CItemData::WEAR_EAR;
const DWORD c_Equipment_Unique1	= c_Equipment_Start + CItemData::WEAR_UNIQUE1;
const DWORD c_Equipment_Unique2	= c_Equipment_Start + CItemData::WEAR_UNIQUE2;
const DWORD c_Equipment_Arrow	= c_Equipment_Start + CItemData::WEAR_ARROW;
const DWORD c_Equipment_Shield	= c_Equipment_Start + CItemData::WEAR_SHIELD;
#ifdef ENABLE_GLOVE_SYSTEM
const DWORD c_Equipment_Glove	= c_Equipment_Start + CItemData::WEAR_GLOVE;
#endif


#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
const DWORD c_New_Equipment_Start = c_Equipment_Start + 27;
#ifdef ENABLE_NEW_PET_SYSTEM
	const DWORD c_New_Equipment_Count = 3 + 4 + 1+6;
#else	const DWORD c_New_Equipment_Count = 3;
#endif
	const DWORD c_Equipment_Ring1 = c_New_Equipment_Start + 0;
	const DWORD c_Equipment_Ring2 = c_New_Equipment_Start + 1;
	const DWORD c_Equipment_Belt  = c_New_Equipment_Start + 2;
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	const DWORD c_SlotNewPet = c_Equipment_Start + 30;
	const DWORD c_SlotNewPetEqStart = c_Equipment_Start + 31;
	const DWORD c_SlotNewPetEqEnd = c_SlotNewPetEqStart + 3;
#endif //ENABLE_NEW_PET_SYSTEM
#ifdef ENABLE_SYSTEM_RUNE
	const DWORD c_Equipment_Rune = c_Equipment_Start + CItemData::WEAR_RUNE;
	const DWORD c_Equipment_Rune_Red = c_Equipment_Start + CItemData::WEAR_RUNE_RED;
	const DWORD c_Equipment_Rune_Yellow = c_Equipment_Start + CItemData::WEAR_RUNE_BLUE;
	const DWORD c_Equipment_Rune_Green = c_Equipment_Start + CItemData::WEAR_RUNE_GREEN;
	const DWORD c_Equipment_Rune_Blue = c_Equipment_Start + CItemData::WEAR_RUNE_YELLOW;
	const DWORD c_Equipment_Rune_Black = c_Equipment_Start + CItemData::WEAR_RUNE_BLACK;
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
	const DWORD c_Artefakty_Slot1 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT1;
	const DWORD c_Artefakty_Slot2 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT2;
	const DWORD c_Artefakty_Slot3 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT3;
	const DWORD c_Artefakty_Slot4 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT4;
	const DWORD c_Artefakty_Slot5 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT5;
	const DWORD c_Artefakty_Slot6 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT6;
	const DWORD c_Artefakty_Slot7 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT7;
	const DWORD c_Artefakty_Slot8 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT8;
	const DWORD c_Artefakty_Slot9 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT9;
	const DWORD c_Artefakty_Slot10 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT10;
	const DWORD c_Artefakty_Slot11 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT11;
	const DWORD c_Artefakty_Slot12 = c_Equipment_Start + CItemData::WEAR_ARTEFAKT12;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	const DWORD c_Equipment_Mount = c_Equipment_Start + CItemData::WEAR_MOUNT_EQUIPMENT;
#endif

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
	ON_TOP_WND_PET_FEED,

	ON_TOP_WND_MAX,
};
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,
};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start	= c_Equipment_Start + 19;
	const DWORD	c_Costume_Slot_Body		= c_Costume_Slot_Start + 0;
	const DWORD	c_Costume_Slot_Hair		= c_Costume_Slot_Start + 1;
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Mount	= c_Costume_Slot_Start + 2;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Acce		= c_Costume_Slot_Start + 3;
#endif
#ifdef ENABLE_STOLE_COSTUME
	const DWORD	c_Costume_Slot_Stole = c_Costume_Slot_Start + 4;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon = c_Costume_Slot_Start + 5;
#endif
#ifdef ENABLE_AURA_SYSTEM
	const DWORD c_Costume_Slot_Aura = c_Costume_Slot_Start + 6;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	const DWORD	c_Costume_Slot_Emblemat = c_Costume_Slot_Start + 7;
#endif
	const DWORD c_Costume_Slot_Count = 8;

	const DWORD c_Costume_Slot_End		= c_Costume_Slot_Start + c_Costume_Slot_Count;

#endif

const DWORD c_Wear_Max = CItemData::WEAR_MAX_NUM;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	// 벨트 아이템이 제공하는 인벤토리
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 4;
	const DWORD c_Belt_Inventory_Height= 4;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;
#endif

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	const DWORD c_Skill_Book_Inventory_Slot_Start = c_Belt_Inventory_Slot_End;
	const DWORD c_Skill_Book_Inventory_Slot_Count = 135;
	const DWORD c_Skill_Book_Inventory_Slot_End = c_Skill_Book_Inventory_Slot_Start + c_Skill_Book_Inventory_Slot_Count;

	const DWORD c_Upgrade_Items_Inventory_Slot_Start = c_Skill_Book_Inventory_Slot_End;
	const DWORD c_Upgrade_Items_Inventory_Slot_Count = 135;
	const DWORD c_Upgrade_Items_Inventory_Slot_End = c_Upgrade_Items_Inventory_Slot_Start + c_Upgrade_Items_Inventory_Slot_Count;

	const DWORD c_Stone_Inventory_Slot_Start = c_Upgrade_Items_Inventory_Slot_End;
	const DWORD c_Stone_Inventory_Slot_Count = 135;
	const DWORD c_Stone_Inventory_Slot_End = c_Stone_Inventory_Slot_Start + c_Stone_Inventory_Slot_Count;

	const DWORD c_Box_Inventory_Slot_Start = c_Stone_Inventory_Slot_End;
	const DWORD c_Box_Inventory_Slot_Count = 135;
	const DWORD c_Box_Inventory_Slot_End = c_Box_Inventory_Slot_Start + c_Box_Inventory_Slot_Count;

	const DWORD c_Efsun_Inventory_Slot_Start = c_Box_Inventory_Slot_End;
	const DWORD c_Efsun_Inventory_Slot_Count = 135;
	const DWORD c_Efsun_Inventory_Slot_End = c_Efsun_Inventory_Slot_Start + c_Efsun_Inventory_Slot_Count;

	const DWORD c_Cicek_Inventory_Slot_Start = c_Efsun_Inventory_Slot_End;
	const DWORD c_Cicek_Inventory_Slot_Count = 135;
	const DWORD c_Cicek_Inventory_Slot_End = c_Cicek_Inventory_Slot_Start + c_Cicek_Inventory_Slot_Count;

	const DWORD c_Inventory_Count = c_Cicek_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count = c_DragonSoul_Equip_End;
#endif

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM 
const DWORD c_Buff_Equipment_Slot_Start = 0;
const DWORD c_Buff_Equipment_Slot_End = c_Buff_Equipment_Slot_Start + CItemData::BUFF_WINDOW_SLOT_MAX_NUM;
#endif	

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	SLOT_TYPE_BUFF_EQUIPMENT,	// SLOT_TYPE_BUFF_EQUIPMENT
#endif
#ifdef ENABLE_SWITCHBOT
	SLOT_TYPE_SWITCHBOT,
#endif
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	SLOT_TYPE_SKILL_BOOK_INVENTORY,
	SLOT_TYPE_UPGRADE_ITEMS_INVENTORY,
	SLOT_TYPE_STONE_INVENTORY,
	SLOT_TYPE_BOX_INVENTORY,
	SLOT_TYPE_EFSUN_INVENTORY,
	SLOT_TYPE_CICEK_INVENTORY,
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SLOT_TYPE_OFFLINE_SHOP,
#endif
	SLOT_TYPE_MAX,
};

#if defined(ENABLE_KEYCHANGE_SYSTEM)
enum EKeySettings
{
	KEY_NONE,
	KEY_MOVE_UP_1,
	KEY_MOVE_DOWN_1,
	KEY_MOVE_LEFT_1,
	KEY_MOVE_RIGHT_1,
	KEY_MOVE_UP_2,
	KEY_MOVE_DOWN_2,
	KEY_MOVE_LEFT_2,
	KEY_MOVE_RIGHT_2,
	KEY_CAMERA_ROTATE_POSITIVE_1,
	KEY_CAMERA_ROTATE_NEGATIVE_1,
	KEY_CAMERA_ZOOM_POSITIVE_1,
	KEY_CAMERA_ZOOM_NEGATIVE_1,
	KEY_CAMERA_PITCH_POSITIVE_1,
	KEY_CAMERA_PITCH_NEGATIVE_1,
	KEY_CAMERA_ROTATE_POSITIVE_2,
	KEY_CAMERA_ROTATE_NEGATIVE_2,
	KEY_CAMERA_ZOOM_POSITIVE_2,
	KEY_CAMERA_ZOOM_NEGATIVE_2,
	KEY_CAMERA_PITCH_POSITIVE_2,
	KEY_CAMERA_PITCH_NEGATIVE_2,
	KEY_ROOTING_1,
	KEY_ROOTING_2,
	KEY_ATTACK,
	KEY_RIDEMYHORS,
	KEY_FEEDMYHORS,
	KEY_BYEMYHORS,
	KEY_RIDEHORS,
	KEY_EMOTION1,
	KEY_EMOTION2,
	KEY_EMOTION3,
	KEY_EMOTION4,
	KEY_EMOTION5,
	KEY_EMOTION6,
	KEY_EMOTION7,
	KEY_EMOTION8,
	KEY_EMOTION9,
	KEY_SLOT_1,
	KEY_SLOT_2,
	KEY_SLOT_3,
	KEY_SLOT_4,
	KEY_SLOT_5,
	KEY_SLOT_6,
	KEY_SLOT_7,
	KEY_SLOT_8,
	KEY_SLOT_CHANGE_1,
	KEY_SLOT_CHANGE_2,
	KEY_SLOT_CHANGE_3,
	KEY_SLOT_CHANGE_4,
	KEY_OPEN_STATE,
	KEY_OPEN_SKILL,
	KEY_OPEN_QUEST,
	KEY_OPEN_INVENTORY,
	KEY_OPEN_DDS,
	KEY_OPEN_MINIMAP,
	KEY_OPEN_LOGCHAT,
	KEY_OPEN_PET,
	KEY_OPEN_GUILD,
	KEY_OPEN_MESSENGER,
	KEY_OPEN_HELP,
	KEY_OPEN_ACTION,
	KEY_SCROLL_ONOFF,
	KEY_PLUS_MINIMAP,
	KEY_MIN_MINIMAP,
	KEY_SCREENSHOT,
	KEY_SHOW_NAME,
	KEY_OPEN_AUTO,
	KEY_AUTO_RUN,
	KEY_TP_MAP,
	KEY_MONSTER_CARD,
	KEY_PARTY_MATCH,
	KEY_SELECT_DSS_1,
	KEY_SELECT_DSS_2,
	KEY_PASSIVE_ATTR1,
	KEY_PASSIVE_ATTR2,
	KEY_OPEN_EXTENDED_INV,
	KEY_OPEN_DOPY,
	KEY_OPEN_MARBLE,
	KEY_OPEN_SAVE_LOCATION,
	KEY_OPEN_RESP,
	KEY_OPEN_BUFF,
	KEY_OPEN_MISSION,
	KEY_OPEN_COLLECTION,
	KEY_RETURN,
	KEY_OPEN_DUNGEONS,
	KEY_CHANGECHANNEL1,
	KEY_CHANGECHANNEL2,
	KEY_CHANGECHANNEL3,
	KEY_CHANGECHANNEL4,
	KEY_CHANGECHANNEL5,
	KEY_CHANGECHANNEL6,

	KEY_ADDKEYBUFFERCONTROL = 100,
	KEY_ADDKEYBUFFERALT = 300,
	KEY_ADDKEYBUFFERSHIFT = 500,
};
#endif

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
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
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

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#pragma pack (push, 1)
#define WORD_MAX 0xffff

#ifdef ENABLE_SWITCHBOT
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5,
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
#ifdef ENABLE_BELT_ATTRIBUTES
	ATTRIBUTE_SET_BELT,
#endif
#ifdef ENABLE_GLOVE_ATTRIBUTES
	ATTRIBUTE_SET_GLOVE,
#endif
	ATTRIBUTE_SET_MAX_NUM,
};

#endif


typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
    SItemPos ()
    {
		window_type =     INVENTORY;
		cell = WORD_MAX;
    }
	SItemPos (BYTE _window_type, WORD _cell)
    {
        window_type = _window_type;
        cell = _cell;
    }

  //  int operator=(const int _cell)
  //  {
		//window_type = INVENTORY;
  //      cell = _cell;
  //      return cell;
  //  }
	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		case BUFF_EQUIPMENT:
			return cell < (CItemData::BUFF_WINDOW_SLOT_MAX_NUM);
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
			break;
#endif
		default:
			return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
			return false;
			break;

		default:
			return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	bool IsSkillBookInventoryCell()
	{
		bool bResult = c_Skill_Book_Inventory_Slot_Start <= cell && c_Skill_Book_Inventory_Slot_End > cell;
		return bResult;
	}
	
	bool IsUpgradeItemsInventoryCell()
	{
		bool bResult = c_Upgrade_Items_Inventory_Slot_Start <= cell && c_Upgrade_Items_Inventory_Slot_End > cell;
		return bResult;
	}
	
	bool IsStoneInventoryCell()
	{
		bool bResult = c_Stone_Inventory_Slot_Start <= cell && c_Stone_Inventory_Slot_End > cell;
		return bResult;
	}
	
	bool IsBoxInventoryCell()
	{
		bool bResult = c_Box_Inventory_Slot_Start <= cell && c_Box_Inventory_Slot_End > cell;
		return bResult;
	}
	
	bool IsEfsunInventoryCell()
	{
		bool bResult = c_Efsun_Inventory_Slot_Start <= cell && c_Efsun_Inventory_Slot_End > cell;
		return bResult;
	}
	
	bool IsCicekInventoryCell()
	{
		bool bResult = c_Cicek_Inventory_Slot_Start <= cell && c_Cicek_Inventory_Slot_End > cell;
		return bResult;
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
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
#ifndef ENABLE_EXTENDED_SOCKETS
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
#else
	ITEM_SOCKET_SLOT_MAX_NUM = 6,
	ITEM_STONES_MAX_NUM = 3, //If you are extending stones in item, change to 6. If you do not want more than 3 stones, keep 3.
#endif
	// refactored attribute slot begin
	ITEM_ATTRIBUTE_SLOT_NORM_NUM	= 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM	= 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START	= 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END	= ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START	= ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END	= ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM		= ITEM_ATTRIBUTE_SLOT_RARE_END, // 7
	// refactored attribute slot end
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
    BYTE        bType;
    short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
    DWORD       vnum;
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	int			count;
#else
	BYTE		count;
#endif
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TItemData;

#ifdef ENABLE_VS_SHOP_SEARCH
typedef struct search_item
{
	DWORD		vnum;
} TSearchItems;
#endif

typedef struct packet_shop_item
{
    DWORD       vnum;
#ifdef ENABLE_LONG_LONG
	long long	price;
#else
	DWORD		price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	long long 	cheque;
#endif
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	int			count;
#else
	BYTE		count;
#endif
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
    TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TShopItemData;

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName
#ifdef ENABLE_DYNAMIC_FONTS
	, bool reload = false
#endif
						);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);

//martysama0134's ceqyqttoaf71vasf9t71218
