#include "StdAfx.h"
#include "PythonCharacterManager.h"
#include "PythonNonPlayer.h"
#include "../gamelib/GameLibDefines.h"

PyObject * chrRaceToJob(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	return Py_BuildValue("i", RaceToJob(race));
}

PyObject * chrRaceToSex(PyObject * poSelf, PyObject * poArgs)
{
	int race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	return Py_BuildValue("i", RaceToSex(race));
}

PyObject * chrDestroy(PyObject * poSelf, PyObject * poArgs)
{
	CPythonCharacterManager::Instance().Destroy();
	return Py_BuildNone();
}

PyObject * chrUpdate(PyObject * poSelf, PyObject * poArgs)
{
	CPythonCharacterManager::Instance().Update();
	return Py_BuildNone();
}

PyObject * chrDeform(PyObject * poSelf, PyObject * poArgs)
{
	CPythonCharacterManager::Instance().Deform();
	return Py_BuildNone();
}

PyObject * chrRender(PyObject * poSelf, PyObject * poArgs)
{
	CPythonCharacterManager::Instance().Render();
	return Py_BuildNone();
}

PyObject * chrRenderCollision(PyObject * poSelf, PyObject * poArgs)
{
	CPythonCharacterManager::Instance().RenderCollision();
	return Py_BuildNone();
}

// Functions For Python Test Code
PyObject * chrCreateInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	if (PyTuple_Size(poArgs)==2)
	{
		PyObject* poDict=PyTuple_GetItem(poArgs, 1);
		if (!PyDict_Check(poDict))
			return Py_BuildException();

		CInstanceBase::SCreateData kCreateData;
		kCreateData.m_bType=CActorInstance::TYPE_PC;
		kCreateData.m_dwLevel = 0;
		kCreateData.m_dwGuildID=0;
		kCreateData.m_dwEmpireID=0;
		kCreateData.m_dwVID=iVirtualID;
		kCreateData.m_dwMountVnum=0;
#ifdef ENABLE_QUIVER_SYSTEM
		kCreateData.m_dwArrow = 0;
#endif
		kCreateData.m_dwRace=0;
		kCreateData.m_fRot=CInstanceBase::DIR_NORTH;
 		kCreateData.m_lPosX=0;
		kCreateData.m_lPosY=0;
		kCreateData.m_stName="NONAME";
		kCreateData.m_dwStateFlags=0;
		kCreateData.m_dwMovSpd=100;
		kCreateData.m_dwAtkSpd=100;
		kCreateData.m_sAlignment=0;
		kCreateData.m_byPKMode=0;
		kCreateData.m_kAffectFlags.Clear();
		kCreateData.m_dwArmor=8;
		kCreateData.m_dwWeapon=0;
		kCreateData.m_dwHair=0;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		kCreateData.m_dwAcce = 0;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
		kCreateData.m_dwEmblemat = 0;
#endif
		kCreateData.m_isMain=false;

		PyObject* poHorse=PyDict_GetItemString(poDict, "horse");
		if (poHorse)
			kCreateData.m_dwMountVnum=PyLong_AsLong(poHorse);

		PyObject* poX=PyDict_GetItemString(poDict, "x");
		if (poX)
			kCreateData.m_lPosX=PyLong_AsLong(poX);

		PyObject* poY=PyDict_GetItemString(poDict, "y");
		if (poY)
			kCreateData.m_lPosX=PyLong_AsLong(poY);

		CPythonCharacterManager::Instance().CreateInstance(kCreateData);
	}
	else
	{
		CPythonCharacterManager::Instance().RegisterInstance(iVirtualID);
	}

	return Py_BuildNone();
}

PyObject * chrDeleteInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonCharacterManager::Instance().DeleteInstance(iVirtualID);
	return Py_BuildNone();
}

PyObject * chrDeleteInstanceByFade(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonCharacterManager::Instance().DeleteInstanceByFade(iVirtualID);
	return Py_BuildNone();
}

PyObject * chrSelectInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonCharacterManager::Instance().SelectInstance(iVirtualID);
	return Py_BuildNone();
}

PyObject * chrHasInstance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	bool bFlag = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID) ? TRUE : FALSE;
	return Py_BuildValue("i", bFlag);
}

PyObject * chrIsEnemy(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (!pInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pInstance->IsEnemy());
}

PyObject * chrIsNPC(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (!pInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pInstance->IsNPC());
}

PyObject * chrIsGameMaster(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = (iVirtualID) ? CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID) : CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (!pInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pInstance->IsGameMaster());
}

PyObject * chrIsPartyMember(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (!pInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pInstance->IsPartyMember());
}

PyObject * chrSelect(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pkInst)
		return Py_BuildNone();

	switch (CPythonNonPlayer::Instance().GetEventType(pkInst->GetVirtualNumber()))
	{
		case CPythonNonPlayer::ON_CLICK_EVENT_SHOP:
			pkInst->SetAddRenderMode();
			pkInst->SetAddColor(D3DXCOLOR(0.0f, 0.3f, 0.0f, 1.0f));
			break;

		default:
			//pkInst->SetAddColor(D3DXCOLOR(0.3f, 0.0f, 0.0f, 1.0f));
			break;
	}

	return Py_BuildNone();
}

PyObject * chrSetAddRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetAddRenderMode();
	pkInst->SetAddColor(D3DXCOLOR(fr, fg, fb, 1.0f));

	return Py_BuildNone();
}

PyObject * chrSetBlendRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	float fAlpha;
	if (!PyTuple_GetFloat(poArgs, 1, &fAlpha))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetAlpha(fAlpha);

	return Py_BuildNone();
}

PyObject * chrUnselect(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (!pkInst)
		return Py_BuildNone();

	pkInst->RestoreRenderMode();
	return Py_BuildNone();
}

PyObject * chrPick(PyObject* poSelf, PyObject* poArgs)
{
	DWORD VirtualID = 0;
	if (CPythonCharacterManager::Instance().OLD_GetPickedInstanceVID(&VirtualID))
		return Py_BuildValue("i", VirtualID);
	else
		return Py_BuildValue("i", -1);
}

PyObject * chrHide(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->Hide();
	return Py_BuildNone();
}

