#-*- coding: iso-8859-1 -*-
import net
import player
import grp
import localeInfo
import ui
import uiToolTip
import wndMgr
import constInfo
import app

ROOT_PATH = "d:/ymir work/ui/game/ranking/"

class LegendDamageWindow(ui.Window):
	def __init__(self):
		ui.Window.__init__(self)
		self.add_button = []
		self.rankDataBackground = None
		self.rankData = None
		self.number_page = 0
		self.vid = 0
		self.autoRefreshLastTime = app.GetTime() + 1.0
		# self.toolTip = None
		self.BuildWindow()

	def __del__(self):
		ui.Window.__del__(self)

	def Destroy(self):
		self.interface = None
		self.add_button = []
		self.rankDataBackground = None
		self.rankData = None
		self.number_page = 0
		self.vid = 0
		self.autoRefreshLastTime = 0
		# self.toolTip = None

	def BuildWindow(self):
		self.Board = ui.BoardWithTitleBar()
		self.Board.SetSize(332, 468)
		self.Board.SetCenterPosition()
		self.Board.AddFlag('movable')
		self.Board.SetTitleName("Ranking Obra�e� w Legende")
		self.Board.SetCloseEvent(self.Close)

		# self.ask_image = ui.Button()
		# self.ask_image.SetParent(self.Board)
		# self.ask_image.SetPosition(275, 7)
		# self.ask_image.SetUpVisual("d:/ymir work/ui/game/q_mark/q_mark_01.png")
		# self.ask_image.SetOverVisual("d:/ymir work/ui/game/q_mark/q_mark_02.png")
		# self.ask_image.SetDownVisual("d:/ymir work/ui/game/q_mark/q_mark_03.png")
		# # self.ask_image.SetOverInEvent(ui.__mem_func__(self.__OverInButton))
		# # self.ask_image.SetOverOutEvent(ui.__mem_func__(self.__OverOutButton))
		# self.ask_image.Show()

		self.slider_image = ui.ImageBox()
		self.slider_image.SetParent(self.Board)
		self.slider_image.SetPosition(1, 34)
		self.slider_image.LoadImage(ROOT_PATH + "header.sub")
		self.slider_image.Show()

		self.slide_text = ui.TextLine()
		self.slide_text.SetParent(self.slider_image)
		self.slide_text.SetWindowHorizontalAlignCenter()
		self.slide_text.SetHorizontalAlignCenter()
		self.slide_text.SetWindowVerticalAlignCenter()
		self.slide_text.SetVerticalAlignCenter()
		self.slide_text.SetPackedFontColor(0xffe0c198)
		self.slide_text.SetText("Ranking TOP 15")
		self.slide_text.Show()

		self.list_header = ui.ImageBox()
		self.list_header.SetParent(self.Board)
		self.list_header.SetPosition(0, 59)
		self.list_header.LoadImage(ROOT_PATH + "list_header.sub")
		self.list_header.Show()

		self.x_2 = 4
		self.y_2 = 85
		self.rank_slot = []
		self.rankDataBackground = {
			"ID": [],
			"EMPIRE": [],
			"NAME": [],
			"LEVEL": [],
			"DAMAGE": [],
		}
		self.rankData = {
			"ID": [],
			"EMPIRE": [],
			"NAME": [],
			"LEVEL": [],
			"DAMAGE": [],

		}

		for i in xrange(15):
			rank_slots = ui.ImageBox()
			rank_slots.SetParent(self.Board)
			rank_slots.SetPosition(self.x_2, self.y_2)
			rank_slots.LoadImage(ROOT_PATH + str(i + 1) + ".sub")
			self.y_2 += 25

			rank_slots.Hide()
			self.rank_slot.append(rank_slots)
			self.rank_slot[i].Show()
			ID_BACKGROUND = ui.Bar("TOP_MOST")
			ID_BACKGROUND.SetParent(self.rank_slot[i])
			ID_BACKGROUND.SetPosition(5, 2)

			ID_BACKGROUND.SetSize(63, 22)
			ID_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			ID_BACKGROUND.Show()
			self.rankDataBackground["ID"].append(ID_BACKGROUND)

			EMPIRE_BACKGROUND = ui.Bar("TOP_MOST")
			EMPIRE_BACKGROUND.SetParent(self.rank_slot[i])
			EMPIRE_BACKGROUND.SetPosition(72, 6)

			EMPIRE_BACKGROUND.SetSize(20, 10)
			EMPIRE_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			EMPIRE_BACKGROUND.Show()
			self.rankDataBackground["EMPIRE"].append(EMPIRE_BACKGROUND)

			NAME_BACKGROUND = ui.Bar("TOP_MOST")
			NAME_BACKGROUND.SetParent(self.rank_slot[i])
			NAME_BACKGROUND.SetPosition(69, 2)

			NAME_BACKGROUND.SetSize(111, 22)
			NAME_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			NAME_BACKGROUND.Show()
			self.rankDataBackground["NAME"].append(NAME_BACKGROUND)

			LEVEL_BACKGROUND = ui.Bar("TOP_MOST")
			LEVEL_BACKGROUND.SetParent(self.rank_slot[i])
			LEVEL_BACKGROUND.SetPosition(181, 2)

			LEVEL_BACKGROUND.SetSize(61, 22)
			LEVEL_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			LEVEL_BACKGROUND.Show()
			self.rankDataBackground["LEVEL"].append(LEVEL_BACKGROUND)

			# NAME_BACKGROUND = ui.MakeSlotBar(self.rankLine[i], 55, 2, 114, 16)
			ODZNAKA_BACKGROUND = ui.Bar("TOP_MOST")
			ODZNAKA_BACKGROUND.SetParent(self.rank_slot[i])
			ODZNAKA_BACKGROUND.SetPosition(243, 2)

			ODZNAKA_BACKGROUND.SetSize(75, 22)
			ODZNAKA_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
			ODZNAKA_BACKGROUND.Show()
			self.rankDataBackground["DAMAGE"].append(ODZNAKA_BACKGROUND)

			ID = ui.MakeTextLine(self.rankDataBackground["ID"][i])
			EMPIRE = ui.MakeTextLine(self.rankDataBackground["EMPIRE"][i])
			NAME = ui.MakeTextLine(self.rankDataBackground["NAME"][i])
			LEVEL = ui.MakeTextLine(self.rankDataBackground["LEVEL"][i])
			DAMAGE = ui.MakeTextLine(self.rankDataBackground["DAMAGE"][i])
			self.rankData["ID"].append(ID)
			self.rankData["EMPIRE"].append(EMPIRE)
			self.rankData["NAME"].append(NAME)
			self.rankData["LEVEL"].append(LEVEL)
			self.rankData["DAMAGE"].append(DAMAGE)

		# self.toolTip = uiToolTip.ToolTip(350)
		# self.toolTip.Hide()
		# self.tooltipInfo = [self.toolTip] * 8
		# self.InformationText = ["Ranking Tygodniowy resetuje si� w ka�d� niedziel� o godzinie 23:59!",
		# 						"Przy resecie rankingu zostaj� rozdane nagrody dla TOP 3,",
		# 						"s� to specjalne tytu�y, kt�re znajdziecie w magazynie IS!",
		# 						"\n"
		# 						"Ranking TOP 10 w ka�dej kategorii jest od�wie�any co 7 minut...",
		# 						"\n",
		# 						"Zliczanie punkt�w w kategorii Zniszczone Metiny:",
		# 						"maks. r�nica poziomu mi�dzy graczem, a metinem: -30Lv / +30Lv,",
		# 						"minimalny poziom zniszczonego metina: 45Lv.",
		# 						]

		# for i in xrange(8):
		# 	self.tooltipInfo[i].SetFollow(True)
		# 	#			self.tooltipInfo[i].AlignHorizonalCenter()
		# 	self.tooltipInfo[i].AppendTextLine(self.InformationText[i])
		# 	self.tooltipInfo[i].AppendSpace(5)
		# 	self.tooltipInfo[i].Hide()

		self.Board.Hide()

	# def EventProgress(self, event_type):
	# 	if "MOUSE_OVER_IN" == str(event_type):
	# 		self.__OverInButton()
	# 	elif "MOUSE_OVER_OUT" == str(event_type):
	# 		self.__OverOutButton()
	# 	else:
	# 		return

	# def __OverInButton(self):
	# 	for i in xrange(4):
	# 		self.tooltipInfo[i].Show()

	# def __OverOutButton(self):
	# 	for i in xrange(4):
	# 		self.tooltipInfo[i].Hide()

	def OnUpdate(self):
		if app.GetTime() > self.autoRefreshLastTime:
			self.autoRefreshLastTime = app.GetTime() + 1.0

			if self.vid in constInfo.LEGEND_DAMAGE_DATA:
				for pos in range(len(constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"])):
					if constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos] == "":
						self.rankData["EMPIRE"][pos].SetText("")
						self.rankData["NAME"][pos].SetText("-")
						self.rankData["LEVEL"][pos].SetText("-")
						self.rankData["DAMAGE"][pos].SetText("-")
						continue
	
					self.rankData["EMPIRE"][pos].SetText("|Eranking/flag_%d|e" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["EMPIRE"][pos]))
					self.rankData["NAME"][pos].SetText("%s" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos]))
					self.rankData["LEVEL"][pos].SetText("%s" % (constInfo.LEGEND_DAMAGE_DATA[self.vid]["LEVEL"][pos]))
					self.rankData["DAMAGE"][pos].SetText("%s" % (localeInfo.NumberWithDots(constInfo.LEGEND_DAMAGE_DATA[self.vid]["DAMAGE"][pos])))

					if constInfo.LEGEND_DAMAGE_DATA[self.vid]["NAME"][pos] == player.GetName():
						self.rankData["NAME"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["NAME"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
						self.rankData["LEVEL"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["LEVEL"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
						self.rankData["DAMAGE"][pos].SetPackedFontColor(0xffe0c198)
						self.rankData["DAMAGE"][pos].SetFontName(localeInfo.UI_BOLD_FONT)
					else:
						self.rankData["NAME"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["NAME"][pos].SetFontName(localeInfo.UI_DEF_FONT)
						self.rankData["LEVEL"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["LEVEL"][pos].SetFontName(localeInfo.UI_DEF_FONT)
						if pos >= 10:
							self.rankData["DAMAGE"][pos].SetPackedFontColor(0xffed443e)
						else:
							self.rankData["DAMAGE"][pos].SetPackedFontColor(grp.GenerateColor(0.7607, 0.7607, 0.7607, 1.0))
						self.rankData["DAMAGE"][pos].SetFontName(localeInfo.UI_DEF_FONT)
			else:
				for pos in range(14):
					self.rankData["EMPIRE"][pos].SetText("")
					self.rankData["NAME"][pos].SetText("-")
					self.rankData["LEVEL"][pos].SetText("-")
					self.rankData["DAMAGE"][pos].SetText("-")

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def Show(self, vid):
		if self.Board.IsShow():
			self.Close()
		else:
			wndMgr.Show(self.hWnd)
			self.Board.Show()
			self.vid = vid

	def Close(self):
		self.Board.Hide()

	def OnPressEscapeKey(self):
		if self.Board.IsShow():
			self.Close()
			return TRUE
		return FALSE
