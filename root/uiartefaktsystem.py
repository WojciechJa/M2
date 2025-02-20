import ui, item, uiToolTip, player, constInfo, net, mouseModule, snd

class ArtefaktWindow(ui.ScriptWindow):
	isLoaded = 0
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.__LoadWindow()
	
	def __del__(self):
		ui.ScriptWindow.__del__(self)
	
	def Open(self):
		if self.isLoaded:
			self.RefreshSlot()
			ui.ScriptWindow.Show(self)
		else:
			self.__LoadWindow()
			ui.ScriptWindow.Show(self)
	
	def Close(self):
		ui.ScriptWindow.Hide(self)
	
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/ArtefaktSystemWindow.py")
		except:
			import exception
			exception.Abort("ArtefaktSystemWindow.LoadWindow.LoadObject")

		try:
			self.Board = self.GetChild("Board")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			
			ArtefaktSlots = self.GetChild("ArtefaktSlots")
			ArtefaktSlots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			ArtefaktSlots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			ArtefaktSlots.SetUseSlotEvent(ui.__mem_func__(self.UseItem))
			ArtefaktSlots.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItem))
			ArtefaktSlots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			ArtefaktSlots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.ArtefaktSlots = ArtefaktSlots
			
			self.RefreshSlot()
			self.SetCenterPosition()
			
		except:
			return
			
	def OverInItem(self, slot):
		if self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slot)
			self.tooltipItem.Show()
	
	def SelectEmptySlot(self, slot):
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()

			vnum = player.GetItemIndex(attachedSlotNumber)
			item.SelectItem(vnum)
			
			if item.GetItemType() != item.ITEM_TYPE_ARTEFAKT:
				return
				
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				net.SendItemUsePacket(attachedSlotNumber)
				self.OverOutItem()
				self.RefreshSlot()
				
			mouseModule.mouseController.DeattachObject()
		
		else:
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, slot, player.GetItemIndex(slot), player.GetItemCount(slot))
			snd.PlaySound("sound/ui/pick.wav")
			
	def UseItem(self, slot):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return
		else:
			net.SendItemUsePacket(slot)
			mouseModule.mouseController.DeattachObject()
			self.OverOutItem()
			self.RefreshSlot()
			
	def OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.HideToolTip()
	
	def RefreshSlot(self):
		for i in xrange(item.ARTEFAKT_SLOT_MAX):
			slotNumber = item.EQUIPMENT_ARTEFAKT1+i
			self.ArtefaktSlots.SetItemSlot(slotNumber, player.GetItemIndex(slotNumber), 0)
			self.ArtefaktSlots.RefreshSlot()
			