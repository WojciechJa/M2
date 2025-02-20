#include "StdAfx.h"
#include "PythonApplication.h"


PyObject* renderTargetSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SelectModel(modelIndex);

	return Py_BuildNone();
}
PyObject* renderTargetSetArmor(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetArmor(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetSetWeapon(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetWeapon(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetSetHair(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeHair(modelIndex);

	return Py_BuildNone();

}

PyObject* renderTargetChangeEffect(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->ChangeEffect();

	return Py_BuildNone();
}	

PyObject* renderTargetSetAcce(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetAcce(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetSetVisibility(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	bool isShow = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetVisibility(isShow);

	return Py_BuildNone();
}

PyObject* renderTargetSetBackground(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	char * szPathName;
	if (!PyTuple_GetString(poArgs, 1, &szPathName))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->CreateBackground(
		szPathName, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject* renderTargetSetZoom(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t byRenderIndex = 0;
	if (!PyTuple_GetByte(poArgs, 0, &byRenderIndex))
		return Py_BadArgument();

	bool bZoom = true;
	if (!PyTuple_GetBoolean(poArgs, 1, &bZoom))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(byRenderIndex)->SetZoom(bZoom);
	return Py_BuildNone();
}


void initRenderTarget() {
	static PyMethodDef s_methods[] =
	{
		{ "SelectModel", renderTargetSelectModel, METH_VARARGS },
		{ "SetVisibility", renderTargetSetVisibility, METH_VARARGS },
		{ "SetBackground", renderTargetSetBackground, METH_VARARGS },
		{ "SetArmor", renderTargetSetArmor, METH_VARARGS },
		{ "SetWeapon", renderTargetSetWeapon, METH_VARARGS },
		{ "SetHair", renderTargetSetHair, METH_VARARGS },
		{ "ChangeEffect", renderTargetChangeEffect, METH_VARARGS },
		{ "SetAcce", renderTargetSetAcce, METH_VARARGS },
		{ "SetZoom", renderTargetSetZoom, METH_VARARGS },
		{nullptr, nullptr, 0 },
	};

	PyObject* poModule = Py_InitModule("renderTarget", s_methods);

}