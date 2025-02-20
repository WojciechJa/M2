import ui
import wndMgr
import chat
import localeInfo
import grp
import net
import uiToolTip

PATH = "d:/ymir work/ui/battle_pass/"

PAGE_REWARD = 0
PAGE_QUEST = 1

QUEST_INDEX = 0
QUEST_GROUP = 1
QUEST_COUNT = 2
QUEST_EXP = 3

REWARD_LEVEL = 0
REWARD_ITEM_FREE = 1
REWARD_COUNT_FREE = 2
REWARD_ITEM_PREMIUM = 3
REWARD_COUNT_PREMIUM = 4

QUEST_DATA_COUNT = 0
QUEST_DATA_STATUS = 1

REWARD_DATA_FREE = 0
REWARD_DATA_PREMIUM = 1

REWARD_DATA_COLLECT = 1
REWARD_DATA_COLLECTED = 2

LEVEL_LEVEL = 0
LEVEL_EXP = 1
LEVEL_EXP_MAX = 2

QUEST_MAX_ELEMENT = 8
REWARD_MAX_ELEMENT = 12
REWARD_MAX_ELEMENT_FIXED = REWARD_MAX_ELEMENT / 2

battlePassInfo = {
	"quest" : [],
	"quest_data" : {"info" : {}},

	"reward" : [],
	"reward_data" : {"info" : {}},

	"pages" : [0, 0],
	"level" : [0, 0, 0],
}

class BattlePassDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.windowReward = None
		self.windowQuest = None

		self.buttonMoveQuest = None
		self.buttonMoveReward = None
		self.buttonBuyPremium = None
		self.buttonPreviousPage_r = None
		self.buttonPreviousPage_q = None
		self.buttonNextPage_r = None
		self.buttonNextPage_q = None

		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()

		self.levelBackground = []
		self.levelBackground_r = None
		self.levelBackground_q = None
		self.levelBackgroundExpWindow = None
		self.levelBackgroundExpText = None
		self.levelBackgroundLevelWindow = None
		self.levelBackgroundLevelText = None
		self.levelBackgroundLevelGauge = None

		self.rewardSlotImage = {}
		self.rewardSlotBase = {}
		self.rewardSlotBase2 = {}
		self.rewardButtonImage = {}
		self.rewardButtonImage2 = {}
		self.rewardLevelImage = {}
		self.rewardLevelWindow = {}
		self.rewardLevelText = {}

		self.questBackground = {}
		self.questTitle = {}
		self.questDesc = {}
		self.questGauge = {}
		self.questGaugeWindow = {}
		self.questGaugeText = {}

		self.currentWindow = PAGE_REWARD
		self.currentPage = 1

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Close(self):
		wndMgr.Hide(self.hWnd)

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def OnPressExitKey(self):
		self.Hide()
		return True

	def Destory(self):
		self.ClearDictionary()

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/battlepassdialog.py")

		except:
			import exception
			exception.Abort("BattlePassDialog.LoadWindow")

		try:
			self.windowReward						= self.GetChild2("backgroundReward")
			self.windowQuest						= self.GetChild2("backgroundQuest")

			self.buttonMoveQuest					= self.GetChild2("buttonMoveQuest")
			self.buttonMoveReward					= self.GetChild2("buttonMoveReward")
			self.buttonBuyPremium					= self.GetChild2("buttonBuyPremium")

			self.buttonPreviousPage_r				= self.GetChild2("buttonPreviousPage_r")
			self.buttonPreviousPage_q				= self.GetChild2("buttonPreviousPage_q")
			self.buttonNextPage_r					= self.GetChild2("buttonNextPage_r")
			self.buttonNextPage_q					= self.GetChild2("buttonNextPage_q")

			self.levelBackground_r					= self.GetChild2("backgroundLevel_r")
			self.levelBackground_q					= self.GetChild2("backgroundLevel_q")
			self.exitbutton							= self.GetChild2("ExitButton")
		except:
			import exception
			exception.Abort("BattlePassDialog.LoadWindow.Bind")
		self.buttonMoveQuest.SetEvent(ui.__mem_func__(self.WindowManager), PAGE_QUEST)
		self.buttonMoveReward.SetEvent(ui.__mem_func__(self.WindowManager), PAGE_REWARD)
		self.buttonBuyPremium.SetEvent(ui.__mem_func__(self.OnClickBuyPremium))

		self.buttonPreviousPage_r.SetEvent(ui.__mem_func__(self.OnClickPageManager), "left")
		self.buttonPreviousPage_q.SetEvent(ui.__mem_func__(self.OnClickPageManager), "left")
		self.buttonNextPage_r.SetEvent(ui.__mem_func__(self.OnClickPageManager), "right")
		self.buttonNextPage_q.SetEvent(ui.__mem_func__(self.OnClickPageManager), "right")

		self.exitbutton.SetEvent(ui.__mem_func__(self.Close))

		self.levelBackground.append(self.levelBackground_r)
		self.levelBackground.append(self.levelBackground_q)

		self.BuildElements()
		self.WindowManager(PAGE_REWARD)

	def BuildElements(self):
		for element in range(8):
			posX = (element % 2) * 291
			posY = (element // 2) * 66

			self.questBackground[element] = ui.ImageBox()
			self.questBackground[element].SetParent(self.windowQuest)
			self.questBackground[element].SetPosition(103 + posX, 177 + posY)
			self.questBackground[element].LoadImage("{}".format(PATH + "quest_view.tga"))
			self.questBackground[element].Show()

			self.questTitle[element] = ui.TextLine()
			self.questTitle[element].SetParent(self.questBackground[element])
			self.questTitle[element].SetPosition(60, 6)
			self.questTitle[element].SetText("")
			self.questTitle[element].SetOutline(True)
			self.questTitle[element].SetPackedFontColor(grp.GenerateColor(0.6431, 0.9607, 0.4862, 1.0))
			self.questTitle[element].Show()

			self.questDesc[element] = ui.TextLine()
			self.questDesc[element].SetParent(self.questBackground[element])
			self.questDesc[element].SetPosition(60, 21)
			self.questDesc[element].SetFontName("Tahoma:12")
			self.questDesc[element].SetOutline(True)
			self.questDesc[element].SetText("")
			self.questDesc[element].Show()

			self.questGauge[element] = ui.ExpandedImageBox()
			self.questGauge[element].SetParent(self.questBackground[element])
			self.questGauge[element].SetPosition(57, 36)
			self.questGauge[element].LoadImage("{}".format(PATH + "quest_gauge.tga"))
			self.questGauge[element].SetPercentage(0, 100)
			self.questGauge[element].Show()

			self.questGaugeWindow[element] = ui.Window()
			self.questGaugeWindow[element].SetParent(self.questBackground[element])
			self.questGaugeWindow[element].SetPosition(57, 36)
			self.questGaugeWindow[element].SetSize(210, 18)
			self.questGaugeWindow[element].Show()

			self.questGaugeText[element] = ui.TextLine()
			self.questGaugeText[element].SetParent(self.questGaugeWindow[element])
			self.questGaugeText[element].SetWindowHorizontalAlignCenter()
			self.questGaugeText[element].SetHorizontalAlignCenter()
			self.questGaugeText[element].SetPosition(1, 1)
			self.questGaugeText[element].SetFontName("Tahoma:12")
			self.questGaugeText[element].SetOutline(True)
			self.questGaugeText[element].SetText("")
			self.questGaugeText[element].Show()

		for element in range(12):
			posX = (element % 6) * 78
			posY = (element // 6) * 143

			self.rewardSlotImage[element] = ui.ImageBox()
			self.rewardSlotImage[element].SetParent(self.windowReward)
			self.rewardSlotImage[element].SetPosition(226 + posX, 182 + posY)
			self.rewardSlotImage[element].LoadImage("{}".format(PATH + ("reward_premium_locked.png" if element > 5 else "reward_free_locked.png")))
			self.rewardSlotImage[element].Show()

			for x in range(6):
				fix_posX = (x % 6) * 78
				fix_posY = (x // 6) * 143

				self.rewardSlotBase[x] = ui.SlotWindow()
				self.rewardSlotBase[x].SetParent(self.windowReward)
				self.rewardSlotBase[x].SetPosition(243 + fix_posX, 208 + fix_posY)
				self.rewardSlotBase[x].SetSize(32, 32)
				self.rewardSlotBase[x].AppendSlot(x, 0, 0, 32, 32)
				# self.rewardSlotBase[element].SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
				self.rewardSlotBase[x].RefreshSlot()
				self.rewardSlotBase[x].Show()

				self.rewardSlotBase2[x] = ui.SlotWindow()
				self.rewardSlotBase2[x].SetParent(self.windowReward)
				self.rewardSlotBase2[x].SetPosition(243 + fix_posX, 352 + fix_posY)
				self.rewardSlotBase2[x].SetSize(32, 32)
				self.rewardSlotBase2[x].AppendSlot(x, 0, 0, 32, 32)
				# self.rewardSlotBase[element].SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
				self.rewardSlotBase2[x].RefreshSlot()
				self.rewardSlotBase2[x].Show()

			self.rewardButtonImage[element] = ui.ImageBox()
			self.rewardButtonImage[element].SetParent(self.windowReward)
			self.rewardButtonImage[element].SetPosition(226 + posX, 266 + posY)
			self.rewardButtonImage[element].LoadImage("{}".format(PATH + "buttons/" + ("reward_premium_locked_01.png" if element > 5 else "reward_free_locked_01.png")))
			self.rewardButtonImage[element].Show()

			self.rewardButtonImage2[element] = ui.ImageBox()
			self.rewardButtonImage2[element].SetParent(self.windowReward)
			self.rewardButtonImage2[element].SetPosition(270 + posX,185 + posY)
			self.rewardButtonImage2[element].LoadImage("{}".format(PATH + "lock1.png" if element > 5 else PATH+"lock.png"))
			self.rewardButtonImage2[element].Show()

			if element < 6:
				self.rewardLevelImage[element] = ui.ImageBox()
				self.rewardLevelImage[element].SetParent(self.windowReward)
				self.rewardLevelImage[element].SetPosition(250 + posX, 296 + posY)
				self.rewardLevelImage[element].LoadImage("{}".format(PATH + "level_tier.png"))
				self.rewardLevelImage[element].Show()

				self.rewardLevelWindow[element] = ui.Window()
				self.rewardLevelWindow[element].SetParent(self.rewardLevelImage[element])
				self.rewardLevelWindow[element].SetPosition(0, 0)
				self.rewardLevelWindow[element].SetSize(20, 20)
				self.rewardLevelWindow[element].Show()

				self.rewardLevelText[element] = ui.TextLine()
				self.rewardLevelText[element].SetParent(self.rewardLevelWindow[element])
				self.rewardLevelText[element].SetWindowHorizontalAlignCenter()
				self.rewardLevelText[element].SetHorizontalAlignCenter()
				self.rewardLevelText[element].SetPosition(0, 2)
				self.rewardLevelText[element].SetText("")
				self.rewardLevelText[element].SetOutline(True)
				self.rewardLevelText[element].Show()

		self.levelBackgroundExpWindow = ui.Window()
		self.levelBackgroundExpWindow.SetParent(self.levelBackground[self.currentWindow])
		self.levelBackgroundExpWindow.SetPosition(99, 29)
		self.levelBackgroundExpWindow.SetSize(107, 13)
		self.levelBackgroundExpWindow.Show()

		self.levelBackgroundLevelWindow = ui.Window()
		self.levelBackgroundLevelWindow.SetParent(self.levelBackground[self.currentWindow])
		self.levelBackgroundLevelWindow.SetPosition(5, 0)
		self.levelBackgroundLevelWindow.SetSize(33, 33)
		self.levelBackgroundLevelWindow.Show()

		self.levelBackgroundExpText = ui.TextLine()
		self.levelBackgroundExpText.SetParent(self.levelBackgroundExpWindow)
		self.levelBackgroundExpText.SetWindowHorizontalAlignCenter()
		self.levelBackgroundExpText.SetHorizontalAlignCenter()
		self.levelBackgroundExpText.SetPosition(0, 1)
		self.levelBackgroundExpText.SetFontName("Tahoma:12")
		self.levelBackgroundExpText.SetOutline(True)
		self.levelBackgroundExpText.SetText("")
		self.levelBackgroundExpText.Show()

		self.levelBackgroundLevelText = ui.TextLine()
		self.levelBackgroundLevelText.SetParent(self.levelBackgroundLevelWindow)
		self.levelBackgroundLevelText.SetWindowHorizontalAlignCenter()
		self.levelBackgroundLevelText.SetHorizontalAlignCenter()
		self.levelBackgroundLevelText.SetPosition(0, 7)
		self.levelBackgroundLevelText.SetFontName("Tahoma:16b")
		self.levelBackgroundLevelText.SetOutline(True)
		self.levelBackgroundLevelText.SetText("")
		self.levelBackgroundLevelText.Show()

		self.levelBackgroundLevelGauge = ui.ExpandedImageBox()
		self.levelBackgroundLevelGauge.SetParent(self.levelBackground[self.currentWindow])
		self.levelBackgroundLevelGauge.SetPosition(38, 4)
		self.levelBackgroundLevelGauge.LoadImage("{}".format(PATH + "level_gauge.tga"))
		self.levelBackgroundLevelGauge.SetPercentage(0, 100)
		self.levelBackgroundLevelGauge.Show()

	def OverOutItem(self):
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItemFree(self, slot):
		if None != self.tooltipItem:
			if self.currentPage == 1:
				reward = battlePassInfo["reward"][slot]
			elif self.currentPage > 1:
				reward = battlePassInfo["reward"][REWARD_MAX_ELEMENT_FIXED * (self.currentPage - 1) + slot]

			self.tooltipItem.SetItemToolTip(reward[REWARD_ITEM_FREE])

	def OverInItemPremium(self, slot):
		if None != self.tooltipItem:
			if self.currentPage == 1:
				reward = battlePassInfo["reward"][slot]
			elif self.currentPage > 1:
				reward = battlePassInfo["reward"][REWARD_MAX_ELEMENT_FIXED * (self.currentPage - 1) + slot]

			self.tooltipItem.SetItemToolTip(reward[REWARD_ITEM_PREMIUM])

	def WindowManager(self, window):
		self.currentWindow = window
		self.currentPage = 1

		self.levelBackgroundExpWindow.SetParent(self.levelBackground[window])
		self.levelBackgroundLevelWindow.SetParent(self.levelBackground[window])
		self.levelBackgroundLevelGauge.SetParent(self.levelBackground[self.currentWindow])

		self.RefreshLevel()

		windows = [self.windowReward, self.windowQuest]

		for element in range(len(windows)):
			windows[element].Hide()

		windows[window].Show()

	def WindowRebuild(self, window):
		totalPages = 0

		dataCount = len(battlePassInfo[window])
		maxElements = (dataCount - 1) // (QUEST_MAX_ELEMENT if window == "quest" else REWARD_MAX_ELEMENT / 2) + 1

		for _ in range(1, maxElements + 1):
			totalPages += 1

		battlePassInfo["pages"][PAGE_QUEST if window == "quest" else PAGE_REWARD] = totalPages

	def GetQuestString(self, index):
		nameList = {
			0 : localeInfo.BATTLE_PASS_QUEST_NAME_01,
			1 : localeInfo.BATTLE_PASS_QUEST_NAME_02,
			2 : localeInfo.BATTLE_PASS_QUEST_NAME_03,
			3 : localeInfo.BATTLE_PASS_QUEST_NAME_04,
			4 : localeInfo.BATTLE_PASS_QUEST_NAME_05,
			5 : localeInfo.BATTLE_PASS_QUEST_NAME_06,
		}

		descList = {
			0 : localeInfo.BATTLE_PASS_QUEST_DESC_01,
			1 : localeInfo.BATTLE_PASS_QUEST_DESC_02,
			2 : localeInfo.BATTLE_PASS_QUEST_DESC_03,
			3 : localeInfo.BATTLE_PASS_QUEST_DESC_04,
			4 : localeInfo.BATTLE_PASS_QUEST_DESC_05,
			5 : localeInfo.BATTLE_PASS_QUEST_DESC_06,
		}

		return nameList.get(index, "NoName"), descList.get(index, "NoName")

	def GetQuestData(self, index, arg):
		if index in battlePassInfo["quest_data"]:
			return battlePassInfo["quest_data"][index][0][arg]
		else:
			return 0

	def GetNumberFormat(self, number):
		if number <= 0 :
			return "0"

		str_number = str(number)
		result = []

		for i in range(len(str_number)):
			if (len(str_number) - i) % 3 == 0 and i != 0:
				result.append('.')

			result.append(str_number[i])

		return ''.join(result)

	def OnClickBuyPremium(self):
		net.SendChatPacket("/buy_battlepass")

	def OnClickPageManager(self, direction):
		currentWindow = self.currentWindow
		currentPage = self.currentPage

		maxElements = (QUEST_MAX_ELEMENT if currentWindow != PAGE_REWARD else REWARD_MAX_ELEMENT / 2)
		pageCount = ((len(battlePassInfo["quest"]) - 1) // maxElements + 1 if currentWindow != PAGE_REWARD else (len(battlePassInfo["reward"]) - 1) // maxElements + 1)

		if (currentPage == 1 and direction == "left") or (currentPage == pageCount and direction == "right"):
			return

		if direction == "left":
			self.currentPage = currentPage - 1

		elif direction == "right":
			self.currentPage = currentPage + 1

		if self.currentWindow == PAGE_QUEST:
			self.RefreshPageQuest()

		elif self.currentWindow == PAGE_REWARD:
			self.RefreshPageReward()

	def OnMouseRewardButton(self, index, level, arg):
		if index == -1:
			return

		imageLoad = ("collect_free" if index < 6 else "collect_premium")
		imageString = ("_01" if arg == True else "_01")

		self.rewardButtonImage[index].LoadImage(PATH + "buttons/{}{}.png".format(imageLoad, imageString))

	def OnClickRewardButton(self, level, reward):
		if level == -1:
			return
			
		net.PacketBattlePassCollect(level, reward)

	def ClearQuest(self):
		battlePassInfo["quest"] = []
		battlePassInfo["quest_data"] = {"info" : {}}

	def ClearPageQuest(self):
		for element in range(QUEST_MAX_ELEMENT):
			self.questTitle[element].SetText("")
			self.questDesc[element].SetText("")
			self.questGaugeText[element].SetText("")
			self.questGauge[element].SetPercentage(0, 100)

	def ClearCompletedQuest(self, index):
		if index in battlePassInfo["quest_data"]:
			del battlePassInfo["quest_data"][index]

		for quest in battlePassInfo["quest"]:
			if index == quest[0]:
				battlePassInfo["quest"].remove(quest)

	def ClearReward(self):
		battlePassInfo["reward"] = []
		battlePassInfo["reward_data"] = {"info" : {}}

	def ClearPageReward(self):
		for element in range(len(self.rewardLevelText)):
			self.rewardLevelText[element].SetText("")

		for element in range(len(self.rewardSlotBase)):
			self.rewardSlotBase[element].SetItemSlot(element, 0, 0)

		for element in range(len(self.rewardSlotBase2)):
			self.rewardSlotBase2[element].SetItemSlot(element, 0, 0)

		for element in range(REWARD_MAX_ELEMENT):
			self.rewardButtonImage[element].OnMouseOverIn = lambda this = self, index = -1, level = 0, hover = False: this.OnMouseRewardButton(index, level, hover)
			self.rewardButtonImage[element].OnMouseOverOut = lambda this = self, index = -1, level = 0, hover = False: this.OnMouseRewardButton(index, level, hover)
			self.rewardButtonImage[element].OnMouseLeftButtonDown = lambda this = self, level = -1, group = 0: this.OnClickRewardButton(level, group)
			self.rewardButtonImage[element].LoadImage("{}".format(PATH + "buttons/" + ("reward_premium_locked_01.png" if element > 5 else "reward_free_locked_01.png")))
			self.rewardButtonImage2[element].Show()

	def RefreshPageQuest(self):
		self.ClearPageQuest()

		currentPage = self.currentPage
#		questCount = sum(1 for quest in battlePassInfo["quest"] if quest[QUEST_DATA_STATUS] != 1)
		questCount = len(battlePassInfo["quest"])
		pageCount = (questCount - 1) // QUEST_MAX_ELEMENT + 1

		if currentPage <= pageCount:
			for element_on_page in range(0, QUEST_MAX_ELEMENT):
				element_number = QUEST_MAX_ELEMENT * (currentPage - 1) + element_on_page

				if 0 <= element_number < questCount:
					quest = battlePassInfo["quest"][element_number]
					quest_count = self.GetQuestData(quest[QUEST_INDEX], QUEST_DATA_COUNT)
					(title, desc) = self.GetQuestString(quest[QUEST_GROUP])

					self.questTitle[element_on_page].SetText("{}".format(title))
					self.questDesc[element_on_page].SetText("{}".format(desc))

					self.questGauge[element_on_page].SetPercentage(float(quest_count) / max(1, float(quest[QUEST_COUNT])) * 100, 100)
					self.questGaugeText[element_on_page].SetText("{}".format("{} / {}".format(self.GetNumberFormat(quest_count), self.GetNumberFormat(quest[QUEST_COUNT]))))
				else:
					break

	def RefreshPageReward(self):
		self.ClearPageReward()

		currentPage = self.currentPage
		rewardCount = len(battlePassInfo["reward"])
		pageCount = (rewardCount - 1) // REWARD_MAX_ELEMENT_FIXED + 1

		if currentPage <= pageCount:
			for element_on_page in range(0, REWARD_MAX_ELEMENT_FIXED):
				element_number = REWARD_MAX_ELEMENT_FIXED * (currentPage - 1) + element_on_page

				if 0 <= element_number < rewardCount:
					reward = battlePassInfo["reward"][element_number]

					self.rewardSlotBase[element_on_page].SetItemSlot(element_on_page, reward[REWARD_ITEM_FREE], reward[REWARD_COUNT_FREE])
					self.rewardSlotBase[element_on_page].SetOverInItemEvent(ui.__mem_func__(self.OverInItemFree))
					self.rewardSlotBase[element_on_page].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

					self.rewardSlotBase2[element_on_page].SetItemSlot(element_on_page, reward[REWARD_ITEM_PREMIUM], reward[REWARD_COUNT_PREMIUM])
					self.rewardSlotBase2[element_on_page].SetOverInItemEvent(ui.__mem_func__(self.OverInItemPremium))
					self.rewardSlotBase2[element_on_page].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))


					self.rewardLevelText[element_on_page].SetText("{}".format(reward[REWARD_LEVEL]))

					for reward_type in [REWARD_DATA_FREE, REWARD_DATA_PREMIUM]:
						element = (element_on_page if reward_type == REWARD_DATA_FREE else element_on_page + REWARD_MAX_ELEMENT_FIXED)

						if reward[REWARD_LEVEL] in battlePassInfo["reward_data"] and battlePassInfo["reward_data"][reward[REWARD_LEVEL]][0][reward_type] == REWARD_DATA_COLLECT:
							suffix = "collect_free" if reward_type == REWARD_DATA_FREE else "collect_premium"
							reward_type = (REWARD_DATA_FREE if element < 6 else REWARD_DATA_PREMIUM)

							self.rewardButtonImage[element].OnMouseOverIn = lambda this = self, index = element, level = reward[REWARD_LEVEL], hover = True: this.OnMouseRewardButton(index, level, hover)
							self.rewardButtonImage[element].OnMouseOverOut = lambda this = self, index = element, level = reward[REWARD_LEVEL], hover = False: this.OnMouseRewardButton(index, level, hover)
							self.rewardButtonImage[element].OnMouseLeftButtonDown = lambda this = self, level = reward[REWARD_LEVEL], group = reward_type: this.OnClickRewardButton(level, group)
							self.rewardButtonImage[element].LoadImage(PATH + "buttons/%s_01.png" % suffix)
							self.rewardButtonImage2[element].Hide()
						elif reward[REWARD_LEVEL] in battlePassInfo["reward_data"] and battlePassInfo["reward_data"][reward[REWARD_LEVEL]][0][reward_type] == REWARD_DATA_COLLECTED:
							self.rewardButtonImage[element].LoadImage(PATH + "buttons/%s_01.png" % ("reward_premium_collected" if element > 5 else "reward_free_collected"))
				else:
					break

	def RefreshLevel(self):
		data = battlePassInfo["level"]

		self.levelBackgroundLevelText.SetText("{}".format(data[LEVEL_LEVEL]))
		self.levelBackgroundExpText.SetText("{} {:.2f}%".format(localeInfo.BATTLE_PASS_EXP, float(data[LEVEL_EXP]) / max(1, float(data[LEVEL_EXP_MAX])) * 100))
		self.levelBackgroundLevelGauge.SetPercentage(float(data[LEVEL_EXP]) / max(1, float(data[LEVEL_EXP_MAX])) * 100, 100)

	def AppendQuest(self, *data):
		battlePassInfo["quest"].append([data[QUEST_INDEX], data[QUEST_GROUP], data[QUEST_COUNT], data[QUEST_EXP]])

	def AppendQuestData(self, index, count, status):
		battlePassInfo["quest_data"][index] = []
		battlePassInfo["quest_data"][index].append([count, status])
		if status:
			self.ClearCompletedQuest(index)
			
	def AppendReward(self, *data):
		battlePassInfo["reward"].append([data[REWARD_LEVEL], data[REWARD_ITEM_FREE], data[REWARD_COUNT_FREE], data[REWARD_ITEM_PREMIUM], data[REWARD_COUNT_PREMIUM]])

	def AppendRewardData(self, level, status_free, status_premium):
		battlePassInfo["reward_data"][level] = []
		battlePassInfo["reward_data"][level].append([status_free, status_premium])

	def AppendLevel(self, level, exp, exp_max):
		battlePassInfo["level"] = [level, exp, exp_max]