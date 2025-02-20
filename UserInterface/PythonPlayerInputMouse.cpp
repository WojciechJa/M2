#include "StdAfx.h"
#include "PythonPlayer.h"
#include "PythonApplication.h"

#include "../eterlib/Camera.h"

void CPythonPlayer::NEW_SetMouseMoveState(int eMBS)
{
	if (MBS_PRESS==eMBS)
	{
		NEW_MoveToMouseScreenDirection();

		m_isDirMov=true;
	}
	else if (MBS_CLICK==eMBS)
	{
		NEW_Stop();

		m_isDirMov=false;
	}
}

bool CPythonPlayer::NEW_MoveToMouseScreenDirection()
{
	UI::CWindowManager& rkWndMgr=UI::CWindowManager::Instance();

	long lMouseX;
	long lMouseY;
	rkWndMgr.GetMousePosition(lMouseX, lMouseY);

	long lScrWidth = rkWndMgr.GetScreenWidth();
	long lScrHeight = rkWndMgr.GetScreenHeight();
	float fMouseX=lMouseX/float(lScrWidth);
	float fMouseY=lMouseY/float(lScrHeight);

	float fDirRot;
	NEW_GetMouseDirRotation(fMouseX, fMouseY, &fDirRot);

	return NEW_MoveToDirection(fDirRot);
}

void CPythonPlayer::NEW_SetMouseCameraState(int eMBS)
{
	CPythonApplication & rkApp = CPythonApplication::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CCamera* pkCmrCur=CCameraManager::Instance().GetCurrentCamera();

	if (pkCmrCur)
	{
		if (MBS_PRESS==eMBS)
		{
			UI::CWindowManager& rkWndMgr=UI::CWindowManager::Instance();

			long lMouseX;
			long lMouseY;
			rkWndMgr.GetMousePosition(lMouseX, lMouseY);

			pkCmrCur->BeginDrag(lMouseX, lMouseY);

			if ( !rkBG.IsMapReady() )
				return;

			rkApp.SetCursorNum(CPythonApplication::CAMERA_ROTATE);
			if ( CPythonApplication::CURSOR_MODE_HARDWARE == rkApp.GetCursorMode())
				rkApp.SetCursorVisible(FALSE, true);

		}
		else if (MBS_CLICK==eMBS)
		{
			bool isCameraDrag=pkCmrCur->EndDrag();

			if ( !rkBG.IsMapReady() )
				return;

			rkApp.SetCursorNum(CPythonApplication::NORMAL);
			if ( CPythonApplication::CURSOR_MODE_HARDWARE == rkApp.GetCursorMode())
				rkApp.SetCursorVisible(TRUE);

			if (!isCameraDrag)
			{
#ifdef __AUTO_QUQUE_ATTACK__
				const bool isClickingAutoFarm = CPythonApplication::Instance().IsPressed(DIK_LSHIFT);
				if (!isClickingAutoFarm)
					__ChangeTargetToPickedInstance();
				CInstanceBase* pkInstPicked;
				if (__GetPickedActorPtr(&pkInstPicked))
				{
					if (isClickingAutoFarm && pkInstPicked->IsStone())
					{
						AutoFarmQuqueSet(true, pkInstPicked->GetVirtualID());
						return;
					}
					OpenCharacterMenu(pkInstPicked->GetVirtualID());
				}
#else
				__ChangeTargetToPickedInstance();
				CInstanceBase* pkInstPicked;
				if (__GetPickedActorPtr(&pkInstPicked))
				{
					OpenCharacterMenu(pkInstPicked->GetVirtualID());
				}
#endif
			}
		}
	}
}

void CPythonPlayer::NEW_SetMouseSmartState(int eMBS, bool isAuto)
{
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain) return;

	// PrivateShop
	if (IsOpenPrivateShop())
	{
		m_isSmtMov=false;
		return;
	}

	// Emotion
	if (__IsProcessingEmotion())
	{
		return;
	}

	if (pkInstMain->IsSleep())
	{
		return;
	}

	if (MBS_PRESS==eMBS)
	{
		m_isSmtMov=true;

		__OnPressSmart(*pkInstMain, isAuto);
	}
	else if (MBS_CLICK==eMBS)
	{
		m_isSmtMov=false;

		__OnClickSmart(*pkInstMain, isAuto);
	}
}

