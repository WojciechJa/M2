#include "StdAfx.h"
#include "PythonSystem.h"
#include "PythonBackground.h"
#include "../eterlib/StateManager.h"
#include "../gamelib/MapOutDoor.h"

PyObject * backgroundIsSoftwareTiling(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	return Py_BuildValue("i", rkBG.IsSoftwareTilingEnable());
}

PyObject * backgroundEnableSoftwareTiling(PyObject * poSelf, PyObject * poArgs)
{
	int nIsEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nIsEnable))
		return Py_BadArgument();

	bool isEnable=nIsEnable ? true : false;

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.ReserveSoftwareTilingEnable(isEnable);

	CPythonSystem& rkSystem=CPythonSystem::Instance();
	rkSystem.SetSoftwareTiling(isEnable);
	return Py_BuildNone();
}

PyObject * backgroundEnableSnow(PyObject * poSelf, PyObject * poArgs)
{
	int nIsEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nIsEnable))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
	if (nIsEnable)
		rkBG.EnableSnowEnvironment();
	else
		rkBG.DisableSnowEnvironment();

	return Py_BuildNone();
}

PyObject * backgroundLoadMap(PyObject * poSelf, PyObject * poArgs)
{
	char * pszMapPathName;

	if (!PyTuple_GetString(poArgs, 0, &pszMapPathName))
		return Py_BadArgument();

	float x, y, z;

	if (!PyTuple_GetFloat(poArgs, 1, &x))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 2, &y))
		return Py_BadArgument();

	if (!PyTuple_GetFloat(poArgs, 3, &z))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.LoadMap(pszMapPathName, x, y, z);

//#ifdef _DEBUG
//	CMapOutdoor& rkMap=rkBG.GetMapOutdoorRef();
//	rkMap.EnablePortal(TRUE);
//	rkBG.EnableTerrainOnlyForHeight();
//#endif

	return Py_BuildNone();
}

PyObject * backgroundDestroy(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.SetShadowLevel(CPythonBackground::SHADOW_NONE);
	rkBG.Destroy();
	return Py_BuildNone();
}

PyObject * backgroundRegisterEnvironmentData(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	char * pszEnvironmentFileName;
	if (!PyTuple_GetString(poArgs, 1, &pszEnvironmentFileName))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
 	if (!rkBG.RegisterEnvironmentData(iIndex, pszEnvironmentFileName))
	{
		TraceError("background.RegisterEnvironmentData(iIndex=%d, szEnvironmentFileName=%s)", iIndex, pszEnvironmentFileName);

		// TODO:
	}

	return Py_BuildNone();
}

PyObject * backgroundSetEnvironmentData(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	const TEnvironmentData * c_pEnvironmenData;

	CPythonBackground& rkBG=CPythonBackground::Instance();
	if (rkBG.GetEnvironmentData(iIndex, &c_pEnvironmenData))
		rkBG.ResetEnvironmentDataPtr(c_pEnvironmenData);

	return Py_BuildNone();
}

PyObject * backgroundGetCurrentMapName(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	return Py_BuildValue("s", rkBG.GetWarpMapName());
}

PyObject * backgroundGetPickingPoint(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	TPixelPosition kPPosPicked(0.0f, 0.0f, 0.0f);
	if (rkBG.GetPickingPoint(&kPPosPicked))
	{
		kPPosPicked.y=-kPPosPicked.y;
	}
	return Py_BuildValue("fff", kPPosPicked.x, kPPosPicked.y, kPPosPicked.z);
}

PyObject * backgroundBeginEnvironment(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.BeginEnvironment();
	return Py_BuildNone();
}

PyObject * backgroundEndEnvironemt(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.EndEnvironment();
	return Py_BuildNone();
}

PyObject * backgroundSetCharacterDirLight(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	rkBG.SetCharacterDirLight();
	return Py_BuildNone();
}

PyObject * backgroundSetBackgroundDirLight(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	rkBG.SetBackgroundDirLight();
	return Py_BuildNone();
}

PyObject * backgroundInitialize(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.Create();
	return Py_BuildNone();
}

PyObject * backgroundUpdate(PyObject * poSelf, PyObject * poArgs)
{
	float fCameraX;
	if (!PyTuple_GetFloat(poArgs, 0, &fCameraX))
		return Py_BadArgument();

	float fCameraY;
	if (!PyTuple_GetFloat(poArgs, 1, &fCameraY))
		return Py_BadArgument();

	float fCameraZ;
	if (!PyTuple_GetFloat(poArgs, 2, &fCameraZ))
		return Py_BadArgument();

	CPythonBackground::Instance().Update(fCameraX, fCameraY, fCameraZ);
	return Py_BuildNone();
}

