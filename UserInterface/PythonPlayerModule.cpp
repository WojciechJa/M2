#include "StdAfx.h"
#include "PythonPlayer.h"
#include "PythonApplication.h"
#include "../GameLib/GameLibDefines.h"

#ifndef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
extern const DWORD c_iSkillIndex_Tongsol	= 121;
extern const DWORD c_iSkillIndex_Combo		= 122;
extern const DWORD c_iSkillIndex_Fishing	= 123;
extern const DWORD c_iSkillIndex_Mining		= 124;
extern const DWORD c_iSkillIndex_Making		= 125;
extern const DWORD c_iSkillIndex_Language1	= 126;
extern const DWORD c_iSkillIndex_Language2	= 127;
extern const DWORD c_iSkillIndex_Language3	= 128;
extern const DWORD c_iSkillIndex_Polymorph	= 129;
extern const DWORD c_iSkillIndex_Riding		= 130;
extern const DWORD c_iSkillIndex_Summon		= 131;
#endif
enum
{
	EMOTION_CLAP		= 1,
	EMOTION_CHEERS_1,
	EMOTION_CHEERS_2,
	EMOTION_DANCE_1,
	EMOTION_DANCE_2,
	EMOTION_DANCE_3,
	EMOTION_DANCE_4,
	EMOTION_DANCE_5,
	EMOTION_DANCE_6,
	EMOTION_CONGRATULATION,
	EMOTION_FORGIVE,
	EMOTION_ANGRY,

	EMOTION_ATTRACTIVE,
	EMOTION_SAD,
	EMOTION_SHY,
	EMOTION_CHEERUP,
	EMOTION_BANTER,
	EMOTION_JOY,

	EMOTION_KISS		= 51,
	EMOTION_FRENCH_KISS,
	EMOTION_SLAP,
};

#ifndef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
std::map<int, CGraphicImage *> m_kMap_iEmotionIndex_pkIconImage;
#endif

extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
class CBeltInventoryHelper
{
public:
	typedef BYTE	TGradeUnit;

	static TGradeUnit GetBeltGradeByRefineLevel(int refineLevel)
	{
		static TGradeUnit beltGradeByLevelTable[] =
		{
			0,
			1,			// +1
			1,			// +2
			2,			// +3
			2,			// +4,
			3,			// +5
			4,			// +6,
			5,			// +7,
			6,			// +8,
			7,			// +9
		};

		return beltGradeByLevelTable[refineLevel];
	}

	static const TGradeUnit* GetAvailableRuleTableByGrade()
	{
		static TGradeUnit availableRuleByGrade[c_Belt_Inventory_Slot_Count] = {
			1, 2, 4, 6,
			3, 3, 4, 6,
			5, 5, 5, 6,
			7, 7, 7, 7
		};

		return availableRuleByGrade;
	}

	static bool IsAvailableCell(WORD cell, int beltGrade /*int beltLevel*/)
	{
		//const TGradeUnit beltGrade = GetBeltGradeByRefineLevel(beltLevel);
		const TGradeUnit* ruleTable = GetAvailableRuleTableByGrade();

		return ruleTable[cell] <= beltGrade;
	}
};
#endif

PyObject * playerPickCloseItem(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().PickCloseItem();
	return Py_BuildNone();
}

#ifdef FAST_ITEMS_PICKUP
PyObject* playerPickCloseItemVector(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().PickCloseItemVector();
	return Py_BuildNone();
}
#endif

PyObject * playerSetGameWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject * pyHandle;
	if (!PyTuple_GetObject(poArgs, 0, &pyHandle))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetGameWindow(pyHandle);
	return Py_BuildNone();
}

PyObject * playerSetQuickCameraMode(PyObject* poSelf, PyObject* poArgs)
{
	int nIsEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nIsEnable))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetQuickCameraMode(nIsEnable ? true : false);

	return Py_BuildNone();
}

PyObject* playerIsAntiFlagBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int antiflag;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &antiflag))
			return Py_BadArgument();
		break;

	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &antiflag))
			return Py_BadArgument();
		break;
	default:
		return Py_BuildException();
	}

	DWORD itemAntiFlags = CPythonPlayer::Instance().GetItemAntiFlags(Cell);
	return Py_BuildValue("i", IS_SET(itemAntiFlags, antiflag) != 0 ? TRUE : FALSE);
}

PyObject* playerGetItemTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BadArgument();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemTypeBySlot(Cell));
}

PyObject* playerGetItemSubTypeBySlot(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BadArgument();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonPlayer::Instance().GetItemSubTypeBySlot(Cell));
}

PyObject* playerIsSameItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iItemVNum;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iItemVNum))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iItemVNum))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BuildException();
	}

	return Py_BuildValue("i", (iItemVNum == CPythonPlayer::Instance().GetItemIndex(Cell)) ? TRUE : FALSE);
}

// Test Code
PyObject * playerSetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetMainCharacterIndex(iVID);
	CPythonCharacterManager::Instance().SetMainInstance(iVID);

	return Py_BuildNone();
}
// Test Code

PyObject * playerGetMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetMainCharacterIndex());
}

PyObject * playerGetMainCharacterName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject * playerGetMainCharacterPosition(PyObject* poSelf, PyObject* poArgs)
{
	TPixelPosition kPPosMainActor;
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_GetMainActorPosition(&kPPosMainActor);
	return Py_BuildValue("fff", kPPosMainActor.x, kPPosMainActor.y, kPPosMainActor.z);
}

PyObject * playerIsMainCharacterIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsMainCharacterIndex(iVID));
}

PyObject * playerCanAttackInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsAttackableInstance(*pTargetInstance));
}

PyObject * playerIsActingEmotion(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsActingEmotion());
}

PyObject * playerIsPVPInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", 0);
	if (!pTargetInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pMainInstance->IsPVPInstance(*pTargetInstance));
}

PyObject * playerIsSameEmpire(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);
	if (!pTargetInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsSameEmpire(*pTargetInstance));
}

PyObject * playerIsChallengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsChallengeInstance(iVID));
}

PyObject * playerIsRevengeInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsRevengeInstance(iVID));
}

PyObject * playerIsCantFightInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsCantFightInstance(iVID));
}

PyObject * playerGetCharacterDistance(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pMainInstance)
		return Py_BuildValue("f", -1.0f);
	if (!pTargetInstance)
		return Py_BuildValue("f", -1.0f);

	return Py_BuildValue("f", pMainInstance->GetDistance(pTargetInstance));
}

PyObject * playerIsInSafeArea(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsInSafe());
}

PyObject * playerIsMountingHorse(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pMainInstance)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pMainInstance->IsMountingHorse());
}

PyObject * playerIsObserverMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.IsObserverMode());
}

PyObject * playerActEmotion(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.ActEmotion(iVID);
	return Py_BuildNone();
}

PyObject * playerShowPlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Show();
	return Py_BuildNone();
}

PyObject * playerHidePlayer(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pMainInstance)
		pMainInstance->GetGraphicThingInstanceRef().Hide();
	return Py_BuildNone();
}

PyObject * playerComboAttack(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Attack();
	return Py_BuildNone();
}

PyObject * playerRegisterEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, false))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject * playerRegisterCacheEffect(PyObject* poSelf, PyObject* poArgs)
{
	int iEft;
	if (!PyTuple_GetInteger(poArgs, 0, &iEft))
		return Py_BadArgument();

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	if (!rkPlayer.RegisterEffect(iEft, szFileName, true))
		return Py_BuildException("CPythonPlayer::RegisterEffect(eEft=%d, szFileName=%s", iEft, szFileName);

	return Py_BuildNone();
}

PyObject * playerSetAttackKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 0, &isPressed))
		return Py_BuildException("playerSetAttackKeyState(isPressed) - There is no first argument");

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.SetAttackKeyState(isPressed ? true : false);
	return Py_BuildNone();
}

PyObject * playerSetSingleDIKKeyState(PyObject* poSelf, PyObject* poArgs)
{
	int eDIK;
	if (!PyTuple_GetInteger(poArgs, 0, &eDIK))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no first argument");

	int isPressed;
	if (!PyTuple_GetInteger(poArgs, 1, &isPressed))
		return Py_BuildException("playerSetSingleDIKKeyState(eDIK, isPressed) - There is no second argument");

	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();
	rkPlayer.NEW_SetSingleDIKKeyState(eDIK, isPressed ? true : false);
	return Py_BuildNone();
}

PyObject * playerEndKeyWalkingImmediately(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_Stop();
	return Py_BuildNone();
}

PyObject * playerStartMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject * playerEndMouseWalking(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject * playerResetCameraRotation(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().NEW_ResetCameraRotation();
	return Py_BuildNone();
}

PyObject* playerSetAutoCameraRotationSpeed(PyObject* poSelf, PyObject* poArgs)
{
	float fCmrRotSpd;
	if (!PyTuple_GetFloat(poArgs, 0, &fCmrRotSpd))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetAutoCameraRotationSpeed(fCmrRotSpd);
	return Py_BuildNone();
}

PyObject* playerSetMouseState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseState(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerSetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 1, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseFunc(eMBT, eMBS);

	return Py_BuildNone();
}

PyObject* playerGetMouseFunc(PyObject* poSelf, PyObject* poArgs)
{
	int eMBT;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBT))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.NEW_GetMouseFunc(eMBT));
}

PyObject* playerSetMouseMiddleButtonState(PyObject* poSelf, PyObject* poArgs)
{
	int eMBS;
	if (!PyTuple_GetInteger(poArgs, 0, &eMBS))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.NEW_SetMouseMiddleButtonState(eMBS);

	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject * playerGetName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPlayer::Instance().GetName());
}

PyObject * playerGetRace(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetRace());
}

PyObject* playerGetOriginalRace(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetOriginalRace());
}

PyObject * playerGetJob(PyObject* poSelf, PyObject* poArgs)
{
	int race = CPythonPlayer::Instance().GetRace();
	int job = RaceToJob(race);
	return Py_BuildValue("i", job);
}

PyObject * playerGetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPlayTime());
}

PyObject * playerSetPlayTime(PyObject* poSelf, PyObject* poArgs)
{
	int iTime;
	if (!PyTuple_GetInteger(poArgs, 0, &iTime))
		return Py_BuildException();

	CPythonPlayer::Instance().SetPlayTime(iTime);
	return Py_BuildNone();
}

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
PyObject * playerGetCurrentOfflineShopMoney(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("L", CPythonPlayer::Instance().GetCurrentOfflineShopMoney());
}
PyObject* playerGetCurrentOfflineShopZarobek(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPlayer::Instance().GetCurrentOfflineShopZarobek());
}
PyObject* playerGetCurrentOfflineShopZarobekCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPlayer::Instance().GetCurrentOfflineShopZarobekCheque());
}
PyObject* playerGetCurrentOfflineShopStatus(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetCurrentOfflineShopStatus());
}
PyObject* playerGetCurrentOfflineShopChequeMoney(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPlayer::Instance().GetCurrentOfflineShopChequeMoney());
}
#endif

PyObject * playerIsSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillCoolTime(iSlotIndex));
}

PyObject * playerGetSkillCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	float fCoolTime = CPythonPlayer::Instance().GetSkillCoolTime(iSlotIndex);
	float fElapsedCoolTime = CPythonPlayer::Instance().GetSkillElapsedCoolTime(iSlotIndex);
	return Py_BuildValue("ff", fCoolTime, fElapsedCoolTime);
}

PyObject * playerIsSkillActive(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsSkillActive(iSlotIndex));
}

PyObject * playerUseGuildSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlotIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().UseGuildSkill(iSkillSlotIndex);
	return Py_BuildNone();
}