void CPythonPlayer::__OnPressSmart(CInstanceBase& rkInstMain, bool isAuto)
{
	DWORD dwPickedItemID;
	DWORD dwPickedActorID;
	TPixelPosition kPPosPickedGround;

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	DWORD dwPickedShopID;
	bool isPickedPrivateShopID = __GetPickedPrivateShopID(&dwPickedShopID);
#endif

	bool isPickedItemID=__GetPickedItemID(&dwPickedItemID);
	bool isPickedActorID=__GetPickedActorID(&dwPickedActorID);
	bool isPickedGroundPos=__GetPickedGroundPos(&kPPosPickedGround);

	if (isPickedItemID)
	{
		__OnPressItem(rkInstMain, dwPickedItemID);
	}
	else if (isPickedActorID && dwPickedActorID != GetMainCharacterIndex())
	{
		__OnPressActor(rkInstMain, dwPickedActorID, isAuto);
	}
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	else if (isPickedPrivateShopID)
	{
		__OnPressPrivateShop(rkInstMain, dwPickedShopID);
	}
#endif
	else if (isPickedGroundPos)
	{
		__OnPressGround(rkInstMain, kPPosPickedGround);
	}
	else
	{
		__OnPressScreen(rkInstMain);
	}
}

void CPythonPlayer::__OnClickSmart(CInstanceBase& rkInstMain, bool isAuto)
{
	DWORD dwPickedItemID;
	DWORD dwPickedActorID;
	TPixelPosition kPPosPickedGround;
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	DWORD dwPickedShopID;
#endif


	if (__GetPickedItemID(&dwPickedItemID))
	{
		__OnClickItem(rkInstMain, dwPickedItemID);
	}
	else if (__GetPickedActorID(&dwPickedActorID))
	{
		__OnClickActor(rkInstMain, dwPickedActorID, isAuto);
	}
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	else if (__GetPickedPrivateShopID(&dwPickedShopID))
	{
		__OnClickPrivateShop(dwPickedShopID);
	}
#endif
	else if (__GetPickedGroundPos(&kPPosPickedGround))
	{
		__OnClickGround(rkInstMain, kPPosPickedGround);
	}
	else
	{
		rkInstMain.NEW_Stop();
	}
}

void CPythonPlayer::__ShowPickedEffect(const TPixelPosition& c_rkPPosPickedGround)
{
#ifdef __MOVIE_MODE__
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain) return;

	if (pkInstMain->IsMovieMode())
		return;
#endif
	NEW_ShowEffect(EFFECT_PICK, c_rkPPosPickedGround);
}

bool CPythonPlayer::NEW_SetMouseFunc(int eMBT, int eMBF)
{
	if (eMBT>=MBT_NUM)
		return false;

	m_aeMBFButton[eMBT]=eMBF;

	return true;
}

int CPythonPlayer::NEW_GetMouseFunc(int eMBT)
{
	if (eMBT>=MBT_NUM)
		return false;

	return m_aeMBFButton[eMBT];
}

void CPythonPlayer::SetQuickCameraMode(bool isEnable)
{
	if (isEnable)
	{
	}
	else
	{
		NEW_SetMouseCameraState(MBS_CLICK);
	}
}

