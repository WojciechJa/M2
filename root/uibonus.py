import ui 
import app
import localeInfo
import player
import chat
import dbg

PATH = "d:/ymir work/ui/itemshop/"
REAL_PATH = "d:/ymir work/ui/bonus_window/"

bonus_data = [
	[6, localeInfo.TOOLTIP_MAX_HP],
	[8, localeInfo.TOOLTIP_MAX_SP],
	[13, localeInfo.TOOLTIP_CON],
	[15, localeInfo.TOOLTIP_INT],
	[12, localeInfo.TOOLTIP_STR],
	[14, localeInfo.TOOLTIP_DEX],
	[158, localeInfo.TOOLTIP_STAT_BONUS],
	[17, localeInfo.TOOLTIP_ATT_SPEED],
	[19, localeInfo.TOOLTIP_MOV_SPEED],
	[21, localeInfo.TOOLTIP_CAST_SPEED],
	[32, localeInfo.TOOLTIP_HP_REGEN],
	[33, localeInfo.TOOLTIP_SP_REGEN],
	[37, localeInfo.TOOLTIP_APPLY_POISON_PCT],
	[38, localeInfo.TOOLTIP_APPLY_STUN_PCT],
	[39, localeInfo.TOOLTIP_APPLY_SLOW_PCT],
	[40, localeInfo.TOOLTIP_APPLY_CRITICAL_PCT],
	[41, localeInfo.TOOLTIP_APPLY_PENETRATE_PCT],
	[53, localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER],
	[44, localeInfo.TOOLTIP_APPLY_ATTBONUS_ANIMAL],
	[45, localeInfo.TOOLTIP_APPLY_ATTBONUS_ORC],
	[46, localeInfo.TOOLTIP_APPLY_ATTBONUS_MILGYO],
	[47, localeInfo.TOOLTIP_APPLY_ATTBONUS_UNDEAD],
	[48, localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL],
	[152, localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS],
	[153, localeInfo.TOOLTIP_APPLY_ATTBONUS_WLADCA],
	[154, localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE],
	[159, localeInfo.TOOLTIP_ATTBONUS_DUNGEON],
	[160, localeInfo.TOOLTIP_APPLY_ATTBONUS_LEGENDA],
	[163, localeInfo.TOOLTIP_APPLY_DMG_BONUS],
	[164, localeInfo.TOOLTIP_APPLY_FINAL_DMG_BONUS],
	[43, localeInfo.TOOLTIP_APPLY_ATTBONUS_HUMAN],
	[54, localeInfo.TOOLTIP_APPLY_ATTBONUS_WARRIOR],
	[55, localeInfo.TOOLTIP_APPLY_ATTBONUS_ASSASSIN],
	[56, localeInfo.TOOLTIP_APPLY_ATTBONUS_SURA],
	[57, localeInfo.TOOLTIP_APPLY_ATTBONUS_SHAMAN],
	[63, localeInfo.TOOLTIP_APPLY_STEAL_HP],
	[64, localeInfo.TOOLTIP_APPLY_STEAL_SP],
	[65, localeInfo.TOOLTIP_APPLY_MANA_BURN_PCT],
	[66, localeInfo.TOOLTIP_APPLY_DAMAGE_SP_RECOVER],
	[67, localeInfo.TOOLTIP_APPLY_BLOCK],
	[68, localeInfo.TOOLTIP_APPLY_DODGE],
	[69, localeInfo.TOOLTIP_APPLY_RESIST_SWORD],
	[70, localeInfo.TOOLTIP_APPLY_RESIST_TWOHAND],
	[71, localeInfo.TOOLTIP_APPLY_RESIST_DAGGER],
	[72, localeInfo.TOOLTIP_APPLY_RESIST_BELL],
	[73, localeInfo.TOOLTIP_APPLY_RESIST_FAN],
	[74, localeInfo.TOOLTIP_RESIST_BOW],
	[79, localeInfo.TOOLTIP_APPLY_REFLECT_MELEE],
	[80, localeInfo.TOOLTIP_APPLY_REFLECT_CURSE],
	[81, localeInfo.TOOLTIP_APPLY_POISON_REDUCE],
	[82, localeInfo.TOOLTIP_APPLY_KILL_SP_RECOVER],
	[83, localeInfo.TOOLTIP_APPLY_EXP_DOUBLE_BONUS],
	[84, localeInfo.TOOLTIP_APPLY_GOLD_DOUBLE_BONUS],
	[85, localeInfo.TOOLTIP_APPLY_ITEM_DROP_BONUS],
	[87, localeInfo.TOOLTIP_APPLY_KILL_HP_RECOVER],
	[88, localeInfo.TOOLTIP_APPLY_IMMUNE_STUN],
	[89, localeInfo.TOOLTIP_APPLY_IMMUNE_SLOW],
	[90, localeInfo.TOOLTIP_APPLY_IMMUNE_FALL],
	[95, localeInfo.TOOLTIP_ATT_GRADE],
	[96, localeInfo.TOOLTIP_DEF_GRADE],
	[97, localeInfo.TOOLTIP_MAGIC_ATT_GRADE],
	[98, localeInfo.TOOLTIP_MAGIC_DEF_GRADE],
	[93, localeInfo.TOOLTIP_MALL_ATTBONUS],
	[115, localeInfo.TOOLTIP_MALL_DEFBONUS],
	[116, localeInfo.TOOLTIP_MALL_EXPBONUS],
	[117, localeInfo.TOOLTIP_MALL_ITEMBONUS],
	[118, localeInfo.TOOLTIP_MALL_GOLDBONUS],
	[121, localeInfo.TOOLTIP_SKILL_DAMAGE_BONUS],
	[122, localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS],
	[123, localeInfo.TOOLTIP_SKILL_DEFEND_BONUS],
	[124, localeInfo.TOOLTIP_NORMAL_HIT_DEFEND_BONUS],
	[119, localeInfo.TOOLTIP_APPLY_MAX_HP_PCT],
	[120, localeInfo.TOOLTIP_APPLY_MAX_SP_PCT],
	[131, localeInfo.TOOLTIP_MAGIC_ATTBONUS_PER],
	[132, localeInfo.TOOLTIP_MELEE_MAGIC_ATTBONUS_PER],
	[136, localeInfo.TOOLTIP_ANTI_CRITICAL_PCT],
	[137, localeInfo.TOOLTIP_ANTI_PENETRATE_PCT],
	[151, localeInfo.TOOLTIP_APPLY_RESIST_HUMAN],
	[155, localeInfo.TOOLTIP_RESIST_BOSS],
	[156, localeInfo.TOOLTIP_RESIST_WLADCA],
	[157, localeInfo.TOOLTIP_RESIST_MONSTER],
	[162, localeInfo.TOOLTIP_APPLY_RESIST_KLASY],
	[59, localeInfo.TOOLTIP_APPLY_RESIST_WARRIOR],
	[61, localeInfo.TOOLTIP_APPLY_RESIST_ASSASSIN],
	[60, localeInfo.TOOLTIP_APPLY_RESIST_SURA],
	[62, localeInfo.TOOLTIP_APPLY_RESIST_SHAMAN],
]

