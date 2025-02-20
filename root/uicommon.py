import ui
import localeInfo
import app
import ime
import uiScriptLocale
if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	import player
	import uiToolTip
import re

class PopupDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.acceptEvent = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/PopupDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.accceptButton = self.GetChild("accept")
			self.accceptButton.SetEvent(ui.__mem_func__(self.Close))

		except:
			import exception
			exception.Abort("PopupDialog.LoadDialog.BindObject")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.Hide()
		self.acceptEvent()

	def Destroy(self):
		self.Close()
		self.ClearDictionary()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SetText(self, text):
		self.message.SetText(text)

	def SetAcceptEvent(self, event):
		self.acceptEvent = event

	def SetButtonName(self, name):
		self.accceptButton.SetText(name)

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnIMEReturn(self):
		self.Close()
		return True

class InputDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialog.py")

		getObject = self.GetChild
		self.board = getObject("Board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputSlot = getObject("InputSlot")
		self.inputValue = getObject("InputValue")

	def Open(self):
		self.inputValue.SetFocus()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputSlot = None
		self.inputValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetNumberMode(self):
		self.inputValue.SetNumberMode()

	def SetSecretMode(self):
		self.inputValue.SetSecret()

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		width = length * 6 + 10
		self.SetBoardWidth(max(width + 50, 160))
		self.SetSlotWidth(width)
		self.inputValue.SetMax(length)

	def SetSlotWidth(self, width):
		self.inputSlot.SetSize(width, self.inputSlot.GetHeight())
		self.inputValue.SetSize(width, self.inputValue.GetHeight())
		if self.IsRTL():
			self.inputValue.SetPosition(self.inputValue.GetWidth(), 0)

	def SetBoardWidth(self, width):
		self.SetSize(max(width + 50, 160), self.GetHeight())
		self.board.SetSize(max(width + 50, 160), self.GetHeight())
		if self.IsRTL():
			self.board.SetPosition(self.board.GetWidth(), 0)
		self.UpdateRect()

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def GetText(self):
		return self.inputValue.GetText()

class InputDialogWithDescription(InputDialog):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		if localeInfo.IsARABIC() :
			pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "inputdialogwithdescription.py")
		else:
			pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description = getObject("Description")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription(self, text):
		self.description.SetText(text)

class InputDialogWithDescription2(InputDialog):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()

	def __del__(self):
		InputDialog.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/inputdialogwithdescription2.py")

		try:
			getObject = self.GetChild
			self.board = getObject("Board")
			self.acceptButton = getObject("AcceptButton")
			self.cancelButton = getObject("CancelButton")
			self.inputSlot = getObject("InputSlot")
			self.inputValue = getObject("InputValue")
			self.description1 = getObject("Description1")
			self.description2 = getObject("Description2")

		except:
			import exception
			exception.Abort("InputDialogWithDescription.LoadBoardDialog.BindObject")

	def SetDescription1(self, text):
		self.description1.SetText(text)

	def SetDescription2(self, text):
		self.description2.SetText(text)

class QuestionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__CreateDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog.py")

		self.board = self.GetChild("board")
		self.textLine = self.GetChild("message")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		self.SetWidth(self.textLine.GetTextWidth()+50)

	def Close(self):
		self.Hide()

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SAFE_SetAcceptEvent(self, event):
		self.acceptButton.SAFE_SetEvent(event)

	def SAFE_SetCancelEvent(self, event):
		self.cancelButton.SAFE_SetEvent(event)

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)

	def SetCancelEvent(self, event):
		self.cancelButton.SetEvent(event)

	def SetText(self, text):
		self.textLine.SetText(text)

	def SetAcceptText(self, text):
		self.acceptButton.SetText(text)

	def SetCancelText(self, text):
		self.cancelButton.SetText(text)

	def OnPressEscapeKey(self):
		self.Close()
		return True