PyObject * playerAffectIndexToSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iAffectIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iAffectIndex))
		return Py_BuildException();

	DWORD dwSkillIndex;
	if (!CPythonPlayer::Instance().AffectIndexToSkillIndex(iAffectIndex, &dwSkillIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", dwSkillIndex);
}

PyObject * playerGetEXP(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwEXP = CPythonPlayer::Instance().GetStatus(POINT_EXP);
	return Py_BuildValue("l", dwEXP);
}

PyObject* playerGetLevel(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwLevel = CPythonPlayer::Instance().GetStatus(POINT_LEVEL);
	return Py_BuildValue("l", dwLevel);
}

PyObject * playerGetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	long iValue = CPythonPlayer::Instance().GetStatus(iType);

	if (POINT_ATT_SPEED == iType)
	{
		CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
		if (pInstance && (CItemData::WEAPON_TWO_HANDED == pInstance->GetWeaponType()))
		{
			iValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
		}
	}

	return Py_BuildValue("i", iValue);
}

PyObject * playerSetStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iValue;
	if (!PyTuple_GetInteger(poArgs, 1, &iValue))
		return Py_BuildException();

	CPythonPlayer::Instance().SetStatus(iType, iValue);
	return Py_BuildNone();
}

#ifdef ENABLE_LONG_LONG
PyObject * playerGetElk(PyObject* poSelf, PyObject* poArgs)
{
	return PyLong_FromLongLong(CPythonPlayer::Instance().GetStatus(POINT_GOLD));
}
#else
PyObject * playerGetElk(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_GOLD));
}
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
PyObject * playerGetCheque(PyObject* poSelf, PyObject* poArgs)
{
	return PyLong_FromLongLong(CPythonPlayer::Instance().GetStatus(POINT_CHEQUE));
}
#endif

#ifdef ENABLE_PUNKTY_OSIAGNIEC
PyObject* playerGetPktOsiag(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetStatus(POINT_PKT_OSIAG));
}
#endif

PyObject * playerGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pInstance)
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", pInstance->GetGuildID());
}

PyObject * playerGetGuildName(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (pInstance)
	{
		DWORD dwID = pInstance->GetGuildID();
		std::string strName;
		if (CPythonGuild::Instance().GetGuildName(dwID, &strName))
			return Py_BuildValue("s", strName.c_str());
	}
	return Py_BuildValue("s", "");
}

PyObject * playerGetAlignmentData(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	int iAlignmentPoint = 0;
	int iAlignmentGrade = 4;
	if (pInstance)
	{
		iAlignmentPoint = pInstance->GetAlignment();
		iAlignmentGrade = pInstance->GetAlignmentGrade();
	}
	return Py_BuildValue("ii", iAlignmentPoint, iAlignmentGrade);
}

PyObject * playerSetSkill(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSkillIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetSkill(iSlotIndex, iSkillIndex);
	return Py_BuildNone();
}

PyObject * playerGetSkillIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillIndex(iSlotIndex));
}

PyObject * playerGetSkillSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
		return Py_BuildException();

	DWORD dwSlotIndex;
	if (!CPythonPlayer::Instance().GetSkillSlotIndex(iSkillIndex, &dwSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", dwSlotIndex);
}

PyObject * playerGetSkillGrade(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillGrade(iSlotIndex));
}

PyObject * playerGetSkillLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().GetSkillLevel(iSlotIndex));
}

PyObject * playerGetSkillCurrentEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillCurrentEfficientPercentage(iSlotIndex));
}
PyObject * playerGetSkillNextEfficientPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("f", CPythonPlayer::Instance().GetSkillNextEfficientPercentage(iSlotIndex));
}

PyObject * playerClickSkillSlot(PyObject * poSelf, PyObject * poArgs)
{
	int iSkillSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillSlot))
		return Py_BadArgument();

	CPythonPlayer::Instance().ClickSkillSlot(iSkillSlot);

	return Py_BuildNone();
}

PyObject * playerChangeCurrentSkillNumberOnly(PyObject * poSelf, PyObject * poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BadArgument();

	CPythonPlayer::Instance().ChangeCurrentSkillNumberOnly(iSlotIndex);

	return Py_BuildNone();
}

PyObject * playerClearSkillDict(PyObject * poSelf, PyObject * poArgs)
{
	CPythonPlayer::Instance().ClearSkillDict();
	return Py_BuildNone();
}

PyObject * playerMoveItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos srcCell;
	TItemPos dstCell;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		// int iSourceSlotIndex;
		if (!PyTuple_GetInteger(poArgs, 0, &srcCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &dstCell.cell))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetByte(poArgs, 0, &srcCell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &srcCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetByte(poArgs, 2, &dstCell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &dstCell.cell))
			return Py_BuildException();
	default:
		return Py_BuildException();
	}

	CPythonPlayer::Instance().MoveItemData(srcCell, dstCell);
	return Py_BuildNone();
}

PyObject * playerSendClickItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int ivid;
	if (!PyTuple_GetInteger(poArgs, 0, &ivid))
		return Py_BuildException();

	CPythonPlayer::Instance().SendClickItemPacket(ivid);
	return Py_BuildNone();
}

PyObject * playerGetItemIndex(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos (INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemIndex);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemIndex = CPythonPlayer::Instance().GetItemIndex(Cell);
			return Py_BuildValue("i", ItemIndex);
		}
	default:
		return Py_BuildException();

	}
}

PyObject * playerGetItemFlags(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(TItemPos(INVENTORY, iSlotIndex));
			return Py_BuildValue("i", flags);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			DWORD flags = CPythonPlayer::Instance().GetItemFlags(Cell);
			return Py_BuildValue("i", flags);
		}
	default:
		return Py_BuildException();
	}
}

PyObject * playerGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(TItemPos (INVENTORY, iSlotIndex));
			return Py_BuildValue("i", ItemNum);
		}
	case 2:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

			int ItemNum = CPythonPlayer::Instance().GetItemCount(Cell);

			return Py_BuildValue("i", ItemNum);
		}
	default:
		return Py_BuildException();

	}
}

PyObject * playerSetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		{
			int iSlotIndex;
			if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
				return Py_BuildException();

#ifdef ENABLE_EXTENDED_ITEM_COUNT
			int bCount;
#else
			BYTE bCount;
#endif
			if (!PyTuple_GetInteger(poArgs, 1, &bCount))
				return Py_BuildException();

			if (0 == bCount)
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(TItemPos (INVENTORY, iSlotIndex), bCount);
			return Py_BuildNone();
		}
	case 3:
		{
			TItemPos Cell;
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();

#ifdef ENABLE_EXTENDED_ITEM_COUNT
			int bCount;
#else
			BYTE bCount;
#endif
			if (!PyTuple_GetInteger(poArgs, 2, &bCount))
				return Py_BuildException();

			CPythonPlayer::Instance().SetItemCount(Cell, bCount);

			return Py_BuildNone();
		}
	default:
		return Py_BuildException();

	}
}

PyObject * playerGetItemCountByVnum(PyObject* poSelf, PyObject* poArgs)
{
	int ivnum;
	if (!PyTuple_GetInteger(poArgs, 0, &ivnum))
		return Py_BuildException();

	int ItemNum = CPythonPlayer::Instance().GetItemCountByVnum(ivnum);
	return Py_BuildValue("i", ItemNum);
}

PyObject * playerGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int iMetinSocketIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketIndex))
			return Py_BuildException();

		break;

	default:
		return Py_BuildException();

	}
	int nMetinSocketValue = CPythonPlayer::Instance().GetItemMetinSocket(Cell, iMetinSocketIndex);
	return Py_BuildValue("i", nMetinSocketValue);
}

PyObject * playerGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	// int iSlotPos;
	int iAttributeSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	BYTE byType;
	short sValue;
	CPythonPlayer::Instance().GetItemAttribute(Cell, iAttributeSlotIndex, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject * playerGetItemLink(PyObject * poSelf, PyObject * poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	const TItemData * pPlayerItem = CPythonPlayer::Instance().GetItemData(Cell);
	CItemData * pItemData = NULL;
	char buf[1024];

	if (pPlayerItem && CItemManager::Instance().GetItemDataPointer(pPlayerItem->vnum, &pItemData))
	{
		char itemlink[256];
		int len;
		bool isAttr = false;

		len = _snprintf(itemlink, sizeof(itemlink), "item:%x:%x:%x:%x:%x:%x:%x:%x",
				pPlayerItem->vnum, pPlayerItem->flags,
				pPlayerItem->alSockets[0], pPlayerItem->alSockets[1], pPlayerItem->alSockets[2], pPlayerItem->alSockets[3], pPlayerItem->alSockets[4], pPlayerItem->alSockets[5]);

		for (int i = 0; i < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++i)
		{
			// if (pPlayerItem->aAttr[i].bType != 0) // @fixme009 (this line must be commented)
			{
				len += _snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d",
						pPlayerItem->aAttr[i].bType, pPlayerItem->aAttr[i].sValue);
				isAttr = true;
			}
		}

		if( GetDefaultCodePage() == CP_ARABIC ) {
			if (isAttr)
				//"item:ąřČŘ:ÇĂ·ˇ±×:ĽŇÄĎ0:ĽŇÄĎ1:ĽŇÄĎ2"
				_snprintf(buf, sizeof(buf), " |h|r[%s]|cffffc700|H%s|h", pItemData->GetName(), itemlink);
			else
				_snprintf(buf, sizeof(buf), " |h|r[%s]|cfff1e6c0|H%s|h", pItemData->GetName(), itemlink);
		} else {
			if (isAttr)
				//"item:ąřČŘ:ÇĂ·ˇ±×:ĽŇÄĎ0:ĽŇÄĎ1:ĽŇÄĎ2"
				_snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, pItemData->GetName());
			else
				_snprintf(buf, sizeof(buf), "|cfff1e6c0|H%s|h[%s]|h|r", itemlink, pItemData->GetName());
		}
	}
	else
		buf[0] = '\0';

	return Py_BuildValue("s", buf);
}

PyObject * playerGetISellItemPrice(PyObject * poSelf, PyObject * poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CItemData * pItemData;

	if (!CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(Cell), &pItemData))
		return Py_BuildValue("i", 0);

	int iPrice;

	if (pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD))
		iPrice = CPythonPlayer::Instance().GetItemCount(Cell) / pItemData->GetISellItemPrice();
	else
		iPrice = pItemData->GetISellItemPrice() * CPythonPlayer::Instance().GetItemCount(Cell);

	iPrice /= 2;
	return Py_BuildValue("i", iPrice);
}

PyObject * playerGetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetQuickPage());
}

PyObject * playerSetQuickPage(PyObject* poSelf, PyObject* poArgs)
{
	int iPageIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iPageIndex))
		return Py_BuildException();

	CPythonPlayer::Instance().SetQuickPage(iPageIndex);
	return Py_BuildNone();
}

PyObject * playerLocalQuickSlotIndexToGlobalQuickSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iLocalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	return Py_BuildValue("i", rkPlayer.LocalQuickSlotIndexToGlobalQuickSlotIndex(iLocalSlotIndex));
}

PyObject * playerGetLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	DWORD dwWndType;
	DWORD dwWndItemPos;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.GetLocalQuickSlotData(iSlotIndex, &dwWndType, &dwWndItemPos);

	return Py_BuildValue("ii", dwWndType, dwWndItemPos);
}

PyObject * playerGetGlobalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	DWORD dwWndType;
	DWORD dwWndItemPos;

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.GetGlobalQuickSlotData(iSlotIndex, &dwWndType, &dwWndItemPos);

	return Py_BuildValue("ii", dwWndType, dwWndItemPos);
}

PyObject * playerRequestAddLocalQuickSlot(PyObject * poSelf, PyObject * poArgs)
{
	int nSlotIndex;
	int nWndType;
	int nWndItemPos;

	if (!PyTuple_GetInteger(poArgs, 0, &nSlotIndex))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &nWndType))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &nWndItemPos))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestAddLocalQuickSlot(nSlotIndex, nWndType, nWndItemPos);

	return Py_BuildNone();
}

PyObject * playerRequestAddToEmptyLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int nWndType;
	if (!PyTuple_GetInteger(poArgs, 0, &nWndType))
		return Py_BuildException();

	int nWndItemPos;
	if (!PyTuple_GetInteger(poArgs, 1, &nWndItemPos))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestAddToEmptyLocalQuickSlot(nWndType, nWndItemPos);

	return Py_BuildNone();
}

PyObject * playerRequestDeleteGlobalQuickSlot(PyObject * poSelf, PyObject * poArgs)
{
	int nGlobalSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestDeleteGlobalQuickSlot(nGlobalSlotIndex);
	return Py_BuildNone();
}

PyObject * playerRequestMoveGlobalQuickSlotToLocalQuickSlot(PyObject * poSelf, PyObject * poArgs)
{
	int nGlobalSrcSlotIndex;
	int nLocalDstSlotIndex;

	if (!PyTuple_GetInteger(poArgs, 0, &nGlobalSrcSlotIndex))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &nLocalDstSlotIndex))
		return Py_BuildException();

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.RequestMoveGlobalQuickSlotToLocalQuickSlot(nGlobalSrcSlotIndex, nLocalDstSlotIndex);
	return Py_BuildNone();
}

PyObject * playerRequestUseLocalQuickSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iLocalPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iLocalPosition))
		return Py_BuildException();

	CPythonPlayer::Instance().RequestUseLocalQuickSlot(iLocalPosition);

	return Py_BuildNone();
}

PyObject * playerRemoveQuickSlotByValue(PyObject* poSelf, PyObject* poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
		return Py_BuildException();

	int iPosition;
	if (!PyTuple_GetInteger(poArgs, 1, &iPosition))
		return Py_BuildException();

	CPythonPlayer::Instance().RemoveQuickSlotByValue(iType, iPosition);

	return Py_BuildNone();
}

PyObject * playerisItem(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsItem(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
PyObject * playerIsBeltInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsBeltInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}
#endif

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
PyObject * playerIsSkillBookInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsSkillBookInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

PyObject * playerIsUpgradeItemsInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsUpgradeItemsInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

PyObject * playerIsStoneInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsStoneInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

PyObject * playerIsBoxInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsBoxInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

PyObject * playerIsEfsunInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsEfsunInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}

PyObject * playerIsCicekInventorySlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	char Flag = CPythonPlayer::Instance().IsCicekInventorySlot(TItemPos(INVENTORY, iSlotIndex));

	return Py_BuildValue("i", Flag);
}
#endif

PyObject * playerIsEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	if (iSlotIndex >= c_Equipment_Start)
	if (iSlotIndex <= c_DragonSoul_Equip_End)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject * playerIsDSEquipmentSlot(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
		return Py_BuildException();

	if (INVENTORY == bWindowType)
	if (iSlotIndex >= c_DragonSoul_Equip_Start)
	if (iSlotIndex <= c_DragonSoul_Equip_End)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

PyObject * playerIsCostumeSlot(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

#ifdef ENABLE_COSTUME_SYSTEM
	if (iSlotIndex >= c_Costume_Slot_Start)
	if (iSlotIndex <= c_Costume_Slot_End)
		return Py_BuildValue("i", 1);
#endif

	return Py_BuildValue("i", 0);
}

PyObject * playerIsOpenPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().IsOpenPrivateShop());
}

PyObject * playerIsValuableItem(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;

	switch (PyTuple_Size (poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell)) // @fixme013 arg idx from 0 to 1
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}

	DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	BOOL hasMetinSocket = FALSE;
	BOOL isHighPrice = FALSE;

	for (int i = 0; i < METIN_SOCKET_COUNT; ++i)
		if (CPythonPlayer::METIN_SOCKET_TYPE_NONE != CPythonPlayer::Instance().GetItemMetinSocket(SlotIndex, i))
			hasMetinSocket = TRUE;

	DWORD dwValue = pItemData->GetISellItemPrice();
	if (dwValue > 5000)
		isHighPrice = TRUE;

	return Py_BuildValue("i", hasMetinSocket || isHighPrice);
}

int GetItemGrade(const char * c_szItemName)
{
	std::string strName = c_szItemName;
	if (strName.empty())
		return 0;

	char chGrade = strName[strName.length() - 1];
	if (chGrade < '0' || chGrade > '9')
		chGrade = '0';

	int iGrade = chGrade - '0';
	return iGrade;
}

PyObject * playerGetItemGrade(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &SlotIndex.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}

	int iItemIndex = CPythonPlayer::Instance().GetItemIndex(SlotIndex);
	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", GetItemGrade(pItemData->GetName()));
}

#if defined(GAIDEN)
PyObject * playerGetItemUnbindTime(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	return Py_BuildValue("i", (int) CPythonPlayer::instance().GetItemUnbindTime(iSlotIndex));
}
#endif

enum
{
	REFINE_SCROLL_TYPE_MAKE_SOCKET = 1,
	REFINE_SCROLL_TYPE_UP_GRADE = 2,
};

enum
{
	REFINE_CANT,
	REFINE_OK,
	REFINE_ALREADY_MAX_SOCKET_COUNT,
	REFINE_NEED_MORE_GOOD_SCROLL,
	REFINE_CANT_MAKE_SOCKET_ITEM,
	REFINE_NOT_NEXT_GRADE_ITEM,
	REFINE_CANT_REFINE_METIN_TO_EQUIPMENT,
	REFINE_CANT_REFINE_ROD,
};

PyObject * playerCanRefine(PyObject * poSelf, PyObject * poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
	{
		return Py_BuildValue("i", REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildValue("i", REFINE_CANT);
	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();
	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", REFINE_CANT);
	if (iScrollSubType != CItemData::USE_TUNING)
		return Py_BuildValue("i", REFINE_CANT);

	// Target Item
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);

	CItemData* pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildValue("i", REFINE_CANT);

	int iTargetType = pTargetItemData->GetType();
	int iTargetSubType = pTargetItemData->GetSubType();

	if (CItemData::ITEM_TYPE_ROD == iTargetType)
		return Py_BuildValue("i", REFINE_CANT_REFINE_ROD);
	else if (CItemData::ITEM_TYPE_COSTUME == iTargetType && CItemData::COSTUME_ACCE == iTargetSubType)
	{
		int absorbedSocket = CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, ACCE_ABSORBED_SOCKET);

		CItemManager::Instance().SelectItemData(absorbedSocket);
		CItemData* pAcceRefineItemData = CItemManager::Instance().GetSelectedItemDataPointer();

		if (pAcceRefineItemData->HasNextGrade())
			return Py_BuildValue("i", REFINE_OK);
		else
			return Py_BuildValue("i", REFINE_NOT_NEXT_GRADE_ITEM);
	}
	else
	{
		if (pTargetItemData->HasNextGrade())
			return Py_BuildValue("i", REFINE_OK);
		else
			return Py_BuildValue("i", REFINE_NOT_NEXT_GRADE_ITEM);
	}
#else
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildValue("i", REFINE_CANT);
	int iTargetType = pTargetItemData->GetType();
	//int iTargetSubType = pTargetItemData->GetSubType();
	if (CItemData::ITEM_TYPE_ROD == iTargetType)
		return Py_BuildValue("i", REFINE_CANT_REFINE_ROD);

	if (pTargetItemData->HasNextGrade())
	{
		return Py_BuildValue("i", REFINE_OK);
	}
	else
	{
		return Py_BuildValue("i", REFINE_NOT_NEXT_GRADE_ITEM);
	}
#endif

	return Py_BuildValue("i", REFINE_CANT);
}

enum
{
	ATTACH_METIN_CANT,
	ATTACH_METIN_OK,
	ATTACH_METIN_NOT_MATCHABLE_ITEM,
	ATTACH_METIN_NO_MATCHABLE_SOCKET,
	ATTACH_METIN_NOT_EXIST_GOLD_SOCKET,
	ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT,
};

PyObject * playerCanAttachMetin(PyObject* poSelf, PyObject* poArgs)
{
	int iMetinItemID;
	TItemPos TargetSlotIndex;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iMetinItemID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	if (CPythonPlayer::Instance().IsEquipmentSlot(TargetSlotIndex))
	{
		return Py_BuildValue("i", ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);
	}

	CItemData * pMetinItemData;
	if (!CItemManager::Instance().GetItemDataPointer(iMetinItemID, &pMetinItemData))
		return Py_BuildException("can't find item data");

	DWORD dwTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemData * pTargetItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwTargetItemIndex, &pTargetItemData))
		return Py_BuildException("can't find item data");

	DWORD dwMetinWearFlags = pMetinItemData->GetWearFlags();
	DWORD dwTargetWearFlags = pTargetItemData->GetWearFlags();
	if (0 == (dwMetinWearFlags & dwTargetWearFlags))
		return Py_BuildValue("i", ATTACH_METIN_NOT_MATCHABLE_ITEM);
	if (CItemData::ITEM_TYPE_ROD == pTargetItemData->GetType())
		return Py_BuildValue("i", ATTACH_METIN_CANT);

	BOOL bNotExistGoldSocket = FALSE;

	int iSubType = pMetinItemData->GetSubType();
	for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
	{
		DWORD dwSocketType = CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, i);
		if (CItemData::METIN_NORMAL == iSubType)
		{
			if (CPythonPlayer::METIN_SOCKET_TYPE_SILVER == dwSocketType ||
				CPythonPlayer::METIN_SOCKET_TYPE_GOLD == dwSocketType)
			{
				return Py_BuildValue("i", ATTACH_METIN_OK);
			}
		}
		else if (CItemData::METIN_GOLD == iSubType)
		{
			if (CPythonPlayer::METIN_SOCKET_TYPE_GOLD == dwSocketType)
			{
				return Py_BuildValue("i", ATTACH_METIN_OK);
			}
			else if (CPythonPlayer::METIN_SOCKET_TYPE_SILVER == dwSocketType)
			{
				bNotExistGoldSocket = TRUE;
			}
		}
	}

	if (bNotExistGoldSocket)
	{
		return Py_BuildValue("i", ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);
	}

	return Py_BuildValue("i", ATTACH_METIN_NO_MATCHABLE_SOCKET);
}

enum
{
	DETACH_METIN_CANT,
	DETACH_METIN_OK,
};

