#-*- coding: iso-8859-1 -*-
import app
import ui
import player
import net
import wndMgr
import messenger
import guild
import chr
import uiChestDropInfo
import nonplayer
import chat
import localeInfo
import emoji
import mouseModule
import constInfo
import chrmgr
import grp

RACE_FLAG_DICT = {
    nonplayer.RACE_FLAG_ANIMAL: [localeInfo.TARGET_RACE_FLAG_ANIMAL, 44],
    nonplayer.RACE_FLAG_UNDEAD: [localeInfo.TARGET_RACE_FLAG_UNDEAD, 47],
    nonplayer.RACE_FLAG_DEVIL: [localeInfo.TARGET_RACE_FLAG_DEVIL, 48],
    nonplayer.RACE_FLAG_HUMAN: [localeInfo.TARGET_RACE_FLAG_HUMAN, 43],
    nonplayer.RACE_FLAG_ORC: [localeInfo.TARGET_RACE_FLAG_ORC, 45],
    nonplayer.RACE_FLAG_MILGYO: [localeInfo.TARGET_RACE_FLAG_MILGYO, 46],
    nonplayer.RACE_FLAG_FIRE: [localeInfo.TARGET_RACE_FLAG_FIRE, 50],
    nonplayer.RACE_FLAG_ICE: [localeInfo.TARGET_RACE_FLAG_ICE, 51],
    nonplayer.RACE_FLAG_DESERT: [localeInfo.TARGET_RACE_FLAG_DESERT, 52],
    nonplayer.RACE_FLAG_TREE: [localeInfo.TARGET_RACE_FLAG_TREE, 58],
	nonplayer.RACE_FLAG_ATT_ELEC: ["-", 0],
	nonplayer.RACE_FLAG_ATT_FIRE: ["-", 0],
	nonplayer.RACE_FLAG_ATT_ICE: ["-", 0],
	nonplayer.RACE_FLAG_ATT_WIND: ["-", 0],
	nonplayer.RACE_FLAG_ATT_EARTH: ["-", 0],
	nonplayer.RACE_FLAG_ATT_DARK: ["-", 0],
	nonplayer.RACE_FLAG_BOSS: [localeInfo.TARGET_RACE_FLAG_BOSS, 152],
	nonplayer.RACE_FLAG_WLADCA: [localeInfo.TARGET_RACE_FLAG_WLADCA, 153],
	nonplayer.RACE_FLAG_STONE: [localeInfo.TARGET_RACE_FLAG_STONE, 154],
	nonplayer.RACE_FLAG_DUNGEON: [localeInfo.TARGET_RACE_FLAG_DUNGEON, 159],
	nonplayer.RACE_FLAG_LEGENDA: [localeInfo.TARGET_RACE_FLAG_LEGENDA, 160],
}