class QuestionDialog2(QuestionDialog):

	def __init__(self):
		QuestionDialog.__init__(self)
		self.__CreateDialog()

	def __del__(self):
		QuestionDialog.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def SetText1(self, text):
		self.textLine1.SetText(text)

	def SetText2(self, text):
		self.textLine2.SetText(text)

	def SetWidth(self, width):
		height = self.GetHeight()
		self.SetSize(width, height)
		self.board.SetSize(width, height)
		self.SetCenterPosition()
		self.UpdateRect()

	def SAFE_SetAcceptEvent(self, event):
		self.acceptButton.SAFE_SetEvent(event)

	def SAFE_SetCancelEvent(self, event):
		self.cancelButton.SAFE_SetEvent(event)

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)

	def SetCancelEvent(self, event):
		self.cancelButton.SetEvent(event)

	def SetAcceptText(self, text):
		self.acceptButton.SetText(text)

	def SetCancelText(self, text):
		self.cancelButton.SetText(text)

	def Open(self):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()
		self.SetWidth(self.textLine1.GetTextWidth()+50)

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return True

class QuestionDialogWithTimeLimit(QuestionDialog2):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()
		self.endTime = 0

	def __del__(self):
		QuestionDialog2.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self, msg, timeout):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		self.SetText1(msg)
		self.endTime = app.GetTime() + timeout

	def OnUpdate(self):
		leftTime = max(0, self.endTime - app.GetTime())
		self.SetText2(localeInfo.UI_LEFT_TIME % (leftTime))

class MoneyInputDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.moneyHeaderText = localeInfo.MONEY_INPUT_DIALOG_SELLPRICE
		self.__CreateDialog()
		self.SetMaxLength(13)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __CreateDialog(self):

		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/moneyinputdialog.py")

		getObject = self.GetChild
		self.board = self.GetChild("board")
		self.acceptButton = getObject("AcceptButton")
		self.cancelButton = getObject("CancelButton")
		self.inputValue = getObject("InputValue")
		self.inputValue.SetNumberMode()
		self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
		self.moneyText = getObject("MoneyValue")
		if app.ENABLE_CHEQUE_SYSTEM:
			self.chequeText = getObject("ChequeValue")
			self.inputChequeValue = getObject("InputValue_Cheque")
			self.inputChequeValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
			self.inputChequeValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickChequeEditLine)
			self.inputValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickValueEditLine)

	def Open(self):
		self.inputValue.SetText("")
		self.inputValue.SetFocus()
		self.__OnValueUpdate()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

	def Close(self):
		self.ClearDictionary()
		self.board = None
		self.acceptButton = None
		self.cancelButton = None
		self.inputValue = None
		if app.ENABLE_CHEQUE_SYSTEM:
			self.inputChequeValue = None
		self.Hide()

	def SetTitle(self, name):
		self.board.SetTitleName(name)

	def SetFocus(self):
		self.inputValue.SetFocus()

	def SetMaxLength(self, length):
		length = min(13, length)
		#length = min(9, length)
		self.inputValue.SetMax(length)

	def SetMoneyHeaderText(self, text):
		self.moneyHeaderText = text

	def SetAcceptEvent(self, event):
		self.acceptButton.SetEvent(event)
		self.inputValue.OnIMEReturn = event

	def SetCancelEvent(self, event):
		self.board.SetCloseEvent(event)
		self.cancelButton.SetEvent(event)
		self.inputValue.OnPressEscapeKey = event

	def SetValue(self, value):
		value=str(value)
		self.inputValue.SetText(value)
		self.__OnValueUpdate()
		ime.SetCursorPosition(len(value))

	def GetText(self):
		return self.inputValue.GetText()

	if app.ENABLE_CHEQUE_SYSTEM:
		def SetCheque(self, cheque):
			cheque=str(cheque)
			self.inputChequeValue.SetText(cheque)
			self.__OnValueUpdate()
			ime.SetCursorPosition(len(cheque)+1)

		def GetTextCheque(self):
			return self.inputChequeValue.GetText()

		def __ClickChequeEditLine(self):
			self.inputChequeValue.SetFocus()
			if len(self.inputValue.GetText()) <= 0:
				self.inputValue.SetText(str(0))

		def __ClickValueEditLine(self):
			self.inputValue.SetFocus()
			if len(self.inputChequeValue.GetText()) <= 0:
				self.inputChequeValue.SetText(str(0))

		def GetCheque(self):
			return self.inputChequeValue.GetText()

		def __OnValueUpdate(self):
			if self.inputValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputValue)
			elif self.inputChequeValue.IsFocus():
				ui.EditLine.OnIMEUpdate(self.inputChequeValue)
			else:
				pass

			text = self.inputValue.GetText()
			cheque_text = self.inputChequeValue.GetText()

			money = 0
			cheque = 0
			GOLD_MAX = 1000000000000000
			CHEQUE_MAX = 1000000000000000

			if text and text.isdigit():
				try:
					money = int(text)
					
					if money >= GOLD_MAX:
						money = GOLD_MAX - 1
						self.inputValue.SetText(str(money))
				except ValueError:
					money = 0

			if cheque_text and cheque_text.isdigit():
				try:
					cheque = int(cheque_text)
					
					if cheque >= CHEQUE_MAX:
						cheque = CHEQUE_MAX - 1
						self.inputValue.SetText(str(cheque))
				except ValueError:
					cheque = 0
			self.chequeText.SetText(localeInfo.NumberToCheque(cheque))
			self.moneyText.SetText(localeInfo.NumberToMoneyString(money))
	else:
		def __OnValueUpdate(self):
			ui.EditLine.OnIMEUpdate(self.inputValue)

			text = self.inputValue.GetText()

			money = 0
			if text and text.isdigit():
				try:
					money = int(text)
				except ValueError:
					money = 199999999

			self.moneyText.SetText(self.moneyHeaderText + localeInfo.NumberToMoneyString(money))


