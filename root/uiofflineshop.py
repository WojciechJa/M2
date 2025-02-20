#-*- coding: iso-8859-1 -*-
import net
import player
import item
import snd
import shop
import wndMgr
import app
import chat
import chr
import background
import ui
import uiCommon
# import chrmgr
import uiToolTip
import mouseModule
import localeInfo
import constInfo
import interfaceModule
import emoji
# import uiMessenger

savedData = {
	"elements" : {"info" : {}}
}

g_isEditingOfflineShop = False

def IsEditingOfflineShop():
	global g_isEditingOfflineShop
	if (g_isEditingOfflineShop):
		return True
	else:
		return False

###################################################################################################
## Offline Shop Admin Panel
class OfflineShopAdminPanelWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.wndOfflineShopAddItem = None
		self.wndOfflineShopRemoveItem = None
		self.wndOfflineShopMyBank = None
		self.wndOfflineShopLogs = None
		self.closeQuestionDialog = None
		self.LoadWindow()
		self.LoadOtherWindows()
		
	def __del__(self):	
		ui.ScriptWindow.__del__(self)
		
	def Show(self):
		self.SetCenterPosition()
		self.SetTop()
		self.LoadWindow()
			
		ui.ScriptWindow.Show(self)
		
	def LoadWindow(self):
		if (self.isLoaded == 1):
			return
			
		self.isLoaded = 1
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "offline/adminpanel.py")
		except:
			import exception
			exception.Abort("OfflineShopAdminPanelWindow.LoadWindow.LoadObject")
			
		try:
			self.board = self.GetChild("Board")
			self.findShopButton = self.GetChild("FindShopButton")
			self.ShopInfoText = self.GetChild("ShopInfoText")
			self.ShopTimer = self.GetChild("ShopTimer")
		except:
			import exception
			exception.Abort("OfflineShopAdminPanelWindow.LoadWindow.BindObject")

		gc = self.GetChild

		self.button = {
			1:gc("StartShopButton"),2:gc("CloseShopButton"),
			3:gc("FindShopButton"),4:gc("ReceiveMoneyShopButton"),
			5:gc("SendItemShopButton"),6:gc("RemoveItemShopButton"),
			7:gc("LogsButton")
		}
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.button[1].SetEvent(ui.__mem_func__(self.ClickOpenOfflineShopButton))
		self.button[2].SetEvent(ui.__mem_func__(self.ClickCloseOfflineShopButton))
		self.button[5].SetEvent(ui.__mem_func__(self.ClickAddItemButton))
		self.button[6].SetEvent(ui.__mem_func__(self.ClickRemoveItemButton))
		self.button[4].SetEvent(ui.__mem_func__(self.ClickMyBankButton))
		
		if app.ENABLE_VS_SHOP_SEARCH:
			self.button[3].SetEvent(ui.__mem_func__(self.WyszukiwaraOpen))

		self.button[7].SetEvent(ui.__mem_func__(self.ClickLogsButton))

	if app.ENABLE_VS_SHOP_SEARCH:
		def WyszukiwaraOpen(self):
			self.Close()
			self.interface.ToggleOfflineShopSearch()

	def SetAdditionalInfo(self, info):
		info = info.split(":")
		
		if info[0] == '0':
			self.ShopInfoText.SetText("---")
			self.ShopTimer.SetText("---")
		else:
			posX = int(info[1])
			posY = int(info[2])
			
			if posX != 0 and posY != 0:
				(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(posX, posY)
				localeMapName = localeInfo.MINIMAP_ZONE_NAME_DICT.get(mapName, "")

				self.ShopInfoText.SetText("%s: " % localeMapName + "(%d, %d)" % (int(posX - xBase) / 100, int(posY - yBase) / 100) + " - CH%s" % int(info[4]))

			seconds = int(info[0])
			days, remain = divmod(seconds, 3600 * 24)
			hours, remain = divmod(remain, 3600)
			minutes, remain = divmod(remain, 60)
			self.ShopTimer.Show()
			self.ShopTimer.SetText("%d dni, %s godz, %s min." % (days, hours, minutes))

	def LoadOtherWindows(self):		
		wndOfflineShopAddItem = OfflineShopAddItemWindow()
		self.wndOfflineShopAddItem = wndOfflineShopAddItem
		wndOfflineShopRemoveItem = OfflineShopRemoveItemWindow()
		self.wndOfflineShopRemoveItem = wndOfflineShopRemoveItem
		wndOfflineShopMyBank = OfflineShopBankDialog()
		self.wndOfflineShopMyBank = wndOfflineShopMyBank
		wndOfflineShopLogs = OfflineShopLogsWindow()
		self.wndOfflineShopLogs = wndOfflineShopLogs

	def ClickOpenOfflineShopButton(self):
		self.Close()
		net.SendChatPacket("/open_offlineshop")
		return True

	def ClickCloseOfflineShopButton(self):
		self.Close()
		closeQuestionDialog = uiCommon.QuestionDialog()
		closeQuestionDialog.SetText(localeInfo.DO_YOU_WANT_TO_CLOSE_OFFLINE_SHOP)
		closeQuestionDialog.SetAcceptEvent(lambda arg = True: self.AnswerCloseOfflineShop(arg))
		closeQuestionDialog.SetCancelEvent(lambda arg = False: self.AnswerCloseOfflineShop(arg))
		closeQuestionDialog.Open()
		self.closeQuestionDialog = closeQuestionDialog

	def AnswerCloseOfflineShop(self, flag):
		if (flag):
			net.SendDestroyOfflineShop()
			shop.ClearOfflineShopStock()
		else:
			self.Show()
			
		self.closeQuestionDialog = None
		
	def ClickAddItemButton(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Opcja chwilowo niedostepna..")
		return False
		
	def ClickRemoveItemButton(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Opcja chwilowo niedostepna..")
		return False
		
	def ClickMyBankButton(self):
		self.Close()
		self.wndOfflineShopMyBank.Open()
		return True

	def ClickLogsButton(self):
		self.Close()
		net.SendChatPacket("/offline_shop_logs")
		return True

	def BindInterfaceClass(self, interface):
		self.interface = interface
	
	def Destroy(self):
		self.ClearDictionary()
		self.ShopTimer = None
		self.ShopInfoText = None
		self.interface = None
		
	def Close(self):
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
###################################################################################################
## Offline Shop Add Item Window
import uiOfflineShopBuilder

class OfflineShopAddItemWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.LoadWindow()
		self.tooltipItem = None
		self.priceInputBoard = None
		self.title = ""
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Refresh(self):
		net.SendRefreshOfflineShop()	
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			itemCount = shop.GetOfflineShopItemCount(i)
			if (itemCount <= 1):
				itemCount = 0
			
			self.itemSlot.SetItemSlot(i, shop.GetOfflineShopItemID(i))
		
		wndMgr.RefreshSlot(self.itemSlot.GetWindowHandle())
		
	def SetItemData(self, pos, itemID, itemCount, itemPrice, itemPrice2):
		shop.SetOfflineShopItemData(pos, itemID, itemCount, itemPrice, itemPrice2)
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "offline/builder.py")
		except:
			import exception
			exception.Abort("OfflineShopAddItemWindow.LoadWindow.LoadObject")
			
		try:
			self.nameLine = self.GetChild("NameLine")
			self.itemSlot = self.GetChild("ItemSlot")
			self.btnOk = self.GetChild("OkButton")
			self.btnClose = self.GetChild("CloseButton")
			self.board = self.GetChild("Board")
		except:
			import exception
			exception.Abort("OfflineShopAddItemWindow.LoadWindow.BindObject")
			
		self.btnOk.Hide()
		self.btnClose.Hide()
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		self.itemSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		
		# RESIZE OFFLINE SHOP ADD ITEM WINDOW
		self.board.SetSize(406, 400)
		self.SetSize(406, 400)
		# END OF RESIZE OFFLINE SHOP ADD ITEM WINDOW
		
	def Destroy(self):
		self.ClearDictionary()
		
		self.nameLine = None
		self.itemSlot = None
		self.btnOk = None
		self.btnClose = None
		self.board = None
		
	def Open(self, title):
		self.title = title
		
		if (len(title) > 25):
			self.title = title[:22] + "..."
			
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.board.SetTitleName(localeInfo.OFFLINE_SHOP_ADD_ITEM_WINDOW)		
		self.Refresh()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		
		self.nameLine.SetText(title)
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = True
		
	def Close(self):
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = False
		
		self.title = ""
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
	def SelectEmptySlot(self, slotIndex):
		try:
			if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
				return
				
			if (mouseModule.mouseController.isAttached()):
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				mouseModule.mouseController.DeattachObject()
				
				if player.SLOT_TYPE_INVENTORY != attachedSlotType and player.SLOT_TYPE_DRAGON_SOUL_INVENTORY != attachedSlotType and\
					(player.SLOT_TYPE_SKILL_BOOK_INVENTORY != attachedSlotType and\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY != attachedSlotType and\
					player.SLOT_TYPE_STONE_INVENTORY != attachedSlotType and\
					player.SLOT_TYPE_BOX_INVENTORY != attachedSlotType and\
					player.SLOT_TYPE_EFSUN_INVENTORY != attachedSlotType and\
					player.SLOT_TYPE_CICEK_INVENTORY != attachedSlotType and app.ENABLE_SPLIT_INVENTORY_SYSTEM):

					return

					
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				itemVnum = player.GetItemIndex(attachedInvenType, attachedSlotPos)
				item.SelectItem(itemVnum)
				
				if (item.IsAntiFlag(item.ANTIFLAG_GIVE) or item.IsAntiFlag(item.ANTIFLAG_MYSHOP)):
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_CANNOT_SELL_ITEM)
					return
					
				priceInputBoard = uiCommon.MoneyInputDialog()
				priceInputBoard.SetTitle(item.GetItemName())
				priceInputBoard.SetAcceptEvent(lambda arg = itemVnum : self.AcceptInputPrice(arg))
				priceInputBoard.SetCancelEvent(ui.__mem_func__(self.CancelInputPrice))
				priceInputBoard.Open()
				
				if itemVnum in uiOfflineShopBuilder.ITEM_PRICE_LIST_DICT.keys():
					price, price2 = uiOfflineShopBuilder.ITEM_PRICE_LIST_DICT[itemVnum]
					priceInputBoard.SetValue(price)
					if price2 is not None:
						priceInputBoard.SetCheque(price2)
				
				self.priceInputBoard = priceInputBoard
				self.priceInputBoard.bDisplayPos = slotIndex
				self.priceInputBoard.bInventory = attachedInvenType
				self.priceInputBoard.bPos = attachedSlotPos
		except Exception, e:
			import dbg
			dbg.TraceError("Exception : SelectEmptySlot, %s" %(e))
				
	def AcceptInputPrice(self, itemVnum):
		if (not self.priceInputBoard):
			return True
			
		text = self.priceInputBoard.GetText()
		text2 = self.priceInputBoard.inputChequeValue.GetText()
		
		if (not text) or (not text2):
			return True
			
		if (not text.isdigit()) or (not text2.isdigit()):
			return True
			
		if (str(text) <= 0):
			return True
			
		net.SendAddOfflineShopItem(self.priceInputBoard.bInventory, self.priceInputBoard.bPos, self.priceInputBoard.bDisplayPos, str(self.priceInputBoard.GetText()), str(self.priceInputBoard.inputChequeValue.GetText()))
		snd.PlaySound("sound/ui/drop.wav")
		self.Refresh()
		
		uiOfflineShopBuilder.AddPriceToList(itemVnum, text, text2)
		
		self.priceInputBoard = None
		return True
		
	def CancelInputPrice(self):
		self.priceInputBoard = None
		return True
		
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def OverInItem(self, slotIndex):
		if (mouseModule.mouseController.isAttached()):
			return
			
		if (self.tooltipItem != 0):
			self.tooltipItem.SetOfflineShopItem(slotIndex)
			
	def OverOutItem(self):
		if (self.tooltipItem != 0):
			self.tooltipItem.HideToolTip()
			
	def OnUpdate(self):
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			itemCount = shop.GetOfflineShopItemCount(i)
			if (itemCount <= 1):
				itemCount
				
			self.itemSlot.SetItemSlot(i, shop.GetOfflineShopItemID(i))
		
		wndMgr.RefreshSlot(self.itemSlot.GetWindowHandle())		
		
			
###################################################################################################
## Offline Shop Remove Item Window
class OfflineShopRemoveItemWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.LoadWindow()
		self.tooltipItem = None
		self.questionDialog = None
		self.title = ""
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def Refresh(self):
		net.SendRefreshOfflineShop()
		iCount = 0
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			if (shop.GetOfflineShopItemID(i) == -842150451):
				iCount = iCount + 1
				
		if (iCount == shop.OFFLINE_SHOP_SLOT_COUNT):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_IS_EMPTY)
			return
			
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			itemCount = shop.GetOfflineShopItemCount(i)
			if (itemCount <= 1):
				itemCount = 0
					
			self.itemSlot.SetItemSlot(i, shop.GetOfflineShopItemID(i))

		wndMgr.RefreshSlot(self.itemSlot.GetWindowHandle())
		
	def SetItemData(self, pos, itemID, itemCount, itemPrice, itemPrice2):
		shop.SetOfflineShopItemData(pos, itemID, itemCount, itemPrice, itemPrice2)
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "offline/builder.py")
		except:
			import exception
			exception.Abort("OfflineShopRemoveItemWindow.LoadWindow.LoadObject")
			
		try:
			self.nameLine = self.GetChild("NameLine")
			self.itemSlot = self.GetChild("ItemSlot")
			self.btnOk = self.GetChild("OkButton")
			self.btnClose = self.GetChild("CloseButton")
			self.board = self.GetChild("Board")
		except:
			import exception
			exception.Abort("OfflineShopRemoveItemWindow.LoadWindow.BindObject")
			
		self.btnOk.Hide()
		self.btnClose.Hide()
		self.board.SetCloseEvent(ui.__mem_func__(self.Close))
		
		self.itemSlot.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlot.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)
		self.itemSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))				
		
		# RESIZE OFFLINE SHOP REMOVE ITEM WINDOW
		self.board.SetSize(406, 400)
		self.SetSize(406, 400)
		# END OF RESIZE OFFLINE SHOP REMOVE ITEM WINDOW
		
	def Destroy(self):
		self.ClearDictionary()
		
		self.nameLine = None
		self.itemSlot = None
		self.btnOk = None
		self.btnClose = None
		self.board = None
		
	def Open(self, title):
		self.title = title
		
		if (len(title) > 25):
			self.title = title[:22] + "..."
			
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.tooltipItem.Hide()
		self.board.SetTitleName(localeInfo.OFFLINE_SHOP_DELETE_ITEM_WINDOW)		
		self.Refresh()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		
		self.nameLine.SetText(title)
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = True
		
	def Close(self):
		global g_isEditingOfflineShop
		g_isEditingOfflineShop = False
		
		if (self.questionDialog):
			self.questionDialog.Close()
			self.questionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		
		self.title = ""
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
	def UnselectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return
			
		itemIndex = shop.GetOfflineShopItemID(selectedSlotPos)
		item.SelectItem(itemIndex)
		itemName = item.GetItemName()
		
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.DO_YOU_WANT_TO_REMOVE_ITEM % (itemName))
		questionDialog.SetAcceptEvent(lambda arg = True : self.AnswerRemoveItem(arg))
		questionDialog.SetCancelEvent(lambda arg = False : self.AnswerRemoveItem(arg))
		questionDialog.Open()
		questionDialog.pos = selectedSlotPos
		self.questionDialog = questionDialog
		
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		
	def AnswerRemoveItem(self, flag):
		if (flag):
			pos = self.questionDialog.pos
			net.SendRemoveOfflineShopItem(pos)
			
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		self.Refresh()
		
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def OverInItem(self, slotIndex):
		if (mouseModule.mouseController.isAttached()):
			return
			
		if (self.tooltipItem != 0):
			self.tooltipItem.SetOfflineShopItem(slotIndex)
			
	def OverOutItem(self):
		if (self.tooltipItem != 0):
			self.tooltipItem.HideToolTip()
			
	def OnUpdate(self):
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			itemCount = shop.GetOfflineShopItemCount(i)
			if (itemCount <= 1):
				itemCount
				
			self.itemSlot.SetItemSlot(i, shop.GetOfflineShopItemID(i))
		
		wndMgr.RefreshSlot(self.itemSlot.GetWindowHandle())	
		
					
