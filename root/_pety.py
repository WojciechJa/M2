#-*- coding: iso-8859-1 -*-
## /\><><><><><><><><><><><><><><><><><><><><><><><><><><><>/\
## \/		|===================================|		  \/
## /\		|	   System: Pety Expione v1	 |		  /\
## \/		|	UdostÄ™pniony przez kamil2321   |		  \/
## /\		|===================================|		  /\
## \/					 GG: 1889862					   \/
## /\			 strona: https://kamil2321.pl			  /\
## \/			 Forum: https://metin2time.eu			  \/
## /\				   Google: kamil2321				   /\
## \/				  YouTube: kamil2321				   \/
## /\		|===================================|		  /\
## \/				  Do uÅ¼ytku wÅ‚asnego!				  \/
## /\	W razie problemÃ³w/ poprawek itp. proszÄ™ pisaÄ‡.	 /\
## \/><><><><><><><><><><><><><><><><><><><><><><><><><><><>\/
	
import app
import dbg
import ui
import constInfo
import event
import uiToolTip 
import exception 
import wndMgr 
import net 
import item 
import player 
import localeInfo 
import chat
import uiScriptLocale
import pack
from uiToolTip import ItemToolTip
AFFECT_DICT = ItemToolTip.AFFECT_DICT

skill_name = (localeInfo.PET_SKILL_1, localeInfo.PET_SKILL_2, localeInfo.PET_SKILL_3, localeInfo.PET_SKILL_4, localeInfo.PET_SKILL_5, localeInfo.PET_SKILL_6, localeInfo.PET_SKILL_7, localeInfo.PET_SKILL_8, localeInfo.PET_SKILL_9, localeInfo.PET_SKILL_10, localeInfo.PET_SKILL_11)

#Max pointÃ³w na jeden skill(czyta teÅ¼ z src):
PET_MAX_SKILL_LV_BY_POINT = 10

# Skill startowy:
PET_SKILL_START = 301
PET_SKILL_END = 312
PET_SKILL_COUNT = PET_SKILL_END-PET_SKILL_START

