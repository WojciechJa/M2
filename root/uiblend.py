#-*- coding: iso-8859-1 -*-
import ui
import dbg
import app
import chat
import player
import os
import net
from _weakref import proxy
import uiToolTip
import localeInfo
import uiOfflineShopBuilder
import uiOfflineShop
import exchange

slot_x_Count = 8
slot_y_Count = 3
checkd_slots_count = slot_x_Count * slot_y_Count

class AutoDopalacze(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.enableDop = {}
		self.itemsID = {}
		self.tooltip = uiToolTip.ItemToolTip()
		self.tooltip.Hide()
		self.itemInSlotID = [80400, 80401, 80402, 80403, 80404, 80405, 80406, 80407, 80408, 50835, 50837, 50838, 50839, 50840, 71016]
		self.checkedItems = [0] * checkd_slots_count
	
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Open(self):
		self.Show()

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def Close(self):
		self.Hide()

	def OnPressExitKey(self):
		self.Hide()
		return True

	def OnPressEscapeKey(self):
		self.Hide()
		return True

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/dopalaczewindow.py")
		except:
			import exception
			exception.Abort("DopalaczeWindow.__LoadWindow.LoadObject")
		try:
			self.board = self.GetChild("board")
			self.wndItem = self.GetChild("item_slot")
			self.useButton = self.GetChild("use_button")
			self.clearButton = self.GetChild("clear_button")
			self.titleBar = self.GetChild("TitleBar")
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.useButton.SetEvent(ui.__mem_func__(self.btnUse))
			self.clearButton.SetEvent(ui.__mem_func__(self.btnClear))

			for i in range(len(self.itemInSlotID)):
				self.wndItem.SetItemSlot(i, self.itemInSlotID[i], 0)

				slot_counts = [slot_x_Count] * slot_y_Count
				y_positions = [40, 72, 102]

				for j, slot_count in enumerate(slot_counts):
					for i in range(slot_count):
						index = i + sum(slot_counts[:j])
						y_position = y_positions[j]
						event = lambda argSelf=proxy(self), argIndex=index: apply(argSelf.OnEnableGeneral, (argIndex,))
						self.enableDop[index] = CheckBox(self.board, 22 + i * 32, y_position, event)
			
			for i in range(len(self.itemInSlotID)):
				self.enableDop[i].OnMouseOverIn = lambda arg=i: self.OverInItem(arg)
				self.enableDop[i].OnMouseOverOut = self.OverOutItem

			if not os.path.exists('_cfg'):
				os.makedirs('_cfg')
			dopPath = '_cfg\dopalacze_%s' % (player.GetName())
			if os.path.exists(dopPath):
				file=open(dopPath, "r")
				lines=file.readlines()
				for i in range(len(lines)):
					self.checkedItems[i] = str(lines[i]).split(':')[1]
					self.enableDop[i].SetCheck(int(self.checkedItems[i]))
				file.close()
		except:
			exception.Abort("DopalaczeWindow.__LoadWindow.BindObject")

		self.SetCenterPosition()
		self.SetTop()

	def OnEnableGeneral(self, arg):
		if arg >= len(self.itemInSlotID):
			return

		if self.enableDop[arg].IsChecked():
			self.enableDop[arg].SetCheck(FALSE)
			self.checkedItems[arg] = 0
		else:
			self.enableDop[arg].SetCheck(TRUE)
			self.checkedItems[arg] = 1

		file=open('_cfg\dopalacze_%s'%(player.GetName()), "w")
		for i in range(len(self.itemInSlotID)):
			file.write("%d:%d\n"%(i,int(self.checkedItems[i])))
		file.close()

	def btnUse(self):
		toUseItemsID = []
		toUseItemsSlots = []
		for i in range(len(self.itemInSlotID)):
			if int(self.checkedItems[i]) == 1:
				toUseItemsID.append(self.itemInSlotID[i])
				itemIndex = player.GetItemFirstIndexByVnum(int(self.itemInSlotID[i]))
				if itemIndex != -1:
					toUseItemsSlots.append(itemIndex)

		if len(toUseItemsID) == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie zaznaczy³eœ ¿adnego dopalacza!")
			return

		if len(toUseItemsSlots) == 0:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie masz ¿adnego z wybranych dopalaczy w ekwipunku!")
			return

		if exchange.isTrading():
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return
					
			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return
			

		for i in range(len(toUseItemsSlots)):
			net.SendItemUsePacket(int(toUseItemsSlots[i]))

	def btnClear(self):
		file=open('_cfg\dopalacze_%s'%(player.GetName()), "w")
		for i in range(len(self.itemInSlotID)):
			self.checkedItems[i] = 0
			file.write("%d:0\n"%(i))
			self.enableDop[i].SetCheck(FALSE)
		file.close()

	def OverInItem(self, overSlotPos):
		if self.tooltip:
			self.tooltip.SetItemToolTip(self.itemInSlotID[overSlotPos])
			self.tooltip.Show()

	def OverOutItem(self):
		if self.tooltip:
			self.tooltip.HideToolTip()

class CheckBox(ui.ImageBox):
	def __init__(self, parent, x, y, event, filename = "d:/ymir work/ui/dop_off.png"):
		ui.ImageBox.__init__(self)
		self.SetParent(parent)
		self.SetPosition(x, y)
		self.LoadImage(filename)

		image = ui.MakeImageBox(self, "d:/ymir work/ui/dop_on.png", 0, 0)
		image.AddFlag("not_pick")
		image.SetWindowHorizontalAlignCenter()
		image.SetWindowVerticalAlignCenter()
		image.Hide()
		self.Enable = TRUE
		self.image = image
		self.event = event
		self.Show()

	def __del__(self):
		ui.ImageBox.__del__(self)

	def SetCheck(self, flag):
		if flag:
			self.image.Show()
		else:
			self.image.Hide()

	def IsChecked(self):
		if self.image.IsShow():
			return TRUE
		return FALSE

	def Disable(self):
		self.Enable = FALSE

	def OnMouseLeftButtonDown(self):
		if not self.Enable:
			return

	def OnMouseLeftButtonUp(self):
		if not self.Enable:
			return
		self.event()