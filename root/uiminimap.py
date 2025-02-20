#-*- coding: iso-8859-1 -*-
import ui
import uiScriptLocale
import wndMgr
import player
import miniMap
import localeInfo
import net
import app
# import colorInfo
import constInfo
import uiToolTip
# import background
import interfaceModule
import time
# import chat

class MapTextToolTip(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)

		textLine = ui.TextLine()
		textLine.SetParent(self)
		textLine.SetHorizontalAlignCenter()
		textLine.SetOutline()
		textLine.SetHorizontalAlignRight()
		textLine.Show()
		self.textLine = textLine

	def __del__(self):
		ui.Window.__del__(self)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetTooltipPosition(self, PosX, PosY):
		if localeInfo.IsARABIC():
			w, h = self.textLine.GetTextSize()
			self.textLine.SetPosition(PosX - w - 5, PosY)
		else:
			self.textLine.SetPosition(PosX - 5, PosY)

	def SetTextColor(self, TextColor):
		self.textLine.SetPackedFontColor(TextColor)

	def GetTextSize(self):
		return self.textLine.GetTextSize()

class AtlasWindow(ui.ScriptWindow):

	class AtlasRenderer(ui.Window):
		def __init__(self):
			ui.Window.__init__(self)
			self.AddFlag("not_pick")

		def OnUpdate(self):
			miniMap.UpdateAtlas()

		def OnRender(self):
			(x, y) = self.GetGlobalPosition()
			fx = float(x)
			fy = float(y)
			miniMap.RenderAtlas(fx, fy)

		def HideAtlas(self):
			miniMap.HideAtlas()

		def ShowAtlas(self):
			miniMap.ShowAtlas()

	def __init__(self):
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Hide()
		self.infoGuildMark = ui.MarkBox()
		self.infoGuildMark.Hide()
		self.AtlasMainWindow = None
		self.mapName = ""
		self.board = 0

		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetMapName(self, mapName):
		if 949==app.GetDefaultCodePage():
			try:
				self.board.SetTitleName(localeInfo.MINIMAP_ZONE_NAME_DICT[mapName])
			except:
				pass

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/AtlasWindow.py")
		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.LoadScript")

		try:
			self.board = self.GetChild("board")

		except:
			import exception
			exception.Abort("AtlasWindow.LoadWindow.BindObject")

		self.AtlasMainWindow = self.AtlasRenderer()
		self.board.SetCloseEvent(self.Hide)
		self.AtlasMainWindow.SetParent(self.board)
		self.AtlasMainWindow.SetPosition(7, 30)
		self.tooltipInfo.SetParent(self.board)
		self.infoGuildMark.SetParent(self.board)
		self.SetPosition(40, 120)
		self.Hide()

		miniMap.RegisterAtlasWindow(self)

	def Destroy(self):
		miniMap.UnregisterAtlasWindow()
		self.ClearDictionary()
		self.AtlasMainWindow = None
		self.tooltipAtlasClose = 0
		self.tooltipInfo = None
		self.infoGuildMark = None
		self.board = None

	def OnUpdate(self):
		(x, y) = self.GetGlobalPosition()
		(mouseX, mouseY) = wndMgr.GetMousePosition()
		(bFind, sName, iPosX, iPosY, dwTextColor, dwGuildID) = miniMap.GetAtlasInfo(mouseX, mouseY)
			
		if not self.tooltipInfo:
			return

		if not self.infoGuildMark:
			return

		self.infoGuildMark.Hide()
		self.tooltipInfo.Hide()

		if self.board.IsIn():
			self.tooltipInfo.SetText("|cFFffffff%s|r (|cFFfcc923%d, %d|r)" % (sName, iPosX, iPosY))
			self.tooltipInfo.SetTooltipPosition(mouseX - x, mouseY - y)
			self.tooltipInfo.Show()
			self.tooltipInfo.SetTop()
		else:
			self.tooltipInfo.Hide()

		if False == self.board.IsIn():
			return

		if False == bFind:
			return

		if "empty_guild_area" == sName:
			sName = localeInfo.GUILD_EMPTY_AREA

		if 0 != dwGuildID:
			textWidth, textHeight = self.tooltipInfo.GetTextSize()
			self.infoGuildMark.SetIndex(dwGuildID)
			self.infoGuildMark.SetPosition(mouseX - x - textWidth - 18 - 5, mouseY - y)
			self.infoGuildMark.Show()

	def Hide(self):
		if self.AtlasMainWindow:
			self.AtlasMainWindow.HideAtlas()
			self.AtlasMainWindow.Hide()
		ui.ScriptWindow.Hide(self)

	def Show(self):
		if self.AtlasMainWindow:
			(bGet, iSizeX, iSizeY) = miniMap.GetAtlasSize()
			if bGet:
				self.SetSize(iSizeX + 15, iSizeY + 38)

				if localeInfo.IsARABIC():
					self.board.SetPosition(iSizeX+15, 0)

				self.board.SetSize(iSizeX + 15, iSizeY + 38)
				#self.AtlasMainWindow.SetSize(iSizeX, iSizeY)
				self.AtlasMainWindow.ShowAtlas()
				self.AtlasMainWindow.Show()
		ui.ScriptWindow.Show(self)
		self.SetTop()

	def SetCenterPositionAdjust(self, x, y):
		self.SetPosition((wndMgr.GetScreenWidth() - self.GetWidth()) / 2 + x, (wndMgr.GetScreenHeight() - self.GetHeight()) / 2 + y)

	def OnPressEscapeKey(self):
		self.Hide()
		return True

