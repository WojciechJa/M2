#-*- coding: iso-8859-1 -*-
import ui
import player
import mouseModule
import net
import app
import snd
import item
import player
import chat
import uibonus
import grp
import uiScriptLocale
import uiRefine
import uiAttachMetin
import uiPickMoney
import uiCommon
# import math
import uiPrivateShopBuilder
import localeInfo
import constInfo
# import uiremoveitem
# import exchange
if app.ENABLE_AURA_SYSTEM:
	import aura
import ime
if app.ENABLE_NEW_STONE_DETACH:
	import uistonedetach
import wndMgr
import uiToolTip
# import interfaceModule
from _weakref import proxy
if app.ENABLE_CHEQUE_SYSTEM:
	import uiToolTip
	import uiPickETC
if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder
	import uiOfflineShop
if app.ENABLE_ACCE_COSTUME_SYSTEM:
	import acce
import emoji
import zmianaeq
import uiDepoIS
if app.ITEM_CHECKINOUT_UPDATE:
	import exchange

if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
	import uiPrivateShop
	import privateShop

# import uiPickEtc
if app.WJ_SPLIT_INVENTORY_SYSTEM:
	SPECIAL_INV_NAMES = ["Ksiêgi", "Ulepszacze", "KD i Alchemia", "Skrzynie", "Przedmioty U¿ytkowe", "Mikstury"]
	start_val_table = [item.SKILL_BOOK_INVENTORY_SLOT_START, item.UPGRADE_ITEMS_INVENTORY_SLOT_START, item.STONE_INVENTORY_SLOT_START, item.BOX_INVENTORY_SLOT_START, item.EFSUN_INVENTORY_SLOT_START, item.CICEK_INVENTORY_SLOT_START]
	end_val_table = [item.SKILL_BOOK_INVENTORY_SLOT_END, item.UPGRADE_ITEMS_INVENTORY_SLOT_END, item.STONE_INVENTORY_SLOT_END, item.BOX_INVENTORY_SLOT_END, item.EFSUN_INVENTORY_SLOT_END, item.CICEK_INVENTORY_SLOT_END]
		
INSTANT_OPEN_COUNT = 1000

SYSTEMS_WINDOW=1
ITEM_MALL_BUTTON_ENABLE = True
ITEM_FLAG_APPLICABLE = 1 << 14

ALLOWED_TYPES = [
"USE_ADD_ATTRIBUTE", "USE_CHANGE_ATTRIBUTE", 
"USE_ADD_ATTRIBUTE_SPECIAL_1", "USE_ADD_ATTRIBUTE", 
"USE_CHANGE_ATTRIBUTE2", "USE_TUNING", "USE_ADD_IS_BONUS",
"USE_ADD_NEW_BONUS", "USE_ADD_EXTRA_BONUS"
]

class SystemsWindow(ui.ScriptWindow):
	def __init__(self, wndInventory, tooltip):
		import exception

		if not wndInventory:
			return
			 	 
		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.selectedSlotPos=0
		self.wndInventory = wndInventory;
		self.expandBtn = None
		self.minBtn = None
		self.toolTip = tooltip

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def SetTooltip(self, tooltip):
		self.toolTip = tooltip

	def Show(self, gelen):
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)
		
		if gelen == 1:
			constInfo.SYSTEMS_WINDOW_OPEN=1
			self.OpenInventory()
		else:
			constInfo.SYSTEMS_WINDOW_OPEN=0
			self.CloseInventory()

	def BindInterfaceClass(self, interface):
		self.interface = interface

	def Close(self):
		constInfo.SYSTEMS_WINDOW_OPEN=0
		for i in xrange(1,self.TOOLTIP_VALUE_MAX):
			self.toolTip[i].HideToolTip()
		self.Hide()

	def Destroy(self):
		constInfo.SYSTEMS_WINDOW_OPEN=0
		self.interface = None
		for i in xrange(1,self.TOOLTIP_VALUE_MAX):
			self.toolTip[i].HideToolTip()
		self.Hide()
		
	def IsOpeningInventory(self):
		return self.wndSystemsWindowLayer.IsShow()
		
	def OpenInventory(self):
		global SYSTEMS_WINDOW
		self.wndSystemsWindowLayer.Show()
		self.expandBtn.Hide()
		SYSTEMS_WINDOW = 1

		self.AdjustPositionAndSize()

	def CloseInventory(self):
		global SYSTEMS_WINDOW
		self.wndSystemsWindowLayer.Hide()
		self.expandBtn.Show()
		SYSTEMS_WINDOW = 0
		
		self.AdjustPositionAndSize()

	def GetBasePosition(self):
		ckontrol=(int(open("lib/ustawienia","r").readlines()[0]) == 1)
		ckontrolx=(int(open("lib/ustawienia","r").readlines()[0]) == 0)
		if ckontrol==True:
			x, y = self.wndInventory.GetGlobalPosition()
			return x - 160+15, y + 100
		elif ckontrolx==True:
			x, y = self.wndInventory.GetGlobalPosition()
			return x - 160 + 90, y + 100 -30
		
	def AdjustPositionAndSize(self):
		ckontrol=(int(open("lib/ustawienia","r").readlines()[0]) == 1)
		ckontrolx=(int(open("lib/ustawienia","r").readlines()[0]) == 0)
		if ckontrol==True:
			bx, by = self.GetBasePosition()
			byEK=37
			if self.IsOpeningInventory():			
				self.SetPosition(bx, by+6-byEK)
				self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
			else:
				self.SetPosition(bx + 158-15, by+10-byEK);
				self.SetSize(10, self.GetHeight())
		elif ckontrolx==True:
			bx, by = self.GetBasePosition()
			byEK=37
			if self.IsOpeningInventory():			
				self.SetPosition(bx-17, by+6-byEK)
				self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())
			else:
				self.SetPosition(bx + 158 - 90, by+6-byEK);
				self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			ckontrol=(int(open("lib/ustawienia","r").readlines()[0]) == 1)
			ckontrolx=(int(open("lib/ustawienia","r").readlines()[0]) == 0)
			if ckontrolx==True:
				constInfo.MenuSelectModel2=0
				pyScrLoader.LoadScriptFile(self, "UIScript/SystemsWindow.py")
			elif ckontrol==True:
				constInfo.MenuSelectModel1=0
				pyScrLoader.LoadScriptFile(self, "UIScript/nowy_systemswindow.py")
		except:
			import exception
			exception.Abort("SystemsWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			self.wndSystemsWindowLayer = self.GetChild("SystemsWindowLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")
			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))


			gc = self.GetChild
			m = ui.__mem_func__
			t = uiToolTip.ToolTipNew
			i = self.wndInventory

			self.TOOLTIP_VALUE_MAX = 23
			self.toolTip = {
				1:t(120),2:t(120), #mapa 			#zapis
				3:t(98),4:t(150), #sklep-offline 	#antyexp
				5:t(150),6:t(132), #dodatkowe-eq 	#pet
				7:t(147),8:t(132), #ranking			#dopalacze
				9:t(129),10:t(129), #tabela-bon		#switcher
				11:t(134),12:t(134), #pomocnik		#marmury
				13:t(55),14:t(55), #kosz			#runy
				15:t(132),16:t(80),#biolog			#artefakty
				17:t(134),18:t(132),
				19:t(138),20:t(118),
				21:t(140),22:t(140),
			}
			self.button = {
				1:gc("Mapa_Button"),2:gc("Localizacja_Button"),
				3:gc("OfflineShopButton"),4:gc("aExp_Button"),
				5:gc("Dodatkowe_Button"),6:gc("Pet_Button"),
				7:gc("Ranking_Button"),8:gc("Dopek_Button"),
				9:gc("TabelaBon_Button"),10:gc("Switcher_Button"),
				11:gc("Shaman_Button"),12:gc("Marmur_Button"),
				13:gc("Kosz_Button"),14:gc("Rune_Button"),
				15:gc("BiologButton"),
				16:gc("Artefakt_Button"),
				17:gc("BossTp_Button"),
				18:gc("ZlomiarzButton"),
				19:gc("Mount_Button"),
				20:gc("Dung_Button",),
				21:gc("ChangeEq_Button"),
				22:gc("DepoIs_Button")
			}

			self.button[1].SetEvent(m(i.ToggleMapa))
			self.button[2].SetEvent(m(i.ToggleLokalizacja))
			self.button[3].SetEvent(m(i.ToggleOfflineShop))
			self.button[4].SetEvent(m(i.ToggleAntyExp))
			self.button[5].SetEvent(m(i.ToggleDodatkowy))
			self.button[6].SetEvent(m(i.TogglePet))
			self.button[7].SetEvent(m(i.ToggleRanking))
			self.button[8].SetEvent(m(i.ToggleWspomagacze))
			self.button[9].SetEvent(m(i.ToggleBonus))
			self.button[10].SetEvent(m(i.ToggleSwitcher))
			self.button[11].SetEvent(m(i.ToggleShaman))
			self.button[12].SetEvent(m(i.ToggleMarmur))
			self.button[13].SetEvent(m(i.ToggleKosz))
			self.button[14].SetEvent(m(i.ClickRuneButton))
			self.button[15].SetEvent(m(i.ToggleCollectWindow))
			self.button[16].SetEvent(m(i.ToggleArtefakt))
			self.button[17].SetEvent(m(i.ToggleTpMetkiBossy))
			self.button[18].SetEvent(m(i.ToggleZlomiarzWindow))
			self.button[19].SetEvent(m(i.ToggleMount))
			self.button[20].SetEvent(m(i.ToggleDungeon))
			self.button[21].SetEvent(m(i.ClickFastEquipButton))
			self.button[22].SetEvent(m(i.DepoIsToggle))

			self.toolTip[22].SetTitle("Magazyn / ItemShop")
			self.toolTip[21].SetTitle("Zmiana ekwipunku")
			self.toolTip[20].SetTitle("Panel Wypraw")
			self.toolTip[20].AppendHorizontalLine()
			self.toolTip[20].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f10.png")))
			self.toolTip[19].SetTitle("Panel Wierzchowca")
			# self.toolTip[19].AppendHorizontalLine()
			# self.toolTip[19].AppendGreenTextLine("Dostêpne skróty:")
			# self.toolTip[19].AppendTextLine("{} - Otwórz okno".format(Emoji("icon\emoji\key_f12.png")))
			self.toolTip[15].SetTitle("Panel Misji")
			self.toolTip[15].AppendHorizontalLine()
			self.toolTip[15].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f11.png")))

			self.toolTip[18].SetTitle("Z³omiarz")
			self.toolTip[18].AppendHorizontalLine()
			self.toolTip[18].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f12.png")))

			self.toolTip[13].SetTitle("Kosz")
			self.toolTip[14].SetTitle("Runy")
			self.toolTip[16].SetTitle("Artefakty")

			self.toolTip[17].SetTitle("Tp Metki/Bossy")
			self.toolTip[17].AppendHorizontalLine()
			self.toolTip[17].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f8.png")))

			self.toolTip[12].SetTitle("Sklep z Marmurami")
			self.toolTip[12].AppendHorizontalLine()
			self.toolTip[12].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f6.png")))

			self.toolTip[11].SetTitle("Panel Pomocnika")
			self.toolTip[11].AppendHorizontalLine()
			self.toolTip[11].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f9.png")))

			self.toolTip[10].SetTitle("Bonus Switcher")
			# self.toolTip[10].AppendHorizontalLine()
			# self.toolTip[10].AppendGreenTextLine("Dostêpne skróty:")
			# self.toolTip[10].AppendTextLine("{} - Otwórz okno".format(Emoji("icon/emoji/key_f5.png")))

			self.toolTip[9].SetTitle("Tabela Bonusów")

			self.toolTip[8].SetTitle("Dopalacze")
			self.toolTip[8].AppendHorizontalLine()
			self.toolTip[8].AppendTextLine("{} - Aktywuj".format(emoji.AppendEmoji("icon/emoji/key_f5.png")))

			self.toolTip[7].SetTitle("Ranking Tygodniowy")

			self.toolTip[6].SetTitle("Panel Zwierzaka")
			self.toolTip[6].AppendHorizontalLine()
			self.toolTip[6].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_p.png")))

			self.toolTip[5].SetTitle("Dodatkowy Ekwipunek")
			self.toolTip[5].AppendHorizontalLine()
			self.toolTip[5].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_k.png")))

			self.toolTip[4].SetTitle("Blokada Doœwiadczenia")
			self.toolTip[3].SetTitle("Sklep Offline")

			self.toolTip[2].SetTitle("Zapis Pozycji")
			self.toolTip[2].AppendHorizontalLine()
			self.toolTip[2].AppendTextLine("{} - Otwórz okno".format(emoji.AppendEmoji("icon/emoji/key_f7.png")))

			self.toolTip[1].SetTitle("Mapa Teleportacji")
		except:
			import exception
			exception.Abort("SystemsWindow.LoadWindow.BindObject")

	def OpenSafeBox(self):
		self.uiDepoIS.Open()

	def OnUpdate(self):
		for i in xrange(1,self.TOOLTIP_VALUE_MAX):
			self.toolTip[i].HideToolTip()
			if self.button[i].IsIn():
				self.toolTip[i].Show()
			else:
				self.toolTip[i].HideToolTip()
		
		global SYSTEMS_WINDOW
		self.AdjustPositionAndSize()
		
		if constInfo.SYSTEMS_WINDOW_KAPAT==1:
			self.Close()
			constInfo.SYSTEMS_WINDOW_KAPAT=0

class RuneWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):				
			 	 
		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		self.RefreshEquipSlotWindow()
		self.SetCenterPosition()

		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/RuneWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			wndEquip = self.GetChild("RuneSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.InfoBtn = self.GetChild("InfoBtn")

			t = uiToolTip.ToolTipNew
			self.tt = {
				0:t(190),
			}
			self.tt[0].AppendSpace(6)
			self.tt[0].SetTitle("Dodatkowe bonusy")
			self.tt[0].AppendSpace(6)
			self.tt[0].AppendTextLine("Ca³e ko³o Run +5:")
			self.tt[0].AppendGreenTextLine("Koñcowe Obra¿enia +5%")
			self.tt[0].AppendGreenTextLine("Œrednie Obra¿enia +15%")
			self.tt[0].AppendGreenTextLine("Silny przeciwko Ludziom +25%")
			# self.tt[0].AppendSpace(6)
			# self.tt[0].AppendTextLine("Ca³e ko³o Run +9:")
			# self.tt[0].AppendGreenTextLine("Koñcowe Obra¿enia: +10%")
			# self.tt[0].AppendGreenTextLine("Œrednie Obra¿enia +25%")
			# self.tt[0].AppendGreenTextLine("Silny przeciwko Ludziom +40%")
			# self.tt[0].AppendSpace(6)
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))						
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndEquip = wndEquip
	 
	def RefreshEquipSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndEquip.SetItemSlot
		for i in xrange(player.EQUIPMENT_PAGE_COUNT):
			slotNumber = player.EQUIPMENT_SLOT_START + i
			itemCount = getItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
			
		if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			for i in xrange(player.NEW_EQUIPMENT_SLOT_COUNT):
				slotNumber = player.NEW_EQUIPMENT_SLOT_START + i
				itemCount = getItemCount(slotNumber)
				if itemCount <= 1:
					itemCount = 0
				setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
				# print "ENABLE_NEW_EQUIPMENT_SYSTEM", slotNumber, itemCount, getItemVNum(slotNumber)

		self.wndEquip.RefreshSlot()

	def OnUpdate(self):
		if self.InfoBtn.IsIn():
			self.tt[0].Show()
		else:
			self.tt[0].Hide()

class CostumeWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):
		import exception

		if not app.ENABLE_COSTUME_SYSTEM:
			exception.Abort("What do you do?")
			return

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList = []

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.__LoadWindow()
		self.RefreshCostumeSlot()

		ui.ScriptWindow.Show(self)

	def Close(self):
		self.Hide()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 147, y

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		self.SetPosition(bx, by)
		self.SetSize(self.GetWidth(), self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/CostumeWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			wndEquip = self.GetChild("CostumeSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))

			if app.ENABLE_HIDE_COSTUME_SYSTEM:
				self.visibleButtonList.append(self.GetChild("BodyToolTipButton"))
				self.visibleButtonList.append(self.GetChild("HairToolTipButton"))
				self.visibleButtonList.append(self.GetChild("AcceToolTipButton"))
				self.visibleButtonList.append(self.GetChild("WeaponToolTipButton"))
				self.visibleButtonList.append(self.GetChild("AuraToolTipButton"))
				self.visibleButtonList.append(self.GetChild("StoleToolTipButton"))

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndEquip = wndEquip

		if app.ENABLE_HIDE_COSTUME_SYSTEM:
			self.visibleButtonList[0].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 1, 0)
			self.visibleButtonList[1].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 2, 0)
			self.visibleButtonList[2].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 3, 0)
			self.visibleButtonList[3].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 4, 0)
			self.visibleButtonList[4].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 5, 0)
			self.visibleButtonList[5].SetToggleUpEvent(ui.__mem_func__(self.VisibleCostume), 6, 0)

			self.visibleButtonList[0].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 1, 1)
			self.visibleButtonList[1].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 2, 1)
			self.visibleButtonList[2].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 3, 1)
			self.visibleButtonList[3].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 4, 1)
			self.visibleButtonList[4].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 5, 1)
			self.visibleButtonList[5].SetToggleDownEvent(ui.__mem_func__(self.VisibleCostume), 6, 1)

	def RefreshCostumeSlot(self):
		getItemVNum=player.GetItemIndex

		for i in xrange(item.COSTUME_SLOT_COUNT):
			slotNumber = item.COSTUME_SLOT_START + i
			self.wndEquip.SetItemSlot(slotNumber, getItemVNum(slotNumber), 0)

		if app.ENABLE_WEAPON_COSTUME_SYSTEM:
			self.wndEquip.SetItemSlot(item.COSTUME_SLOT_WEAPON, getItemVNum(item.COSTUME_SLOT_WEAPON), 0)

		self.wndEquip.RefreshSlot()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			body = constInfo.HIDDEN_BODY_COSTUME
			hair = constInfo.HIDDEN_HAIR_COSTUME
			acce = constInfo.HIDDEN_ACCE_COSTUME
			weapon = constInfo.HIDDEN_WEAPON_COSTUME
			aura = constInfo.HIDDEN_AURA_COSTUME
			stole = constInfo.HIDDEN_STOLE_COSTUME

			if body == 1:
				# self.visibleButtonList[0].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[0].Down()
			else:
				# self.visibleButtonList[0].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[0].SetUp()

			if hair == 1:
				# self.visibleButtonList[1].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[1].Down()
			else:
				# self.visibleButtonList[1].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[1].SetUp()

			if acce == 1:
				# self.visibleButtonList[2].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[2].Down()
			else:
				# self.visibleButtonList[2].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[2].SetUp()

			if weapon == 1:
				# self.visibleButtonList[3].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[3].Down()
			else:
				# self.visibleButtonList[3].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[3].SetUp()

			if aura == 1:
				# self.visibleButtonList[4].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[4].Down()
			else:
				# self.visibleButtonList[4].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[4].SetUp()
		
			if stole == 1:
				# self.visibleButtonList[2].SetToolTipText(localeInfo.SHOW_COSTUME)
				self.visibleButtonList[5].Down()
			else:
				# self.visibleButtonList[2].SetToolTipText(localeInfo.HIDE_COSTUME)
				self.visibleButtonList[5].SetUp()

		def VisibleCostume(self, part, hidden):
			net.SendChatPacket("/hide_costume %d %d" % (part, hidden))