PyObject * chrShow(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->Show();
	return Py_BuildNone();
}

PyObject * chrPickAll(PyObject* poSelf, PyObject* poArgs)
{
	DWORD VirtualID = CPythonCharacterManager::Instance().PickAll();
	return Py_BuildValue("i", VirtualID);
}

PyObject * chrSetRace(PyObject* poSelf, PyObject* poArgs)
{
	int iRace;
	if (!PyTuple_GetInteger(poArgs, 0, &iRace))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetRace(iRace);

	return Py_BuildNone();
}

PyObject * chrSetHair(PyObject* poSelf, PyObject* poArgs)
{
	int iRace;
	if (!PyTuple_GetInteger(poArgs, 0, &iRace))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetHair(iRace);

	return Py_BuildNone();
}

PyObject * chrChangeHair(PyObject* poSelf, PyObject* poArgs)
{
	int iHair;

	if (!PyTuple_GetInteger(poArgs, 0, &iHair))
		return Py_BuildException();
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->ChangeHair(iHair);
	return Py_BuildNone();
}

PyObject * chrSetArmor(PyObject* poSelf, PyObject* poArgs)
{
	int iForm;
	if (!PyTuple_GetInteger(poArgs, 0, &iForm))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetArmor(iForm);

	pkInst->RegisterBoundingSphere();

	return Py_BuildNone();
}

PyObject * chrChangeShape(PyObject* poSelf, PyObject* poArgs)
{
	int iForm;
	if (!PyTuple_GetInteger(poArgs, 0, &iForm))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->ChangeArmor(iForm);

	return Py_BuildNone();
}

PyObject * chrSetWeapon(PyObject* poSelf, PyObject* poArgs)
{
	int iForm;
	if (!PyTuple_GetInteger(poArgs, 0, &iForm))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetWeapon(iForm);

	return Py_BuildNone();
}

PyObject * chrSetVirtualID(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->SetVirtualID(iVID);
	return Py_BuildNone();
}

PyObject * chrSetNameString(PyObject* poSelf, PyObject* poArgs)
{
	char * c_szName;
	if (!PyTuple_GetString(poArgs, 0, &c_szName))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->SetNameString(c_szName, strlen(c_szName));
	return Py_BuildNone();
}

PyObject * chrSetInstanceType(PyObject* poSelf, PyObject* poArgs)
{
	int iInstanceType;
	if (!PyTuple_GetInteger(poArgs, 0, &iInstanceType))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->SetInstanceType(iInstanceType);
	return Py_BuildNone();
}

PyObject * chrAttachEffectByName(PyObject* poSelf, PyObject* poArgs)
{
	int iParentPartIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iParentPartIndex))
		return Py_BuildException();

	char * pszBoneName;
	if (!PyTuple_GetString(poArgs, 1, &pszBoneName))
	{
		pszBoneName = NULL;
		//return Py_BuildException();
	}

	char * pszEffectName;
	if (!PyTuple_GetString(poArgs, 2, &pszEffectName))
		return Py_BuildException();

	int iLife = CActorInstance::EFFECT_LIFE_INFINITE;

	if (PyTuple_Size(poArgs)==4)
	{
		if (!PyTuple_GetInteger(poArgs, 3, &iLife))
			return Py_BuildException();
	}

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pkInst)
		return Py_BuildNone();

	//pkInst->AttachEffectByName(iParentPartIndex, pszBoneName, pszEffectName, iLife);
	return Py_BuildNone();
}

PyObject * chrAttachEffectByID(PyObject* poSelf, PyObject* poArgs)
{
	int iParentPartIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iParentPartIndex))
		return Py_BuildException();

	char * pszBoneName;
	if (!PyTuple_GetString(poArgs, 1, &pszBoneName))
	{
		pszBoneName = NULL;
	}
	//	return Py_BuildException();

	// FIXME : bug or error on getting unsigned value
	int iEffectID;
	if (!PyTuple_GetInteger(poArgs, 2, &iEffectID))
		return Py_BuildException();

	int iLife = CActorInstance::EFFECT_LIFE_INFINITE;

	if (PyTuple_Size(poArgs)==4)
	{
		if (!PyTuple_GetInteger(poArgs, 3, &iLife))
			return Py_BuildException();
	}

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pkInst)
		return Py_BuildNone();

	//pkInst->AttachEffectByID(iParentPartIndex, pszBoneName, iEffectID, iLife);
	return Py_BuildNone();
}

PyObject * chrRefresh(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pkInst)
		return Py_BuildNone();

	//pkInst->Refresh(CRaceMotionData::NAME_WAIT, true);
	return Py_BuildNone();
}

PyObject * chrMountHorse(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
 	pkInst->MountHorse(20030);
	return Py_BuildNone();
}

PyObject * chrDismountHorse(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
 	pkInst->DismountHorse();
	return Py_BuildNone();
}

PyObject * chrRevive(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
 	pkInst->Revive();
	return Py_BuildNone();
}

PyObject * chrDie(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
 	pkInst->Die();
	return Py_BuildNone();
}

PyObject * chrLookAt(PyObject* poSelf, PyObject* poArgs)
{
	int iCellX;
	if (!PyTuple_GetInteger(poArgs, 0, &iCellX))
		return Py_BuildException();

	int iCellY;
	if (!PyTuple_GetInteger(poArgs, 1, &iCellY))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
// 	pkInst->LookAt(TPixelPosition(iCellX * c_Section_xAttributeCellSize, iCellY * c_Section_xAttributeCellSize));
	return Py_BuildNone();
}

PyObject * chrSetMotionMode(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionMode))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetMotionMode(iMotionMode);
	return Py_BuildNone();
}

PyObject * chrSetLoopMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionIndex))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->SetLoopMotion(WORD(iMotionIndex));

	return Py_BuildNone();
}

PyObject * chrBlendLoopMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionIndex))
		return Py_BuildException();
	float fBlendTime;
	if (!PyTuple_GetFloat(poArgs, 1, &fBlendTime))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->SetLoopMotion(WORD(iMotionIndex), fBlendTime);

	return Py_BuildNone();
}