###################################################################################################
## Offline Shop Bank Dialog
class OfflineShopBankDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.updateTime = 0
		self.withdrawMoneyTime = 0
		self.LoadWindow()
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "offline/bankdialog.py")
		except:
			import exception
			exception.Abort("OfflineShopBankDialog.LoadWindow.LoadScript")
			
		try:
			self.Board = self.GetChild("Board")
			self.currentMoneyLine = self.GetChild("CurrentMoneyLine")
			self.currentChequeLine = self.GetChild("CurrentChequeLine")
			self.CurrentMoneyZarobek = self.GetChild("CurrentMoneyZarobek")
			self.CurrentChequeZarobek = self.GetChild("CurrentChequeZarobek")
			self.requiredMoneyLine = self.GetChild("RequiredMoneyLine")
			self.requiredChequeLine = self.GetChild("RequiredChequeLine")
			self.WithdrawMoneyButton = self.GetChild("withdraw_button")
			self.withdrawall_button = self.GetChild("withdrawall_button")
			self.CancelButton = self.GetChild("cancel_button")
		except:
			import exception
			exception.Abort("OfflineShopBankDialog.LoadWindow.BindObject")
			
		self.Board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.CancelButton.SetEvent(ui.__mem_func__(self.Close))
		self.WithdrawMoneyButton.SetEvent(ui.__mem_func__(self.WithdrawMoney))
		self.withdrawall_button.SetEvent(ui.__mem_func__(self.WithdrawAll))

	def Close(self):
		self.currentMoneyLine.SetText("")
		self.CurrentMoneyZarobek.SetText("")
		self.CurrentChequeZarobek.SetText("")
		self.currentChequeLine.SetText("")
		self.requiredMoneyLine.SetText("")
		self.requiredChequeLine.SetText("")
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		
		net.SendRefreshOfflineShopMoney()
		net.SendRefreshOfflineShopChequeMoney()

		self.currentMoneyLine.SetText("|cFFc99f36|H|h"+localeInfo.NumberToMoneyString(player.GetCurrentOfflineShopMoney()))
		self.currentChequeLine.SetText("|cFFb8b8b8|H|h"+localeInfo.NumberToCheque(player.GetCurrentOfflineShopChequeMoney()))	
		
		self.CurrentMoneyZarobek.SetText("|cFFc99f36|H|h"+localeInfo.NumberToMoneyString(player.GetCurrentOfflineShopZarobek()))
		self.CurrentChequeZarobek.SetText("|cFFb8b8b8|H|h"+localeInfo.NumberToCheque(player.GetCurrentOfflineShopZarobekCheque()))	
		
		self.requiredMoneyLine.SetText("0")
		self.requiredChequeLine.SetText("0")
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_BANK_INFO)
		self.Show()
		
	def WithdrawMoney(self):
		try:
			currentMoney = player.GetCurrentOfflineShopMoney()
			currentMoney2 = player.GetCurrentOfflineShopChequeMoney()
			requiredMoney = str(self.requiredMoneyLine.GetText())
			requiredCheque = str(self.requiredChequeLine.GetText())
			
			if (requiredMoney < 0) or (requiredCheque < 0):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_BANK_BAD_VALUE)
				return
				
			if (app.GetTime() < self.withdrawMoneyTime + 30):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_BANK_WAIT)
				return
				
			net.SendOfflineShopWithdrawMoney(requiredMoney)
			net.SendOfflineShopWithdrawCheque(requiredCheque)
			self.withdrawMoneyTime = app.GetTime()
		except ValueError:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_BANK_BAD_VALUE)
		
	def WithdrawAll(self):
		currentMoney = player.GetCurrentOfflineShopMoney()
		currentMoney2 = player.GetCurrentOfflineShopChequeMoney()

		if (app.GetTime() < self.withdrawMoneyTime + 30):
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_BANK_WAIT)
			return

		net.SendOfflineShopWithdrawMoney(str(currentMoney))
		net.SendOfflineShopWithdrawCheque(str(currentMoney2))
		self.withdrawMoneyTime = app.GetTime()
		

	def OnUpdate(self):
		# All time update.
		try:
			if (app.GetTime() < self.updateTime + 5):
				return
			
			self.updateTime = app.GetTime()
			net.SendRefreshOfflineShopMoney()
			net.SendRefreshOfflineShopChequeMoney()

			self.currentMoneyLine.SetText("|cFFc99f36|H|h"+localeInfo.NumberToMoneyString(player.GetCurrentOfflineShopMoney()))
			self.currentChequeLine.SetText("|cFFb8b8b8|H|h"+localeInfo.NumberToCheque(player.GetCurrentOfflineShopChequeMoney()))	
		
			self.CurrentMoneyZarobek.SetText("|cFFc99f36|H|h"+localeInfo.NumberToMoneyString(player.GetCurrentOfflineShopZarobek()))
			self.CurrentChequeZarobek.SetText("|cFFb8b8b8|H|h"+localeInfo.NumberToCheque(player.GetCurrentOfflineShopZarobekCheque()))	
		except:
			pass
		
		