PyObject * backgroundRender(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().Render();
	return Py_BuildNone();
}

PyObject * backgroundRenderPCBlocker(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderPCBlocker();
	return Py_BuildNone();
}

PyObject * backgroundRenderCollision(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderCollision();
	return Py_BuildNone();
}

PyObject * backgroundRenderSky(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderSky();
	return Py_BuildNone();
}

PyObject * backgroundRenderCloud(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderCloud();
	return Py_BuildNone();
}

PyObject * backgroundRenderWater(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderWater();
	return Py_BuildNone();
}

PyObject * backgroundRenderEffect(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderEffect();
	return Py_BuildNone();
}

PyObject * backgroundRenderBeforeLensFlare(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderBeforeLensFlare();
	return Py_BuildNone();
}

PyObject * backgroundRenderAfterLensFlare(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderAfterLensFlare();
	return Py_BuildNone();
}

PyObject * backgroundRenderCharacterShadowToTexture(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().RenderCharacterShadowToTexture();
	return Py_BuildNone();
}

PyObject * backgroundRenderDungeon(PyObject * poSelf, PyObject * poArgs)
{
	assert(!"background.RenderDungeon() - Don't use this function - [levites]");
	return Py_BuildNone();
}

PyObject * backgroundGetHeight(PyObject * poSelf, PyObject * poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BadArgument();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BadArgument();

	float fz = CPythonBackground::Instance().GetHeight(fx, fy);
	return Py_BuildValue("f", fz);
}

PyObject * backgroundGetRenderedSplatNum(PyObject * poSelf, PyObject * poArgs)
{
	int iPatch;
	int iSplat;
	float fSplatRatio;

	std::vector<int> & aTextureNumVector = CPythonBackground::Instance().GetRenderedSplatNum(&iPatch, &iSplat, &fSplatRatio);

	char szOutput[MAX_PATH] = "";
	int iOutput = 0;
	for( std::vector<int>::iterator it = aTextureNumVector.begin(); it != aTextureNumVector.end(); it++ ) {
		iOutput += _snprintf(szOutput + iOutput, sizeof(szOutput) - iOutput, "%d ", *it);
	}
	//std::copy(aTextureNumVector.begin(),aTextureNumVector.end(),std::ostream_iterator<int>(ostr," "));

	return Py_BuildValue("iifs", iPatch, iSplat, fSplatRatio, szOutput);
}

PyObject * backgroundGetRenderedGTINum(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwGraphicThingInstanceNum;
	DWORD dwCRCNum;

	/*CArea::TCRCWithNumberVector & rCRCWithNumberVector = */
	CPythonBackground::Instance().GetRenderedGraphicThingInstanceNum(&dwGraphicThingInstanceNum, &dwCRCNum);

/*
	std::ostringstream ostr;

	std::for_each(rCRCWithNumberVector.begin(),rCRCWithNumberVector.end(),std::ostream_iterator<CArea::TCRCWithNumberVector>(ostr," "));
*/

	return Py_BuildValue("ii", dwGraphicThingInstanceNum, dwCRCNum);
}

PyObject * backgroundGetRenderShadowTime(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();
	return Py_BuildValue("i", rkBG.GetRenderShadowTime());
}

PyObject * backgroundGetShadowMapcolor(PyObject * poSelf, PyObject * poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BadArgument();

	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BadArgument();

	DWORD dwColor = CPythonBackground::Instance().GetShadowMapColor(fx, fy);
	return Py_BuildValue("i", dwColor);
}

PyObject * backgroundSetShadowLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iLevel;

	if (!PyTuple_GetInteger(poArgs, 0, &iLevel))
		return Py_BadArgument();

	CPythonBackground& rkBG = CPythonBackground::Instance();
	rkBG.SetShadowLevel(iLevel);
	return Py_BuildNone();
}

PyObject * backgroundSetVisiblePart(PyObject * poSelf, PyObject * poArgs)
{
	int ePart;
	if (!PyTuple_GetInteger(poArgs, 0, &ePart))
		return Py_BadArgument();

	int isVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &isVisible))
		return Py_BadArgument();

	if (ePart>=CMapOutdoor::PART_NUM)
		return Py_BuildException("ePart(%d)<background.PART_NUM(%d)", ePart, CMapOutdoor::PART_NUM);

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.SetVisiblePart(ePart, isVisible ? true : false);

	return Py_BuildNone();
}

