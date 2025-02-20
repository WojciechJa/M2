#include "StdAfx.h"
#include "PythonNetworkStream.h"
//#include "PythonNetworkDatagram.h"
#include "AccountConnector.h"
#include "PythonGuild.h"
#include "Test.h"


#include "../gamelib/ItemManager.h"
#include "AbstractPlayer.h"

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
	#include "PythonPlayerSettingsModule.h"
#endif

#include "PythonDef.hpp"

#if defined(__BL_MOVE_CHANNEL__)
static std::map<int, std::string> g_ChannelNameMap;
static std::string gs_ServerName{};
static int gi_ChannelIndex{};
static long gl_MapIndex{};
#endif

static std::string gs_stServerInfo;
extern BOOL gs_bEmpireLanuageEnable;
std::list<std::string> g_kList_strCommand;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
inline bool str_to_numbers(long long& out, const char *in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (long long)_strtoui64(in, NULL, 10);
	return true;
}
extern inline bool str_to_numbers(long long& out, const char *in);
#endif

PyObject* netGetBettingGuildWarValue(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.EXPORT_GetBettingGuildWarValue(szName));
}

PyObject* netSetServerInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	gs_stServerInfo=szFileName;
	return Py_BuildNone();
}

PyObject* netGetServerInfo(PyObject* poSelf, PyObject* poArgs)
{
#if defined(__BL_MOVE_CHANNEL__)
	auto it = g_ChannelNameMap.find(gi_ChannelIndex);
	if (it != g_ChannelNameMap.end())
		gs_stServerInfo = gs_ServerName + ", " + it->second;
#endif
	return Py_BuildValue("s", gs_stServerInfo.c_str());
}

PyObject* netPreserveServerCommand(PyObject* poSelf, PyObject* poArgs)
{
	char* szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
		return Py_BuildException();

	g_kList_strCommand.push_back(szLine);

	return Py_BuildNone();
}

PyObject* netGetPreservedServerCommand(PyObject* poSelf, PyObject* poArgs)
{
	if (g_kList_strCommand.empty())
		return Py_BuildValue("s", "");

	std::string strCommand = g_kList_strCommand.front();
	g_kList_strCommand.pop_front();

	return Py_BuildValue("s", strCommand.c_str());
}

PyObject* netStartGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	return Py_BuildNone();
}

PyObject* netIsTest(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", __IS_TEST_SERVER_MODE__);
}

PyObject* netWarp(PyObject* poSelf, PyObject* poArgs)
{
	int nX;
	if (!PyTuple_GetInteger(poArgs, 0, &nX))
		return Py_BuildException();

	int nY;
	if (!PyTuple_GetInteger(poArgs, 1, &nY))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.Warp(nX, nY);

	return Py_BuildNone();
}

PyObject* netLoadInsultList(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.LoadInsultList(szFileName));
}

PyObject* netUploadMark(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadMark(szFileName));
}

PyObject* netUploadSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadSymbol(szFileName));
}

PyObject* netGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetGuildID());
}

PyObject* netGetEmpireID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetEmpireID());
}

PyObject* netGetMainActorVID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorVID());
}

PyObject* netGetMainActorRace(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorRace());
}

PyObject* netGetMainActorEmpire(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorEmpire());
}

PyObject* netGetMainActorSkillGroup(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorSkillGroup());
}

PyObject* netIsSelectedEmpire(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsSelectedEmpire());
}

PyObject* netGetAccountCharacterSlotDataInteger(PyObject* poSelf, PyObject* poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
		return Py_BuildException();

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	UINT uResult=rkNetStream.GetAccountCharacterSlotDatau(nIndex, nType);
	return Py_BuildValue("i", uResult);
}

PyObject* netGetAccountCharacterSlotDataString(PyObject* poSelf, PyObject* poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
		return Py_BuildException();

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetAccountCharacterSlotDataz(nIndex, nType));
}

// SUPPORT_BGM
PyObject* netGetFieldMusicFileName(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetFieldMusicFileName());
}

PyObject* netGetFieldMusicVolume(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("f", rkNetStream.GetFieldMusicVolume());
}
// END_OF_SUPPORT_BGM

PyObject* netSetPhaseWindow(PyObject* poSelf, PyObject* poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
		return Py_BuildException();

	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netClearPhaseWindow(PyObject* poSelf, PyObject* poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
		return Py_BuildException();

	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ClearPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetServerCommandParserWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetServerCommandParserWindow(poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetAccountConnectorHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
		return Py_BuildException();

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.SetHandler(poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;

	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetHandler(poHandler);
	return Py_BuildNone();
}

PyObject* netSetTCPRecvBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetTCPSendBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetSendBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetUDPRecvBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

	//CPythonNetworkDatagram::Instance().SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetMarkServer(PyObject* poSelf, PyObject* poArgs)
{
	char* szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetMarkServer(szAddr, port);
	return Py_BuildNone();
}

PyObject* netConnectTCP(PyObject* poSelf, PyObject* poArgs)
{
	char* szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ConnectLoginServer(szAddr, port);
	return Py_BuildNone();
}

PyObject* netConnectUDP(PyObject* poSelf, PyObject* poArgs)
{
	char * c_szIP;
	if (!PyTuple_GetString(poArgs, 0, &c_szIP))
		return Py_BuildException();
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();

	//CPythonNetworkDatagram::Instance().SetConnection(c_szIP, iPort);
	return Py_BuildNone();
}

PyObject* netConnectToAccountServer(PyObject* poSelf, PyObject* poArgs)
{
	char* addr;
	if (!PyTuple_GetString(poArgs, 0, &addr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	char* account_addr;
	if (!PyTuple_GetString(poArgs, 2, &account_addr))
		return Py_BuildException();

	int account_port;
	if (!PyTuple_GetInteger(poArgs, 3, &account_port))
		return Py_BuildException();

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.Connect(addr, port, account_addr, account_port);
	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject* netSetLoginInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char* szPwd;
	if (!PyTuple_GetString(poArgs, 1, &szPwd))
		return Py_BuildException();

	char* szPin;
	if (!PyTuple_GetString(poArgs, 2, &szPin))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkNetStream.SetLoginInfo(szName, szPwd, szPin);
	rkAccountConnector.SetLoginInfo(szName, szPwd, szPin);
	return Py_BuildNone();
}

PyObject* netSetOfflinePhase(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	return Py_BuildNone();
}

PyObject* netSendSelectEmpirePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iEmpireIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iEmpireIndex))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendSelectEmpirePacket(iEmpireIndex);
	return Py_BuildNone();
}

PyObject* netSendLoginPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char* szPwd;
	if (!PyTuple_GetString(poArgs, 1, &szPwd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendLoginPacket(szName, szPwd);
	return Py_BuildNone();
}

PyObject* netDirectEnter(PyObject* poSelf, PyObject* poArgs)
{
	int nChrSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &nChrSlot))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ConnectGameServer(nChrSlot);
	return Py_BuildNone();
}

PyObject* netSendSelectCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int Index;
	if (!PyTuple_GetInteger(poArgs, 0, &Index))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendSelectCharacterPacket((BYTE) Index);
	return Py_BuildNone();
}

PyObject* netSendChangeNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	char* szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendChangeNamePacket((BYTE)iIndex, szName);
	return Py_BuildNone();
}

PyObject* netEnableChatInsultFilter(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.EnableChatInsultFilter(nEnable ? true : false);
	return Py_BuildNone();
}

PyObject* netIsChatInsultIn(PyObject* poSelf, PyObject* poArgs)
{
	char* szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsChatInsultIn(szMsg));
}

PyObject* netIsInsultIn(PyObject* poSelf, PyObject* poArgs)
{
	char* szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsInsultIn(szMsg));
}

PyObject* netSendWhisperPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	char* szLine;

	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	if (!PyTuple_GetString(poArgs, 1, &szLine))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendWhisperPacket(szName, szLine);
	return Py_BuildNone();
}

PyObject* netSendMobileMessagePacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	char* szLine;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	if (!PyTuple_GetString(poArgs, 1, &szLine))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendMobileMessagePacket(szName, szLine);
	return Py_BuildNone();
}

PyObject* netSendCharacterPositionPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iPosition))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendCharacterPositionPacket((BYTE) iPosition);
	return Py_BuildNone();
}

PyObject* netSendChatPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		iType = CHAT_TYPE_TALKING;
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendChatPacket(szLine, iType);
	return Py_BuildNone();
}

PyObject* netSendEmoticon(PyObject* poSelf, PyObject* poArgs)
{
	int eEmoticon;
	if (!PyTuple_GetInteger(poArgs, 0, &eEmoticon))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendEmoticon(eEmoticon);
	return Py_BuildNone();
}

