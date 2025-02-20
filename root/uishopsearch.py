import ui
import net
import item
import dbg
import app
import chat
import player
import uiToolTip
import mouseModule
import snd

if app.ENABLE_VS_SHOP_SEARCH:
	PATH = "d:/ymir work/ui/itemshop/"
	START_WIDTH = 175
	START_HEIGHT = 140
	HEIGHT_STEP = 20

	CATEGORY_LIST = [
		#NAZWA 		#TYPE #SUBTYPE
		["Kostiumy", 28, 0],
		["Fryzury", 28, 1],
		["Wierzchowce", 28, 2],
		["Zwierzaki", 5, 5],
		["Nak³adki (Bronie)", 28, 5],
		["Nak³adki (Szarfy)", 28, 4],
		["Szarfy", 28, 3],
		["Emblematy", 28, 7],
		["Alchemia", 29, 0],
		["Artefakty", 46, 0],
		["Runy", 45, 0],
		["Przetopy", 3, 21],
	]

	class SearchWindow(ui.ScriptWindow):
		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.isLoaded = 0
			self.grid = None
			self.vnu_list = []
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()
			self.LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/shopsearch.py")
			except:
				import exception
				exception.Abort("shopsearch.LoadDialog.LoadScript")
			try:
				self.TitleBar = self.GetChild("TitleBar")
				self.inputValue = self.GetChild("InputValue")
				self.inputValue1 = self.GetChild("InputValue1")
				self.SearchButton = self.GetChild("SearchButton0")
				self.SearchButton1 = self.GetChild("SearchButton1")
				self.SearchButton2 = self.GetChild("SearchButton2")
				self.ClearButton1 = self.GetChild("ClearButton1")
				self.ClearButton0 = self.GetChild("ClearButton0")
				self.border = self.GetChild("border2")
				self.border1 = self.GetChild("border4")
				self.item_slot = self.GetChild("item_slot")
			except:
				import exception
				exception.Abort("shopsearch.LoadDialog.BindObject")

			self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
			self.inputValue1.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate1)

			self.SearchButton.SetEvent(ui.__mem_func__(self.OnPressSearch), 0)
			self.SearchButton2.SetEvent(ui.__mem_func__(self.OnPressSearch), 1)
			self.SearchButton1.SetEvent(ui.__mem_func__(self.OnPressSearch), 2)

			self.ClearButton1.SetEvent(ui.__mem_func__(self.OnPressClearSearch))
			self.ClearButton0.SetEvent(ui.__mem_func__(self.OnPressClearSearch0))
			self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			self.item_slot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.item_slot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.item_slot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.ItemsList = ui.ListBoxEx()
			self.ItemsList.SetParent(self.border)
			self.ItemsList.SetSize(214, 163)
			self.ItemsList.SetPosition(2, 2)
			self.ItemsList.SetSelectEvent(self.OnSelectCategory)
			self.ItemsList.SetItemSize(214, 21)
			self.ItemsList.SetViewItemCount(8)
			self.ItemsList.Show()

			self.catList = ui.ListBoxEx()
			self.catList.SetParent(self.border1)
			self.catList.SetSize(214, 200)
			self.catList.SetPosition(2, 2)
			# self.catList.SetSelectEvent(self.OnSelectCategory1)
			self.catList.SetItemSize(214, 21)
			self.catList.SetViewItemCount(8)
			self.catList.Show()

			self.scrollBar = ScrollBar()
			self.scrollBar.SetParent(self)
			self.scrollBar.SetScrollBarSize(205)
			self.scrollBar.SetPosition(224, 57)
			self.scrollBar.EnableSmoothMode()
			self.scrollBar.Show()

			self.catScrollBar = ScrollBar()
			self.catScrollBar.SetParent(self)
			self.catScrollBar.SetScrollBarSize(205)
			self.catScrollBar.SetPosition(646, 57)
			self.catScrollBar.EnableSmoothMode()
			self.catScrollBar.Show()

			self.ItemsList.SetScrollBar(self.scrollBar)
			self.ItemsList.OnMouseWheel = self.scrollBar.OnMouseWheel

			self.catList.SetScrollBar(self.catScrollBar)
			self.catList.OnMouseWheel = self.catScrollBar.OnMouseWheel

			self.grid = Grid(6, 6)

			self.SetCenterPosition()

		def Destroy(self):
			if self.tooltipItem:
				if self.tooltipItem.IsShow():
					self.tooltipItem.HideToolTip()
			self.vnu_list = []
			self.inputValue.SetText("")
			self.inputValue1.SetText("")
			self.grid.reset()
			self.catList.RemoveAllItems()
			self.ItemsList.RemoveAllItems()
			self.RefreshSlot()
			self.Hide()
			self.ClearDictionary()

		def Close(self):
			if self.tooltipItem:
				if self.tooltipItem.IsShow():
					self.tooltipItem.HideToolTip()
			self.vnu_list = []
			self.inputValue.SetText("")
			self.inputValue1.SetText("")
			self.grid.reset()
			self.catList.RemoveAllItems()
			self.ItemsList.RemoveAllItems()
			self.RefreshSlot()
			self.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return True

		def Show(self):
			self.LoadWindow()
			ui.ScriptWindow.Show(self)
			self.UpdateItemsList()

		def Open(self):
			self.Show()

		def OverInItem(self, slotIndex):
			for i, (pos, itemVnum) in enumerate(self.vnu_list):
				if pos == slotIndex:
					item.SelectItem(itemVnum)
					self.tooltipItem.SetItemToolTip(int(item.GetItemVnum()))

		def OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def __OnValueUpdate(self):
			ui.EditLine.OnIMEUpdate(self.inputValue)
			self.ItemsList.RemoveAllItems()
			self.UpdateItemsList()

		def __OnValueUpdate1(self):
			ui.EditLine.OnIMEUpdate(self.inputValue1)
			self.catList.RemoveAllItems()
			self.UpdateItemsList()

		def OnSelectCategory(self, list):
			self.selectedItem = list.GetIDX()

		def OnPressSearch(self, search_type):
			if search_type == 0:
				selected_item = self.ItemsList.GetSelectedItem()
				if selected_item is not None:
					self.selectedVnum = selected_item.GetIDX()
					net.SendChatPacket("/search_in_shops 0 " + str(self.selectedVnum) + " 0 0 0")
				else:
					return
			elif search_type == 1:
				selected_item = self.catList.GetSelectedItem()
				if selected_item is not None:
					selected_index = selected_item.GetIDX()
					type = CATEGORY_LIST[selected_index][1]
					subtype = CATEGORY_LIST[selected_index][2]
					net.SendChatPacket("/search_in_shops 1 0 " + str(type) + " " + str(subtype))
				else:
					return
			elif search_type == 2:
				item_list = []
				for pos, itemVnum in self.vnu_list:
					item_list.append([itemVnum])
				net.SendSearch(item_list)

		def UpdateItemsList(self):
			self.catList.RemoveAllItems()

			#wyszukiwarka kategorii
			if len(self.inputValue1.GetText()) > 2:
				search_text = self.inputValue1.GetText().lower()
				for index, (item_name, item_id, item_subtype) in enumerate(CATEGORY_LIST):
					if search_text in item_name.lower():
						item2_instance = Item2(item_name, index, self)
						self.catList.AppendItem(item2_instance)
			else:
				for index, (item_name, item_id, item_subtype) in enumerate(CATEGORY_LIST):
					item2_instance = Item2(item_name, index, self)
					self.catList.AppendItem(item2_instance)

			#wyszukiwarka itemu
			if len(self.inputValue.GetText()) > 2:
				items_list = item.GetItemsByName(str(self.inputValue.GetText()))
				for items in items_list:
					item.SelectItem(items)
					itemName = item.GetItemName()
					button = Item(itemName, items, item.GetItemVnum(), self)
					self.ItemsList.AppendItem(button)

		def RefreshSlot(self):
			for i in range(self.item_slot.GetSlotCount()):
				self.item_slot.ClearSlot(i)

			for pos, itemVnum in self.vnu_list:
				self.item_slot.SetItemSlot(pos, itemVnum, 1)

			self.item_slot.RefreshSlot()

		def SelectItemSlot(self, slotIndex):
			if mouseModule.mouseController.isAttached():
				snd.PlaySound("sound/ui/loginfail.wav")
				mouseModule.mouseController.DeattachObject()
				return

			for i, (pos, itemVnum) in enumerate(self.vnu_list):
				if pos == slotIndex:
					item.SelectItem(itemVnum)
					(w, h) = item.GetItemSize()
					self.vnu_list.pop(i)
					self.grid.clear(slotIndex, w, h)
					self.item_slot.ClearSlot(slotIndex)
					break

			self.RefreshSlot() 

		def OnPressClearSearch(self):
			self.vnu_list = []
			self.grid.reset()
			self.RefreshSlot()

		def OnPressClearSearch0(self):
			net.SendChatPacket("/clear_shopsearch")

	class Item(ui.ListBoxEx.Item):
		def __init__(self, text, idx, vnum, parent_window):
			ui.ListBoxEx.Item.__init__(self)
			self.text = text
			self.id = idx
			self.vnu = vnum
			self.parent_window = parent_window
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()

			self.OnMouseOverIn = ui.__mem_func__(self.__OverInItem)
			self.OnMouseOverOut = ui.__mem_func__(self.__OverOutItem)
			self.OnMouseRightButtonDown = ui.__mem_func__(self.__AddToItemSlot)

			self.textLine = self.__CreateTextLine(text[:50])
			ui.ListBoxEx.Item.SetPosition(self, 30, 0)

		def __del__(self):
			ui.ListBoxEx.Item.__del__(self)

		def GetText(self):
			return self.text

		def GetIDX(self):
			return int(self.id)

		def __OverInItem(self):
			if self.tooltipItem:
				self.tooltipItem.SetItemToolTip(self.vnu, showIcon=1)

		def __OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def __AddToItemSlot(self):
			item.SelectItem(self.vnu)
			(w, h) = item.GetItemSize()
			pos = self.parent_window.grid.find_blank(w, h)

			if pos == -1 or not self.parent_window.grid.is_empty(pos, w, h):
				return

			for existing_pos, existing_vnum in self.parent_window.vnu_list:
				if existing_vnum == self.vnu:
					return

			self.parent_window.grid.put(pos, w, h)
			self.parent_window.vnu_list.append([pos, self.vnu])

			self.parent_window.RefreshSlot()

		def __CreateTextLine(self, text):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetPosition(214/2, 3)
			textLine.SetText(text)
			textLine.Show()
			return textLine
			
	class Item2(ui.ListBoxEx.Item):
		def __init__(self, text, idx, parent_window):
			ui.ListBoxEx.Item.__init__(self)
			self.text = text
			self.id = idx
			self.parent_window = parent_window

			self.textLine = self.__CreateTextLine(text[:50])
			ui.ListBoxEx.Item.SetPosition(self, 30, 0)

		def __del__(self):
			ui.ListBoxEx.Item.__del__(self)

		def GetText(self):
			return self.text

		def GetIDX(self):
			return int(self.id)

		def __CreateTextLine(self, text):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignCenter()
			textLine.SetPosition(214/2, 3)
			textLine.SetText(text)
			textLine.Show()
			return textLine

	class Grid:
		"""
			Args:
				width (int): Grid's width.
				height (int): Grid's height.

			Attributes:
				grid (list): The list will hold the position empty or not information.
				width (int): Grid's width
				height (int): Grid's height
		"""

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
			"""
				Args:
					width (int): The item's width you can call width item.GetItemSize()[0]
					height (int): The item's height you can call width item.GetItemSize()[1]

				Returns:
					int: The return value would be an int if successful. Otherwise -1.
			"""
			if width > self.width or height > self.height:
				return -1

			for row in range(self.height):
				for col in range(self.width):
					index = row * self.width + col
					if self.is_empty(index, width, height):
						return index

			return -1

		def put(self, pos, width, height):
			"""
				Args:
					pos (int): Position of the item to put.
					width (int): The item's width you can call width item.GetItemSize()[0]
					height (int): The item's height you can call width item.GetItemSize()[1]

				Returns:
					bool: The return value. True for success, False otherwise.
			"""
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
			"""
				Args:
					pos (int): Position of the item to put.
					width (int): The item's width you can call width item.GetItemSize()[0]
					height (int): The item's height you can call width item.GetItemSize()[1]

				Returns:
					There is nothing to return
			"""
			if pos < 0 or pos >= (self.width * self.height):
				return

			for row in range(height):
				start = pos + (row * self.width)
				self.grid[start] = False
				col = 1
				while col < width:
					self.grid[start + col] = False
					col += 1

		def is_empty(self, pos, width, height):
			"""
				Args:
					pos (int): Position of the item to put.
					width (int): The item's width you can call width item.GetItemSize()[0]
					height (int): The item's height you can call width item.GetItemSize()[1]

				Returns:
					bool: The return value. True for success, False otherwise.
			"""
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
			"""
				Returns:
					int: The return value will give you maximum capacity of grid. (width * height)
			"""
			return self.width * self.height

		def reset(self):
			"""
				With this function, you can reset instead of deleting it and create again.
			"""
			self.grid = [False] * (self.width * self.height)

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