PyObject * playerCanDetach(PyObject * poSelf, PyObject * poArgs)
{
	int iScrollItemIndex;
	TItemPos TargetSlotIndex;
	switch (PyTuple_Size (poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iScrollItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	// Scroll
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	int iScrollType = pScrollItemData->GetType();
	int iScrollSubType = pScrollItemData->GetSubType();
	if (iScrollType != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", DETACH_METIN_CANT);
	if (iScrollSubType != CItemData::USE_DETACHMENT)
		return Py_BuildValue("i", DETACH_METIN_CANT);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (pScrollItemData->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0
		|| pScrollItemData->GetIndex() == ACCE_REVERSAL_VNUM_1
		|| pScrollItemData->GetIndex() == ACCE_REVERSAL_VNUM_2
	)
	{
		if ((pTargetItemData->GetType() != CItemData::ITEM_TYPE_COSTUME) || (pTargetItemData->GetSubType() != CItemData::COSTUME_ACCE))
			return Py_BuildValue("i", DETACH_METIN_CANT);

		const TItemData * pPlayerItem = CPythonPlayer::Instance().GetItemData(TargetSlotIndex);
		if (pPlayerItem)
		{
			if (pPlayerItem->alSockets[ACCE_ABSORBED_SOCKET] > 0)
				return Py_BuildValue("i", DETACH_METIN_OK);
			else
				return Py_BuildValue("i", DETACH_METIN_CANT);
		}
		else
			return Py_BuildValue("i", DETACH_METIN_CANT);
	}
#endif

	//int iTargetType = pTargetItemData->GetType();
	//int iTargetSubType = pTargetItemData->GetSubType();

	if (pTargetItemData->IsFlag(CItemData::ITEM_FLAG_REFINEABLE))
	{
		for (int iSlotCount = 0; iSlotCount < METIN_SOCKET_COUNT; ++iSlotCount)
			if (CPythonPlayer::Instance().GetItemMetinSocket(TargetSlotIndex, iSlotCount) > 2)
			{
				return Py_BuildValue("i", DETACH_METIN_OK);
			}
	}

	return Py_BuildValue("i", DETACH_METIN_CANT);
}

PyObject * playerCanUnlock(PyObject * poSelf, PyObject * poArgs)
{
	int iKeyItemIndex;
	TItemPos TargetSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iKeyItemIndex))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 1, &TargetSlotIndex.window_type))
			return Py_BadArgument();
		if (!PyTuple_GetInteger(poArgs, 2, &TargetSlotIndex.cell))
			return Py_BadArgument();
		break;
	default:
		return Py_BadArgument();
	}

	// Key
	CItemManager::Instance().SelectItemData(iKeyItemIndex);
	CItemData * pKeyItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pKeyItemData)
		return Py_BuildException("Can't find item data");
	int iKeyType = pKeyItemData->GetType();
	if (iKeyType != CItemData::ITEM_TYPE_TREASURE_KEY)
		return Py_BuildValue("i", FALSE);

	// Target Item
	int iTargetItemIndex = CPythonPlayer::Instance().GetItemIndex(TargetSlotIndex);
	CItemManager::Instance().SelectItemData(iTargetItemIndex);
	CItemData * pTargetItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pTargetItemData)
		return Py_BuildException("Can't find item data");
	int iTargetType = pTargetItemData->GetType();
	if (iTargetType != CItemData::ITEM_TYPE_TREASURE_BOX)
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", TRUE);
}

PyObject * playerIsRefineGradeScroll(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ScrollSlotIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.cell))
			return Py_BuildException();
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &ScrollSlotIndex.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ScrollSlotIndex.cell))
			return Py_BuildException();
	default:
		return Py_BuildException();
	}

	int iScrollItemIndex = CPythonPlayer::Instance().GetItemIndex(ScrollSlotIndex);
	CItemManager::Instance().SelectItemData(iScrollItemIndex);
	CItemData * pScrollItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pScrollItemData)
		return Py_BuildException("Can't find item data");

	return Py_BuildValue("i", REFINE_SCROLL_TYPE_UP_GRADE == pScrollItemData->GetValue(0));
}

PyObject * playerUpdate(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Update();
	return Py_BuildNone();
}

PyObject * playerRender(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildNone();
}

PyObject * playerClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().Clear();
	return Py_BuildNone();
}

PyObject * playerClearTarget(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().SetTarget(0);
	return Py_BuildNone();
}

PyObject * playerSetTarget(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().SetTarget(iVID);
	return Py_BuildNone();
}

PyObject * playerOpenCharacterMenu(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonPlayer::Instance().OpenCharacterMenu(iVID);
	return Py_BuildNone();
}

PyObject * playerIsPartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonPlayer::Instance().IsPartyMemberByVID(iVID));
}

PyObject * playerIsPartyLeader(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	DWORD dwPID;
	if (!CPythonPlayer::Instance().PartyMemberVIDToPID(iVID, &dwPID))
		return Py_BuildValue("i", FALSE);

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject * playerIsPartyLeaderByPID(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", CPythonPlayer::PARTY_ROLE_LEADER == pPartyMemberInfo->byState);
}

PyObject * playerGetPartyMemberHPPercentage(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byHPPercentage);
}

PyObject * playerGetPartyMemberState(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("i", pPartyMemberInfo->byState);
}

PyObject * playerGetPartyMemberAffects(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(iPID, &pPartyMemberInfo))
		return Py_BuildValue("i", FALSE);

	return Py_BuildValue("iiiiiii",	pPartyMemberInfo->sAffects[0],
									pPartyMemberInfo->sAffects[1],
									pPartyMemberInfo->sAffects[2],
									pPartyMemberInfo->sAffects[3],
									pPartyMemberInfo->sAffects[4],
									pPartyMemberInfo->sAffects[5],
									pPartyMemberInfo->sAffects[6]);
}

PyObject * playerRemovePartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();

	CPythonPlayer::Instance().RemovePartyMember(iPID);
	return Py_BuildNone();
}

PyObject * playerExitParty(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().ExitParty();
	return Py_BuildNone();
}

PyObject * playerGetPKMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetPKMode());
}

PyObject * playerHasMobilePhoneNumber(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().HasMobilePhoneNumber());
}

PyObject * playerSetWeaponAttackBonusFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject * playerToggleCoolTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleCoolTime());
}

PyObject * playerToggleLevelLimit(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().__ToggleLevelLimit());
}

PyObject * playerGetTargetVID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPlayer::Instance().GetTargetVID());
}

PyObject * playerRegisterEmotionIcon(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BuildException();

	CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szFileName);
	m_kMap_iEmotionIndex_pkIconImage.emplace(iIndex, pImage);

	return Py_BuildNone();
}

PyObject * playerGetEmotionIconImage(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	if (m_kMap_iEmotionIndex_pkIconImage.end() == m_kMap_iEmotionIndex_pkIconImage.find(iIndex))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", m_kMap_iEmotionIndex_pkIconImage[iIndex]);
}

PyObject * playerSetItemData(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualID))
		return Py_BuildException();

	int iNum;
	if (!PyTuple_GetInteger(poArgs, 2, &iNum))
		return Py_BuildException();

	TItemData kItemInst;
	ZeroMemory(&kItemInst, sizeof(kItemInst));
	kItemInst.vnum=iVirtualID;
	kItemInst.count=iNum;
	CPythonPlayer::Instance().SetItemData(TItemPos(INVENTORY, iSlotIndex), kItemInst);
	return Py_BuildNone();
}

PyObject * playerSetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iMetinSocketNumber;
	int iNum;

	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketNumber))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iNum))
			return Py_BuildException();

		break;
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iMetinSocketNumber))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &iNum))
			return Py_BuildException();

		break;
	default:
		return Py_BuildException();
	}

	CPythonPlayer::Instance().SetItemMetinSocket(ItemPos, iMetinSocketNumber, iNum);
	return Py_BuildNone();
}

PyObject * playerSetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos ItemPos;
	int iAttributeSlotIndex;
	int iAttributeType;
	int iAttributeValue;

	switch (PyTuple_Size(poArgs))
	{
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &iAttributeSlotIndex))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeType))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &iAttributeValue))
			return Py_BuildException();
		break;
	case 5:
		if (!PyTuple_GetInteger(poArgs, 0, &ItemPos.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &ItemPos.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 2, &iAttributeSlotIndex))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &iAttributeType))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 4, &iAttributeValue))
			return Py_BuildException();
		break;
	}
	CPythonPlayer::Instance().SetItemAttribute(ItemPos, iAttributeSlotIndex, iAttributeType, iAttributeValue);
	return Py_BuildNone();
}

PyObject * playerSetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	int potionType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &potionType))
		return Py_BadArgument();

	CPythonPlayer* player = CPythonPlayer::InstancePtr();

	CPythonPlayer::SAutoPotionInfo& potionInfo = player->GetAutoPotionInfo(potionType);

	if (!PyTuple_GetBoolean(poArgs, 1, &potionInfo.bActivated))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 2, &potionInfo.currentAmount))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 3, &potionInfo.totalAmount))
		return Py_BadArgument();

	if (!PyTuple_GetLong(poArgs, 4, &potionInfo.inventorySlotIndex))
		return Py_BadArgument();

	return Py_BuildNone();
}

PyObject * playerGetAutoPotionInfo(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer* player = CPythonPlayer::InstancePtr();

	int potionType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &potionType))
		return Py_BadArgument();

	CPythonPlayer::SAutoPotionInfo& potionInfo = player->GetAutoPotionInfo(potionType);

	return Py_BuildValue("biii", potionInfo.bActivated, int(potionInfo.currentAmount), int(potionInfo.totalAmount), int(potionInfo.inventorySlotIndex));
}

PyObject * playerSlotTypeToInvenType(PyObject* poSelf, PyObject* poArgs)
{
	int slotType = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &slotType))
		return Py_BadArgument();

	return Py_BuildValue("i", SlotTypeToInvenType((BYTE)slotType));
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
PyObject * playerIsEquippingBelt(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	bool bEquipping = false;

	const TItemData* data = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Belt));

	if (NULL != data)
		bEquipping = 0 < data->count;

	return Py_BuildValue("b", bEquipping);
}

PyObject * playerIsAvailableBeltInventoryCell(PyObject* poSelf, PyObject* poArgs)
{
	const CPythonPlayer* player = CPythonPlayer::InstancePtr();
	const TItemData* pData = player->GetItemData(TItemPos(EQUIPMENT, c_Equipment_Belt));

	if (NULL == pData || 0 == pData->count)
		return Py_BuildValue("b", false);

	CItemManager::Instance().SelectItemData(pData->vnum);
	CItemData * pItem = CItemManager::Instance().GetSelectedItemDataPointer();

	long beltGrade = pItem->GetValue(0);

	int pos = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BadArgument();

	//return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, GetItemGrade(pItem->GetName())));
	return Py_BuildValue("b", CBeltInventoryHelper::IsAvailableCell(pos - c_Belt_Inventory_Slot_Start, beltGrade));
}
#endif

PyObject* playerSendDragonSoulRefine(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bSubHeader;
	PyObject* pDic;
	TItemPos RefineItemPoses[DS_REFINE_WINDOW_MAX_NUM];
	if (!PyTuple_GetByte(poArgs, 0, &bSubHeader))
		return Py_BuildException();
	switch (bSubHeader)
	{
	case DS_SUB_HEADER_CLOSE:
		break;
	case DS_SUB_HEADER_DO_UPGRADE:
	case DS_SUB_HEADER_DO_IMPROVEMENT:
	case DS_SUB_HEADER_DO_REFINE:
		{
			if (!PyTuple_GetObject(poArgs, 1, &pDic))
				return Py_BuildException();
			int pos = 0;
			PyObject* key, *value;
			int size = PyDict_Size(pDic);

			while (PyDict_Next(pDic, &pos, &key, &value))
			{
				int i = PyInt_AsLong(key);
				if (i > DS_REFINE_WINDOW_MAX_NUM)
					return Py_BuildException();

				if (!PyTuple_GetByte(value, 0, &RefineItemPoses[i].window_type)
					|| !PyTuple_GetInteger(value, 1, &RefineItemPoses[i].cell))
				{
					return Py_BuildException();
				}
			}
		}
		break;
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendDragonSoulRefinePacket(bSubHeader, RefineItemPoses);

	return Py_BuildNone();
}

PyObject * playerGetItemFirstIndexByVnum(PyObject* poSelf, PyObject* poArgs)
{
	int ivnum;
	if (!PyTuple_GetInteger(poArgs, 0, &ivnum))
		return Py_BuildException();
 
	int itemIndex = CPythonPlayer::Instance().GetItemFirstIndexByVnum(ivnum);
	// TraceError("playerGetItemFirstIndexByVnum(%d): %d", ivnum, itemIndex);
	return Py_BuildValue("i", itemIndex);
}

#if defined(ENABLE_KEYCHANGE_SYSTEM)
PyObject* playerOpenKeyChangeWindow(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().OpenKeyChangeWindow();
	return Py_BuildNone();
}

PyObject* playerIsOpenKeySettingWindow(PyObject* poSelf, PyObject* poArgs)
{
	BOOL bSet = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &bSet))
		return Py_BadArgument();

	CPythonPlayer::Instance().IsOpenKeySettingWindow(bSet);
	return Py_BuildNone();
}