PyObject* netSendCreateCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	char* name;
	if (!PyTuple_GetString(poArgs, 1, &name))
		return Py_BuildException();

	int job;
	if (!PyTuple_GetInteger(poArgs, 2, &job))
		return Py_BuildException();

	int shape;
	if (!PyTuple_GetInteger(poArgs, 3, &shape))
		return Py_BuildException();

	int stat1;
	if (!PyTuple_GetInteger(poArgs, 4, &stat1))
		return Py_BuildException();
	int stat2;
	if (!PyTuple_GetInteger(poArgs, 5, &stat2))
		return Py_BuildException();
	int stat3;
	if (!PyTuple_GetInteger(poArgs, 6, &stat3))
		return Py_BuildException();
	int stat4;
	if (!PyTuple_GetInteger(poArgs, 7, &stat4))
		return Py_BuildException();

	if (index<0 && index>3)
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendCreateCharacterPacket((BYTE) index, name, (BYTE) job, (BYTE) shape, stat1, stat2, stat3, stat4);
	return Py_BuildNone();
}

PyObject* netSendDestroyCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	char * szPrivateCode;
	if (!PyTuple_GetString(poArgs, 1, &szPrivateCode))
		return Py_BuildException();

	if (index<0 && index>3)
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDestroyCharacterPacket((BYTE) index, szPrivateCode);
	return Py_BuildNone();
}

PyObject* netSendEnterGamePacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendEnterGame();
	return Py_BuildNone();
}

PyObject* netOnClickPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendOnClickPacket(index);

	return Py_BuildNone();
}

PyObject* netSendItemUsePacket(PyObject* poSelf, PyObject* poArgs)
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

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemUsePacket(Cell);
	return Py_BuildNone();
}

#ifdef ENABLE_INSTANT_CHEST_OPEN
PyObject* netSendOpenChestPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		return Py_BuildException();

	int iOpenCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iOpenCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOpenChestPacket(Cell, iOpenCount);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_INSTANT_INGOT_OPEN
PyObject* netSendOpenIngotPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		return Py_BuildException();

	int iOpenCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iOpenCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOpenIngotPacket(Cell, iOpenCount);
	return Py_BuildNone();
}
#endif

PyObject* netSendItemUseToItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SourceCell;
	TItemPos TargetCell;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SourceCell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &TargetCell.cell))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetByte(poArgs, 0, &SourceCell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &SourceCell.cell))
			return Py_BuildException();

		if (!PyTuple_GetByte(poArgs, 2, &TargetCell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &TargetCell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemUseToItemPacket(SourceCell, TargetCell);
	return Py_BuildNone();
}

PyObject* netSendItemDropPacket(PyObject* poSelf, PyObject* poArgs)
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

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(Cell, 0);
	return Py_BuildNone();
}

PyObject* netSendItemDropPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int count;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &count))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &count))
			return Py_BuildException();

		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(Cell, 0, count);
	return Py_BuildNone();
}

PyObject* netSendElkDropPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(TItemPos(RESERVED_WINDOW, 0), (DWORD) iElk);
	return Py_BuildNone();
}

PyObject* netSendGoldDropPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(TItemPos (RESERVED_WINDOW, 0), (DWORD) iElk, 0);
	return Py_BuildNone();
}

PyObject* netSendItemMovePacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	TItemPos ChangeCell;
	int num;

	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ChangeCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &num))
			return Py_BuildException();
		break;
	case 5:
		{
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			if (!PyTuple_GetByte(poArgs, 2, &ChangeCell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 3, &ChangeCell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 4, &num))
				return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
#ifdef ENABLE_EXTENDED_ITEM_COUNT
	rkNetStream.SendItemMovePacket(Cell, ChangeCell, (int)num);
#else
	rkNetStream.SendItemMovePacket(Cell, ChangeCell, (BYTE)num);
#endif
	return Py_BuildNone();
}

PyObject* netSendItemPickUpPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemPickUpPacket(vid);
	return Py_BuildNone();
}

PyObject* netSendGiveItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iTargetVID;
	TItemPos Cell;
	int iItemCount;
	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iItemCount))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
			return Py_BuildException();
		if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &iItemCount))
			return Py_BuildException();
		break;
	default:
		break;
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendGiveItemPacket(iTargetVID, Cell, iItemCount);
	return Py_BuildNone();
}

PyObject* netSendShopEndPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopEndPacket();
	return Py_BuildNone();
}

PyObject* netSendShopBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 0, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopBuyPacket(iCount);
	return Py_BuildNone();
}

PyObject* netSendShopSellPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
		return Py_BuildException();
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopSellPacket(iSlotNumber);
	return Py_BuildNone();
}

PyObject* netSendShopSellPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
		return Py_BuildException();
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iCount))
		return Py_BuildException();
//#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	int iType;
	if (!PyTuple_GetInteger(poArgs, 2, &iType))
		return Py_BuildException();
//#endif

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
//#ifdef WJ_SPLIT_INVENTORY_SYSTEM
	rkNetStream.SendShopSellPacketNew(iSlotNumber, iCount, iType);
//#else
	//rkNetStream.SendShopSellPacketNew(iSlotNumber, iCount);
//#endif
	return Py_BuildNone();
}

PyObject* netSendExchangeStartPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeStartPacket(vid);
	return Py_BuildNone();
}

#ifdef ENABLE_LONG_LONG
PyObject* netSendExchangeElkAddPacket(PyObject* poSelf, PyObject* poArgs)
{
	long long iElk;
	if (!PyTuple_GetLongLong(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeElkAddPacket(iElk);
	return Py_BuildNone();
}
#else
PyObject* netSendExchangeElkAddPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeElkAddPacket(iElk);
	return Py_BuildNone();
}
#endif


#ifdef ENABLE_CHEQUE_SYSTEM
PyObject* netSendExchangeChequeAddPacket(PyObject* poSelf, PyObject* poArgs)
{
	long long cheque;
	if (!PyTuple_GetLongLong(poArgs, 0, &cheque))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeChequeAddPacket(cheque);
	return Py_BuildNone();
}
#endif

PyObject* netSendExchangeItemAddPacket(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();
	WORD wSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wSlotIndex))
		return Py_BuildException();
	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
#if defined(ITEM_CHECKINOUT_UPDATE)
	rkNetStream.SendExchangeItemAddPacket(TItemPos(bWindowType, wSlotIndex), iDisplaySlotIndex, iDisplaySlotIndex == -1);
#else
	rkNetStream.SendExchangeItemAddPacket(TItemPos(bWindowType, wSlotIndex), iDisplaySlotIndex);
#endif
	return Py_BuildNone();
}

PyObject* netSendExchangeItemDelPacket(PyObject* poSelf, PyObject* poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeItemDelPacket((BYTE) pos);
	return Py_BuildNone();
}

PyObject* netSendExchangeAcceptPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeAcceptPacket();
	return Py_BuildNone();
}

PyObject* netSendExchangeExitPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeExitPacket();
	return Py_BuildNone();
}

PyObject* netExitApplication(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ExitApplication();
	return Py_BuildNone();
}

PyObject* netExitGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ExitGame();
	return Py_BuildNone();
}

PyObject* netLogOutGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.LogOutGame();
	return Py_BuildNone();
}

PyObject* netDisconnect(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	rkNetStream.Disconnect();

	return Py_BuildNone();
}

PyObject* netIsConnect(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsOnline());
}

PyObject* netToggleGameDebugInfo(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ToggleGameDebugInfo();
	return Py_BuildNone();
}

PyObject* netRegisterEmoticonString(PyObject* poSelf, PyObject* poArgs)
{
	char * pcEmoticonString;
	if (!PyTuple_GetString(poArgs, 0, &pcEmoticonString))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.RegisterEmoticonString(pcEmoticonString);
	return Py_BuildNone();
}

PyObject* netSendMessengerAddByVIDPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerAddByVIDPacket(vid);

	return Py_BuildNone();
}

PyObject* netSendMessengerAddByNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerAddByNamePacket(szName);

	return Py_BuildNone();
}

PyObject* netSendMessengerRemovePacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();
	char * szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerRemovePacket(szKey, szName);

	return Py_BuildNone();
}

PyObject* netSendPartyInvitePacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyInvitePacket(vid);

	return Py_BuildNone();
}

#ifdef ENABLE_CHEQUE_SYSTEM
PyObject* netSendWonExchangeSellPacket(PyObject* poSelf, PyObject* poArgs)
{
	WORD wValue = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &wValue))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendWonExchangeSellPacket(wValue);
	return Py_BuildNone();
}

PyObject* netSendWonExchangeBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	WORD wValue = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &wValue))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendWonExchangeBuyPacket(wValue);
	return Py_BuildNone();
}
#endif

PyObject* netSendPartyInviteAnswerPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();
	int answer;
	if (!PyTuple_GetInteger(poArgs, 1, &answer))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyInviteAnswerPacket(vid, answer);

	return Py_BuildNone();
}

PyObject* netSendPartyExitPacket(PyObject* poSelf, PyObject* poArgs)
{
	IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();

	DWORD dwVID = rPlayer.GetMainCharacterIndex();
	DWORD dwPID;
	if (rPlayer.PartyMemberVIDToPID(dwVID, &dwPID))
		rns.SendPartyRemovePacket(dwPID);

	return Py_BuildNone();
}

PyObject* netSendPartyRemovePacketPID(PyObject* poSelf, PyObject* poArgs)
{
	int pid;
	if (!PyTuple_GetInteger(poArgs, 0, &pid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyRemovePacket(pid);

	return Py_BuildNone();
}

PyObject* netSendPartyRemovePacketVID(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	DWORD dwPID;
	if (rPlayer.PartyMemberVIDToPID(vid, &dwPID))
		rns.SendPartyRemovePacket(dwPID);

	return Py_BuildNone();
}

PyObject* netSendPartySetStatePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();
	int iState;
	if (!PyTuple_GetInteger(poArgs, 1, &iState))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 2, &iFlag))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartySetStatePacket(iVID, iState, iFlag);

	return Py_BuildNone();
}

PyObject* netSendPartyUseSkillPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
		return Py_BuildException();
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyUseSkillPacket(iSkillIndex, iVID);

	return Py_BuildNone();
}

PyObject* netSendPartyParameterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyParameterPacket(iMode);

	return Py_BuildNone();
}

PyObject* netSendSafeboxSaveMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_SAVE, iMoney);

	return Py_BuildNone();
}

PyObject* netSendSafeboxWithdrawMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_WITHDRAW, iMoney);

	return Py_BuildNone();
}

PyObject* netSendSafeboxCheckinPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos InventoryPos;
	int iSafeBoxPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		InventoryPos.window_type = INVENTORY;
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &iSafeBoxPos))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iSafeBoxPos))
			return Py_BuildException();
		break;

	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckinPacket(InventoryPos, iSafeBoxPos);

	return Py_BuildNone();
}

PyObject* netSendSafeboxCheckoutPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSafeBoxPos;
	TItemPos InventoryPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
			return Py_BuildException();
		break;
	default:
		break;
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckoutPacket(iSafeBoxPos, InventoryPos);

	return Py_BuildNone();
}

PyObject* netSendSafeboxItemMovePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSourcePos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSourcePos))
		return Py_BuildException();
	int iTargetPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetPos))
		return Py_BuildException();
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxItemMovePacket(iSourcePos, iTargetPos, iCount);

	return Py_BuildNone();
}

PyObject* netSendMallCheckoutPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMallPos;
	TItemPos InventoryPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMallCheckoutPacket(iMallPos, InventoryPos);

	return Py_BuildNone();
}

PyObject* netSendAnswerMakeGuildPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendAnswerMakeGuildPacket(szName);

	return Py_BuildNone();
}

PyObject* netSendQuestInputStringPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendQuestInputStringPacket(szString);

	return Py_BuildNone();
}

PyObject* netSendQuestConfirmPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 0, &iAnswer))
		return Py_BuildException();
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 1, &iPID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendQuestConfirmPacket(iAnswer, iPID);

	return Py_BuildNone();
}

PyObject* netSendGuildAddMemberPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildAddMemberPacket(iVID);

	return Py_BuildNone();
}

PyObject* netSendGuildRemoveMemberPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();

	CPythonGuild::TGuildMemberData * pGuildMemberData;
	if (!CPythonGuild::Instance().GetMemberDataPtrByName(szKey, &pGuildMemberData))
	{
		TraceError("netSendGuildRemoveMemberPacket(szKey=%s) - Can't Find Guild Member\n", szKey);
		return Py_BuildNone();
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildRemoveMemberPacket(pGuildMemberData->dwPID);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeGradeNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
		return Py_BuildException();
	char * szGradeName;
	if (!PyTuple_GetString(poArgs, 1, &szGradeName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeNamePacket(iGradeNumber, szGradeName);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeGradeAuthorityPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
		return Py_BuildException();
	int iAuthority;
	if (!PyTuple_GetInteger(poArgs, 1, &iAuthority))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeAuthorityPacket(iGradeNumber, iAuthority);

	return Py_BuildNone();
}

PyObject* netSendGuildOfferPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iExperience;
	if (!PyTuple_GetInteger(poArgs, 0, &iExperience))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildOfferPacket(iExperience);

	return Py_BuildNone();
}

PyObject* netSnedGuildPostCommentPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szComment;
	if (!PyTuple_GetString(poArgs, 0, &szComment))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildPostCommentPacket(szComment);

	return Py_BuildNone();
}

PyObject* netSnedGuildDeleteCommentPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildDeleteCommentPacket(iIndex);

	return Py_BuildNone();
}

PyObject* netSendGuildRefreshCommentsPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iHightestIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iHightestIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildRefreshCommentsPacket(iHightestIndex);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeMemberGradePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iGradeNumber))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGradePacket(iPID, iGradeNumber);

	return Py_BuildNone();
}

PyObject* netSendGuildUseSkillPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillID;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillID))
		return Py_BuildException();
	int iTargetVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildUseSkillPacket(iSkillID, iTargetVID);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeMemberGeneralPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGeneralPacket(iPID, iFlag);

	return Py_BuildNone();
}

PyObject* netSendGuildInviteAnswerPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 0, &iGuildID))
		return Py_BuildException();
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 1, &iAnswer))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildInviteAnswerPacket(iGuildID, iAnswer);

	return Py_BuildNone();
}

PyObject* netSendGuildChargeGSPPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChargeGSPPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendGuildDepositMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildDepositMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendGuildWithdrawMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildWithdrawMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendRequestRefineInfoPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

//	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
//	rns.SendRequestRefineInfoPacket(iSlotIndex);
	assert(!"netSendRequestRefineInfoPacket - This function is not used anymore");

	return Py_BuildNone();
}

PyObject* netSendRefinePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendRefinePacket(iSlotIndex, iType);

	return Py_BuildNone();
}

PyObject* netSendSelectItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iItemPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemPos))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSelectItemPacket(iItemPos);

	return Py_BuildNone();
}

PyObject* netSetPacketSequenceMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rns.SetPacketSequenceMode(true);
	rkAccountConnector.SetPacketSequenceMode(true);

	return Py_BuildNone();
}

PyObject* netSetEmpireLanguageMode(PyObject* poSelf, PyObject* poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
		return Py_BuildException();

	//CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	gs_bEmpireLanuageEnable = iMode;

	return Py_BuildNone();
}

PyObject* netSetSkillGroupFake(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.__TEST_SetSkillGroupFake(iIndex);

	return Py_BuildNone();
}

#include "GuildMarkUploader.h"
#include "GuildMarkDownloader.h"
#include "PythonTextTail.h"
#include "PythonCharacterManager.h"

PyObject* netSendGuildSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char * szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
		return Py_BuildException();
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 2, &szFileName))
		return Py_BuildException();
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 3, &iGuildID))
		return Py_BuildException();

	CNetworkAddress kAddress;
	kAddress.Set(szIP, iPort);

	UINT uiError;

	CGuildMarkUploader& rkGuildMarkUploader=CGuildMarkUploader::Instance();
	if (!rkGuildMarkUploader.ConnectToSendSymbol(kAddress, 0, 0, iGuildID, szFileName, &uiError))
	{
		assert(!"Failed connecting to send symbol");
	}

	return Py_BuildNone();
}

PyObject* netDisconnectUploader(PyObject* poSelf, PyObject* poArgs)
{
	CGuildMarkUploader& rkGuildMarkUploader=CGuildMarkUploader::Instance();
	rkGuildMarkUploader.Disconnect();
	return Py_BuildNone();
}

PyObject* netRecvGuildSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char * szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
		return Py_BuildException();
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 2, &iGuildID))
		return Py_BuildException();

	CNetworkAddress kAddress;
	kAddress.Set(szIP, iPort);

	std::vector<DWORD> kVec_dwGuildID;
	kVec_dwGuildID.clear();
	kVec_dwGuildID.push_back(iGuildID);

	// @fixme006
	if (kVec_dwGuildID.size()>0)
	{
		CGuildMarkDownloader& rkGuildMarkDownloader=CGuildMarkDownloader::Instance();
		if (!rkGuildMarkDownloader.ConnectToRecvSymbol(kAddress, 0, 0, kVec_dwGuildID))
		{
			assert(!"Failed connecting to recv symbol");
		}
	}

	return Py_BuildNone();
}

PyObject* netRegisterErrorLog(PyObject* poSelf, PyObject* poArgs)
{
	char * szLog;
	if (!PyTuple_GetString(poArgs, 0, &szLog))
		return Py_BuildException();

	return Py_BuildNone();
}