###################################################################################################
## Offline Shop
class OfflineShopDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.tooltipItem = 0
		self.xShopStart = 0
		self.yShopStart = 0
		self.questionDialog = None
		self.popup = None
		self.itemBuyQuestionDialog = None
		self.eventWhisper = None
		self.interface = interfaceModule.Interface()
	
	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Refresh(self):
		for i in xrange(shop.OFFLINE_SHOP_SLOT_COUNT):
			itemCount = shop.GetOfflineShopItemCount(i)
			if (itemCount <= 1):
				itemCount = 0
			
			self.itemSlotWindow.SetItemSlot(i, shop.GetOfflineShopItemID(i), itemCount)
		
		wndMgr.RefreshSlot(self.itemSlotWindow.GetWindowHandle())
		
	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "offline/dialog.py")
		except:
			import exception
			exception.Abort("OfflineShopDialog.LoadDialog.LoadObject")
			
		try:
			self.itemSlotWindow = self.GetChild("ItemSlot")
			self.btnBuy = self.GetChild("BuyButton")
			self.titleBar = self.GetChild("TitleBar")
			self.titleName = self.GetChild("TitleName")
			self.sendpm = self.GetChild("wiadomosc")
			self.statusoff = self.GetChild("status_off")
			self.statuson = self.GetChild("status_on")
		except:	
			import exception
			exception.Abort("OfflineShopDialog.LoadDialog.BindObject")			
			
		self.itemSlotWindow.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
		self.itemSlotWindow.SAFE_SetButtonEvent("LEFT", "EXIST", self.SelectItemSlot)
		self.itemSlotWindow.SAFE_SetButtonEvent("RIGHT", "EXIST", self.UnselectItemSlot)
		
		self.itemSlotWindow.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		self.itemSlotWindow.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
		
		self.btnBuy.SetToggleUpEvent(ui.__mem_func__(self.CancelShopping))
		self.btnBuy.SetToggleDownEvent(ui.__mem_func__(self.OnBuy))

		self.sendpm.SetEvent(ui.__mem_func__(self.WyslijWiadomosc))

		self.statuson.Hide()
		self.statusoff.Hide()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))			
		self.Refresh()

	def WyslijWiadomosc(self):
		self.interface.OpenWhisperDialog(str(chr.GetNameByVID(self.ShopPlayerName)))

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def Destroy(self):
		self.Close()
		self.ClearDictionary()
		
		self.eventWhisper = None

		self.tooltipItem = 0
		self.itemSlotWindow = 0
		self.btnBuy = 0
		self.titleBar = 0
		self.questionDialog = None
		self.popup = None
		self.interface = None
		self.ShopPlayerName = None

	def Open(self, vid):

		shop.Open(False, False, True)

		itemBuyQuestionDialog = uiCommon.QuestionDialog()
		self.itemBuyQuestionDialog = itemBuyQuestionDialog
		if (self.itemBuyQuestionDialog):
			itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		chr.SelectInstance(vid)
		if chr.GetRace() == 30000 or not chr.IsNPC(vid):
			# chr.testSetModulateRenderModeRGB(vid, float(0)/255.0, float(100)/255.0, float(0)/255.0)
			chr.SetInstanceType(chr.INSTANCE_TYPE_OBJECT)
			chr.SetRace(30009)
			chr.SetArmor(0)
			# chr.Refresh()

		self.Refresh()
		self.SetTop()
		if constInfo.OFFLINESHOP_DIALOG == 0:
			self.SetCenterPosition()
		self.Show()
		constInfo.OFFLINESHOP_DIALOG = 1
		self.ShopPlayerName = vid

		# Set Title Name
		self.titleName.SetText("Sklep gracza - "+chr.GetNameByVID(vid))		
		
		(self.xShopStart, self.yShopStart, z) = player.GetMainCharacterPosition()
		
	def Close(self):
		constInfo.OFFLINESHOP_DIALOG = 0
		if (self.itemBuyQuestionDialog):
			self.itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
			
		if (self.questionDialog):	
			self.OnCloseQuestionDialog()

		shop.Close()
		net.SendOfflineShopEndPacket()
		self.CancelShopping()
		self.tooltipItem.HideToolTip()
		self.Hide()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True
		
	def OnBuy(self):
		chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OFFLINE_SHOP_WARNING1)
		constInfo.buy_without_dialog = 1
		app.SetCursor(app.BUY)
		
	def CancelShopping(self):
		self.btnBuy.SetUp()
		constInfo.buy_without_dialog = 0
		app.SetCursor(app.NORMAL)
		
	def OnCloseQuestionDialog(self):
		if (not self.questionDialog):
			return
			
		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
		
	def UnselectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return
			
		if app.__BL_CHEST_DROP_INFO__:
			if app.IsPressed(app.DIK_LCONTROL):
				itemIndex = shop.GetOfflineShopItemID(selectedSlotPos)
				isMain = not app.IsPressed(app.DIK_LSHIFT)
				if item.HasDropInfo(itemIndex, isMain):
					self.interface.OpenChestDropWindow(itemIndex, isMain)
				return

		self.AskBuyItem(selectedSlotPos)
		
	def SelectItemSlot(self, selectedSlotPos):
		if (constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1):
			return
	
		isAttached = mouseModule.mouseController.isAttached()
		if (not isAttached):
			curCursorNum = app.GetCursor()
			if (app.BUY == curCursorNum):
				net.SendOfflineShopBuyPacket(selectedSlotPos)
			else:
				selectedItemID = shop.GetOfflineShopItemID(selectedSlotPos)
				itemCount = shop.GetOfflineShopItemCount(selectedSlotPos)
				
				type = player.SLOT_TYPE_OFFLINE_SHOP
				mouseModule.mouseController.AttachObject(self, type, selectedSlotPos, selectedItemID, itemCount)
				mouseModule.mouseController.SetCallBack("INVENTORY", ui.__mem_func__(self.DropToInventory))
				snd.PlaySound("sound/ui/pick.wav")

	def DropToInventory(self):
		attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
		self.AskBuyItem(attachedSlotPos)

	def AskBuyItem(self, slotPos):
		itemIndex = shop.GetOfflineShopItemID(slotPos)
		itemPrice = shop.GetOfflineShopItemPriceGold(slotPos)
		itemPrice2 = shop.GetOfflineShopItemPriceCheque(slotPos)
		itemCount = shop.GetOfflineShopItemCount(slotPos)

		item.SelectItem(itemIndex)
		itemName = item.GetItemName()	

		itemBuyQuestionDialog = uiCommon.QuestionDialog()
		if itemPrice2 == 0:
			itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToMoneyString(itemPrice)))
		elif itemPrice == 0:
			itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM(itemName, itemCount, localeInfo.NumberToCheque(itemPrice2)))
		else:
			itemBuyQuestionDialog.SetText(localeInfo.DO_YOU_BUY_ITEM_YANG_CHEQUE(itemName, itemCount, localeInfo.NumberToMoneyString(itemPrice), localeInfo.NumberToCheque(itemPrice2)))
		itemBuyQuestionDialog.SetAcceptEvent(lambda arg=True: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.SetCancelEvent(lambda arg=False: self.AnswerBuyItem(arg))
		itemBuyQuestionDialog.Open()
		itemBuyQuestionDialog.pos = slotPos
		self.itemBuyQuestionDialog = itemBuyQuestionDialog
		
		if constInfo.buy_without_dialog == 0:
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)
		else:
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)
			pos = self.itemBuyQuestionDialog.pos
			self.itemBuyQuestionDialog.Close()
			self.itemBuyQuestionDialog = None
			net.SendOfflineShopBuyPacket(pos)
		
	def AnswerBuyItem(self, flag):
		if (flag):
			pos = self.itemBuyQuestionDialog.pos
			net.SendOfflineShopBuyPacket(pos)

		self.itemBuyQuestionDialog.Close()
		self.itemBuyQuestionDialog = None
		
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)		
		
	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem
		
	def OverInItem(self, slotIndex):
		if (mouseModule.mouseController.isAttached()):
			return
			
		if (self.tooltipItem != 0):
			self.tooltipItem.SetOfflineShopItem(slotIndex)
			
	def OverOutItem(self):
		if (self.tooltipItem != 0):
			self.tooltipItem.HideToolTip()
		
	def OnUpdate(self):
		status = player.GetCurrentOfflineShopStatus()
		if status == 1:
			self.statuson.Show()
			self.statusoff.Hide()
		else:
			self.statuson.Hide()
			self.statusoff.Show()

		USE_SHOP_LIMIT_RANGE = 1500
		(x, y, z) = player.GetMainCharacterPosition()
		if abs(x - self.xShopStart) > USE_SHOP_LIMIT_RANGE or abs(y - self.yShopStart) > USE_SHOP_LIMIT_RANGE:
			self.Close()

