import ui
import constInfo

class IsIngameIcon(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/ikonki.py")
		except:
			import exception
			exception.Abort("Ikonki.LoadWindow.LoadObject")
			
		try:
			self.button = self.GetChild("ItemShopButton")
			self.button1 = self.GetChild("BattlepassButton")
			self.button2 = self.GetChild("EventButton")
			self.expander = self.GetChild("expander")
		except:
			import exception
			exception.Abort("Ikonki.LoadWindow.BindObject")

		self.button.SetEvent(ui.__mem_func__(self.__OnClickIs))
		self.button1.SetEvent(ui.__mem_func__(self.__OpenCodeWindow))
		self.button2.SetEvent(ui.__mem_func__(self.interface.OpenEventCalendar))
		self.expander.SetEvent(ui.__mem_func__(self.Expander))

		if constInfo.IkonkiExpander == 0:
			self.button.Hide()
			self.button1.Hide()
			self.button2.Hide()
		else:
			self.button.Show()
			self.button1.Show()
			self.button2.Show()
		
		self.button1.Hide()

	def Expander(self):
		if constInfo.IkonkiExpander == 1:
			self.button.Hide()
			self.button1.Hide()
			self.button2.Hide()
			constInfo.IkonkiExpander = 0
		else:
			self.button.Show()
			self.button1.Show()
			self.button2.Show()
			constInfo.IkonkiExpander = 1

	def __OpenCodeWindow(self):
		self.interface.BattlePassManager()

	def __OnClickIs(self):
		self.interface.RequestOpenItemShop()

	def Open(self, id):
		self.id = id
		self.Show()

	def Close(self):
		self.Hide()

	def Destroy(self):
		self.ClearDictionary()