#ifdef ENABLE_REMOVE_ITEM
PyObject* netSendRemoveItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poItemPos = PyTuple_GetItem(poArgs, 0);
	if (!PyList_Check(poItemPos))
		return Py_BadArgument();

	int item_count = PyList_Size(poItemPos);
	std::vector<TItemPos> vec_item_pos;
	vec_item_pos.resize(item_count);

	for (int i = 0; i < item_count; i++)
	{
		PyObject* poTempItemPos = PyList_GetItem(poItemPos, i);
		if (!poTempItemPos)
			continue;

		vec_item_pos[i].window_type = PyInt_AsLong(PyList_GetItem(poTempItemPos, 0));
		vec_item_pos[i].cell = PyInt_AsLong(PyList_GetItem(poTempItemPos, 1));
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendRemoveItemPacket(vec_item_pos);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_RESP_SYSTEM
PyObject* netSendRespFetchDropPacket(PyObject* poSelf, PyObject* poArgs)
{
	uint32_t mobVnum;
	if (!PyTuple_GetUnsignedInteger(poArgs, 0, &mobVnum))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendRespFetchDropPacket(mobVnum);

	return Py_BuildNone();
}

PyObject* netSendRespFetchRespPacket(PyObject* poSelf, PyObject* poArgs)
{
	uint32_t mobVnum;
	if (!PyTuple_GetUnsignedInteger(poArgs, 0, &mobVnum))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendRespFetchRespPacket(mobVnum);

	return Py_BuildNone();
}

PyObject* netSendRespTeleportPacket(PyObject* poSelf, PyObject* poArgs)
{
	size_t id;
	if (!PyTuple_GetUnsignedInteger(poArgs, 0, &id))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendRespTeleportPacket(id);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_ODLAMKI_SYSTEM
PyObject* netSendOdlamkiItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poItemPos = PyTuple_GetItem(poArgs, 0);
	if (!PyList_Check(poItemPos))
		return Py_BadArgument();

	int item_count = PyList_Size(poItemPos);
	std::vector<TItemPos> vec_item_pos;
	vec_item_pos.resize(item_count);

	for (int i = 0; i < item_count; i++)
	{
		PyObject* poTempItemPos = PyList_GetItem(poItemPos, i);
		if (!poTempItemPos)
			continue;

		vec_item_pos[i].window_type = PyInt_AsLong(PyList_GetItem(poTempItemPos, 0));
		vec_item_pos[i].cell = PyInt_AsLong(PyList_GetItem(poTempItemPos, 1));
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendOdlamkiItemPacket(vec_item_pos);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_VS_SHOP_SEARCH
PyObject* netSendSearchPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* items = PyTuple_GetItem(poArgs, 0);
	if (!PyList_Check(items))
		return Py_BadArgument();

	int item_count = PyList_Size(items);
	std::vector<TSearchItems> vec_items;
	vec_items.resize(item_count);

	for (int i = 0; i < item_count; i++)
	{
		PyObject* poTempItemPos = PyList_GetItem(items, i);
		if (!poTempItemPos)
			continue;

		vec_items[i].vnum = PyInt_AsLong(PyList_GetItem(poTempItemPos, 0));
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSearchPacket(vec_items);

	return Py_BuildNone();
}
#endif

#ifdef WEEKLY_RANK_BYLUZER
PyObject* netSelectWeeklyRankPage(PyObject* poSelf, PyObject* poArgs)
{
	int id;
	if (!PyTuple_GetInteger(poArgs, 0, &id))
		return Py_BuildException();
	if (id < 0 || id > 6)
		return Py_BuildException();
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SelectWeeklyRankPage(id);

	return Py_BuildNone();
}
#endif
#ifdef TITLE_SYSTEM_BYLUZER
PyObject* netSendSelectTitle(PyObject* poSelf, PyObject* poArgs)
{
	int id;
	if (!PyTuple_GetInteger(poArgs, 0, &id))
		return Py_BuildException();
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSelectTitle(id);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_ITEMSHOP
PyObject* netSendItemShopOpenPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* val;
	if (!PyTuple_GetObject(poArgs, 0, &val))
		return Py_BuildException();

	time_t ttTime = PyLong_AsLongLong(val);

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemShopOpenPacket(ttTime);

	return Py_BuildNone();
}

PyObject* netSendItemShopBuyPacket(PyObject* poSelf, PyObject* poArgs)
{

	int id, category, count, bonus_type;

	if (!PyTuple_GetInteger(poArgs, 0, &id))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &category))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &count))
		return Py_BuildException();
	
	if (!PyTuple_GetInteger(poArgs, 3, &bonus_type))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemShopBuyPacket(id, category, count, bonus_type);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_TOMBOLA
PyObject* netSendTombolaStartPacket(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byGroup = 0;
	if (!PyTuple_GetByte(poArgs, 0, &byGroup))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendTombolaStartPacket(byGroup);

	return Py_BuildNone();
}

PyObject* netSendTombolaOpenPacket(PyObject* poSelf, PyObject* poArgs)
{

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendTombolaOpenPacket();

	return Py_BuildNone();
}
#endif
#ifdef ENABLE_DROP_INFO
PyObject* netSendDropInfoPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendDropInfoPacket();

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
PyObject* netLoadResourcesInCache(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayerSettingsModule::Instance().startLoadThread();
	return Py_BuildNone();
}

PyObject* netRegisterSkills(PyObject* poSelf, PyObject* poArgs)
{
	int32_t race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	int32_t skill_group;
	if (!PyTuple_GetInteger(poArgs, 1, &skill_group))
		return Py_BuildException();

	int32_t empire;
	if (!PyTuple_GetInteger(poArgs, 2, &empire))
		return Py_BuildException();

	CPythonPlayerSettingsModule::Instance().RegisterSkills(race, skill_group, empire);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_LRN_LOCATION_SYSTEM
PyObject* netSendLocationManagerPacket(PyObject* poSelf, PyObject* poArgs)
{
	int bAction;
	if (!PyTuple_GetInteger(poArgs, 0, &bAction))
		return Py_BuildException();
	
	BYTE bPosition;
	if (!PyTuple_GetInteger(poArgs, 1, &bPosition))
		return Py_BuildException();
	
	char* szName;
	if (!PyTuple_GetString(poArgs, 2, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPlayerLocationManager(bAction, bPosition, szName);
	
	return Py_BuildNone();
}
#endif

#if defined(__BL_MOVE_CHANNEL__)
PyObject* netSetChannelName(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
	{
		int iChannelIndex;
		if (!PyTuple_GetInteger(poArgs, 0, &iChannelIndex))
			return Py_BuildException();

		gi_ChannelIndex = iChannelIndex;
	}
	break;
	case 2:
	{
		int iChannelIndex;
		if (!PyTuple_GetInteger(poArgs, 0, &iChannelIndex))
			return Py_BuildException();

		char* szChannelName;
		if (!PyTuple_GetString(poArgs, 1, &szChannelName))
			return Py_BuildException();

		g_ChannelNameMap[iChannelIndex] = szChannelName;
	}
	break;
	default:
		return Py_BuildException();
	}

	return Py_BuildNone();
}

PyObject* netGetChannelName(PyObject* poSelf, PyObject* poArgs)
{
	int iChannelIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iChannelIndex))
		return Py_BuildException();

	auto it = g_ChannelNameMap.find(iChannelIndex);
	if (it != g_ChannelNameMap.end())
		return Py_BuildValue("s", it->second.c_str());

	return Py_BuildValue("s", "");
}

PyObject* netGetChannelNumber(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", gi_ChannelIndex);
}

PyObject* netGetChannelCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", g_ChannelNameMap.size());
}

PyObject* netSetMapIndex(PyObject* poSelf, PyObject* poArgs)
{
	long lMapIndex;
	if (!PyTuple_GetLong(poArgs, 0, &lMapIndex))
		return Py_BuildException();

	gl_MapIndex = lMapIndex;

	return Py_BuildNone();
}

PyObject* netGetMapIndex(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", gl_MapIndex);
}

PyObject* netSetServerName(PyObject* poSelf, PyObject* poArgs)
{
	char* szServerName;
	if (!PyTuple_GetString(poArgs, 0, &szServerName))
		return Py_BuildException();

	gs_ServerName = szServerName;
	return Py_BuildNone();
}

PyObject* netClearServerInfo(PyObject* poSelf, PyObject* poArgs)
{
	g_ChannelNameMap.clear();

	return Py_BuildNone();
}

PyObject* netMoveChannelGame(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bChannelIndex;
	if (!PyTuple_GetByte(poArgs, 0, &bChannelIndex))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendMoveChannelPacket(bChannelIndex);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
PyObject* netSendOfflineShopEndPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOfflineShopEndPacket();
	return Py_BuildNone();
}

PyObject* netSendOfflineShopBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 0, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOfflineShopBuyPacket(iCount);
	return Py_BuildNone();
}

PyObject * netSendRefreshOfflineShop(PyObject * poSelf, PyObject * poArgs)
{
	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendRefreshOfflineShop();
	return Py_BuildNone();
}

PyObject * netSendAddOfflineShopItem(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bDisplayPos;
	TItemPos itemPos;

	if (!PyTuple_GetInteger(poArgs, 0, &itemPos.window_type))
		return Py_BuildException();
	
	if (!PyTuple_GetInteger(poArgs, 1, &itemPos.cell))
		return Py_BuildException();
	
	
	if (!PyTuple_GetInteger(poArgs, 2, &bDisplayPos))
		return Py_BuildException();

	long long iPrice;
	char * str;
	if (!PyTuple_GetString(poArgs, 3, &str))
		return Py_BuildException();

	str_to_numbers(iPrice, str);

	long long iPrice2;
	char* str2;
	if (!PyTuple_GetString(poArgs, 4, &str2))
		return Py_BuildException();

	str_to_numbers(iPrice2, str2);


	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendAddOfflineShopItem(bDisplayPos, itemPos, iPrice, iPrice2);
	return Py_BuildNone();
}

PyObject * netSendRemoveOfflineShopItem(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendRemoveOfflineShopItem(bPos);
	return Py_BuildNone();
}

PyObject * netSendDestroyOfflineShop(PyObject * poSelf, PyObject * poArgs)
{
	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendDestroyOfflineShop();
	return Py_BuildNone();
}