PyObject * chrPushOnceMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionIndex))
		return Py_BuildException();

	float fBlendTime;
	if (!PyTuple_GetFloat(poArgs, 1, &fBlendTime))
	{
		fBlendTime = 0.1f;
	}

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->PushOnceMotion(WORD(iMotionIndex), fBlendTime);

	return Py_BuildNone();
}

PyObject * chrPushLoopMotion(PyObject* poSelf, PyObject* poArgs)
{
	int iMotionIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMotionIndex))
		return Py_BuildException();

	float fBlendTime;
	if (!PyTuple_GetFloat(poArgs, 1, &fBlendTime))
	{
		fBlendTime = 0.1f;
	}

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();
	pkInst->PushLoopMotion(WORD(iMotionIndex), fBlendTime);

	return Py_BuildNone();
}

PyObject * chrSetPixelPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iX;
	if (!PyTuple_GetInteger(poArgs, 0, &iX))
		return Py_BuildException();
	int iY;
	if (!PyTuple_GetInteger(poArgs, 1, &iY))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	int iZ;
	if (PyTuple_GetInteger(poArgs, 2, &iZ))
	{
		pkInst->NEW_SetPixelPosition(TPixelPosition(iX, iY, iZ));
	}
	else
	{
		pkInst->SCRIPT_SetPixelPosition(iX, iY);
	}
	return Py_BuildNone();
}

PyObject * chrSetDirection(PyObject* poSelf, PyObject* poArgs)
{
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 0, &iDirection))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	pkInst->SetDirection(iDirection);
	return Py_BuildNone();
}

PyObject * chrGetPixelPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pkInst)
		return Py_BuildException();

	TPixelPosition PixelPosition;
	pkInst->NEW_GetPixelPosition(&PixelPosition);

	return Py_BuildValue("fff", PixelPosition.x, PixelPosition.y, PixelPosition.z);
}

PyObject * chrSetRotation(PyObject* poSelf, PyObject* poArgs)
{
	float fRotation;
	if (!PyTuple_GetFloat(poArgs, 0, &fRotation))
		return Py_BuildException();

	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pCharacterInstance)
		return Py_BuildNone();

	fRotation = fmod(fRotation + 180.0f, 360.0f);

	pCharacterInstance->SetRotation(fRotation);

	return Py_BuildNone();
}

PyObject * chrSetRotationAll(PyObject* poSelf, PyObject* poArgs)
{
	float fRotX;
	if (!PyTuple_GetFloat(poArgs, 0, &fRotX))
		return Py_BuildException();
	float fRotY;
	if (!PyTuple_GetFloat(poArgs, 1, &fRotY))
		return Py_BuildException();
	float fRotZ;
	if (!PyTuple_GetFloat(poArgs, 2, &fRotZ))
		return Py_BuildException();

	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pCharacterInstance)
		return Py_BuildNone();

	pCharacterInstance->GetGraphicThingInstanceRef().SetXYRotation(fRotX, fRotY);
	pCharacterInstance->GetGraphicThingInstanceRef().SetRotation(fRotZ);
	return Py_BuildNone();
}

PyObject * chrBlendRotation(PyObject* poSelf, PyObject* poArgs)
{
	float fRotation;
	if (!PyTuple_GetFloat(poArgs, 0, &fRotation))
		return Py_BuildException();
	float fBlendTime;
	if (!PyTuple_GetFloat(poArgs, 1, &fBlendTime))
		return Py_BuildException();

	fRotation = fmod(720.0f - fRotation, 360.0f);

	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pCharacterInstance)
		return Py_BuildNone();
	pCharacterInstance->BlendRotation(fRotation, fBlendTime);

	return Py_BuildNone();
}

PyObject * chrGetRotation(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pCharacterInstance)
		return Py_BuildValue("f", 0.0f);

	float fRotation = pCharacterInstance->GetRotation();

	return Py_BuildValue("f", 360.0f - fRotation);
}

PyObject * chrGetRace(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pCharacterInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pCharacterInstance->GetRace());
}

PyObject * chrGetName(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pCharacterInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("s", pCharacterInstance->GetNameString());
}

PyObject * chrGetNameByVID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("s", "None");

	return Py_BuildValue("s", pInstance->GetNameString());
}

PyObject * chrGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pInstance->GetGuildID());
}

PyObject * chrGetProjectPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 1, &iHeight))
		return Py_BuildException();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (!pInstance)
		return Py_BuildValue("ii", -100, -100);

	TPixelPosition PixelPosition;
	pInstance->NEW_GetPixelPosition(&PixelPosition);

	CPythonGraphic & rpyGraphic = CPythonGraphic::Instance();

	float fx, fy, fz;
	rpyGraphic.ProjectPosition(PixelPosition.x,
							   -PixelPosition.y,
							   PixelPosition.z + float(iHeight),
							   &fx, &fy, &fz);

	if (1 == int(fz))
		return Py_BuildValue("ii", -100, -100);

	return Py_BuildValue("ii", int(fx), int(fy));
}

PyObject * chrGetVirtualNumber(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (NULL != pkInst)
		return Py_BuildValue("i", pkInst->GetVirtualNumber());

	return Py_BuildValue("i", CActorInstance::TYPE_PC);
}

PyObject * chrGetInstanceType(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);

	if (NULL != pkInst)
		return Py_BuildValue("i", pkInst->GetInstanceType());

	return Py_BuildValue("i", CActorInstance::TYPE_PC);
}

PyObject * chrGetBoundBoxOnlyXY(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pkInst)
		return Py_BuildValue("ffff", 0.0f, 0.0f, 0.0f, 0.0f);

	D3DXVECTOR3 v3Min, v3Max;
	pkInst->GetBoundBox(&v3Min, &v3Max);

	return Py_BuildValue("ffff", v3Min.x, v3Min.y, v3Max.x, v3Max.y);
}

PyObject * chrtestGetPKData(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	if (!pkInst)
		return Py_BuildValue("ii", 0, 4);

	return Py_BuildValue("ii", pkInst->GetAlignment(), pkInst->GetAlignmentGrade());
}

PyObject * chrtestSetComboType(PyObject* poSelf, PyObject* poArgs)
{
	int iComboType;
	if (!PyTuple_GetInteger(poArgs, 0, &iComboType))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->GetGraphicThingInstanceRef().SetComboType(iComboType);

	return Py_BuildNone();
}