class QuestionDialogWithTimeLimit(QuestionDialog2):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.__CreateDialog()
		self.endTime = 0
		self.timeoverMsg = None
		self.isCancelOnTimeover = False

	def __del__(self):
		QuestionDialog2.__del__(self)

	def __CreateDialog(self):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, "uiscript/questiondialog2.py")

		self.board = self.GetChild("board")
		self.textLine1 = self.GetChild("message1")
		self.textLine2 = self.GetChild("message2")
		self.acceptButton = self.GetChild("accept")
		self.cancelButton = self.GetChild("cancel")

	def Open(self, msg, timeout):
		self.SetCenterPosition()
		self.SetTop()
		self.Show()

		self.SetText1(msg)
		self.endTime = app.GetTime() + timeout

	def OnUpdate(self):
		leftTime = max(0, self.endTime - app.GetTime())
		self.SetText2(localeInfo.UI_LEFT_TIME % (leftTime))
		if leftTime<0.5:
			if self.timeoverMsg:
				chat.AppendChat(chat.CHAT_TYPE_INFO, self.timeoverMsg)
			if self.isCancelOnTimeover:
				self.cancelButton.CallEvent()

	def SetTimeOverMsg(self, msg):
		self.timeoverMsg = msg

	def SetCancelOnTimeOver(self):
		self.isCancelOnTimeover = True