#ifdef __AUTO_QUQUE_ATTACK__
DWORD CPythonPlayer::GetAutoFarmTarget()
{
	return m_vecQuqueAutoAttack.size() ? m_vecQuqueAutoAttack[0] : 0;
}
bool CPythonPlayer::AutoFarmQuqueSet(const bool isAdd, const DWORD dwVirtualID)
{
	if (isAdd)
	{
		if (std::find(m_vecQuqueAutoAttack.begin(), m_vecQuqueAutoAttack.end(), dwVirtualID) != m_vecQuqueAutoAttack.end())
		{
			AutoFarmQuqueSet(false, dwVirtualID);
			return false;
		}
		else if (m_vecQuqueAutoAttack.size() + 1 > bTotalQuqueAutoAttack)
		{
			CPythonChat::Instance().AppendChat(1, "Kolejka jest pe�na.");
			return false;
		}

		m_vecQuqueAutoAttack.emplace_back(dwVirtualID);
	}
	else
	{
		const auto it = std::find(m_vecQuqueAutoAttack.begin(), m_vecQuqueAutoAttack.end(), dwVirtualID);
		if (it == m_vecQuqueAutoAttack.end())
			return false;
		m_vecQuqueAutoAttack.erase(it);
	}
	CInstanceBase* pkInstTarget = NEW_FindActorPtr(dwVirtualID);
	if (pkInstTarget)
	{
		pkInstTarget->SetAutoFarmAffect(isAdd);
		PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_AddToQueue", Py_BuildValue("(ii)", dwVirtualID, isAdd));
		if (!isAdd)
		{
			if (__GetTargetVID() == dwVirtualID)
			{
				NEW_Stop();
				__ClearAutoAttackTargetActorID();
				__ClearTarget();
				__ClearReservedAction();
			}
		}
	}
	return true;
}
void CPythonPlayer::AutoFarmLoop()
{
	if (!m_vecQuqueAutoAttack.size())
		return;

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		if (pkInstMain->IsAttacking() || pkInstMain->IsWalking())
			return;
		const DWORD dwVirtualID = GetAutoFarmTarget();
		if (dwVirtualID != 0)
		{
			CInstanceBase* pkInstVictim = __GetTargetActorPtr();
			if (!pkInstVictim || pkInstVictim->IsDead())
			{
				pkInstVictim = NEW_FindActorPtr(dwVirtualID);
				if (!pkInstVictim || pkInstVictim->IsDead() || !pkInstVictim->IsStone())
				{
					AutoFarmQuqueSet(false, dwVirtualID);
					return;
				}
				SetTarget(pkInstVictim->GetVirtualID());
				__OnPressActor(*pkInstMain, m_dwTargetVID, true);
				OpenCharacterMenu(pkInstVictim->GetVirtualID());
			}
			else
			{
				if (pkInstVictim->GetVirtualID() == dwVirtualID)
				{
					if (pkInstMain->IsWaiting())
					{
						SetTarget(pkInstVictim->GetVirtualID());
						__OnPressActor(*pkInstMain, m_dwTargetVID, true);
						OpenCharacterMenu(pkInstVictim->GetVirtualID());
					}
					__ReserveProcess_ClickActor();
				}

				else
				{
					NEW_Stop();
					__ClearAutoAttackTargetActorID();
					__ClearTarget();
					__ClearReservedAction();
				}
			}
		}
	}
}
#endif

bool CPythonPlayer::NEW_SetMouseState(int eMBT, int eMBS)
{
	if (eMBT>=MBT_NUM)
		return false;

	int eMBF=m_aeMBFButton[eMBT];
	switch (eMBF)
	{
		case MBF_MOVE:
			if (__CanMove())
				NEW_SetMouseMoveState(eMBS);
			break;
		case MBF_SMART:
			if (CPythonApplication::Instance().IsPressed(DIK_LCONTROL) || CPythonApplication::Instance().IsPressed(DIK_RCONTROL))
			{
				NEW_Attack();
			}
			else
			{
				NEW_SetMouseSmartState(eMBS, false);
			}
			break;
		case MBF_CAMERA:
			NEW_SetMouseCameraState(eMBS);
			break;
		case MBF_AUTO:
			NEW_SetMouseSmartState(eMBS, true);
			break;
		case MBF_ATTACK:
			NEW_Attack();
			break;
		case MBF_SKILL:
			if (CPythonApplication::Instance().IsPressed(DIK_LCONTROL))
			{
				NEW_SetMouseCameraState(eMBS);
			}
			else
			{
				if (MBS_PRESS == eMBS)
				{
 					__ChangeTargetToPickedInstance();
					__UseCurrentSkill();
				}
			}
			break;
	}

	return true;
}

void CPythonPlayer::NEW_SetMouseMiddleButtonState(int eMBState)
{
	NEW_SetMouseCameraState(eMBState);
}