PyObject * backgroundSetSpaltLimit(PyObject * poSelf, PyObject * poArgs)
{
	int iSplatNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iSplatNum))
		return Py_BadArgument();

	if (iSplatNum<0)
		return Py_BuildException("background.SetSplatLimit(iSplatNum(%d)>=0)", iSplatNum);

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.SetSplatLimit(iSplatNum);

	return Py_BuildNone();
}

PyObject * backgroundSelectViewDistanceNum(PyObject * poSelf, PyObject * poArgs)
{
	int iNum;

	if (!PyTuple_GetInteger(poArgs, 0, &iNum))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.SelectViewDistanceNum(iNum);

	return Py_BuildNone();
}

PyObject * backgroundSetViewDistanceSet(PyObject * poSelf, PyObject * poArgs)
{
	int iNum;
	if (!PyTuple_GetInteger(poArgs, 0, &iNum))
		return Py_BadArgument();

	float fFarClip;
	if (!PyTuple_GetFloat(poArgs, 1, &fFarClip))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.SetViewDistanceSet(iNum, fFarClip);
	return Py_BuildNone();
}

PyObject * backgroundGetFarClip(PyObject * poSelf, PyObject * poArgs)
{
	float fFarClip = CPythonBackground::Instance().GetFarClip();
	return Py_BuildValue("f", fFarClip);
}

PyObject * backgroundGetDistanceSetInfo(PyObject * poSelf, PyObject * poArgs)
{
	int iNum;
	float fStart, fEnd, fFarClip;
	CPythonBackground::Instance().GetDistanceSetInfo(&iNum, &fStart, &fEnd, &fFarClip);
	return Py_BuildValue("ifff", iNum, fStart, fEnd, fFarClip);
}

PyObject * backgroundSetBGLoading(PyObject * poSelf, PyObject * poArgs)
{
	bool bBGLoading;
	if (!PyTuple_GetBoolean(poArgs, 0, &bBGLoading))
		return Py_BadArgument();

	//CPythonBackground::Instance().BGLoadingEnable(bBGLoading);
	return Py_BuildNone();
}

PyObject * backgroundSetRenderSort(PyObject * poSelf, PyObject * poArgs)
{
	int eSort;
	if (!PyTuple_GetInteger(poArgs, 0, &eSort))
		return Py_BadArgument();

	CPythonBackground::Instance().SetTerrainRenderSort((CMapOutdoor::ETerrainRenderSort) eSort);
	return Py_BuildNone();
}

PyObject * backgroundSetTransparentTree(PyObject * poSelf, PyObject * poArgs)
{
	int bTransparent;
	if (!PyTuple_GetInteger(poArgs, 0, &bTransparent))
		return Py_BadArgument();

	CPythonBackground::Instance().SetTransparentTree(bTransparent ? true : false);
	return Py_BuildNone();
}

PyObject * backgroundGlobalPositionToLocalPosition(PyObject * poSelf, PyObject * poArgs)
{
	int iX;
	if (!PyTuple_GetInteger(poArgs, 0, &iX))
		return Py_BadArgument();

	int iY;
	if (!PyTuple_GetInteger(poArgs, 1, &iY))
		return Py_BadArgument();

	LONG lX=iX;
	LONG lY=iY;
	CPythonBackground& rkBG=CPythonBackground::Instance();
	rkBG.GlobalPositionToLocalPosition(lX, lY);

	return Py_BuildValue("ii", lX, lY);
}

PyObject * backgroundGlobalPositionToMapInfo(PyObject * poSelf, PyObject * poArgs)
{
	int iX;
	if (!PyTuple_GetInteger(poArgs, 0, &iX))
		return Py_BadArgument();

	int iY;
	if (!PyTuple_GetInteger(poArgs, 1, &iY))
		return Py_BadArgument();

	CPythonBackground& rkBG=CPythonBackground::Instance();
	CPythonBackground::TMapInfo* pkMapInfo=rkBG.GlobalPositionToMapInfo(iX, iY);

	if (pkMapInfo)
		return Py_BuildValue("sii", pkMapInfo->m_strName.c_str(), pkMapInfo->m_dwBaseX, pkMapInfo->m_dwBaseY);
	else
		return Py_BuildValue("sii", "", 0, 0);
}

PyObject * backgroundWarpTest(PyObject * poSelf, PyObject * poArgs)
{
	int iX;
	if (!PyTuple_GetInteger(poArgs, 0, &iX))
		return Py_BadArgument();

	int iY;
	if (!PyTuple_GetInteger(poArgs, 1, &iY))
		return Py_BadArgument();

	CPythonBackground::Instance().Warp((DWORD)iX * 100 , (DWORD)iY * 100);
	return Py_BuildNone();
}

