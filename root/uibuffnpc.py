#-*- coding: iso-8859-1 -*-
# import os
import ui
import net
import app
import snd
import chat
# import grp
# import ime
import wndMgr
import mouseModule
import uiScriptLocale
import renderTarget
import localeInfo
import constInfo
# import uiCommon
import uiToolTip
import player
import item
import skill
import chat

# SKILL_POWER = [0.05, 
# 0.10, 0.30, 0.60, 1.00, 1.30, 1.60, 2.00, 2.30, 2.60, 3.00, # 10
# 3.50, 4.00, 4.20, 4.50, 4.70, 5.00, 5.40, 5.70, 6.00, 6.40,  # 20
# 6.80, 7.00, 7.10, 7.40, 7.70, 8.00, 8.30, 8.60, 9.00, 9.30,  # 30
# 9.60, 10.00, 10.40, 10.70, 11.00, 11.45, 11.75, 12.00, 12.35, 12.65,  # 40
# 12.65, 12.65, 12.65, 12.65]
SKILL_POWER = [0.00, 
0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.18, 0.20, 0.22, # 10
0.24, 0.26, 0.28, 0.30, 0.32, 0.34, 0.36, 0.38, 0.40, 0.50,  # 20
0.52, 0.54, 0.56, 0.58, 0.60, 0.63, 0.66, 0.69, 0.72, 0.82,  # 30
0.85, 0.88, 0.91, 0.94, 0.98, 1.02, 1.06, 1.10, 1.15, 1.25,  # 40
1.25, 1.25, 1.25, 1.25]

INT_TABLE = {
0:[1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1], #10
1:[20,21,22,23,24,25,26,27,28,29,30], #20
2:[31,32,33,34,35,36,37,38,39,40,41,42], #30
3:[44]} #p


LEARN_GRANDMASTER_SKILL_COST = [
	500000,
	1000000,
	2000000,
	3000000,
	5000000,
	7500000,
	12000000,
	15000000,
	20000000,
	25000000,
]

IMG_PATH = "d:/ymir work/ui/buff/"

TARGET_RENDER_INDEX_FIRST_USE = 22

skill_name = ["B³ogos³awieñstwo", "Podwy¿. Ataku", "Pomoc Smoka"]