if app.WJ_SPLIT_INVENTORY_SYSTEM:
	class ExtendedInventoryWindow(ui.ScriptWindow):
		tooltipItem = None
		dlgPickMoney = None

		dlgPickItem = None
		sellingSlotNumber = -1
		isLoaded = 0
		if app.ENABLE_HIGHLIGHT_NEW_ITEM:
			liHighlightedItems = []

		def __init__(self):
			ui.ScriptWindow.__init__(self)
			self.inventoryPageIndex = 0
			self.__LoadWindow()
			
		def __del__(self):
			ui.ScriptWindow.__del__(self)


		def Show(self):
			constInfo.EXTENDED_INVENTORY_IS_OPEN = 1
			self.__LoadWindow()
			ui.ScriptWindow.Show(self)	
			
		def BindInterfaceClass(self, interface):
			self.interface = interface

		def __LoadWindow(self):
			if self.isLoaded == 1:
				return

			self.isLoaded = 1
			
			try:
				pyScrLoader = ui.PythonScriptLoader()
				pyScrLoader.LoadScriptFile(self, "UIScript/ExtendedInventoryWindow.py")
			except:
				import exception
				exception.Abort("ExtendedInventoryWindow.LoadWindow.LoadObject")

			try:
				wndItem = self.GetChild("ItemSlot")
				self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
				# self.titleName = self.GetChild("TitleName")
				self.titleName = self.GetChild("inventory_title")
				self.SkillBookButton = self.GetChild("SkillBookButton")
				self.UpgradeItemsButton = self.GetChild("UpgradeItemsButton")
				self.stoneButton = self.GetChild("StoneButton")
				self.boxButton = self.GetChild("BoxButton")
				self.efsunButton = self.GetChild("EfsunButton")
				self.cicekButton = self.GetChild("CicekButton")

				self.SettingsButton = self.GetChild("SettingsButton")
				self.window_settings = self.GetChild("window_settings")
				self.DirectUseButton = self.GetChild("DirectUseButton")
				self.OpenWithInventoryButton = self.GetChild("OpenWithInventoryButton")
				self.OpenWhenFastMoveButton = self.GetChild("OpenWhenFastMoveButton")

				self.inventoryTab = []
				self.inventoryTab.append(self.GetChild("Inventory_Tab_01"))
				self.inventoryTab.append(self.GetChild("Inventory_Tab_02"))
				self.inventoryTab.append(self.GetChild("Inventory_Tab_03"))
				self.sortinv = self.GetChild("RefreshButton")
				self.packall = self.GetChild("PackAllButton")
				# self.MalzemeDeposuInfo = self.GetChild("MalzemeDeposuInfo")
			except:
				import exception
				exception.Abort("ExtendedInventoryWindow.LoadWindow.BindObject")

			## Info
			self.tlInfo = uiToolTip.ItemToolTip()
			self.tlInfo.Hide()
			self.tooltipInfo = [self.tlInfo]*7
			self.InformationText = [localeInfo.MALZEME_DEPOSU,
									localeInfo.BK_ENVANTER_TEXT1,
									localeInfo.BK_ENVANTER_TEXT2,
									localeInfo.BK_ENVANTER_TEXT3,
									localeInfo.BK_ENVANTER_TEXT4,
									localeInfo.BK_ENVANTER_TEXT5,
									localeInfo.BK_ENVANTER_TEXT6
			]
			
			t = uiToolTip.ToolTipNew
			self.tt = {
				1:t(180),
				2:t(140),
				3:t(50),
				4:t(75),
				5:t(90),
				6:t(60),
				7:t(130),
				8:t(60),
				9:t(190),
				10:t(225),
				11:t(240),
				12:t(233),
			}
			self.tt[1].SetTitle("Po³¹cz przedmioty")
			self.tt[1].AppendSpace(2)
			self.tt[1].AppendHorizontalLine()
			self.tt[1].AppendTextLine("{} {}".format(emoji.AppendEmoji("icon/emoji/key_lclick.png"), "- Po³¹cz w kategorii"))
			self.tt[1].AppendTextLine("{} + {} {}".format(emoji.AppendEmoji("icon/emoji/key_shift.png"),  emoji.AppendEmoji("icon/emoji/key_lclick.png"), "- Po³¹cz wszystkie"))
			
			# self.tt[2].SetTitle("Ustawienia ekwipunku")

			self.tt[9].SetTitle("Otwieraj wraz z ekwipunkiem")

			self.tt[10].SetTitle("Bezpoœrednie u¿ywanie")
			self.tt[10].AppendSpace(2)
			self.tt[10].AppendHorizontalLine()
			self.tt[10].AppendTextLine("Wy³¹czenie tej opcji spowoduje wpadanie")
			self.tt[10].AppendTextLine("przedmiotów do zwyk³ego ekwipunku.")

			self.tt[11].SetTitle("Otwieranie podczas przenoszenia")
			self.tt[11].AppendSpace(2)
			self.tt[11].AppendHorizontalLine()
			self.tt[11].AppendTextLine("W³¹czenie tej opcji spowoduje w³¹czanie")
			self.tt[11].AppendTextLine("danych kategorii dodat. ekwipunku podczas")
			self.tt[11].AppendTextLine("Szybkiego Przenoszenia Przedmiotów")
			self.tt[11].AppendTextLine("skrótem klawiszowym: {} + {}".format(emoji.AppendEmoji("icon/emoji/key_shift.png"), emoji.AppendEmoji("icon/emoji/key_rclick.png")))

			self.tt[12].SetTitle("Przenieœ wszystko")
			self.tt[12].AppendSpace(2)
			self.tt[12].AppendHorizontalLine()
			self.tt[12].AppendTextLine("Przenosi wszystkie przedmioty z podstawowego")
			self.tt[12].AppendTextLine("ekwipunku do aktualnie w³¹czonej kategorii.")

			self.tt[3].SetTitle("Ksiêgi")
			self.tt[4].SetTitle("Ulepszacze")
			self.tt[5].SetTitle("KD i Alchemia")
			self.tt[6].SetTitle("Skrzynie")
			self.tt[7].SetTitle("Przedmioty u¿ytkowe")
			self.tt[8].SetTitle("Mikstury")

			for i in xrange(7):
				self.tooltipInfo[i].SetFollow(True)
				self.tooltipInfo[i].AlignHorizonalCenter()
				if i == 0:
					TITLE_COLOR = grp.GenerateColor(0.9490, 0.9058, 0.7568, 1.0)
					self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i], TITLE_COLOR)
				else:
					self.tooltipInfo[i].AutoAppendTextLine(self.InformationText[i])
				self.tooltipInfo[i].Hide()
				self.tooltipInfo[i].toolTipWidth += 10

			## Item
			wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
			wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
			wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
			wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
			wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

			self.sortinv.SetEvent(ui.__mem_func__(self.sortowanie))
			self.packall.SetEvent(ui.__mem_func__(self.onclickpackall))
			
			self.SettingsButton.SetEvent(ui.__mem_func__(self.OpenSettings))
			self.OpenWithInventoryButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableEnvanter))
			self.OpenWithInventoryButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableEnvanter))
			self.OpenWhenFastMoveButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableFastMoveOpen))
			self.OpenWhenFastMoveButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableFastMoveOpen))
			self.DirectUseButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableWpadanie))
			self.DirectUseButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableWpadanie))
			self.window_settings.Hide()

			if constInfo.EnvanterAcilsinmi == 1:
				self.OpenWithInventoryButton.Down()

			if constInfo.enablefastopenopen == 1:
				self.OpenWhenFastMoveButton.Down()

			if constInfo.WPADANIE_DODATKOWE == 0:
				self.DirectUseButton.Down()

			# self.envanterileacilsin = ui.CheckBox()
			# self.envanterileacilsin.SetParent(self)
			# self.envanterileacilsin.SetPosition(133, 12)
			# self.envanterileacilsin.SetEvent(ui.__mem_func__(self.__OnClickEnableEnvanterOn), "ON_CHECK", True)
			# self.envanterileacilsin.SetEvent(ui.__mem_func__(self.__OnClickDisableEnvanterOf), "ON_UNCKECK", False)
			# # self.envanterileacilsin.SetTextInfo(localeInfo.ENVANTER_ILE_AC)
			# self.envanterileacilsin.SetCheckStatus(constInfo.EnvanterAcilsinmi)
			# self.envanterileacilsin.Show()

			self.SkillBookButton.SetEvent(lambda arg=0: self.SetInventoryType(arg))
			self.UpgradeItemsButton.SetEvent(lambda arg=1: self.SetInventoryType(arg))
			self.stoneButton.SetEvent(lambda arg=2: self.SetInventoryType(arg))
			self.boxButton.SetEvent(lambda arg=3: self.SetInventoryType(arg))
			self.efsunButton.SetEvent(lambda arg=4: self.SetInventoryType(arg))
			self.cicekButton.SetEvent(lambda arg=5: self.SetInventoryType(arg))
			self.SkillBookButton.Down()

			self.inventoryTab[0].SetEvent(lambda arg=0: self.SetInventoryPage(arg))
			self.inventoryTab[1].SetEvent(lambda arg=1: self.SetInventoryPage(arg))
			self.inventoryTab[2].SetEvent(lambda arg=2: self.SetInventoryPage(arg))
			self.inventoryTab[0].Down()
			
			## PickMoneyDialog
			dlgPickMoney = uiPickMoney.PickMoneyDialog()
			dlgPickMoney.LoadDialog()
			dlgPickMoney.Hide()

			## PickItemDialog
			dlgPickItem = uiPickETC.PickETCDialog()
			dlgPickItem.LoadDialog()
			dlgPickItem.Hide()
			
			self.dlgPickMoney = dlgPickMoney
			
			self.wndItem = wndItem
			self.SetInventoryType(0)
			self.SetInventoryPage(0)

			self.dlgPickItem = dlgPickItem

			self.wndCostume = None
			
		def Destroy(self):
			self.ClearDictionary()
			self.dlgPickMoney.Destroy()
			self.dlgPickItem.Destroy()
			self.dlgPickItem = 0
			self.dlgPickMoney = 0
			self.tooltipItem = None
			self.wndItem = 0
			self.interface = None
			self.inventoryTab = []
			# self.envanterileacilsin = []

		def __OnClickEnableEnvanter(self):
			if constInfo.EnvanterAcilsinmi == 1:
				constInfo.EnvanterAcilsinmi = 0
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Otwieranie wraz z ekwipunkiem dezaktywowane.")
			else:
				constInfo.EnvanterAcilsinmi = 1
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Otwieranie wraz z ekwipunkiem aktywowane.")

		def __OnClickEnableFastMoveOpen(self):
			if constInfo.enablefastopenopen == 1:
				constInfo.enablefastopenopen = 0
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Otwieranie podczas szybkiego przenoszenia dezaktywowane.")
			else:
				constInfo.enablefastopenopen = 1
				chat.AppendChat(chat.CHAT_TYPE_INFO, "Otwieranie podczas szybkiego przenoszenia aktywowane.")

		def __OnClickEnableWpadanie(self):
			net.SendChatPacket("/extended_inventory_status")

		def OpenSettings(self):
			if self.window_settings.IsShow():
				self.window_settings.Hide()
			else:
				self.window_settings.Show()

		def sortowanie(self):
			if app.IsPressed(app.DIK_LSHIFT):
				net.SendChatPacket("/sort_special_inventory 6")
			else:
				net.SendChatPacket("/sort_special_inventory %d" % self.inventoryType)

		def GetEmptyItemPosPack(self, type, itemHeight, blocked_slots):
			start_val = start_val_table[type - 1]  # type - 1 because skillbook type is 1 and the table starts by 0.
			end_val = end_val_table[type - 1]
			GetBlockedSlots = lambda slot, size: [slot + (round * 3) for round in xrange(size)]
			free_slots = [
				slot for slot in xrange(start_val, end_val)
				if slot not in blocked_slots and
				all(e not in blocked_slots for e in GetBlockedSlots(slot, itemHeight))
			]
			return free_slots if free_slots else -1

		def onclickpackall(self):
			inventory_size = player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT
			blocked_slots = []

			for i in xrange(inventory_size):
				itemVnum = player.GetItemIndex(i)
				itemCount = player.GetItemCount(i)
				item.SelectItem(itemVnum)

				if item.GetSpecialInvType() != self.inventoryType + 1:
					continue

				(w, h) = item.GetItemSize()
				emptyInvenSlots = self.GetEmptyItemPosPack(self.inventoryType + 1, h, blocked_slots)
				if emptyInvenSlots == -1:
					continue

				self.__SendMoveItemPacket(i, emptyInvenSlots[0], itemCount)

				for round in xrange(h):
					blocked_slot = emptyInvenSlots[0] + (round * 3)
					if blocked_slot not in blocked_slots:
						blocked_slots.append(blocked_slot)

		def Hide(self):
			if None != self.tooltipItem:
				self.tooltipItem.HideToolTip()
				self.tlInfo.Hide()
			if self.dlgPickItem:
				self.dlgPickItem.Close()
			wndMgr.Hide(self.hWnd)
			
		def Close(self):
			constInfo.EXTENDED_INVENTORY_IS_OPEN = 0
			for i in xrange(1, 13):
				self.tt[i].Hide()
			if self.tooltipItem:
				self.tooltipItem.HideToolTip()
			self.tlInfo.Hide()
			self.Hide()

		# def OnUpdate(self):
		# 	for i in xrange(7):
		# 		if self.MalzemeDeposuInfo.IsIn():
		# 			self.tooltipInfo[i].Show()
		# 		else:
		# 			self.tooltipInfo[i].Hide()

		def SetInventoryType(self, type):
			self.inventoryType = type
			if type == 0:
				self.SkillBookButton.Down()
				self.efsunButton.SetUp()
				self.cicekButton.SetUp()
				self.UpgradeItemsButton.SetUp()
				self.stoneButton.SetUp()
				self.boxButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_SKILL_BOOK_TOOLTIP)
			elif type == 2:
				self.stoneButton.Down()
				self.efsunButton.SetUp()
				self.cicekButton.SetUp()
				self.UpgradeItemsButton.SetUp()
				self.SkillBookButton.SetUp()
				self.boxButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_STONE_TOOLTIP)
			elif type == 3:
				self.boxButton.Down()
				self.efsunButton.SetUp()
				self.UpgradeItemsButton.SetUp()
				self.cicekButton.SetUp()
				self.stoneButton.SetUp()
				self.SkillBookButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_BOX_TOOLTIP)
			elif type == 4:
				self.efsunButton.Down()
				self.boxButton.SetUp()
				self.cicekButton.SetUp()
				self.UpgradeItemsButton.SetUp()
				self.stoneButton.SetUp()
				self.SkillBookButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_EFSUN_TOOLTIP)
			elif type == 5:
				self.cicekButton.Down()
				self.efsunButton.SetUp()
				self.boxButton.SetUp()
				self.UpgradeItemsButton.SetUp()
				self.stoneButton.SetUp()
				self.SkillBookButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_CICEK_TOOLTIP)
			else:
				self.UpgradeItemsButton.Down()
				self.SkillBookButton.SetUp()
				self.efsunButton.SetUp()
				self.cicekButton.SetUp()
				self.stoneButton.SetUp()
				self.boxButton.SetUp()
				self.titleName.SetText(localeInfo.INVENTORY_UPGRADE_ITEM_TOOLTIP)
			self.RefreshBagSlotWindow()
			
		def SetInventoryPage(self, page):
			self.inventoryPageIndex = page
			for i in range(0,len(self.inventoryTab)):
				self.inventoryTab[i].SetUp()
			self.inventoryTab[page].Down()
			self.RefreshBagSlotWindow()

		def OnPickItem(self, count):
			itemSlotIndex = self.dlgPickItem.itemGlobalSlotIndex
			selectedItemVNum = player.GetItemIndex(itemSlotIndex)
			mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SKILL_BOOK_INVENTORY, itemSlotIndex, selectedItemVNum, count)

		if app.ENABLE_EXTENDED_BLEND:
			def __CanAttachThisItem(self, itemVNum, itemSlotIndex):
				if constInfo.IS_PERMANANET_BLEND_ITEM(itemVNum):
					isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
					if isActivated == 1:
						return False

				# if constInfo.IS_COMPANION_ITEM(itemVNum):
					# isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
					# if isActivated == 1:
						# return False

				return True
				
		def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
			if self.inventoryType == 0:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.SKILL_BOOK_INVENTORY_SLOT_COUNT + local + item.SKILL_BOOK_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.SKILL_BOOK_INVENTORY_SLOT_COUNT + local + item.SKILL_BOOK_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.SKILL_BOOK_INVENTORY_SLOT_COUNT + local + item.SKILL_BOOK_INVENTORY_SLOT_START)
				
			elif self.inventoryType == 1:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.UPGRADE_ITEMS_INVENTORY_SLOT_COUNT + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.UPGRADE_ITEMS_INVENTORY_SLOT_COUNT + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.UPGRADE_ITEMS_INVENTORY_SLOT_COUNT + local + item.UPGRADE_ITEMS_INVENTORY_SLOT_START)


			elif self.inventoryType == 2:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.STONE_INVENTORY_SLOT_COUNT + local + item.STONE_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.STONE_INVENTORY_SLOT_COUNT + local + item.STONE_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.STONE_INVENTORY_SLOT_COUNT + local + item.STONE_INVENTORY_SLOT_START)

			elif self.inventoryType == 3:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.BOX_INVENTORY_SLOT_COUNT + local + item.BOX_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.BOX_INVENTORY_SLOT_COUNT + local + item.BOX_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.BOX_INVENTORY_SLOT_COUNT + local + item.BOX_INVENTORY_SLOT_START)
			

			elif self.inventoryType == 4:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.EFSUN_INVENTORY_SLOT_COUNT + local + item.EFSUN_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.EFSUN_INVENTORY_SLOT_COUNT + local + item.EFSUN_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.EFSUN_INVENTORY_SLOT_COUNT + local + item.EFSUN_INVENTORY_SLOT_START)
			

			elif self.inventoryType == 5:
				if self.inventoryPageIndex == 0:
					return self.inventoryPageIndex*player.CICEK_INVENTORY_SLOT_COUNT + local + item.CICEK_INVENTORY_SLOT_START
				elif self.inventoryPageIndex == 1:
					return int(0.3334*player.CICEK_INVENTORY_SLOT_COUNT + local + item.CICEK_INVENTORY_SLOT_START)
				elif self.inventoryPageIndex == 2:
					return int(0.6668*player.CICEK_INVENTORY_SLOT_COUNT + local + item.CICEK_INVENTORY_SLOT_START)

			if player.IsSkillBookInventorySlot(local) or player.IsUpgradeItemsInventorySlot(local) or player.IsStoneInventorySlot(local) or player.IsBoxInventorySlot(local) or player.IsEfsunInventorySlot(local) or player.IsCicekInventorySlot(local):
				return local

		def GetInventoryPageIndex(self):
			return self.inventoryPageIndex
			
		def RefreshBagSlotWindow(self):
			getItemVNum=player.GetItemIndex
			getItemCount=player.GetItemCount
			setItemVNum=self.wndItem.SetItemSlot
			
			if self.inventoryType == 0:
				for i in xrange(player.SKILL_BOOK_INVENTORY_SLOT_COUNT):
					slotNumber = item.SKILL_BOOK_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0	
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)
			elif self.inventoryType == 2:
				for i in xrange(player.STONE_INVENTORY_SLOT_COUNT):
					slotNumber = item.STONE_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0	
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)	
			elif self.inventoryType == 3:
				for i in xrange(player.BOX_INVENTORY_SLOT_COUNT):
					slotNumber = item.BOX_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)	
			elif self.inventoryType == 4:
				for i in xrange(player.EFSUN_INVENTORY_SLOT_COUNT):
					slotNumber = item.EFSUN_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)	
			elif self.inventoryType == 5:
				for i in xrange(player.CICEK_INVENTORY_SLOT_COUNT):
					slotNumber = item.CICEK_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)
			else:
				for i in xrange(player.UPGRADE_ITEMS_INVENTORY_SLOT_COUNT):
					slotNumber = item.UPGRADE_ITEMS_INVENTORY_SLOT_START + i
					if self.GetInventoryPageIndex() == 1:
						slotNumber += 45
					elif self.GetInventoryPageIndex() == 2:
						slotNumber += 90
					itemCount = getItemCount(slotNumber)
					if 0 == itemCount:
						self.wndItem.ClearSlot(i)
						continue
					elif 1 == itemCount:
						itemCount = 0
					itemVnum = getItemVNum(slotNumber)
					setItemVNum(i, itemVnum, itemCount)

			self.wndItem.RefreshSlot()	
			if app.ENABLE_HIGHLIGHT_NEW_ITEM:
				self.__RefreshHighlights()
				for i in xrange(135):
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
					if not slotNumber in self.liHighlightedItems:
						self.wndItem.DeactivateSlot(i)

			#blend fix
			for i in xrange(135):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				itemVnum = getItemVNum(slotNumber)
				if constInfo.IS_BLEND_ITEM(itemVnum):
					metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
					isActivated = 0 != metinSocket[1]
				
					if isActivated:
						self.wndItem.ActivateSlot(i)
					else:
						self.wndItem.DeactivateSlot(i)

		# def RefreshBagSlotWindow(self):
		# 	def process_slots(slot_count, slot_start):
		# 		page_index = self.GetInventoryPageIndex()
		# 		offset = 45 * page_index if page_index in (1, 2) else 0

		# 		for i in xrange(slot_count):
		# 			slot_number = slot_start + i + offset
		# 			item_count = player.GetItemCount(slot_number)
		# 			if item_count == 0:
		# 				self.wndItem.ClearSlot(i)
		# 				continue
		# 			if item_count == 1:
		# 				item_count = 0
		# 			item_vnum = player.GetItemIndex(slot_number)
		# 			self.wndItem.SetItemSlot(i, item_vnum, item_count)

		# 	inventory_map = {
		# 		0: (player.SKILL_BOOK_INVENTORY_SLOT_COUNT, item.SKILL_BOOK_INVENTORY_SLOT_START),
		# 		2: (player.STONE_INVENTORY_SLOT_COUNT, item.STONE_INVENTORY_SLOT_START),
		# 		3: (player.BOX_INVENTORY_SLOT_COUNT, item.BOX_INVENTORY_SLOT_START),
		# 		4: (player.EFSUN_INVENTORY_SLOT_COUNT, item.EFSUN_INVENTORY_SLOT_START),
		# 		5: (player.CICEK_INVENTORY_SLOT_COUNT, item.CICEK_INVENTORY_SLOT_START),
		# 		"default": (player.UPGRADE_ITEMS_INVENTORY_SLOT_COUNT, item.UPGRADE_ITEMS_INVENTORY_SLOT_START)
		# 	}

		# 	slot_count, slot_start = inventory_map.get(self.inventoryType, inventory_map["default"])
		# 	process_slots(slot_count, slot_start)

		# 	if app.ENABLE_HIGHLIGHT_NEW_ITEM:
		# 		self.__RefreshHighlights()
		# 		for i in xrange(135):
		# 			slot_number = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
		# 			if slot_number not in self.liHighlightedItems:
		# 				self.wndItem.DeactivateSlot(i)

		# 	for i in xrange(135):
		# 		slot_number = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
		# 		item_vnum = player.GetItemIndex(slot_number)
		# 		if constInfo.IS_BLEND_ITEM(item_vnum):
		# 			metin_socket = [player.GetItemMetinSocket(slot_number, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
		# 			is_activated = metin_socket[1] != 0
		# 			if is_activated:
		# 				self.wndItem.ActivateSlot(i)
		# 			else:
		# 				self.wndItem.DeactivateSlot(i)

		# 	self.wndItem.RefreshSlot()

		if app.ENABLE_HIGHLIGHT_NEW_ITEM:
			def HighlightSlot(self, slot):
				if not slot in self.liHighlightedItems:
					self.liHighlightedItems.append(slot)

			def __RefreshHighlights(self):		
				for i in xrange(135):
					slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

					if slotNumber in self.liHighlightedItems:
						self.wndItem.ActivateSlot(i)

		def RefreshItemSlot(self):
			self.RefreshBagSlotWindow()

		def RefreshStatus(self):
			pass

		def SetItemToolTip(self, tooltipItem):
			self.tooltipItem = tooltipItem
			
		def SelectEmptySlot(self, selectedSlotPos):
			if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
				return
				
			#BONUS SWITCHER
			if constInfo.IS_BONUS_CHANGER == True:
				return
			#END OF BONUS SWITCHER

			if self.inventoryType == 0:
				selectedSlotPos += item.SKILL_BOOK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			elif self.inventoryType == 2:
				selectedSlotPos += item.STONE_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			elif self.inventoryType == 3:
				selectedSlotPos += item.BOX_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			elif self.inventoryType == 4:
				selectedSlotPos += item.EFSUN_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			elif self.inventoryType == 5:
				selectedSlotPos += item.CICEK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			else:
				selectedSlotPos += item.UPGRADE_ITEMS_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					selectedSlotPos += 45
				elif self.GetInventoryPageIndex() == 2:
					selectedSlotPos += 90
			
			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				if player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType:
					mouseModule.mouseController.DeattachObject()
					# chat.AppendChat(chat.CHAT_TYPE_INFO, str(attachedSlotType)+" - "+str(selectedSlotPos))
					return
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
				attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

				if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
					itemCount = player.GetItemCount(attachedSlotPos)
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()

					if self.dlgPickItem and self.dlgPickItem.IsSplitAll():
						net.SendChatPacket("/split_items %d %d %d" % (attachedSlotPos, attachedCount, selectedSlotPos))
						self.dlgPickItem.SplitClear()
					else:
						self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

					if item.IsRefineScroll(attachedItemIndex):
						self.wndItem.SetUseMode(False)

				elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
					mouseModule.mouseController.RunCallBack("INVENTORY")

				elif player.SLOT_TYPE_BUFF_EQUIPMENT == attachedSlotType and app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
					attachedCount = mouseModule.mouseController.GetAttachedItemCount()
					net.SendItemMovePacket(player.BUFF_EQUIPMENT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

				elif player.SLOT_TYPE_SHOP == attachedSlotType:
					net.SendShopBuyPacket(attachedSlotPos)

				elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

					if player.ITEM_MONEY == attachedItemIndex:
						net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
						snd.PlaySound("sound/ui/money.wav")

					else:
						net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

				elif player.SLOT_TYPE_MALL == attachedSlotType:
					net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

				mouseModule.mouseController.DeattachObject()
				
		#BONUS SWITCHER
		def IsChanger(self, itemVnum):
			changerList = [71084] #JeÅ¼eli masz wiÄ™cej zmianek to po przecinku dopisz.
			if itemVnum in changerList:
				return True
			
			return False
		#END OF BONUS SWITCHER

		def SelectItemSlot(self, itemSlotIndex):
			if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
				return

			#BONUS SWITCHER
			if constInfo.IS_BONUS_CHANGER == True:
				return
			#END OF BONUS SWITCHER

			if self.inventoryType == 0:
				itemSlotIndex += item.SKILL_BOOK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90
			elif self.inventoryType == 2:
				itemSlotIndex += item.STONE_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90
			elif self.inventoryType == 3:
				itemSlotIndex += item.BOX_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90
			elif self.inventoryType == 4:
				itemSlotIndex += item.EFSUN_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90
			elif self.inventoryType == 5:
				itemSlotIndex += item.CICEK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90
			else:
				itemSlotIndex += item.UPGRADE_ITEMS_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					itemSlotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					itemSlotIndex += 90

			if mouseModule.mouseController.isAttached():
				attachedSlotType = mouseModule.mouseController.GetAttachedType()
				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()
				attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
				
				#BONUS SWITCHER
				if self.IsChanger(attachedItemVID) and not player.IsEquipmentSlot(itemSlotIndex):
					itemVnum = player.GetItemIndex(itemSlotIndex)
					item.SelectItem(itemVnum)
					if item.GetItemType() == item.ITEM_TYPE_WEAPON or item.GetItemType() == item.ITEM_TYPE_ARMOR:
						self.interface.AddToBonusChange(itemSlotIndex, attachedSlotPos)
						mouseModule.mouseController.DeattachObject()
						# self.wndItem[itemSlotIndex].SetChangerImage()
						return
				#END OF BONUS SWITCHER

				# ??? bugfix
				# if player.GetItemCount(itemSlotIndex) > attachedItemCount:
				# 	return
				
				if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
					player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
					self.__SendMoveItemPacket(attachedSlotPos, itemSlotIndex, attachedItemCount)
		
				mouseModule.mouseController.DeattachObject()
			else:

				curCursorNum = app.GetCursor()
				#if app.SELL == curCursorNum:
					#self.__SellItem(itemSlotIndex)
					
				if app.BUY == curCursorNum:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

				elif app.IsPressed(app.DIK_LALT):
					link = player.GetItemLink(itemSlotIndex)
					ime.PasteString(link)

				elif app.IsPressed(app.DIK_LSHIFT):
					itemCount = player.GetItemCount(itemSlotIndex)
				
					if itemCount > 1:
						self.dlgPickItem.SetTitleName(localeInfo.PICK_ITEM_TITLE)
						self.dlgPickItem.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
						self.dlgPickItem.Open(itemCount)
						self.dlgPickItem.itemGlobalSlotIndex = itemSlotIndex

				elif app.IsPressed(app.DIK_LCONTROL):
					itemIndex = player.GetItemIndex(itemSlotIndex)

					if TRUE == item.CanAddToQuickSlotItem(itemIndex):
						player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
					else:
						chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)
				else:

					selectedItemVNum = player.GetItemIndex(itemSlotIndex)
					itemCount = player.GetItemCount(itemSlotIndex)
					if self.inventoryType == 0:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_SKILL_BOOK_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					elif self.inventoryType == 2:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_STONE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					elif self.inventoryType == 3:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_BOX_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					elif self.inventoryType == 4:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_EFSUN_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					elif self.inventoryType == 5:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CICEK_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					else:
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
					self.wndItem.SetUseMode(True)

					if app.ENABLE_EXTENDED_BLEND:
						if self.__CanAttachThisItem(selectedItemVNum, itemSlotIndex):
							mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_CICEK_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

					snd.PlaySound("sound/ui/pick.wav")

		def OnCloseQuestionDialog(self):
			if not self.questionDialog:
				return
			
			self.questionDialog.Close()
			self.questionDialog = None
			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

		def Sat(self):
			if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
				if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
					net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
					snd.PlaySound("sound/ui/money.wav")
			self.OnCloseQuestionDialog()
			
		def __OnClosePopupDialog(self):
			self.pop = None
			
		def OverOutItem(self):
			self.wndItem.SetUsableItem(False)
			if None != self.tooltipItem:
				self.tooltipItem.HideToolTip()

		def OverInItem(self, overSlotPos):
			# if self.inventoryType == 0:
			# 	overSlotPos += item.SKILL_BOOK_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			# elif self.inventoryType == 2:
			# 	overSlotPos += item.STONE_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			# elif self.inventoryType == 3:
			# 	overSlotPos += item.BOX_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			# elif self.inventoryType == 4:
			# 	overSlotPos += item.EFSUN_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			# elif self.inventoryType == 5:
			# 	overSlotPos += item.CICEK_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			# else:
			# 	overSlotPos += item.UPGRADE_ITEMS_INVENTORY_SLOT_START
			# 	if self.GetInventoryPageIndex() == 1:
			# 		overSlotPos += 45
			# 	elif self.GetInventoryPageIndex() == 2:
			# 		overSlotPos += 90
			self.wndItem.SetUsableItem(False)
			test = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
			self.ShowToolTip(test)

			if app.ENABLE_HIGHLIGHT_NEW_ITEM:
				if test in self.liHighlightedItems:
					self.liHighlightedItems.remove(test)
					self.wndItem.DeactivateSlot(overSlotPos)
			
		def ShowToolTip(self, slotIndex):
			if None != self.tooltipItem:
				self.tooltipItem.SetInventoryItem(slotIndex)
				
		def OnPressEscapeKey(self):
			self.Close()
			return True	
		
		#test
		def GetEmptyItemPos(self, itemHeight):
			inventory_size = player.INVENTORY_PAGE_SIZE * player.INVENTORY_PAGE_COUNT 
			GetBlockedSlots = lambda slot, size: [slot+(round*5) for round in xrange(size)] 
			blocked_slots = [element for sublist in [GetBlockedSlots(slot, item.GetItemSize(item.SelectItem(player.GetItemIndex(slot)))[1]) for slot in xrange(inventory_size) if player.isItem(slot)] for element in sublist] 
			if itemHeight > 1: 
				[[blocked_slots.append(slot) for slot in range(inventory_size/round-(itemHeight-1)*5, inventory_size/round) if not slot in blocked_slots] for round in xrange(1,3)] 
			free_slots = [slot for slot in xrange(inventory_size) if not slot in blocked_slots and not True in [e in blocked_slots for e in [slot+(round*5) for round in xrange(itemHeight)]]] 
			return [free_slots, -1][len(free_slots) == 0]
		#end

		def UseItemSlot(self, slotIndex):
			if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
				return

			slotIndexior = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
			itemVnum1 = player.GetItemIndex(slotIndexior)
			item.SelectItem(itemVnum1)
			if (item.CanUseInSpecialInv() == 1):
				self.__UseItem(slotIndexior)
				return

			if not app.IsPressed(app.DIK_LSHIFT):
				if not app.IsPressed(app.DIK_LCONTROL):
					#test
					if mouseModule.mouseController.isAttached():
						mouseModule.mouseController.DeattachObject()

					test = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
					itemVnum = player.GetItemIndex(test)
					itemCount = player.GetItemCount(test)
					item.SelectItem(itemVnum)
					(w, h) = item.GetItemSize()
					emptyInvenSlots = self.GetEmptyItemPos(h)
					# chat.AppendChat(chat.CHAT_TYPE_INFO, "itemvnum:"+str(itemVnum)+", slotindex:"+str(slotIndex) + ", emptyslot:"+str(emptyInvenSlots[0])+", localslottoglobalslot:"+str(test)+", count:"+str(itemCount))
					if emptyInvenSlots == -1:
						return
					self.__SendMoveItemPacket(test, emptyInvenSlots[0], itemCount)
					#end

			#BONUS SWITCHER
			if constInfo.IS_BONUS_CHANGER:
				return
			#END OF BONUS SWITCHER

			if self.inventoryType == 0:
				slotIndex += item.SKILL_BOOK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
			elif self.inventoryType == 2:
				slotIndex += item.STONE_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
			elif self.inventoryType == 3:
				slotIndex += item.BOX_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
			elif self.inventoryType == 4:
				slotIndex += item.EFSUN_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
			elif self.inventoryType == 5:
				slotIndex += item.CICEK_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
			else:
				slotIndex += item.UPGRADE_ITEMS_INVENTORY_SLOT_START
				if self.GetInventoryPageIndex() == 1:
					slotIndex += 45
				elif self.GetInventoryPageIndex() == 2:
					slotIndex += 90
	
			if app.IsPressed(app.DIK_LSHIFT) and self.interface.wndRemoveItem.IsShow():
				self.interface.wndRemoveItem.AppendSlot(player.INVENTORY, slotIndex)
				return

			if app.ENABLE_ODLAMKI_SYSTEM:
				if app.IsPressed(app.DIK_LSHIFT) and self.interface.wndOdlamki.IsShow():
					itemType = item.GetItemType()
					if not item.ITEM_TYPE_METIN == itemType:
						chat.AppendChat(chat.CHAT_TYPE_INFO, "Mo¿esz w³o¿yæ tylko kamienie dusz!")
						return
					else:
						self.interface.wndOdlamki.AppendSlot(player.INVENTORY, slotIndex)
						return

			self.__UseItem(slotIndex)
			mouseModule.mouseController.DeattachObject()
			self.OverOutItem()

		def __UseItem(self, slotIndex):
			ItemVNum = player.GetItemIndex(slotIndex)

			getItemCount=player.GetItemCount
			itemCount = getItemCount(slotIndex)

			item.SelectItem(ItemVNum)

			if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
				self.questionDialog = uiCommon.QuestionDialog()
				self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
				self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
				self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
				self.questionDialog.Open()
				self.questionDialog.slotIndex = slotIndex
			
				constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

			if app.ENABLE_INSTANT_CHEST_OPEN:
				if (item.GetItemType() == item.ITEM_TYPE_GIFTBOX) or (item.GetItemType() == item.ITEM_TYPE_GACHA):
					if app.IsPressed(app.DIK_LALT) and not app.IsPressed(app.DIK_LCONTROL):
						itemUseCount = player.GetItemCount(slotIndex) if player.GetItemCount(slotIndex) <= INSTANT_OPEN_COUNT else INSTANT_OPEN_COUNT
						net.SendOpenChestPacket(slotIndex, itemUseCount)
						return

			if app.__BL_CHEST_DROP_INFO__:
				if app.IsPressed(app.DIK_LCONTROL):
					isMain = not app.IsPressed(app.DIK_LSHIFT)
					if item.HasDropInfo(ItemVNum, isMain) and self.interface:
						self.interface.OpenChestDropWindow(ItemVNum, isMain)
					return			
				
				else:
					self.__SendUseItemPacket(slotIndex)
				
		def __UseItemQuestionDialog_OnCancel(self):
			self.OnCloseQuestionDialog()

		def __UseItemQuestionDialog_OnAccept(self):
			self.__SendUseItemPacket(self.questionDialog.slotIndex)
			self.OnCloseQuestionDialog()	
			
		def __SendUseItemPacket(self, slotPos):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
				return
				
			if self.interface.wndRemoveItem.IsShow():
				return

			if app.ENABLE_ODLAMKI_SYSTEM:
				if self.interface.wndOdlamki.IsShow():
					return

			net.SendItemUsePacket(slotPos)

		def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
			if uiPrivateShopBuilder.IsBuildingPrivateShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
				return

			net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

		def OnUpdate(self):
			if constInfo.WPADANIE_DODATKOWE == 0:
				self.DirectUseButton.Down()
			else:
				self.DirectUseButton.SetUp()

			self.tt[1].Hide()
			for i in range(2, 13):
				self.tt[i].Hide()
			if self.SkillBookButton.IsIn():
				self.tt[3].Show()
			else:
				self.tt[3].Hide()
			if self.UpgradeItemsButton.IsIn():
				self.tt[4].Show()
			else:
				self.tt[4].Hide()
			if self.stoneButton.IsIn():
				self.tt[5].Show()
			else:
				self.tt[5].Hide()
			if self.boxButton.IsIn():
				self.tt[6].Show()
			else:
				self.tt[7].Hide()
			if self.efsunButton.IsIn():
				self.tt[7].Show()
			else:
				self.tt[7].Hide()
			if self.cicekButton.IsIn():
				self.tt[8].Show()
			else:
				self.tt[8].Hide()

			# if self.SettingsButton.IsIn():
			# 	self.tt[2].Show()
			# else:
			# 	self.tt[2].Hide()

			if self.DirectUseButton.IsIn():
				self.tt[10].Show()
			else:
				self.tt[10].Hide()
			
			if self.OpenWithInventoryButton.IsIn():
				self.tt[9].Show()
			else:
				self.tt[9].Hide()

			if self.OpenWhenFastMoveButton.IsIn():
				self.tt[11].Show()
			else:
				self.tt[11].Hide()

			if self.sortinv.IsIn():
				self.tt[1].Show()
			else:
				self.tt[1].Hide()

			if self.packall.IsIn():
				self.tt[12].Show()
			else:
				self.tt[12].Hide()