class BonusWindow(ui.ScriptWindow):
	def __init__(self): 
		ui.ScriptWindow.__init__(self) 
		self.isLoaded = 0
		self.BonusUpdate = 0
		self.slot = {}
		self.textLine = {}
		self.RefreshBonus()
		self.__LoadWindow()

	def __del__(self): 
		ui.ScriptWindow.__del__(self) 

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/bonus.py")
		except:
			import exception
			exception.Abort("BonusWindow.LoadDialog.LoadScript")

		self.board = self.GetChild("border")
		self.titleBar = self.GetChild("TitleBar")
		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.InputValue = self.GetChild("InputValue")

		self.scrollBar = ScrollBar()
		self.scrollBar.SetParent(self)
		self.scrollBar.SetScrollBarSize(380)
		self.scrollBar.SetPosition(282, 29)
		self.scrollBar.EnableSmoothMode()
		self.scrollBar.Show()

		self.test = ui.NewListBox()
		self.test.SetParent(self.board)
		self.test.SetSize(260, 340)
		self.test.SetPosition(3, 2)
		self.test.itemStep = 0
		self.test.Show()

		self.test.SetScrollBar(self.scrollBar)
		self.test.OnMouseWheel = self.scrollBar.OnMouseWheel

		for i in xrange(len(bonus_data)):
			self.slot[i] = Slot()
			self.slot[i].SetText("-")

			self.test.AppendItem(self.slot[i])

		self.SetCenterPosition()

	def RefreshBonus(self):
		if self.isLoaded==0:
			return
		try:
			for i in xrange(len(bonus_data)):
				self.slot[i].SetText(bonus_data[i][1](player.GetStatus(bonus_data[i][0])))

				if bonus_data[i][0] == 88:
					if player.GetStatus(88):
						self.slot[i].SetText("|cFF89b88d"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))
					else:
						self.slot[i].SetText("|cFFe57875"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))

				if bonus_data[i][0] == 89:
					if player.GetStatus(89) >= 1:
						self.slot[i].SetText("|cFF89b88d"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))
					else:
						self.slot[i].SetText("|cFFe57875"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))

				if bonus_data[i][0] == 90:
					if player.GetStatus(90) >= 1:
						self.slot[i].SetText("|cFF89b88d"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))
					else:
						self.slot[i].SetText("|cFFe57875"+bonus_data[i][1](player.GetStatus(bonus_data[i][0])))

		except:
			import exception
			exception.Abort("BonusWindow.RefreshBonus error.")

	def OnUpdate(self):
		self.BonusUpdate += 1
		if self.BonusUpdate == 25:
			self.BonusUpdate = 0
			self.RefreshBonus()

		search_text = self.InputValue.GetText().lower()

		if not hasattr(self, 'previous_search_text'):
			self.previous_search_text = ""

		if search_text != self.previous_search_text:
			self.previous_search_text = search_text

			self.test.ClearItems()

			if search_text:
				matched_indices = [i for i in xrange(len(bonus_data)) if search_text in self.slot[i].GetText().lower()]
				for i in matched_indices:
					self.test.AppendItem(self.slot[i])
			else:
				for i in xrange(len(bonus_data)):
					self.test.AppendItem(self.slot[i])
		
	def Close(self):
		self.Hide()	
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		
	def Open(self):
		self.Show()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