MAX_LOGS_NUMBER = 20
class OfflineShopLogsWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.interface = interfaceModule.Interface()
		self.itemName = {}
		self.itemCount = {}
		self.price = {}
		self.price2 = {}
		self.date = {}
		self.category = {}
		# self.itemWindow = {}
		self.logs_window = {}
		self.pageindex = 0
		self.toolTipItem = uiToolTip.ItemToolTip()
		self.toolTipItem.HideToolTip()
		self.LoadWindow()
		
	def __del__(self):
		self.itemName = {}
		self.itemCount = {}
		self.price = {}
		self.price2 = {}
		self.date = {}
		self.category = {}
		# self.itemWindow = {}
		self.logs_window = {}
		ui.ScriptWindow.__del__(self)
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "offline/logsdialog.py")
		except:
			import exception
			exception.Abort("OfflineShopLogsDialog.LoadWindow.LoadScript")
			
		gc = self.GetChild

		try:
			self.Board = gc("Board")
			self.page0 = gc("page0_window")
			self.page1 = gc("page1_window")
			self.PageText = gc("PageText")
			self.prevpagebtn = gc("PrevPageBtn")
			self.nextpagebtn = gc("NextPageBtn")
		except:
			import exception
			exception.Abort("OfflineShopLogsDialog.LoadWindow.BindObject")
			
		pos_y = 0
		def_pos_y = 2
		# font_name = "Tahoma:12"
		for i in range(MAX_LOGS_NUMBER):
			self.logs_window[i] = self.GetChild("table%s_window" % str(i))
			pos_y += 21.5
			self.itemName[i] = ui.TextLine()
			self.itemName[i].SetParent(self.logs_window[i])
			self.itemName[i].SetPosition(-249, def_pos_y)
			self.itemName[i].SetWindowHorizontalAlignCenter()
			self.itemName[i].SetHorizontalAlignCenter()
			self.itemName[i].SetOutline(True)
			# self.itemName[i].SetFontName(font_name)
			self.itemName[i].SetText("")
			self.itemName[i].Hide()

			# self.itemWindow[i] = ui.Window()
			# self.itemWindow[i].SetParent(self.itemname_window)
			# self.itemWindow[i].SetPosition(1, def_pos_y+pos_y)
			# self.itemWindow[i].SetSize(200,15)
			# # self.itemWindow[i].LoadImage("d:/ymir work/ui/mount_window/exp_empty.png")
			# # self.itemWindow[i].OnMouseOverIn = lambda arg=i: ui.__mem_func__(self.ShowItemInfo(arg))
			# # self.itemWindow[i].OnMouseOverOut = ui.__mem_func__(self.HideItemInfo)
			# self.itemWindow[i].Hide()
		
			self.itemCount[i] = ui.TextLine()
			self.itemCount[i].SetParent(self.logs_window[i])
			self.itemCount[i].SetPosition(-131, def_pos_y)
			self.itemCount[i].SetWindowHorizontalAlignCenter()
			self.itemCount[i].SetHorizontalAlignCenter()
			# self.itemCount[i].SetFontName(font_name)
			self.itemCount[i].SetOutline(True)
			self.itemCount[i].SetText("")
			self.itemCount[i].Hide()

			self.price[i] = ui.TextLine()
			self.price[i].SetParent(self.logs_window[i])
			self.price[i].SetPosition(-13, def_pos_y)
			self.price[i].SetWindowHorizontalAlignCenter()
			self.price[i].SetHorizontalAlignCenter()
			# self.price[i].SetFontName(font_name)
			self.price[i].SetOutline(True)
			self.price[i].SetText("")
			self.price[i].Hide()

			self.date[i] = ui.TextLine()
			self.date[i].SetParent(self.logs_window[i])
			self.date[i].SetPosition(141, def_pos_y)
			self.date[i].SetWindowHorizontalAlignCenter()
			self.date[i].SetHorizontalAlignCenter()
			# self.date[i].SetFontName(font_name)
			self.date[i].SetOutline(True)
			self.date[i].SetText("")
			self.date[i].Hide()

			self.category[i] = ui.TextLine()
			self.category[i].SetParent(self.logs_window[i])
			self.category[i].SetPosition(270, def_pos_y)
			self.category[i].SetWindowHorizontalAlignCenter()
			self.category[i].SetHorizontalAlignCenter()
			# self.category[i].SetFontName("Tahoma:14b")
			self.category[i].SetOutline(True)
			self.category[i].SetText("")
			self.category[i].Hide()

		self.page0.Show()
		self.page1.Hide()
		self.pageindex = 0

		self.Board.SetCloseEvent(ui.__mem_func__(self.Close))
		self.prevpagebtn.SetEvent(ui.__mem_func__(self.PrevPage))
		self.nextpagebtn.SetEvent(ui.__mem_func__(self.NextPage))

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True
		
	def OnPressExitKey(self):
		self.Close()
		return True

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		self.RefreshData()

	def PrevPage(self):
		self.page0.Show()
		self.page1.Hide()
		self.PageText.SetText("I")
		self.pageindex = 0

	def NextPage(self):
		self.page1.Show()
		self.page0.Hide()
		self.PageText.SetText("II")
		self.pageindex = 1

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def SendLogs(self, id, item, count, price, price2, date, action):
		savedData["elements"][id] = []
		savedData["elements"][id].append([0, 0, 0, 0, "None", "None"])
		savedData["elements"][id][0][0] = item
		savedData["elements"][id][0][1] = count
		savedData["elements"][id][0][2] = price
		savedData["elements"][id][0][3] = price2
		savedData["elements"][id][0][4] = date
		savedData["elements"][id][0][5] = action
		if chr.IsGameMaster(0):
			chat.AppendChat(chat.CHAT_TYPE_INFO, "["+str(id)+"] "+str(savedData["elements"][id]))

	# def ShowItemInfo(self, index):
	# 	# for i in range(10):
	# 	self.toolTipItem.SetItemToolTip(savedData["elements"][index][0][0])
			
	# def HideItemInfo(self):
	# 	self.toolTipItem.HideToolTip()

	def RefreshData(self):
		for i in range(len(savedData["elements"]) - 1):
			all_info = savedData["elements"][i][0]
			info0 = savedData["elements"][i][0][0]
			info1 = savedData["elements"][i][0][1]
			info2 = savedData["elements"][i][0][2]
			info3 = savedData["elements"][i][0][3]
			info4 = savedData["elements"][i][0][4]
			info5 = savedData["elements"][i][0][5]

			item.SelectItem(info0)
			name = item.GetItemName()

			if all_info == "None" or info0 == 0:
				self.itemName[i].SetText("")
				self.itemName[i].Hide()
				self.itemCount[i].SetText("")
				self.itemCount[i].Hide()
				self.price[i].SetText("")
				self.price[i].Hide()
				self.date[i].SetText("")
				self.date[i].Hide()
				self.category[i].SetText("")
				self.category[i].Hide()
				# self.itemWindow[i].Hide()
			else:
				self.itemName[i].Show()
				# self.itemWindow[i].Show()
				self.itemName[i].SetText(str(name))
				self.itemCount[i].Show()
				self.itemCount[i].SetText(str(info1))
				self.price[i].Show()
				if (info3 == 0):
					self.price[i].SetText("|cFFffc700"+localeInfo.NumberToGoldNotText(info2)+" Yang")
				else:
					self.price[i].SetText("|cFFb8b8b8"+localeInfo.NumberToGoldNotText(info3)+" Won |cFF999999/ "+"|cFFffc700"+localeInfo.NumberToGoldNotText(info2)+" Yang")
				self.date[i].Show()
				self.date[i].SetText(str(info4))
				self.category[i].Show()
				if info5 == "BUY":
					self.category[i].SetText(emoji.AppendEmoji("icon/emoji/sell.png")+" - Kupno")
				elif info5 == "SELL":
					self.category[i].SetText(emoji.AppendEmoji("icon/emoji/buy.png")+" - Sprzeda¿")