PyObject * netSendRefreshOfflineShopMoney(PyObject * poSelf, PyObject * poArgs)
{
	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendRefreshOfflineShopMoney();
	return Py_BuildNone();
}
PyObject* netSendRefreshOfflineShopChequeMoney(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendRefreshOfflineShopChequeMoney();
	return Py_BuildNone();
}

PyObject * netOfflineShopWithdrawMoney(PyObject * poSelf, PyObject * poArgs)
{
	long long iMoney;
	char *str;
	if (!PyTuple_GetString(poArgs, 0, &str))
		return Py_BuildException();
	str_to_numbers(iMoney, str);

	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopWithdrawMoney(iMoney);
	return Py_BuildNone();
}

PyObject* netOfflineShopWithdrawCheque(PyObject* poSelf, PyObject* poArgs)
{
	long long iCheque;
	char* str;
	if (!PyTuple_GetString(poArgs, 0, &str))
		return Py_BuildException();
	str_to_numbers(iCheque, str);

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopWithdrawCheque(iCheque);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
PyObject* netSendMountNPCAction(PyObject* poSelf, PyObject* poArgs)
{
	int bAction;
	if (!PyTuple_GetInteger(poArgs, 0, &bAction))
		return Py_BuildException();

	int dValue0;
	if (!PyTuple_GetInteger(poArgs, 1, &dValue0))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendMountNPCAction(bAction, dValue0);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
PyObject* netSendBuffNPCCreate(PyObject* poSelf, PyObject* poArgs)
{
	int bAction;
	if (!PyTuple_GetInteger(poArgs, 0, &bAction))
		return Py_BuildException();

	char* szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	int bSex;
	if (!PyTuple_GetInteger(poArgs, 2, &bSex))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendBuffNPCCreate(bAction, szName, bSex);
	return Py_BuildNone();
}

PyObject* netSendBuffNPCAction(PyObject* poSelf, PyObject* poArgs)
{
	int bAction;
	if (!PyTuple_GetInteger(poArgs, 0, &bAction))
		return Py_BuildException();

	int dValue0;
	if (!PyTuple_GetInteger(poArgs, 1, &dValue0))
		return Py_BuildException();

	int dValue1;
	if (!PyTuple_GetInteger(poArgs, 2, &dValue1))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendBuffNPCAction(bAction, dValue0, dValue1);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
PyObject* netSendPetAction(PyObject* poSelf, PyObject* poArgs)
{
	int skillvnum;
	if (!PyTuple_GetInteger(poArgs, 0, &skillvnum))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendPetAction(skillvnum);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
PyObject* netPacketBattlePassCollect(PyObject* poSelf, PyObject* poArgs)
{
	int32_t rewardLevel;
	if (!PyTuple_GetInteger(poArgs, 0, &rewardLevel))
		return Py_BuildException();
	
	int32_t rewardType;
	if (!PyTuple_GetInteger(poArgs, 1, &rewardType))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.PacketBattlePassCollect(rewardLevel, rewardType);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
PyObject* netSendClosePrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendClosePrivateShopPacket();

	return Py_BuildNone();
}

PyObject* netSendClosePrivateShopPanelPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendClosePrivateShopPanelPacket();

	return Py_BuildNone();
}

PyObject* netSendOpenPrivateShopPanelPacket(PyObject* poSelf, PyObject* poArgs)
{
	bool bState;
	if (!PyTuple_GetBoolean(poArgs, 0, &bState))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendOpenPrivateShopPanelPacket(bState);

	return Py_BuildNone();
}

PyObject* netSendPrivateShopStartPacket(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVID;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVID))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopStartPacket(dwVID);

	return Py_BuildNone();
}

PyObject* netSendPrivateShopEndPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopEndPacket();

	return Py_BuildNone();
}

PyObject* netSendPrivateShopBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	if (iPos < 0)
		return Py_BuildNone();

	if (iPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return Py_BuildNone();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopBuyPacket((WORD)iPos);

	return Py_BuildNone();
}

PyObject* netSendWithdrawPrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendWithdrawPrivateShopPacket();

	return Py_BuildNone();
}

PyObject* netSendModifyPrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendModifyPrivateShopPacket();

	return Py_BuildNone();
}

PyObject* netSendItemMovePrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	WORD wPos;
	if (!PyTuple_GetInteger(poArgs, 0, &wPos))
		return Py_BuildException();

	WORD wChangePos;
	if (!PyTuple_GetInteger(poArgs, 1, &wChangePos))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemMovePrivateShopPacket(wPos, wChangePos);

	return Py_BuildNone();
}

PyObject* netSendItemPriceChangePrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	WORD wPos;
	if (!PyTuple_GetInteger(poArgs, 0, &wPos))
		return Py_BuildException();

	long long llGold;
	if (!PyTuple_GetLongLong(poArgs, 1, &llGold))
		return Py_BuildException();

	int iCheque;
	if (!PyTuple_GetInteger(poArgs, 2, &iCheque))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemPriceChangePrivateShopPacket(wPos, llGold, (DWORD)iCheque);

	return Py_BuildNone();
}

PyObject* netSendItemCheckinPrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	WORD wSrcPos;
	if (!PyTuple_GetInteger(poArgs, 0, &wSrcPos))
		return Py_BuildException();

	WORD wSrcWindow;
	if (!PyTuple_GetInteger(poArgs, 1, &wSrcWindow))
		return Py_BuildException();

	long long llGold;
	if (!PyTuple_GetLongLong(poArgs, 2, &llGold))
		return Py_BuildException();

	DWORD dwCheque;
	if (!PyTuple_GetUnsignedLong(poArgs, 3, &dwCheque))
		return Py_BuildException();

	int iDstPos;
	if (!PyTuple_GetInteger(poArgs, 4, &iDstPos))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendItemCheckinPrivateShopPacket(wSrcPos, wSrcWindow, llGold, dwCheque, iDstPos);

	return Py_BuildNone();
}

PyObject* netSendItemCheckoutPrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
	case 1:
	{
		WORD wSrcPos;
		if (!PyTuple_GetInteger(poArgs, 0, &wSrcPos))
			return Py_BuildException();

		CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
		rns.SendItemCheckoutPrivateShopPacket(wSrcPos);
	} break;

	case 2:
	{
		WORD wSrcPos;
		if (!PyTuple_GetInteger(poArgs, 0, &wSrcPos))
			return Py_BuildException();

		WORD wDstPos;
		if (!PyTuple_GetInteger(poArgs, 1, &wDstPos))
			return Py_BuildException();

		CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
		rns.SendItemCheckoutPrivateShopPacket(wSrcPos, wDstPos);
	} break;

	default:
		return Py_BuildException();
	}

	return Py_BuildNone();
}

PyObject* netSendTitleChangePrivateShopPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szTitle;
	if (!PyTuple_GetString(poArgs, 0, &c_szTitle))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendTitleChangePrivateShopPacket(c_szTitle);

	return Py_BuildNone();
}

PyObject* netSendClosePrivateShopSearchPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendClosePrivateShopSearchPacket();

	return Py_BuildNone();
}

PyObject* netSendPrivateShopSearchPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyDictObject{};
	if (!PyTuple_GetObject(poArgs, 0, &pyDictObject))
		return Py_BuildException();

	TPacketCGPrivateShopSearch packet{};
	packet.bUseFilter = false;

	for (int i = 0; i < FILTER_TYPE_MAX_NUM; ++i)
	{
		PyObject* pyValue = PyDict_GetItem(pyDictObject, PyInt_FromLong(i));

		switch (i)
		{
		case FILTER_TYPE_ITEM_VNUM:					packet.Filter.dwVnum = PyLong_AsUnsignedLong(pyValue); break;
		case FILTER_TYPE_OWNER_NAME:				strncpy(packet.Filter.szOwnerName, PyString_AsString(pyValue), sizeof(packet.Filter.szOwnerName)); break;
		case FILTER_TYPE_ITEM_TYPE:					packet.Filter.iItemType = PyLong_AsLong(pyValue); break;
		case FILTER_TYPE_ITEM_SUBTYPE:				packet.Filter.iItemSubType = PyLong_AsLong(pyValue); break;
		case FILTER_TYPE_CLASS:						packet.Filter.iJob = PyLong_AsLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_GENDER:					packet.Filter.iGender = PyLong_AsLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MIN_STACK:					packet.Filter.wMinStack = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MAX_STACK:					packet.Filter.wMaxStack = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MIN_REFINEMENT:			packet.Filter.bMinRefine = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MAX_REFINEMENT:			packet.Filter.bMaxRefine = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MIN_LEVEL:					packet.Filter.dwMinLevel = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_MAX_LEVEL:					packet.Filter.dwMaxLevel = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_ATTR_1:
		case FILTER_TYPE_ATTR_2:
		case FILTER_TYPE_ATTR_3:
		case FILTER_TYPE_ATTR_4:
		case FILTER_TYPE_ATTR_5:
		{
			// We can filter out by type or by type & value
			BYTE bType = PyLong_AsUnsignedLong(PyList_GetItem(pyValue, 0));
			WORD wValue = PyLong_AsUnsignedLong(PyList_GetItem(pyValue, 1));

			if (bType)
			{
				packet.Filter.aAttr[i - FILTER_TYPE_ATTR_1].bType = bType;
				packet.Filter.aAttr[i - FILTER_TYPE_ATTR_1].sValue = wValue;
				packet.bUseFilter = true;
			}

		}break;
		case FILTER_TYPE_SASH_ABSORPTION:			packet.Filter.wSashAbsorption = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_ALCHEMY_LEVEL:				packet.Filter.bAlchemyLevel = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_TYPE_ALCHEMY_CLARITY:			packet.Filter.bAlchemyClarity = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;
		case FILTER_SEARCH_MODE:					packet.Filter.bSearchMode = PyLong_AsUnsignedLong(pyValue); packet.bUseFilter = true; break;

		default: break;
		}
	}

	packet.Filter.iItemValue0 = -1;

	if (packet.Filter.dwVnum && (packet.Filter.iItemType == -1 && packet.Filter.iItemSubType == -1))
	{
		CItemManager::Instance().SelectItemData(packet.Filter.dwVnum);
		CItemData* pItemDataNew = CItemManager::Instance().GetSelectedItemDataPointer();

		if (pItemDataNew)
		{
			packet.Filter.iItemType = pItemDataNew->GetType();
			packet.Filter.iItemSubType = pItemDataNew->GetSubType();
		}

		if (
			(packet.Filter.dwVnum >= 50401 && packet.Filter.dwVnum <= 50406) ||
			(packet.Filter.dwVnum >= 50416 && packet.Filter.dwVnum <= 50421) ||
			(packet.Filter.dwVnum >= 50431 && packet.Filter.dwVnum <= 50436) ||
			(packet.Filter.dwVnum >= 50446 && packet.Filter.dwVnum <= 50451) ||
			(packet.Filter.dwVnum >= 50461 && packet.Filter.dwVnum <= 50466) ||
			(packet.Filter.dwVnum >= 50476 && packet.Filter.dwVnum <= 50481) ||
			(packet.Filter.dwVnum >= 50491 && packet.Filter.dwVnum <= 50496) ||
			(packet.Filter.dwVnum >= 50506 && packet.Filter.dwVnum <= 50511) ||
			(packet.Filter.dwVnum >= 50530 && packet.Filter.dwVnum <= 50535)

			)
			packet.Filter.iItemValue0 = pItemDataNew->GetValue(0);
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopSearchPacket(&packet);

	return Py_BuildNone();
}

PyObject* netSendPrivateShopSearchBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* pyListObject{};
	if (!PyTuple_GetObject(poArgs, 0, &pyListObject))
		return Py_BuildException();

	Py_ssize_t pyListSize = PyList_Size(pyListObject);
	if (!pyListSize)
		return Py_BuildException();

	if (pyListSize > SELECTED_ITEM_MAX_NUM)
		return Py_BuildException();

	int arSelectedItem[SELECTED_ITEM_MAX_NUM];
	std::fill(std::begin(arSelectedItem), std::begin(arSelectedItem) + SELECTED_ITEM_MAX_NUM, -1);

	for (Py_ssize_t i = 0; i < pyListSize; ++i)
	{
		PyObject* pyPos = PyList_GetItem(pyListObject, i);
		WORD wPos = PyLong_AsUnsignedLong(pyPos);

		arSelectedItem[i] = wPos;
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopSearchBuyPacket(&arSelectedItem[0]);

	return Py_BuildNone();
}

PyObject* netSendPrivateShopMarketItemPriceDataReq(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopMarketItemPriceDataReqPacket();

	return Py_BuildNone();
}

PyObject* netSendPrivateShopMarketItemPriceReq(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopMarketItemPriceReqPacket(dwVnum);

	return Py_BuildNone();
}

PyObject* netSendPrivateShopWarpRequest(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopWarpRequest();

	return Py_BuildNone();
}

PyObject* netSendPrivateShopSlotUnlockRequest(PyObject* poSelf, PyObject* poArgs)
{
	WORD wSrcPos;
	if (!PyTuple_GetInteger(poArgs, 0, &wSrcPos))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPrivateShopSlotUnlockRequest(wSrcPos);

	return Py_BuildNone();
}
#endif

PY_METHOD(netSetVersionId) {
	uint32_t versionId;
	if (!PyArg_ParseTuple(args, "I", &versionId))
		return Py_BuildException();

	CAccountConnector::Instance().SetVersionId(versionId);
	return Py_BuildNone();
}

PY_METHOD(netGetVersionId) {
    return Py_BuildValue("I", CAccountConnector::Instance().GetVersionId());
}