PyObject * chrtestSetAddRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	int iColor;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->SetAddRenderMode();
		pkInst->SetAddColor(0xff000000 | iColor);
	}

	return Py_BuildNone();
}

PyObject * chrtestSetModulateRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	int iColor;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->SetModulateRenderMode();
		pkInst->SetAddColor(0xff000000 | iColor);
	}

	return Py_BuildNone();
}

PyObject * chrtestSetAddRenderModeRGB(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->SetAddRenderMode();
		pkInst->SetAddColor(D3DXCOLOR(fr, fg, fb, 1.0f));
	}

	return Py_BuildNone();
}

PyObject * chrtestSetModulateRenderModeRGB(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
		return Py_BuildException();
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
		return Py_BuildException();
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->SetModulateRenderMode();
		pkInst->SetAddColor(D3DXCOLOR(fr, fg, fb, 1.0f));
	}

	return Py_BuildNone();
}

PyObject * chrtestSetSpecularRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	int iPart;
	if (!PyTuple_GetInteger(poArgs, 1, &iPart))
		return Py_BuildException();
	float fAlpha;
	if (!PyTuple_GetFloat(poArgs, 2, &fAlpha))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->GetGraphicThingInstanceRef().SetSpecularInfo(TRUE, iPart, fAlpha);
	}

	return Py_BuildNone();
}

PyObject * chrtestSetSpecularRenderMode2(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();
	int iPart;
	if (!PyTuple_GetInteger(poArgs, 1, &iPart))
		return Py_BuildException();
	float fAlpha;
	if (!PyTuple_GetFloat(poArgs, 2, &fAlpha))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->GetGraphicThingInstanceRef().SetSpecularInfoForce(TRUE, iPart, fAlpha);
	}

	return Py_BuildNone();
}

PyObject * chrtestRestoreRenderMode(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetInstancePtr(iVirtualID);
	if (pkInst)
	{
		pkInst->RestoreRenderMode();
	}

	return Py_BuildNone();
}

PyObject * chrFaintTest(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (pCharacterInstance)
	{
		if (pCharacterInstance->GetGraphicThingInstanceRef().IsFaint())
		{
			pCharacterInstance->GetGraphicThingInstanceRef().SetFaint(false);
		}
		else
		{
			pCharacterInstance->GetGraphicThingInstanceRef().InterceptOnceMotion(CRaceMotionData::NAME_DAMAGE_FLYING);
			pCharacterInstance->GetGraphicThingInstanceRef().PushOnceMotion(CRaceMotionData::NAME_STAND_UP);
			pCharacterInstance->GetGraphicThingInstanceRef().PushLoopMotion(CRaceMotionData::NAME_WAIT);
			pCharacterInstance->GetGraphicThingInstanceRef().SetFaint(true);
		}
	}

	return Py_BuildNone();
}

PyObject * chrSetMoveSpeed(PyObject* poSelf, PyObject* poArgs)
{
	//int iSpeed;
	//if (!PyTuple_GetInteger(poArgs, 0, &iSpeed))
	//	return Py_BadArgument();

	//CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	//if (!pCharacterInstance)
	//	return Py_BuildValue("i", 0);

	//pCharacterInstance->SetMoveSpeed(iSpeed);

	return Py_BuildNone();
}

PyObject * chrSetAttackSpeed(PyObject* poSelf, PyObject* poArgs)
{
	//int iSpeed;
	//if (!PyTuple_GetInteger(poArgs, 0, &iSpeed))
	//	return Py_BadArgument();

	//CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetSelectedInstancePtr();

	//if (!pCharacterInstance)
	//	return Py_BuildValue("i", 0);

	//pCharacterInstance->SetAttackSpeed(iSpeed);

	return Py_BuildNone();
}

PyObject * chrWeaponTraceSetTexture(PyObject* poSelf, PyObject* poArgs)
{
	char * szPathName;
	if (!PyTuple_GetString(poArgs, 0, &szPathName))
		return Py_BadArgument();

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		pInstance->GetGraphicThingInstanceRef().SetWeaponTraceTexture(szPathName);
	}
	return Py_BuildNone();
}

PyObject * chrWeaponTraceUseTexture(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		pInstance->GetGraphicThingInstanceRef().UseTextureWeaponTrace();
	}
	return Py_BuildNone();
}

PyObject * chrWeaponTraceUseAlpha(PyObject* poSelf, PyObject* poArgs)
{
	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		pInstance->GetGraphicThingInstanceRef().UseAlphaWeaponTrace();
	}
	return Py_BuildNone();
}

PyObject * chrMoveToDestPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BadArgument();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 1, &ix))
		return Py_BadArgument();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 2, &iy))
		return Py_BadArgument();

	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(iVID);
	if (!pCharacterInstance)
		return Py_BuildNone();

	pCharacterInstance->NEW_MoveToDestPixelPositionDirection(TPixelPosition(ix, iy, 0.0f));

	return Py_BuildNone();
}

