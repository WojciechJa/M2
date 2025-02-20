import ui
import net
import wndMgr
# import dbg
import app
# import event
import _weakref
# import localeInfo
# import uiScriptLocale

class SelectEmpireWindow(ui.ScriptWindow):
	class EmpireButton(ui.Window):
	
		def __init__(self, owner, arg):
			ui.Window.__init__(self)
			self.owner = owner
			self.arg = arg
			
		def OnMouseOverIn(self):
			self.owner.OnOverInEmpire(self.arg)
			
		def OnMouseOverOut(self):
			self.owner.OnOverOutEmpire(self.arg)
			
		def OnMouseLeftButtonDown(self):
			if self.owner.empireID != self.arg:
				self.owner.OnSelectEmpire(self.arg)

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, self)

		self.stream = stream
		self.empireID = 3
		self.empireArea = {}
		self.empireAreaButton = {}
		self.empireAreaCurAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0, net.EMPIRE_C:0.0 }
		self.empireAreaDestAlpha = { net.EMPIRE_A:0.0, net.EMPIRE_B:0.0, net.EMPIRE_C:0.0 }
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_EMPIRE, 0)

	def Close(self):
		self.ClearDictionary()
		self.selectButton = None
		self.exitButton = None
		self.empireArea = None

		self.KillFocus()
		self.Hide()

		app.HideCursor()

	def Open(self):
		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("SelectEmpireWindow")
		self.Show()	

		self.__LoadScript("arezzo/ui/selectempirewindow.py")
		self.__CreateButtons()
		app.ShowCursor()

		self.OnSelectEmpire(self.empireID)

	def __CreateButtons(self):
		for key, img in self.empireArea.items():
			img.SetAlpha(0.0)
			(x, y) = img.GetGlobalPosition()
			btn = self.EmpireButton(_weakref.proxy(self), key)
			btn.SetParent(self)
			btn.SetPosition(x, y)
			btn.SetSize(img.GetWidth(), img.GetHeight())
			btn.Show()
			self.empireAreaButton[key] = btn
			
	def OnOverInEmpire(self, arg):
		self.empireAreaDestAlpha[arg] = 1.0

	def OnOverOutEmpire(self, arg):
		if arg != self.empireID:
			self.empireAreaDestAlpha[arg] = 0.0

	def OnSelectEmpire(self, arg):
		for key in self.empireArea.keys():
			self.empireAreaDestAlpha[key] = 0.0
		self.empireAreaDestAlpha[arg] = 1.0
		self.empireID = arg

	def __LoadScript(self, fileName):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, fileName)	
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.LoadObject")

		try:
			self.selectButton = self.GetChild("select_button")
			self.exitButton = self.GetChild("exit_button")
			self.empireArea[net.EMPIRE_A] = self.GetChild("area_a")
			# self.empireArea[net.EMPIRE_B] = self.GetChild("area_b")
			self.empireArea[net.EMPIRE_C] = self.GetChild("area_c")
			
		except:
			import exception
			exception.Abort("SelectEmpireWindow.__LoadScript.BindObject")					

		self.selectButton.SetEvent(ui.__mem_func__(self.ClickSelectButton))
		self.exitButton.SetEvent(ui.__mem_func__(self.ClickExitButton))

	def ClickSelectButton(self):
		net.SendSelectEmpirePacket(self.empireID)
		self.stream.SetSelectCharacterPhase()

	def ClickExitButton(self):
		self.stream.SetLoginPhase()	

	def OnUpdate(self):
		self.__UpdateAlpha(self.empireArea, self.empireAreaCurAlpha, self.empireAreaDestAlpha)

	def __UpdateAlpha(self, dict, curAlphaDict, destAlphaDict):
		for key, img in dict.items():

			curAlpha = curAlphaDict[key]
			destAlpha = destAlphaDict[key]

			if abs(destAlpha - curAlpha) / 10 > 0.0001:
				curAlpha += (destAlpha - curAlpha) / 7
			else:
				curAlpha = destAlpha

			curAlphaDict[key] = curAlpha
			img.SetAlpha(curAlpha)

	def OnPressEscapeKey(self):
		self.ClickExitButton()
		return TRUE
		
	def OnPressExitKey(self):
		self.ClickExitButton()
		return True

class ReselectEmpireWindow(SelectEmpireWindow):
	def ClickSelectButton(self):
		net.SendSelectEmpirePacket(self.empireID)
		self.stream.SetCreateCharacterPhase()

	def ClickExitButton(self):
		self.stream.SetSelectCharacterPhase()