void initnet()
{
	VMProtectBeginUltra("initnet");
	static PyMethodDef s_methods[] =
	{
		{ VMProtectDecryptStringA("GetBettingGuildWarValue"),			netGetBettingGuildWarValue,				METH_VARARGS },
		{ VMProtectDecryptStringA("EnableChatInsultFilter"),				netEnableChatInsultFilter,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetServerInfo"),						netSetServerInfo,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetServerInfo"),						netGetServerInfo,						METH_VARARGS },
		{ VMProtectDecryptStringA("PreserveServerCommand"),				netPreserveServerCommand,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetPreservedServerCommand"),			netGetPreservedServerCommand,			METH_VARARGS },

		{ VMProtectDecryptStringA("StartGame"),							netStartGame,							METH_VARARGS },
		{ VMProtectDecryptStringA("Warp"),								netWarp,								METH_VARARGS },
		{ VMProtectDecryptStringA("IsTest"),								netIsTest,								METH_VARARGS },
		{ VMProtectDecryptStringA("SetMarkServer"),						netSetMarkServer,						METH_VARARGS },
		{ VMProtectDecryptStringA("IsChatInsultIn"),						netIsChatInsultIn,						METH_VARARGS },
		{ VMProtectDecryptStringA("IsInsultIn"),							netIsInsultIn,							METH_VARARGS },
		{ VMProtectDecryptStringA("LoadInsultList"),						netLoadInsultList,						METH_VARARGS },
		{ VMProtectDecryptStringA("UploadMark"),							netUploadMark,							METH_VARARGS },
		{ VMProtectDecryptStringA("UploadSymbol"),						netUploadSymbol,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetGuildID"),							netGetGuildID,							METH_VARARGS },
		{ VMProtectDecryptStringA("GetEmpireID"),						netGetEmpireID,							METH_VARARGS },
		{ VMProtectDecryptStringA("GetMainActorVID"),					netGetMainActorVID,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetMainActorRace"),					netGetMainActorRace,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetMainActorEmpire"),					netGetMainActorEmpire,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetMainActorSkillGroup"),				netGetMainActorSkillGroup,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetAccountCharacterSlotDataInteger"),	netGetAccountCharacterSlotDataInteger,	METH_VARARGS },
		{ VMProtectDecryptStringA("GetAccountCharacterSlotDataString"),	netGetAccountCharacterSlotDataString,	METH_VARARGS },

		// FIELD_MUSIC
		{ VMProtectDecryptStringA("GetFieldMusicFileName"),				netGetFieldMusicFileName,				METH_VARARGS },
		{ VMProtectDecryptStringA("GetFieldMusicVolume"),				netGetFieldMusicVolume,					METH_VARARGS },
		// END_OF_FIELD_MUSIC

		{ VMProtectDecryptStringA("ToggleGameDebugInfo"),				netToggleGameDebugInfo,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetLoginInfo"),						netSetLoginInfo,						METH_VARARGS },
		{ VMProtectDecryptStringA("SetPhaseWindow"),						netSetPhaseWindow,						METH_VARARGS },
		{ VMProtectDecryptStringA("ClearPhaseWindow"),					netClearPhaseWindow,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetServerCommandParserWindow"),		netSetServerCommandParserWindow,		METH_VARARGS },
		{ VMProtectDecryptStringA("SetAccountConnectorHandler"),			netSetAccountConnectorHandler,			METH_VARARGS },
		{ VMProtectDecryptStringA("SetHandler"),							netSetHandler,							METH_VARARGS },
		{ VMProtectDecryptStringA("SetTCPRecvBufferSize"),				netSetTCPRecvBufferSize,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetTCPSendBufferSize"),				netSetTCPSendBufferSize,				METH_VARARGS },
		{ VMProtectDecryptStringA("SetUDPRecvBufferSize"),				netSetUDPRecvBufferSize,				METH_VARARGS },
		{ VMProtectDecryptStringA("DirectEnter"),						netDirectEnter,							METH_VARARGS },

		{ VMProtectDecryptStringA("LogOutGame"),							netLogOutGame,							METH_VARARGS },
		{ VMProtectDecryptStringA("ExitGame"),							netExitGame,							METH_VARARGS },
		{ VMProtectDecryptStringA("ExitApplication"),					netExitApplication,						METH_VARARGS },
		{ VMProtectDecryptStringA("ConnectTCP"),							netConnectTCP,							METH_VARARGS },
		{ VMProtectDecryptStringA("ConnectUDP"),							netConnectUDP,							METH_VARARGS },
		{ VMProtectDecryptStringA("ConnectToAccountServer"),				netConnectToAccountServer,				METH_VARARGS },

		{ VMProtectDecryptStringA("SendLoginPacket"),					netSendLoginPacket,						METH_VARARGS },
		{ VMProtectDecryptStringA("SendSelectEmpirePacket"),				netSendSelectEmpirePacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendSelectCharacterPacket"),			netSendSelectCharacterPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendChangeNamePacket"),				netSendChangeNamePacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendCreateCharacterPacket"),			netSendCreateCharacterPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendDestroyCharacterPacket"),			netSendDestroyCharacterPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendEnterGamePacket"),				netSendEnterGamePacket,					METH_VARARGS },

		{ VMProtectDecryptStringA("SendItemUsePacket"),					netSendItemUsePacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemUseToItemPacket"),			netSendItemUseToItemPacket,				METH_VARARGS },
#ifdef ENABLE_INSTANT_CHEST_OPEN
		{ VMProtectDecryptStringA("SendOpenChestPacket"),				netSendOpenChestPacket,					METH_VARARGS },
#endif
#ifdef ENABLE_INSTANT_INGOT_OPEN
		{ VMProtectDecryptStringA("SendOpenIngotPacket"),				netSendOpenIngotPacket,					METH_VARARGS },
#endif
		{ VMProtectDecryptStringA("SendItemDropPacket"),					netSendItemDropPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemDropPacketNew"),				netSendItemDropPacketNew,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendElkDropPacket"),					netSendElkDropPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendGoldDropPacketNew"),				netSendGoldDropPacketNew,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemMovePacket"),					netSendItemMovePacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemPickUpPacket"),				netSendItemPickUpPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGiveItemPacket"),					netSendGiveItemPacket,					METH_VARARGS },

		{ VMProtectDecryptStringA("SetOfflinePhase"),					netSetOfflinePhase,						METH_VARARGS },
		{ VMProtectDecryptStringA("Disconnect"),							netDisconnect,							METH_VARARGS },
		{ VMProtectDecryptStringA("IsConnect"),							netIsConnect,							METH_VARARGS },

		{ VMProtectDecryptStringA("SendChatPacket"),						netSendChatPacket,						METH_VARARGS },
		{ VMProtectDecryptStringA("SendEmoticon"),						netSendEmoticon,						METH_VARARGS },
		{ VMProtectDecryptStringA("SendWhisperPacket"),					netSendWhisperPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendMobileMessagePacket"),			netSendMobileMessagePacket,				METH_VARARGS },

		{ VMProtectDecryptStringA("SendCharacterPositionPacket"),		netSendCharacterPositionPacket,			METH_VARARGS },

		{ VMProtectDecryptStringA("SendShopEndPacket"),					netSendShopEndPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendShopBuyPacket"),					netSendShopBuyPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendShopSellPacket"),					netSendShopSellPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendShopSellPacketNew"),				netSendShopSellPacketNew,				METH_VARARGS },

		{ VMProtectDecryptStringA("SendExchangeStartPacket"),			netSendExchangeStartPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendExchangeItemAddPacket"), netSendExchangeItemAddPacket, METH_VARARGS },
		{ VMProtectDecryptStringA("SendExchangeItemDelPacket"),			netSendExchangeItemDelPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendExchangeElkAddPacket"),			netSendExchangeElkAddPacket,			METH_VARARGS },
#ifdef ENABLE_CHEQUE_SYSTEM
		{ VMProtectDecryptStringA("SendExchangeChequeAddPacket"),		netSendExchangeChequeAddPacket,			METH_VARARGS },
#endif
		{ VMProtectDecryptStringA("SendExchangeAcceptPacket"),			netSendExchangeAcceptPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendExchangeExitPacket"),				netSendExchangeExitPacket,				METH_VARARGS },

		{ VMProtectDecryptStringA("SendOnClickPacket"),					netOnClickPacket,						METH_VARARGS },

		// Emoticon String
		{ VMProtectDecryptStringA("RegisterEmoticonString"),				netRegisterEmoticonString,				METH_VARARGS },

		// Messenger
		{ VMProtectDecryptStringA("SendMessengerAddByVIDPacket"),		netSendMessengerAddByVIDPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendMessengerAddByNamePacket"),		netSendMessengerAddByNamePacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendMessengerRemovePacket"),			netSendMessengerRemovePacket,			METH_VARARGS },
		
		// Party
		{ VMProtectDecryptStringA("SendPartyInvitePacket"),				netSendPartyInvitePacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyInviteAnswerPacket"),		netSendPartyInviteAnswerPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyExitPacket"),				netSendPartyExitPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyRemovePacket"),				netSendPartyRemovePacketPID,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyRemovePacketPID"),			netSendPartyRemovePacketPID,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyRemovePacketVID"),			netSendPartyRemovePacketVID,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartySetStatePacket"),			netSendPartySetStatePacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyUseSkillPacket"),			netSendPartyUseSkillPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPartyParameterPacket"),			netSendPartyParameterPacket,			METH_VARARGS },
		
		// Safebox
		{ VMProtectDecryptStringA("SendSafeboxSaveMoneyPacket"),			netSendSafeboxSaveMoneyPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendSafeboxWithdrawMoneyPacket"),		netSendSafeboxWithdrawMoneyPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendSafeboxCheckinPacket"),			netSendSafeboxCheckinPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendSafeboxCheckoutPacket"),			netSendSafeboxCheckoutPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendSafeboxItemMovePacket"),			netSendSafeboxItemMovePacket,			METH_VARARGS },
		
		// Mall
		{ VMProtectDecryptStringA("SendMallCheckoutPacket"),				netSendMallCheckoutPacket,				METH_VARARGS },
		
		// Guild
		{ VMProtectDecryptStringA("SendAnswerMakeGuildPacket"),				netSendAnswerMakeGuildPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendQuestInputStringPacket"),				netSendQuestInputStringPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendQuestConfirmPacket"),					netSendQuestConfirmPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildAddMemberPacket"),				netSendGuildAddMemberPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildRemoveMemberPacket"),			netSendGuildRemoveMemberPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildChangeGradeNamePacket"),			netSendGuildChangeGradeNamePacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildChangeGradeAuthorityPacket"),	netSendGuildChangeGradeAuthorityPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildOfferPacket"),					netSendGuildOfferPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildPostCommentPacket"),				netSnedGuildPostCommentPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildDeleteCommentPacket"),			netSnedGuildDeleteCommentPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildRefreshCommentsPacket"),			netSendGuildRefreshCommentsPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildChangeMemberGradePacket"),		netSendGuildChangeMemberGradePacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildUseSkillPacket"),				netSendGuildUseSkillPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildChangeMemberGeneralPacket"),		netSendGuildChangeMemberGeneralPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildInviteAnswerPacket"),			netSendGuildInviteAnswerPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildChargeGSPPacket"),				netSendGuildChargeGSPPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildDepositMoneyPacket"),			netSendGuildDepositMoneyPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendGuildWithdrawMoneyPacket"),			netSendGuildWithdrawMoneyPacket,			METH_VARARGS },
		
		// Refine
		{ VMProtectDecryptStringA("SendRequestRefineInfoPacket"),			netSendRequestRefineInfoPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendRefinePacket"),						netSendRefinePacket,						METH_VARARGS },
		{ VMProtectDecryptStringA("SendSelectItemPacket"),					netSendSelectItemPacket,					METH_VARARGS },
		
		// SYSTEM
		{ VMProtectDecryptStringA("SetPacketSequenceMode"),					netSetPacketSequenceMode,					METH_VARARGS },
		{ VMProtectDecryptStringA("SetEmpireLanguageMode"),					netSetEmpireLanguageMode,					METH_VARARGS },
		
		// For Test
		{ VMProtectDecryptStringA("SetSkillGroupFake"),						netSetSkillGroupFake,						METH_VARARGS },
		
		// Guild Symbol
		{ VMProtectDecryptStringA("SendGuildSymbol"),						netSendGuildSymbol,							METH_VARARGS },
		{ VMProtectDecryptStringA("DisconnectUploader"),						netDisconnectUploader,						METH_VARARGS },
		{ VMProtectDecryptStringA("RecvGuildSymbol"),						netRecvGuildSymbol,							METH_VARARGS },
		
		// Log
		{ VMProtectDecryptStringA("RegisterErrorLog"),						netRegisterErrorLog,						METH_VARARGS },
#ifdef ENABLE_REMOVE_ITEM
		{ VMProtectDecryptStringA("SendRemoveItemPacket"),					netSendRemoveItemPacket,					METH_VARARGS },
#endif
#ifdef ENABLE_RESP_SYSTEM
		{ VMProtectDecryptStringA("SendRespFetchDropPacket"),				netSendRespFetchDropPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendRespFetchRespPacket"),				netSendRespFetchRespPacket,					METH_VARARGS },
		{ VMProtectDecryptStringA("SendRespTeleportPacket"),					netSendRespTeleportPacket,					METH_VARARGS },
#endif
#ifdef ENABLE_ODLAMKI_SYSTEM
		{ VMProtectDecryptStringA("SendOdlamki"),							netSendOdlamkiItemPacket,					METH_VARARGS },
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
		{ VMProtectDecryptStringA("SendSearch"),								netSendSearchPacket,						METH_VARARGS },
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
		{ VMProtectDecryptStringA("SendWonExchangeSellPacket"),				netSendWonExchangeSellPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendWonExchangeBuyPacket"),				netSendWonExchangeBuyPacket,				METH_VARARGS },
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
		{ VMProtectDecryptStringA("SendBuffNPCCreate"), netSendBuffNPCCreate, METH_VARARGS },
		{ VMProtectDecryptStringA("SendBuffNPCAction"), netSendBuffNPCAction, METH_VARARGS },
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		{ VMProtectDecryptStringA("SendMountNPCAction"), netSendMountNPCAction, METH_VARARGS },
#endif
#ifdef WEEKLY_RANK_BYLUZER
		{ VMProtectDecryptStringA("SelectWeeklyRankPage"), netSelectWeeklyRankPage, METH_VARARGS },
#endif
#ifdef TITLE_SYSTEM_BYLUZER
		{ VMProtectDecryptStringA("SendSelectTitle"),	netSendSelectTitle,		METH_VARARGS },
#endif
#ifdef ENABLE_DROP_INFO
		{ VMProtectDecryptStringA("SendDropInfoPacket"), netSendDropInfoPacket, METH_VARARGS },
#endif
#ifdef ENABLE_PERFORMANCE_IMPROVEMENTS_NEW
		{ VMProtectDecryptStringA("LoadResourcesInCache"),					netLoadResourcesInCache,					METH_VARARGS },
		{ VMProtectDecryptStringA("RegisterSkills"),							netRegisterSkills,							METH_VARARGS },
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
		{ VMProtectDecryptStringA("SendPetAction"),						netSendPetAction,				METH_VARARGS },
#endif
#ifdef ENABLE_LRN_LOCATION_SYSTEM
		{ VMProtectDecryptStringA("SendLocationManagerPacket"),				netSendLocationManagerPacket,				METH_VARARGS },
#endif
#ifdef ENABLE_ITEMSHOP
		{ VMProtectDecryptStringA("SendItemShopOpenPacket"), netSendItemShopOpenPacket, METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemShopBuyPacket"), netSendItemShopBuyPacket, METH_VARARGS },
#endif

#ifdef ENABLE_TOMBOLA
		{ VMProtectDecryptStringA("SendTombolaStartPacket"),		netSendTombolaStartPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendTombolaOpenPacket"),		netSendTombolaOpenPacket,		METH_VARARGS },
#endif

#if defined(__BL_MOVE_CHANNEL__)		
		{ VMProtectDecryptStringA("SetChannelName"),						netSetChannelName,						METH_VARARGS },
		{ VMProtectDecryptStringA("GetChannelName"),						netGetChannelName,						METH_VARARGS },

		{ VMProtectDecryptStringA("GetChannelNumber"),					netGetChannelNumber,					METH_VARARGS },
		{ VMProtectDecryptStringA("GetChannelCount"),					netGetChannelCount,						METH_VARARGS },

		{ VMProtectDecryptStringA("SetMapIndex"),						netSetMapIndex,							METH_VARARGS },
		{ VMProtectDecryptStringA("GetMapIndex"),						netGetMapIndex,							METH_VARARGS },

		{ VMProtectDecryptStringA("SetServerName"),						netSetServerName,						METH_VARARGS },
		{ VMProtectDecryptStringA("ClearServerInfo"),					netClearServerInfo,						METH_VARARGS },

		{ VMProtectDecryptStringA("MoveChannelGame"),					netMoveChannelGame,						METH_VARARGS },
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
		{ VMProtectDecryptStringA("SendOfflineShopEndPacket"),				netSendOfflineShopEndPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendOfflineShopBuyPacket"),				netSendOfflineShopBuyPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendAddOfflineShopItem"),					netSendAddOfflineShopItem,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendRemoveOfflineShopItem"),				netSendRemoveOfflineShopItem,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendDestroyOfflineShop"),					netSendDestroyOfflineShop,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendRefreshOfflineShop"),					netSendRefreshOfflineShop,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendRefreshOfflineShopMoney"),			netSendRefreshOfflineShopMoney,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendRefreshOfflineShopChequeMoney"),			netSendRefreshOfflineShopChequeMoney,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendOfflineShopWithdrawMoney"),			netOfflineShopWithdrawMoney,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendOfflineShopWithdrawCheque"),			netOfflineShopWithdrawCheque,			METH_VARARGS },
#endif

#ifdef ENABLE_LRN_BATTLE_PASS
		{ VMProtectDecryptStringA("PacketBattlePassCollect"),				netPacketBattlePassCollect,					METH_VARARGS },
#endif

#ifdef ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL
		{ VMProtectDecryptStringA("SendClosePrivateShopPacket"),				netSendClosePrivateShopPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendClosePrivateShopPanelPacket"),		netSendClosePrivateShopPanelPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendOpenPrivateShopPanelPacket"),			netSendOpenPrivateShopPanelPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopStartPacket"),				netSendPrivateShopStartPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopEndPacket"),				netSendPrivateShopEndPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopBuyPacket"),				netSendPrivateShopBuyPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendWithdrawPrivateShopPacket"),			netSendWithdrawPrivateShopPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendModifyPrivateShopPacket"),			netSendModifyPrivateShopPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemMovePrivateShopPacket"),			netSendItemMovePrivateShopPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemPriceChangePrivateShopPacket"),	netSendItemPriceChangePrivateShopPacket,	METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemCheckinPrivateShopPacket"),		netSendItemCheckinPrivateShopPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendItemCheckoutPrivateShopPacket"),		netSendItemCheckoutPrivateShopPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendTitleChangePrivateShopPacket"),		netSendTitleChangePrivateShopPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopWarpRequest"),				netSendPrivateShopWarpRequest,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopSlotUnlockRequest"),		netSendPrivateShopSlotUnlockRequest,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendClosePrivateShopSearchPacket"),		netSendClosePrivateShopSearchPacket,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopSearchPacket"),			netSendPrivateShopSearchPacket,				METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopSearchBuyPacket"),			netSendPrivateShopSearchBuyPacket,			METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopMarketItemPriceDataReq"),	netSendPrivateShopMarketItemPriceDataReq,		METH_VARARGS },
		{ VMProtectDecryptStringA("SendPrivateShopMarketItemPriceReq"),		netSendPrivateShopMarketItemPriceReq,			METH_VARARGS },