void CPythonPlayer::NEW_RefreshMouseWalkingDirection()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain) return;

	switch (m_eReservedMode)
	{
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
	case MODE_CLICK_PRIVATE_SHOP:
	{
		CPythonPrivateShop& rkPrivateShop = CPythonPrivateShop::Instance();

		TPixelPosition kPPosPickedPrivateShop;
		if (rkPrivateShop.GetPrivateShopPosition(m_dwPSIDReserved, &kPPosPickedPrivateShop))
		{
			if (pkInstMain->NEW_GetDistanceFromDestPixelPosition(kPPosPickedPrivateShop) < 500.0f)
			{
				CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();

				TPixelPosition kPPosCur;
				pkInstMain->NEW_GetPixelPosition(&kPPosCur);

				float fCurRot = pkInstMain->GetRotation();
				rkNetStream.SendCharacterStatePacket(kPPosCur, fCurRot, CInstanceBase::FUNC_WAIT, 0);
				SendClickPrivateShopPacket(m_dwPSIDReserved);

				pkInstMain->NEW_Stop();

				__ClearReservedAction();
			}
			else
			{
				pkInstMain->NEW_MoveToDestPixelPositionDirection(kPPosPickedPrivateShop);
			}
		}
		else
		{
			__ClearReservedAction();
		}

		break;
	}
#endif

		case MODE_CLICK_ITEM:
		{
			CPythonItem& rkIT=CPythonItem::Instance();

			TPixelPosition kPPosPickedItem;
			if (rkIT.GetGroundItemPosition(m_dwIIDReserved, &kPPosPickedItem))
			{
				if (pkInstMain->NEW_GetDistanceFromDestPixelPosition(kPPosPickedItem)<20.0f)
				{
					CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();

					TPixelPosition kPPosCur;
					pkInstMain->NEW_GetPixelPosition(&kPPosCur);

					float fCurRot=pkInstMain->GetRotation();
					rkNetStream.SendCharacterStatePacket(kPPosCur,  fCurRot, CInstanceBase::FUNC_WAIT, 0);
					SendClickItemPacket(m_dwIIDReserved);

					pkInstMain->NEW_Stop();

					__ClearReservedAction();
				}
				else
				{
					pkInstMain->NEW_MoveToDestPixelPositionDirection(kPPosPickedItem);
				}
			}
			else
			{
				__ClearReservedAction();
			}

			break;
		}

		case MODE_CLICK_ACTOR:
		{
			__ReserveProcess_ClickActor();
			break;
		}

		case MODE_CLICK_POSITION:
		{
			if (!pkInstMain->isLock())
			if (NEW_IsEmptyReservedDelayTime(CPythonApplication::Instance().GetGlobalElapsedTime()))
			{
				pkInstMain->NEW_MoveToDestPixelPositionDirection(m_kPPosReserved);
				__ClearReservedAction();
			}
			break;
		}

		case MODE_USE_SKILL:
		{
			CInstanceBase* pkInstReserved=NEW_FindActorPtr(m_dwVIDReserved);
			if (pkInstReserved)
			{
				float fDistance=pkInstMain->GetDistance(pkInstReserved);

				extern bool IS_HUGE_RACE(unsigned int vnum);
				if (IS_HUGE_RACE(pkInstReserved->GetRace()))
				{
					fDistance -= 200.0f;
				}

				if (fDistance < float(m_dwSkillRangeReserved))
				{
					SetTarget(m_dwVIDReserved);
					if (__UseSkill(m_dwSkillSlotIndexReserved))
						__ClearReservedAction();
				}
				else
				{
					pkInstMain->NEW_MoveToDestInstanceDirection(*pkInstReserved);
				}
			}
			else
			{
				__ClearReservedAction();
			}
			break;
		}
	}

	if (m_isSmtMov)
		__OnPressSmart(*pkInstMain, false);

	if (m_isDirMov)
		NEW_MoveToMouseScreenDirection();

	if (m_isDirKey)
		NEW_SetMultiDirKeyState(m_isLeft, m_isRight, m_isUp, m_isDown);

	if (m_isAtkKey)
		NEW_Attack();

	m_iComboOld=pkInstMain->GetComboIndex();
}

BOOL CPythonPlayer::__IsRightButtonSkillMode()
{
	return MBF_SKILL == m_aeMBFButton[MBT_RIGHT];
}
//martysama0134's ceqyqttoaf71vasf9t71218
