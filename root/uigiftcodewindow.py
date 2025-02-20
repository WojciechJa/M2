import ui
import net
import wndMgr

class UiGiftCodeWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/giftcodewindow.py")

		except:
			import exception
			
			exception.Abort("UiGiftCodeWindow.LoadWindow")

		try:
			self.promo_code = self.GetChild("CodeFrameSlotText")
			
			self.accept_button = self.GetChild("AcceptButton")
			self.cancel_button = self.GetChild("CancelButton")

		except:
			import exception
			
			exception.Abort("UiGiftCodeWindowElements.LoadWindow")
			
		self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.accept_button.SetEvent(ui.__mem_func__(self.ClickAccept))
		self.cancel_button.SetEvent(ui.__mem_func__(self.ClickClear))
		
	def ClickAccept(self):
		promo_code = self.promo_code.GetText()
		
		net.SendChatPacket("/promo_code_system check_code %s" % promo_code)
		self.Close()
		
	def ClickClear(self):
		self.promo_code.SetText("")
		
	def Close(self):
		self.ClickClear()
		
		wndMgr.Hide(self.hWnd)
		
	def OnPressEscapeKey(self):
		self.Close()

	def OnPressExitKey(self):
		self.Close()