PyObject * backgroundSetXMasTree(PyObject * poSelf, PyObject * poArgs)
{
	int iGrade;
	if (!PyTuple_GetInteger(poArgs, 0, &iGrade))
		return Py_BadArgument();

	CPythonBackground::Instance().SetXMaxTree(iGrade);
	return Py_BuildNone();
}

PyObject * backgroundRegisterDungeonMapName(PyObject * poSelf, PyObject * poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BadArgument();

	CPythonBackground::Instance().RegisterDungeonMapName(szName);
	return Py_BuildNone();
}

PyObject * backgroundVisibleGuildArea(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().VisibleGuildArea();
	return Py_BuildNone();
}

PyObject * backgroundDisableGuildArea(PyObject * poSelf, PyObject * poArgs)
{
	CPythonBackground::Instance().DisableGuildArea();
	return Py_BuildNone();
}

PyObject * backgroundGlobalPositionToMapSize(PyObject * poSelf, PyObject * poArgs)
{
    
    int iX;
    if (!PyTuple_GetInteger(poArgs, 0, &iX))
        return Py_BadArgument();

    int iY;
    if (!PyTuple_GetInteger(poArgs, 1, &iY))
        return Py_BadArgument();

    CPythonBackground& rkBG = CPythonBackground::Instance();
    CPythonBackground::TMapInfo* pkMapInfo = rkBG.GlobalPositionToMapInfo(iX, iY);

    int iOutX = 0;
    int iOutY = 0;

    if (pkMapInfo)
    {
        iOutX = pkMapInfo->m_dwSizeX * CTerrainImpl::TERRAIN_XSIZE;
        iOutY = pkMapInfo->m_dwSizeY * CTerrainImpl::TERRAIN_YSIZE;
    }
    return Py_BuildValue("ii", iOutX, iOutY);
}