if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	class PrivateShopPriceInputDialog(ui.ScriptWindow):

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.toolTip = None
			self.cancelEvent = None
			self.itemVnum = -1
			self.itemCount = 1
			self.inputMarketPrice = False
			self.marketGoldValue = 0
			self.marketChequeValue = 0
			self.__CreateDialog()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def __CreateDialog(self):

			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/privateshoppriceinputdialog.py")

			self.board = self.GetChild("board")
			self.acceptButton = self.GetChild("AcceptButton")
			self.cancelButton = self.GetChild("CancelButton")
			self.inputValue = self.GetChild("InputValue")
			self.inputMoneyText = self.GetChild("InputMoneyText")
			self.marketMoneyText = self.GetChild("MarketMoneyValue")
			self.priceHintButton = self.GetChild("PriceHintButton")
			self.marketPriceButton = self.GetChild("MarketPriceButton")

			self.inputValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)

			self.priceHintButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "PRICE_HINT")
			self.priceHintButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))

			self.marketPriceButton.SetEvent(ui.__mem_func__(self.__OnClickMarketPriceButton))
			self.marketPriceButton.SetShowToolTipEvent(ui.__mem_func__(self.__OnOverInButton), "AUTO_MARKET_PRICE_INPUT")
			self.marketPriceButton.SetHideToolTipEvent(ui.__mem_func__(self.__OnOverOutButton))
			
			if app.ENABLE_CHEQUE_SYSTEM:
				self.inputChequeText = self.GetChild("InputChequeText")
				self.marketChequeText = self.GetChild("MarketChequeValue")
				self.inputChequeValue = self.GetChild("InputValue_Cheque")

				self.inputChequeValue.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
				self.inputChequeValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickChequeEditLine)
				self.inputChequeValue.SetTabEvent(self.inputValue.SetFocus)
				self.inputValue.OnMouseLeftButtonDown = ui.__mem_func__(self.__ClickValueEditLine)
				self.inputValue.SetTabEvent(self.inputChequeValue.SetFocus)

			self.toolTip = uiToolTip.ToolTip()
			self.toolTip.HideToolTip()

		def Open(self):
			self.inputChequeText.SetText(localeInfo.NumberToMoneyStringNoUnit(int(0)) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON)
			self.inputValue.SetFocus()
			self.__OnValueUpdate()
			self.SetCenterPosition()
			self.SetTop()
			self.Show()

		def Clear(self):
			self.itemVnum = -1
			self.itemCount = 1
			self.marketGoldValue = 0
			self.marketChequeValue = 0

			self.inputValue.SetText("")
			if app.ENABLE_CHEQUE_SYSTEM:
				self.inputChequeValue.SetText("")

		def Close(self):
			self.Clear()
			self.cancelEvent()
			self.Hide()

		def SetItemVnum(self, vnum):
			self.itemVnum = vnum

		def GetItemVnum(self):
			return self.itemVnum
			
		def SetItemCount(self, count):
			self.itemCount = count

		def GetItemCount(self):
			return self.itemCount

		def SetTitle(self, name):
			self.board.SetTitleName(name)

		def SetFocus(self):
			self.inputValue.SetFocus()

		def KillFocus(self):
			if self.inputValue.IsFocus():
				self.inputValue.KillFocus()
			
			if app.ENABLE_CHEQUE_SYSTEM and self.inputChequeValue.IsFocus():
				self.inputChequeValue.KillFocus()

		def SetAcceptEvent(self, event):
			self.acceptButton.SetEvent(event)
			self.inputValue.SetReturnEvent(event)

			if app.ENABLE_CHEQUE_SYSTEM:
				self.inputChequeValue.SetReturnEvent(event)

		def SetCancelEvent(self, event):
			self.cancelEvent = event

			self.board.SetCloseEvent(self.Close)
			self.cancelButton.SetEvent(self.Close)
			self.inputValue.SetEscapeEvent(self.Close)

			if app.ENABLE_CHEQUE_SYSTEM:
				self.inputChequeValue.SetEscapeEvent(self.Close)

		def SetValue(self, value):
			value=str(value)
			self.inputValue.SetText(value)
			self.__OnValueUpdate()
			ime.SetCursorPosition(len(value)+1)		

		def GetText(self):
			if len(self.inputValue.GetText()) <= 0:
				return "0"

			return self.inputValue.GetText()
			
		def SetMarketValue(self, gold, cheque):
			self.marketGoldValue = gold * self.itemCount
			self.marketChequeValue = cheque * self.itemCount

			if gold:
				self.marketMoneyText.SetText(localeInfo.NumberToMoneyString(gold))
				
				if self.inputMarketPrice:
					self.InputMarketPrice()
			else:
				self.marketMoneyText.SetText(localeInfo.PREMIUM_PRIVATE_SHOP_MARKET_PRICE_NOT_AVAILABLE)
				
			if app.ENABLE_CHEQUE_SYSTEM:
				if not gold and not cheque:
					self.marketChequeText.Hide()
				else:
					self.marketChequeText.SetText(localeInfo.NumberToMoneyStringNoUnit(cheque) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON)
					self.marketChequeText.Show()

					if self.inputMarketPrice:
						self.InputMarketPrice()

		if app.ENABLE_CHEQUE_SYSTEM:
			def SetCheque(self, cheque):
				cheque=str(cheque)
				self.inputChequeValue.SetText(cheque)
				self.inputChequeText.SetText(localeInfo.NumberToMoneyStringNoUnit(int(cheque)) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON)
				self.__OnValueUpdate()
				ime.SetCursorPosition(len(cheque)+1)
			
			def __ClickChequeEditLine(self) :
				self.inputChequeValue.SetFocus()
				if len(self.inputChequeValue.GetText()) <= 0:
					self.inputChequeValue.SetText("")

			def __ClickValueEditLine(self) :
				self.inputValue.SetFocus()
				if len(self.inputValue.GetText()) <= 0:
					self.inputValue.SetText("")
							
			def GetCheque(self):
				if len(self.inputChequeValue.GetText()) <= 0:
					return "0"

				return self.inputChequeValue.GetText()
				
			def __OnValueUpdate(self):
				if self.inputValue.IsFocus() :
					ui.EditLine.OnIMEUpdate(self.inputValue)

					money = self.inputValue.GetText()
					money = money.upper()
					if len(money) <= 0:
						money = "0"
					else:
						k_pos = money.find('K')
						if k_pos >= 0:
							money = re.sub(r'[^0-9K]', '', money)
							money = money[:k_pos] + '000' * money.count('K')
						else:
							money = re.sub(r'\D', '', money)

					self.inputMoneyText.SetText(localeInfo.NumberToMoneyString(int(money)))

				elif self.inputChequeValue.IsFocus() :
					ui.EditLine.OnIMEUpdate(self.inputChequeValue)

					cheque = self.inputChequeValue.GetText()
					if len(cheque) <= 0:
						cheque = "0"

					self.inputChequeText.SetText(localeInfo.NumberToMoneyStringNoUnit(int(cheque)) + " " + localeInfo.CHEQUE_SYSTEM_UNIT_WON)
				else:
					pass
	
		else:
			def __OnValueUpdate(self):
				ui.EditLine.OnIMEUpdate(self.inputValue)
				
				money = self.inputValue.GetText()
				if len(money) <= 0:
					money = "0"
				else:
					k_pos = money.find('K')
					if k_pos >= 0:
						money = money[:k_pos] + '000' * money.count('K')

				self.inputMoneyText.SetText(localeInfo.NumberToMoneyString(int(money)))

		def __OnClickMarketPriceButton(self):
			if self.inputMarketPrice:
				self.inputMarketPrice = False

				self.marketPriceButton.SetUpVisual("d:/ymir work/ui/game/premium_private_shop/mini_empty_button_default.sub")
				self.marketPriceButton.SetOverVisual("d:/ymir work/ui/game/premium_private_shop/mini_empty_button_over.sub")
				self.marketPriceButton.SetDownVisual("d:/ymir work/ui/game/premium_private_shop/mini_empty_button_down.sub")
			else:
				self.inputMarketPrice = True

				self.marketPriceButton.SetUpVisual("d:/ymir work/ui/game/premium_private_shop/mini_accept_button_default.sub")
				self.marketPriceButton.SetOverVisual("d:/ymir work/ui/game/premium_private_shop/mini_accept_button_over.sub")
				self.marketPriceButton.SetDownVisual("d:/ymir work/ui/game/premium_private_shop/mini_accept_button_down.sub")

				self.InputMarketPrice()

		def InputMarketPrice(self):
			if self.marketGoldValue > 0:
				self.SetValue(str(self.marketGoldValue))

			if app.ENABLE_CHEQUE_SYSTEM:
				if self.marketChequeValue > 0:
					self.SetCheque(str(self.marketChequeValue))

		def __OnOverInButton(self, button):
			self.toolTip.ClearToolTip()

			if button == "PRICE_HINT":
				self.toolTip.SetThinBoardSize(len(localeInfo.PREMIUM_PRIVATE_SHOP_MARKET_PRICE_HINT_MSG) * 4 + 50, 10)
				self.toolTip.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SHOP_MARKET_PRICE_HINT_MSG, self.toolTip.SPECIAL_TITLE_COLOR)

			elif button == "AUTO_MARKET_PRICE_INPUT":
				self.toolTip.SetThinBoardSize(len(localeInfo.PREMIUM_PRIVATE_SHOP_AUTO_MARKET_PRICE_INPUT_TOOLTIP) * 4 + 50, 10)
				self.toolTip.AppendTextLine(localeInfo.PREMIUM_PRIVATE_SHOP_AUTO_MARKET_PRICE_INPUT_TOOLTIP, self.toolTip.SPECIAL_TITLE_COLOR)

			self.toolTip.ShowToolTip()
				
			
		def __OnOverOutButton(self):
			if 0 != self.toolTip:
				self.toolTip.HideToolTip()

		def OnPressEscapeKey(self):
			self.Close()
			return True