#endif

		PY_METHOD_DEF("SetVersionId", netSetVersionId),
		PY_METHOD_DEF("GetVersionId", netGetVersionId),
		{ NULL,										NULL,										NULL },
	};

	PyObject* poModule = Py_InitModule(VMProtectDecryptStringA("net"), s_methods);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_NONE"), CPythonNetworkStream::ERROR_NONE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_CONNECT_MARK_SERVER"), CPythonNetworkStream::ERROR_CONNECT_MARK_SERVER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_LOAD_MARK"), CPythonNetworkStream::ERROR_LOAD_MARK);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_MARK_WIDTH"), CPythonNetworkStream::ERROR_MARK_WIDTH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_MARK_HEIGHT"), CPythonNetworkStream::ERROR_MARK_HEIGHT);

	// MARK_BUG_FIX
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_MARK_UPLOAD_NEED_RECONNECT"), CPythonNetworkStream::ERROR_MARK_UPLOAD_NEED_RECONNECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ERROR_MARK_CHECK_NEED_RECONNECT"), CPythonNetworkStream::ERROR_MARK_CHECK_NEED_RECONNECT);
	// END_OF_MARK_BUG_FIX

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_LOGIN"), CPythonNetworkStream::PHASE_WINDOW_LOGIN);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_SELECT"), CPythonNetworkStream::PHASE_WINDOW_SELECT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_CREATE"), CPythonNetworkStream::PHASE_WINDOW_CREATE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_LOAD"), CPythonNetworkStream::PHASE_WINDOW_LOAD);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_GAME"), CPythonNetworkStream::PHASE_WINDOW_GAME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_EMPIRE"), CPythonNetworkStream::PHASE_WINDOW_EMPIRE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("PHASE_WINDOW_LOGO"), CPythonNetworkStream::PHASE_WINDOW_LOGO);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_ID"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ID);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_NAME"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_NAME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_RACE"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_RACE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_LEVEL"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_LEVEL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_STR"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_STR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_DEX"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_DEX);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_INT"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_INT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_HTH"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HTH);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_PLAYTIME"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PLAYTIME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_FORM"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_FORM);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_ADDR"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ADDR);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_PORT"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PORT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_GUILD_ID"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_ID);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_GUILD_NAME"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_NAME);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_HAIR"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("ACCOUNT_CHARACTER_SLOT_ACCE"), CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ACCE);
#endif
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SERVER_COMMAND_LOG_OUT"), CPythonNetworkStream::SERVER_COMMAND_LOG_OUT);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER"), CPythonNetworkStream::SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("SERVER_COMMAND_QUIT"), CPythonNetworkStream::SERVER_COMMAND_QUIT);

	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMPIRE_A"), 1);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMPIRE_B"), 2);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("EMPIRE_C"), 3);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL"), DS_SUB_HEADER_REFINE_FAIL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE"), DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL"), DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY"), DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL"), DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL"), DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL);
	PyModule_AddIntConstant(poModule, VMProtectDecryptStringA("DS_SUB_HEADER_REFINE_SUCCEED"), DS_SUB_HEADER_REFINE_SUCCEED);
	VMProtectEnd();
}
//martysama0134's ceqyqttoaf71vasf9t71218
