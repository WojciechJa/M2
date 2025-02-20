#-*- coding: iso-8859-1 -*-
import ui
import net
import player
import app
import mouseModule
import item
import chat
import dbg
from _weakref import proxy
try:
	import pickle
except ImportError:
	import cPickle as pickle

class PotionsWindow(ui.BoardWithTitleBar):
	MAX_CATEGORIES = 5
	ITEMS_ROW = 10
	ITEMS_COLUMNS = 3
	ITEMS_SLOTS = ITEMS_COLUMNS*ITEMS_ROW
	
	INVENTORY_PAGES = 12
	
	EXCEPTIONS_LIST = [ 50001, 50002 ] # Add exception vnums here.
	
	def __init__(self):
		super(PotionsWindow, self).__init__()
		self.__ToolTip = None
		self.__LoadVariables()
		self.__LoadWindow()
		
	def __del__(self):
		super(PotionsWindow, self).__del__()
		
	def __LoadVariables(self):
		self.__ActualCategory = 0
		self.__PotionsList = list()
		self.__Deserialize()
	
	def __Deserialize(self):
		try:
			with open('potions.pkl', 'rb') as iFile:
				self.__ActualCategory, self.__PotionsList = pickle.load(iFile)
		except:
			for i in xrange(self.MAX_CATEGORIES):
				self.__PotionsList.append([0]*self.ITEMS_SLOTS)
				
			dbg.TraceError("Cannot load potions.pkl - memseting then")
			
	def __Serialize(self):
		with open('potions.pkl', 'wb') as iFile:
			pickle.dump([self.__ActualCategory, self.__PotionsList], iFile)
			
	def __LoadWindow(self):
		self.SetSize(332, 200)
		self.AddFlag("movable")
		self.SetTitleName("Dopalacze")
		self.SetCloseEvent(self.Hide)
		
		self.__CreateSlots()
		self.__LoadCategories()
		self.__LoadButtons()
		self.__SelectCategory(self.__ActualCategory)
		self.SetCenterPosition()
		
	def __CreateSlots(self):
		self.__Slots = ui.GridSlotWindow()
		self.__Slots.SetParent(self)
		self.__Slots.SetPosition(6, 30)
		self.__Slots.ArrangeSlot(0, self.ITEMS_ROW, self.ITEMS_COLUMNS, 32, 32, 0, 0)
		self.__Slots.SetOverInItemEvent(ui.__mem_func__(self.__OverInGrid))
		self.__Slots.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
		self.__Slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.__SelectEmptySlot))
		self.__Slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.__UseItemSlot))
		self.__Slots.SetSlotBaseImage("d:/ymir work/ui/public/slot_base.sub", 1.0, 1.0, 1.0, 1.0)
		self.__Slots.RefreshSlot()
		self.__Slots.Show()
		
	def __LoadCategories(self):
		self.__CategoryButtons = list()
		catNames = [ "I", "II", "III", "IV", "V" ]
		for i in xrange(self.MAX_CATEGORIES):
			catButton = ui.RadioButton()
			catButton.SetParent(self)
			catButton.SetPosition(75+(i*37), 30+32*self.ITEMS_COLUMNS+8)
			catButton.SetEvent(ui.__mem_func__(self.__SelectCategory), i)
			catButton.SetUpVisual("d:/ymir work/ui/game/windows/tab_button_large_half_01.sub")
			catButton.SetOverVisual("d:/ymir work/ui/game/windows/tab_button_large_half_02.sub")
			catButton.SetDownVisual("d:/ymir work/ui/game/windows/tab_button_large_half_03.sub")
			
			# catButton.SetUpVisual("d:/ymir work/ui/game/windows/tab_button_small_01.sub")
			# catButton.SetOverVisual("d:/ymir work/ui/game/windows/tab_button_small_02.sub")
			# catButton.SetDownVisual("d:/ymir work/ui/game/windows/tab_button_small_03.sub")			
			
			catButton.SetText(catNames[i])
			catButton.Show()
			self.__CategoryButtons.append(catButton)
	
	def __LoadButtons(self):
		# self.__RemoveButton = ui.Button()
		# self.__RemoveButton.SetParent(self)
		# self.__RemoveButton.SetUpVisual("d:/ymir work/ui/public/big_button_01.sub")
		# self.__RemoveButton.SetOverVisual("d:/ymir work/ui/public/big_button_02.sub")
		# self.__RemoveButton.SetDownVisual("d:/ymir work/ui/public/big_button_03.sub")
		# self.__RemoveButton.SetPosition(272, 156)
		# self.__RemoveButton.SetText("Wyczyœæ")
		# self.__RemoveButton.SetToolTipText("[i] Przycisk ten czyœci ca³¹ aktualn¹ stronê dopalaczy.")
		# self.__RemoveButton.SetEvent(ui.__mem_func__(self.__RemoveCategory))
		# self.__RemoveButton.Show()
	
		self.__ActiveButton = ui.Button()
		self.__ActiveButton.SetParent(self)
		self.__ActiveButton.SetUpVisual("d:/ymir work/ui/public/xlarge_button_01.sub")
		self.__ActiveButton.SetOverVisual("d:/ymir work/ui/public/xlarge_button_02.sub")
		self.__ActiveButton.SetDownVisual("d:/ymir work/ui/public/xlarge_button_03.sub")
		self.__ActiveButton.SetPosition(77, 160)
		self.__ActiveButton.SetText("Aktywuj (F6)")
		self.__ActiveButton.SetToolTipText("[i] Przycisk ten aktywuje, ca³¹ aktywn¹ stronê dopalaczy, wybrana strona zapisuje siê nawet po wylogowaniu, nie musisz otwieraæ okna wystarczy wcisn¹æ przycisk F6.")
		self.__ActiveButton.SetEvent(ui.__mem_func__(self.ActivateCategory))
		self.__ActiveButton.Show()
		
	def __RemoveCategory(self):
		for x in xrange(self.ITEMS_SLOTS):
			self.__PotionsList[self.__ActualCategory][x] = 0
		
		self.__RefreshSlots()
	
	def __IsInTable(self, iVnum):
		if iVnum in self.__PotionsList[self.__ActualCategory]:
			return True
		
		return False
	
	def __CanAddItem(self, iVnum):
		if item.GetItemType() == item.ITEM_TYPE_BLEND:
			return True
		
		if item.GetItemType() == item.ITEM_TYPE_USE and item.GetItemSubType() in (item.USE_POTION, item.USE_ABILITY_UP, item.USE_POTION_NODELAY, item.USE_AFFECT):
			return True
		
		if iVnum in self.EXCEPTIONS_LIST:
			return True
		
		return False
	
	def AutoSetItem(self, iVnum):
		if self.__IsInTable(iVnum):
			chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Ten przedmiot jest ju¿ dodany.")
			return
		
		iEmptySlot = self.__GetEmptySlot()
		if iEmptySlot == -1:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Nie masz wolnego miejsca.")
			return
		
		if self.__CanAddItem(iVnum) == False:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Nie mo¿esz dodaæ tego przedmiotu.")
			return
		
		self.__PotionsList[self.__ActualCategory][iEmptySlot] = iVnum
		self.__RefreshSlots()
	
	def __GetEmptySlot(self):
		for x in xrange(self.ITEMS_SLOTS):
			if self.__PotionsList[self.__ActualCategory][x] == 0:
				return x
				
		return -1
		
	def ActivateCategory(self):
		for iVnum in self.__PotionsList[self.__ActualCategory]:
			if iVnum == 0:
				continue
			
			if player.GetItemCountByVnum(iVnum) == 0:
				continue
				
			for iSlot in xrange(self.INVENTORY_PAGES*player.INVENTORY_PAGE_SIZE):
				if player.GetItemIndex(iSlot) == iVnum:
					net.SendItemUsePacket(iSlot)
					break
		
		chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Aktywowano stronê: {}".format(self.__ActualCategory+1))
		
	def __SelectCategory(self, catID):
		self.__CategoryButtons[self.__ActualCategory].SetUp()
		self.__ActualCategory = catID
		self.__CategoryButtons[self.__ActualCategory].Down()
		self.__RefreshSlots()
		
	def __SelectEmptySlot(self, selectedSlotPos):
		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()
			
			item.SelectItem(attachedSlotVnum)
			if self.__IsInTable(attachedSlotVnum):
				chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Ten przedmiot jest ju¿ dodany.")
				mouseModule.mouseController.DeattachObject()
				return
				
			if self.__CanAddItem(attachedSlotVnum):
				self.__PotionsList[self.__ActualCategory][selectedSlotPos] = attachedSlotVnum
				self.__RefreshSlots()
				mouseModule.mouseController.DeattachObject()
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, "[Dopalacze] Nie mo¿esz dodaæ tego przedmiotu.")
				mouseModule.mouseController.DeattachObject()
				return
	
	def __RefreshSlots(self):
		for iSlot, iVnum in enumerate(self.__PotionsList[self.__ActualCategory]):
			if iVnum == 0:
				self.__Slots.ClearSlot(iSlot)
				self.__Slots.RefreshSlot()
			else:
				iCount =  player.GetItemCountByVnum(iVnum)
				self.__Slots.SetItemSlot(iSlot, iVnum, iCount)
				if iCount <= 0:
					self.__Slots.DisableSlot(iSlot)

	def __UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return
			
		self.__PotionsList[self.__ActualCategory][slotIndex] = 0
		self.__RefreshSlots()
		mouseModule.mouseController.DeattachObject()
		self.__OverOutItem()
		
	def __OverInGrid(self, slot):
		if self.__ToolTip:
			self.__ToolTip.SetItemToolTip(self.__PotionsList[self.__ActualCategory][slot])
			
	def __OverOutItem(self):
		if self.__ToolTip:
			self.__ToolTip.Hide()
	
	def SetToolTip(self, tTip):
		self.__ToolTip = proxy(tTip)
	
	def OnPressEscapeKey(self):
		self.Hide()
		return True
		
	def Update(self):
		self.__RefreshSlots()
		
	def Destroy(self):
		self.__Serialize()
		if self.__ToolTip:
			self.__ToolTip = None
		self.__PotionsList = None
		self.__CategoryButtons = None
		self.__Slots = None
		self.__ActiveButton = None
		# self.__RemoveButton = None
		self.Hide()
		
	def Toggle(self):
		if self.IsShow():
			self.Hide()
		else:
			self.Show()