PyObject* playerKeySettingClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayer::Instance().KeySettingClear();
	return Py_BuildNone();
}

PyObject* playerKeySetting(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	int iKeyFunction = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &iKeyFunction))
		return Py_BadArgument();

	CPythonPlayer::Instance().KeySetting(iKey, iKeyFunction);
	return Py_BuildNone();
}

PyObject* playerOnKeyDown(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	CPythonPlayer::Instance().OnKeyDown(iKey);
	return Py_BuildNone();
}

PyObject* playerOnKeyUp(PyObject* poSelf, PyObject* poArgs)
{
	int iKey = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iKey))
		return Py_BadArgument();

	CPythonPlayer::Instance().OnKeyUp(iKey);
	return Py_BuildNone();
}
#endif

PyObject* playerGetItemSlotIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BuildException();

	int slotIndex = CPythonPlayer::Instance().GetItemSlotIndex(iVnum);
	return Py_BuildValue("i", slotIndex);
}

void initPlayer()
{
	VMProtectBeginUltra("initPlayer");
	static PyMethodDef s_methods[] =
	{
		{VMProtectDecryptStringA("GetItemSlotIndex"),			playerGetItemSlotIndex,			METH_VARARGS },
		{VMProtectDecryptStringA("GetAutoPotionInfo"),			playerGetAutoPotionInfo,			METH_VARARGS },
		{VMProtectDecryptStringA("SetAutoPotionInfo"),			playerSetAutoPotionInfo,			METH_VARARGS },

		{VMProtectDecryptStringA("PickCloseItem"),				playerPickCloseItem,				METH_VARARGS },

#ifdef FAST_ITEMS_PICKUP
		{VMProtectDecryptStringA("PickCloseItemVector"),		playerPickCloseItemVector,			METH_VARARGS },
#endif
		{VMProtectDecryptStringA("SetGameWindow"),				playerSetGameWindow,				METH_VARARGS },
		{VMProtectDecryptStringA("RegisterEffect"),				playerRegisterEffect,				METH_VARARGS },
		{VMProtectDecryptStringA("RegisterCacheEffect"),		playerRegisterCacheEffect,			METH_VARARGS },
		{VMProtectDecryptStringA("SetMouseState"),				playerSetMouseState,				METH_VARARGS },
		{VMProtectDecryptStringA("SetMouseFunc"),				playerSetMouseFunc,					METH_VARARGS },
		{VMProtectDecryptStringA("GetMouseFunc"),				playerGetMouseFunc,					METH_VARARGS },
		{VMProtectDecryptStringA("SetMouseMiddleButtonState"),	playerSetMouseMiddleButtonState,	METH_VARARGS },
		{VMProtectDecryptStringA("SetMainCharacterIndex"),		playerSetMainCharacterIndex,		METH_VARARGS },
		{VMProtectDecryptStringA("GetMainCharacterIndex"),		playerGetMainCharacterIndex,		METH_VARARGS },
		{VMProtectDecryptStringA("GetMainCharacterName"),		playerGetMainCharacterName,			METH_VARARGS },
		{VMProtectDecryptStringA("GetMainCharacterPosition"),	playerGetMainCharacterPosition,		METH_VARARGS },
		{VMProtectDecryptStringA("IsMainCharacterIndex"),		playerIsMainCharacterIndex,			METH_VARARGS },
		{VMProtectDecryptStringA("CanAttackInstance"),			playerCanAttackInstance,			METH_VARARGS },
		{VMProtectDecryptStringA("IsActingEmotion"),			playerIsActingEmotion,				METH_VARARGS },
		{VMProtectDecryptStringA("IsPVPInstance"),				playerIsPVPInstance,				METH_VARARGS },
		{VMProtectDecryptStringA("IsSameEmpire"),				playerIsSameEmpire,					METH_VARARGS },
		{VMProtectDecryptStringA("IsChallengeInstance"),		playerIsChallengeInstance,			METH_VARARGS },
		{VMProtectDecryptStringA("IsRevengeInstance"),			playerIsRevengeInstance,			METH_VARARGS },
		{VMProtectDecryptStringA("IsCantFightInstance"),		playerIsCantFightInstance,			METH_VARARGS },
		{VMProtectDecryptStringA("GetCharacterDistance"),		playerGetCharacterDistance,			METH_VARARGS },
		{VMProtectDecryptStringA("IsInSafeArea"),				playerIsInSafeArea,					METH_VARARGS },
		{VMProtectDecryptStringA("IsMountingHorse"),			playerIsMountingHorse,				METH_VARARGS },
		{VMProtectDecryptStringA("IsObserverMode"),				playerIsObserverMode,				METH_VARARGS },
		{VMProtectDecryptStringA("ActEmotion"),					playerActEmotion,					METH_VARARGS },

		{VMProtectDecryptStringA("ShowPlayer"),					playerShowPlayer,					METH_VARARGS },
		{VMProtectDecryptStringA("HidePlayer"),					playerHidePlayer,					METH_VARARGS },

		{VMProtectDecryptStringA("ComboAttack"),				playerComboAttack,					METH_VARARGS },

		{VMProtectDecryptStringA("SetAutoCameraRotationSpeed"),	playerSetAutoCameraRotationSpeed,	METH_VARARGS },
		{VMProtectDecryptStringA("SetAttackKeyState"),			playerSetAttackKeyState,			METH_VARARGS },
		{VMProtectDecryptStringA("SetSingleDIKKeyState"),		playerSetSingleDIKKeyState,			METH_VARARGS },
		{VMProtectDecryptStringA("EndKeyWalkingImmediately"),	playerEndKeyWalkingImmediately,		METH_VARARGS },
		{VMProtectDecryptStringA("StartMouseWalking"),			playerStartMouseWalking,			METH_VARARGS },
		{VMProtectDecryptStringA("EndMouseWalking"),			playerEndMouseWalking,				METH_VARARGS },
		{VMProtectDecryptStringA("ResetCameraRotation"),		playerResetCameraRotation,			METH_VARARGS },
		{VMProtectDecryptStringA("SetQuickCameraMode"),			playerSetQuickCameraMode,			METH_VARARGS },

		///////////////////////////////////////////////////////////////////////////////////////////

		{VMProtectDecryptStringA("SetSkill"),							playerSetSkill,								METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillIndex"),						playerGetSkillIndex,						METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillSlotIndex"),					playerGetSkillSlotIndex,					METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillGrade"),						playerGetSkillGrade,						METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillLevel"),						playerGetSkillLevel,						METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillCurrentEfficientPercentage"),	playerGetSkillCurrentEfficientPercentage,	METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillNextEfficientPercentage"),	playerGetSkillNextEfficientPercentage,		METH_VARARGS },
		{VMProtectDecryptStringA("ClickSkillSlot"),						playerClickSkillSlot,						METH_VARARGS },
		{VMProtectDecryptStringA("ChangeCurrentSkillNumberOnly"),		playerChangeCurrentSkillNumberOnly,			METH_VARARGS },
		{VMProtectDecryptStringA("ClearSkillDict"),						playerClearSkillDict,						METH_VARARGS },

		{VMProtectDecryptStringA("GetItemIndex"),						playerGetItemIndex,							METH_VARARGS },
		{VMProtectDecryptStringA("GetItemFlags"),						playerGetItemFlags,							METH_VARARGS },
		{VMProtectDecryptStringA("GetItemCount"),						playerGetItemCount,							METH_VARARGS },
		{VMProtectDecryptStringA("GetItemCountByVnum"),					playerGetItemCountByVnum,					METH_VARARGS },
		{VMProtectDecryptStringA("GetItemMetinSocket"),					playerGetItemMetinSocket,					METH_VARARGS },
		{VMProtectDecryptStringA("GetItemAttribute"),					playerGetItemAttribute,						METH_VARARGS },
#if defined(GAIDEN)
		{VMProtectDecryptStringA("GetItemUnbindTime"),					playerGetItemUnbindTime,					METH_VARARGS },
#endif
		{VMProtectDecryptStringA("GetISellItemPrice"),					playerGetISellItemPrice,					METH_VARARGS },
		{VMProtectDecryptStringA("MoveItem"),							playerMoveItem,								METH_VARARGS },
		{VMProtectDecryptStringA("SendClickItemPacket"),				playerSendClickItemPacket,					METH_VARARGS },

		///////////////////////////////////////////////////////////////////////////////////////////

		{VMProtectDecryptStringA("GetName"),					playerGetName,						METH_VARARGS },
		{VMProtectDecryptStringA("GetJob"),						playerGetJob,						METH_VARARGS },
		{VMProtectDecryptStringA("GetRace"),					playerGetRace,						METH_VARARGS },
		{VMProtectDecryptStringA("GetOriginalRace"),            playerGetOriginalRace,                METH_VARARGS },
		{VMProtectDecryptStringA("GetPlayTime"),				playerGetPlayTime,					METH_VARARGS },
		{VMProtectDecryptStringA("SetPlayTime"),				playerSetPlayTime,					METH_VARARGS },
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		{VMProtectDecryptStringA("GetCurrentOfflineShopMoney"),	playerGetCurrentOfflineShopMoney,	METH_VARARGS },
		{VMProtectDecryptStringA("GetCurrentOfflineShopZarobek"), playerGetCurrentOfflineShopZarobek, METH_VARARGS },
		{VMProtectDecryptStringA("GetCurrentOfflineShopZarobekCheque"), playerGetCurrentOfflineShopZarobekCheque, METH_VARARGS },
		{VMProtectDecryptStringA("GetCurrentOfflineShopStatus"),	playerGetCurrentOfflineShopStatus,	METH_VARARGS },
		{VMProtectDecryptStringA("GetCurrentOfflineShopChequeMoney"),	playerGetCurrentOfflineShopChequeMoney,	METH_VARARGS },
