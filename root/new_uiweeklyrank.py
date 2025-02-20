#-*- coding: iso-8859-1 -*-
import ui
# import app
# import dbg
import chat
import localeInfo
import net
import player
# import wndMgr
import grp
import exception
import playerSettingModule
import emoji
import time
from datetime import datetime, timedelta


FACE_IMAGE_DICT = {
	playerSettingModule.RACE_WARRIOR_M	: "icon/face/warrior_m.tga",
	playerSettingModule.RACE_WARRIOR_W	: "icon/face/warrior_w.tga",
	playerSettingModule.RACE_ASSASSIN_M	: "icon/face/assassin_m.tga",
	playerSettingModule.RACE_ASSASSIN_W	: "icon/face/assassin_w.tga",
	playerSettingModule.RACE_SURA_M		: "icon/face/sura_m.tga",
	playerSettingModule.RACE_SURA_W		: "icon/face/sura_w.tga",
	playerSettingModule.RACE_SHAMAN_M	: "icon/face/shaman_m.tga",
	playerSettingModule.RACE_SHAMAN_W	: "icon/face/shaman_w.tga",
}

#config
TITLE_NAMES = {
	0 : { 0:"£owca III", 1:"£owca II", 2:"£owca I", },
	1 : { 0:"Oprawca III", 1:"Oprawca II", 2:"Oprawca I", },
	2 : { 0:"Niszczyciel III", 1:"Niszczyciel II", 2:"Niszczyciel I", },
	3 : { 0:"Podró¿nik III", 1:"Podró¿nik II", 2:"Podró¿nik I", },
	4 : { 0:"Alchemik III", 1:"Alchemik II", 2:"Alchemik I", },
	5 : { 0:"Szczêœciarz III", 1:"Szczêœciarz II", 2:"Szczêœciarz I", },
	6 : { 0:"Bogacz III", 1:"Bogacz II", 2:"Bogacz I", },
}
TITLE_BONUSES = {
	0 : { 0:"Silny przeciwko Potworom +25%", 1:"Silny przeciwko Potworom +15%", 2:"Silny przeciwko Potworom +10%" },
	1 : { 0:"Silny przeciwko Bossom +20%", 1:"Silny przeciwko Bossom +12%", 2:"Silny przeciwko Bossom +8%", },
	2 : { 0:"Silny przeciwko Metinom +20%", 1:"Silny przeciwko Metinom +12%", 2:"Silny przeciwko Metinom +8%", },
	3 : { 0:"Silny na Wyprawy +20%", 1: "Silny na Wyprawy +12%", 2:"Silny na Wyprawy +8%", },
	4 : { 0:"Szansa na Uszlachetnienie +10%", 1:"Szansa na Uszlachetnienie +5%", 2:"Szansa na Uszlachetnienie +3%", },
	5 : { 0:"Szansa na Ulepszenie +10%", 1:"Szansa na Ulepszenie +5%", 2:"Szansa na Ulepszenie +3%",  },
	6 : { 0:"Presti¿ :)", 1:"Presti¿ :)", 2:"Presti¿ :)", },
}
TITLE_IDS = {
	0 : { 0: 2, 1: 1, 2: 0, },
	1 : { 0: 5, 1: 4, 2: 3, },
	2 : { 0: 8, 1: 7, 2: 6, },
	3 : { 0: 11, 1: 10, 2: 9, },
	4 : { 0: 14, 1: 13, 2: 12, },
	5 : { 0: 17, 1: 16, 2: 15, },
	6 : { 0: 20, 1: 19, 2: 18, },
}
TITLE_BONUS = [
	"Silny przeciwko Potworom +25%","Silny przeciwko Potworom +15%","Silny przeciwko Potworom +10%",
	"Silny przeciwko Bossom +20%","Silny przeciwko Bossom +12%","Silny przeciwko Bossom +8%",
	"Silny przeciwko Metinom +20%","Silny przeciwko Metinom +12%","Silny przeciwko Metinom +8%",
	"Silny na Wyprawy +20%","Silny na Wyprawy +12%","Silny na Wyprawy +8%",
	"Zwiêkszona szansa na Uszlachetnienie +10%","Zwiêkszona szansa na Uszlachetnienie +5%","Zwiêkszona szansa na Uszlachetnienie +3%",
	"Zwiêkszona szansa na Ulepszenie +10%","Zwiêkszona szansa na Ulepszenie +5%","Zwiêkszona szansa na Ulepszenie +3%", 
	"Presti¿ :)", "Presti¿ :)", "Presti¿ :)" 
]
TITLE_NAME = [
	"£owca I", "£owca II", "£owca III", 
	"Oprawca I", "Oprawca II", "Oprawca III", 
	"Niszczyciel I", "Niszczyciel II", "Niszczyciel III", 
	"Podró¿nik I", "Podró¿nik II", "Podró¿nik III", 
	"Alchemik I", "Alchemik II", "Alchemik III", 
	"Szczêœciarz I", "Szczêœciarz II", "Szczêœciarz III", 
	"Bogacz I", "Bogacz II", "Bogacz III"
]
#end

