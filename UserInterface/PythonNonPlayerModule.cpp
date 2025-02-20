#include "StdAfx.h"
#include "PythonNonPlayer.h"

#include "InstanceBase.h"
#include "PythonCharacterManager.h"

PyObject * nonplayerGetEventType(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualNumber))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventType(iVirtualNumber);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetEventTypeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	BYTE iType = CPythonNonPlayer::Instance().GetEventTypeByVID(iVirtualID);

	return Py_BuildValue("i", iType);
}

PyObject * nonplayerGetLevelByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	float fAverageLevel = pMobTable->bLevel;//(float(pMobTable->abLevelRange[0]) + float(pMobTable->abLevelRange[1])) / 2.0f;
	fAverageLevel = floor(fAverageLevel + 0.5f);
	return Py_BuildValue("i", int(fAverageLevel));
}

PyObject* nonplayerGetRaceFlagByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwRaceFlag);
}

PyObject * nonplayerGetGradeByVID(PyObject * poSelf, PyObject * poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable * pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}

PyObject* nonplayerGetVnumByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", -1);

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(pInstance->GetVirtualNumber());

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwVnum);
}

PyObject * nonplayerGetMonsterName(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return Py_BuildValue("s", rkNonPlayer.GetMonsterName(iVNum));
}

PyObject * nonplayerGetMonsterLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	CPythonNonPlayer& rkNonPlayer=CPythonNonPlayer::Instance();
	return Py_BuildValue("i", rkNonPlayer.GetMonsterLevel(iVNum));
}

PyObject * nonplayerGetMonsterGrade(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(iVNum);

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->bRank);
}

PyObject * nonplayerGetAttElementFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iVNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVNum))
		return Py_BuildException();

	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(iVNum);

	if (!pMobTable)
		return Py_BuildValue("i", -1);

	return Py_BuildValue("i", pMobTable->dwRaceFlag);
}

PyObject * nonplayerLoadNonPlayerData(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if(!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNonPlayer::Instance().LoadNonPlayerData(szFileName);
	return Py_BuildNone();
}

void initNonPlayer()
{
	VMProtectBeginUltra("initNonPlayer");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("GetEventType"),				nonplayerGetEventType,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetEventTypeByVID"),			nonplayerGetEventTypeByVID,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetLevelByVID"),				nonplayerGetLevelByVID,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetGradeByVID"),				nonplayerGetGradeByVID,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetMonsterName"),				nonplayerGetMonsterName,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetVnumByVID"),				nonplayerGetVnumByVID,				METH_VARARGS },

		{ VMProtectDecryptStringA("GetMonsterLevel"),			nonplayerGetMonsterLevel,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetMonsterGrade"),			nonplayerGetMonsterGrade,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetAttElementFlag"),			nonplayerGetAttElementFlag,			METH_VARARGS },

		{ VMProtectDecryptStringA("LoadNonPlayerData"),			nonplayerLoadNonPlayerData,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetRaceFlagByVID"),			nonplayerGetRaceFlagByVID,			METH_VARARGS },

		{ NULL,							NULL,								NULL		 },
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("nonplayer"), s_methods);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_CLICK_EVENT_NONE"), CPythonNonPlayer::ON_CLICK_EVENT_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_CLICK_EVENT_BATTLE"), CPythonNonPlayer::ON_CLICK_EVENT_BATTLE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_CLICK_EVENT_SHOP"), CPythonNonPlayer::ON_CLICK_EVENT_SHOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_CLICK_EVENT_TALK"), CPythonNonPlayer::ON_CLICK_EVENT_TALK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ON_CLICK_EVENT_VEHICLE"), CPythonNonPlayer::ON_CLICK_EVENT_VEHICLE);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PAWN"), 0);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("S_PAWN"), 1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KNIGHT"), 2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("S_KNIGHT"), 3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("BOSS"), 4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("KING"), 5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ELITKA"), 6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("LEGENDA"), 7);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ANIMAL"), CPythonNonPlayer::RACE_FLAG_ANIMAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_UNDEAD"), CPythonNonPlayer::RACE_FLAG_UNDEAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_DEVIL"), CPythonNonPlayer::RACE_FLAG_DEVIL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_HUMAN"), CPythonNonPlayer::RACE_FLAG_HUMAN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ORC"), CPythonNonPlayer::RACE_FLAG_ORC);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_MILGYO"), CPythonNonPlayer::RACE_FLAG_MILGYO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_INSECT"), CPythonNonPlayer::RACE_FLAG_INSECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_FIRE"), CPythonNonPlayer::RACE_FLAG_FIRE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ICE"), CPythonNonPlayer::RACE_FLAG_ICE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_DESERT"), CPythonNonPlayer::RACE_FLAG_DESERT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_TREE"), CPythonNonPlayer::RACE_FLAG_TREE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_ELEC"), CPythonNonPlayer::RACE_FLAG_ATT_ELEC);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_FIRE"), CPythonNonPlayer::RACE_FLAG_ATT_FIRE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_ICE"), CPythonNonPlayer::RACE_FLAG_ATT_ICE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_WIND"), CPythonNonPlayer::RACE_FLAG_ATT_WIND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_EARTH"), CPythonNonPlayer::RACE_FLAG_ATT_EARTH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_ATT_DARK"), CPythonNonPlayer::RACE_FLAG_ATT_DARK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_BOSS"), CPythonNonPlayer::RACE_FLAG_BOSS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_WLADCA"), CPythonNonPlayer::RACE_FLAG_WLADCA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_STONE"), CPythonNonPlayer::RACE_FLAG_STONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_DUNGEON"), CPythonNonPlayer::RACE_FLAG_DUNGEON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("RACE_FLAG_LEGENDA"), CPythonNonPlayer::RACE_FLAG_LEGENDA);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_STUN"), CPythonNonPlayer::IMMUNE_STUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_SLOW"), CPythonNonPlayer::IMMUNE_SLOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_FALL"), CPythonNonPlayer::IMMUNE_FALL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_CURSE"), CPythonNonPlayer::IMMUNE_CURSE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_POISON"), CPythonNonPlayer::IMMUNE_POISON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_TERROR"), CPythonNonPlayer::IMMUNE_TERROR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("IMMUNE_REFLECT"), CPythonNonPlayer::IMMUNE_REFLECT);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_SWORD"), CPythonNonPlayer::MOB_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_TWOHAND"), CPythonNonPlayer::MOB_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_DAGGER"), CPythonNonPlayer::MOB_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_BELL"), CPythonNonPlayer::MOB_RESIST_BELL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_FAN"), CPythonNonPlayer::MOB_RESIST_FAN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_BOW"), CPythonNonPlayer::MOB_RESIST_BOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_FIRE"), CPythonNonPlayer::MOB_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_ELECT"), CPythonNonPlayer::MOB_RESIST_ELECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_MAGIC"), CPythonNonPlayer::MOB_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_WIND"), CPythonNonPlayer::MOB_RESIST_WIND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOB_RESIST_POISON"), CPythonNonPlayer::MOB_RESIST_POISON);
	VMProtectEnd();
}
//martysama0134's ceqyqttoaf71vasf9t71218
