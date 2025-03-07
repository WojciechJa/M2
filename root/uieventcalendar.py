import ui, app, net, constInfo, player
import datetime
import chat
import dbg
import wndMgr
import localeInfo
import exception
import emoji
from uiToolTip import ItemToolTip
IMG_DIR = "d:/ymir work/ui/game/event_calendar/"
IMG_ICON_DIR = "d:/ymir work/ui/game/event_calendar/icons/"
MINI_IMG_ICON_DIR = "d:/ymir work/ui/game/event_calendar/icons/mini_gui/"

events_default_data = {
	# img , eventName
	player.BONUS_EVENT:["bonus_event",localeInfo.BONUS_EVENT],
	player.DOUBLE_BOSS_LOOT_EVENT:["double_boss_loot_event",localeInfo.DOUBLE_BOSS_LOOT_EVENT],
	player.DOUBLE_METIN_LOOT_EVENT:["double_metin_loot_event", localeInfo.DOUBLE_METIN_LOOT_EVENT],
	player.DOUBLE_MISSION_BOOK_EVENT:["double_mission_book_event",localeInfo.DOUBLE_MISSION_BOOK_EVENT],
	player.DUNGEON_COOLDOWN_EVENT:["dungeon_cooldown_event",localeInfo.DUNGEON_COOLDOWN_EVENT],
	player.DUNGEON_CHEST_LOOT_EVENT:["dungeon_ticket_loot_event",localeInfo.DUNGEON_CHEST_LOOT_EVENT],
	player.EMPIRE_WAR_EVENT:["empire_war_event",""],
	player.MOONLIGHT_EVENT:["moonlight_event",localeInfo.MOONLIGHT_EVENT],
	player.TOURNAMENT_EVENT:["tournament_event",""],
	player.WHELL_OF_FORTUNE_EVENT:["whell_of_fortune_event",localeInfo.WHELL_OF_FORTUNE_EVENT],
	player.HALLOWEEN_EVENT:["halloween_event",localeInfo.HALLOWEEN_EVENT],
	player.NPC_SEARCH_EVENT:["npc_search",""],

	player.ITEM_DROP_EVENT:["item_drop_event",localeInfo.ITEM_DROP_EVENT],
	player.YANG_DROP_EVENT:["money_drop_event",localeInfo.YANG_DROP_EVENT],
	player.EXP_EVENT:["exp_event",localeInfo.EXP_EVENT],
}

#Dont touch!
def CalculateDayCount(month, year):
	if month == 2:
		if ((year%400==0) or (year%4==0 and year%100!=0)):
			return 29
		else:
			return 28
	elif month == 1 or month == 3 or month == 5 or month == 7 or month == 8 or month == 10 or month==12:
		return 31
	else:
		return 30
EVENT_DAY_INDEX = 0
EVENT_ID = 1
EVENT_INDEX = 2
EVENT_START_TEXT = 3
EVENT_END_TEXT = 4
EVENT_EMPIRE_FLAG = 5
EVENT_CHANNEL_FLAG = 6
EVENT_VALUE0 = 7
EVENT_VALUE1 = 8
EVENT_VALUE2 = 9
EVENT_VALUE3 = 10
EVENT_START_TIME = 11
EVENT_END_TIME = 12
EVENT_IS_ACTIVE = 13
server_event_data = {}
def SetEventStatus(eventID, eventStatus, endTime, endTimeText):
	for dayIndex, eventList in server_event_data.items():
		if eventList.has_key(eventID):
			eventList[eventID][EVENT_IS_ACTIVE] = eventStatus
			eventList[eventID][EVENT_END_TIME] = endTime
			eventList[eventID][EVENT_END_TEXT] = endTimeText
def SetServerData(dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart):
	if server_event_data.has_key(dayIndex):
		server_event_data[dayIndex][eventID] = [dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart]
	else:
		eventList = {}
		eventList[eventID] = [dayIndex, eventID, eventIndex, startTime, endTime, empireFlag, channelFlag, value0, value1, value2, value3, startRealTime, endRealTime, isAlreadyStart]
		server_event_data[dayIndex] = eventList