class BeltInventoryWindow(ui.ScriptWindow):

	def __init__(self, wndInventory):
		import exception

		if not app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			exception.Abort("What do you do?")
			return

		if not wndInventory:
			exception.Abort("wndInventory parameter must be set to InventoryWindow")
			return

		ui.ScriptWindow.__init__(self)

		self.isLoaded = 0
		self.wndInventory = wndInventory;

		self.ltInventoryLayer = None
		self.wndBeltInventorySlot = None
		self.expandBtn = None
		self.minBtn = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self, openBeltSlot = False):
		self.__LoadWindow()
		self.RefreshSlot()

		ui.ScriptWindow.Show(self)

		if openBeltSlot:
			self.OpenInventory()
		else:
			self.CloseInventory()

	def Close(self):
		self.Hide()

	def IsOpeningInventory(self):
		return self.wndBeltInventoryLayer.IsShow()

	def OpenInventory(self):
		self.wndBeltInventoryLayer.Show()
		self.expandBtn.Hide()

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()

	def CloseInventory(self):
		self.wndBeltInventoryLayer.Hide()
		self.expandBtn.Show()

		if localeInfo.IsARABIC() == 0:
			self.AdjustPositionAndSize()

	def GetBasePosition(self):
		x, y = self.wndInventory.GetGlobalPosition()
		return x - 148, y + 241

	def AdjustPositionAndSize(self):
		bx, by = self.GetBasePosition()

		if self.IsOpeningInventory():
			self.SetPosition(bx, by)
			self.SetSize(self.ORIGINAL_WIDTH, self.GetHeight())

		else:
			self.SetPosition(bx + 138, by);
			self.SetSize(10, self.GetHeight())

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return

		self.isLoaded = 1

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "UIScript/BeltInventoryWindow.py")
		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.LoadObject")

		try:
			self.ORIGINAL_WIDTH = self.GetWidth()
			wndBeltInventorySlot = self.GetChild("BeltInventorySlot")
			self.wndBeltInventoryLayer = self.GetChild("BeltInventoryLayer")
			self.expandBtn = self.GetChild("ExpandBtn")
			self.minBtn = self.GetChild("MinimizeBtn")

			self.expandBtn.SetEvent(ui.__mem_func__(self.OpenInventory))
			self.minBtn.SetEvent(ui.__mem_func__(self.CloseInventory))

			if localeInfo.IsARABIC() :
				self.expandBtn.SetPosition(self.expandBtn.GetWidth() - 2, 15)
				self.wndBeltInventoryLayer.SetPosition(self.wndBeltInventoryLayer.GetWidth() - 5, 0)
				self.minBtn.SetPosition(self.minBtn.GetWidth() + 3, 15)

			for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
				slotNumber = item.BELT_INVENTORY_SLOT_START + i
				wndBeltInventorySlot.SetCoverButton(slotNumber,	"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/quest/slot_button_01.sub",\
												"d:/ymir work/ui/game/belt_inventory/slot_disabled.tga", False, False)

		except:
			import exception
			exception.Abort("CostumeWindow.LoadWindow.BindObject")

		## Equipment
		wndBeltInventorySlot.SetOverInItemEvent(ui.__mem_func__(self.wndInventory.OverInItem))
		wndBeltInventorySlot.SetOverOutItemEvent(ui.__mem_func__(self.wndInventory.OverOutItem))
		wndBeltInventorySlot.SetUnselectItemSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetUseSlotEvent(ui.__mem_func__(self.wndInventory.UseItemSlot))
		wndBeltInventorySlot.SetSelectEmptySlotEvent(ui.__mem_func__(self.wndInventory.SelectEmptySlot))
		wndBeltInventorySlot.SetSelectItemSlotEvent(ui.__mem_func__(self.wndInventory.SelectItemSlot))

		self.wndBeltInventorySlot = wndBeltInventorySlot

	def RefreshSlot(self):
		getItemVNum=player.GetItemIndex

		for i in xrange(item.BELT_INVENTORY_SLOT_COUNT):
			slotNumber = item.BELT_INVENTORY_SLOT_START + i
			self.wndBeltInventorySlot.SetItemSlot(slotNumber, getItemVNum(slotNumber), player.GetItemCount(slotNumber))
			self.wndBeltInventorySlot.SetAlwaysRenderCoverButton(slotNumber, True)

			avail = "0"

			if player.IsAvailableBeltInventoryCell(slotNumber):
				self.wndBeltInventorySlot.EnableCoverButton(slotNumber)
			else:
				self.wndBeltInventorySlot.DisableCoverButton(slotNumber)

		self.wndBeltInventorySlot.RefreshSlot()