#endif
		{VMProtectDecryptStringA("IsAntiFlagBySlot"),			playerIsAntiFlagBySlot,				METH_VARARGS },
		{VMProtectDecryptStringA("GetItemTypeBySlot"),			playerGetItemTypeBySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("GetItemSubTypeBySlot"),		playerGetItemSubTypeBySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsSameItemVnum"),				playerIsSameItemVnum,				METH_VARARGS },
		{VMProtectDecryptStringA("IsSkillCoolTime"),			playerIsSkillCoolTime,				METH_VARARGS },
		{VMProtectDecryptStringA("GetSkillCoolTime"),			playerGetSkillCoolTime,				METH_VARARGS },
		{VMProtectDecryptStringA("IsSkillActive"),				playerIsSkillActive,				METH_VARARGS },
		{VMProtectDecryptStringA("UseGuildSkill"),				playerUseGuildSkill,				METH_VARARGS },
		{VMProtectDecryptStringA("AffectIndexToSkillIndex"),	playerAffectIndexToSkillIndex,		METH_VARARGS },
		{VMProtectDecryptStringA("GetEXP"),						playerGetEXP,						METH_VARARGS },
		{VMProtectDecryptStringA("GetLevel"),					playerGetLevel,						METH_VARARGS },
		{VMProtectDecryptStringA("GetStatus"),					playerGetStatus,					METH_VARARGS },
		{VMProtectDecryptStringA("SetStatus"),					playerSetStatus,					METH_VARARGS },
		{VMProtectDecryptStringA("GetElk"),						playerGetElk,						METH_VARARGS },
		{VMProtectDecryptStringA("GetMoney"),					playerGetElk,						METH_VARARGS },
#ifdef ENABLE_CHEQUE_SYSTEM
		{VMProtectDecryptStringA("GetCheque"),					playerGetCheque,					METH_VARARGS },
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
		{VMProtectDecryptStringA("GetPktOsiag"),				playerGetPktOsiag,					METH_VARARGS },
#endif
		{VMProtectDecryptStringA("GetGuildID"),					playerGetGuildID,					METH_VARARGS },
		{VMProtectDecryptStringA("GetGuildName"),				playerGetGuildName,					METH_VARARGS },
		{VMProtectDecryptStringA("GetAlignmentData"),			playerGetAlignmentData,				METH_VARARGS },
		{VMProtectDecryptStringA("RequestAddLocalQuickSlot"),					playerRequestAddLocalQuickSlot,						METH_VARARGS },
		{VMProtectDecryptStringA("RequestAddToEmptyLocalQuickSlot"),			playerRequestAddToEmptyLocalQuickSlot,				METH_VARARGS },
		{VMProtectDecryptStringA("RequestDeleteGlobalQuickSlot"),				playerRequestDeleteGlobalQuickSlot,					METH_VARARGS },
		{VMProtectDecryptStringA("RequestMoveGlobalQuickSlotToLocalQuickSlot"),	playerRequestMoveGlobalQuickSlotToLocalQuickSlot,	METH_VARARGS },
		{VMProtectDecryptStringA("RequestUseLocalQuickSlot"),					playerRequestUseLocalQuickSlot,						METH_VARARGS },
		{VMProtectDecryptStringA("LocalQuickSlotIndexToGlobalQuickSlotIndex"),	playerLocalQuickSlotIndexToGlobalQuickSlotIndex,	METH_VARARGS },

		{VMProtectDecryptStringA("GetQuickPage"),				playerGetQuickPage,					METH_VARARGS },
		{VMProtectDecryptStringA("SetQuickPage"),				playerSetQuickPage,					METH_VARARGS },
		{VMProtectDecryptStringA("GetLocalQuickSlot"),			playerGetLocalQuickSlot,			METH_VARARGS },
		{VMProtectDecryptStringA("GetGlobalQuickSlot"),			playerGetGlobalQuickSlot,			METH_VARARGS },
		{VMProtectDecryptStringA("RemoveQuickSlotByValue"),		playerRemoveQuickSlotByValue,		METH_VARARGS },

		{VMProtectDecryptStringA("isItem"),						playerisItem,						METH_VARARGS },
		{VMProtectDecryptStringA("IsEquipmentSlot"),			playerIsEquipmentSlot,				METH_VARARGS },
		{VMProtectDecryptStringA("IsDSEquipmentSlot"),			playerIsDSEquipmentSlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsCostumeSlot"),				playerIsCostumeSlot,				METH_VARARGS },
		{VMProtectDecryptStringA("IsValuableItem"),				playerIsValuableItem,				METH_VARARGS },
		{VMProtectDecryptStringA("IsOpenPrivateShop"),			playerIsOpenPrivateShop,			METH_VARARGS },

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
		{VMProtectDecryptStringA("IsBeltInventorySlot"),			playerIsBeltInventorySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsEquippingBelt"),				playerIsEquippingBelt,				METH_VARARGS },
		{VMProtectDecryptStringA("IsAvailableBeltInventoryCell"),	playerIsAvailableBeltInventoryCell,	METH_VARARGS },
#endif

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
		{VMProtectDecryptStringA("IsSkillBookInventorySlot"),		playerIsSkillBookInventorySlot,		METH_VARARGS },
		{VMProtectDecryptStringA("IsUpgradeItemsInventorySlot"),	playerIsUpgradeItemsInventorySlot,	METH_VARARGS },
		{VMProtectDecryptStringA("IsStoneInventorySlot"),			playerIsStoneInventorySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsBoxInventorySlot"),				playerIsBoxInventorySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsEfsunInventorySlot"),			playerIsEfsunInventorySlot,			METH_VARARGS },
		{VMProtectDecryptStringA("IsCicekInventorySlot"),			playerIsCicekInventorySlot,			METH_VARARGS },
#endif

		// Refine
		{VMProtectDecryptStringA("GetItemGrade"),				playerGetItemGrade,					METH_VARARGS },
		{VMProtectDecryptStringA("CanRefine"),					playerCanRefine,					METH_VARARGS },
		{VMProtectDecryptStringA("CanDetach"),					playerCanDetach,					METH_VARARGS },
		{VMProtectDecryptStringA("CanUnlock"),					playerCanUnlock,					METH_VARARGS },
		{VMProtectDecryptStringA("CanAttachMetin"),				playerCanAttachMetin,				METH_VARARGS },
		{VMProtectDecryptStringA("IsRefineGradeScroll"),		playerIsRefineGradeScroll,			METH_VARARGS },

		{VMProtectDecryptStringA("ClearTarget"),				playerClearTarget,					METH_VARARGS },
		{VMProtectDecryptStringA("SetTarget"),					playerSetTarget,					METH_VARARGS },
		{VMProtectDecryptStringA("OpenCharacterMenu"),			playerOpenCharacterMenu,			METH_VARARGS },

		{VMProtectDecryptStringA("Update"),						playerUpdate,						METH_VARARGS },
		{VMProtectDecryptStringA("Render"),						playerRender,						METH_VARARGS },
		{VMProtectDecryptStringA("Clear"),						playerClear,						METH_VARARGS },

		// Party
		{VMProtectDecryptStringA("IsPartyMember"),				playerIsPartyMember,				METH_VARARGS },
		{VMProtectDecryptStringA("IsPartyLeader"),				playerIsPartyLeader,				METH_VARARGS },
		{VMProtectDecryptStringA("IsPartyLeaderByPID"),			playerIsPartyLeaderByPID,			METH_VARARGS },
		{VMProtectDecryptStringA("GetPartyMemberHPPercentage"),	playerGetPartyMemberHPPercentage,	METH_VARARGS },
		{VMProtectDecryptStringA("GetPartyMemberState"),		playerGetPartyMemberState,			METH_VARARGS },
		{VMProtectDecryptStringA("GetPartyMemberAffects"),		playerGetPartyMemberAffects,		METH_VARARGS },
		{VMProtectDecryptStringA("RemovePartyMember"),			playerRemovePartyMember,			METH_VARARGS },
		{VMProtectDecryptStringA("ExitParty"),					playerExitParty,					METH_VARARGS },

		// PK Mode
		{VMProtectDecryptStringA("GetPKMode"),					playerGetPKMode,					METH_VARARGS },

		// Mobile
		{VMProtectDecryptStringA("HasMobilePhoneNumber"),		playerHasMobilePhoneNumber,			METH_VARARGS },

		// Emotion
		{VMProtectDecryptStringA("RegisterEmotionIcon"),		playerRegisterEmotionIcon,			METH_VARARGS },
		{VMProtectDecryptStringA("GetEmotionIconImage"),		playerGetEmotionIconImage,			METH_VARARGS },

		// For System
		{VMProtectDecryptStringA("SetWeaponAttackBonusFlag"),	playerSetWeaponAttackBonusFlag,		METH_VARARGS },
		{VMProtectDecryptStringA("ToggleCoolTime"),				playerToggleCoolTime,				METH_VARARGS },
		{VMProtectDecryptStringA("ToggleLevelLimit"),			playerToggleLevelLimit,				METH_VARARGS },
		{VMProtectDecryptStringA("GetTargetVID"),				playerGetTargetVID,					METH_VARARGS },

		{VMProtectDecryptStringA("SetItemData"),				playerSetItemData,					METH_VARARGS },
		{VMProtectDecryptStringA("SetItemMetinSocket"),			playerSetItemMetinSocket,			METH_VARARGS },
		{VMProtectDecryptStringA("SetItemAttribute"),			playerSetItemAttribute,				METH_VARARGS },
		{VMProtectDecryptStringA("SetItemCount"),				playerSetItemCount,					METH_VARARGS },

		{VMProtectDecryptStringA("GetItemLink"),				playerGetItemLink,					METH_VARARGS },
		{VMProtectDecryptStringA("SlotTypeToInvenType"),		playerSlotTypeToInvenType,			METH_VARARGS },
		{VMProtectDecryptStringA("SendDragonSoulRefine"),		playerSendDragonSoulRefine,			METH_VARARGS },

#ifdef ENABLE_SEALBIND_SYSTEM
		{VMProtectDecryptStringA("CanSealItem"),				playerCanSealItem,					METH_VARARGS },
		{VMProtectDecryptStringA("GetItemSealDate"),			playerGetItemSealDate,				METH_VARARGS },
		{VMProtectDecryptStringA("GetItemUnSealLeftTime"),		GetItemUnSealLeftTime,				METH_VARARGS },
#endif
		{VMProtectDecryptStringA("GetItemFirstIndexByVnum"),	playerGetItemFirstIndexByVnum,		METH_VARARGS },
#if defined(ENABLE_KEYCHANGE_SYSTEM)
		// Keyboard Controls
		{VMProtectDecryptStringA("OpenKeyChangeWindow"), playerOpenKeyChangeWindow , METH_VARARGS },
		{VMProtectDecryptStringA("IsOpenKeySettingWindow"), playerIsOpenKeySettingWindow, METH_VARARGS },
		{VMProtectDecryptStringA("KeySettingClear"), playerKeySettingClear, METH_VARARGS },
		{VMProtectDecryptStringA("KeySetting"), playerKeySetting, METH_VARARGS },
		{VMProtectDecryptStringA("OnKeyDown"), playerOnKeyDown, METH_VARARGS },
		{VMProtectDecryptStringA("OnKeyUp"), playerOnKeyUp, METH_VARARGS },
#endif
		{ NULL,							NULL,								NULL },
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("player"), s_methods);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("LEVEL"), POINT_LEVEL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("VOICE"), POINT_VOICE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EXP"), POINT_EXP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEXT_EXP"), POINT_NEXT_EXP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("HP"), POINT_HP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAX_HP"), POINT_MAX_HP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SP"), POINT_SP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAX_SP"), POINT_MAX_SP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("STAMINA"), POINT_STAMINA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAX_STAMINA"), POINT_MAX_STAMINA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ELK"), POINT_GOLD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ST"), POINT_ST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("HT"), POINT_HT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DX"), POINT_DX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IQ"), POINT_IQ);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATT_POWER"), POINT_ATT_POWER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATT_MIN"), POINT_MIN_ATK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATT_MAX"), POINT_MAX_ATK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MIN_MAGIC_WEP"), POINT_MIN_MAGIC_WEP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAX_MAGIC_WEP"), POINT_MAX_MAGIC_WEP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATT_SPEED"), POINT_ATT_SPEED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATT_BONUS"), POINT_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EVADE_RATE"), POINT_EVADE_RATE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOVING_SPEED"), POINT_MOV_SPEED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DEF_GRADE"), POINT_DEF_GRADE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DEF_BONUS"), POINT_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CASTING_SPEED"), POINT_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAG_ATT"), POINT_MAGIC_ATT_GRADE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAG_DEF"), POINT_MAGIC_DEF_GRADE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMPIRE_POINT"), POINT_EMPIRE_POINT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("STAT"), POINT_STAT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_PASSIVE"), POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_SUPPORT"), POINT_SUB_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_ACTIVE"), POINT_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_HORSE"), POINT_HORSE_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PLAYTIME"), POINT_PLAYTIME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BOW_DISTANCE"), POINT_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("HP_RECOVERY"), POINT_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SP_RECOVERY"), POINT_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACKER_BONUS"), POINT_PARTY_ATT_GRADE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RESIST_MAGIC"), POINT_RESIST_MAGIC);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAX_NUM"), POINT_MAX_NUM);
	////
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_CRITICAL_PCT"), POINT_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_PENETRATE_PCT"), POINT_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MALL_ATTBONUS"), POINT_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_ATT_BONUS"), POINT_ATT_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MALL_DEFBONUS"), POINT_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MALL_EXPBONUS"), POINT_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MALL_ITEMBONUS"), POINT_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MALL_GOLDBONUS"), POINT_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MAX_HP_PCT"), POINT_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_MAX_SP_PCT"), POINT_MAX_SP_PCT);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_ATTBONUS_STONE"), POINT_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_ATTBONUS_LEGENDA"), POINT_ATTBONUS_LEGENDA);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_SKILL_DAMAGE_BONUS"), POINT_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_NORMAL_HIT_DAMAGE_BONUS"), POINT_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_SKILL_DEFEND_BONUS"), POINT_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_NORMAL_HIT_DEFEND_BONUS"), POINT_NORMAL_HIT_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_PC_BANG_EXP_BONUS"), POINT_PC_BANG_EXP_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_PC_BANG_DROP_BONUS"), POINT_PC_BANG_DROP_BONUS);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_WARRIOR_M"), MAIN_RACE_WARRIOR_M);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_ASSASSIN_W"), MAIN_RACE_ASSASSIN_W);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_SURA_M"), MAIN_RACE_SURA_M);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_SHAMAN_W"), MAIN_RACE_SHAMAN_W);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_WARRIOR_W"), MAIN_RACE_WARRIOR_W);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_ASSASSIN_M"), MAIN_RACE_ASSASSIN_M);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_SURA_W"), MAIN_RACE_SURA_W);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_SHAMAN_M"), MAIN_RACE_SHAMAN_M);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_WOLFMAN_M"), MAIN_RACE_WOLFMAN_M);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MAIN_RACE_MAX_NUM"), MAIN_RACE_MAX_NUM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ENERGY"), POINT_ENERGY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ENERGY_END_TIME"), POINT_ENERGY_END_TIME);
#ifdef ENABLE_EVENT_MANAGER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BONUS_EVENT"), BONUS_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DOUBLE_BOSS_LOOT_EVENT"), DOUBLE_BOSS_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DOUBLE_METIN_LOOT_EVENT"), DOUBLE_METIN_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DOUBLE_MISSION_BOOK_EVENT"), DOUBLE_MISSION_BOOK_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DUNGEON_COOLDOWN_EVENT"), DUNGEON_COOLDOWN_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DUNGEON_CHEST_LOOT_EVENT"), DUNGEON_CHEST_LOOT_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMPIRE_WAR_EVENT"), EMPIRE_WAR_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOONLIGHT_EVENT"), MOONLIGHT_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("TOURNAMENT_EVENT"), TOURNAMENT_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WHELL_OF_FORTUNE_EVENT"), WHELL_OF_FORTUNE_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("HALLOWEEN_EVENT"), HALLOWEEN_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NPC_SEARCH_EVENT"), NPC_SEARCH_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EXP_EVENT"), EXP_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ITEM_DROP_EVENT"), ITEM_DROP_EVENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("YANG_DROP_EVENT"), YANG_DROP_EVENT);
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ITEM_CHEQUE"), -1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CHEQUE"), POINT_CHEQUE);
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_PKT_OSIAG"), POINT_PKT_OSIAG);
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_OFFLINE_SHOP"), SLOT_TYPE_OFFLINE_SHOP);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_GRADE_NORMAL"), CPythonPlayer::SKILL_NORMAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_GRADE_MASTER"), CPythonPlayer::SKILL_MASTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_GRADE_GRAND_MASTER"), CPythonPlayer::SKILL_GRAND_MASTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_GRADE_PERFECT_MASTER"), CPythonPlayer::SKILL_PERFECT_MASTER);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CATEGORY_ACTIVE"), CPythonPlayer::CATEGORY_ACTIVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CATEGORY_PASSIVE"), CPythonPlayer::CATEGORY_PASSIVE);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY_PAGE_COLUMN"), c_Inventory_Page_Column);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY_PAGE_ROW"), c_Inventory_Page_Row);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY_PAGE_SIZE"), c_Inventory_Page_Size);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY_PAGE_COUNT"), c_Inventory_Page_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY_SLOT_COUNT"), c_Inventory_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EQUIPMENT_SLOT_START"), c_Equipment_Start);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EQUIPMENT_PAGE_COUNT"), c_Equipment_Count);
#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_EQUIPMENT_SLOT_START"), c_New_Equipment_Start);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_EQUIPMENT_SLOT_COUNT"), c_New_Equipment_Count);
#endif

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_SKILL"), CPythonPlayer::MBF_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_ATTACK"), CPythonPlayer::MBF_ATTACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_CAMERA"), CPythonPlayer::MBF_CAMERA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_SMART"), CPythonPlayer::MBF_SMART);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_MOVE"), CPythonPlayer::MBF_MOVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBF_AUTO"), CPythonPlayer::MBF_AUTO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBS_PRESS"), CPythonPlayer::MBS_PRESS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBS_CLICK"), CPythonPlayer::MBS_CLICK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBT_RIGHT"), CPythonPlayer::MBT_RIGHT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MBT_LEFT"), CPythonPlayer::MBT_LEFT);
#ifdef WJ_ENABLE_TRADABLE_ICON
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_NONE"), ON_TOP_WND_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_SHOP"), ON_TOP_WND_SHOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_EXCHANGE"), ON_TOP_WND_EXCHANGE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_SAFEBOX"), ON_TOP_WND_SAFEBOX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_PRIVATE_SHOP"), ON_TOP_WND_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_ITEM_COMB"), ON_TOP_WND_ITEM_COMB);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_TOP_WND_PET_FEED"), ON_TOP_WND_PET_FEED);
#endif
#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_BOOK_INVENTORY_SLOT_COUNT"), c_Skill_Book_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("UPGRADE_ITEMS_INVENTORY_SLOT_COUNT"), c_Upgrade_Items_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("STONE_INVENTORY_SLOT_COUNT"), c_Stone_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BOX_INVENTORY_SLOT_COUNT"), c_Box_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EFSUN_INVENTORY_SLOT_COUNT"), c_Efsun_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CICEK_INVENTORY_SLOT_COUNT"), c_Cicek_Inventory_Slot_Count);
#endif

	// Public code with server
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_NONE"), SLOT_TYPE_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_INVENTORY"), SLOT_TYPE_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_SKILL"), SLOT_TYPE_SKILL);
	// Special indecies for client
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_SHOP"), SLOT_TYPE_SHOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_EXCHANGE_OWNER"), SLOT_TYPE_EXCHANGE_OWNER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_EXCHANGE_TARGET"), SLOT_TYPE_EXCHANGE_TARGET);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_QUICK_SLOT"), SLOT_TYPE_QUICK_SLOT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_SAFEBOX"), SLOT_TYPE_SAFEBOX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_PRIVATE_SHOP"), SLOT_TYPE_PRIVATE_SHOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_MALL"), SLOT_TYPE_MALL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_EMOTION"), SLOT_TYPE_EMOTION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_DRAGON_SOUL_INVENTORY"), SLOT_TYPE_DRAGON_SOUL_INVENTORY);