class Slot(ui.NewListBoxItem):
	def __init__(self):
		ui.NewListBoxItem.__init__(self)
		self.__LoadWindow()
		self.OnRender = None

	def __LoadWindow(self):
		self.slot = ui.ImageBox()
		self.slot.SetParent(self)
		self.slot.SetPosition(0, 0)
		self.slot.LoadImage(REAL_PATH + "left_box.png")
		self.slot.Show()

		self.txt = ui.TextLine()
		self.txt.SetParent(self.slot)
		self.txt.SetFontName("Tahoma:14")
		self.txt.SetPosition(self.slot.GetWidth()/2, 3)
		self.txt.SetHorizontalAlignCenter()
		self.txt.SetText("-")
		self.txt.Show()

		self.RegisterComponent(self.slot)
		self.RegisterComponent(self.txt)

		self.SetSize(self.slot.GetWidth(), self.slot.GetHeight())

	def ClearDictionary(self):
		self.UnregisterComponent(self.slot)
		self.UnregisterComponent(self.txt)

		self.slot = 0
		self.txt = 0

		self.Hide()

	def SetText(self, text):
		self.txt.SetText(text)

	def GetText(self):
		return self.txt.GetText()

	def __del__(self):
		ui.NewListBoxItem.__del__(self)

class ScrollBar(ui.Window):
	SCROLLBAR_WIDTH = 17
	SCROLLBAR_MIDDLE_HEIGHT = 9
	SCROLLBAR_BUTTON_HEIGHT = 0
	MIDDLE_BAR_POS = 8
	MIDDLE_BAR_UPPER_PLACE = 3
	MIDDLE_BAR_DOWNER_PLACE = 5
	TEMP_SPACE = MIDDLE_BAR_UPPER_PLACE + MIDDLE_BAR_DOWNER_PLACE
	if app.__BL_SMOOTH_SCROLL__:
		SMOOTH_RATIO = 1.5 # UPPER = SLOWER

	def __init__(self):
		ui.Window.__init__(self)

		self.pageSize = 1
		self.curPos = 0.0
		self.eventScroll = lambda *arg: None
		self.lockFlag = False
		self.scrollStep = 0.20
		if app.__BL_SMOOTH_SCROLL__:
			self.smooth_mode = False
			self.actual_pos = 0.0
			self.target_pos = 0.0

		self.CreateScrollBar()

	def __del__(self):
		ui.Window.__del__(self)

	def CreateScrollBar(self):
		barSlot = ui.ExpandedImageBox()
		barSlot.SetParent(self)
		barSlot.AddFlag("not_pick")
		barSlot.LoadImage(PATH+"scroll_slot.png")
		barSlot.Show()

		middleBar = ui.DragButton()
		middleBar.SetParent(self)
		middleBar.AddFlag("movable")
		middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
		middleBar.SetUpVisual(PATH+"scroll_button.png")
		middleBar.SetOverVisual(PATH+"scroll_button.png")
		middleBar.SetDownVisual(PATH+"scroll_button.png")
		middleBar.Show()

		self.middleBar = middleBar
		self.barSlot = barSlot

		self.SCROLLBAR_WIDTH = self.middleBar.GetWidth()
		self.SCROLLBAR_MIDDLE_HEIGHT = self.middleBar.GetHeight()

	def Destroy(self):
		self.middleBar = None
		self.eventScroll = lambda *arg: None

	def SetScrollEvent(self, event):
		self.eventScroll = event

	def SetMiddleBarSize(self, pageScale):
		pass

	def SetScrollBarSize(self, height):
		self.pageSize = (height - self.SCROLLBAR_BUTTON_HEIGHT * 2) - self.SCROLLBAR_MIDDLE_HEIGHT - (self.TEMP_SPACE)
		self.SetSize(self.SCROLLBAR_WIDTH, height)
		self.middleBar.SetRestrictMovementArea(self.MIDDLE_BAR_POS,
											   self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE,
											   self.MIDDLE_BAR_POS + 2,
											   height - self.SCROLLBAR_BUTTON_HEIGHT * 2 - self.TEMP_SPACE)
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS, 0)

		self.UpdateBarSlot()

	def UpdateBarSlot(self):
		self.barSlot.SetScale(1.0, float(self.GetHeight()) / float(self.barSlot.GetHeight()))

	def GetPos(self):
		return self.curPos

	def SetPos(self, pos):
		pos = max(0.0, pos)
		pos = min(1.0, pos)

		newPos = float(self.pageSize) * pos
		self.middleBar.SetPosition(self.MIDDLE_BAR_POS,
								   int(newPos) + self.SCROLLBAR_BUTTON_HEIGHT + self.MIDDLE_BAR_UPPER_PLACE)
		self.OnMove()

	def SetScrollStep(self, step):
		self.scrollStep = step

	def GetScrollStep(self):
		return self.scrollStep

	def OnMouseWheel(self, nLen):
		if nLen > 0:
			self.OnUp()
			return True
		elif nLen < 0:
			self.OnDown()
			return True
		return False

	def OnUp(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos-self.scrollStep))
		else:
			self.SetPos(self.curPos-self.scrollStep)

	def OnDown(self):
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, self.curPos+self.scrollStep))
		else:
			self.SetPos(self.curPos+self.scrollStep)

	def OnMove(self):

		if self.lockFlag:
			return

		if 0 == self.pageSize:
			return

		(xLocal, yLocal) = self.middleBar.GetLocalPosition()
		self.curPos = float(yLocal - self.SCROLLBAR_BUTTON_HEIGHT - self.MIDDLE_BAR_UPPER_PLACE) / float(self.pageSize)

		self.eventScroll()

	def OnMouseLeftButtonDown(self):
		(xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
		pickedPos = yMouseLocalPosition - self.SCROLLBAR_BUTTON_HEIGHT - self.SCROLLBAR_MIDDLE_HEIGHT / 2
		newPos = float(pickedPos) / float(self.pageSize)
		if app.__BL_SMOOTH_SCROLL__ and self.smooth_mode:
			self.actual_pos = min(1.0, max(0.0, self.curPos))
			self.target_pos = min(1.0, max(0.0, newPos))
		else:
			self.SetPos(newPos)

	def LockScroll(self):
		self.lockFlag = True

	def UnlockScroll(self):
		self.lockFlag = False

	if app.__BL_SMOOTH_SCROLL__:
		def EnableSmoothMode(self):
			self.smooth_mode = True

		def OnUpdate(self):
			if not self.smooth_mode:
				return
			
			if self.lockFlag:
				return
			
			if 0 == self.pageSize:
				return
			
			if self.actual_pos == self.target_pos:
				return

			distance = abs(self.actual_pos - self.target_pos)
			smooth_step = max(distance / ScrollBar.SMOOTH_RATIO, 0.005)
		
			if self.actual_pos < self.target_pos:
				self.actual_pos = min(self.actual_pos + smooth_step, self.target_pos)
			elif self.actual_pos > self.target_pos:
				self.actual_pos = max(self.actual_pos - smooth_step, self.target_pos)
			
			self.SetPos(self.actual_pos)
