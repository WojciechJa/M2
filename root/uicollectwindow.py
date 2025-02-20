#-*- coding: iso-8859-1 -*-
import uiToolTip
import time
import event
import ui
import constInfo
import app
# import item
import renderTarget
import chat
import player
import nonplayer
import net
import emoji

ITEM_LIST_BY_CAT = {
	0 : { 0 : 71036, 1: 72348 },
	1 : { 0 : 71037, 1: 72348 },
}

BLOCKED_CAT_LIST = [2, 3, 4]
RENDER_INDEX = 10
CAT_LIST = ["Badania Biologa", "Kolekcjoner", "Polowanie", "Bossologia", "Metinologia"]
LOCATION_BY_MOBID = {
	0:"0",
	101:"Miasta Pierwsze",
	502:"Miasta Drugie",
	636:"Dolina Orków",691:"Dolina Orków",
	2074:"Loch Paj¹ków", 2091:"Loch Pajaków", 2034:"Loch Paj¹ków", 2075:"Loch Paj¹ków",
	3190:"Dolina Cyklopów", 3102:"Dolina Cyklopów", 3103:"Dolina Cyklopów", 3104:"Dolina Cyklopów",
	8213:"Pustkowie Faraona", 9673:"Pustkowie Faraona", 9671:"Pustkowie Faraona",
	6775:"MroŸna Kraina", 6781:"MroŸna Kraina", 6779:"MroŸna Kraina",
	6006:"Ognista Ziemia", 34600:"Ognista Ziemia", 6002:"Ognista Ziemia",
	3302:"Zaczarowany Las", 3304:"Zaczarowany Las",
	591:"Miasta Drugie",3191:"Dolina Cyklopów",8214:"Pustkowie Faraona",
	6789:"MroŸna Kraina",34601:"Ognista Ziemia",3391:"Zaczarowany Las",
	8006:"Miasta Drugie",8008:"Dolina Orków",2095:"Loch Paj¹ków",8052:"Dolina Cyklopów",
	8210:"Pustkowie Faraona",6798:"MroŸna Kraina",6801:"Ognista Ziemia",6803:"Zaczarowany Las"
}
BONUS_LIST = {
	0 : { 0 : {
			0:"Silny przeciwko Potworom +10%", 1:"Wartoœæ Ataku +200", 2:"Silny przeciwko W³adcom +5%",
			3:"Obra¿enia Koñcowe +5%",  4:"Obra¿enia Koñcowe +10%", 5:"Silny przeciwko Diab³om +15%", 
			6:"-", 7:"-", 8:"-",
		}, 1 : { 
			0:"-", 1:"-", 2:"-",
			3:"Odpornoœæ na Obra¿enia Um. +5", 4:"Œrednie Obra¿enia +10%", 5:"Silny przeciwko Orkom +15%",
			6:"-", 7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-", 2:"-", 3:"-",
			4:"Obra¿enia Umiejêtnoœci +5%", 5:"Silny przeciwko Ludziom +30%",
			6:"-", 7:"-", 8:"-",
		},
	},
	1 : { 0 : {
			0:"Silny przeciwko Potworom +10%", 1:"Silny przeciwko Potworom +15%", 
			2:"Silny przeciwko Metinom +10%", 3:"Œrednie obra¿enia +15%", 
			4:"Silny przeciwko Potworom +20%", 5:"Silny na Wyprawy +10%",
			6:"Obra¿enia Koñcowe +5%",
			7:"-", 8:"-",
		}, 1 : {
			0:"Silny przeciwko Ludziom +10%", 1:"Silny przeciwko Ludziom +15%",
			2:"-", 3:"-", 4:"Silny przeciwko Ludziom +20%", 5:"-", 
			6:"Obra¿enia umiejêtnoœci +10",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-", 2:"-", 3:"-", 4:"-", 5:"-",
			6:"Silny przeciwko Bossom +10%",
			7:"-", 8:"-",
		},
	},
	2 : { 0 : {
			0:"Silny przeciwko Potworom +1%", 1:"Silny przeciwko Potworom +2%", 
			2:"Silny przeciwko Potworom +3%", 3:"Silny przeciwko Potworom +4%",
			4:"Silny przeciwko Potworom +5%", 5:"Silny przeciwko Potworom +6%",
			6:"Silny przeciwko Potworom +8%", 7:"Silny przeciwko Potworom +10%", 
			8:"-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	},
	3 : { 0 : {
			0:"Silny przeciwko Bossom +1%", 1:"Silny przeciwko Bossom +1%", 
			2:"Silny przeciwko Bossom +1%", 3:"Silny przeciwko Bossom +1%",
			4:"Silny przeciwko Bossom +1%", 5:"Silny przeciwko Bossom +2%",
			6:"Silny przeciwko Bossom +3%", 7:"Silny przeciwko Bossom +4%", 
			8:"-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	},
	4 : { 0 : {
			0:"Silny przeciwko Metinom +1%", 1:"Silny przeciwko Metinom +1%", 
			2:"Silny przeciwko Metinom +1%", 3:"Silny przeciwko Metinom +1%",
			4:"Silny przeciwko Metinom +1%", 5:"Silny przeciwko Metinom +2%",
			6:"Silny przeciwko Metinom +3%", 7:"Silny przeciwko Metinom +4%", 
			8:"-",
		}, 1 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		}, 2 : {
			0:"-", 1:"-",
			2:"-", 3:"-",
			4:"-", 5:"-",
			6:"-",
			7:"-", 8:"-",
		},
	}
}

CATEGORY_BUTTON_NAME = ["Badania Biologa", "Kolekcjoner", "Polowanie", "Bossologia", "Metinologia"]
QUEST_COUNTER = {
	0 : 6,
	1 : 7,
	2 : 8,
	3 : 8,
	4 : 8,
}

class CollectWindow(ui.ScriptWindow):		
	def __init__(self):				
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.tooltipItem = uiToolTip.ItemToolTip()
		self.selectedWindow = 0
		self.realwindow = 0
		self.data = {i : {"ITEM_VNUM" : 0, "TIME" : -1, "COUNT" : 0, "COUNT_TOTAL" : 0, "TAKE_CHANCE" : 0, "RENDERTARGET_VNUM" : 0, "QUEST_INDEX" : 0, "REQUIRED_LEVEL" : 0, } for i in xrange(len(constInfo.CollectWindowQID))}
		self.progress = {i : {"COMPLETED" : 0} for i in xrange(len(constInfo.CollectWindowQID))}
		self.CategoryButtonList = []
		self.bonus_text = {}
		# self.categorytext = {}

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/collectwindow.py")
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.LoadObject")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			for i in xrange(3):
				self.bonus_text[i] = self.GetChild("bonustext_{}".format(i))

			# for i in xrange(5):
			# 	self.categorytext[i] = self.GetChild("categorytext{}".format(i))

			for i in xrange(len(constInfo.CollectWindowQID)):
				self.CategoryButtonList.append(self.GetChild("CategoryButton_%d" % i))
				self.CategoryButtonList[i].SetEvent(ui.__mem_func__(self.__ChangeCategory), i)

			self.header_text_main = self.GetChild("header_text_main")
			self.time_left = self.GetChild("time_left")
			self.item_count = self.GetChild("item_count")
			self.chance_text = self.GetChild("chance_text")
			self.ItemSlot = self.GetChild("item_slot")
			self.ItemSlot.SetOverInItemEvent(ui.__mem_func__(self.__OverInItem))
			self.ItemSlot.SetOverOutItemEvent(ui.__mem_func__(self.__OverOutItem))
			self.blocked_slot = self.GetChild("blocked_slot")

			self.render = self.GetChild("RenderTarget")
			self.location_text = self.GetChild("location_text")
			self.mobname_text = self.GetChild("mobname_text")

			self.collect_button = self.GetChild("collect_button")
			self.collect_button.SetEvent(ui.__mem_func__(self.__ClickCollectButton))
			self.time_button1 = self.GetChild("time_button1")
			self.chance_button = self.GetChild("chance_button")
			self.chance_button.SetEvent(ui.__mem_func__(self.__useitem), 0)
			self.time_button1.SetEvent(ui.__mem_func__(self.__useitem), 1)

			self.PageText = self.GetChild("PageText")
			self.PrevPageBtn = self.GetChild("PrevPageBtn")
			self.NextPageBtn = self.GetChild("NextPageBtn")

			self.PrevPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 0)
			self.NextPageBtn.SetEvent(ui.__mem_func__(self.SendCommand), 1)

			self.progressbar = self.GetChild("progressbar")
			
			self.lowlevel_bg = self.GetChild("lowlevel_bg")
			self.lowlevel_text = self.GetChild("lowlevel_text")
		except:
			import exception
			exception.Abort("CollectWindow.LoadWindow.BindObject")

		self.__ChangeCategory(self.selectedWindow)
		self.SetCenterPosition()

	def Open(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Show(self):
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __ChangeCategory(self, category):
		self.realwindow = category
		self.selectedWindow = category

		for obj in self.CategoryButtonList:
			obj.Enable()
			obj.SetUp()

		if (player.GetLevel() < self.data[self.selectedWindow]["REQUIRED_LEVEL"]):
			self.lowlevel_bg.Show()
			self.lowlevel_text.SetText("Wymagany poziom: "+str(self.data[self.selectedWindow]["REQUIRED_LEVEL"]))
		else:
			self.lowlevel_bg.Hide()

		self.header_text_main.SetText(CAT_LIST[category])
		self.ItemSlot.SetItemSlot(0, self.data[category]["ITEM_VNUM"], 0)
		self.item_count.SetText(str(self.data[category]["COUNT"])+"/"+str(self.data[category]["COUNT_TOTAL"]))

		if category not in BLOCKED_CAT_LIST:
			self.chance_text.Show()
			self.time_left.Show()
			# self.chance_text.SetText(str(self.data[category]["TAKE_CHANCE"]) + "%")
			self.blocked_slot.Hide()

			self.collect_button.Show()
			self.chance_button.Show()
			self.time_button1.Show()
		else:
			self.chance_text.Hide()
			self.blocked_slot.Show()
			self.time_left.Hide()

			self.collect_button.Hide()
			self.chance_button.Hide()
			self.time_button1.Hide()

		renderTarget.SetBackground(RENDER_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
		renderTarget.SetVisibility(RENDER_INDEX, True)

		if self.data[category]["RENDERTARGET_VNUM"] != 0:
			renderTarget.SelectModel(RENDER_INDEX, self.data[category]["RENDERTARGET_VNUM"])

			self.location_text.SetText("|cFFf1e6c0Lokacja: |r"+LOCATION_BY_MOBID[self.data[category]["RENDERTARGET_VNUM"]])
			self.mobname_text.SetText(str(nonplayer.GetMonsterName(self.data[category]["RENDERTARGET_VNUM"])) + " |cFFa7ff33(Lv. "+str(nonplayer.GetMonsterLevel(self.data[category]["RENDERTARGET_VNUM"]))+")")
			for i in xrange(3):
				self.bonus_text[i].SetText(BONUS_LIST[category][i][self.data[category]["QUEST_INDEX"]])
		else:
			renderTarget.SelectModel(RENDER_INDEX, -1)
			
			self.location_text.SetText("|cFFf1e6c0Lokacja: |r-")
			self.mobname_text.SetText("-")
			for i in xrange(3):
				self.bonus_text[i].SetText("-")

		# for i in xrange(5):
		# 	self.categorytext[i].SetText(CATEGORY_BUTTON_NAME[i]+" |cFFfffeab("+str(self.data[i]["QUEST_INDEX"])+" / "+str(QUEST_COUNTER[i])+")")

		self.PageText.SetText(str(self.data[category]["QUEST_INDEX"]))

		self.progressbar.SetPercentage(self.data[category]["COUNT"], self.data[category]["COUNT_TOTAL"])

		self.CategoryButtonList[category].Disable()
		self.CategoryButtonList[category].Down()

	def SendCommand(self, type):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		questid = self.data[self.selectedWindow]["QUEST_INDEX"]
		if self.selectedWindow == 1:
			if type == 0:
				if questid == 0 or questid < 0:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid-1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			elif type == 1:
				if questid < 0 or questid == 6 or questid > 6:
					return
				else:
					net.SendChatPacket("/choose_quest "+str(questid+1)+" "+str(qid))
					self.__ChangeCategory(self.selectedWindow)
			self.PageText.SetText(str(self.data[self.selectedWindow]["QUEST_INDEX"]))
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, "Tej opcji mo¿esz u¿yæ tylko w misjach Kolekcjonera.")

	def	__OverInItem(self, slot_num):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.AddItemData(self.data[self.selectedWindow]["ITEM_VNUM"], 0, 0)
		
	def	__OverOutItem(self):
		if self.tooltipItem:
			self.tooltipItem.ClearToolTip()
			self.tooltipItem.HideToolTip()
		
	def AddData(self, windowType, time, count, itemVnum, countTotal, chance, rendertargetvnum, questindex, requiredLevel):
		self.data[windowType]["ITEM_VNUM"] = itemVnum
		self.data[windowType]["TIME"] = max(0, app.GetTime()+time)
		self.data[windowType]["COUNT"] = count
		self.data[windowType]["COUNT_TOTAL"] = countTotal
		self.data[windowType]["TAKE_CHANCE"] = chance
		self.data[windowType]["RENDERTARGET_VNUM"] = rendertargetvnum
		self.data[windowType]["QUEST_INDEX"] = questindex
		self.data[windowType]["REQUIRED_LEVEL"] = requiredLevel
		self.quest = questindex
		# if windowType != self.realwindow:
		self.__ChangeCategory(self.realwindow)

		#chat.AppendChat(chat.CHAT_TYPE_INFO, str(self.data[windowType]))

	def OnUpdate(self):
		# qid = constInfo.CollectWindowQID[self.selectedWindow]
		# chat.AppendChat(chat.CHAT_TYPE_INFO, str(qid))
		if self.selectedWindow not in BLOCKED_CAT_LIST:
			self.time_left.SetText(time.strftime("%H:%M:%S", time.gmtime(max(float(self.data[self.selectedWindow]["TIME"])-app.GetTime(), 0))))
			self.chance_text.SetText(str(self.data[self.selectedWindow]["TAKE_CHANCE"])+"%")

	def __ClickCollectButton(self):
		qid = constInfo.CollectWindowQID[self.selectedWindow]
		event.QuestButtonClick(qid, 1)

	def __useitem(self, value):
		item_map = {
			0: {1: 71037, 0: 71036},
			1: {1: 72349, 0: 72348}
		}

		item_to_search = item_map.get(value, {}).get(self.selectedWindow)
		if item_to_search is None:
			return

		inventory_ranges = [
			range(player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT),
			range(820, 954)
		]

		for inventory_range in inventory_ranges:
			for i in inventory_range:
				if player.GetItemIndex(i) == item_to_search:
					net.SendItemUsePacket(i)
					return

	def SendTime(self, val, time):
		val_conv = str(val)
		time_conv = float(time)
		if val_conv == str(0):
			self.data[0]["TIME"] = max(0, app.GetTime()+time_conv)
		elif val_conv == str(1):
			self.data[1]["TIME"] = max(0, app.GetTime()+time_conv)

	def SendChance(self, val, chance):
		chat.AppendChat(chat.CHAT_TYPE_INFO, str(val)+","+str(chance))
		chance_conv = str(chance)
		val_conv = str(val)
		if val_conv == str(0):
			self.data[0]["TAKE_CHANCE"] = chance_conv
		elif val_conv == str(1):
			self.data[1]["TAKE_CHANCE"] = chance_conv

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def OnPressExitKey(self):
		self.Close()
		return True