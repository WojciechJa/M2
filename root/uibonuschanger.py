#-*- coding: iso-8859-1 -*-
import net, ui, uiToolTip, player, item, exception, chat, constInfo

class ChangerWindow(ui.ScriptWindow):
	
	MAX_BONUSES = 5
	
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.ChangerPosition = None
		self.ItemPosition = None
		self.ChangersCount = 0
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def LoadWindow(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/bonuschanger.py")
		except:
			exception.Abort("ChangerWindow.LoadDialog.LoadObject")
		
		try:
			self.TitleBar = self.GetChild("TitleBar")
			self.ItemsSlots = self.GetChild("ItemsSlots")
			self.Bonuses = [self.GetChild("BonusName1"), self.GetChild("BonusName2"), self.GetChild("BonusName3"), self.GetChild("BonusName4"), self.GetChild("BonusName5")]
			self.ChangeBonusButton = self.GetChild("ChangeButton")
			
		except:
			exception.Abort("ChangerWindow.LoadDialog.BindObject")

		self.ChangeBonusButton.SetEvent(ui.__mem_func__(self.ChangeBonus))
		self.TitleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		self.ItemsSlots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.ItemsSlots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.ItemToolTip = uiToolTip.ItemToolTip()
		self.ItemToolTip.Hide()
	
	def OverInItem(self, slot):
		if self.ItemToolTip and self.ItemPosition != None and self.ChangerPosition != None:
			if slot == 0:
				self.ItemToolTip.SetInventoryItem(self.ItemPosition)
				self.ItemToolTip.Show()
				
			elif slot == 1:
				self.ItemToolTip.SetInventoryItem(self.ChangerPosition)
				self.ItemToolTip.Show()
	
	def OverOutItem(self):
		if self.ItemToolTip:
			self.ItemToolTip.Hide()
			
	def AddItems(self, itemPositon, changerPosition):
		if item.ITEM_TYPE_WEAPON == item.GetItemType() or item.ITEM_TYPE_ARMOR == item.GetItemType():
			attrSlot = [player.GetItemAttribute(itemPositon, i) for i in xrange(self.MAX_BONUSES)]
			for i in xrange(self.MAX_BONUSES):
				type = attrSlot[i][0]
				value = attrSlot[i][1]
				affectString = self.__GetBonusName(type, value)
				
				self.Bonuses[i].SetText("") #Clear old bonuses.
				if affectString:
					self.Bonuses[i].SetText(affectString)
		else:
			return
		
		self.ItemsSlots.SetItemSlot(0, player.GetItemIndex(itemPositon), 0)
		self.ItemsSlots.SetItemSlot(1, player.GetItemIndex(changerPosition), player.GetItemCount(changerPosition))
		
		self.ItemPosition = itemPositon
		self.ChangerPosition = changerPosition
		
		self.ChangersCount = player.GetItemCount(self.ChangerPosition)
		self.Open()
	
	def __GetBonusName(self, affectType, affectValue):
		if 0 == affectType:
			return None
		
		if 0 == affectValue:
			return None
			
		try:
			return self.ItemToolTip.AFFECT_DICT[affectType](affectValue)
		
		except TypeError:
			pass
		except KeyError:
			pass
		
			
	def ChangeBonus(self):
		if self.ChangerPosition == None or self.ItemPosition == None:
			return
			
		if self.ChangersCount > 0:
			net.SendItemUseToItemPacket(self.ChangerPosition, self.ItemPosition)
			
			if player.GetItemIndex(self.ChangerPosition) != 0:
				self.ItemsSlots.SetItemSlot(1, player.GetItemIndex(self.ChangerPosition), self.ChangersCount)
				
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie masz wiêcej Zaczarowañ Przedmiotu na tej pozycji.")
	

	def OnUpdate(self):
		if constInfo.IS_BONUS_CHANGER:
			# import app
			# if app.IsPressed(app.DIK_RETURN):
			# 	self.ChangeBonus()
			
			attrSlot = [player.GetItemAttribute(self.ItemPosition, i) for i in xrange(self.MAX_BONUSES)]	
			for i in xrange(self.MAX_BONUSES):
				type = attrSlot[i][0]
				value = attrSlot[i][1]
				affectString = self.__GetBonusName(type, value)
				
				self.Bonuses[i].SetText("") #Clear old bonuses.
				
				if affectString:
					self.Bonuses[i].SetText(affectString)
		else:
			return
		
	def Destroy(self):
		self.Close()
	
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def Close(self):
		if self.IsShow():
			self.ChangerPosition = None
			self.ItemPosition = None
			constInfo.IS_BONUS_CHANGER = False
			self.Hide()
	
	def Open(self):
		constInfo.IS_BONUS_CHANGER = True
		self.Show()
		self.SetCenterPosition()
	