if app.ENABLE_DROP_INFO:
	import item
	import uiToolTip

	class Grid:
		def __init__(self, width, height):
			self.grid = [False] * (width * height)
			self.width = width
			self.height = height

		def __str__(self):
			output = "Grid {}x{} Information\n".format(self.width, self.height)
			for row in range(self.height):
				for col in range(self.width):
					output += "Status of %d: " % (row * self.width + col)
					output += "NotEmpty, " if self.grid[row *
														self.width + col] else "Empty, "
				output += "\n"

			return output

		def find_blank(self, width, height):
			if width > self.width or height > self.height:
				return -1

			for row in range(self.height):
				for col in range(self.width):
					index = row * self.width + col
					if self.is_empty(index, width, height):
						return index

			return -1

		def put(self, pos, width, height):
			if not self.is_empty(pos, width, height):
				return False

			for row in range(height):
				start = pos + (row * self.width)
				self.grid[start] = True
				col = 1
				while col < width:
					self.grid[start + col] = True
					col += 1

			return True

		def clear(self, pos, width, height):
			if pos < 0 or pos >= (self.width * self.height):
				return

			for row in range(height):
				start = pos + (row * self.width)
				self.grid[start] = True
				col = 1
				while col < width:
					self.grid[start + col] = False
					col += 1

		def is_empty(self, pos, width, height):
			if pos < 0:
				return False

			row = pos // self.width
			if (row + height) > self.height:
				return False

			if (pos + width) > ((row * self.width) + self.width):
				return False

			for row in range(height):
				start = pos + (row * self.width)
				if self.grid[start]:
					return False

				col = 1
				while col < width:
					if self.grid[start + col]:
						return False
					col += 1

			return True

		def get_size(self):
			return self.width * self.height

		def reset(self):
			self.grid = [False] * (self.width * self.height)


	# @https://stackoverflow.com/questions/579310/formatting-long-numbers-as-strings-in-python
	def human_format(num):
		if num == 0:
			return "-"
		num = float('{:.3g}'.format(num))
		magnitude = 0
		while abs(num) >= 1000:
			magnitude += 1
			num /= 1000.0
		return '{}{}'.format('{:f}'.format(num).rstrip('0').rstrip('.'), ['', 'K', 'KK', 'KKK', 'KKKK'][magnitude])

	class DropInfoBoard(ui.ThinBoard):

		MARGIN = 8

		def __init__(self):
			ui.ThinBoard.__init__(self)
			self.wndItem = None
			self.slotDataDict = {}
			self.itemNameList = []
			self.chestdropinfo = uiChestDropInfo.ChestDropInfoWindow()
			self.toolTipItem = uiToolTip.ItemToolTip()
			self.toolTipItem.HideToolTip()

		def __del__(self):
			ui.ThinBoard.__del__(self)

		def GetWidth(self):
			return self.MARGIN * 2 + (9 * 32)

		def Refresh(self, mob_vnum):
			self.Show()

			self.mob_vnum = mob_vnum

			data = constInfo.dropInfoDict.get(mob_vnum)
			if not data: return

			row = self.__UpdateGridSize(data)

			if self.wndItem:
				self.wndItem.ClearSlot(self.wndItem.GetSlotCount())
				self.wndItem.Hide()
				self.wndItem = None

			self.wndItem = ui.GridSlotWindow()
			self.wndItem.SetParent(self)
			self.wndItem.SetPosition(self.MARGIN, self.MARGIN)
			self.wndItem.ArrangeSlot(0, 9, row, 32, 32, 0, 0)
			self.wndItem.SetSlotBaseImage("d:/ymir work/ui/public/Slot_Base.sub", 1.0, 1.0, 1.0, 1.0)
			self.wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			# self.wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.UseSlotEvent))
			# self.wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseSlotEvent))
			self.wndItem.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UseSlotEvent)
			self.wndItem.Show()

			for pos, data in self.slotDataDict.items():
				if data["vnum"][0] == 1:
					self.wndItem.SetItemSlot(pos, data["vnum"][0], 0)
				else:
					self.wndItem.SetItemSlot(pos, data["vnum"][0], data["max_count"])

			self.wndItem.RefreshSlot()
			self.SetSize(self.MARGIN * 2 + (9 * 32), self.MARGIN * 2 + (row * 32))

		def UseSlotEvent(self, selectedSlotPos):
			data = self.slotDataDict[selectedSlotPos]

			item.SelectItem(data["vnum"][0])
			if item.GetItemType() == item.ITEM_TYPE_GIFTBOX or item.GetItemType() == item.ITEM_TYPE_GACHA:
				if app.IsPressed(app.DIK_LCONTROL):
					isMain = not app.IsPressed(app.DIK_LSHIFT)
					if item.HasDropInfo(data["vnum"][0], isMain):
						self.chestdropinfo.Open(data["vnum"][0], isMain)
					return


		def OverInItem(self, slotIndex):
			data = self.slotDataDict[slotIndex]

			self.toolTipItem.SetItemToolTip(data["vnum"][0])

			if data["min_count"] != data["max_count"]:
				self.toolTipItem.AppendSpace(5)
				self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_RANGE_COUNT % (
					human_format(data["min_count"]), human_format(data["max_count"])))

			if len(data["vnum"]) > 1:
				self.toolTipItem.AppendSpace(5)
				self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_SINGLE_COUNT)
				for vnum in data["vnum"]:
					item.SelectItem(vnum)
					self.toolTipItem.AppendTextLine(localeInfo.TOOLTIP_DROP_INFO_PREFIX % item.GetItemName())
			self.toolTipItem.AppendSpace(5)
			self.toolTipItem.AppendTextLine("|cFFc9f1c0Szansa na drop: "+str(data["percentage"]/40000)+"%")
		def OverOutItem(self):
			if self.toolTipItem:
				self.toolTipItem.HideToolTip()

		def __UpdateGridSize(self, data):
			row = len(data) / 9

			self.slotDataDict = {}
			self.itemNameList = []

			while True:
				grid_ok = True
				grid = Grid(width=9, height=row)

				for slot in data:
					if not slot["max_count"]:
						continue

					item.SelectItem(slot["vnum"][0])

					(width, height) = item.GetItemSize()
					item_name = item.GetItemName()
					if item_name.find("+") != -1:
						item_name = item_name.split("+")[0]

						if item_name in self.itemNameList:
							for key, value in self.slotDataDict.items():
								if value["name"] == item_name and slot["vnum"][0] not in value["vnum"]:
									self.slotDataDict[key]["vnum"].append(slot["vnum"][0])
									break

							continue

					pos = grid.find_blank(width, height)

					if pos == -1:
						grid_ok = False
						break

					grid.put(pos, width, height)
					slot.update({"name" : item_name})

					self.slotDataDict[pos] = slot
					self.itemNameList.append(item_name)

				if not grid_ok:
					row += 1
					self.slotDataDict = {}
					self.itemNameList = []
				else:
					break

			return row