#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_SKILL_BOOK_INVENTORY"), SLOT_TYPE_SKILL_BOOK_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_UPGRADE_ITEMS_INVENTORY"), SLOT_TYPE_UPGRADE_ITEMS_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_STONE_INVENTORY"), SLOT_TYPE_STONE_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_BOX_INVENTORY"), SLOT_TYPE_BOX_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_EFSUN_INVENTORY"), SLOT_TYPE_EFSUN_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_CICEK_INVENTORY"), SLOT_TYPE_CICEK_INVENTORY);
#endif

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RESERVED_WINDOW"), RESERVED_WINDOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INVENTORY"), INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EQUIPMENT"), EQUIPMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SAFEBOX"), SAFEBOX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MALL"), MALL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_INVENTORY"), DRAGON_SOUL_INVENTORY);
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BUFF_EQUIPMENT"), BUFF_EQUIPMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_BUFF_EQUIPMENT"), SLOT_TYPE_BUFF_EQUIPMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BUFF_EQUIPMENT_SLOT_START"), c_Buff_Equipment_Slot_Start);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BUFF_EQUIPMENT_SLOT_COUNT"), CItemData::BUFF_WINDOW_SLOT_MAX_NUM);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("GROUND"), GROUND);
#ifdef ENABLE_SWITCHBOT
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SWITCHBOT"), SWITCHBOT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SLOT_TYPE_SWITCHBOT"), SLOT_TYPE_SWITCHBOT);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ITEM_MONEY"), -1);