PyObject * chrtestSetRideMan(PyObject* poSelf, PyObject* poArgs)
{
	int ix;
	if (!PyTuple_GetInteger(poArgs, 0, &ix))
		return Py_BadArgument();
	int iy;
	if (!PyTuple_GetInteger(poArgs, 1, &iy))
		return Py_BadArgument();
	int imount = 20030;
	PyTuple_GetInteger(poArgs, 2, &imount);

	CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().RegisterInstance(1);
	CInstanceBase::SCreateData kCreateData;
	ZeroMemory(&kCreateData, sizeof(kCreateData));
	kCreateData.m_bType			= CActorInstance::TYPE_PC;
	kCreateData.m_dwRace		= 0;
	kCreateData.m_dwArmor		= 0;
	kCreateData.m_dwHair		= 100;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	kCreateData.m_dwAcce		= 0;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	kCreateData.m_dwEmblemat = 0;
#endif
	kCreateData.m_dwMovSpd		= 100;
	kCreateData.m_dwAtkSpd		= 100;
	kCreateData.m_dwMountVnum	= imount;
	kCreateData.m_lPosX			= ix;
	kCreateData.m_lPosY			= iy;

	pCharacterInstance->Create(kCreateData);

	return Py_BuildNone();
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
PyObject * chrSetAcce(PyObject* poSelf, PyObject* poArgs)
{
	int dwAcce;
	if (!PyTuple_GetInteger(poArgs, 0, &dwAcce))
		return Py_BuildException();

	CInstanceBase * pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetAcce(dwAcce);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_COSTUME_EMBLEMAT
PyObject* chrSetEmblemat(PyObject* poSelf, PyObject* poArgs)
{
	int iRace;
	if (!PyTuple_GetInteger(poArgs, 0, &iRace))
		return Py_BuildException();

	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->SetEmblemat(iRace);
	return Py_BuildNone();
}

PyObject* chrChangeEmblemat(PyObject* poSelf, PyObject* poArgs)
{
	int iEmblemat;

	if (!PyTuple_GetInteger(poArgs, 0, &iEmblemat))
		return Py_BuildException();

	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetSelectedInstancePtr();
	if (!pkInst)
		return Py_BuildNone();

	pkInst->ChangeEmblemat(iEmblemat);
	return Py_BuildNone();
}
#endif

void initchr()
{
	VMProtectBeginUltra("initchr");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("DismountHorse"),				chrDismountHorse,					METH_VARARGS },
		{ VMProtectDecryptStringA("MountHorse"),					chrMountHorse,						METH_VARARGS },

		{ VMProtectDecryptStringA("Destroy"),					chrDestroy,							METH_VARARGS },
		{ VMProtectDecryptStringA("Update"),						chrUpdate,							METH_VARARGS },
		{ VMProtectDecryptStringA("Deform"),						chrDeform,							METH_VARARGS },
		{ VMProtectDecryptStringA("Render"),						chrRender,							METH_VARARGS },
		{ VMProtectDecryptStringA("RenderCollision"),			chrRenderCollision,					METH_VARARGS },

		// Functions For Python Code
		{ VMProtectDecryptStringA("CreateInstance"),				chrCreateInstance,					METH_VARARGS },
		{ VMProtectDecryptStringA("DeleteInstance"),				chrDeleteInstance,					METH_VARARGS },
		{ VMProtectDecryptStringA("DeleteInstanceByFade"),		chrDeleteInstanceByFade,			METH_VARARGS },
		{ VMProtectDecryptStringA("SelectInstance"),				chrSelectInstance,					METH_VARARGS },

		{ VMProtectDecryptStringA("HasInstance"),				chrHasInstance,						METH_VARARGS },
		{ VMProtectDecryptStringA("IsEnemy"),					chrIsEnemy,							METH_VARARGS },
		{ VMProtectDecryptStringA("IsNPC"),						chrIsNPC,							METH_VARARGS },
		{ VMProtectDecryptStringA("IsGameMaster"),				chrIsGameMaster,					METH_VARARGS },
		{ VMProtectDecryptStringA("IsPartyMember"),				chrIsPartyMember,					METH_VARARGS },

		{ VMProtectDecryptStringA("Select"),						chrSelect,							METH_VARARGS },
		{ VMProtectDecryptStringA("SetAddRenderMode"),			chrSetAddRenderMode,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetBlendRenderMode"),			chrSetBlendRenderMode,				METH_VARARGS },
		{ VMProtectDecryptStringA("Unselect"),					chrUnselect,						METH_VARARGS },

		{ VMProtectDecryptStringA("Hide"),						chrHide,							METH_VARARGS },
		{ VMProtectDecryptStringA("Show"),						chrShow,							METH_VARARGS },
		{ VMProtectDecryptStringA("Pick"),						chrPick,							METH_VARARGS },
		{ VMProtectDecryptStringA("PickAll"),					chrPickAll,							METH_VARARGS },

		{ VMProtectDecryptStringA("SetArmor"),					chrSetArmor,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetWeapon"),					chrSetWeapon,						METH_VARARGS },
		{ VMProtectDecryptStringA("ChangeShape"),				chrChangeShape,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetRace"),					chrSetRace,							METH_VARARGS },
		{ VMProtectDecryptStringA("SetHair"),					chrSetHair,							METH_VARARGS },
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		{ VMProtectDecryptStringA("SetAcce"),						chrSetAcce,							METH_VARARGS },
#endif
		{ VMProtectDecryptStringA("ChangeHair"),					chrChangeHair,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetVirtualID"),				chrSetVirtualID,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetNameString"),				chrSetNameString,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetInstanceType"),			chrSetInstanceType,					METH_VARARGS },

		{ VMProtectDecryptStringA("SetPixelPosition"),			chrSetPixelPosition,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetDirection"),				chrSetDirection,					METH_VARARGS },
		{ VMProtectDecryptStringA("Refresh"),					chrRefresh,							METH_VARARGS },
		{ VMProtectDecryptStringA("Revive"),						chrRevive,							METH_VARARGS },
		{ VMProtectDecryptStringA("Die"),						chrDie,								METH_VARARGS },

		{ VMProtectDecryptStringA("AttachEffectByID"),			chrAttachEffectByID,				METH_VARARGS },
		{ VMProtectDecryptStringA("AttachEffectByName"),			chrAttachEffectByName,				METH_VARARGS },

		{ VMProtectDecryptStringA("LookAt"),						chrLookAt,							METH_VARARGS },
		{ VMProtectDecryptStringA("SetMotionMode"),				chrSetMotionMode,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetLoopMotion"),				chrSetLoopMotion,					METH_VARARGS },
		{ VMProtectDecryptStringA("BlendLoopMotion"),			chrBlendLoopMotion,					METH_VARARGS },
		{ VMProtectDecryptStringA("PushOnceMotion"),				chrPushOnceMotion,					METH_VARARGS },
		{ VMProtectDecryptStringA("PushLoopMotion"),				chrPushLoopMotion,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetPixelPosition"),			chrGetPixelPosition,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetRotation"),				chrSetRotation,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetRotationAll"),				chrSetRotationAll,					METH_VARARGS },
		{ VMProtectDecryptStringA("BlendRotation"),				chrBlendRotation,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetRotation"),				chrGetRotation,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetRace"),					chrGetRace,							METH_VARARGS },
		{ VMProtectDecryptStringA("GetName"),					chrGetName,							METH_VARARGS },
		{ VMProtectDecryptStringA("GetNameByVID"),				chrGetNameByVID,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetGuildID"),					chrGetGuildID,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetProjectPosition"),			chrGetProjectPosition,				METH_VARARGS },

		{ VMProtectDecryptStringA("GetVirtualNumber"),			chrGetVirtualNumber,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetInstanceType"),			chrGetInstanceType,					METH_VARARGS },

		{ VMProtectDecryptStringA("GetBoundBoxOnlyXY"),			chrGetBoundBoxOnlyXY,				METH_VARARGS },

		{ VMProtectDecryptStringA("RaceToJob"),					chrRaceToJob,							METH_VARARGS },
		{ VMProtectDecryptStringA("RaceToSex"),					chrRaceToSex,							METH_VARARGS },

		// For Test
		{ VMProtectDecryptStringA("testGetPKData"),					chrtestGetPKData,					METH_VARARGS },
		{ VMProtectDecryptStringA("FaintTest"),						chrFaintTest,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetMoveSpeed"),					chrSetMoveSpeed,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetAttackSpeed"),					chrSetAttackSpeed,					METH_VARARGS },
		{ VMProtectDecryptStringA("WeaponTraceSetTexture"),			chrWeaponTraceSetTexture,			METH_VARARGS },
		{ VMProtectDecryptStringA("WeaponTraceUseAlpha"),			chrWeaponTraceUseAlpha,				METH_VARARGS },
		{ VMProtectDecryptStringA("WeaponTraceUseTexture"),			chrWeaponTraceUseTexture,			METH_VARARGS },
		{ VMProtectDecryptStringA("MoveToDestPosition"),				chrMoveToDestPosition,				METH_VARARGS },
		{ VMProtectDecryptStringA("testSetComboType"),				chrtestSetComboType,				METH_VARARGS },
		{ VMProtectDecryptStringA("testSetAddRenderMode"),			chrtestSetAddRenderMode,			METH_VARARGS },
		{ VMProtectDecryptStringA("testSetModulateRenderMode"),		chrtestSetModulateRenderMode,		METH_VARARGS },
		{ VMProtectDecryptStringA("testSetAddRenderModeRGB"),		chrtestSetAddRenderModeRGB,			METH_VARARGS },
		{ VMProtectDecryptStringA("testSetModulateRenderModeRGB"),	chrtestSetModulateRenderModeRGB,	METH_VARARGS },
		{ VMProtectDecryptStringA("testSetSpecularRenderMode"),		chrtestSetSpecularRenderMode,		METH_VARARGS },
		{ VMProtectDecryptStringA("testSetSpecularRenderMode2"),		chrtestSetSpecularRenderMode2,		METH_VARARGS },
		{ VMProtectDecryptStringA("testRestoreRenderMode"),			chrtestRestoreRenderMode,			METH_VARARGS },
		{ VMProtectDecryptStringA("testSetRideMan"),					chrtestSetRideMan,					METH_VARARGS },
#ifdef ENABLE_COSTUME_EMBLEMAT
		{ VMProtectDecryptStringA("SetEmble"),						chrSetEmblemat,							METH_VARARGS },
		{ VMProtectDecryptStringA("ChangeEmble"),					chrChangeEmblemat,						METH_VARARGS },
#endif
		{ NULL,								NULL,								NULL		 },
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("chr"), s_methods);

	// Length
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PLAYER_NAME_MAX_LEN"), PLAYER_NAME_MAX_LEN);

	// General
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_NONE"), CRaceMotionData::NAME_NONE);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SPAWN"), CRaceMotionData::NAME_SPAWN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_WAIT"), CRaceMotionData::NAME_WAIT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_WALK"), CRaceMotionData::NAME_WALK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_RUN"), CRaceMotionData::NAME_RUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CHANGE_WEAPON"), CRaceMotionData::NAME_CHANGE_WEAPON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DAMAGE"), CRaceMotionData::NAME_DAMAGE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DAMAGE_FLYING"), CRaceMotionData::NAME_DAMAGE_FLYING);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_STAND_UP"), CRaceMotionData::NAME_STAND_UP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DAMAGE_BACK"), CRaceMotionData::NAME_DAMAGE_BACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DAMAGE_FLYING_BACK"), CRaceMotionData::NAME_DAMAGE_FLYING_BACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_STAND_UP_BACK"), CRaceMotionData::NAME_STAND_UP_BACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DEAD"), CRaceMotionData::NAME_DEAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DEAD_BACK"), CRaceMotionData::NAME_DEAD_BACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_NORMAL_ATTACK"), CRaceMotionData::NAME_NORMAL_ATTACK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_1"), CRaceMotionData::NAME_COMBO_ATTACK_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_2"), CRaceMotionData::NAME_COMBO_ATTACK_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_3"), CRaceMotionData::NAME_COMBO_ATTACK_3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_4"), CRaceMotionData::NAME_COMBO_ATTACK_4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_5"), CRaceMotionData::NAME_COMBO_ATTACK_5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_6"), CRaceMotionData::NAME_COMBO_ATTACK_6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_7"), CRaceMotionData::NAME_COMBO_ATTACK_7);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_COMBO_ATTACK_8"), CRaceMotionData::NAME_COMBO_ATTACK_8);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_INTRO_WAIT"), CRaceMotionData::NAME_INTRO_WAIT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_INTRO_SELECTED"), CRaceMotionData::NAME_INTRO_SELECTED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_INTRO_NOT_SELECTED"), CRaceMotionData::NAME_INTRO_NOT_SELECTED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_THROW"), CRaceMotionData::NAME_FISHING_THROW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_WAIT"), CRaceMotionData::NAME_FISHING_WAIT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_STOP"), CRaceMotionData::NAME_FISHING_STOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_REACT"), CRaceMotionData::NAME_FISHING_REACT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_CATCH"), CRaceMotionData::NAME_FISHING_CATCH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FISHING_FAIL"), CRaceMotionData::NAME_FISHING_FAIL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_STOP"), CRaceMotionData::NAME_STOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SKILL"), CRaceMotionData::NAME_SKILL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CLAP"), CRaceMotionData::NAME_CLAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_1"), CRaceMotionData::NAME_DANCE_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_2"), CRaceMotionData::NAME_DANCE_2);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_3"), CRaceMotionData::NAME_DANCE_3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_4"), CRaceMotionData::NAME_DANCE_4);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_5"), CRaceMotionData::NAME_DANCE_5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DANCE_6"), CRaceMotionData::NAME_DANCE_6);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CONGRATULATION"), CRaceMotionData::NAME_CONGRATULATION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FORGIVE"), CRaceMotionData::NAME_FORGIVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_ANGRY"), CRaceMotionData::NAME_ANGRY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_ATTRACTIVE"), CRaceMotionData::NAME_ATTRACTIVE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SAD"), CRaceMotionData::NAME_SAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SHY"), CRaceMotionData::NAME_SHY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CHEERUP"), CRaceMotionData::NAME_CHEERUP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_BANTER"), CRaceMotionData::NAME_BANTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_JOY"), CRaceMotionData::NAME_JOY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CHEERS_1"), CRaceMotionData::NAME_CHEERS_1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_CHEERS_2"), CRaceMotionData::NAME_CHEERS_2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_KISS_WITH_WARRIOR"), CRaceMotionData::NAME_KISS_WITH_WARRIOR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_KISS_WITH_ASSASSIN"), CRaceMotionData::NAME_KISS_WITH_ASSASSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_KISS_WITH_SURA"), CRaceMotionData::NAME_KISS_WITH_SURA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_KISS_WITH_SHAMAN"), CRaceMotionData::NAME_KISS_WITH_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_KISS_WITH_WOLFMAN"), CRaceMotionData::NAME_KISS_WITH_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FRENCH_KISS_WITH_WARRIOR"), CRaceMotionData::NAME_FRENCH_KISS_WITH_WARRIOR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FRENCH_KISS_WITH_ASSASSIN"), CRaceMotionData::NAME_FRENCH_KISS_WITH_ASSASSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FRENCH_KISS_WITH_SURA"), CRaceMotionData::NAME_FRENCH_KISS_WITH_SURA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FRENCH_KISS_WITH_SHAMAN"), CRaceMotionData::NAME_FRENCH_KISS_WITH_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_FRENCH_KISS_WITH_WOLFMAN"), CRaceMotionData::NAME_FRENCH_KISS_WITH_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HIT_WITH_WARRIOR"), CRaceMotionData::NAME_SLAP_HIT_WITH_WARRIOR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HIT_WITH_ASSASSIN"), CRaceMotionData::NAME_SLAP_HIT_WITH_ASSASSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HIT_WITH_SURA"), CRaceMotionData::NAME_SLAP_HIT_WITH_SURA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HIT_WITH_SHAMAN"), CRaceMotionData::NAME_SLAP_HIT_WITH_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HIT_WITH_WOLFMAN"), CRaceMotionData::NAME_SLAP_HIT_WITH_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HURT_WITH_WARRIOR"), CRaceMotionData::NAME_SLAP_HURT_WITH_WARRIOR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HURT_WITH_ASSASSIN"), CRaceMotionData::NAME_SLAP_HURT_WITH_ASSASSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HURT_WITH_SURA"), CRaceMotionData::NAME_SLAP_HURT_WITH_SURA);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HURT_WITH_SHAMAN"), CRaceMotionData::NAME_SLAP_HURT_WITH_SHAMAN);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_SLAP_HURT_WITH_WOLFMAN"), CRaceMotionData::NAME_SLAP_HURT_WITH_WOLFMAN);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_DIG"), CRaceMotionData::NAME_DIG);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_RESERVED"), CRaceMotionData::MODE_RESERVED);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_GENERAL"), CRaceMotionData::MODE_GENERAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_ONEHAND_SWORD"), CRaceMotionData::MODE_ONEHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_TWOHAND_SWORD"), CRaceMotionData::MODE_TWOHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_DUALHAND_SWORD"), CRaceMotionData::MODE_DUALHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_BOW"), CRaceMotionData::MODE_BOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_FAN"), CRaceMotionData::MODE_FAN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_BELL"), CRaceMotionData::MODE_BELL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_FISHING"), CRaceMotionData::MODE_FISHING);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE"), CRaceMotionData::MODE_HORSE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_ONEHAND_SWORD"), CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_TWOHAND_SWORD"), CRaceMotionData::MODE_HORSE_TWOHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_DUALHAND_SWORD"), CRaceMotionData::MODE_HORSE_DUALHAND_SWORD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_BOW"), CRaceMotionData::MODE_HORSE_BOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_FAN"), CRaceMotionData::MODE_HORSE_FAN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_BELL"), CRaceMotionData::MODE_HORSE_BELL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_WEDDING_DRESS"), CRaceMotionData::MODE_WEDDING_DRESS);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_CLAW"), CRaceMotionData::MODE_CLAW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("MOTION_MODE_HORSE_CLAW"), CRaceMotionData::MODE_HORSE_CLAW);
#endif

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_NORTH"), CInstanceBase::DIR_NORTH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_NORTHEAST"), CInstanceBase::DIR_NORTHEAST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_EAST"), CInstanceBase::DIR_EAST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_SOUTHEAST"), CInstanceBase::DIR_SOUTHEAST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_SOUTH"), CInstanceBase::DIR_SOUTH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_SOUTHWEST"), CInstanceBase::DIR_SOUTHWEST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_WEST"), CInstanceBase::DIR_WEST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DIR_NORTHWEST"), CInstanceBase::DIR_NORTHWEST);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_PLAYER"), CActorInstance::TYPE_PC);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_NPC"), CActorInstance::TYPE_NPC);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_ENEMY"), CActorInstance::TYPE_ENEMY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_BUILDING"), CActorInstance::TYPE_BUILDING);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_OBJECT"), CActorInstance::TYPE_OBJECT);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_WEAPON"), CRaceData::PART_WEAPON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_HEAD"), CRaceData::PART_HEAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_WEAPON_LEFT"), CRaceData::PART_WEAPON_LEFT);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_ACCE"), CRaceData::PART_ACCE);
#endif
#ifdef ENABLE_AFFECT_POLYMORPH_REMOVE
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_POLYMORPH"), CInstanceBase::NEW_AFFECT_POLYMORPH);
#endif
	/////
