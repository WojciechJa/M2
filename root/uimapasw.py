import ui, grp
# import wndMgr
# import player
# import localeinfo
import constInfo
import event
# import uiTip
import net
import time

class MapaSwWindow(ui.ScriptWindow):
	def __init__(self): 
		ui.ScriptWindow.__init__(self) 
		self.__LoadWindow()

		
	def Laduj(self):
		if self.IsShow():
			self.aktualny_czas = 0
			self.przenies = 0
			self.Hide()
		else:
			self.aktualny_czas = 0
			self.przenies = 0
			self.Show()
			self.chui.Show()
			
	def __LoadWindow(self):
		self.ID_MAPY = 0
		self.tekst = 0

		try:
			self.__LoadScript("UIScript/mapasw.py")
			self.chui			= self.GetChild("chui")
			self.chui.SetColor(grp.GenerateColor(0.0,0.0,0.0,140.0/255.0))

			self.shinso			= self.GetChild("shinso")
			self.jinno			= self.GetChild("jinno")
			self.shinso2			= self.GetChild("shinso2")
			self.jinno2			= self.GetChild("jinno2")
			self.dolina			= self.GetChild("orki")
			self.loch			= self.GetChild("loch_paj2")
			self.dolina_cyk	= self.GetChild("dolina_cyklopow")
			self.zakatki = self.GetChild("dalsze_zakatki")
			self.pustkowie = self.GetChild("pustykowie_faraona")
			self.mrozna = self.GetChild("mrozna_kraina")
			self.ognista = self.GetChild("ognista_ziemia")
			self.zaczarowany = self.GetChild("zaczarowany_las")
			self.sohan = self.GetChild("sohan")
			self.las = self.GetChild("lasduchow")

			self.shinso.SetEvent(ui.__mem_func__(self.mapa_shinso))
			self.jinno.SetEvent(ui.__mem_func__(self.mapa_jinno))
			self.shinso2.SetEvent(ui.__mem_func__(self.mapa_shinso2))
			self.jinno2.SetEvent(ui.__mem_func__(self.mapa_jinno2))
			self.dolina.SetEvent(ui.__mem_func__(self.dolinaorkow))
			self.loch.SetEvent(ui.__mem_func__(self.loch_pajakow))
			self.dolina_cyk.SetEvent(ui.__mem_func__(self.dolina_cyklopow))
			self.zakatki.SetEvent(ui.__mem_func__(self.DalszeZakatki))
			self.pustkowie.SetEvent(ui.__mem_func__(self.pustkowiefaraona))
			self.mrozna.SetEvent(ui.__mem_func__(self.mroznakraina))
			self.ognista.SetEvent(ui.__mem_func__(self.ognistaziemia))
			self.zaczarowany.SetEvent(ui.__mem_func__(self.zaczarowanylas))
			self.sohan.SetEvent(ui.__mem_func__(self.sohank))
			self.las.SetEvent(ui.__mem_func__(self.lask))
		except:
			import exception
			exception.Abort("MapaSw.__LoadWindow - b³¹d ³adowania mapy swiata") 

	def OkienkoWiad(self):
		Board = ui.BoardWithTitleBar()
		Board.SetSize(231, 125)
		Board.SetCenterPosition()
		Board.AddFlag('float')
		Board.SetTitleName('Teleporter')
		Board.SetCloseEvent(self.OnCancelWyjscie)
		self.Board = Board
		quest = ui.TextLine()
		quest.SetParent(self.Board)
		quest.SetDefaultFontName()
		quest.SetPosition(0, 43)
		quest.SetHorizontalAlignCenter()
		quest.SetWindowHorizontalAlignCenter()
		quest.SetText(str(self.tekst))
		quest.SetOutline()
		self.quest = quest
		button = ui.Button()
		button.SetParent(self.Board)
		button.SetPosition(0, 64)
		button.SetUpVisual("d:/ymir work/ui/public/XLarge_Button_01.sub")
		button.SetOverVisual("d:/ymir work/ui/public/XLarge_Button_02.sub")
		button.SetDownVisual("d:/ymir work/ui/public/XLarge_Button_03.sub")
		button.SetText("Tak, ide")
		button.SetWindowHorizontalAlignCenter()
		self.button = button
		button.SetEvent(self.OnWyjscieYes)
		button2 = ui.Button()
		button2.SetParent(self.Board)
		button2.SetPosition(0, 64+24)
		button2.SetUpVisual("d:/ymir work/ui/public/XLarge_button_01.sub")
		button2.SetOverVisual("d:/ymir work/ui/public/XLarge_button_02.sub")
		button2.SetDownVisual("d:/ymir work/ui/public/XLarge_button_03.sub")
		button2.SetText("Nie, nie ide")
		button2.SetWindowHorizontalAlignCenter()
		button2.SetEvent(self.OnCancelWyjscie)
		self.button2 = button2
		self.Board.Show()
		self.quest.Show()
		self.button.Show()
		self.button2.Show()

	def OnCancelWyjscie(self):
		self.Board.Hide()
		self.button.Hide()
		self.quest.Hide()
		self.button2.Hide()
		self.AllHide()
		return TRUE

	def mapa_shinso(self):
		self.ID_MAPY = "shinso_m1"
		self.przenies = 1
		
	def mapa_jinno(self):
		self.ID_MAPY = "jinno_m1"
		self.przenies = 1

	def mapa_shinso2(self):
		self.ID_MAPY = "shinso_m2"
		self.przenies = 1
		
	def mapa_jinno2(self):
		self.ID_MAPY = "jinno_m2"
		self.przenies = 1

	def dolinaorkow(self):
		self.ID_MAPY = "dolina_orkow"
		self.przenies = 1

	def loch_pajakow(self):
		self.ID_MAPY = "loch_pajakow"
		self.przenies = 1

	def dolina_cyklopow(self):
		self.ID_MAPY = "dolina_cyklopow"
		self.przenies = 1

	def DalszeZakatki(self):
		self.ID_MAPY = "zakatki"
		self.przenies = 1

	def pustkowiefaraona(self):
		self.ID_MAPY = "pustynia"
		self.przenies = 1

	def mroznakraina(self):
		self.ID_MAPY = "mrozna"
		self.przenies = 1

	def ognistaziemia(self):
		self.ID_MAPY = "ognista"
		self.przenies = 1

	def zaczarowanylas(self):
		self.ID_MAPY = "zaczarowanylas"
		self.przenies = 1

	def sohank(self):
		self.ID_MAPY = "sohan"
		self.przenies = 1

	def lask(self):
		self.ID_MAPY = "las"
		self.przenies = 1

	def OnWyjscieYes(self):
		# self.Board.Hide()
		# self.button.Hide()
		# self.quest.Hide()
		# self.button2.Hide()
		#event.QuestButtonClick(constInfo.id_quest_mapa)
		net.SendQuestInputStringPacket(str(self.ID_MAPY))
		self.aktualny_czas = int(time.strftime("%M%S"))+1
		self.przenies = 1
		self.chui.Hide()
		return TRUE

	
	def AllHide(self):
		self.Laduj()

	def OnUpdate(self):
		if int(time.strftime("%M%S"))  > self.aktualny_czas and self.przenies == 1:
			self.przenies = 0
			event.QuestButtonClick(constInfo.id_quest_mapa)
			net.SendQuestInputStringPacket(str(self.ID_MAPY))
			self.AllHide()
		#pass
	
	def OnPressEscapeKey(self):
		self.Hide()
		return TRUE

	def __LoadScript(self, fileName):
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)

	def __del__(self): 
		ui.ScriptWindow.__del__(self) 