class BuffNPCWindow(ui.ScriptWindow):

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
		self.upgradeBookSlotVnum = 0
		self.showUpgradeGrandmasterSkill = -1
		self.canUpgradeGrandmasterSkillList = []
		self.skill_arrow_page = 0
		
		self.toolTipSkill = 0

		self.isActive = False
		self.sex = 0
		self.intPoints = 0
		self.skillInfo = [[94, 0], [96, 0], [111, 0]]
		self.skillActiveStatus = [0, 0, 0]
		
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.RefreshEquipSlotWindow()
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		for i in xrange(3):
			self.skill_slots.DeactivateSlot(i)
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpcwindow.py")
		except:
			import exception
			exception.Abort("BuffNPCWindow.LoadWindow.LoadObject")
			
		try:
			self.toolTip 	= uiToolTip.ItemToolTip()
			
			self.GetChild("board").SetCloseEvent(self.Close)
			self.skill_slots = self.GetChild("skill_slots")
			self.eq_slots = self.GetChild("equipment_slots")

			self.summonbtn2 = self.GetChild("summon_button2")
			self.summonbtn1 = self.GetChild("summon_button1")
			self.summonbtn2.SetEvent(self.ClickSummonButton)
			self.summonbtn1.SetEvent(self.ClickSummonButton)

			self.sexbtn = self.GetChild("sex_button")
			self.sexbtn.SetEvent(self.ChangeSex)

			self.GetChild("face_icon_0").Hide()
			self.GetChild("face_icon_1").Hide()
			
			# for i in xrange(3):
			# 	self.GetChild("skill_active_icon_%d" % i).Hide()

			self.eq_slots.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			self.eq_slots.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			self.eq_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			self.eq_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			self.eq_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))
			
			self.SetCenterPosition()
		except:
			import exception
			exception.Abort("BuffNPCWindow.LoadWindow.BindObject")

	def GetTabState(self):
		return self.state

	def ChangeSex(self):
		net.SendBuffNPCAction(6, 0, 0)

	def ClickSummonButton(self):
		if self.isActive == True:
			net.SendBuffNPCAction(1, 0, 0)
		else:
			return

	# def SetSkillUseStatus(self, slot0, slot1, slot2):
	# 	for i in xrange(3):
	# 		self.GetChild("skill_active_icon_%d" % i).Show()
	# 	self.skillActiveStatus = [slot0, slot1, slot2]
	# 	iconFile = [IMG_PATH+"on.png", IMG_PATH+"off.png"]
	# 	for i in xrange(len(self.skillActiveStatus)):
	# 		self.GetChild("skill_active_icon_%d" % i).LoadImage(iconFile[int(self.skillActiveStatus[i])])
		
	# def ClickSkillSlot(self, slot):
	# 	net.SendBuffNPCAction(5, slot, self.skillActiveStatus[slot], 0, 0)
		
	def OverInItem(self,index):
		if None != self.toolTip:
			self.toolTip.ClearToolTip()
			self.toolTip.SetInventoryItem(index, player.BUFF_EQUIPMENT)

	def OverOutItem(self):
		self.toolTip.HideToolTip()
		
	def RefreshEquipSlotWindow(self):
		for i in xrange(3):
			itemVnum = player.GetItemIndex(player.BUFF_EQUIPMENT,i)
			itemCount = player.GetItemCount(player.BUFF_EQUIPMENT,i)
			if itemCount <= 1:
				itemCount = 0

				self.eq_slots.SetItemSlot(i, itemVnum, itemCount)
			
		self.eq_slots.RefreshSlot()
	
	def SetClear(self):
		self.isActive = False
		self.GetChild("face_icon_0").Hide()
		self.GetChild("face_icon_1").Hide()
		self.GetChild("buff_name").SetText("")
		self.GetChild("int_points").SetText("")
		# for slot in xrange(3):
		# 	self.skill_slots.ClearSlot(slot)
		# 	self.GetChild("skill_active_icon_%d" % slot).Hide()

	def SetBasicInfo(self, name, sex, intvalue):
		self.isActive = True
		self.sex = sex
		self.intPoints = intvalue

		self.GetChild("buff_name").SetText(str(name))
		self.GetChild("int_points").SetText("Inteligencja: "+str(self.intPoints))
		
		if self.sex == 0:
			self.GetChild("face_icon_1").Hide()
			self.GetChild("face_icon_0").Show()
		else:
			self.GetChild("face_icon_0").Hide()
			self.GetChild("face_icon_1").Show()
		
	def SetSkillInfo(self, skill1, skill2, skill3):
		self.skillInfo = [ [94, int(skill1)], [96, int(skill2)], [111, int(skill3)] ]

		if self.skillInfo[0][1] == 40 and self.skillInfo[1][1] == 40 and self.skillInfo[2][1] == 40:
			constInfo.BuffSkillInfo_Master = 1
		else:
			constInfo.BuffSkillInfo_Master = 0
			
		self.SetSkillSlots()

	def SetSummon(self):
		self.summonbtn1.Show()
		self.summonbtn2.Hide()
		
	def SetUnsummon(self):
		self.summonbtn2.Show()
		self.summonbtn1.Hide()

	def SetSkillSlots(self):
		for slot in xrange(3):
			# self.skill_slots.ClearSlot(slot)
			
			skillIndex = self.skillInfo[slot][0]
			skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
			skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])

			if self.skill_slots.IsActivatedSlot(slot):
				if skillGrade != 2:
					self.skill_slots.DeactivateSlot(slot)
					net.SendBuffNPCAction(5, -1, 0)
			
			self.skill_slots.SetSlotStyle(wndMgr.SLOT_STYLE_NONE)
			self.skill_slots.SetSkillSlotNew(slot, skillIndex, skillGrade, skillLevel)
			self.skill_slots.SetSlotCountNew(slot, skillGrade, skillLevel)
			self.skill_slots.SetCoverButton(slot)
			# self.skill_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			# self.skill_slots.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot))
			self.skill_slots.SetOverInItemEvent(ui.__mem_func__(self.OverInSkillSlot))
			self.skill_slots.SetOverOutItemEvent(ui.__mem_func__(self.OverOutSkillSlot))

			self.skill_slots.SetUseSlotEvent(ui.__mem_func__(self.ClickSkillSlot2))
			self.skill_slots.SetUnselectItemSlotEvent(ui.__mem_func__(self.ClickSkillSlot2))

	def ClickSkillSlot2(self, slotNumber):
		skillGrade = self.GetSkillGrade(self.skillInfo[slotNumber][1])
		# skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slotNumber][1])
		# skillIndex = self.skillInfo[slotNumber][0]
		if skillGrade == 2 and not skillGrade == 3:
			if self.skill_slots.IsActivatedSlot(slotNumber):
				self.skill_slots.DeactivateSlot(slotNumber)
				net.SendBuffNPCAction(5, -1, 0)
			else:
				for i in xrange(3):
					net.SendBuffNPCAction(5, -1, 0)
					self.skill_slots.DeactivateSlot(i)

				self.skill_slots.ActivateSlot(slotNumber)
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Wybrana umiejêtnoœæ: "+str(skill_name[slotNumber]))
				chat.AppendChat(chat.CHAT_TYPE_INFO, str(slotNumber))
				net.SendBuffNPCAction(5, slotNumber, 0)

	def SetSkillCooltime(self, slot, value):
		self.skill_slots.SetSlotCoolTime(int(slot), int(value))
		
	def SelectEmptySlot(self, selectedSlotPos):

		isAttached = mouseModule.mouseController.isAttached()

		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.BUFF_EQUIPMENT, selectedSlotPos, attachedCount)
			
	def SelectItemSlot(self, selectedSlotPos):
	
		isAttached = mouseModule.mouseController.isAttached()
		
		if isAttached:
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			mouseModule.mouseController.DeattachObject()
			itemIndex = player.GetItemIndex(attachedSlotPos)
			itemCount = player.GetItemCount(attachedSlotPos)

			item.SelectItem(itemIndex)
			itemType = item.GetItemType()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.INVENTORY, attachedSlotPos, player.BUFF_EQUIPMENT, selectedSlotPos, attachedCount)
		else:
			itemVnum = player.GetItemIndex(player.BUFF_EQUIPMENT, selectedSlotPos)
			itemCount = player.GetItemCount(player.BUFF_EQUIPMENT, selectedSlotPos)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_BUFF_EQUIPMENT, selectedSlotPos, itemVnum, itemCount)

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		net.SendItemUsePacket(player.BUFF_EQUIPMENT, slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def OverInSkillSlot(self, slot):
		if mouseModule.mouseController.isAttached():
			return

		if 0 == self.toolTipSkill:
			return

		skillIndex = self.skillInfo[slot][0]
		skillGrade = self.GetSkillGrade(self.skillInfo[slot][1])
		skillLevel = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])
		
		if skillGrade == 3 and skillLevel == 40:
			skillLevel_tooltip = 0
		else:
			skillLevel_tooltip = self.GetSkillLevel(skillGrade, self.skillInfo[slot][1])

		# for i in xrange(2):
			# if (self.GetSkillLevel(skillGrade, self.skillInfo[i][1]) == 40):
			# 	intPoints1 = 730+237+self.intPoints
			# 	intPoints0 = INT_TABLE[skillGrade][skillLevel_tooltip]+45+self.intPoints
			# else:
		intPoints1 = 165
		intPoints0 = INT_TABLE[skillGrade][skillLevel_tooltip]

		if slot == 0 or slot == 1:
			self.toolTipSkill.SetSkillBuffNPC(skillIndex, skillIndex, skillGrade, skillLevel, 
			SKILL_POWER[self.skillInfo[slot][1]], 
			SKILL_POWER[self.skillInfo[slot][1]+1], 
			intPoints0
			)
		elif slot == 2:
			self.toolTipSkill.SetSkillBuffNPC(skillIndex, skillIndex, skillGrade, skillLevel, 
			SKILL_POWER[self.skillInfo[slot][1]], 
			SKILL_POWER[self.skillInfo[slot][1]+1], 
			intPoints1
			)

	def GetSkillGrade(self, skillLevel):
		if skillLevel >= 0 and skillLevel < 20:
			return 0
		if skillLevel >= 20 and skillLevel < 30:
			return 1
		if skillLevel >= 30 and skillLevel < 40: 
			return 2
		if skillLevel == 40: 
			return 3

	def GetSkillLevel(self, skillGrade, skillLevel):
		if skillGrade == 0:
			return skillLevel
		if skillGrade == 1:
			return skillLevel - 19
		if skillGrade == 2:
			return skillLevel - 29
		if skillGrade == 3:
			return 40

	def OverOutSkillSlot(self):
		if 0 != self.toolTipSkill:
			self.toolTipSkill.HideToolTip()
			
	def SetSkillToolTip(self, toolTipSkill):
		self.toolTipSkill = toolTipSkill
		
	def OnPressEscapeKey(self):
		self.Close()
		return True

class BuffNPCCreateWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		
		self.selectSexButton = []
		
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.GetChild("editline_name").SetText("")
		self.GetChild("editline_name").SetFocus()
		self.__OnClickChangeSexButton(0)
		ui.ScriptWindow.Show(self)
			
	def Close(self):
		self.Hide()
	
	def Destroy(self):
		self.Hide()
		self.ClearDictionary()
		
	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/buffnpccreatewindow.py")
		except:
			import exception
			exception.Abort("BuffNPCCreateWindow.LoadWindow.LoadObject")
			
		try:
			self.GetChild("board").SetCloseEvent(self.Close)
			self.GetChild("accept_button").SetEvent(self.ClickAcceptButton)
			self.GetChild("cancel_button").SetEvent(self.Close)
			
			self.selectSexButton = [self.GetChild("button_shaman_m"), self.GetChild("button_shaman_w")]
		
			self.ModelPreview = ui.RenderTarget()
			self.ModelPreview.SetParent(self.GetChild("render_target_background"))
			self.ModelPreview.SetSize(170, 220)
			self.ModelPreview.SetPosition(5, 5)
			self.ModelPreview.SetRenderTarget(TARGET_RENDER_INDEX_FIRST_USE)
			self.ModelPreview.Show()
			renderTarget.SetBackground(TARGET_RENDER_INDEX_FIRST_USE, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetVisibility(TARGET_RENDER_INDEX_FIRST_USE, True)
			
			self.__OnClickChangeSexButton(0)
			
			for i in xrange(len(self.selectSexButton)):
				self.selectSexButton[i].SetEvent(lambda sex = i : self.__OnClickChangeSexButton(sex))
			
		except:
			import exception
			exception.Abort("BuffNPCCreateWindow.LoadWindow.BindObject")

	def __OnClickChangeSexButton(self, sex):
		self.selected_sex = sex
		if sex == 0:
			self.selectSexButton[0].Down()
			self.selectSexButton[1].SetUp()
			renderTarget.SelectModel(TARGET_RENDER_INDEX_FIRST_USE, 7)
			renderTarget.SetArmor(TARGET_RENDER_INDEX_FIRST_USE, 1)
		if sex == 1:
			self.selectSexButton[0].SetUp()
			self.selectSexButton[1].Down()
			renderTarget.SelectModel(TARGET_RENDER_INDEX_FIRST_USE, 3)
			renderTarget.SetArmor(TARGET_RENDER_INDEX_FIRST_USE, 1)
		
	def ClickAcceptButton(self):
		if self.GetChild("editline_name").GetText() == "":
			chat.AppendChat(chat.CHAT_TYPE_INFO, uiScriptLocale.ASLAN_BUFF_CHAT_INFO_NAME_IS_EMPTY)
			return
		net.SendBuffNPCCreate(1, self.GetChild("editline_name").GetText(), self.selected_sex)
		self.Close()
		
	def OnPressEscapeKey(self):
		self.Close()
		return True