def IsEventIDActive(eventID):
	for dayIndex, eventList in server_event_data.items():
		if eventList.has_key(eventID):
			return eventList[EVENT_IS_ACTIVE]
	return False
def IsEventIndexActive(eventIndex):
	for dayIndex, eventList in server_event_data.items():
		for eventID, eventData in eventList.items():
			if eventList[EVENT_IS_ACTIVE]:
				return eventList[EVENT_INDEX] == eventIndex
	return False
def GetEventIndexData(eventIndex):
	for dayIndex, eventList in server_event_data.items():
		for eventID, eventData in eventList.items():
			if eventList[EVENT_INDEX] == eventIndex:
				return eventData
	return None
def GetEventIDData(eventID):
	for dayIndex, eventList in server_event_data.items():
		if eventList.has_key(eventID):
			return eventList[eventID]
	return None
#Dont touch!

class ImageBoxSpecial(ui.ImageBox):
	def __del__(self):
		ui.ImageBox.__del__(self)
	def Destroy(self):
		self.miniIcon = None
		self.eventList=[]
		self.imageIndex = 0
		self.isMiniIcon = False

		self.waitingTime = 0.0
		self.sleepTime = 0.0
		self.alphaValue = 0.0
		self.increaseValue = 0.0
		self.minAlpha = 0.0
		self.maxAlpha = 0.0
		self.alphaStatus = False

	def __init__(self, isMiniIcon = False):
		ui.ImageBox.__init__(self)
		self.Destroy()
		self.isMiniIcon = isMiniIcon

		self.waitingTime = 2.0
		self.alphaValue = 0.3
		self.increaseValue = 0.05
		self.minAlpha = 0.3
		self.maxAlpha = 1.0
		self.AddFlag("enable_pick")

		if isMiniIcon:
			self.AddFlag("attach")
			self.AddFlag("movable")
			#self.SetSize(57, 57)
			(x,y) = (30, 200)
			self.SetPosition(x, y)
			self.SetEvent(ui.__mem_func__(self.OnClickEventIcon),"mouse_click")
			self.SetMouseLeftButtonDoubleClickEvent(ui.__mem_func__(self.OnClickDouble))
			self.SetMouseRightButtonDownEvent(ui.__mem_func__(self.NextEventWithKey))
		else:
			self.SetEvent(ui.__mem_func__(self.OnClickDay), "mouse_click")

	def OnClickDay(self):
		if self.parentWindow:
			self.parentWindow.OnClick(self.dayIndex)

	def SetParentWindow(self, window):
		self.parentWindow = window

	def OnMoveWindow(self, x, y):
		(screenWidth, screenHeight) = (wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())

		if x < 0:
			x = 0
		elif x+self.GetWidth() >= screenWidth-70:
			x = screenWidth-80 - self.GetWidth()

		if y < 0:
			y = 0
		elif y+self.GetHeight() >= screenHeight-100:
			y = screenHeight-100 - self.GetHeight()

		self.SetPosition(x, y)

	def NextEventWithKey(self):
		if len(self.eventList) > 1:
			self.sleepTime = 0
			self.alphaValue = self.maxAlpha
			self.alphaStatus = True

	def SetBackgroundImage(self, image):
		self.LoadImage(image)
		self.SAFE_SetStringEvent("MOUSE_OVER_IN",self.OverInItem)
		self.SAFE_SetStringEvent("MOUSE_OVER_OUT",self.OverOutItem)
		self.SetEvent(ui.__mem_func__(self.OnClickEventIcon),"mouse_click")

	def OnClickEventIcon(self):
		(_index, eventID) = self.GetNextImage(self.imageIndex)
		if GetEventIDData(eventID) != None:
			if GetEventIDData(eventID)[EVENT_IS_ACTIVE] == True:
				interface = constInfo.GetInterfaceInstance()
				if interface:
					if interface.wndEventManager:
						interface.wndEventManager.OnClick(GetEventIDData(eventID)[EVENT_INDEX])

	def SetImage(self, folder):
		if self.miniIcon == None:
			miniIcon = ui.ImageBox()
			miniIcon.SetParent(self)
			miniIcon.AddFlag("not_pick")
			self.miniIcon = miniIcon

		if self.isMiniIcon == True:
			self.miniIcon.LoadImage(MINI_IMG_ICON_DIR+folder+".tga")
		else:
			self.miniIcon.LoadImage(IMG_ICON_DIR+folder+".tga")
		self.miniIcon.SetPosition(6, 6)
		self.miniIcon.Show()

		if self.isMiniIcon:
			self.SetSize(57,57)

		self.alphaValue = self.minAlpha
		self.alphaStatus = False

	def OnClickDouble(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			interface.OpenEventCalendar()
	def OverOutItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.tooltipItem:
				interface.tooltipItem.HideToolTip()
	def OverInItem(self):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			if interface.wndEventManager:
				interface.wndEventManager.OverInItem(self.dayIndex)
	def Clear(self):
		self.miniIcon = None
		self.eventList = []
		if self.isMiniIcon:
			self.SetSize(0,0)
			
	def DeleteImage(self, eventIndex):
		del self.eventList[eventIndex]
		if self.isMiniIcon and len(self.eventList) == 0:
			self.SetSize(0,0)

	def AppendImage(self, eventID):
		if eventID in self.eventList:
			return
		self.eventList.append(eventID)
		if len(self.eventList) == 1:
			self.imageIndex = 0
			if GetEventIDData(eventID) != None:
				self.SetImage(events_default_data[GetEventIDData(eventID)[EVENT_INDEX]][0])
		
		if self.isMiniIcon:
			self.SetSize(57,57)

	def GetNextImage(self, listIndex):
		if listIndex >= len(self.eventList):
			if len(self.eventList) > 0:
				return (0,self.eventList[0])
			return (0,0)
		return (listIndex, self.eventList[listIndex])
	
	def OnUpdate(self):
		if len(self.eventList) <= 1:
			self.imageIndex=0
			return
		elif self.sleepTime > app.GetTime():
			return
		if self.alphaStatus == True:
			self.alphaValue -= self.increaseValue
			if self.alphaValue < self.minAlpha:
				self.alphaValue = self.minAlpha
				self.alphaStatus = False
				(imageIndex, eventID) = self.GetNextImage(self.imageIndex+1)
				if GetEventIDData(eventID) != None:
					self.SetImage(events_default_data[GetEventIDData(eventID)[EVENT_INDEX]][0])
				self.imageIndex = imageIndex
		else:
			self.alphaValue += self.increaseValue
			if self.alphaValue > self.maxAlpha:
				self.alphaStatus = True
				self.sleepTime = app.GetTime()+self.waitingTime
		if self.miniIcon != None:
			self.miniIcon.SetAlpha(self.alphaValue)

class EventCalendarWindow(ui.ScriptWindow):
	def __del__(self):
		ui.ScriptWindow.__del__(self)
	def Destroy(self):
		self.children = {}
		self.currentMonth =0
		self.currentYear = 0
		self.eventManageWindow = None  # TEST EVENTMANAGE
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.Destroy()
		self.eventManageWindow = None  # TEST EVENTMANAGE
		self.__LoadWindow()

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/eventcalendar.py")
		except:
			exception.Abort("EventCalendar.LoadDialog.LoadScript")

		try:
			self.board = self.GetChild("board")
			self.exitbtn = self.GetChild("ExitButton")
			self.final_date_text = self.GetChild("final_date_text")

			self.exitbtn.SetEvent(ui.__mem_func__(self.Close))
		except:
			exception.Abort("EventCalendar.BindObjects")

		dt = datetime.datetime.today()
		(self.currentMonth,self.currentYear) = (dt.month,dt.year)
		dayCount = CalculateDayCount(self.currentMonth, self.currentYear)

		for day in xrange(dayCount):
			yCalculate = day/8
			xCalculate = day-(yCalculate*8)

			dayImages = ImageBoxSpecial(False)
			dayImages.SetParent(self.board)
			if dt.day == day+1:
				dayImages.SetBackgroundImage(IMG_DIR+"today_bg.png")
			else:
				dayImages.SetBackgroundImage(IMG_DIR+"black_bg.png")
			dayImages.SetPosition(8 + (xCalculate*66),8+(yCalculate*62))
			dayImages.dayIndex = day+1
			#TEST EVENTMANAGE
			dayImages.SetEvent(ui.__mem_func__(self.OnDayClick), "mouse_click")
			#TEST EVENTMANAGE
			dayImages.Show()
			self.children["dayImages%d"%day] = dayImages

			dayIndex = ui.NumberLine()
			dayIndex.SetParent(dayImages)
			dayIndex.SetNumber(str(day+1))
			dayIndex.SetPosition(8,8)
			dayIndex.Show()
			self.children["dayIndex%d"%day] = dayIndex
		# self.SetCenterPosition()
		
		self.Refresh()

	#TEST EVENTMANAGE
	def OnDayClick(self):
		if not hasattr(self, "eventManageWindow"):
			self.eventManageWindow = EventManageWindow()

		self.eventManageWindow.Open(self.dayIndex)
		if server_event_data.has_key(self.dayIndex):
			self.eventManageWindow.LoadEvents(server_event_data[self.dayIndex])


	def Open(self):
		self.Show()
		self.Refresh()
		self.SetTop()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnUpdate(self):
		for dayIndex in xrange(31):
			if self.children.has_key("dayImages%d"%dayIndex):
				self.children["dayImages%d"%dayIndex].OnUpdate()

	def Refresh(self):
		dt = datetime.datetime.today()
		
		dayCount = CalculateDayCount(self.currentMonth, self.currentYear)
		self.final_date_text.SetText("|cFF9cca67%02d-%02d  |r-  |cFF9cca67%02d-%02d  |r(%04d)" % 
                            (int(self.currentMonth), int(1), int(self.currentMonth), int(dayCount), int(self.currentYear)))

		for dayIndex in xrange(31):
			if self.children.has_key("dayImages%d"%dayIndex):
				dayEventImage = self.children["dayImages%d"%dayIndex]
				dayEventImage.Clear()
				if server_event_data.has_key(dayIndex+1):
					eventDict = server_event_data[dayIndex+1]
					if dt.day == dayIndex+1:
						dayEventImage.SetBackgroundImage(IMG_DIR+"today_bg.png")
					else:
						dayEventImage.SetBackgroundImage(IMG_DIR+"blue_bg.tga")
					for eventID, _data in eventDict.items():
						dayEventImage.AppendImage(eventID)
				else:
					if dt.day == dayIndex+1:
						dayEventImage.SetBackgroundImage(IMG_DIR+"today_bg.png")
					else:
						dayEventImage.SetBackgroundImage(IMG_DIR+"black_bg.png")
				dayEventImage.Show()
	#TEST EVENTMANAGE
	def OnClick(self, dayIndex):
		import chr
		if not chr.IsGameMaster(player.GetMainCharacterIndex()):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
			return

		if not hasattr(self, "eventManageWindow") or not self.eventManageWindow:
			try:
				import uieventmanager
				self.eventManageWindow = uieventmanager.EventManageWindow()
			except:
				import exception
				exception.Abort("EventCalendarWindow.CreateEventManageWindow")

		if self.eventManageWindow:
			self.eventManageWindow.Open(dayIndex)
			if server_event_data.has_key(dayIndex):
				self.eventManageWindow.LoadEvents(server_event_data[dayIndex])
	#TEST EVENTMANAGE
	def __GetBonusName(self, affect, value):
		return ItemToolTip.AFFECT_DICT[affect](value)

	def __CalculateTime(self, eventIndex, startTimeText, endTimeText, endTime):
		if endTimeText == "1970-01-01 03:00:00" or endTimeText == "1970-01-01 02:00:00" or endTimeText == "1970-01-01 01:00:00" or endTimeText == "1970-01-01 00:00:00" or endTimeText == "":
			startTimeSecond = startTimeText.split(" ")[1]
			return localeInfo.PVP_EVENT_TIME % (startTimeSecond)

		startTimeFirst = startTimeText.split(" ")[0]
		startTimeSecond = startTimeText.split(" ")[1]
		endTimeFirst = endTimeText.split(" ")[0]
		endTimeSecond = endTimeText.split(" ")[1]
		beginTimeText = ""
		endTimeText = ""
		if startTimeFirst != endTimeFirst:
			beginTimeText += startTimeFirst.split("-")[2]+"/"+startTimeFirst.split("-")[1]
			beginTimeText+=" "
			endTimeText += endTimeFirst.split("-")[2]+"/"+endTimeFirst.split("-")[1]
			endTimeText+=" "
		beginTimeText+=startTimeSecond
		endTimeText+=endTimeSecond
		return localeInfo.NORMAL_EVENT_TIME % (beginTimeText, endTimeText)

	def __GetMapName(self, mapIndex):
		mapNames = {
			61:localeInfo.MOUNT_SOHAN_MAP_NAME,
			62:localeInfo.MOUNT_DOYUMHWAN_MAP_NAME,
			63:localeInfo.MOUNT_YONGBI_MAP_NAME,
		}
		if mapNames.has_key(mapIndex):
			return mapNames[mapIndex]
		return "Unknown Map Name"

	def OverInItem(self, dayIndex):
		interface = constInfo.GetInterfaceInstance()
		if interface:
			tooltipItem = interface.tooltipItem
			if tooltipItem:
				tooltipItem.ClearToolTip()
				tooltipItem.ShowToolTip()
				tooltipItem.SetTitle(localeInfo.EVENT_TOOLTIP_TITLE % "%04d-%02d-%02d" % (int(self.currentYear), int(self.currentMonth), int(dayIndex)))
				tooltipItem.AppendSpace(5)

				if server_event_data.has_key(dayIndex):
					eventList = server_event_data[dayIndex]

					for eventID, eventData in eventList.items():
						eventName=""
						topInfo=""

						empireTexts = [localeInfo.ALL_KINGDOMS,localeInfo.RED_KINGDOM, localeInfo.YELLOW_KINGDOM, localeInfo.BLUE_KINGDOM]
						if eventData[EVENT_EMPIRE_FLAG] >= len(empireTexts):
							eventData[EVENT_EMPIRE_FLAG] = 0
						topInfo += empireTexts[eventData[EVENT_EMPIRE_FLAG]]+", "

						if eventData[EVENT_CHANNEL_FLAG] == 0:
							topInfo += "|cFF97AE99"+localeInfo.ALL_CHANNEL+"|r "
						else:
							topInfo += ("|cFF97AE99"+localeInfo.ONLY_CHANNEL+"|r") % eventData[EVENT_CHANNEL_FLAG]

						if eventData[EVENT_INDEX] == player.BONUS_EVENT:
							if eventData[EVENT_VALUE0] > 0 and eventData[EVENT_VALUE1] > 0:
								eventName+=events_default_data[eventData[EVENT_INDEX]][1]+" "+self.__GetBonusName(eventData[EVENT_VALUE0],eventData[EVENT_VALUE1])
							else:
								eventName+=events_default_data[eventData[EVENT_INDEX]][1]+" "+localeInfo.NONE_AFFECT

						# elif eventData[EVENT_INDEX] == player.EMPIRE_WAR_EVENT:
						# 	eventName += localeInfo.EMPIRE_WAR_EVENT % (eventData[EVENT_VALUE0],eventData[EVENT_VALUE1])

						# elif eventData[EVENT_INDEX] == player.TOURNAMENT_EVENT:
						# 	warType = [localeInfo.TOURNAMENT_ALL_CHARACTER, localeInfo.CHARACTER_WARRIOR, localeInfo.CHARACTER_ASSASSIN,localeInfo.CHARACTER_SURA, localeInfo.CHARACTER_SHAMAN]
						# 	if eventData[EVENT_VALUE0] >= len(warType):
						# 		eventData[EVENT_VALUE0] = 0
						# 	eventName += localeInfo.TOURNAMENT_EVENT % (warType[eventData[EVENT_VALUE0]], eventData[EVENT_VALUE1], eventData[EVENT_VALUE2])
						
						elif eventData[EVENT_INDEX] == player.ITEM_DROP_EVENT or eventData[EVENT_INDEX] == player.YANG_DROP_EVENT or eventData[EVENT_INDEX] == player.EXP_EVENT:
							eventName+= (events_default_data[eventData[EVENT_INDEX]][1] % eventData[EVENT_VALUE0])

						elif eventData[EVENT_INDEX] == player.NPC_SEARCH_EVENT:
							tooltipItem.AppendTextLine(localeInfo.NPC_SEARCH)
							tooltipItem.AppendTextLine(localeInfo.NPC_SEARCH_TEXT)
							for j in xrange(4):
								if eventData[EVENT_VALUE0+j] > 0:
									tooltipItem.AppendTextLine(self.__GetMapName(eventData[EVENT_VALUE0+j]))
							tooltipItem.AppendSpace(5)
							tooltipItem.AppendTextLine(self.__CalculateTime(eventData[EVENT_INDEX],eventData[EVENT_START_TEXT],eventData[EVENT_END_TEXT],eventData[EVENT_END_TIME]))
							tooltipItem.AppendSpace(5)
							continue
						else:
							eventName += events_default_data[eventData[EVENT_INDEX]][1]

						tooltipItem.AppendTextLine(eventName)
						tooltipItem.AppendSpace(5)
						tooltipItem.AppendTextLine(topInfo)
						tooltipItem.AppendSpace(5)
						tooltipItem.AppendTextLine(self.__CalculateTime(eventData[EVENT_INDEX],eventData[EVENT_START_TEXT],eventData[EVENT_END_TEXT],eventData[EVENT_END_TIME]))
						tooltipItem.AppendSpace(5)

						tooltipItem.ResizeToolTipWidth(272)
						tooltipItem.AlignTextLineHorizonalCenter()
				else:
					tooltipItem.AppendTextLine(localeInfo.EVENT_TOOLTIP_DOESNT_HAVE_EVENT)

class MovableImage(ImageBoxSpecial):
	def __del__(self):
		ImageBoxSpecial.__del__(self)

	def Destroy(self):
		self.window = None
		self.eventCache = []
		self.timeList = []
		self.timeText = None
		ImageBoxSpecial.Destroy(self)

	def __init__(self):
		self.Destroy()
		ImageBoxSpecial.__init__(self, True)

		window = ui.Window()
		window.SetParent(self)
		window.AddFlag("not_pick")
		window.OnUpdate = ui.__mem_func__(self.OnUpdate)
		window.Show()
		self.window = window

		timeText = ui.TextLine()
		timeText.SetParent(self)
		timeText.AddFlag("not_pick")
		timeText.SetHorizontalAlignCenter()
		timeText.SetPosition(25, 55)
		timeText.SetOutline()
		timeText.Show()
		self.timeText = timeText

		timeTextEx = ui.TextLine()
		timeTextEx.SetParent(self)
		timeTextEx.AddFlag("not_pick")
		timeTextEx.SetHorizontalAlignCenter()
		timeTextEx.SetPosition(25, 73)
		timeTextEx.SetText(localeInfo.BONUS_NEXT_EVENT)
		timeTextEx.SetOutline()
		timeTextEx.Show()
		self.timeTextEx = timeTextEx

	def Clear(self):
		self.eventCache = []
		self.timeList = []
		self.timeText.SetText("")
		self.timeTextEx.Hide()
		ImageBoxSpecial.Clear(self)
		self.Hide()

	def Refresh(self):
		self.Clear()
		for dayIndex, eventList in server_event_data.items():
			for eventID, eventData in eventList.items():
				self.AppendEvent(eventData[EVENT_ID], eventData[EVENT_START_TIME], eventData[EVENT_END_TIME], eventData[EVENT_IS_ACTIVE])

	def LoadTime(self, eventID, startTime, endTime, isAlreadyStart):
		self.AppendImage(eventID)
		self.timeList.append([startTime, endTime, isAlreadyStart])

		if len(self.timeList) > 1:
			self.timeTextEx.Show()
		else:
			self.timeTextEx.Hide()
		self.Show()

	def CheckCacheEvent(self):
		if len(self.eventCache) == 0:
			return
		clientGlobalTime = app.GetGlobalTimeStamp()
		for j in xrange(len(self.eventCache)):
			startTime = self.eventCache[j][1] - clientGlobalTime
			endTime = self.eventCache[j][2] - clientGlobalTime
			if startTime >= 0 and startTime <= (60*30):
				self.Refresh()
				return

	def AppendEvent(self, eventID, startTime, endTime, isAlreadyStart):
		clientGlobalTime = app.GetGlobalTimeStamp()

		newStartTime = startTime
		newEndTime = endTime
		if newStartTime != 0:
			newStartTime -= clientGlobalTime
		if newEndTime != 0:
			newEndTime -= clientGlobalTime

		if (startTime != 0 and newStartTime <= 0 and endTime == startTime and isAlreadyStart == True) or (startTime != 0 and newStartTime <= 0 and endTime == 0 and isAlreadyStart == True):
			self.LoadTime(eventID, startTime, endTime, 2)
		elif newStartTime <= 0 and newEndTime <= 0:
			return
		elif newStartTime > 0 and newStartTime <= (60*30):#start-in
			self.LoadTime(eventID, startTime, endTime, 0)
		elif newEndTime > 0 and isAlreadyStart == True:#end-in
			self.LoadTime(eventID, startTime, endTime, 1)
		else:
			self.eventCache.append([eventID, startTime, endTime, isAlreadyStart])

		if len(self.timeList) > 1:
			self.timeTextEx.Show()
		else:
			self.timeTextEx.Hide()

	def DeleteEvent(self, index):
		self.DeleteImage(index)
		del self.timeList[index]

		if len(self.timeList) <= 1:
			self.timeTextEx.Hide()

	def FormatTime(self, seconds):
		if seconds == 0:
			return ""
		m, s = divmod(seconds, 60)
		h, m = divmod(m, 60)
		return "%02dh %02dm %02ds" % (h, m, s)

	def OnUpdate(self):
		ImageBoxSpecial.OnUpdate(self)
		self.CheckCacheEvent()

		if self.imageIndex < len(self.timeList):
			timeData = self.timeList[self.imageIndex]

			if timeData[2] == 0:
				leftTime = timeData[0] - app.GetGlobalTimeStamp()
				if leftTime > 0:
					self.timeText.SetText("Start za: "+localeInfo.BONUS_START_IN%self.FormatTime(leftTime))

					if not self.timeText.IsShow():
						self.timeText.Show()

			elif timeData[2] == 1:
				leftTime = timeData[1] - app.GetGlobalTimeStamp()
				if leftTime > 0:
					self.timeText.SetText("{} ".format(emoji.AppendEmoji("icon/emoji/image-example-007.png"))+localeInfo.BONUS_END_IN%self.FormatTime(leftTime))
					self.timeText.SetFontName("Tahoma:14")
					if not self.timeText.IsShow():
						self.timeText.Show()

			elif timeData[2] == 2:
				if self.timeText.IsShow():
					self.timeText.Hide()
					
					