savedData = {
	"elements" : {"info" : {}}
}

def get_week_timestamps():
	now = datetime.now()
	start_of_week = now - timedelta(days=now.weekday())
	start_of_week = start_of_week.replace(hour=0, minute=0, second=0, microsecond=0)
	end_of_week = start_of_week + timedelta(days=6, hours=23, minutes=59, seconds=58)
	start_timestamp = int(time.mktime(start_of_week.timetuple()))
	end_timestamp = int(time.mktime(end_of_week.timetuple()))
	return start_timestamp, end_timestamp

class WeeklyRankWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.number_page = 0
		self.window_page = 0
		self.previous_arg = 0
		self.previous_arg1 = 0
		self.rankDataBackground = None
		self.rankData = None
		self.active_id = -1
		self.select_id = -1
		self.title_text = {}
		self.title_accept_btn = {}
		self.title_select_bonus_btn = {}
		self.category_btn = {}
		self.box = {}
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Destroy(self):
		self.number_page = 0
		self.window_page = 0
		self.previous_arg = 0
		self.previous_arg1 = 0
		self.active_id = -1
		self.select_id = -1
		self.rankDataBackground = None
		self.rankData = None
		self.title_text = {}
		self.title_accept_btn = {}
		self.title_select_bonus_btn = {}
		self.category_btn = {}
		self.box = {}
		self.Hide()
		self.ClearDictionary()

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		self.SelectPage(self.number_page)

	def Open(self):
		self.Show()

	def Close(self):
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/weeklyrank.py")
		except:
			exception.Abort("WeeklyRank_Window.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.titleName = self.GetChild("TitleName")

			self.titleButton = self.GetChild("title_button")
			self.TitleWindow = self.GetChild("title_window")
			self.RankWindow = self.GetChild("rank_window")
			self.info_slot_text1 = self.GetChild("info_slot_text1")
			self.info_slot_text2 = self.GetChild("info_slot_text2")
			self.character_slot = self.GetChild("character_slot_real")
			self.titleaccept_button = self.GetChild("titleaccept_button")
			self.titleaccept_button.SetEvent(ui.__mem_func__(self.DetachTitle))
			self.info_slot_text1.SetText(player.GetName())
			self.info_slot_text2.SetText("Brak Tytu³u")
			self.bonus_text = self.GetChild("bonus_text")
			self.progressbar = self.GetChild("progressbar")
			self.progress_text = self.GetChild("progress_text")

			for i in xrange(3):
				self.title_text[i] = self.GetChild("title{}_text".format(i))
				self.title_accept_btn[i] = self.GetChild("title{}_accept_button".format(i))
				self.title_select_bonus_btn[i] = self.GetChild("title{}_bonus_button".format(i))
				self.title_select_bonus_btn[i].SetEvent(lambda arg=i: self.OnSelectBonusBtn(arg))
				self.title_accept_btn[i].SetEvent(lambda arg=i: self.OnSelectTitle(arg))

			for i in xrange(7):
				self.category_btn[i] = self.GetChild("CategoryButton_{}".format(i))

			for i in xrange(11):
				self.box[i] = self.GetChild("box{}".format(i))

			self.faceImage = ui.ImageBox()
			self.faceImage.SetParent(self.character_slot)
			self.faceImage.SetPosition(4,4)
			race = net.GetMainActorRace()
			faceImageName = FACE_IMAGE_DICT[race]
			self.faceImage.LoadImage(faceImageName)
			self.faceImage.Show()

			if self.window_page == 0:
				self.TitleWindow.Hide()
			else:
				self.TitleWindow.Show()

			self.rankDataBackground = {
								"NAME" : [],
								"KILL" : [],
								"EMPIRE" : [],
								"JOB" : [],
			}
			self.rankData = {
								"NAME" : [],
								"KILL" : [],
								"EMPIRE" : [],
								"JOB" : [],
								
			}
			for i in xrange(11):
				NAME_BACKGROUND = ui.Bar("TOP_MOST")
				NAME_BACKGROUND.SetParent(self.box[i])
				NAME_BACKGROUND.SetPosition(82, 1)
				NAME_BACKGROUND.SetSize(114, 16)
				NAME_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				NAME_BACKGROUND.Show()
				self.rankDataBackground["NAME"].append(NAME_BACKGROUND)
				
				KILL_BACKGROUND = ui.Bar("TOP_MOST")
				KILL_BACKGROUND.SetParent(self.box[i])
				KILL_BACKGROUND.SetPosition(265, 1)
				KILL_BACKGROUND.SetSize(20, 16)
				KILL_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				KILL_BACKGROUND.Show()
				self.rankDataBackground["KILL"].append(KILL_BACKGROUND)

				EMPIRE_BACKGROUND = ui.Bar("TOP_MOST")
				EMPIRE_BACKGROUND.SetParent(self.box[i])
				EMPIRE_BACKGROUND.SetPosition(329, 1)
				EMPIRE_BACKGROUND.SetSize(20, 16)
				EMPIRE_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				EMPIRE_BACKGROUND.Show()
				self.rankDataBackground["EMPIRE"].append(EMPIRE_BACKGROUND)

				JOB_BACKGROUND = ui.Bar("TOP_MOST")
				JOB_BACKGROUND.SetParent(self.box[i])
				JOB_BACKGROUND.SetPosition(367, 1)
				JOB_BACKGROUND.SetSize(20, 16)
				JOB_BACKGROUND.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
				JOB_BACKGROUND.Show()
				self.rankDataBackground["JOB"].append(JOB_BACKGROUND)
				
				NAME = ui.MakeTextLine(self.rankDataBackground["NAME"][i])
				KILL = ui.MakeTextLine(self.rankDataBackground["KILL"][i])
				EMPIRE = ui.MakeTextLine(self.rankDataBackground["EMPIRE"][i])
				JOB = ui.MakeExpandedImageBox(self.rankDataBackground["JOB"][i], 0, 0, 1, 0)
				JOB.SetScale(0.4, 0.4)

				self.rankData["NAME"].append(NAME)
				self.rankData["KILL"].append(KILL)
				self.rankData["EMPIRE"].append(EMPIRE)
				self.rankData["JOB"].append(JOB)

				player_empire = net.GetEmpireID()
				if i == 10:
					self.rankData["NAME"][i].SetText("%s" % (player.GetName()))
					if player_empire == 3:
						self.rankData["EMPIRE"][i].SetText("%s" % emoji.AppendEmoji("icon/emoji/flag_jinno.png"))
					elif player_empire == 1:
						self.rankData["EMPIRE"][i].SetText("%s" % emoji.AppendEmoji("icon/emoji/flag_shinsoo.png"))
					elif player_empire == 0 or player_empire is None:
						self.rankData["EMPIRE"][i].SetText("")

			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
			self.titleButton.SetEvent(ui.__mem_func__(self.OpenTitlePage))
			for i2 in xrange(7):
				self.category_btn[i2].SetEvent(ui.__mem_func__(self.SelectPage), i2)
		except:
			exception.Abort("WeeklyRank_Window.LoadDialog.BindObject")

		self.SetCenterPosition()
		self.SetTop()

	def OnSelectBonusBtn(self, arg):
		for i in xrange(3):
			self.title_select_bonus_btn[i].SetUp()
		self.title_select_bonus_btn[arg].Down()
		self.previous_arg = arg
		self.bonus_text.SetText(TITLE_BONUSES[self.number_page][arg])

	def OnSelectTitle(self, arg):
		for i in xrange(3):
			self.title_accept_btn[i].SetUp()
		self.previous_arg1 = arg
		net.SendSelectTitle(TITLE_IDS[self.number_page][arg])

	def Active(self, id):
		self.active_id = id
		self.info_slot_text2.SetText("%s" % (TITLE_NAME[self.active_id]))

	def Enable(self, id, val):
		savedData["elements"][id] = []
		savedData["elements"][id].append([0, "None"])
		savedData["elements"][id][0][0] = id
		savedData["elements"][id][0][1] = val

	def DetachTitle(self):
		net.SendChatPacket("/detach_title "+str(self.active_id))

	def GetActiveNow(self):
		return self.active_id

	def OnUpdate(self):
		start_timestamp, end_timestamp = get_week_timestamps()
		current_timestamp = int(time.time())
		total_week_time = end_timestamp - start_timestamp
		time_left = end_timestamp - current_timestamp
		if time_left < 0:
			time_left = 0
		self.progressbar.SetPercentage(time_left, total_week_time)

		days = time_left // 86400  # Calculate full days
		hours = (time_left % 86400) // 3600  # Calculate remaining hours
		minutes = (time_left % 3600) // 60  # Calculate remaining minutes

		self.progress_text.SetText("Koniec sezonu: |cFFdeffb3"+str(days)+"d. "+str(hours)+"h. "+str(minutes)+"min. ")

		if not self.active_id == -1:
			if savedData["elements"][self.active_id][0][1] == "Hidden":
				self.info_slot_text2.SetText("Brak tytu³u")

	def PutPage(self, page, season):
		MY_SCORE = [player.WEEKLY1, player.WEEKLY2, player.WEEKLY3, player.WEEKLY4, player.WEEKLY5, player.WEEKLY6, player.WEEKLY7]
		self.rankData["KILL"][10].SetText("%s" % (localeInfo.MoneyFormat(int(player.GetStatus(MY_SCORE[page])))))
		self.number_page = page
		self.category_btn[page].Down()
		self.titleName.SetText("Ranking Tygodniowy (Sezon %d)" % (season+1))

		for i in xrange(3):
			self.title_text[i].SetText(TITLE_NAMES[self.number_page][i])
			self.title_select_bonus_btn[i].SetUp()
			self.title_select_bonus_btn[self.previous_arg].Down()
			self.bonus_text.SetText(TITLE_BONUSES[self.number_page][self.previous_arg])

	def LoadPage(self, pos, name, points, empire, job):
		self.rankData["NAME"][pos].SetText("%s" % (name))
		if empire != 0:
			self.rankData["KILL"][pos].SetText("%s" % (localeInfo.MoneyFormat(points)))

		if empire == 3:
			self.rankData["EMPIRE"][pos].SetText("%s" % emoji.AppendEmoji("icon/emoji/flag_jinno.png"))
		elif empire == 1:
			self.rankData["EMPIRE"][pos].SetText("%s" % emoji.AppendEmoji("icon/emoji/flag_shinsoo.png"))
		elif empire == 0 or empire is None:
			self.rankData["EMPIRE"][pos].SetText("")

		job_key = FACE_IMAGE_DICT[job]
		if empire != 0:
			self.rankData["JOB"][pos].LoadImage(job_key)
			self.rankData["JOB"][pos].SetScale(0.4, 0.4)

		job_key_my_char = FACE_IMAGE_DICT[net.GetMainActorRace()]
		self.rankData["JOB"][10].LoadImage(job_key_my_char)
		self.rankData["JOB"][10].SetScale(0.4, 0.4)


	def SelectPage(self, select):
		net.SelectWeeklyRankPage(select)
		for i in xrange(7):
			self.category_btn[i].SetUp()

	def OpenTitlePage(self):
		if self.window_page == 0:
			self.window_page = 1
			self.RankWindow.Hide()
			self.TitleWindow.Show()
			for i in xrange(3):
				self.title_accept_btn[i].SetUp()
		else:
			self.window_page = 0
			self.TitleWindow.Hide()
			self.RankWindow.Show()
			for i in xrange(11):
				self.rankData["NAME"][i].Show()