def __RegisterMiniMapColor(type, rgb):
	miniMap.RegisterColor(type, rgb[0], rgb[1], rgb[2])

class MiniMap(ui.ScriptWindow):

	CANNOT_SEE_INFO_MAP_DICT = {
		"metin2_map_monkeydungeon" : False,
		"metin2_map_monkeydungeon_02" : False,
		"metin2_map_monkeydungeon_03" : False,
		"metin2_map_devilsCatacomb" : False,
	}

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__Initialize()

		miniMap.Create()
		miniMap.SetScale(2.0)

		self.AtlasWindow = AtlasWindow()
		self.AtlasWindow.LoadWindow()
		self.AtlasWindow.Hide()

		self.tooltipMiniMapOpen = MapTextToolTip()
		self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)
		self.tooltipMiniMapOpen.Show()
		# self.tooltipMiniMapClose = MapTextToolTip()
		# self.tooltipMiniMapClose.SetText(localeInfo.UI_CLOSE)
		# self.tooltipMiniMapClose.Show()
		# self.tooltipScaleUp = MapTextToolTip()
		# self.tooltipScaleUp.SetText(localeInfo.MINIMAP_INC_SCALE)
		# self.tooltipScaleUp.Show()
		# self.tooltipScaleDown = MapTextToolTip()
		# self.tooltipScaleDown.SetText(localeInfo.MINIMAP_DEC_SCALE)
		# self.tooltipScaleDown.Show()
		self.tooltipAtlasOpen = MapTextToolTip()
		# self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		# self.tooltipAtlasOpen.Show()
		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
			# self.tooltipDungeonInfoOpen = MapTextToolTip()
			# self.tooltipDungeonInfoOpen.SetText(localeInfo.DUNGEON_INFO_MINIMAP_TOOL_TIP)
			# self.tooltipDungeonInfoOpen.Show()
		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		if not miniMap.IsAtlas():
			# self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_SHOW_AREAMAP)
		# else:
			self.tooltipAtlasOpen.SetText(localeInfo.MINIMAP_CAN_NOT_SHOW_AREAMAP)

		self.tooltipInfo = MapTextToolTip()
		self.tooltipInfo.Show()

		self.mapName = ""

		self.isLoaded = 0
		self.canSeeInfo = True

		# AUTOBAN
		self.imprisonmentDuration = 0
		self.imprisonmentEndTime = 0
		self.imprisonmentEndTimeText = ""
		# END_OF_AUTOBAN

		# self.interface = interfaceModule.Interface()

	def __del__(self):
		miniMap.Destroy()
		ui.ScriptWindow.__del__(self)

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	self.interface = None

	def __Initialize(self):
		self.positionInfo = 0
		self.observerCount = 0

		self.OpenWindow = 0
		self.CloseWindow = 0
		self.ScaleUpButton = 0
		self.ScaleDownButton = 0
		self.MiniMapHideButton = 0
		self.MiniMapShowButton = 0
		self.AtlasShowButton = 0
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.DungeonInfoShowButton = 0

		self.tooltipMiniMapOpen = 0
		# self.tooltipMiniMapClose = 0
		# self.tooltipScaleUp = 0
		# self.tooltipScaleDown = 0
		self.tooltipAtlasOpen = 0
		if app.ENABLE_DUNGEON_INFO_SYSTEM:
			self.tooltipDungeonInfoOpen = 0
		self.tooltipInfo = None
		self.serverInfo = None
		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	self.interface = None

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def SetMapName(self, mapName):
		self.mapName=mapName
		self.AtlasWindow.SetMapName(mapName)

		if self.CANNOT_SEE_INFO_MAP_DICT.has_key(mapName):
			self.canSeeInfo = False
			self.HideMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP_CANNOT_SEE)
		else:
			self.canSeeInfo = True
			self.ShowMiniMap()
			self.tooltipMiniMapOpen.SetText(localeInfo.MINIMAP)

	# AUTOBAN
	def SetImprisonmentDuration(self, duration):
		self.imprisonmentDuration = duration
		self.imprisonmentEndTime = app.GetGlobalTimeStamp() + duration

		self.__UpdateImprisonmentDurationText()

	def __UpdateImprisonmentDurationText(self):
		restTime = max(self.imprisonmentEndTime - app.GetGlobalTimeStamp(), 0)

		imprisonmentEndTimeText = localeInfo.SecondToDHM(restTime)
		if imprisonmentEndTimeText != self.imprisonmentEndTimeText:
			self.imprisonmentEndTimeText = imprisonmentEndTimeText
			self.serverInfo.SetText("%s: %s" % (uiScriptLocale.AUTOBAN_QUIZ_REST_TIME, self.imprisonmentEndTimeText))
	# END_OF_AUTOBAN

	def Show(self):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			if localeInfo.IsARABIC():
				pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "Minimap.py")
			else:
				pyScrLoader.LoadScriptFile(self, "UIScript/MiniMap.py")
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.LoadScript")

		try:
			self.OpenWindow = self.GetChild("OpenWindow")
			self.MiniMapWindow = self.GetChild("MiniMapWindow")
			self.ScaleUpButton = self.GetChild("ScaleUpButton")
			self.ScaleDownButton = self.GetChild("ScaleDownButton")
			self.MiniMapHideButton = self.GetChild("MiniMapHideButton")
			self.AtlasShowButton = self.GetChild("AtlasShowButton")
			self.CloseWindow = self.GetChild("CloseWindow")
			self.MiniMapShowButton = self.GetChild("MiniMapShowButton")
			self.positionInfo = self.GetChild("PositionInfo")
			self.observerCount = self.GetChild("ObserverCount")
			self.serverInfo = self.GetChild("ServerInfo")

			#ZMIANA KANALU
			self.buttonChangeChannel = {
				1:self.GetChild("Channel1"),
				2:self.GetChild("Channel2"),
				3:self.GetChild("Channel3"),
				4:self.GetChild("Channel4"),
				5:self.GetChild("Channel5"),
				6:self.GetChild("Channel6")
			}
			#ZMIANA KANALU
		except:
			import exception
			exception.Abort("MiniMap.LoadWindow.Bind")

		if constInfo.MINIMAP_POSITIONINFO_ENABLE==0:
			self.positionInfo.Hide()

		self.serverInfo.SetText(net.GetServerInfo())
		self.ScaleUpButton.SetEvent(ui.__mem_func__(self.ScaleUp))
		self.ScaleDownButton.SetEvent(ui.__mem_func__(self.ScaleDown))
		self.MiniMapHideButton.SetEvent(ui.__mem_func__(self.HideMiniMap))
		self.MiniMapShowButton.SetEvent(ui.__mem_func__(self.ShowMiniMap))

		#ZMIANA KANALU
		self.buttonChangeChannel[1].SetEvent(ui.__mem_func__(self.ChangeChannel1))
		self.buttonChangeChannel[2].SetEvent(ui.__mem_func__(self.ChangeChannel2))
		self.buttonChangeChannel[3].SetEvent(ui.__mem_func__(self.ChangeChannel3))
		self.buttonChangeChannel[4].SetEvent(ui.__mem_func__(self.ChangeChannel4))
		self.buttonChangeChannel[5].SetEvent(ui.__mem_func__(self.ChangeChannel5))
		self.buttonChangeChannel[6].SetEvent(ui.__mem_func__(self.ChangeChannel6))
		#ZMIANA KANALU

		if miniMap.IsAtlas():
			self.AtlasShowButton.SetEvent(ui.__mem_func__(self.ToggleAtlasWindow)) # @fixme014 ShowAtlas

		(ButtonPosX, ButtonPosY) = self.MiniMapShowButton.GetGlobalPosition()
		self.tooltipMiniMapOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# (ButtonPosX, ButtonPosY) = self.MiniMapHideButton.GetGlobalPosition()
		# self.tooltipMiniMapClose.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# (ButtonPosX, ButtonPosY) = self.ScaleUpButton.GetGlobalPosition()
		# self.tooltipScaleUp.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# (ButtonPosX, ButtonPosY) = self.ScaleDownButton.GetGlobalPosition()
		# self.tooltipScaleDown.SetTooltipPosition(ButtonPosX, ButtonPosY)

		(ButtonPosX, ButtonPosY) = self.AtlasShowButton.GetGlobalPosition()
		self.tooltipAtlasOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	(ButtonPosX, ButtonPosY) = self.DungeonInfoShowButton.GetGlobalPosition()
			# self.tooltipDungeonInfoOpen.SetTooltipPosition(ButtonPosX, ButtonPosY)
		self.ShowMiniMap()

	def Destroy(self):
		self.HideMiniMap()

		self.AtlasWindow.Destroy()
		self.AtlasWindow = None

		self.ClearDictionary()

		self.__Initialize()

	def UpdateObserverCount(self, observerCount):
		if observerCount>0:
			self.observerCount.Show()
		elif observerCount<=0:
			self.observerCount.Hide()

		self.observerCount.SetText(localeInfo.MINIMAP_OBSERVER_COUNT % observerCount)

	def OnUpdate(self):
		(x, y, z) = player.GetMainCharacterPosition()
		miniMap.Update(x, y)

		self.GetChild("dayInfo").SetText(time.strftime("%d.%m.%y")+" - "+time.strftime("%H:%M:%S"))
		self.positionInfo.SetText("(%.0f, %.0f)" % (x/100, y/100))

		if self.tooltipInfo:
			if True == self.MiniMapWindow.IsIn():
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				(bFind, sName, iPosX, iPosY, dwTextColor) = miniMap.GetInfo(mouseX, mouseY)
				if bFind == 0:
					self.tooltipInfo.Hide()
				elif not self.canSeeInfo:
					self.tooltipInfo.SetText("%s(%s)" % (sName, localeInfo.UI_POS_UNKNOWN))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
				else:
					if localeInfo.IsARABIC() and sName[-1].isalnum():
						self.tooltipInfo.SetText("(%s)%d, %d" % (sName, iPosX, iPosY))
					else:
						self.tooltipInfo.SetText("%s(%d, %d)" % (sName, iPosX, iPosY))
					self.tooltipInfo.SetTooltipPosition(mouseX - 5, mouseY)
					self.tooltipInfo.SetTextColor(dwTextColor)
					self.tooltipInfo.Show()
			else:
				self.tooltipInfo.Hide()

			# AUTOBAN
			if self.imprisonmentDuration:
				self.__UpdateImprisonmentDurationText()
			# END_OF_AUTOBAN

		if True == self.MiniMapShowButton.IsIn():
			self.tooltipMiniMapOpen.Show()
		else:
			self.tooltipMiniMapOpen.Hide()

		# if True == self.MiniMapHideButton.IsIn():
		# 	self.tooltipMiniMapClose.Show()
		# else:
		# 	self.tooltipMiniMapClose.Hide()

		# if True == self.ScaleUpButton.IsIn():
		# 	self.tooltipScaleUp.Show()
		# else:
		# 	self.tooltipScaleUp.Hide()

		# if True == self.ScaleDownButton.IsIn():
		# 	self.tooltipScaleDown.Show()
		# else:
		# 	self.tooltipScaleDown.Hide()

		if True == self.AtlasShowButton.IsIn():
			self.tooltipAtlasOpen.Show()
		else:
			self.tooltipAtlasOpen.Hide()

		# if app.ENABLE_DUNGEON_INFO_SYSTEM:
		# 	if True == self.DungeonInfoShowButton.IsIn():
		# 		self.tooltipDungeonInfoOpen.Show()
		# 	else:
		# 		self.tooltipDungeonInfoOpen.Hide()

	def OnRender(self):
		(x, y) = self.GetGlobalPosition()
		fx = float(x)
		fy = float(y)
		miniMap.Render(fx-2.0, fy + 24.0)

	def Close(self):
		self.HideMiniMap()

	def HideMiniMap(self):
		miniMap.Hide()
		self.OpenWindow.Hide()
		self.CloseWindow.Show()

	#ZMIANA KANALU
	def ChangeChannel1(self):
		if self.buttonChangeChannel[1]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[1].Down()
			net.MoveChannelGame(1)

	def ChangeChannel2(self):
		if self.buttonChangeChannel[2]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[2].Down()
			net.MoveChannelGame(2)

	def ChangeChannel3(self):
		if self.buttonChangeChannel[3]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[3].Down()
			net.MoveChannelGame(3)

	def ChangeChannel4(self):
		if self.buttonChangeChannel[4]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[4].Down()
			net.MoveChannelGame(4)

	def ChangeChannel5(self):
		if self.buttonChangeChannel[5]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[5].Down()
			net.MoveChannelGame(5)

	def ChangeChannel6(self):
		if self.buttonChangeChannel[6]:
			for i in xrange(1,7):
				self.buttonChangeChannel[i].SetUp()
				self.buttonChangeChannel[6].Down()
			net.MoveChannelGame(6)
	#ZMIANA KANALU

	# def OpenEventGui(self):
	# 	self.eventgui.Show()
	# 	self.eventgui.SetCenterPosition()

	def ShowMiniMap(self):
		if not self.canSeeInfo:
			return

		miniMap.Show()
		self.OpenWindow.Show()
		self.CloseWindow.Hide()

	def isShowMiniMap(self):
		return miniMap.isShow()

	def ScaleUp(self):
		miniMap.ScaleUp()

	def ScaleDown(self):
		miniMap.ScaleDown()
		
	#ZMIANA KANALU
	if app.BL_MOVE_CHANNEL:
		def RefreshServerInfo(self, channelNumber):
			if net.GetChannelNumber() == 99:
				for i in xrange(1,7):
					self.buttonChangeChannel[i].Down()
			elif net.GetChannelNumber() == 1:
				self.buttonChangeChannel[1].Down()
			elif net.GetChannelNumber() == 2:
				self.buttonChangeChannel[2].Down()
			elif net.GetChannelNumber() == 3:
				self.buttonChangeChannel[3].Down()
			elif net.GetChannelNumber() == 4:
				self.buttonChangeChannel[4].Down()
			elif net.GetChannelNumber() == 5:
				self.buttonChangeChannel[5].Down()
			elif net.GetChannelNumber() == 6:
				self.buttonChangeChannel[6].Down()
				
			if net.GetChannelNumber() == 99:
				serverInfoStr = '|cFFa9d26aArezzo|r - Kana� Globalny'
			else:
				serverInfoStr = '|cFFa9d26aArezzo|r, CH' + str(channelNumber)

			self.serverInfo.SetText(serverInfoStr)
			net.SetServerInfo(serverInfoStr)
			# else:
			# 	serverInfoStr = 'Arezzo.pl, Kana� ' + str(channelNumber)
			# 	self.serverInfo.SetText(serverInfoStr)
			# 	net.SetServerInfo(serverInfoStr)
	#ZMIANA KANALU

	def ShowAtlas(self):
		if not miniMap.IsAtlas():
			return
		if not self.AtlasWindow.IsShow():
			self.AtlasWindow.Show()

	def ToggleAtlasWindow(self):
		if not miniMap.IsAtlas():
			return
		if self.AtlasWindow.IsShow():
			self.AtlasWindow.Hide()
		else:
			self.AtlasWindow.Show()

	if app.ENABLE_LOADING_PERFORMANCE:
		def IsShowingAtlas(self):
			return self.AtlasWindow.IsShow()
			