class InventoryWindow(ui.ScriptWindow):

	USE_TYPE_TUPLE = ("USE_CLEAN_SOCKET", "USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ATTRIBUTE2", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET", "USE_ADD_ATTRIBUTE_SPECIAL_1", "USE_ADD_IS_BONUS", "USE_ADD_NEW_BONUS", "USE_ADD_EXTRA_BONUS")
	if app.ENABLE_USE_COSTUME_ATTR:
		USE_TYPE_TUPLE = tuple(list(USE_TYPE_TUPLE) + ["USE_CHANGE_COSTUME_ATTR", "USE_RESET_COSTUME_ATTR"])

	questionDialog = None
	tooltipItem = None
	wndCostume = None
	wndRune = None
	if app.ENABLE_NEW_STONE_DETACH:
		wndStoneDetach = None
	wndBelt = None
	dlgPickMoney = None
	interface = None
	if app.WJ_ENABLE_TRADABLE_ICON:
		bindWnds = []
	dlgPickItem = None
	if app.ENABLE_CHEQUE_SYSTEM:
		dlgPickETC = None
	wndSystemsWindow = None

	sellingSlotNumber = -1
	isLoaded = 0
	isOpenedCostumeWindowWhenClosingInventory = 0
	isOpenedBeltWindowWhenClosingInventory = 0

	if app.ENABLE_HIGHLIGHT_NEW_ITEM:
		liHighlightedItems = []

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		wndPrivateShop			= None
		wndPrivateShopSearch	= None

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.isOpenedBeltWindowWhenClosingInventory = 0

		self.okno3 = zmianaeq.changeequip()

		self.inventoryPageIndex = 0
		self.uiDepoIS = uiDepoIS.DepoIsWindow()

		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			self.wndExpandedMoneyBar = None
			self.wndPktOsiag = None

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			self.wndExpandedMoneyBar = None
			self.wndPktOsiag = None

	def ToggleMapa(self):
		self.interface.ToggleMapaSwWindow()

	def ToggleLokalizacja(self):
		self.interface.OpenLocationWindow()

	def ToggleOfflineShop(self):
		#self.interface.TogglePrivateShopPanelWindowCheck()
		net.SendChatPacket("/register_off_shop")

	def ToggleWspomagacze(self):
		self.interface.OpenBlendWindow()

	def ToggleAntyExp(self):
		net.SendChatPacket("/toggle_antyexp")

	def ToggleDodatkowy(self):
		self.interface.ToggleExtendedInventoryWindow()

	def TogglePet(self):
		self.interface.pet_window_open()

	def ToggleRanking(self):
		self.interface.ToggleWeeklyRankWindow()

	def ToggleSwitcher(self):
		self.interface.ToggleSwitchbotWindow()

	def ToggleMarmur(self):
		self.interface.ToggleMarmurShopWindow()

	def ToggleBonus(self):
		self.interface.ToggleBonusWindow()

	def ToggleKosz(self):
		self.interface.OpenRemoveItem(self.interface.wndInventory)

	def ToggleShaman(self):
		self.interface.BuffNPCOpenWindow()

	if app.ENABLE_HIDE_COSTUME_SYSTEM:
		def RefreshVisibleCostume(self):
			if self.wndCostume:
				self.wndCostume.RefreshVisibleCostume()
			else:
				self.wndCostume = CostumeWindow(self)
				self.wndCostume.RefreshVisibleCostume()

	def ToggleArtefakt(self):
		self.interface.OpenArtefaktWindow()

	def ToggleCollectWindow(self):
		net.SendChatPacket("/collect_window")

	def ToggleZlomiarzWindow(self):
		self.interface.ToggleCollectionWindow()

	def ToggleTpMetkiBossy(self):
		self.interface.OpenRespWindow()

	def ToggleMount(self):
		self.interface.ToggleMountWindow()

	def ToggleDungeon(self):
		self.interface.ToggleDungeonInfoWindow()

	def DepoIsToggle(self):
		if self.uiDepoIS.IsShow():
			self.uiDepoIS.Close()
		else:
			self.uiDepoIS.Open()
		
	def Show(self):
		global SYSTEMS_WINDOW
		self.__LoadWindow()

		ui.ScriptWindow.Show(self)
		constInfo.SYSTEMS_WINDOW_KAPAT=0
		self.wndSystemsWindow.Show(1)

		if self.isOpenedCostumeWindowWhenClosingInventory and self.wndCostume:
			self.wndCostume.Show()

		# if self.wndBelt:
			# self.wndBelt.Show(self.isOpenedBeltWindowWhenClosingInventory)

	def BindInterfaceClass(self, interface):
		self.interface = interface

	if app.WJ_ENABLE_TRADABLE_ICON:
		def BindWindow(self, wnd):
			self.bindWnds.append(wnd)

	def __LoadWindow(self):
		global SYSTEMS_WINDOW
		if self.isLoaded == 1:
			return

		self.isLoaded = 1
		tooltip = uiToolTip.ToolTip()
		self.toolTip = tooltip
		self.wndSystemsWindow = SystemsWindow(self,tooltip)

		try:
			pyScrLoader = ui.PythonScriptLoader()

			if app.ENABLE_EXTEND_INVEN_SYSTEM:
				pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindowEx.py")
			else:
				if ITEM_MALL_BUTTON_ENABLE:
					pyScrLoader.LoadScriptFile(self, uiScriptLocale.LOCALE_UISCRIPT_PATH + "InventoryWindow.py")
				else:
					pyScrLoader.LoadScriptFile(self, "UIScript/InventoryWindow.py")
		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.LoadObject")

		try:
			wndItem = self.GetChild("ItemSlot")
			wndEquip = self.GetChild("EquipmentSlot")
			self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
			self.wndMoney = self.GetChild("Money")
			self.wndMoneySlot = self.GetChild("Money_Slot")
			# self.mallButton = self.GetChild2("MallButton")
			self.DSSButton = self.GetChild2("DSSButton")
			self.costumeButton = self.GetChild2("CostumeButton")
			self.refreshbutton = self.GetChild2("RefreshButton")

			#expanded money taskbar
			if app.ENABLE_CHEQUE_SYSTEM:
				self.wndCheque = self.GetChild("Cheque")
				self.wndChequeSlot = self.GetChild("Cheque_Slot")
				self.wndChequeIcon = self.GetChild("Cheque_Icon")

			if app.ENABLE_PUNKTY_OSIAGNIEC:
				self.wndPktOsiag = self.GetChild("Pkt_Osiag")
				self.wndPktOsiagSlot = self.GetChild("Pkt_Osiag_Slot")
				self.wndPktOsiagIcon = self.GetChild("Pkt_Osiag_Icon")

			self.wndMoneyIcon = self.GetChild("Money_Icon")
			self.wndMoneyIcon.Hide()
			self.wndMoneySlot.Hide()
			self.wndChequeIcon.Hide()
			self.wndChequeSlot.Hide()
			self.wndPktOsiag.Hide()
			self.wndPktOsiagSlot.Hide()
			self.wndPktOsiagIcon.Hide()

			height = self.GetHeight()
			width = self.GetWidth()
			self.SetSize(width, height - 39)
			self.GetChild("board").SetSize(width, height - 39)
			#end

			self.inventoryTab = []
			for i in xrange(player.INVENTORY_PAGE_COUNT):
				self.inventoryTab.append(self.GetChild("Inventory_Tab_%02d" % (i+1)))

			self.equipmentTab = []
			self.equipmentTab.append(self.GetChild("Equipment_Tab_01"))
			self.equipmentTab.append(self.GetChild("Equipment_Tab_02"))

			if self.costumeButton and not app.ENABLE_COSTUME_SYSTEM:
				self.costumeButton.Hide()
				self.costumeButton.Destroy()
				self.costumeButton = 0

			# Belt Inventory Window
			# self.wndBelt = None

			# if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
				# self.wndBelt = BeltInventoryWindow(self)

		except:
			import exception
			exception.Abort("InventoryWindow.LoadWindow.BindObject")

		## Item
		wndItem.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndItem.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndItem.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndItem.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndItem.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## Equipment
		wndEquip.SetSelectEmptySlotEvent(ui.__mem_func__(self.SelectEmptySlot))
		wndEquip.SetSelectItemSlotEvent(ui.__mem_func__(self.SelectItemSlot))
		wndEquip.SetUnselectItemSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetUseSlotEvent(ui.__mem_func__(self.UseItemSlot))
		wndEquip.SetOverInItemEvent(ui.__mem_func__(self.OverInItem))
		wndEquip.SetOverOutItemEvent(ui.__mem_func__(self.OverOutItem))

		## PickMoneyDialog
		dlgPickMoney = uiPickMoney.PickMoneyDialog()
		dlgPickMoney.LoadDialog()
		dlgPickMoney.Hide()

		## PickItemDialog
		dlgPickItem = uiPickETC.PickETCDialog()
		dlgPickItem.LoadDialog()
		dlgPickItem.Hide()

		## RefineDialog
		self.refineDialog = uiRefine.RefineDialog()
		self.refineDialog.Hide()

		## PickETCDialog
		if app.ENABLE_CHEQUE_SYSTEM:
			dlgPickETC = uiPickETC.PickETCDialog()
			dlgPickETC.LoadDialog()
			dlgPickETC.Hide()

		## AttachMetinDialog
		if app.WJ_ENABLE_TRADABLE_ICON:  
			self.attachMetinDialog = uiAttachMetin.AttachMetinDialog(self)
			self.BindWindow(self.attachMetinDialog)
		else:
			self.attachMetinDialog = uiAttachMetin.AttachMetinDialog()
		self.attachMetinDialog.Hide()
		

		self.refreshbutton.SetEvent(ui.__mem_func__(self.StackItem))

		for i in xrange(player.INVENTORY_PAGE_COUNT):
			self.inventoryTab[i].SetEvent(lambda arg=i: self.SetInventoryPage(arg))
		self.inventoryTab[0].Down()

		self.equipmentTab[0].SetEvent(lambda arg=0: self.SetEquipmentPage(arg))
		self.equipmentTab[1].SetEvent(lambda arg=1: self.SetEquipmentPage(arg))
		self.equipmentTab[0].Down()
		self.equipmentTab[0].Hide()
		self.equipmentTab[1].Hide()

		self.wndItem = wndItem
		self.wndEquip = wndEquip
		self.dlgPickMoney = dlgPickMoney
		self.dlgPickItem = dlgPickItem

		if app.ENABLE_CHEQUE_SYSTEM:
			self.dlgPickETC = dlgPickETC

		# MallButton
		# if self.mallButton:
		# 	self.mallButton.SetEvent(ui.__mem_func__(self.ClickMallButton))

		if self.DSSButton:
			self.DSSButton.SetEvent(ui.__mem_func__(self.ClickDSSButton))

		# Costume Button
		if self.costumeButton:
			self.costumeButton.SetEvent(ui.__mem_func__(self.ClickCostumeButton))
			# self.costumeButton.OnMouseRightButtonDown = ui.__mem_func__(self.ClickFastEquipButton)

		self.wndCostume = None

 		#####
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.listAttachedAcces = []
		if app.ENABLE_AURA_SYSTEM:
			self.listAttachedAuras = []
		## Refresh
		self.SetInventoryPage(0)
		self.SetEquipmentPage(0)
		self.RefreshItemSlot()
		self.RefreshStatus()

		self.SetTop()

	# def SetPktOsiag(self):
	# 	self.pktosiag = self.GetChild("Pkt_Osiag")
	# 	self.pktosiag.SetText("|cFF85d455"+str(localeInfo.NumberToGoldNotText(player.GetPktOsiag())))

	# def OnUpdate(self):
	# 	if app.ENABLE_PUNKTY_OSIAGNIEC:
	# 		self.SetPktOsiag()

	def Destroy(self):
		self.ClearDictionary()

		self.dlgPickMoney.Destroy()
		self.dlgPickMoney = 0

		if app.ENABLE_CHEQUE_SYSTEM:
			self.dlgPickETC.Destroy()
			self.dlgPickETC = 0
		
		if app.ENABLE_PUNKTY_OSIAGNIEC:
			self.wndPktOsiag = 0

		self.refineDialog.Destroy()
		self.refineDialog = 0

		# if app.ENABLE_NEW_STONE_DETACH:
		# 	self.wndStoneDetach.Destroy()
		# 	self.wndStoneDetach = None

		self.attachMetinDialog.Destroy()
		self.attachMetinDialog = 0

		self.tooltipItem = None
		self.wndItem = 0
		self.wndEquip = 0
		self.dlgPickMoney = 0
		self.wndMoney = 0
		self.wndMoneySlot = 0
		self.dlgPickItem.Destroy()
		self.dlgPickItem = 0
		if app.ENABLE_CHEQUE_SYSTEM:
			self.wndCheque = 0
			self.wndChequeSlot = 0
			self.dlgPickETC = 0
		self.questionDialog = None
		# self.mallButton = None
		self.DSSButton = None
		self.DSSButtonEffect = None
		self.interface = None
		if app.WJ_ENABLE_TRADABLE_ICON:
			self.bindWnds = []

		if self.wndCostume:
			self.wndCostume.Destroy()
			self.wndCostume = 0
			
		if self.wndRune:
			self.wndRune.Destroy()
			self.wndRune = 0

		# if self.wndBelt:
			# self.wndBelt.Destroy()
			# self.wndBelt = None

		self.wndSystemsWindow.Destroy()
		self.wndSystemsWindow=None

		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			self.wndAcceCombine = None
			self.wndAcceAbsorption = None

		self.inventoryTab = []
		self.equipmentTab = []
		if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
			self.wndExpandedMoneyBar = None

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.wndPrivateShop:
				self.wndPrivateShop = None
				
			if self.wndPrivateShopSearch:
				self.wndPrivateShopSearch = None

	def OpenWonExchangeWindow(self):
		self.interface.ToggleWonExchangeWindow()

	def StackItem(self):
		net.SendChatPacket("/sort_inventory")

	def Hide(self):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			self.OnCloseQuestionDialog()
			return
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

		if self.wndCostume:
			self.isOpenedCostumeWindowWhenClosingInventory = self.wndCostume.IsShow()
			self.wndCostume.Close()

		# if self.wndBelt:
			# self.isOpenedBeltWindowWhenClosingInventory = self.wndBelt.IsOpeningInventory()
			# print "Is Opening Belt Inven?? ", self.isOpenedBeltWindowWhenClosingInventory
			# self.wndBelt.Close()

		if self.dlgPickMoney:
			self.dlgPickMoney.Close()
			
		if self.dlgPickItem:
			self.dlgPickItem.Close()

		if app.ENABLE_CHEQUE_SYSTEM:
			if self.dlgPickETC:
				self.dlgPickETC.Close()

		if self.wndSystemsWindow:
			self.wndSystemsWindow.Destroy()

		# if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
		# 	if self.wndExpandedMoneyBar:
		# 		self.wndExpandedMoneyBar.Close()

		wndMgr.Hide(self.hWnd)


	def Close(self):
		self.Hide()

	def ClickFastEquipButton(self):
		if self.okno3.IsShow():
			self.okno3.Close()
		else:
			self.okno3.Open()

	if constInfo.ENABLE_EXPANDED_MONEY_TASKBAR:
		def SetExpandedMoneyBar(self, wndBar):
			self.wndExpandedMoneyBar = wndBar
			if self.wndExpandedMoneyBar:
				self.wndMoneySlot = self.wndExpandedMoneyBar.GetMoneySlot()
				self.wndMoney = self.wndExpandedMoneyBar.GetMoney()
				if app.ENABLE_CHEQUE_SYSTEM:
					self.wndMoneyIcon = self.wndExpandedMoneyBar.GetMoneyIcon()
					self.wndMoneyIcon.SetEvent(ui.__mem_func__(self.SellWon))
					if self.wndMoneySlot:
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindow), 0)
					self.wndChequeIcon = self.wndExpandedMoneyBar.GetChequeIcon()
					self.wndChequeIcon.SetEvent(ui.__mem_func__(self.BuyWon))
					self.wndChequeSlot = self.wndExpandedMoneyBar.GetChequeSlot() 
					if self.wndChequeSlot:
						self.wndChequeSlot.SetEvent(ui.__mem_func__(self.OpenWonExchangeWindow), 1)
					self.wndCheque = self.wndExpandedMoneyBar.GetCheque()
				if app.ENABLE_PUNKTY_OSIAGNIEC:
					self.wndPktOsiagIcon = self.wndExpandedMoneyBar.GetPktOsiagIcon()
					self.wndPktOsiag = self.wndExpandedMoneyBar.GetPktOsiag()
				else:
					if self.wndMoneySlot:
						self.wndMoneySlot.SetEvent(ui.__mem_func__(self.OpenPickMoneyDialog))

	def BuyWon(self):
		net.SendWonExchangeBuyPacket(int(1))

	def SellWon(self):
		net.SendWonExchangeSellPacket(int(1))

	def SetInventoryPage(self, page):
		self.inventoryPageIndex = page
		for i in xrange(player.INVENTORY_PAGE_COUNT):
			if i!=page:
				self.inventoryTab[i].SetUp()
		self.RefreshBagSlotWindow()

	def SetEquipmentPage(self, page):
		self.equipmentPageIndex = page
		self.equipmentTab[1-page].SetUp()
		self.RefreshEquipSlotWindow()

	# def ClickMallButton(self):
	# 	print "click_mall_button"
	# 	net.SendChatPacket("/click_mall")

	# DSSButton
	def ClickDSSButton(self):
		print "click_dss_button"
		self.interface.ToggleDragonSoulWindow()

	def UseDSSButtonEffect(self, enable):
		if self.DSSButton:
			DSSButtonEffect = ui.SlotWindow()
			DSSButtonEffect.AddFlag("attach")
			DSSButtonEffect.SetParent(self.DSSButton)
			DSSButtonEffect.SetPosition(3.2, 0)

			DSSButtonEffect.AppendSlot(0, 0, 0, 32, 32)
			DSSButtonEffect.SetRenderSlot(0)
			DSSButtonEffect.RefreshSlot()

			if enable == True:
				DSSButtonEffect.ActivateSlot(0)
				DSSButtonEffect.Show()
			else:
				DSSButtonEffect.DeactivateSlot(0)
				DSSButtonEffect.Hide()
			self.DSSButtonEffect = DSSButtonEffect

	def ClickCostumeButton(self):
		print "Click Costume Button"
		if self.wndCostume:
			if self.wndCostume.IsShow():
				self.wndCostume.Hide()
			else:
				self.wndCostume.Show()
		else:
			self.wndCostume = CostumeWindow(self)
			self.wndCostume.Show()

	def ClickRuneButton(self):
		print "Click Rune Button"
		if self.wndRune:
			if self.wndRune.IsShow(): 
				self.wndRune.Hide()
			else:
				self.wndRune.Show()
		else:
			self.wndRune = RuneWindow(self)
			self.wndRune.Show()

	def OpenPickMoneyDialog(self):

		if mouseModule.mouseController.isAttached():

			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			if player.SLOT_TYPE_SAFEBOX == mouseModule.mouseController.GetAttachedType():

				if player.ITEM_MONEY == mouseModule.mouseController.GetAttachedItemIndex():
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

			mouseModule.mouseController.DeattachObject()

		else:
			curMoney = player.GetElk()

			if curMoney <= 0:
				return

			# self.dlgPickMoney.SetTitleName(localeInfo.PICK_MONEY_TITLE)
			# self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickMoney))
			# self.dlgPickMoney.Open(curMoney)
			# self.dlgPickMoney.SetMax(9)

	if app.ENABLE_CHEQUE_SYSTEM:
		def OnPickMoney(self, money, cheque):
			if cheque > 0 and money > 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.CHEQUE_ONLY_VALUE)
				return
			if cheque > 0:
				mouseModule.mouseController.AttacCheque(self, player.SLOT_TYPE_INVENTORY, cheque)
			else:
				mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)
	else:
		def OnPickMoney(self, money):
			mouseModule.mouseController.AttachMoney(self, player.SLOT_TYPE_INVENTORY, money)

	def OnPickItem(self, count):
		# if app.ENABLE_CHEQUE_SYSTEM:
		# 	itemSlotIndex = self.dlgPickETC.itemGlobalSlotIndex
		# else:
		# 	itemSlotIndex = self.dlgPickMoney.itemGlobalSlotIndex
		# selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		# mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)
		itemSlotIndex = self.dlgPickItem.itemGlobalSlotIndex
		selectedItemVNum = player.GetItemIndex(itemSlotIndex)
		mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, count)

	def __InventoryLocalSlotPosToGlobalSlotPos(self, local):
		if player.IsEquipmentSlot(local) or player.IsCostumeSlot(local) or (app.ENABLE_NEW_EQUIPMENT_SYSTEM and player.IsBeltInventorySlot(local)):
			return local

		return self.inventoryPageIndex*player.INVENTORY_PAGE_SIZE + local

	def GetInventoryPageIndex(self):
		return self.inventoryPageIndex

	if app.WJ_ENABLE_TRADABLE_ICON:
		def RefreshMarkSlots(self, localIndex=None):
			if not self.interface:
				return

			onTopWnd = self.interface.GetOnTopWindow()
			if localIndex:
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(localIndex)
				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(localIndex)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(localIndex)

				return

			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

				if onTopWnd == player.ON_TOP_WND_NONE:
					self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SELL):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_EXCHANGE:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_GIVE):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_PRIVATE_SHOP:
					if player.IsAntiFlagBySlot(slotNumber, item.ITEM_ANTIFLAG_MYSHOP):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

				elif onTopWnd == player.ON_TOP_WND_SAFEBOX:
					if player.IsAntiFlagBySlot(slotNumber, item.ANTIFLAG_SAFEBOX):
						self.wndItem.SetUnusableSlotOnTopWnd(i)
					else:
						self.wndItem.SetUsableSlotOnTopWnd(i)

	def RefreshBagSlotWindow(self):
		if not self.wndItem:
			return
		
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndItem.SetItemSlot

		for i in xrange(player.INVENTORY_PAGE_SIZE):
			slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)

			itemCount = getItemCount(slotNumber)
			if 0 == itemCount:
				self.wndItem.ClearSlot(i)
				continue
			elif 1 == itemCount:
				itemCount = 0

			itemVnum = getItemVNum(slotNumber)
			setItemVNum(i, itemVnum, itemCount)

			if constInfo.IS_AUTO_POTION(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]

				isActivated = 0 != metinSocket[0]

				if isActivated:
					self.wndItem.ActivateSlot(i)
					potionType = 0
					if constInfo.IS_AUTO_POTION_HP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_HP
					elif constInfo.IS_AUTO_POTION_SP(itemVnum):
						potionType = player.AUTO_POTION_TYPE_SP

					usedAmount = int(metinSocket[1])
					totalAmount = int(metinSocket[2])
					player.SetAutoPotionInfo(potionType, isActivated, (totalAmount - usedAmount), totalAmount, self.__InventoryLocalSlotPosToGlobalSlotPos(i))

				else:
					self.wndItem.DeactivateSlot(i)

			if app.WJ_ENABLE_TRADABLE_ICON:
				self.RefreshMarkSlots(i)

			if constInfo.IS_BLEND_ITEM(itemVnum):
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				# blendColor = constInfo.GetBlendColor(itemVnum) ## In case you have the slot highlight system with colors.

				isActivated = 0 != metinSocket[1]
				
				if isActivated:
					self.wndItem.ActivateSlot(i)
				else:
					self.wndItem.DeactivateSlot(i)
					
			itemType = item.GetItemType()
			if app.__ENABLE_TOGGLE_ITEMS__ and itemType == item.ITEM_TYPE_TOGGLE:
				metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				isActivated = 0 != metinSocket[2]
				if isActivated:
					self.wndItem.ActivateSlot(i)
				else:
					self.wndItem.DeactivateSlot(i)

			# elif (itemVnum >= 51821 and itemVnum <= 51826):
				# metinSocket = [player.GetItemMetinSocket(slotNumber, j) for j in xrange(player.METIN_SOCKET_MAX_NUM)]
				# isActivated = 0 != metinSocket[1]
				# if isActivated:
					# self.wndItem.ActivateSlot(i)
				# else:
					# self.wndItem.DeactivateSlot(i)

			if app.ENABLE_AURA_SYSTEM:
				slotNumberChecked = 0
				for j in xrange(aura.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = aura.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							self.wndItem.ActivateSlotEffect(i, (36.00 / 255.0), (222.00 / 255.0), (3.00 / 255.0), 1.0)
							if not slotNumber in self.listAttachedAuras:
								self.listAttachedAuras.append(slotNumber)
							
							slotNumberChecked = 1
					else:
						if slotNumber in self.listAttachedAuras and not slotNumberChecked:
							self.wndItem.DeactivateSlotEffect(i)
							self.listAttachedAuras.remove(slotNumber)

			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				slotNumberChecked = 0
				if not constInfo.IS_AUTO_POTION(itemVnum) and not constInfo.IS_BLEND_ITEM(itemVnum) and not itemType == item.ITEM_TYPE_TOGGLE:
					if app.ENABLE_HIGHLIGHT_NEW_ITEM:
						if not slotNumber in self.liHighlightedItems:
							self.wndItem.DeactivateSlot(i)
					else:
						self.wndItem.DeactivateSlot(i)

				for j in xrange(acce.WINDOW_MAX_MATERIALS):
					(isHere, iCell) = acce.GetAttachedItem(j)
					if isHere:
						if iCell == slotNumber:
							self.wndItem.ActivateSlot(i, (36.00 / 255.0), (222.00 / 255.0), (3.00 / 255.0), 1.0)
							if not slotNumber in self.listAttachedAcces:
								self.listAttachedAcces.append(slotNumber)

							slotNumberChecked = 1
					else:
						if slotNumber in self.listAttachedAcces and not slotNumberChecked:
							self.wndItem.DeactivateSlot(i)
							self.listAttachedAcces.remove(slotNumber)

			elif app.ENABLE_HIGHLIGHT_NEW_ITEM and not constInfo.IS_AUTO_POTION(itemVnum) and not constInfo.IS_BLEND_ITEM(itemVnum) and not itemType == item.ITEM_TYPE_TOGGLE:
				if not slotNumber in self.liHighlightedItems:
					self.wndItem.DeactivateSlot(i)

		if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
			if self.wndPrivateShop and self.wndPrivateShop.IsShow():
				self.wndPrivateShop.RefreshLockedSlot()

		self.wndItem.RefreshSlot()
		if app.ENABLE_HIGHLIGHT_NEW_ITEM:
			self.__RefreshHighlights()

		# if self.wndBelt:
			# self.wndBelt.RefreshSlot()

		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)

	if app.ENABLE_HIGHLIGHT_NEW_ITEM:
		def HighlightSlot(self, slot):
			if not slot in self.liHighlightedItems:
				self.liHighlightedItems.append(slot)

		def __RefreshHighlights(self):
			for i in xrange(player.INVENTORY_PAGE_SIZE):
				slotNumber = self.__InventoryLocalSlotPosToGlobalSlotPos(i)
				if slotNumber in self.liHighlightedItems:
					self.wndItem.ActivateSlot(i)

	def RefreshEquipSlotWindow(self):
		getItemVNum=player.GetItemIndex
		getItemCount=player.GetItemCount
		setItemVNum=self.wndEquip.SetItemSlot
		for i in xrange(player.EQUIPMENT_PAGE_COUNT):
			slotNumber = player.EQUIPMENT_SLOT_START + i
			itemCount = getItemCount(slotNumber)
			if itemCount <= 1:
				itemCount = 0
			setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)

		if app.ENABLE_NEW_EQUIPMENT_SYSTEM:
			for i in xrange(player.NEW_EQUIPMENT_SLOT_COUNT):
				slotNumber = player.NEW_EQUIPMENT_SLOT_START + i
				itemCount = getItemCount(slotNumber)
				if itemCount <= 1:
					itemCount = 0
				setItemVNum(slotNumber, getItemVNum(slotNumber), itemCount)
				# print "ENABLE_NEW_EQUIPMENT_SYSTEM", slotNumber, itemCount, getItemVNum(slotNumber)

		self.wndEquip.RefreshSlot()

		if self.wndCostume:
			self.wndCostume.RefreshCostumeSlot()
			
		if self.wndRune:
			self.wndRune.RefreshEquipSlotWindow()

	def RefreshItemSlot(self):
		self.RefreshBagSlotWindow()
		self.RefreshEquipSlotWindow()

	def RefreshStatus(self):
		money = player.GetElk()
		self.wndMoney.SetText("|cFFffd74c"+localeInfo.NumberToGoldNotText(money))
		if app.ENABLE_CHEQUE_SYSTEM:
			cheque = player.GetCheque()
			self.wndCheque.SetText("|cFFb8b8b8"+localeInfo.NumberToGoldNotText(cheque))

	def SetItemToolTip(self, tooltipItem):
		self.tooltipItem = tooltipItem

	def SellItem(self):
		if self.sellingSlotitemIndex == player.GetItemIndex(self.sellingSlotNumber):
			if self.sellingSlotitemCount == player.GetItemCount(self.sellingSlotNumber):
				net.SendShopSellPacketNew(self.sellingSlotNumber, self.questionDialog.count, player.INVENTORY)
				snd.PlaySound("sound/ui/money.wav")
		self.OnCloseQuestionDialog()

	def OnDetachMetinFromItem(self):
		if None == self.questionDialog:
			return

		#net.SendItemUseToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.__SendUseItemToItemPacket(self.questionDialog.sourcePos, self.questionDialog.targetPos)
		self.OnCloseQuestionDialog()

	def OnCloseQuestionDialog(self):
		if not self.questionDialog:
			return

		self.questionDialog.Close()
		self.questionDialog = None
		constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(0)

	## Slot Event
	def SelectEmptySlot(self, selectedSlotPos):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		#BONUS SWITCHER
		if constInfo.IS_BONUS_CHANGER == True:
			return
		#END OF BONUS SWITCHER

		selectedSlotPos = self.__InventoryLocalSlotPosToGlobalSlotPos(selectedSlotPos)

		if mouseModule.mouseController.isAttached():

			attachedSlotType = mouseModule.mouseController.GetAttachedType()

			if player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType:
				# chat.AppendChat(chat.CHAT_TYPE_INFO, str(attachedSlotType)+" - "+str(selectedSlotPos))
				mouseModule.mouseController.DeattachObject()
				return

			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemCount = mouseModule.mouseController.GetAttachedItemCount()
			attachedItemIndex = mouseModule.mouseController.GetAttachedItemIndex()

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_DRAGON_SOUL_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
				itemCount = player.GetItemCount(attachedSlotPos)
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				# self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)
				if self.dlgPickItem and self.dlgPickItem.IsSplitAll():
					net.SendChatPacket("/split_items %d %d %d" % (attachedSlotPos, attachedCount, selectedSlotPos))
					self.dlgPickItem.SplitClear()
				else:
					self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				#@fixme011 BEGIN (block ds equip)
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mouseModule.mouseController.DeattachObject()
					return
				#@fixme011 END

				# itemCount = player.GetItemCount(attachedSlotPos)
				# attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				# self.__SendMoveItemPacket(attachedSlotPos, selectedSlotPos, attachedCount)

				if item.IsRefineScroll(attachedItemIndex):
					self.wndItem.SetUseMode(False)

			elif app.ENABLE_SWITCHBOT and player.SLOT_TYPE_SWITCHBOT == attachedSlotType:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.SWITCHBOT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)
	
			elif player.SLOT_TYPE_PRIVATE_SHOP == attachedSlotType:
				if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
					if not uiPrivateShopBuilder.IsBuildingPrivateShop():
						self.wndPrivateShop.SendItemCheckoutPacket(attachedSlotPos, selectedSlotPos)
						mouseModule.mouseController.DeattachObject()
						return
						
				mouseModule.mouseController.RunCallBack("INVENTORY")
				
			elif player.SLOT_TYPE_BUFF_EQUIPMENT == attachedSlotType and app.ENABLE_ASLAN_BUFF_NPC_SYSTEM:
				attachedCount = mouseModule.mouseController.GetAttachedItemCount()
				net.SendItemMovePacket(player.BUFF_EQUIPMENT, attachedSlotPos, player.INVENTORY, selectedSlotPos, attachedCount)

			elif player.SLOT_TYPE_SHOP == attachedSlotType:
				net.SendShopBuyPacket(attachedSlotPos)

			elif player.SLOT_TYPE_SAFEBOX == attachedSlotType:

				if player.ITEM_MONEY == attachedItemIndex:
					net.SendSafeboxWithdrawMoneyPacket(mouseModule.mouseController.GetAttachedItemCount())
					snd.PlaySound("sound/ui/money.wav")

				else:
					net.SendSafeboxCheckoutPacket(attachedSlotPos, selectedSlotPos)

			elif player.SLOT_TYPE_MALL == attachedSlotType:
				net.SendMallCheckoutPacket(attachedSlotPos, selectedSlotPos)

			mouseModule.mouseController.DeattachObject()

	#BONUS SWITCHER
	def IsChanger(self, itemVnum):
		changerList = [71084] #JeÅ¼eli masz wiÄ™cej zmianek to po przecinku dopisz.
		if itemVnum in changerList:
			return True
		
		return False
	#END OF BONUS SWITCHER

	def SelectItemSlot(self, itemSlotIndex):
		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS() == 1:
			return

		#BONUS SWITCHER
		if constInfo.IS_BONUS_CHANGER == True:
			return
		#END OF BONUS SWITCHER

		itemSlotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(itemSlotIndex)

		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedItemVID = mouseModule.mouseController.GetAttachedItemIndex()

			#BONUS SWITCHER
			if self.IsChanger(attachedItemVID) and not player.IsEquipmentSlot(itemSlotIndex):
				itemVnum = player.GetItemIndex(itemSlotIndex)
				item.SelectItem(itemVnum)
				if item.GetItemType() == item.ITEM_TYPE_WEAPON or item.GetItemType() == item.ITEM_TYPE_ARMOR:
					self.interface.AddToBonusChange(itemSlotIndex, attachedSlotPos)
					mouseModule.mouseController.DeattachObject()
					# self.wndItem[itemSlotIndex].SetChangerImage()
					return
			#END OF BONUS SWITCHER

			if player.SLOT_TYPE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_SKILL_BOOK_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_UPGRADE_ITEMS_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_STONE_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_BOX_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_EFSUN_INVENTORY == attachedSlotType or\
				player.SLOT_TYPE_CICEK_INVENTORY == attachedSlotType:
				#@fixme011 BEGIN (block ds equip)
				attachedInvenType = player.SlotTypeToInvenType(attachedSlotType)
				if player.IsDSEquipmentSlot(attachedInvenType, attachedSlotPos):
					mouseModule.mouseController.DeattachObject()
					return
				#@fixme011 END
				self.__DropSrcItemToDestItemInInventory(attachedItemVID, attachedSlotPos, itemSlotIndex)

			mouseModule.mouseController.DeattachObject()

		else:

			curCursorNum = app.GetCursor()
			if app.SELL == curCursorNum:
				self.__SellItem(itemSlotIndex)

			elif app.BUY == curCursorNum:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.SHOP_BUY_INFO)

			elif app.IsPressed(app.DIK_LALT):
				link = player.GetItemLink(itemSlotIndex)
				ime.PasteString(link)

			elif app.IsPressed(app.DIK_LSHIFT):
				itemCount = player.GetItemCount(itemSlotIndex)

				# if app.ENABLE_CHEQUE_SYSTEM:
				# 	if itemCount > 1:
				# 		self.dlgPickETC.SetTitleName(localeInfo.PICK_ITEM_TITLE)
				# 		self.dlgPickETC.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
				# 		self.dlgPickETC.Open(itemCount)
				# 		self.dlgPickETC.itemGlobalSlotIndex = itemSlotIndex
				# else:
				# 	if itemCount > 1:
				# 		self.dlgPickMoney.SetTitleName(localeInfo.PICK_ITEM_TITLE)
				# 		self.dlgPickMoney.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
				# 		self.dlgPickMoney.Open(itemCount)
				# 		self.dlgPickMoney.itemGlobalSlotIndex = itemSlotIndex
				if itemCount > 1:
					self.dlgPickItem.SetTitleName(localeInfo.PICK_ITEM_TITLE)
					self.dlgPickItem.SetAcceptEvent(ui.__mem_func__(self.OnPickItem))
					self.dlgPickItem.Open(itemCount)
					self.dlgPickItem.itemGlobalSlotIndex = itemSlotIndex
				#else:
					#selectedItemVNum = player.GetItemIndex(itemSlotIndex)
					#mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum)

			elif app.IsPressed(app.DIK_LCONTROL):
				itemIndex = player.GetItemIndex(itemSlotIndex)

				if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
					if self.wndPrivateShop and self.wndPrivateShop.IsShow():
						self.wndPrivateShop.AttachItemToPrivateShop(itemSlotIndex, player.SLOT_TYPE_INVENTORY)
						return
						
					if self.wndPrivateShopSearch and self.wndPrivateShopSearch.IsShow():
						self.wndPrivateShopSearch.SelectItem(itemIndex)
						return

				if True == item.CanAddToQuickSlotItem(itemIndex):
					player.RequestAddToEmptyLocalQuickSlot(player.SLOT_TYPE_INVENTORY, itemSlotIndex)
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.QUICKSLOT_REGISTER_DISABLE_ITEM)

			else:
				selectedItemVNum = player.GetItemIndex(itemSlotIndex)
				itemCount = player.GetItemCount(itemSlotIndex)
				if app.ENABLE_EXTENDED_BLEND:
					if self.__CanAttachThisItem(selectedItemVNum, itemSlotIndex):
						mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)
				else:
					mouseModule.mouseController.AttachObject(self, player.SLOT_TYPE_INVENTORY, itemSlotIndex, selectedItemVNum, itemCount)

				if self.__IsUsableItemToItem(selectedItemVNum, itemSlotIndex):
					self.wndItem.SetUseMode(True)
				else:
					self.wndItem.SetUseMode(False)

				snd.PlaySound("sound/ui/pick.wav")

	def __DropSrcItemToDestItemInInventory(self, srcItemVID, srcItemSlotPos, dstItemSlotPos):
		if srcItemSlotPos == dstItemSlotPos:
			return

		item.SelectItem(player.GetItemIndex(dstItemSlotPos))
		destItemType = item.GetItemType()
		destItemSubType = item.GetItemSubType()
		destItemVnum = player.GetItemIndex(dstItemSlotPos)

		# cyh itemseal 2013 11 08
		if app.ENABLE_SOULBIND_SYSTEM and item.IsSealScroll(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.IsRefineScroll(srcItemVID):
			if item.GetUseType(srcItemVID) in ALLOWED_TYPES and\
			item.GetUseType(player.GetItemIndex(dstItemSlotPos))\
			in ALLOWED_TYPES:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
			else:
				self.RefineItem(srcItemSlotPos, dstItemSlotPos)
				self.wndItem.SetUseMode(False)

		elif item.GetUseType(srcItemVID) in ALLOWED_TYPES and\
			item.GetUseType(player.GetItemIndex(dstItemSlotPos))\
			in ALLOWED_TYPES:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

		# elif item.IsMetin(srcItemVID):
		elif item.IsMetin(srcItemVID) and not item.IsMetin(player.GetItemIndex(dstItemSlotPos)):
			self.AttachMetinToItem(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) == "USE_PUT_INTO_ACCESSORY_SOCKET" and not self.__CanPutAccessorySocket(dstItemSlotPos, srcItemVID):
			self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)

		elif srcItemVID == 39046 and destItemSubType == item.COSTUME_TYPE_ACCE or srcItemVID == 90000 and destItemSubType == item.COSTUME_TYPE_ACCE:
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsDetachScroll(srcItemVID):
			self.DetachMetinFromItem(srcItemSlotPos, dstItemSlotPos)

		elif item.IsKey(srcItemVID):
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif (player.GetItemFlags(srcItemSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		elif item.GetUseType(srcItemVID) in self.USE_TYPE_TUPLE:
			self.__SendUseItemToItemPacket(srcItemSlotPos, dstItemSlotPos)

		else:
			#snd.PlaySound("sound/ui/drop.wav")

			if player.IsEquipmentSlot(dstItemSlotPos):

				if item.IsEquipmentVID(srcItemVID):
					self.__UseItem(srcItemSlotPos)

			else:
				self.__SendMoveItemPacket(srcItemSlotPos, dstItemSlotPos, 0)
				#net.SendItemMovePacket(srcItemSlotPos, dstItemSlotPos, 0)

	def __SellItem(self, itemSlotPos):
		if not player.IsEquipmentSlot(itemSlotPos):
			self.sellingSlotNumber = itemSlotPos
			itemIndex = player.GetItemIndex(itemSlotPos)
			itemCount = player.GetItemCount(itemSlotPos)


			self.sellingSlotitemIndex = itemIndex
			self.sellingSlotitemCount = itemCount

			item.SelectItem(itemIndex)
			## 20140220
			if item.IsAntiFlag(item.ANTIFLAG_SELL):
				popup = uiCommon.PopupDialog()
				popup.SetText(localeInfo.SHOP_CANNOT_SELL_ITEM)
				popup.SetAcceptEvent(self.__OnClosePopupDialog)
				popup.Open()
				self.popup = popup
				return

			itemPrice = item.GetISellItemPrice()

			if item.Is1GoldItem():
				itemPrice = itemCount / itemPrice / 2
			else:
				itemPrice = itemPrice * itemCount / 2

			item.GetItemName(itemIndex)
			itemName = item.GetItemName()

			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.DO_YOU_SELL_ITEM(itemName, itemCount, itemPrice))
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.SellItem))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
			self.questionDialog.Open()
			self.questionDialog.count = itemCount

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

	def __OnClosePopupDialog(self):
		self.pop = None

	def RefineItem(self, scrollSlotPos, targetSlotPos):

		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if player.REFINE_OK != player.CanRefine(scrollIndex, targetSlotPos):
			return

		if app.ENABLE_REFINE_RENEWAL:
			constInfo.AUTO_REFINE_TYPE = 1
			constInfo.AUTO_REFINE_DATA["ITEM"][0] = scrollSlotPos
			constInfo.AUTO_REFINE_DATA["ITEM"][1] = targetSlotPos

		###########################################################
		self.__SendUseItemToItemPacket(scrollSlotPos, targetSlotPos)
		#net.SendItemUseToItemPacket(scrollSlotPos, targetSlotPos)
		return
		###########################################################

		###########################################################
		#net.SendRequestRefineInfoPacket(targetSlotPos)
		#return
		###########################################################

		result = player.CanRefine(scrollIndex, targetSlotPos)

		if player.REFINE_ALREADY_MAX_SOCKET_COUNT == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_MORE_SOCKET)

		elif player.REFINE_NEED_MORE_GOOD_SCROLL == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NEED_BETTER_SCROLL)

		elif player.REFINE_CANT_MAKE_SOCKET_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_SOCKET_DISABLE_ITEM)

		elif player.REFINE_NOT_NEXT_GRADE_ITEM == result:
			#snd.PlaySound("sound/ui/jaeryun_fail.wav")
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_UPGRADE_DISABLE_ITEM)

		elif player.REFINE_CANT_REFINE_METIN_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.REFINE_OK != result:
			return

		self.refineDialog.Open(scrollSlotPos, targetSlotPos)

	if app.ENABLE_NEW_STONE_DETACH:
		def OpenNewStoneDetachWindow(self, scrollSlotPos, targetSlotPos):
			if not self.wndStoneDetach:
				self.wndStoneDetach = uistonedetach.StoneDetachWindow()

			self.wndStoneDetach.Open(scrollSlotPos, targetSlotPos)

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		def BindPrivateShopClass(self, window):
			self.wndPrivateShop = window
			
		def BindPrivateShopSearchClass(self, window):
			self.wndPrivateShopSearch = window	

	def DetachMetinFromItem(self, scrollSlotPos, targetSlotPos):
		scrollIndex = player.GetItemIndex(scrollSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		if not player.CanDetach(scrollIndex, targetSlotPos):
			if app.ENABLE_ACCE_COSTUME_SYSTEM:
				item.SelectItem(scrollIndex)
				if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
					chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mo¿esz tego zrobiæ.")
				else:
					chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			else:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_METIN_INSEPARABLE_ITEM)
			return

		self.questionDialog = uiCommon.QuestionDialog()
		self.questionDialog.SetText(localeInfo.REFINE_DO_YOU_SEPARATE_METIN)
		
		item.SelectItem(targetIndex)
		if item.GetItemType() == item.ITEM_TYPE_COSTUME and item.GetItemSubType() == item.COSTUME_TYPE_ACCE:
			item.SelectItem(scrollIndex)
			if item.GetValue(0) == acce.CLEAN_ATTR_VALUE0:
				self.questionDialog.SetText("Czy chcesz wyczyœciæ bonusy?")
		else:
			self.OpenNewStoneDetachWindow(scrollSlotPos, targetSlotPos)
			return

		self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDetachMetinFromItem))
		self.questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		self.questionDialog.Open()
		self.questionDialog.sourcePos = scrollSlotPos
		self.questionDialog.targetPos = targetSlotPos

	def AttachMetinToItem(self, metinSlotPos, targetSlotPos):
		metinIndex = player.GetItemIndex(metinSlotPos)
		targetIndex = player.GetItemIndex(targetSlotPos)

		item.SelectItem(metinIndex)
		itemName = item.GetItemName()

		result = player.CanAttachMetin(metinIndex, targetSlotPos)

		if player.ATTACH_METIN_NOT_MATCHABLE_ITEM == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_CAN_NOT_ATTACH(itemName))

		if player.ATTACH_METIN_NO_MATCHABLE_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_SOCKET(itemName))

		elif player.ATTACH_METIN_NOT_EXIST_GOLD_SOCKET == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_NO_GOLD_SOCKET(itemName))

		elif player.ATTACH_METIN_CANT_ATTACH_TO_EQUIPMENT == result:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.REFINE_FAILURE_EQUIP_ITEM)

		if player.ATTACH_METIN_OK != result:
			return

		self.attachMetinDialog.Open(metinSlotPos, targetSlotPos)



	def OverOutItem(self):
		self.wndItem.SetUsableItem(False)
		if None != self.tooltipItem:
			self.tooltipItem.HideToolTip()

	def OverInItem(self, overSlotPos):
		overSlotPosGlobal = self.__InventoryLocalSlotPosToGlobalSlotPos(overSlotPos)
		self.wndItem.SetUsableItem(False)


		if app.ENABLE_HIGHLIGHT_NEW_ITEM and overSlotPosGlobal in self.liHighlightedItems:
			self.liHighlightedItems.remove(overSlotPosGlobal)
			self.wndItem.DeactivateSlot(overSlotPos)


		if mouseModule.mouseController.isAttached():
			attachedItemType = mouseModule.mouseController.GetAttachedType()
			if player.SLOT_TYPE_INVENTORY == attachedItemType or player.SLOT_TYPE_STONE_INVENTORY == attachedItemType or player.SLOT_TYPE_EFSUN_INVENTORY == attachedItemType:

				attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
				attachedItemVNum = mouseModule.mouseController.GetAttachedItemIndex()

				if attachedItemVNum==player.ITEM_MONEY: # @fixme005
					pass
				elif self.__CanUseSrcItemToDstItem(attachedItemVNum, attachedSlotPos, overSlotPosGlobal):
					self.wndItem.SetUsableItem(True)
					self.wndItem.SetUseMode(True)
					self.ShowToolTip(overSlotPosGlobal)
					return

		self.ShowToolTip(overSlotPosGlobal)


	def __IsUsableItemToItem(self, srcItemVNum, srcSlotPos):
		if item.IsRefineScroll(srcItemVNum):
			return True
		elif item.IsMetin(srcItemVNum):
			return True
		elif srcItemVNum == 39046 or srcItemVNum == 90000:
			return True
		elif item.IsDetachScroll(srcItemVNum):
			return True
		elif item.IsKey(srcItemVNum):
			return True
		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True
		else:
			if item.GetUseType(srcItemVNum) in self.USE_TYPE_TUPLE:
				return True

		return False

	def __CanUseSrcItemToDstItem(self, srcItemVNum, srcSlotPos, dstSlotPos):
		if srcSlotPos == dstSlotPos and not item.IsMetin(srcItemVNum):
			return False

		item.SelectItem(player.GetItemIndex(dstSlotPos))
		destItemType = item.GetItemType()
		destItemSubType = item.GetItemSubType()

		if item.IsRefineScroll(srcItemVNum):
			if player.REFINE_OK == player.CanRefine(srcItemVNum, dstSlotPos):
				return True
		elif item.IsMetin(srcItemVNum):
			if player.ATTACH_METIN_OK == player.CanAttachMetin(srcItemVNum, dstSlotPos) or (item.IsMetin(player.GetItemIndex(dstSlotPos)) and player.GetItemIndex(dstSlotPos) == srcItemVNum):
				return True
		elif srcItemVNum == 39046 and destItemSubType == item.COSTUME_TYPE_ACCE or srcItemVNum == 90000 and destItemSubType == item.COSTUME_TYPE_ACCE:
			return True
		elif item.IsDetachScroll(srcItemVNum):
			if player.DETACH_METIN_OK == player.CanDetach(srcItemVNum, dstSlotPos):
				return True
		elif item.IsKey(srcItemVNum):
			if player.CanUnlock(srcItemVNum, dstSlotPos):
				return True

		if (srcItemVNum == 33052 or srcItemVNum == 33053) and player.GetItemIndex(dstSlotPos) >= 41001 and player.GetItemIndex(dstSlotPos) <= 41999:
			return True
			
		elif (srcItemVNum == 33054 or srcItemVNum == 33055) and player.GetItemIndex(dstSlotPos) >= 45001 and player.GetItemIndex(dstSlotPos) <= 45999:
			return True
			
		elif (srcItemVNum == 33056 or srcItemVNum == 33057) and player.GetItemIndex(dstSlotPos) >= 40100 and player.GetItemIndex(dstSlotPos) <= 40999:
			return True

		elif (srcItemVNum == 33051 or srcItemVNum == 33050) and player.GetItemIndex(dstSlotPos) >= 53001 and player.GetItemIndex(dstSlotPos) <= 53999:
			return True		
			
		elif srcItemVNum in [33050,33051,33052,33053,33054,33055,33056,33057]:
			return False

		elif (player.GetItemFlags(srcSlotPos) & ITEM_FLAG_APPLICABLE) == ITEM_FLAG_APPLICABLE:
			return True

		else:
			useType=item.GetUseType(srcItemVNum)

			if "USE_CLEAN_SOCKET" == useType:
				if self.__CanCleanBrokenMetinStone(dstSlotPos):
					return True
			elif "USE_CHANGE_ATTRIBUTE" == useType:
				if self.__CanChangeItemAttrList(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE2" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ATTRIBUTE_SPECIAL_1" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_NEW_BONUS" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_EXTRA_BONUS" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_IS_BONUS" == useType:
				if self.__CanAddItemAttr(dstSlotPos):
					return True
			elif "USE_ADD_ACCESSORY_SOCKET" == useType:
				if self.__CanAddAccessorySocket(dstSlotPos):
					return True
			elif "USE_PUT_INTO_ACCESSORY_SOCKET" == useType:
				if self.__CanPutAccessorySocket(dstSlotPos, srcItemVNum):
					return True;
			elif "USE_PUT_INTO_BELT_SOCKET" == useType:
				dstItemVNum = player.GetItemIndex(dstSlotPos)
				print "USE_PUT_INTO_BELT_SOCKET", srcItemVNum, dstItemVNum

				item.SelectItem(dstItemVNum)

				if item.ITEM_TYPE_BELT == item.GetItemType():
					return True
			elif app.ENABLE_USE_COSTUME_ATTR and "USE_CHANGE_COSTUME_ATTR" == useType:
				if self.__CanChangeCostumeAttrList(dstSlotPos):
					return True
			elif app.ENABLE_USE_COSTUME_ATTR and "USE_RESET_COSTUME_ATTR" == useType:
				if self.__CanResetCostumeAttr(dstSlotPos):
					return True

		return False

	def __CanCleanBrokenMetinStone(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.ITEM_TYPE_WEAPON != item.GetItemType():
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemMetinSocket(dstSlotPos, i) == constInfo.ERROR_METIN_STONE:
				return True

		return False

	def __CanChangeItemAttrList(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_BELT):
			return False

		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				return True

		return False

	if app.ENABLE_USE_COSTUME_ATTR:
		def __CanChangeCostumeAttrList(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return True

			return False

		def __CanResetCostumeAttr(self, dstSlotPos):
			dstItemVNum = player.GetItemIndex(dstSlotPos)
			if dstItemVNum == 0:
				return False

			item.SelectItem(dstItemVNum)

			if item.GetItemType() != item.ITEM_TYPE_COSTUME:
				return False

			for i in xrange(player.METIN_SOCKET_MAX_NUM):
				if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
					return True

			return False

	def __CanPutAccessorySocket(self, dstSlotPos, mtrlVnum):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		if mtrlVnum != constInfo.GET_ACCESSORY_MATERIAL_VNUM(dstItemVNum, item.GetItemSubType()):
			return False

		if curCount>=maxCount:
			return False

		return True

	def __CanAddAccessorySocket(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if item.GetItemType() != item.ITEM_TYPE_ARMOR:
			return False

		if not item.GetItemSubType() in (item.ARMOR_WRIST, item.ARMOR_NECK, item.ARMOR_EAR):
			return False

		curCount = player.GetItemMetinSocket(dstSlotPos, 0)
		maxCount = player.GetItemMetinSocket(dstSlotPos, 1)

		ACCESSORY_SOCKET_MAX_SIZE = 3
		if maxCount >= ACCESSORY_SOCKET_MAX_SIZE:
			return False

		return True

	def __CanAddItemAttr(self, dstSlotPos):
		dstItemVNum = player.GetItemIndex(dstSlotPos)
		if dstItemVNum == 0:
			return False

		item.SelectItem(dstItemVNum)

		if not item.GetItemType() in (item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_BELT):
			return False

		attrCount = 0
		for i in xrange(player.METIN_SOCKET_MAX_NUM):
			if player.GetItemAttribute(dstSlotPos, i)[0] != 0:
				attrCount += 1

		if attrCount<4:
			return True

		return False

	def ShowToolTip(self, slotIndex):
		if None != self.tooltipItem:
			self.tooltipItem.SetInventoryItem(slotIndex)

	def OnTop(self):
		if None != self.tooltipItem:
			self.tooltipItem.SetTop()

		if app.WJ_ENABLE_TRADABLE_ICON:
			map(lambda wnd:wnd.RefreshLockedSlot(), self.bindWnds)
			self.RefreshMarkSlots()

	def OnPressEscapeKey(self):
		self.Close()
		return True

	#test
	if app.WJ_SPLIT_INVENTORY_SYSTEM:
		def GetEmptyItemPos(self, type, itemHeight):
			start_val = start_val_table[type - 1] # type - 1 because skillbook type is 1 and the table start by 0.
			end_val = end_val_table[type - 1]

			GetBlockedSlots = lambda slot, size: [slot+(round*3) for round in xrange(size)] 
			blocked_slots = [element for sublist in [GetBlockedSlots(slot, item.GetItemSize(item.SelectItem(player.GetItemIndex(slot)))[1]) for slot in xrange(start_val, end_val) if player.isItem(slot)] for element in sublist] 
			if itemHeight > 1: 
				[[blocked_slots.append(slot) for slot in range(start_val, end_val/round-(itemHeight-1)*3, start_val/round) if not slot in blocked_slots] for round in xrange(1,3)] 
			free_slots = [slot for slot in xrange(start_val, end_val) if not slot in blocked_slots and not True in [e in blocked_slots for e in [slot+(round*3) for round in xrange(itemHeight)]]] 
			return [free_slots, -1][len(free_slots) == 0]

		def SetPage(self, val):
			calc = val/45
			page_table = [2,1,0]
			self.interface.wndExtendedInventory.SetInventoryPage(page_table[calc])

	def UseItemSlot(self, slotIndex):
		curCursorNum = app.GetCursor()
		if app.SELL == curCursorNum:
			return

		test = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		itemVnum = player.GetItemIndex(test)
		itemCount = player.GetItemCount(test)
		item.SelectItem(itemVnum)
		specialinvtype = item.GetSpecialInvType()

		(w, h) = item.GetItemSize()
		
		emptyInvenSlots = self.GetEmptyItemPos(specialinvtype, h)
		end_val = end_val_table[specialinvtype - 1]
		page_calculation = end_val-emptyInvenSlots[0]

		if app.WJ_SPLIT_INVENTORY_SYSTEM:
			if specialinvtype != 0:
				if not app.IsPressed(app.DIK_LCONTROL):
					if not self.interface.wndRemoveItem.IsShow() and not self.interface.wndOdlamki.IsShow():
						if app.IsPressed(app.DIK_LSHIFT):
							if mouseModule.mouseController.isAttached():
								mouseModule.mouseController.DeattachObject()

							if constInfo.enablefastopenopen == 1:
								if constInfo.EXTENDED_INVENTORY_IS_OPEN == 0:
									self.interface.ToggleExtendedInventoryWindow()

							if emptyInvenSlots == -1:
								chat.AppendChat(chat.CHAT_TYPE_INFO, "Dodatkowy ekwipunek ("+SPECIAL_INV_NAMES[specialinvtype - 1]+") jest pe³ny!")
								return
							else:
								self.__SendMoveItemPacket(test, emptyInvenSlots[0], itemCount)
								if constInfo.enablefastopenopen == 1:
									self.interface.wndExtendedInventory.SetInventoryType(specialinvtype-1)
									self.SetPage(page_calculation)

							# chat.AppendChat(chat.CHAT_TYPE_INFO, "itemvnum:"+str(itemVnum)+", slotindex:"+str(slotIndex) + ", emptyslot:"+str(emptyInvenSlots[0])+", localslottoglobalslot:"+str(test)+", count:"+str(itemCount))

		if constInfo.GET_ITEM_QUESTION_DIALOG_STATUS():
			return

		#BONUS SWITCHER
		if constInfo.IS_BONUS_CHANGER:
			return
		#END OF BONUS SWITCHER

		slotIndex = self.__InventoryLocalSlotPosToGlobalSlotPos(slotIndex)
		
		if app.IsPressed(app.DIK_LSHIFT) and self.interface.wndRemoveItem.IsShow():
			self.interface.wndRemoveItem.AppendSlot(player.INVENTORY, slotIndex)
			return

		if app.ENABLE_ODLAMKI_SYSTEM:
			if app.IsPressed(app.DIK_LSHIFT) and self.interface.wndOdlamki.IsShow():
				itemType = item.GetItemType()
				if not item.ITEM_TYPE_METIN == itemType:
					chat.AppendChat(chat.CHAT_TYPE_INFO, "Mo¿esz w³o¿yæ tylko kamienie dusz!")
					return
				else:
					self.interface.wndOdlamki.AppendSlot(player.INVENTORY, slotIndex)
					return
			
		# if app.IsPressed(app.DIK_LSHIFT) and not exchange.isTrading() and not self.interface.wndSafebox.IsShow():
		# 	net.SendInstantChestOpenPacket(player.INVENTORY, slotIndex)
		# 	return

		# if app.IsPressed(app.DIK_LSHIFT):
		# 	net.SendChatPacket("/fast_opener %d" % (slotIndex))
		# 	return

		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			if self.wndDragonSoulRefine.IsShow():
				ItemVNum = player.GetItemIndex(slotIndex)
				if ItemVNum == 100300 or ItemVNum == 100400 or ItemVNum == 100500:
					self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), player.GetItemCount(slotIndex))
				else:
					self.wndDragonSoulRefine.AutoSetItem((player.INVENTORY, slotIndex), 1)
				return
		if app.ITEM_CHECKINOUT_UPDATE:
			if exchange.isTrading() and slotIndex < player.EQUIPMENT_SLOT_START:
				net.SendExchangeItemAddPacket(player.INVENTORY, slotIndex, -1)
				return
		if app.ENABLE_ACCE_COSTUME_SYSTEM:
			if self.isShowAcceWindow():
				acce.Add(player.INVENTORY, slotIndex, 255)
				return
		if app.ENABLE_AURA_SYSTEM:
			itemVNum = player.GetItemIndex(slotIndex)
			if self.isShowAuraWindow():
				if itemVNum >= 21900 and itemVNum <= 21999:
					return
				aura.Add(player.INVENTORY, slotIndex, 255)
				return

		self.__UseItem(slotIndex)
		mouseModule.mouseController.DeattachObject()
		self.OverOutItem()

	def __UseItem(self, slotIndex):
		ItemVNum = player.GetItemIndex(slotIndex)

		item.SelectItem(ItemVNum)

		if item.IsFlag(item.ITEM_FLAG_CONFIRM_WHEN_USE):
			self.questionDialog = uiCommon.QuestionDialog()
			self.questionDialog.SetText(localeInfo.INVENTORY_REALLY_USE_ITEM)
			self.questionDialog.SetAcceptEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnAccept))
			self.questionDialog.SetCancelEvent(ui.__mem_func__(self.__UseItemQuestionDialog_OnCancel))
			self.questionDialog.Open()
			self.questionDialog.slotIndex = slotIndex

			constInfo.SET_ITEM_QUESTION_DIALOG_STATUS(1)

		if app.ENABLE_INSTANT_CHEST_OPEN:
			if (item.GetItemType() == item.ITEM_TYPE_GIFTBOX) or (item.GetItemType() == item.ITEM_TYPE_GACHA):
				if app.IsPressed(app.DIK_LALT) and not app.IsPressed(app.DIK_LCONTROL):
					itemUseCount = player.GetItemCount(slotIndex) if player.GetItemCount(slotIndex) <= 1000 else 1000
					net.SendOpenChestPacket(slotIndex, itemUseCount)
					return
		
		if app.ENABLE_INSTANT_INGOT_OPEN:
			if ItemVNum == 80007 or ItemVNum == 80006 or ItemVNum == 80005 or ItemVNum == 80008 or ItemVNum == 80009:
				if app.IsPressed(app.DIK_LALT):
					itemUseCount = player.GetItemCount(slotIndex) if player.GetItemCount(slotIndex) <= 500 else 500
					net.SendOpenIngotPacket(slotIndex, itemUseCount)
					return

		if app.__BL_CHEST_DROP_INFO__:
			if app.IsPressed(app.DIK_LCONTROL):
				isMain = not app.IsPressed(app.DIK_LSHIFT)
				if item.HasDropInfo(ItemVNum, isMain) and self.interface:
					self.interface.OpenChestDropWindow(ItemVNum, isMain)
				return
			
			else:
				self.__SendUseItemPacket(slotIndex)
			#net.SendItemUsePacket(slotIndex)

	def __UseItemQuestionDialog_OnCancel(self):
		self.OnCloseQuestionDialog()

	def __UseItemQuestionDialog_OnAccept(self):
		self.__SendUseItemPacket(self.questionDialog.slotIndex)
		self.OnCloseQuestionDialog()

	def __SendUseItemToItemPacket(self, srcSlotPos, dstSlotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
		
		if self.interface.wndRemoveItem.IsShow():
			return

		if app.ENABLE_ODLAMKI_SYSTEM:
			if self.interface.wndOdlamki.IsShow():
				return
		
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if (uiOfflineShopBuilder.IsBuildingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return

			if (uiOfflineShop.IsEditingOfflineShop()):
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return

		net.SendItemUseToItemPacket(srcSlotPos, dstSlotPos)

	def __SendUseItemPacket(self, slotPos):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_PRIVATE_SHOP)
			return
			
		if self.interface.wndRemoveItem.IsShow():
			return

		if app.ENABLE_ODLAMKI_SYSTEM:
			if self.interface.wndOdlamki.IsShow():
				return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return
					
			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.USE_ITEM_FAILURE_OFFLINE_SHOP)
				return

		net.SendItemUsePacket(slotPos)

	def __SendMoveItemPacket(self, srcSlotPos, dstSlotPos, srcItemCount):
		if uiPrivateShopBuilder.IsBuildingPrivateShop():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_PRIVATE_SHOP)
			return

		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			if uiOfflineShopBuilder.IsBuildingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_OFFLINE_SHOP)
				return
					
			if uiOfflineShop.IsEditingOfflineShop():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.MOVE_ITEM_FAILURE_OFFLINE_SHOP)
				return

		net.SendItemMovePacket(srcSlotPos, dstSlotPos, srcItemCount)

	def SetDragonSoulRefineWindow(self, wndDragonSoulRefine):
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoulRefine = wndDragonSoulRefine

	def OnMoveWindow(self, x, y):
		# print "Inventory Global Pos : ", self.GetGlobalPosition()
		# if self.wndBelt:
			# print "Belt Global Pos : ", self.wndBelt.GetGlobalPosition()
			# self.wndBelt.AdjustPositionAndSize()
		if self.wndSystemsWindow:
			self.wndSystemsWindow.AdjustPositionAndSize()
		if self.wndCostume:
			self.wndCostume.AdjustPositionAndSize()

	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		def SetAcceWindow(self, wndAcceCombine, wndAcceAbsorption):
			self.wndAcceCombine = wndAcceCombine
			self.wndAcceAbsorption = wndAcceAbsorption

		def isShowAcceWindow(self):
			if self.wndAcceCombine:
				if self.wndAcceCombine.IsShow():
					return 1
			if self.wndAcceAbsorption:
				if self.wndAcceAbsorption.IsShow():
					return 1
			return 0

	if app.ENABLE_AURA_SYSTEM:
		def SetAuraWindow(self, wndAuraAbsorption, wndAuraRefine):
			self.wndAuraRefine = wndAuraRefine
			self.wndAuraAbsorption = wndAuraAbsorption

		def isShowAuraWindow(self):
			if self.wndAuraRefine:
				if self.wndAuraRefine.IsShow():
					return 1

			if self.wndAuraAbsorption:
				if self.wndAuraAbsorption.IsShow():
					return 1
			
			return 0

	if app.ENABLE_EXTENDED_BLEND:
		def __CanAttachThisItem(self, itemVNum, itemSlotIndex):
			if constInfo.IS_PERMANANET_BLEND_ITEM(itemVNum):
				isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
				if isActivated == 1:
					return False

			# if constInfo.IS_COMPANION_ITEM(itemVNum):
				# isActivated = player.GetItemMetinSocket(itemSlotIndex, 1)
				# if isActivated == 1:
					# return False

			return True
		