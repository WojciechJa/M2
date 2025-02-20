#include "StdAfx.h"
#include "PythonSystem.h"

PyObject * systemGetWidth(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject * systemGetHeight(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject * systemSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject * systemDestroyInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject * systemReserveResource(PyObject* poSelf, PyObject* poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject * systemisInterfaceConfig(PyObject* poSelf, PyObject* poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig();
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject * systemSaveWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject * systemGetWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus & c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
								  c_rWindowStatus.isMinimized,
								  c_rWindowStatus.ixPosition,
								  c_rWindowStatus.iyPosition,
								  c_rWindowStatus.iHeight);
}

PyObject * systemGetConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii",  iRes,
									  iFrequency,
									  tmp->is_software_cursor,
									  tmp->is_object_culling,
									  tmp->music_volume,
									  tmp->voice_volume,
									  tmp->gamma,
									  tmp->iDistance);
}

PyObject * systemSetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char * szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject * systemisSaveID(PyObject * poSelf, PyObject * poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject * systemGetSaveID(PyObject * poSelf, PyObject * poArgs)
{
	const char * c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject * systemGetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject * systemGetSoundVolume(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

PyObject * systemSetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject * systemSetSoundVolumef(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

PyObject * systemIsSoftwareCursor(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject * systemSetViewChatFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsViewChat(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject * systemSetAlwaysShowNameFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowDamageFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemSetShowSalesTextFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

PyObject * systemIsAlwaysShowName(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject * systemIsShowDamage(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject * systemIsShowSalesText(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject * systemSetConfig(PyObject * poSelf, PyObject * poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD *) &width, (DWORD *) &height, (DWORD *) &bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index,frequency_index, (DWORD *) &frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width				= width;
	tmp.height				= height;
	tmp.bpp					= bpp;
	tmp.frequency			= frequency;
	tmp.is_software_cursor	= software_cursor ? true : false;
	tmp.is_object_culling	= object_culling ? true : false;
	tmp.music_volume		= (char) music_volume;
	tmp.voice_volume		= (char) voice_volume;
	tmp.gamma				= gamma;
	tmp.iDistance			= distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject * systemApplyConfig(PyObject * poSelf, PyObject * poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject * systemSaveConfig(PyObject * poSelf, PyObject * poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject * systemGetResolutionCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject * systemGetFrequencyCount(PyObject * poSelf, PyObject * poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject * systemGetResolution(PyObject * poSelf, PyObject * poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject * systemGetCurrentResolution(PyObject * poSelf, PyObject *poArgs)
{
	CPythonSystem::TConfig *tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject * systemGetFrequency(PyObject * poSelf, PyObject * poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject * systemGetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject * systemSetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}

#ifdef ENABLE_DOG_MODE
PyObject* systemSetDogMode(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetDogMode(iFlag);

	return Py_BuildNone();
}

PyObject* systemGetDogMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetDogMode());
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
PyObject* systemSetShowAutoBuffMode(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowAutoBuffMode(iFlag);

	return Py_BuildNone();
}

PyObject* systemGetShowAutoBuffMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShowAutoBuffMode());
}
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
PyObject* systemSetShowPetMode(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowPetMode(iFlag);

	return Py_BuildNone();
}

PyObject* systemGetShowPetMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShowPetMode());
}
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
PyObject* systemSetShowMountMode(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMountMode(iFlag);

	return Py_BuildNone();
}

PyObject* systemGetShowMountMode(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShowMountMode());
}
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
PyObject* systemSetShowOfflineShopRange(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowOfflineShopRange(iFlag);

	return Py_BuildNone();
}

PyObject* systemGetShowOfflineShopRange(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShowOfflineShopRange());
}
#endif

#if defined(ENABLE_FOV_OPTION)
PyObject* systemSetFOV(PyObject* poSelf, PyObject* poArgs)
{
	float fValue;
	if (!PyTuple_GetFloat(poArgs, 0, &fValue))
		return Py_BuildException();

	CPythonSystem::Instance().SetFOV(fValue);
	return Py_BuildNone();
}

PyObject* systemGetFOV(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetFOV());
}
#endif

PyObject* SetEmoji(PyObject* poSelf, PyObject* poArgs)
{
	char* path;
	if (!PyTuple_GetString(poArgs, 0, &path))
		return Py_BuildException();

	char buf[MAX_PATH+1];
	sprintf(buf, "|E%s|e", path);

	return Py_BuildValue("s", buf);
}

void initemojiSystem()
{
	VMProtectBeginUltra("initemojiSystem");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("AppendEmoji"),			SetEmoji,			METH_VARARGS },

		{ NULL,						NULL,				NULL },
	};
	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("emoji"), s_methods);
	VMProtectEnd();
}

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
PyObject* systemGetPrivateShopViewDistance(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetPrivateShopViewDistance());
}

PyObject* systemSetPrivateShopViewDistance(PyObject* poSelf, PyObject* poArgs)
{
	float fDistance;

	if (!PyTuple_GetFloat(poArgs, 0, &fDistance))
		return Py_BuildException();

	CPythonSystem::Instance().SetPrivateShopViewDistance(fDistance);

	return Py_BuildNone();
}
#endif

void initsystemSetting()
{
	VMProtectBeginUltra("initsystemSetting");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("GetWidth"),					systemGetWidth,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetHeight"),					systemGetHeight,				METH_VARARGS },

		{ VMProtectDecryptStringA("SetInterfaceHandler"),		systemSetInterfaceHandler,		METH_VARARGS },
		{ VMProtectDecryptStringA("DestroyInterfaceHandler"),	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ VMProtectDecryptStringA("ReserveResource"),			systemReserveResource,			METH_VARARGS },

		{ VMProtectDecryptStringA("isInterfaceConfig"),			systemisInterfaceConfig,		METH_VARARGS },
		{ VMProtectDecryptStringA("SaveWindowStatus"),			systemSaveWindowStatus,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetWindowStatus"),			systemGetWindowStatus,			METH_VARARGS },

		{ VMProtectDecryptStringA("GetResolutionCount"),			systemGetResolutionCount,		METH_VARARGS },
		{ VMProtectDecryptStringA("GetFrequencyCount"),			systemGetFrequencyCount,		METH_VARARGS },

		{ VMProtectDecryptStringA("GetCurrentResolution"),		systemGetCurrentResolution,		METH_VARARGS },

		{ VMProtectDecryptStringA("GetResolution"),				systemGetResolution,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetFrequency"),				systemGetFrequency,				METH_VARARGS },

		{ VMProtectDecryptStringA("ApplyConfig"),				systemApplyConfig,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetConfig"),					systemSetConfig,				METH_VARARGS },
		{ VMProtectDecryptStringA("SaveConfig"),					systemSaveConfig,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetConfig"),					systemGetConfig,				METH_VARARGS },

		{ VMProtectDecryptStringA("SetSaveID"),					systemSetSaveID,				METH_VARARGS },
		{ VMProtectDecryptStringA("isSaveID"),					systemisSaveID,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetSaveID"),					systemGetSaveID,				METH_VARARGS },

		{ VMProtectDecryptStringA("GetMusicVolume"),				systemGetMusicVolume,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetSoundVolume"),				systemGetSoundVolume,			METH_VARARGS },

		{ VMProtectDecryptStringA("SetMusicVolume"),				systemSetMusicVolume,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetSoundVolumef"),			systemSetSoundVolumef,			METH_VARARGS },
		{ VMProtectDecryptStringA("IsSoftwareCursor"),			systemIsSoftwareCursor,			METH_VARARGS },

		{ VMProtectDecryptStringA("SetViewChatFlag"),			systemSetViewChatFlag,			METH_VARARGS },
		{ VMProtectDecryptStringA("IsViewChat"),					systemIsViewChat,				METH_VARARGS },

		{ VMProtectDecryptStringA("SetAlwaysShowNameFlag"),		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ VMProtectDecryptStringA("IsAlwaysShowName"),			systemIsAlwaysShowName,			METH_VARARGS },

		{ VMProtectDecryptStringA("SetShowDamageFlag"),			systemSetShowDamageFlag,		METH_VARARGS },
		{ VMProtectDecryptStringA("IsShowDamage"),				systemIsShowDamage,				METH_VARARGS },

		{ VMProtectDecryptStringA("SetShowSalesTextFlag"),		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ VMProtectDecryptStringA("IsShowSalesText"),			systemIsShowSalesText,			METH_VARARGS },

		{ VMProtectDecryptStringA("GetShadowLevel"),				systemGetShadowLevel,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetShadowLevel"),				systemSetShadowLevel,			METH_VARARGS },
#ifdef ENABLE_DOG_MODE
		{ VMProtectDecryptStringA("SetDogMode"),					systemSetDogMode,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetDogMode"),					systemGetDogMode,				METH_VARARGS },
#endif

#if defined(ENABLE_FOV_OPTION)
		{ VMProtectDecryptStringA("SetFOV"), 					systemSetFOV, 					METH_VARARGS },
		{ VMProtectDecryptStringA("GetFOV"), 					systemGetFOV, 					METH_VARARGS },
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		{ VMProtectDecryptStringA("SetShowAutoBuffMode"),		systemSetShowAutoBuffMode,		METH_VARARGS },
		{ VMProtectDecryptStringA("GetShowAutoBuffMode"),		systemGetShowAutoBuffMode,		METH_VARARGS },
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
		{ VMProtectDecryptStringA("SetShowPetMode"),				systemSetShowPetMode,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetShowPetMode"),				systemGetShowPetMode,			METH_VARARGS },
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		{ VMProtectDecryptStringA("SetShowMountMode"),			systemSetShowMountMode,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetShowMountMode"),			systemGetShowMountMode,			METH_VARARGS },
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		{ VMProtectDecryptStringA("SetShowOfflineShopRange"),	systemSetShowOfflineShopRange,	METH_VARARGS },
		{ VMProtectDecryptStringA("GetShowOfflineShopRange"),	systemGetShowOfflineShopRange,	METH_VARARGS },
#endif

		#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
		{ VMProtectDecryptStringA("GetPrivateShopViewDistance"),				systemGetPrivateShopViewDistance,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetPrivateShopViewDistance"),				systemSetPrivateShopViewDistance,			METH_VARARGS },
#endif
		{ NULL,							NULL,							NULL }
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("systemSetting"), s_methods);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_STATUS"), CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_INVENTORY"), CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_ABILITY"), CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_SOCIETY"), CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_JOURNAL"), CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_COMMAND"), CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_QUICK"), CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_GAUGE"), CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_MINIMAP"), CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("WINDOW_CHAT"), CPythonSystem::WINDOW_CHAT);
	VMProtectEnd();
}
//martysama0134's ceqyqttoaf71vasf9t71218