#ifdef ENABLE_CHEQUE_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ITEM_CHEQUE"), -2);
#endif

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_SLOT_COUNT"), SKILL_MAX_NUM);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EFFECT_PICK"), CPythonPlayer::EFFECT_PICK);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("METIN_SOCKET_TYPE_NONE"), CPythonPlayer::METIN_SOCKET_TYPE_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("METIN_SOCKET_TYPE_SILVER"), CPythonPlayer::METIN_SOCKET_TYPE_SILVER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("METIN_SOCKET_TYPE_GOLD"), CPythonPlayer::METIN_SOCKET_TYPE_GOLD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("METIN_SOCKET_MAX_NUM"), ITEM_SOCKET_SLOT_MAX_NUM);
#ifdef ENABLE_EXTENDED_SOCKETS
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ITEM_STONES_MAX_NUM"), ITEM_STONES_MAX_NUM);
#endif
	// refactored attribute slot begin
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_NORM_NUM"), ITEM_ATTRIBUTE_SLOT_NORM_NUM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_RARE_NUM"), ITEM_ATTRIBUTE_SLOT_RARE_NUM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_NORM_START"), ITEM_ATTRIBUTE_SLOT_NORM_START);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_NORM_END"), ITEM_ATTRIBUTE_SLOT_NORM_END);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_RARE_START"), ITEM_ATTRIBUTE_SLOT_RARE_START);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_RARE_END"), ITEM_ATTRIBUTE_SLOT_RARE_END);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTRIBUTE_SLOT_MAX_NUM"), ITEM_ATTRIBUTE_SLOT_MAX_NUM);
	// refactored attribute slot end

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_CANT"), REFINE_CANT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_OK"), REFINE_OK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_ALREADY_MAX_SOCKET_COUNT"), REFINE_ALREADY_MAX_SOCKET_COUNT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_NEED_MORE_GOOD_SCROLL"), REFINE_NEED_MORE_GOOD_SCROLL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_CANT_MAKE_SOCKET_ITEM"), REFINE_CANT_MAKE_SOCKET_ITEM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_NOT_NEXT_GRADE_ITEM"), REFINE_NOT_NEXT_GRADE_ITEM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_CANT_REFINE_METIN_TO_EQUIPMENT"), REFINE_CANT_REFINE_METIN_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REFINE_CANT_REFINE_ROD"), REFINE_CANT_REFINE_ROD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_CANT"), ATTACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_OK"), ATTACH_METIN_OK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_NOT_MATCHABLE_ITEM"), ATTACH_METIN_NOT_MATCHABLE_ITEM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_NO_MATCHABLE_SOCKET"), ATTACH_METIN_NO_MATCHABLE_SOCKET);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_NOT_EXIST_GOLD_SOCKET"), ATTACH_METIN_NOT_EXIST_GOLD_SOCKET);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT"), ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DETACH_METIN_CANT"), DETACH_METIN_CANT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DETACH_METIN_OK"), DETACH_METIN_OK);

	// Party
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_NORMAL"), CPythonPlayer::PARTY_ROLE_NORMAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_LEADER"), CPythonPlayer::PARTY_ROLE_LEADER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_ATTACKER"), CPythonPlayer::PARTY_ROLE_ATTACKER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_TANKER"), CPythonPlayer::PARTY_ROLE_TANKER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_BUFFER"), CPythonPlayer::PARTY_ROLE_BUFFER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_SKILL_MASTER"), CPythonPlayer::PARTY_ROLE_SKILL_MASTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_BERSERKER"), CPythonPlayer::PARTY_ROLE_BERSERKER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_DEFENDER"), CPythonPlayer::PARTY_ROLE_DEFENDER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_STATE_MAX_NUM"), CPythonPlayer::PARTY_ROLE_MAX_NUM);

	// Skill Index
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_TONGSOL"), c_iSkillIndex_Tongsol);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_FISHING"), c_iSkillIndex_Fishing);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_MINING"), c_iSkillIndex_Mining);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_MAKING"), c_iSkillIndex_Making);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_COMBO"), c_iSkillIndex_Combo);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_LANGUAGE1"), c_iSkillIndex_Language1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_LANGUAGE2"), c_iSkillIndex_Language2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_LANGUAGE3"), c_iSkillIndex_Language3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_POLYMORPH"), c_iSkillIndex_Polymorph);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_RIDING"), c_iSkillIndex_Riding);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKILL_INDEX_SUMMON"), c_iSkillIndex_Summon);

	// PK Mode
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_PEACE"), PK_MODE_PEACE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_REVENGE"), PK_MODE_REVENGE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_FREE"), PK_MODE_FREE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_PROTECT"), PK_MODE_PROTECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_GUILD"), PK_MODE_GUILD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PK_MODE_MAX_NUM"), PK_MODE_MAX_NUM);

	// Block Mode
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_EXCHANGE"), BLOCK_EXCHANGE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_PARTY"), BLOCK_PARTY_INVITE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_GUILD"), BLOCK_GUILD_INVITE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_WHISPER"), BLOCK_WHISPER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_FRIEND"), BLOCK_MESSENGER_INVITE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BLOCK_PARTY_REQUEST"), BLOCK_PARTY_REQUEST);

	// Party
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_EXP_NON_DISTRIBUTION"), PARTY_EXP_DISTRIBUTION_NON_PARITY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PARTY_EXP_DISTRIBUTION_PARITY"), PARTY_EXP_DISTRIBUTION_PARITY);

	// Emotion
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_CLAP"), EMOTION_CLAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_CHEERS_1"), EMOTION_CHEERS_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_CHEERS_2"), EMOTION_CHEERS_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_1"), EMOTION_DANCE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_2"), EMOTION_DANCE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_3"), EMOTION_DANCE_3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_4"), EMOTION_DANCE_4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_5"), EMOTION_DANCE_5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_DANCE_6"), EMOTION_DANCE_6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_CONGRATULATION"), EMOTION_CONGRATULATION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_FORGIVE"), EMOTION_FORGIVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_ANGRY"), EMOTION_ANGRY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_ATTRACTIVE"), EMOTION_ATTRACTIVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_SAD"), EMOTION_SAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_SHY"), EMOTION_SHY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_CHEERUP"), EMOTION_CHEERUP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_BANTER"), EMOTION_BANTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_JOY"), EMOTION_JOY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_KISS"), EMOTION_KISS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_FRENCH_KISS"), EMOTION_FRENCH_KISS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMOTION_SLAP"), EMOTION_SLAP);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AUTO_POTION_TYPE_HP"), CPythonPlayer::AUTO_POTION_TYPE_HP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AUTO_POTION_TYPE_SP"), CPythonPlayer::AUTO_POTION_TYPE_SP);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_PAGE_SIZE"), c_DragonSoul_Inventory_Box_Size);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_PAGE_COUNT"), DRAGON_SOUL_GRADE_MAX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_SLOT_COUNT"), c_DragonSoul_Inventory_Count);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_EQUIPMENT_SLOT_START"), c_DragonSoul_Equip_Start);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_EQUIPMENT_PAGE_COUNT"), DS_DECK_MAX_NUM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_EQUIPMENT_FIRST_SIZE"), c_DragonSoul_Equip_Slot_Max);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_REFINE_CLOSE"), DS_SUB_HEADER_CLOSE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_DO_UPGRADE"), DS_SUB_HEADER_DO_UPGRADE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_DO_IMPROVEMENT"), DS_SUB_HEADER_DO_IMPROVEMENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_DO_REFINE"), DS_SUB_HEADER_DO_REFINE);
#ifdef ENABLE_REMOVE_ITEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REMOVE_ITEM_SLOT_COUNT_X"), REMOVE_ITEM_SLOT_COUNT_X);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REMOVE_ITEM_SLOT_COUNT_Y"), REMOVE_ITEM_SLOT_COUNT_Y);
#endif
#ifdef RENEWAL_DEAD_PACKET
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REVIVE_TYPE_HERE"), REVIVE_TYPE_HERE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REVIVE_TYPE_TOWN"), REVIVE_TYPE_TOWN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REVIVE_TYPE_AUTO_TOWN"), REVIVE_TYPE_AUTO_TOWN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("REVIVE_TYPE_MAX"), REVIVE_TYPE_MAX);
#endif
#ifdef WEEKLY_RANK_BYLUZER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY1"), POINT_WEEKLY1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY2"), POINT_WEEKLY2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY3"), POINT_WEEKLY3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY4"), POINT_WEEKLY4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY5"), POINT_WEEKLY5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY6"), POINT_WEEKLY6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY7"), POINT_WEEKLY7);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WEEKLY_SEASON"), POINT_WEEKLY_SEASON);
#endif
#if defined(ENABLE_KEYCHANGE_SYSTEM)
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_UP_1"), KEY_MOVE_UP_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_DOWN_1"), KEY_MOVE_DOWN_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_LEFT_1"), KEY_MOVE_LEFT_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_RIGHT_1"), KEY_MOVE_RIGHT_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_UP_2"), KEY_MOVE_UP_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_DOWN_2"), KEY_MOVE_DOWN_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_LEFT_2"), KEY_MOVE_LEFT_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MOVE_RIGHT_2"), KEY_MOVE_RIGHT_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ROTATE_POSITIVE_1"), KEY_CAMERA_ROTATE_POSITIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ROTATE_NEGATIVE_1"), KEY_CAMERA_ROTATE_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ZOOM_POSITIVE_1"), KEY_CAMERA_ZOOM_POSITIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ZOOM_NEGATIVE_1"), KEY_CAMERA_ZOOM_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_PITCH_POSITIVE_1"), KEY_CAMERA_PITCH_POSITIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_PITCH_NEGATIVE_1"), KEY_CAMERA_PITCH_NEGATIVE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ROTATE_POSITIVE_2"), KEY_CAMERA_ROTATE_POSITIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ROTATE_NEGATIVE_2"), KEY_CAMERA_ROTATE_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ZOOM_POSITIVE_2"), KEY_CAMERA_ZOOM_POSITIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_ZOOM_NEGATIVE_2"), KEY_CAMERA_ZOOM_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_PITCH_POSITIVE_2"), KEY_CAMERA_PITCH_POSITIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CAMERA_PITCH_NEGATIVE_2"), KEY_CAMERA_PITCH_NEGATIVE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ROOTING_1"), KEY_ROOTING_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ROOTING_2"), KEY_ROOTING_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ATTACK"), KEY_ATTACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_RIDEMYHORS"), KEY_RIDEMYHORS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_FEEDMYHORS"), KEY_FEEDMYHORS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_BYEMYHORS"), KEY_BYEMYHORS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_RIDEHORS"), KEY_RIDEHORS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION1"), KEY_EMOTION1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION2"), KEY_EMOTION2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION3"), KEY_EMOTION3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION4"), KEY_EMOTION4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION5"), KEY_EMOTION5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION6"), KEY_EMOTION6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION7"), KEY_EMOTION7);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION8"), KEY_EMOTION8);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_EMOTION9"), KEY_EMOTION9);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_1"), KEY_SLOT_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_2"), KEY_SLOT_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_3"), KEY_SLOT_3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_4"), KEY_SLOT_4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_5"), KEY_SLOT_5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_6"), KEY_SLOT_6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_7"), KEY_SLOT_7);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_8"), KEY_SLOT_8);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_CHANGE_1"), KEY_SLOT_CHANGE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_CHANGE_2"), KEY_SLOT_CHANGE_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_CHANGE_3"), KEY_SLOT_CHANGE_3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SLOT_CHANGE_4"), KEY_SLOT_CHANGE_4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_STATE"), KEY_OPEN_STATE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_SKILL"), KEY_OPEN_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_QUEST"), KEY_OPEN_QUEST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_INVENTORY"), KEY_OPEN_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_DDS"), KEY_OPEN_DDS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_MINIMAP"), KEY_OPEN_MINIMAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_LOGCHAT"), KEY_OPEN_LOGCHAT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_PET"), KEY_OPEN_PET);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_GUILD"), KEY_OPEN_GUILD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_MESSENGER"), KEY_OPEN_MESSENGER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_HELP"), KEY_OPEN_HELP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_ACTION"), KEY_OPEN_ACTION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SCROLL_ONOFF"), KEY_SCROLL_ONOFF);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_PLUS_MINIMAP"), KEY_PLUS_MINIMAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MIN_MINIMAP"), KEY_MIN_MINIMAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SCREENSHOT"), KEY_SCREENSHOT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SHOW_NAME"), KEY_SHOW_NAME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_AUTO"), KEY_OPEN_AUTO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_AUTO_RUN"), KEY_AUTO_RUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_TP_MAP"), KEY_TP_MAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_MONSTER_CARD"), KEY_MONSTER_CARD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_PARTY_MATCH"), KEY_PARTY_MATCH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SELECT_DSS_1"), KEY_SELECT_DSS_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_SELECT_DSS_2"), KEY_SELECT_DSS_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_PASSIVE_ATTR1"), KEY_PASSIVE_ATTR1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_PASSIVE_ATTR2"), KEY_PASSIVE_ATTR2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_EXTENDED_INV"), KEY_OPEN_EXTENDED_INV);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_PET"), KEY_OPEN_PET);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_DOPY"), KEY_OPEN_DOPY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_MARBLE"), KEY_OPEN_MARBLE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_SAVE_LOCATION"), KEY_OPEN_SAVE_LOCATION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_RESP"), KEY_OPEN_RESP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_BUFF"), KEY_OPEN_BUFF);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_MISSION"), KEY_OPEN_MISSION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_COLLECTION"), KEY_OPEN_COLLECTION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_RETURN"), KEY_RETURN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_OPEN_DUNGEONS"), KEY_OPEN_DUNGEONS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL1"), KEY_CHANGECHANNEL1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL2"), KEY_CHANGECHANNEL2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL3"), KEY_CHANGECHANNEL3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL4"), KEY_CHANGECHANNEL4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL5"), KEY_CHANGECHANNEL5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_CHANGECHANNEL6"), KEY_CHANGECHANNEL6);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ADDKEYBUFFERCONTROL"), KEY_ADDKEYBUFFERCONTROL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ADDKEYBUFFERALT"), KEY_ADDKEYBUFFERALT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KEY_ADDKEYBUFFERSHIFT"), KEY_ADDKEYBUFFERSHIFT);
#endif

	
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("POINT_PRIVATE_SHOP_UNLOCKED_SLOT"), POINT_PRIVATE_SHOP_UNLOCKED_SLOT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("CHARACTER_NAME_MAX_LEN"), CHARACTER_NAME_MAX_LEN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_NORMAL"), DRAGON_SOUL_GRADE_NORMAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_BRILLIANT"), DRAGON_SOUL_GRADE_BRILLIANT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_RARE"), DRAGON_SOUL_GRADE_RARE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_ANCIENT"), DRAGON_SOUL_GRADE_ANCIENT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_LEGENDARY"), DRAGON_SOUL_GRADE_LEGENDARY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_GRADE_MAX"), DRAGON_SOUL_GRADE_MAX);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_LOWEST"), DRAGON_SOUL_STEP_LOWEST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_LOW"), DRAGON_SOUL_STEP_LOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_MID"), DRAGON_SOUL_STEP_MID);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_HIGH"), DRAGON_SOUL_STEP_HIGH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_HIGHEST"), DRAGON_SOUL_STEP_HIGHEST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DRAGON_SOUL_STEP_MAX"), DRAGON_SOUL_STEP_MAX);
#endif

	VMProtectEnd();
}

//martysama0134's ceqyqttoaf71vasf9t71218