#ifdef __AUTO_QUQUE_ATTACK__
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("INSTANCE_TYPE_STONE"), CActorInstance::TYPE_STONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_AUTO_METIN_FARM"), CInstanceBase::NEW_AFFECT_AUTO_METIN_FARM);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_POISON"), CInstanceBase::AFFECT_POISON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_SLOW"), CInstanceBase::AFFECT_SLOW);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_STUN"), CInstanceBase::AFFECT_STUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_MOV_SPEED_POTION"), CInstanceBase::AFFECT_MOV_SPEED_POTION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_ATT_SPEED_POTION"), CInstanceBase::AFFECT_ATT_SPEED_POTION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_FISH_MIND"), CInstanceBase::AFFECT_FISH_MIND);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_JEONGWI"), CInstanceBase::AFFECT_JEONGWI);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_GEOMGYEONG"), CInstanceBase::AFFECT_GEOMGYEONG);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_CHEONGEUN"), CInstanceBase::AFFECT_CHEONGEUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_GYEONGGONG"), CInstanceBase::AFFECT_GYEONGGONG);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_EUNHYEONG"), CInstanceBase::AFFECT_EUNHYEONG);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_GWIGEOM"), CInstanceBase::AFFECT_GWIGEOM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_GONGPO"), CInstanceBase::AFFECT_GONGPO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_JUMAGAP"), CInstanceBase::AFFECT_JUMAGAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_HOSIN"), CInstanceBase::AFFECT_HOSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_BOHO"), CInstanceBase::AFFECT_BOHO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_KWAESOK"), CInstanceBase::AFFECT_KWAESOK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_HEUKSIN"), CInstanceBase::AFFECT_HEUKSIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_MUYEONG"), CInstanceBase::AFFECT_MUYEONG);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_GICHEON"), CInstanceBase::AFFECT_GICHEON);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_JEUNGRYEOK"), CInstanceBase::AFFECT_JEUNGRYEOK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_PABEOP"), CInstanceBase::AFFECT_PABEOP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_FALLEN_CHEONGEUN"), CInstanceBase::AFFECT_FALLEN_CHEONGEUN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_CHINA_FIREWORK"), CInstanceBase::AFFECT_CHINA_FIREWORK);
#ifdef ENABLE_WOLFMAN_CHARACTER
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_BLEEDING"), CInstanceBase::AFFECT_BLEEDING);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_RED_POSSESSION"), CInstanceBase::AFFECT_RED_POSSESSION);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_BLUE_POSSESSION"), CInstanceBase::AFFECT_BLUE_POSSESSION);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_MALL"), CInstanceBase::NEW_AFFECT_MALL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_NO_DEATH_PENALTY"), CInstanceBase::NEW_AFFECT_NO_DEATH_PENALTY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILL_BOOK_BONUS"), CInstanceBase::NEW_AFFECT_SKILL_BOOK_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILL_BOOK_NO_DELAY"), CInstanceBase::NEW_AFFECT_SKILL_BOOK_NO_DELAY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_PAS_BOOK_BONUS"), CInstanceBase::NEW_AFFECT_SKILE_PAS_BOOK_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_PAS_NO_BOOK_DELAY"), CInstanceBase::NEW_AFFECT_SKILE_PAS_NO_BOOK_DELAY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_PET_BOOK_BONUS"), CInstanceBase::NEW_AFFECT_SKILE_PET_BOOK_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_PET_NO_BOOK_DELAY"), CInstanceBase::NEW_AFFECT_SKILE_PET_NO_BOOK_DELAY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_BUFF_BOOK_BONUS"), CInstanceBase::NEW_AFFECT_SKILE_BUFF_BOOK_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SKILE_BUFF_NO_BOOK_DELAY"), CInstanceBase::NEW_AFFECT_SKILE_BUFF_NO_BOOK_DELAY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_EXP_BONUS"), CInstanceBase::NEW_AFFECT_EXP_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_EXP_BONUS_EURO_FREE"), CInstanceBase::NEW_AFFECT_EXP_BONUS_EURO_FREE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15"), CInstanceBase::NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_ITEM_BONUS"), CInstanceBase::NEW_AFFECT_ITEM_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_SAFEBOX"), CInstanceBase::NEW_AFFECT_SAFEBOX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_AUTOLOOT"), CInstanceBase::NEW_AFFECT_AUTOLOOT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_FISH_MIND"), CInstanceBase::NEW_AFFECT_FISH_MIND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_MARRIAGE_FAST"), CInstanceBase::NEW_AFFECT_MARRIAGE_FAST);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_GOLD_BONUS"), CInstanceBase::NEW_AFFECT_GOLD_BONUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_PICKUP"), CInstanceBase::AFFECT_PICKUP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_AUTO_HP_RECOVERY"), CInstanceBase::NEW_AFFECT_AUTO_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_AUTO_SP_RECOVERY"), CInstanceBase::NEW_AFFECT_AUTO_SP_RECOVERY);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_DRAGON_SOUL_QUALIFIED"), CInstanceBase::NEW_AFFECT_DRAGON_SOUL_QUALIFIED);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_DRAGON_SOUL_DECK1"), CInstanceBase::NEW_AFFECT_DRAGON_SOUL_DECK1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_DRAGON_SOUL_DECK2"), CInstanceBase::NEW_AFFECT_DRAGON_SOUL_DECK2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_BLEND"), CInstanceBase::NEW_AFFECT_BLEND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_BLEND_FISH"), CInstanceBase::AFFECT_BLEND_FISH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_RUNE_DECK5"), CInstanceBase::AFFECT_RUNE_DECK5);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("AFFECT_RUNE_DECK9"), CInstanceBase::AFFECT_RUNE_DECK9);
#ifdef ENABLE_EXTENDED_BLEND
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_MALL_EX"), CInstanceBase::NEW_AFFECT_MALL_EX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_WATER"), CInstanceBase::NEW_AFFECT_WATER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_BLEND_EX"), CInstanceBase::NEW_AFFECT_BLEND_EX);
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_EMBLEMAT"), CRaceData::PART_EMBLEMAT);
#endif
	#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("NEW_AFFECT_PREMIUM_PRIVATE_SHOP"), CInstanceBase::NEW_AFFECT_PREMIUM_PRIVATE_SHOP);
#endif

	VMProtectEnd();

}
//martysama0134's ceqyqttoaf71vasf9t71218