class Pety(ui.ScriptWindow):
	class TextToolTip(ui.Window):
		def __init__(self, y):
			ui.Window.__init__(self, "TOP_MOST")

			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetHorizontalAlignLeft()
			textLine.SetOutline()
			textLine.Hide()
			self.y = y
			self.textLine = textLine

		def __del__(self):
			ui.Window.__del__(self)

		def SetText(self, text):
			self.textLine.SetText(text)

		def OnRender(self):
			(mouseX, mouseY) = wndMgr.GetMousePosition()
			self.textLine.SetPosition(mouseX - 30, mouseY - 30 + self.y)
			self.textLine.Show()

	def __init__(self, wndInventory = None):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.wndInventory = wndInventory
		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.Hide()
		self.__Load_BindObject()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __Initialize(self):
		# self.titleBar = 0
		self.toolTip = None
		self.toolTipExp = None
		self.gui = {
			"points" : None,
			"name" : None,
			"petname_t" : None,
			"lv" : None,
			"exp" : None,
			"exp_need" : None,
			"skills" : None,
			"islot" : None,
			"eqs" : None,
		}
		
	def Destroy(self):
		self.ClearDictionary()
		self.__Initialize()	
			
	def __Load_BindObject(self):
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/_pety_script.py")
		except KeyError, msg:
			dbg.TraceError("[Pety] Problem z plikiem: _pety_script.py /dodana wartosc jest bledna/ brak pliku.")
			
		self.SetCenterPosition()
	
		try:
			GetObject = self.GetChild
			self.titleBar = self.GetChild("TitleBar")
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			self.extrabonus = GetObject("ExtraBonus")
			t = uiToolTip.ToolTipNew
			self.tt = {
				0:t(330),
			}
			self.tt[0].SetDefaultFontName("Tahoma:14")
			self.tt[0].AppendSpace(3)
			self.tt[0].AppendTextLine("Posiadaj¹c wszystkie umiejêtnoœci na poziomie P")
			self.tt[0].AppendTextLine("otrzymujesz dodatkowy bonus |cFFc8ff9c+30%|r do umiejêtnoœci.")

			self.gui["islot"] = GetObject("islot")

			self.gui["islot"].SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
			self.gui["islot"].SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
			self.gui["islot"].SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			self.gui["islot"].SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			self.gui["islot"].SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
			self.gui["islot"].SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

			
			self.gui["eqs"] = GetObject("eqs")
			
			self.gui["eqs"].SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
			self.gui["eqs"].SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
			self.gui["eqs"].SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			self.gui["eqs"].SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
			self.gui["eqs"].SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
			self.gui["eqs"].SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))
			
			
			self.gui["points"] = GetObject("skill_points")
			self.gui["lv"] = GetObject("mount_level")
			self.exp_hover_info = self.GetChild("exp_hover_info")

			self.exp_gauges = [self.GetChild("exp_gauge_01"), self.GetChild("exp_gauge_02"), self.GetChild("exp_gauge_03"), self.GetChild("exp_gauge_04")]
			for i in xrange(4):
				self.exp_gauges[i].SetSize(0, 0)
				self.exp_gauges[i].Hide()

			self.gui["skills"] = GetObject("skills")
			self.gui["skills"].SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.gui["skills"].SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.gui["skills"].SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			self.gui["skills"].SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
			self.gui["skills"].AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
											"d:/ymir work/ui/game/windows/btn_plus_down.sub")

			self.gui["skills"].SetSelectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			self.gui["skills"].SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
										
			self.PetReload()				

		except KeyError, msg:
			dbg.TraceError("[Pety] Nie mozna odczytac calego gui. Czy dodane zostaly wszystkie zdjecia, .sub, locale_game.txt? Polecam sprawdzic.")
		
	def ClickSkillSlot(self, slotNumber):
		skill_point = constInfo.gui_pets["SKILLS"][slotNumber]["lv"]
		if skill_point > 19:
			if skill_point < 30:
				if self.gui["skills"].IsActivatedSlot(slotNumber):
					self.gui["skills"].DeactivateSlot(slotNumber)
					net.SendPetAction(0)
				else:
					for i in xrange(PET_SKILL_COUNT):
						self.gui["skills"].DeactivateSlot(i)
						net.SendPetAction(0)
							
					self.gui["skills"].ActivateSlot(slotNumber)
					net.SendPetAction(slotNumber+PET_SKILL_START)
					chat.AppendChat(chat.CHAT_TYPE_INFO, "Wybrana umiejêtnoœæ: "+str(skill_name[slotNumber]))

	def OnPressedSlotButton(self, slotNumber):
		net.SendChatPacket("/pet_skillup {}".format(slotNumber  + PET_SKILL_START))
		
	def PetReload(self):
		points = int(constInfo.gui_pets["POINTS"])
		self.gui["skills"].HideAllSlotButton()

		skill_nr = PET_SKILL_START
		for skill_data in constInfo.gui_pets["SKILLS"]:		
			src_skill_nr = skill_nr - PET_SKILL_START
			skill_point = constInfo.gui_pets["SKILLS"][src_skill_nr]["lv"]

			lv = skill_point
			grade = 0	
			
			if (lv >= 10 and lv <= 19):
				lv -= 9
				grade = 1
			elif (lv > 19 and lv <= 29):
				lv -= 19
				grade = 2
			elif (lv > 29):
				grade = 3

			self.gui["skills"].SetSkillSlotNew(src_skill_nr, skill_nr, grade, lv)
			self.gui["skills"].SetSlotCountNew(src_skill_nr, grade, lv)

			if skill_point < PET_MAX_SKILL_LV_BY_POINT and points > 0:
				self.gui["skills"].ShowSlotButton(src_skill_nr)
			
			skill_nr += 1

	if app.ENABLE_EXTRABONUS_SYSTEM:
		def CheckAllSkills(self):
			for i in xrange(PET_SKILL_START,PET_SKILL_END):
				value = (i - PET_SKILL_START)
				level = constInfo.gui_pets["SKILLS"][value]["lv"]
				if level != 30:
					return FALSE

			return TRUE

	def OverInItem(self, slotNumber):
		self.act_slot = slotNumber
		self.toolTip.ClearToolTip()	
		self.toolTip.SetTitle(str(skill_name[slotNumber]))
		try:
			tab = constInfo.gui_pets["SKILLS"][slotNumber]["affect"]
			level = constInfo.gui_pets["SKILLS"][slotNumber]["lv"]
			for t in tab:
				if not len(t):
					continue
				bon_data = t.split(",")
				self.toolTip.AppendSpace(2)
				if str(bon_data[0]) == 153:
					if app.ENABLE_EXTRABONUS_SYSTEM:
						if self.CheckAllSkills():
							self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
							self.toolTip.AppendSpace(5)
							self.toolTip.AppendTextLine("Dodatkowy bonus: |cFFc8ff9c+"+str(((int(bon_data[1])*20)/100)))
							self.toolTip.AppendSpace(2)
						else:
							self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
					else:
						self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
				else:
					if app.ENABLE_EXTRABONUS_SYSTEM:
						if self.CheckAllSkills():
							self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
							self.toolTip.AppendSpace(5)
							self.toolTip.AppendTextLine("Dodatkowy bonus: |cFFc8ff9c+"+str(((int(bon_data[1])*20)/100)))
						else:
							self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
					else:
						self.toolTip.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
		except:
			pass
			
		self.toolTip.Show()

	def SetExp(self, curPoint, maxPoint):
		cur_exp = int(curPoint)
		exp = int(maxPoint)

		if exp > 0:	
			exp_perc =  float(cur_exp) / exp * 100
		else:
			exp_perc =  float(0)

		curPoint = int(max(cur_exp, 0))
		maxPoint = int(max(exp, 0))
		quarterPoint = maxPoint / 4
		FullCount = 0

		if 0 != quarterPoint:
			FullCount = min(4, curPoint / quarterPoint)

		for i in xrange(4):
			self.exp_gauges[i].Hide()

		for i in xrange(FullCount):
			self.exp_gauges[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
			self.exp_gauges[i].Show()

		if 0 != quarterPoint:
			if FullCount < 4:
				Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
				self.exp_gauges[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
				self.exp_gauges[FullCount].Show()

		self.toolTipExp = self.TextToolTip(15)
		self.toolTipExp.SetText(uiScriptLocale.ASLAN_BUFF_HOVER_EXP_INFO % (localeInfo.MoneyFormat(str(cur_exp)), localeInfo.MoneyFormat(str(exp)), exp_perc))
		self.toolTipExp.Hide()

	def OnUpdate(self):
		if self.extrabonus.IsIn():
			self.tt[0].Show()
		else:
			self.tt[0].Hide()

		exp = int(constInfo.gui_pets["EXP"])
		exp_need = int(constInfo.gui_pets["EXP_NEED"])

		if self.toolTip.IsShow():
			wndMgr.RefreshSlot(self.gui["skills"].GetWindowHandle())

		p = int(constInfo.gui_pets["POINTS"])
		if p > 0:
			self.gui["points"].Show()
			self.gui["points"].SetText(str(p))
		else:
			self.gui["points"].SetText(str(p))

		lv = str(constInfo.gui_pets["LEVEL"])
		self.gui["lv"].SetText("Lv. "+lv)
			
		self.SetExp(int(exp), int(exp_need))
		if self.exp_hover_info.IsIn():
			self.toolTipExp.Show()
		else:
			self.toolTipExp.Hide()

		for x in xrange(PET_SKILL_COUNT):
			skill_point = constInfo.gui_pets["SKILLS"][x]["lv"]
			if skill_point > 29:
				if self.gui["skills"].IsActivatedSlot(x):
					self.gui["skills"].DeactivateSlot(x)
					net.SendPetAction(0)

		slotNumber = item.SLOT_ITEM_NEW_PET
		self.gui["islot"].SetItemSlot(slotNumber, player.GetItemIndex(slotNumber), 0)
		#self.gui["islot"].RefreshSlot()
		
		for slotNumber in xrange(item.SLOT_ITEM_NEW_PET_EQ_START, item.SLOT_ITEM_NEW_PET_EQ_END):
			self.gui["eqs"].SetItemSlot(slotNumber, player.GetItemIndex(slotNumber), 0)
			self.gui["eqs"].RefreshSlot()

	def OverOutItem(self):
		if self.toolTip:
			self.toolTip.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
	
	def Show(self):
		ui.ScriptWindow.Show(self)

	def Close(self):
		for i in xrange(PET_SKILL_COUNT):
			self.gui["skills"].DeactivateSlot(i)
			net.SendPetAction(0)

		if self.tt[0]:
			self.tt[0].Hide()

		if self.toolTipExp:
			self.toolTipExp.Hide()
			
		self.Hide()
		self.OverOutItem()
		
