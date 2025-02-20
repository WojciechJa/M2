import ui
import net
import app
import uiScriptLocale
import uiWarpShower

if app.ENABLE_LOADING_PERFORMANCE:
	class NewLoadingWindow(ui.ScriptWindow):
		def __init__(self, stream):
			ui.ScriptWindow.__init__(self)
			self.stream = stream
			net.SendSelectCharacterPacket(self.stream.GetCharacterSlot())
			self.loadingImage = None

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Open(self):
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "arezzo/ui/loadingwindow.py")
			except:
				import exception
				exception.Abort("LodingWindow.Open - LoadScriptFile Error")

			self.Show()

			chrSlot=self.stream.GetCharacterSlot()
			net.SendSelectCharacterPacket(chrSlot)

			app.SetFrameSkip(0)

		def Close(self):
			app.SetFrameSkip(1)

			self.loadingImage = None

			self.ClearDictionary()
			self.Hide()

		def OnPressEscapeKey(self):
			self.stream.SetLoginPhase()
			return True

class LoadingWindow(ui.ScriptWindow):
	def __init__(self, stream):
		ui.Window.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, self)

		self.stream=stream
		self.update=0
		self.playerX=0
		self.playerY=0

	def __del__(self):
		net.SetPhaseWindow(net.PHASE_WINDOW_LOAD, 0)
		ui.Window.__del__(self)

	def Open(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "arezzo/ui/loadingwindow.py")
		except:
			import exception
			exception.Abort("LodingWindow.Open - LoadScriptFile Error")

		self.Show()

		chrSlot=self.stream.GetCharacterSlot()
		net.SendSelectCharacterPacket(chrSlot)

		app.SetFrameSkip(0)

	def DEBUG_LoadData(self, playerX, playerY):
		self.playerX=playerX
		self.playerY=playerY
		self.__StartGame()

	def LoadData(self, playerX, playerY):
		self.playerX=playerX
		self.playerY=playerY

		self.__StartGame()

	def __StartGame(self):
		app.SetGlobalCenterPosition(self.playerX, self.playerY)

		net.StartGame()
		
	def Close(self):
		app.SetFrameSkip(1)
		self.ClearDictionary()
		self.Hide()

	def OnPressEscapeKey(self):
		app.SetFrameSkip(1)
		self.stream.SetLoginPhase()
		return True

