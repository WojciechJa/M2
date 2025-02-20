#-*- coding: iso-8859-1 -*-
import ui
import uiToolTip
import net
import player
import item
import exception
import constInfo
import mouseModule
import snd
import localeInfo
# import chat
import wndMgr
import emoji
import uiScriptLocale
import app
from uiToolTip import ItemToolTip
AFFECT_DICT = ItemToolTip.AFFECT_DICT

#config
skill_name = ("1", "2", "3", "4", "5")
PODKOWA_VNUM = 80069
PODKOWA_PRICE = [5000, 30000]
REQUIRED_ITEMS = {
	0 : 71129,
	1 : 71122
}
REQUIRED_ITEMS_AMOUNT = {
	0 : 1,
	1 : 1
}
#config end

if app.ENABLE_MOUNT_COSTUME_SYSTEM:
	class MountWindow(ui.ScriptWindow):
		class TextToolTip(ui.Window):
			def __init__(self, y):
				ui.Window.__init__(self, "TOP_MOST")

				textLine = ui.TextLine()
				textLine.SetParent(self)
				textLine.SetHorizontalAlignLeft()
				textLine.SetOutline()
				textLine.Show()
				self.y = y
				self.textLine = textLine

			def __del__(self):
				ui.Window.__del__(self)

			def SetText(self, text):
				self.textLine.SetText(text)

			def OnRender(self):
				(mouseX, mouseY) = wndMgr.GetMousePosition()
				self.textLine.SetPosition(mouseX - 30, mouseY - 30 + self.y)

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.isLoaded = 0
			self.level = 0
			self.expInfo = [0, 0]
			self.tooltipItem = uiToolTip.ItemToolTip()
			self.tooltipItem.Hide()
			self.toolTipExp = None
			self.gui = {
				"skills" : None,
			}
			self.__LoadWindow()

		def __del__(self):
			ui.ScriptWindow.__del__(self)

		def Show(self):
			if self.isLoaded:
				self.RefreshSlots()
				ui.ScriptWindow.Show(self)
			else:
				self.__LoadWindow()
				ui.ScriptWindow.Show(self)

		def Open(self):
			self.Show()

		def SetClear(self):
			self.GetChild("mount_level").SetText("")
			self.GetChild("skill_points").SetText("")
			# self.GetChild("HorseShoeName").SetText("")
			# if self.toolTipExp != None:
			# 	self.toolTipExp.SetText("")
			# for slot in xrange(5):
			# 	self.skill_slots.ClearSlot(slot)
			# self.exp_info = [ 0, 0]
			# for i in xrange(4):
			# 	self.exp_gauges[i].Hide()
			# self.skillPoints = 0

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return
				
			self.isLoaded = 1
			try:
				ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/mount_window.py")
			except:
				exception.Abort("MountWindow.LoadDialog.LoadScript")

			try:
				self.titleBar = self.GetChild("TitleBar")
				self.mount_level = self.GetChild("mount_level")
				# self.exp_hover_info = self.GetChild("exp_hover_info")

				# self.skill_slots = self.GetChild("skills")

				mountSlot = self.GetChild("MountSlot")
				mountSlot.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
				mountSlot.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
				mountSlot.SetUseSlotEvent(ui.__mem_func__(self.UseItem))
				mountSlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItem))
				mountSlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
				mountSlot.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectEmptySlot))

				self.mountSlot = mountSlot

				# self.HorseShoeName = self.GetChild("HorseShoeName")

				# horseshoe = self.GetChild("HorseshoeSlot")
				# horseshoe.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
				# horseshoe.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
				# horseshoe.SetUseSlotEvent(ui.__mem_func__(self.UseItem))
				# horseshoe.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItem))
				# horseshoe.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
				# horseshoe.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectEmptySlot))

				# self.horseshoe = horseshoe

				self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

				# self.exp_gauges = [self.GetChild("exp_gauge_01"), self.GetChild("exp_gauge_02"), self.GetChild("exp_gauge_03"), self.GetChild("exp_gauge_04")]
				# for i in xrange(4):
				# 	self.exp_gauges[i].SetSize(0, 0)
				# 	self.exp_gauges[i].Hide()

					# self.gui["skills"] = self.GetChild("skills")
					# self.gui["skills"].SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
					# self.gui["skills"].SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot))
					# self.gui["skills"].SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))
					# self.gui["skills"].SetPressedSlotButtonEvent(ui.__mem_func__(self.OnPressedSlotButton))
					# self.gui["skills"].AppendSlotButton("d:/ymir work/ui/game/windows/btn_plus_up.sub",\
					# 								"d:/ymir work/ui/game/windows/btn_plus_over.sub",\
					# 								"d:/ymir work/ui/game/windows/btn_plus_down.sub")
					# self.gui["skills"].SetSelectItemSlotEvent(ui.__mem_func__(self.OverOutSkillSlot))
					# self.gui["skills"].SetUnselectItemSlotEvent(ui.__mem_func__(self.OverOutSkillSlot))	
			except:
				exception.Abort("MountWindow.LoadDialog.BindObject")

			self.RefreshSlots()
			self.SetCenterPosition()
			self.SetTop()

		# def OnPressedSlotButton(self, skillslot):
		# 	if self.skillPoints != 0:
		# 		net.SendMountNPCAction(1, skillslot)
				# self.SetSkillSlots()

		def SelectEmptySlot(self, slot):
			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotNumber = mouseModule.mouseController.GetAttachedSlotNumber()

				vnum = player.GetItemIndex(attachedSlotNumber)
				item.SelectItem(vnum)
				
				if item.GetItemType() != item.COSTUME_SLOT_MOUNT:
					return
					
				if player.SLOT_TYPE_INVENTORY == attachedSlotType:
					net.SendItemUsePacket(attachedSlotNumber)
					self.OverOutItem()
					self.RefreshSlots()
					
				mouseModule.mouseController.DeattachObject()
			
			else:
				mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, slot, player.GetItemIndex(slot), player.GetItemCount(slot))
				snd.PlaySound("sound/ui/pick.wav")
				

		def UseItem(self, slot):
			if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
				return
			else:
				net.SendItemUsePacket(slot)
				mouseModule.mouseController.DeattachObject()
				self.OverOutItem()
				self.RefreshSlots()

		def OverInItem(self, slot):
			if self.tooltipItem:
				self.tooltipItem.SetInventoryItem(slot)
				self.tooltipItem.Show()

		def OverOutItem(self):
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def Destroy(self):
			self.Hide()
			self.ClearDictionary()

		def Close(self):
			self.Hide()

		# def SetEXPInfo(self, level, cur_exp, exp):
		# 	self.level = str(level)
		# 	self.expInfo = [cur_exp, exp]
		# 	self.mount_level.SetText("Lv. "+str(level))
			
		# 	cur_exp = int(cur_exp)
		# 	exp = int(exp)
		# 	self.exp_info = [ cur_exp, exp]
			
		# 	if exp > 0:	
		# 		exp_perc =  float(cur_exp) / exp * 100
		# 	else:
		# 		exp_perc =  float(0)

		# 	curPoint = int(max(cur_exp, 0))
		# 	maxPoint = int(max(exp, 0))
		# 	quarterPoint = maxPoint / 4
		# 	FullCount = 0

		# 	if 0 != quarterPoint:
		# 		FullCount = min(4, curPoint / quarterPoint)

		# 	for i in xrange(4):
		# 		self.exp_gauges[i].Hide()

		# 	for i in xrange(FullCount):
		# 		self.exp_gauges[i].SetRenderingRect(0.0, 0.0, 0.0, 0.0)
		# 		self.exp_gauges[i].Show()

		# 	if 0 != quarterPoint:
		# 		if FullCount < 4:
		# 			Percentage = float(curPoint % quarterPoint) / quarterPoint - 1.0
		# 			self.exp_gauges[FullCount].SetRenderingRect(0.0, Percentage, 0.0, 0.0)
		# 			self.exp_gauges[FullCount].Show()
					
		# 	self.toolTipExp = self.TextToolTip(15)
		# 	self.toolTipExp.SetText(uiScriptLocale.ASLAN_BUFF_HOVER_EXP_INFO % (localeInfo.MoneyFormat(str(cur_exp)), localeInfo.MoneyFormat(str(exp)), exp_perc))
		# 	self.toolTipExp.Hide()

		# def SetSkillInfo(self, skillpoints, skilldata):
		# 	self.skillPoints = skillpoints
		# 	self.GetChild("skill_points").SetText(str(self.skillPoints))

		# 	skill_data = skilldata.split("#")
		# 	constInfo.gui_mount["SKILLS"] = []
		# 	for skill in skill_data:
		# 		if not len(skill):
		# 			continue
			
		# 		tmp = {}
		# 		t = skill.split("&")
		# 		if len(t) < 2:
		# 			continue
					
		# 		tmp["lv"] = int(t[0])

		# 		tmp["affect"] = []
		# 		for s in t[1].split("@"):
		# 			tmp["affect"].append(s)
				
		# 		constInfo.gui_mount["SKILLS"].append(tmp)

		# 	# for skill_data in constInfo.gui_mount["SKILLS"]:
		# 	# 	skill_point = constInfo.gui_mount["SKILLS"][2]["affect"]
		# 	# 	chat.AppendChat(chat.CHAT_TYPE_INFO, str(skill_point))

		# 	self.gui["skills"].HideAllSlotButton()
		# 	skill_nr = 295
		# 	for skill_data in constInfo.gui_mount["SKILLS"]:		
		# 		src_skill_nr = skill_nr - 295
		# 		skill_point = constInfo.gui_mount["SKILLS"][src_skill_nr]["lv"]

		# 		lv = skill_point
		# 		grade = 0	
				
		# 		if (lv >= 11 and lv <= 19):
		# 			lv -= 9
		# 			grade = 1
		# 		elif (lv > 19 and lv <= 29):
		# 			lv -= 19
		# 			grade = 2
		# 		elif (lv > 29):
		# 			grade = 3

		# 		self.gui["skills"].SetSkillSlotNew(src_skill_nr, skill_nr, grade, lv)
		# 		self.gui["skills"].SetSlotCountNew(src_skill_nr, grade, lv)

		# 		if skill_point < 10 and skillpoints > 0:
		# 			self.gui["skills"].ShowSlotButton(src_skill_nr)
				
		# 		skill_nr += 1

		# def OverOutSkillSlot(self):
		# 	if self.toolTipSkill:
		# 		self.toolTipSkill.Hide()

		# def OverInSkillSlot(self, skillslot):
		# 	self.toolTipSkill.ClearToolTip()
		# 	self.toolTipSkill.SetTitle(str("Umiejêtnoœæ: "+skill_name[skillslot]))
		# 	try:
		# 		tab = constInfo.gui_mount["SKILLS"][skillslot]["affect"]
		# 		skilllevel = constInfo.gui_mount["SKILLS"][skillslot]["lv"]
		# 		for t in tab:
		# 			if not len(t):
		# 				continue
		# 			bon_data = t.split(",")
		# 			self.toolTipSkill.AppendSpace(5)
		# 			self.toolTipSkill.AppendTextLine("Aktualny poziom:")
		# 			self.toolTipSkill.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1]))), 0xff89b88d)
		# 			self.toolTipSkill.AppendSpace(5)
		# 			if skilllevel < 10:
		# 				self.toolTipSkill.AppendTextLine("Nastêpny poziom: ", 0xffc97171)
		# 				self.toolTipSkill.AppendTextLine(str(AFFECT_DICT[int(bon_data[0])](int(bon_data[1])+1)), 0xffc97171)
		# 			else:
		# 				self.toolTipSkill.AppendTextLine("Maksymalny poziom", 0xff8fbd79)
		# 	except:
		# 		pass
				
		# 	self.toolTipSkill.Show()
			
		# def GetSkillGrade(self, skillLevel):
		# 	if skillLevel >= 0 and skillLevel < 20:
		# 		return 0
		# 	if skillLevel >= 20 and skillLevel < 30:
		# 		return 1
		# 	if skillLevel >= 30 and skillLevel < 40: 
		# 		return 2
		# 	if skillLevel == 40: 
		# 		return 3

		# def GetSkillLevel(self, skillGrade, skillLevel):
		# 	if skillGrade == 0:
		# 		return skillLevel
		# 	if skillGrade == 1:
		# 		return skillLevel - 19
		# 	if skillGrade == 2:
		# 		return skillLevel - 29
		# 	if skillGrade == 3:
		# 		return 40

		# def SetSkillToolTip(self, toolTipSkill):
		# 	self.toolTipSkill = toolTipSkill

		# def OnUpdate(self):
		# 	if self.toolTipExp != None:
		# 		if self.exp_hover_info.IsIn():
		# 			self.toolTipExp.Show()
		# 		else:
		# 			self.toolTipExp.Hide()

		def OnPressEscapeKey(self):
			self.Close()
			return TRUE

		def RefreshSlots(self):
			slotNumber = item.COSTUME_SLOT_MOUNT
			slotNumber2 = item.EQUIPMENT_MOUNT

			# self.horseshoe.SetItemSlot(slotNumber2, player.GetItemIndex(slotNumber2), 0)
			# self.horseshoe.RefreshSlot()

			self.mountSlot.SetItemSlot(slotNumber, player.GetItemIndex(slotNumber), 0)
			self.mountSlot.RefreshSlot()

			vnum = player.GetItemIndex(slotNumber2)
			item.SelectItem(vnum)
			# self.HorseShoeName.SetText(item.GetItemName())

	# class HorseShoeWindow(ui.ScriptWindow):
	# 	def __init__(self):
	# 		ui.ScriptWindow.__init__(self)
	# 		self.isLoaded = 0
	# 		self.count = 1
	# 		self.__LoadWindow()

	# 	def __del__(self):
	# 		ui.ScriptWindow.__del__(self)

	# 	def Show(self):
	# 		if self.isLoaded:
	# 			ui.ScriptWindow.Show(self)
	# 		else:
	# 			ui.ScriptWindow.Show(self)

	# 	def Open(self):
	# 		self.Show()

	# 	def BindInterfaceClass(self, interface):
	# 		self.interface = interface

	# 	def __LoadWindow(self):
	# 		if self.isLoaded == 1:
	# 			return
				
	# 		self.isLoaded = 1
	# 		try:
	# 			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/horseshoe.py")
	# 		except:
	# 			exception.Abort("MountWindow.LoadDialog.LoadScript")

	# 		try:
	# 			self.titleBar = self.GetChild("TitleBar")
	# 			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

	# 			itemtoolTip = uiToolTip.ItemToolTip()
	# 			self.ItemToolTip = itemtoolTip

	# 			self.AcceptButton = self.GetChild("AcceptButton")
	# 			self.AcceptButton.SetEvent(ui.__mem_func__(self.BuyHorseshoe))

	# 			self.price = self.GetChild("Price")
	# 			self.price.SetText("Cena: |cFFb8b8b8"+localeInfo.NumberToGoldNotText(str(PODKOWA_PRICE[0]))+" "+emoji.AppendEmoji("icon/emoji/cheque_icon.png")+"  |cFF85d455"+localeInfo.NumberToGoldNotText(str(PODKOWA_PRICE[1]))+" "+emoji.AppendEmoji("d:/ymir work/ui/stone_point/stone_point.tga"))
			
	# 			self.itemslots = {
	# 				"horseshoe" : [self.GetChild("HorseShoeSlot")],
	# 				"required" : [self.GetChild("Real_Slot%d" % (i)) for i in xrange(2)],
	# 			}
	# 			self.itemslots["horseshoe"][0].SetOverInItemEvent(ui.__mem_func__(self.OnOverInHorseShoe))
	# 			self.itemslots["horseshoe"][0].SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
				
	# 			self.itemslots["required"][0].SetItemSlot(0, REQUIRED_ITEMS[0], REQUIRED_ITEMS_AMOUNT[0]*self.count)
	# 			self.itemslots["required"][1].SetItemSlot(0, REQUIRED_ITEMS[1], REQUIRED_ITEMS_AMOUNT[1]*self.count)
	# 			for i in xrange(2):
	# 				self.itemslots["required"][i].SetOverInItemEvent(lambda slotNumber, iPos = i : self.OnOverInItem(slotNumber, iPos))
	# 				self.itemslots["required"][i].SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))
	# 		except:
	# 			exception.Abort("MountWindow.LoadDialog.BindObject")

	# 		self.itemslots["horseshoe"][0].SetItemSlot(0, PODKOWA_VNUM, 1)
			
	# 		self.SetCenterPosition()
	# 		self.SetTop()

	# 	def Destroy(self):
	# 		self.Hide()
	# 		self.ClearDictionary()

	# 	def Close(self):
	# 		self.Hide()

	# 	def OnPressEscapeKey(self):
	# 		self.Close()
	# 		return TRUE

	# 	def OnOverInItem(self, slotNumber, pos):
	# 		item = REQUIRED_ITEMS[pos]
	# 		if self.ItemToolTip:
	# 			self.ItemToolTip.ClearToolTip()
	# 			self.ItemToolTip.AddItemData(item, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])

	# 	def OnOverInHorseShoe(self):
	# 		if self.ItemToolTip:
	# 			self.ItemToolTip.ClearToolTip()
	# 			self.ItemToolTip.AddItemData(PODKOWA_VNUM, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])

	# 	def OnOverOutItem(self):
	# 		if self.ItemToolTip:
	# 			self.ItemToolTip.HideToolTip()

	# 	def BuyHorseshoe(self):
	# 		net.SendChatPacket("/horseshoe b")