void initBackground()
{
	VMProtectBeginUltra("initBackground");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("IsSoftwareTiling"),					backgroundIsSoftwareTiling,					METH_VARARGS },
		{ VMProtectDecryptStringA("EnableSoftwareTiling"),				backgroundEnableSoftwareTiling,				METH_VARARGS },
		{ VMProtectDecryptStringA("EnableSnow"),							backgroundEnableSnow,						METH_VARARGS },
		{ VMProtectDecryptStringA("GlobalPositionToLocalPosition"),		backgroundGlobalPositionToLocalPosition,	METH_VARARGS },
		{ VMProtectDecryptStringA("GlobalPositionToMapInfo"),			backgroundGlobalPositionToMapInfo,			METH_VARARGS },
		{ VMProtectDecryptStringA("GetRenderShadowTime"),				backgroundGetRenderShadowTime,				METH_VARARGS },
		{ VMProtectDecryptStringA("LoadMap"),							backgroundLoadMap,							METH_VARARGS },
		{ VMProtectDecryptStringA("Destroy"),							backgroundDestroy,							METH_VARARGS },
		{ VMProtectDecryptStringA("RegisterEnvironmentData"),			backgroundRegisterEnvironmentData,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetEnvironmentData"),					backgroundSetEnvironmentData,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetCurrentMapName"),					backgroundGetCurrentMapName,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetPickingPoint"),					backgroundGetPickingPoint,					METH_VARARGS },

		{ VMProtectDecryptStringA("BeginEnvironment"),					backgroundBeginEnvironment,					METH_VARARGS },
		{ VMProtectDecryptStringA("EndEnvironment"),						backgroundEndEnvironemt,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetCharacterDirLight"),				backgroundSetCharacterDirLight,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetBackgroundDirLight"),				backgroundSetBackgroundDirLight,			METH_VARARGS },

		{ VMProtectDecryptStringA("Initialize"),							backgroundInitialize,						METH_VARARGS },
		{ VMProtectDecryptStringA("Update"),								backgroundUpdate,							METH_VARARGS },
		{ VMProtectDecryptStringA("Render"),								backgroundRender,							METH_VARARGS },
		{ VMProtectDecryptStringA("RenderPCBlocker"),					backgroundRenderPCBlocker,					METH_VARARGS },
		{ VMProtectDecryptStringA("RenderCollision"),					backgroundRenderCollision,					METH_VARARGS },
		{ VMProtectDecryptStringA("RenderSky"),							backgroundRenderSky,						METH_VARARGS },
		{ VMProtectDecryptStringA("RenderCloud"),						backgroundRenderCloud,						METH_VARARGS },
		{ VMProtectDecryptStringA("RenderWater"),						backgroundRenderWater,						METH_VARARGS },
		{ VMProtectDecryptStringA("RenderEffect"),						backgroundRenderEffect,						METH_VARARGS },
		{ VMProtectDecryptStringA("RenderBeforeLensFlare"),				backgroundRenderBeforeLensFlare,			METH_VARARGS },
		{ VMProtectDecryptStringA("RenderAfterLensFlare"),				backgroundRenderAfterLensFlare,				METH_VARARGS },
		{ VMProtectDecryptStringA("RenderCharacterShadowToTexture"),		backgroundRenderCharacterShadowToTexture,	METH_VARARGS },
		{ VMProtectDecryptStringA("RenderDungeon"),						backgroundRenderDungeon,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetHeight"),							backgroundGetHeight,						METH_VARARGS },

		{ VMProtectDecryptStringA("SetShadowLevel"),						backgroundSetShadowLevel,					METH_VARARGS },

		{ VMProtectDecryptStringA("SetVisiblePart"),						backgroundSetVisiblePart,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetShadowMapColor"),					backgroundGetShadowMapcolor,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetSplatLimit"),						backgroundSetSpaltLimit,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetRenderedSplatNum"),				backgroundGetRenderedSplatNum,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetRenderedGraphicThingInstanceNum"),	backgroundGetRenderedGTINum,				METH_VARARGS },
		{ VMProtectDecryptStringA("SelectViewDistanceNum"),				backgroundSelectViewDistanceNum,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetViewDistanceSet"),					backgroundSetViewDistanceSet,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetFarClip"),							backgroundGetFarClip,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetDistanceSetInfo"),					backgroundGetDistanceSetInfo,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetBGLoading"),						backgroundSetBGLoading,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetRenderSort"),						backgroundSetRenderSort,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetTransparentTree"),					backgroundSetTransparentTree,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetXMasTree"),						backgroundSetXMasTree,						METH_VARARGS },
		{ VMProtectDecryptStringA("RegisterDungeonMapName"),				backgroundRegisterDungeonMapName,			METH_VARARGS },

		{ VMProtectDecryptStringA("VisibleGuildArea"),					backgroundVisibleGuildArea,					METH_VARARGS },
		{ VMProtectDecryptStringA("DisableGuildArea"),					backgroundDisableGuildArea,					METH_VARARGS },

		{ VMProtectDecryptStringA("WarpTest"),							backgroundWarpTest,							METH_VARARGS },

		{ VMProtectDecryptStringA("GlobalPositionToMapSize"),            backgroundGlobalPositionToMapSize,            METH_VARARGS },
		{ NULL, NULL, NULL },
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("background"), s_methods);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_SKY"), CMapOutdoor::PART_SKY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_TREE"), CMapOutdoor::PART_TREE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_CLOUD"), CMapOutdoor::PART_CLOUD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_WATER"), CMapOutdoor::PART_WATER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_OBJECT"), CMapOutdoor::PART_OBJECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PART_TERRAIN"), CMapOutdoor::PART_TERRAIN);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_DEFAULT"), CSkyObject::SKY_RENDER_MODE_DEFAULT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_DIFFUSE"), CSkyObject::SKY_RENDER_MODE_DIFFUSE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_TEXTURE"), CSkyObject::SKY_RENDER_MODE_TEXTURE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_MODULATE"), CSkyObject::SKY_RENDER_MODE_MODULATE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_MODULATE2X"), CSkyObject::SKY_RENDER_MODE_MODULATE2X);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SKY_RENDER_MODE_MODULATE4X"), CSkyObject::SKY_RENDER_MODE_MODULATE4X);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_NONE"), CPythonBackground::SHADOW_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_GROUND"), CPythonBackground::SHADOW_GROUND);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_GROUND_AND_SOLO"), CPythonBackground::SHADOW_GROUND_AND_SOLO);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_ALL"), CPythonBackground::SHADOW_ALL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_ALL_HIGH"), CPythonBackground::SHADOW_ALL_HIGH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SHADOW_ALL_MAX"), CPythonBackground::SHADOW_ALL_MAX);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE0"), CPythonBackground::DISTANCE0);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE1"), CPythonBackground::DISTANCE1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE2"), CPythonBackground::DISTANCE2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE3"), CPythonBackground::DISTANCE3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE4"), CPythonBackground::DISTANCE4);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DISTANCE_SORT"), CMapOutdoor::DISTANCE_SORT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("TEXTURE_SORT"), CMapOutdoor::TEXTURE_SORT);
	VMProtectEnd();
}
//martysama0134's ceqyqttoaf71vasf9t71218
