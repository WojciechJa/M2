#blackdragonx61 (28.11.21)
import ui
import item
import uiToolTip
# import shop
import app
# import chat
import player
import constInfo

class ChestDropInfoWindow(ui.ScriptWindow):
	DROP_SLOT_SIZE = 5 * 8
	ITEM = 0
	COUNT = 1

	def __init__(self) :
		ui.ScriptWindow.__init__(self)
		self.isLoaded = False
		self.ItemVnum = -1
		self.PageCount = 0
		self.CurrentPage = 0
		self.DropDict = dict()
		self.MainItemSlot = None
		self.DropItemSlot = None
		self.prevButton = None
		self.nextButton = None
		self.currentPageText = None
		self.tooltipitem = uiToolTip.ItemToolTip()

	def __del__(self) :
		ui.ScriptWindow.__del__(self)
		self.DropDict = None
		self.MainItemSlot = None
		self.DropItemSlot = None
		self.prevButton = None
		self.nextButton = None
		self.currentPageText = None
		self.tooltipitem = None
		
	def __LoadWindow(self):
		if self.isLoaded:
			return

		self.isLoaded = True

		# script
		try:
			self.__LoadScript("UIScript/ChestDropInfoWindow.py")
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__LoadScript")

		# object
		try:
			self.__BindObject()
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__BindObject")

		# event
		try:
			self.__BindEvent()
		except:
			import exception
			exception.Abort("ChestDropInfoWindow.__LoadWindow.__BindEvent")

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __BindObject(self):
		self.MainItemSlot		= self.GetChild("main_item_slot")
		self.DropItemSlot		= self.GetChild("drop_item_slot")
		self.prevButton			= self.GetChild("prev_button")
		self.nextButton			= self.GetChild("next_button")
		self.currentPageText	= self.GetChild("CurrentPage")
		self.backbtn			= self.GetChild("BackBtn")
		self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.title = self.GetChild("TitleName")

	def __BindEvent(self):
		# self.GetChild("board").SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.MainItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInMainItemSlot))
		self.MainItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.DropItemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInDropItemSlot))
		self.DropItemSlot.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UseSlotEvent)
		self.DropItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		self.backbtn.SetEvent(ui.__mem_func__(self.BackBtn))

		self.prevButton.SetEvent(ui.__mem_func__(self.SetPage), -1)
		self.nextButton.SetEvent(ui.__mem_func__(self.SetPage), +1)
	
	def BtnCheck(self):
		item.SelectItem(int(constInfo.LAST_CHESTINFO_VNUM))

		if constInfo.LAST_CHESTINFO_VNUM == 0:
			self.backbtn.Hide()
		else:
			if str(self.ItemVnum) == str(constInfo.LAST_CHESTINFO_VNUM):
				self.backbtn.Hide()
			else:
				if not item.GetItemType() in (item.ITEM_TYPE_GIFTBOX, item.ITEM_TYPE_GACHA):
					self.backbtn.Hide()
				else:
					self.backbtn.Show()

	def BackBtn(self):
		if constInfo.LAST_CHESTINFO_VNUM == 0:
			return

		isMain = not app.IsPressed(app.DIK_LSHIFT)
		self.Open(constInfo.LAST_CHESTINFO_VNUM, isMain)
		constInfo.LAST_CHESTINFO_VNUM = 0
		self.backbtn.Hide()

	def UseSlotEvent(self, slotIndex):
		constInfo.LAST_CHESTINFO_VNUM = self.ItemVnum

		data = self.DropDict[self.CurrentPage][slotIndex]
		item_vnum = data[ChestDropInfoWindow.ITEM]
		item.SelectItem(item_vnum)
		if item.GetItemType() == item.ITEM_TYPE_GIFTBOX or item.GetItemType() == item.ITEM_TYPE_GACHA:
			if app.IsPressed(app.DIK_LCONTROL):
				isMain = not app.IsPressed(app.DIK_LSHIFT)
				if item.HasDropInfo(item_vnum, isMain):
					self.Open(item_vnum, isMain)
				return

	def UpdateItems(self):
		for i in range(ChestDropInfoWindow.DROP_SLOT_SIZE):
			self.DropItemSlot.ClearSlot(i)

		if self.CurrentPage in self.DropDict:
			for pos in self.DropDict[self.CurrentPage]:
				data = self.DropDict[self.CurrentPage][pos]
				self.DropItemSlot.SetItemSlot(pos, data[ChestDropInfoWindow.ITEM], data[ChestDropInfoWindow.COUNT])
		
		self.DropItemSlot.RefreshSlot()
	
	def SetPage(self, page):
		nextpage = page + self.CurrentPage
		if 0 <= nextpage <= self.PageCount:
			self.CurrentPage = nextpage
			self.currentPageText.SetText(str(self.CurrentPage + 1))
			self.UpdateItems()
	
	def SetUp(self, itemVnum, isMain):
		self.BtnCheck()
		self.ItemVnum = itemVnum
		self.MainItemSlot.SetItemSlot(0, self.ItemVnum, 0)
		self.MainItemSlot.RefreshSlot()

		item.SelectItem(itemVnum)
		itemanem = item.GetItemName()
		self.title.SetText(itemanem)


		(self.PageCount, DropList) = item.GetDropInfo(self.ItemVnum, isMain)

		self.DropDict.clear()
		for i in range(self.PageCount + 1):
			self.DropDict[i] = dict()

		for page, pos, vnum, count in DropList:
			self.DropDict[page][pos] = (vnum, count)

		self.CurrentPage = 0
		self.SetPage(0)
	
	def Open(self, itemVnum, isMain):
		constInfo.CHESTDROP_INFO_IS_OPEN = 1
		if self.IsShow():
			self.Close()

		self.__LoadWindow()
		self.SetUp(itemVnum, isMain)
		self.BtnCheck()
		# self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def SetItemToolTip(self, toolTip):
		self.toolTipItem = toolTip

	def OverInMainItemSlot(self, slotIndex):
		if self.tooltipitem:
			self.tooltipitem.SetItemToolTip(self.ItemVnum)

	def OverInDropItemSlot(self, slotIndex):
		if self.tooltipitem:
			if self.CurrentPage in self.DropDict:
				if slotIndex in self.DropDict[self.CurrentPage]:
					data = self.DropDict[self.CurrentPage][slotIndex]
					self.tooltipitem.SetItemToolTip(data[ChestDropInfoWindow.ITEM])

	def OverOutItem(self):
		if self.tooltipitem:
			self.tooltipitem.HideToolTip()
			self.tooltipitem.ClearToolTip()
	
	def Close(self):
		constInfo.CHESTDROP_INFO_IS_OPEN = 0
		self.OverOutItem()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True