class TargetBoard(ui.ThinBoard):

	BUTTON_NAME_LIST = (
		localeInfo.TARGET_BUTTON_WHISPER,
		localeInfo.TARGET_BUTTON_EXCHANGE,
		localeInfo.TARGET_BUTTON_FIGHT,
		localeInfo.TARGET_BUTTON_ACCEPT_FIGHT,
		localeInfo.TARGET_BUTTON_AVENGE,
		localeInfo.TARGET_BUTTON_FRIEND,
		localeInfo.TARGET_BUTTON_INVITE_PARTY,
		localeInfo.TARGET_BUTTON_LEAVE_PARTY,
		localeInfo.TARGET_BUTTON_EXCLUDE,
		localeInfo.TARGET_BUTTON_INVITE_GUILD,
		localeInfo.TARGET_BUTTON_DISMOUNT,
		localeInfo.TARGET_BUTTON_EXIT_OBSERVER,
		localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
		localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
		localeInfo.TARGET_BUTTON_BUILDING_DESTROY,
		localeInfo.TARGET_BUTTON_EMOTION_ALLOW,
		"VOTE_BLOCK_CHAT",
	)

	GRADE_NAME =	{
						nonplayer.PAWN : localeInfo.TARGET_LEVEL_PAWN,
						nonplayer.S_PAWN : localeInfo.TARGET_LEVEL_S_PAWN,
						nonplayer.KNIGHT : localeInfo.TARGET_LEVEL_KNIGHT,
						nonplayer.S_KNIGHT : localeInfo.TARGET_LEVEL_S_KNIGHT,
						nonplayer.BOSS : localeInfo.TARGET_LEVEL_BOSS,
						nonplayer.KING : localeInfo.TARGET_LEVEL_KING,
						nonplayer.ELITKA : localeInfo.TARGET_LEVEL_ELITKA,
						nonplayer.LEGENDA : localeInfo.TARGET_LEVEL_LEGENDA,
					}
	EXCHANGE_LIMIT_RANGE = 3000

	def __init__(self):
		ui.ThinBoard.__init__(self)

		name = ui.TextLine()
		name.SetParent(self)
		name.SetDefaultFontName()
		name.SetOutline()
		name.Show()
		
		hpGauge = ui.Gauge()
		hpGauge.SetParent(self)
		hpGauge.MakeGauge(130, "red")
		hpGauge.Hide()

		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			hpDecimal = ui.TextLine()
			hpDecimal.SetParent(hpGauge)
			hpDecimal.SetDefaultFontName()
			hpDecimal.SetPosition(5, 5)
			hpDecimal.SetOutline()
			hpDecimal.Hide()
		
		hpText = ui.TextLine()
		hpText.SetParent(hpGauge)
		hpText.SetPosition(0, -15)
		hpText.SetWindowHorizontalAlignCenter()
		hpText.SetHorizontalAlignCenter()
		hpText.SetOutline(True)
		hpText.Hide()

		hpText2 = ui.TextLine()
		hpText2.SetParent(hpGauge)
		hpText2.SetPosition(0, 7)
		hpText2.SetWindowHorizontalAlignCenter()
		hpText2.SetHorizontalAlignCenter()
		hpText2.SetOutline(True)
		hpText2.Hide()

		# kreska = ui.TextLine()
		# kreska.SetParent(name)
		# kreska.SetPosition(72, 3)
		# kreska.SetWindowHorizontalAlignCenter()
		# kreska.SetHorizontalAlignCenter()
		# kreska.SetOutline(True)
		# kreska.Hide()

		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_0111.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_0222.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_0333.sub")
		closeButton.SetPosition(24, 9)
		
		if app.ENABLE_DROP_INFO:
			dropInfoButton = ui.Button()
			dropInfoButton.SetParent(self)
			dropInfoButton.SetUpVisual("d:/ymir work/arezzo/target-loot-button-0.png")
			dropInfoButton.SetOverVisual("d:/ymir work/arezzo/target-loot-button-1.png")
			dropInfoButton.SetDownVisual("d:/ymir work/arezzo/target-loot-button-2.png")
			dropInfoButton.SetEvent(ui.__mem_func__(self.__OnClickDropInfoButton))
			dropInfoButton.Hide()

			self.dropInfoButton = dropInfoButton

			dropInfoBoard = DropInfoBoard()
			dropInfoBoard.SetPosition((wndMgr.GetScreenWidth() - dropInfoBoard.GetWidth()) / 2, 65)
			dropInfoBoard.Hide()

			self.dropInfoBoard = dropInfoBoard

		if localeInfo.IsARABIC():
			hpGauge.SetPosition(55, 22)
			hpGauge.SetWindowHorizontalAlignLeft()
			closeButton.SetWindowHorizontalAlignLeft()
			dropInfoButton.SetWindowHorizontalAlignLeft()
		else:
			hpGauge.SetPosition(185, 22)
			hpGauge.SetWindowHorizontalAlignRight()
			closeButton.SetWindowHorizontalAlignRight()
			dropInfoButton.SetWindowHorizontalAlignRight()

		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()

		self.buttonDict = {}
		self.showingButtonList = []
		for buttonName in self.BUTTON_NAME_LIST:
			button = ui.Button()
			button.SetParent(self)

			if localeInfo.IsARABIC():
				button.SetUpVisual("d:/ymir work/ui/public/Small_Button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/Small_Button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/Small_Button_03.sub")
			else:
				button.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")

			button.SetWindowHorizontalAlignCenter()
			button.SetText(buttonName)
			button.Hide()
			self.buttonDict[buttonName] = button
			self.showingButtonList.append(button)

		self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER].SetEvent(ui.__mem_func__(self.OnWhisper))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE].SetEvent(ui.__mem_func__(self.OnExchange))
		self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_ACCEPT_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_AVENGE].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyInvite))
		self.buttonDict[localeInfo.TARGET_BUTTON_LEAVE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyExit))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCLUDE].SetEvent(ui.__mem_func__(self.OnPartyRemove))

		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_GUILD].SAFE_SetEvent(self.__OnGuildAddMember)
		self.buttonDict[localeInfo.TARGET_BUTTON_DISMOUNT].SAFE_SetEvent(self.__OnDismount)
		self.buttonDict[localeInfo.TARGET_BUTTON_EXIT_OBSERVER].SAFE_SetEvent(self.__OnExitObserver)
		self.buttonDict[localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT].SAFE_SetEvent(self.__OnViewEquipment)
		self.buttonDict[localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY].SAFE_SetEvent(self.__OnRequestParty)
		self.buttonDict[localeInfo.TARGET_BUTTON_BUILDING_DESTROY].SAFE_SetEvent(self.__OnDestroyBuilding)
		self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW].SAFE_SetEvent(self.__OnEmotionAllow)

		self.buttonDict["VOTE_BLOCK_CHAT"].SetEvent(ui.__mem_func__(self.__OnVoteBlockChat))

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			damageButton = ui.Button()
			damageButton.SetParent(self)
			damageButton.SetUpVisual("d:/ymir work/arezzo/ranking_btn0.png")
			damageButton.SetOverVisual("d:/ymir work/arezzo/ranking_btn1.png")
			damageButton.SetDownVisual("d:/ymir work/arezzo/ranking_btn2.png")
			damageButton.SetWindowHorizontalAlignRight()
			damageButton.SetPosition(46, 26)
			damageButton.SetEvent(ui.__mem_func__(self.__OnPressDamageButton))
			damageButton.Hide()
			self.damageButton = damageButton

		bonusName = ui.TextLine()
		bonusName.SetParent(self)
		bonusName.SetPosition(0, 18)
		bonusName.SetHorizontalAlignCenter()
		bonusName.SetWindowHorizontalAlignCenter()
		bonusName.SetOutline()
		bonusName.UpdateRect()
		bonusName.Hide()
		self.bonusName = bonusName

		self.name = name
		self.hpGauge = hpGauge
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = hpDecimal
		# self.hpText = hpText
		self.hpText2 = hpText2
		self.closeButton = closeButton
		self.nameString = 0
		self.nameLength = 0
		self.vid = 0
		self.eventWhisper = None
		self.isShowButton = False

		self.__Initialize()
		self.ResetTargetBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

		print "===================================================== DESTROYED TARGET BOARD"

	def __Initialize(self):
		self.nameString = ""
		self.nameLength = 0
		self.vid = 0
		self.isShowButton = False
		self.interface = None

	def Destroy(self):
		self.interface = None
		self.eventWhisper = None
		self.closeButton = None
		self.bonusName = None
		self.showingButtonList = None
		self.buttonDict = None
		self.name = None
		self.hpGauge = None
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal = None
		# self.hpText = None
		self.hpText2 = None

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			self.damageButton = None

		self.__Initialize()

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def OnPressedCloseButton(self):
		player.ClearTarget()
		self.Close()

	def Close(self):
		self.__Initialize()
		self.Hide()
		if app.ENABLE_DROP_INFO:
			self.dropInfoBoard.Hide()

	def Open(self, vid, name):
		if vid:
			if not constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
				if not player.IsSameEmpire(vid):
					self.Hide()
					return

			if app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
				if chr.GetVirtualNumber(vid) == 10 or chr.GetVirtualNumber(vid) == 11:
					self.Hide()
					return

			if vid != self.GetTargetVID():
				self.ResetTargetBoard()
				self.SetTargetVID(vid)
				self.SetTargetName(name)

			if player.IsMainCharacterIndex(vid):
				self.__ShowMainCharacterMenu()
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
				self.Hide()
			else:
				self.RefreshButton()
				self.Show()
		else:
			self.HideAllButton()
			self.__ShowButton(localeInfo.TARGET_BUTTON_WHISPER)
			self.__ShowButton("VOTE_BLOCK_CHAT")
			self.__ArrangeButtonPosition()
			self.SetTargetName(name)
			self.Show()

	def Refresh(self):
		if self.IsShow():
			if self.IsShowButton():
				self.RefreshButton()

	def RefreshByVID(self, vid):
		if vid == self.GetTargetVID():
			self.Refresh()

	def RefreshByName(self, name):
		if name == self.GetTargetName():
			self.Refresh()

	def __ShowMainCharacterMenu(self):
		canShow=0

		self.HideAllButton()

		if player.IsMountingHorse():
			self.__ShowButton(localeInfo.TARGET_BUTTON_DISMOUNT)
			canShow=1

		if player.IsObserverMode():
			self.__ShowButton(localeInfo.TARGET_BUTTON_EXIT_OBSERVER)
			canShow=1

		if canShow:
			self.__ArrangeButtonPosition()
			self.Show()
		else:
			self.Hide()

	def __ShowNameOnlyMenu(self):
		self.HideAllButton()

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def UpdatePosition(self):
		self.SetPosition(wndMgr.GetScreenWidth()/2 - self.GetWidth()/2, 10)

	def ResetTargetBoard(self):

		for btn in self.buttonDict.values():
			btn.Hide()

		self.__Initialize()

		self.name.SetPosition(0, 13)
		self.name.SetHorizontalAlignCenter()
		self.name.SetWindowHorizontalAlignCenter()
		self.hpGauge.Hide()
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			self.hpDecimal.Hide()
		self.bonusName.Hide()
		self.hpText2.Hide()
		self.SetSize(490, 40)
		if app.ENABLE_DROP_INFO:
			self.dropInfoButton.Hide()
			self.dropInfoBoard.Hide()

		if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
			self.damageButton.Hide()
			self.damageButton.SetUp()
			self.damageButton.Enable()

	def SetTargetVID(self, vid):
		self.vid = vid

	def SetEnemyVID(self, vid):
		self.SetTargetVID(vid)

		name = chr.GetNameByVID(vid)
		level = nonplayer.GetLevelByVID(vid)
		grade = nonplayer.GetGradeByVID(vid)

		nameFront = ""
		if -1 != level:
			nameFront += "|cFF98ff33Lv." + str(level) + " |r"
		if self.GRADE_NAME.has_key(grade):
			nameFront += "(" + self.GRADE_NAME[grade] + ") "

		if grade == 5:
			self.SetTargetName(nameFront + "|cFFffffff"+name+"|r")
		else:
			self.SetTargetName(nameFront + "|cFFeb1609"+name+"|r")

		if app.ENABLE_DROP_INFO:
			(x, y), (w, _) = self.name.GetLocalPosition(), self.name.GetTextSize()
			# self.dropInfoButton.SetPosition(x + w + self.dropInfoButton.GetWidth() + 10, y)
			self.dropInfoButton.SetPosition(42, 9)
			self.dropInfoButton.Show()

	def GetTargetVID(self):
		return self.vid

	def GetTargetName(self):
		return self.nameString

	def SetTargetName(self, name):
		self.nameString = name
		self.nameLength = len(name)
		self.name.SetText(name)

	def SetHP(self, iMinHP, iMaxHP):
		if not self.hpGauge.IsShow():
			showingButtonCount = len(self.showingButtonList)
			if chr.GetInstanceType(self.vid) == chr.INSTANCE_TYPE_PLAYER:
				if showingButtonCount > 16:
					self.SetSize(490, self.GetHeight())
				else:
					self.SetSize(490, self.GetHeight())
			else:
				self.SetSize(490, self.GetHeight())

			# self.SetSize(200 + 4 * self.nameLength, self.GetHeight())
			self.name.SetPosition(23, 10)
			self.name.SetWindowHorizontalAlignLeft()
			self.name.SetHorizontalAlignLeft()

			self.bonusName.SetPosition(23, 25)
			self.bonusName.SetWindowHorizontalAlignLeft()
			self.bonusName.SetHorizontalAlignLeft()

			self.hpGauge.Show()
			# self.hpText.Show()

			self.UpdatePosition()

		if chrmgr.HasAffectByVID(self.GetTargetVID(), chr.AFFECT_POISON):
			self.hpGauge.SetGaugeColor("lime")
		else:
			self.hpGauge.SetGaugeColor("red")


		if chr.GetInstanceType(self.vid) != chr.INSTANCE_TYPE_PLAYER and not player.IsMainCharacterIndex(self.vid):
			vnum = nonplayer.GetVnumByVID(self.GetTargetVID())
			data = nonplayer.GetAttElementFlag(vnum)
			race_flags = ["|cFF89B88D{} (+{}%)".format(text[0], 
			player.GetStatus(text[1])) for flag, text in RACE_FLAG_DICT.items() if data & flag]
			self.bonusName.SetText("Bonus: " + "|r, |cFF89B88D".join(race_flags) if race_flags else "Bonus: Brak")
			self.bonusName.UpdateRect()
			self.bonusName.Show()
			if (200+4*self.nameLength < 200+self.bonusName.GetTextWidth()):
				self.SetSize(200+self.bonusName.GetTextWidth(), 52)
			else:
				self.SetSize(200 + 4 * self.nameLength, 52)
			self.UpdatePosition()
			if chr.IsGameMaster(0):
				chat.AppendChat(chat.CHAT_TYPE_INFO, "[TEST_SERVER] Mob_Vnum: |cFFfcce00"+str(vnum)+"|r, "+"Bonus_List: " + "|r, |cFF89B88D".join(race_flags) if race_flags else "Bonus_List: None")

			if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
				combined_flags = " ".join(race_flags)
				if "Legendy" in combined_flags:
					self.damageButton.Show()

			self.hpText2.Show()
			self.hpText2.SetText("%.2f%%"
								% max(0, (float(iMinHP) / max(1, float(iMaxHP)) * 100)))

		self.hpGauge.SetPercentage(iMinHP, iMaxHP)
		if app.ENABLE_VIEW_TARGET_DECIMAL_HP:
			iMinHPText = '.'.join([i - 3 < 0 and str(iMinHP)[:i] or str(iMinHP)[i-3:i] for i in range(len(str(iMinHP)) % 3, len(str(iMinHP))+1, 3) if i])
			iMaxHPText = '.'.join([i - 3 < 0 and str(iMaxHP)[:i] or str(iMaxHP)[i-3:i] for i in range(len(str(iMaxHP)) % 3, len(str(iMaxHP))+1, 3) if i])
			self.hpDecimal.SetText(str(iMinHPText) + " / " + str(iMaxHPText))
			(textWidth, textHeight)=self.hpDecimal.GetTextSize()
			if localeInfo.IsARABIC():
				self.hpDecimal.SetPosition(120 / 2 + textWidth / 2, -16)
			else:
				self.hpDecimal.SetPosition(130 / 2 - textWidth / 2, -16)
			
			self.hpDecimal.Show()

	if app.TAKE_LEGEND_DAMAGE_BOARD_SYSTEM:
		def __OnPressDamageButton(self):
			if self.interface:
				import dbg
				dbg.TraceError("__OnPressDamageButton: %d ", self.vid)
				self.interface.OpenLegendDamageWindow(self.vid)

	def ShowDefaultButton(self):

		self.isShowButton = True
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW])
		for button in self.showingButtonList:
			button.Show()

	def HideAllButton(self):
		self.isShowButton = False
		for button in self.showingButtonList:
			button.Hide()
		self.showingButtonList = []

	def __ShowButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		self.buttonDict[name].Show()
		self.showingButtonList.append(self.buttonDict[name])

	def __HideButton(self, name):

		if not self.buttonDict.has_key(name):
			return

		button = self.buttonDict[name]
		button.Hide()

		for btnInList in self.showingButtonList:
			if btnInList == button:
				self.showingButtonList.remove(button)
				break

	def OnWhisper(self):
		if None != self.eventWhisper:
			self.eventWhisper(self.nameString)

	def OnExchange(self):
		net.SendExchangeStartPacket(self.vid)

	def OnPVP(self):
		net.SendChatPacket("/pvp %d" % (self.vid))

	def OnAppendToMessenger(self):
		net.SendMessengerAddByVIDPacket(self.vid)

	def OnPartyInvite(self):
		net.SendPartyInvitePacket(self.vid)

	def OnPartyExit(self):
		net.SendPartyExitPacket()

	def OnPartyRemove(self):
		net.SendPartyRemovePacketVID(self.vid)

	def __OnGuildAddMember(self):
		net.SendGuildAddMemberPacket(self.vid)

	def __OnDismount(self):
		net.SendChatPacket("/unmount")

	def __OnExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __OnViewEquipment(self):
		net.SendChatPacket("/view_equip " + str(self.vid))

	def __OnRequestParty(self):
		net.SendChatPacket("/party_request " + str(self.vid))

	def __OnDestroyBuilding(self):
		net.SendChatPacket("/build d %d" % (self.vid))

	def __OnEmotionAllow(self):
		net.SendChatPacket("/emotion_allow %d" % (self.vid))

	def __OnVoteBlockChat(self):
		cmd = "/vote_block_chat %s" % (self.nameString)
		net.SendChatPacket(cmd)

	def OnPressEscapeKey(self):
		self.OnPressedCloseButton()
		return True

	def IsShowButton(self):
		return self.isShowButton

	def RefreshButton(self):

		self.HideAllButton()

		if chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
			#self.__ShowButton(localeInfo.TARGET_BUTTON_BUILDING_DESTROY)
			#self.__ArrangeButtonPosition()
			return

		if player.IsPVPInstance(self.vid) or player.IsObserverMode():
			# PVP_INFO_SIZE_BUG_FIX
			self.SetSize(490, 40)
			self.UpdatePosition()
			# END_OF_PVP_INFO_SIZE_BUG_FIX
			return

		self.ShowDefaultButton()

		if guild.MainPlayerHasAuthority(guild.AUTH_ADD_MEMBER):
			if not guild.IsMemberByName(self.nameString):
				if 0 == chr.GetGuildID(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_GUILD)

		if not messenger.IsFriendByName(self.nameString):
			self.__ShowButton(localeInfo.TARGET_BUTTON_FRIEND)

		if player.IsPartyMember(self.vid):

			self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if player.IsPartyLeader(self.vid):
				self.__ShowButton(localeInfo.TARGET_BUTTON_LEAVE_PARTY)
			elif player.IsPartyLeader(player.GetMainCharacterIndex()):
				self.__ShowButton(localeInfo.TARGET_BUTTON_EXCLUDE)

		else:
			if player.IsPartyMember(player.GetMainCharacterIndex()):
				if player.IsPartyLeader(player.GetMainCharacterIndex()):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
			else:
				if chr.IsPartyMember(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY)
				else:
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)

			if player.IsRevengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_AVENGE)
			elif player.IsChallengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_ACCEPT_FIGHT)
			elif player.IsCantFightInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if not player.IsSameEmpire(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
				self.__HideButton(localeInfo.TARGET_BUTTON_FRIEND)
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

		distance = player.GetCharacterDistance(self.vid)
		if distance > self.EXCHANGE_LIMIT_RANGE:
			self.__HideButton(localeInfo.TARGET_BUTTON_EXCHANGE)
			self.__ArrangeButtonPosition()

		self.__ArrangeButtonPosition()

	def __ArrangeButtonPosition(self):
		showingButtonCount = len(self.showingButtonList)
		pos = -(showingButtonCount / 2) * 68
		if 0 == showingButtonCount % 2:
			pos += 34

		for button in self.showingButtonList:
			button.SetPosition(pos, 33)
			pos += 68
		
		if app.ENABLE_VIEW_TARGET_PLAYER_HP:
			if showingButtonCount <= 2:
				self.SetSize(490, 65)
			else:
				self.SetSize(490, 65)
		else:
			self.SetSize(490, 65)
		
		self.UpdatePosition()

	def OnUpdate(self):
		if self.isShowButton:

			exchangeButton = self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE]
			distance = player.GetCharacterDistance(self.vid)

			if distance < 0:
				return

			if exchangeButton.IsShow():
				if distance > self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

			else:
				if distance < self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

	if app.ENABLE_DROP_INFO:
		def __OnClickDropInfoButton(self):
			if self.dropInfoBoard.IsShow():
				self.dropInfoBoard.Hide()
				return

			vnum = chr.GetVirtualNumber(self.GetTargetVID())

			data = constInfo.dropInfoDict.get(vnum)
			if data:
				self.DropInfoRefresh(vnum)
			else:
				net.SendDropInfoPacket()

		def DropInfoRefresh(self, mob_vnum):
			self.dropInfoBoard.Show()
			self.dropInfoBoard.Refresh(mob_vnum)

		def DropInfoClear(self):
			if self.dropInfoBoard.IsShow():
				net.SendDropInfoPacket()
