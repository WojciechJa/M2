#-*- coding: iso-8859-1 -*-
import ui
# import app
# import dbg
import renderTarget
import uiToolTip
import chat
import localeInfo
import net
import player
# import wndMgr
import exception
import emoji

RENDER_INDEX = 8
MOBS_NAMES = [
	"Silny Dziki S³uga", 
	"Czarny Ork", 
	"M³ody Paj¹k"
]
# MOBS = [101, 551, 552, 636, 701, 2001]
MOBS = [552, 636, 2001]
PRICE = [50000000,1,3]

REQUIRED_ITEMS = [71001,71094,50513]
REQUIRED_ITEMS_AMOUNT_ST2 = 3
REQUIRED_ITEMS_AMOUNT_ST3 = 10

class MarbleShopWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.isLoaded = 0
		self.marmurLevel = 0
		self.index = 0
		self.count = 1
		self.modelshow = 1
		self.mStatus = "W³¹czony"
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def Show(self):
		self.index = 0
		self.marmurLevel = 0
		self.__LoadWindow()
		ui.ScriptWindow.Show(self)
		self.UpdateModel()

	def Open(self):
		self.Show()

	def __LoadWindow(self):
		if self.isLoaded == 1:
			return
			
		self.isLoaded = 1
		try:
			ui.PythonScriptLoader().LoadScriptFile(self, "uiscript/marmur_window.py")
		except:
			exception.Abort("MarbleShop.LoadDialog.LoadScript")

		try:
			self.titleBar = self.GetChild("TitleBar")
			self.MainHeader = self.GetChild("MainHeader")
			self.MainHeaderText = self.GetChild("MainHeader_Text")
			self.NextButton = self.GetChild("NextButton")
			self.PrevButton = self.GetChild("PrevButton")
			self.status = self.GetChild("status")
			self.IloscText = self.GetChild("IloscText")
			self.PlusButton = self.GetChild("PlusButton")
			self.MinusButton = self.GetChild("MinusButton")
			self.MoneyText = self.GetChild("MoneyText")
			self.BuyButton = self.GetChild("BuyButton")
			self.rtarget = self.GetChild("RenderModel")

			self.stopien = {
				1 : self.GetChild("stopien_low"),
				2 : self.GetChild("stopien_medium"),
				3 : self.GetChild("stopien_high")
			}

			self.stopien[1].SetEvent(ui.__mem_func__(self.SetStopienLow))
			self.stopien[2].SetEvent(ui.__mem_func__(self.SetStopienMed))
			self.stopien[3].SetEvent(ui.__mem_func__(self.SetStopienHigh))

			self.NextButton.SetEvent(ui.__mem_func__(self.NextMarmur))
			self.PrevButton.SetEvent(ui.__mem_func__(self.PrevMarmur))
			self.PlusButton.SetEvent(ui.__mem_func__(self.CountPlus))
			self.MinusButton.SetEvent(ui.__mem_func__(self.CountMinus))
			self.BuyButton.SetEvent(ui.__mem_func__(self.BuyMarmur))

			self.status.SetToggleUpEvent(ui.__mem_func__(self.ChangeModelStatus))
			self.status.SetToggleDownEvent(ui.__mem_func__(self.ChangeModelStatus))

			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

			self.itemslots = {
				"required" : [self.GetChild("Real_Slot%d" % (i)) for i in xrange(3)],
			}

			for i in xrange(3):
				self.itemslots["required"][i].SetOverInItemEvent(lambda slotNumber, iPos = i : self.OnOverInItem(slotNumber, iPos))
				self.itemslots["required"][i].SetOverOutItemEvent(ui.__mem_func__(self.OnOverOutItem))

			tooltip = uiToolTip.ToolTip()
			itemtoolTip = uiToolTip.ItemToolTip()
			self.toolTip = tooltip
			self.ItemToolTip = itemtoolTip

			self.toolTip = {
				1 : uiToolTip.ToolTipNew(180),
				2 : uiToolTip.ToolTipNew(180),
				3 : uiToolTip.ToolTipNew(180),
				4 : uiToolTip.ToolTipNew(160),
				5 : uiToolTip.ToolTip(200),
			}
			self.toolTip[1].SetTitle("Stopieñ I")
			self.toolTip[1].AppendSpace(5)
			self.toolTip[1].AppendGreenTextLine("Brak bonusu")
			
			self.toolTip[2].SetTitle("Stopieñ II")
			self.toolTip[2].AppendSpace(5)
			self.toolTip[2].SetDefaultFontName("Tahoma:12")
			self.toolTip[2].AppendGreenTextLine("Silny przeciwko Potworom +10%")
			self.toolTip[2].AppendGreenTextLine("Silny przeciwko Bossom +10%")

			
			self.toolTip[3].SetTitle("Stopieñ III")
			self.toolTip[3].AppendSpace(5)
			self.toolTip[3].SetDefaultFontName("Tahoma:12")
			self.toolTip[3].AppendGreenTextLine("Silny przeciwko Potworom +20%")
			self.toolTip[3].AppendGreenTextLine("Silny przeciwko Bossom +15%")
			self.toolTip[3].AppendGreenTextLine("Silny przeciwko Legendom +5%")
				
			self.toolTip[4].SetTitle("Podgl¹d modelu")
			self.toolTip[4].AppendSpace(5)
			self.toolTip[4].SetDefaultFontName("Tahoma:12")
			self.toolTip[4].AppendTextLine("Wciœnij, aby prze³¹czyæ.")	

			self.ItemToolTip.ClearToolTip()
			self.ItemToolTip.HideToolTip()

			renderTarget.SetBackground(RENDER_INDEX, "d:/ymir work/ui/game/myshop_deco/model_view_bg.sub")
			renderTarget.SetVisibility(RENDER_INDEX, True)
			for i in xrange(3):
				renderTarget.SelectModel(RENDER_INDEX, MOBS[i])
		except:
			exception.Abort("MarbleShop.LoadDialog.BindObject")

		self.itemslots["required"][0].SetItemSlot(0, 0, 0)
		self.itemslots["required"][1].SetItemSlot(0, 0, 0)
		self.itemslots["required"][2].SetItemSlot(0, 0, 0)

		if self.modelshow == 1:
			self.status.SetUp()
		else:
			self.status.Down()

		self.SetCenterPosition()
		self.SetTop()

	def OnOverInItem(self, slotNumber, pos):
		if self.marmurLevel == 1:
			item = REQUIRED_ITEMS[pos]
		elif self.marmurLevel == 2:
			item = REQUIRED_ITEMS[pos]
		if self.ItemToolTip:
			self.ItemToolTip.ClearToolTip()
			self.ItemToolTip.AddItemData(item, metinSlot = [0 for i in xrange(player.METIN_SOCKET_MAX_NUM)])

	def OnOverOutItem(self):
		if self.ItemToolTip:
			self.ItemToolTip.HideToolTip()

	def OnUpdate(self):
		for i in xrange(1,5):
			self.toolTip[i].HideToolTip()
		if self.marmurLevel == 0:
			# self.itemslots["required"][0].Hide()
			# self.itemslots["required"][1].Hide()
			self.stopien[1].Down()
			self.stopien[2].SetUp()
			self.stopien[3].SetUp()
		elif self.marmurLevel == 1:
			# self.itemslots["required"][0].Show()
			# self.itemslots["required"][1].Hide()
			self.stopien[2].Down()
			self.stopien[1].SetUp()
			self.stopien[3].SetUp()
		elif self.marmurLevel == 2:
			# self.itemslots["required"][0].Show()
			# self.itemslots["required"][1].Show()
			self.stopien[3].Down()
			self.stopien[1].SetUp()
			self.stopien[2].SetUp()
		else:
			self.stopien[1].SetUp()
			self.stopien[2].SetUp()
			self.stopien[3].SetUp()

		if self.stopien[1].IsIn():
			self.toolTip[1].Show()
		elif self.stopien[2].IsIn():
			self.toolTip[2].Show()
		elif self.stopien[3].IsIn():
			self.toolTip[3].Show()
		elif self.status.IsIn():
			self.toolTip[4].Show()

	def SetStopienLow(self):
		self.marmurLevel = 0
		self.Update()

	def SetStopienMed(self):
		self.marmurLevel = 1
		self.Update()

	def SetStopienHigh(self):
		self.marmurLevel = 2
		self.Update()

	def NextMarmur(self):
		if self.index < len(MOBS) - 1:
			self.index += 1
		else:
			self.index = 0

		self.UpdateModel()

	def PrevMarmur(self):
		if self.index > 0:
			self.index -= 1
		else:
			self.index = len(MOBS) - 1

		self.UpdateModel()

	def CountPlus(self):
		if self.count >= 20:
			return

		self.count += 1

		self.Update()

	def CountMinus(self):
		if self.count > 1:
			self.count -= 1

		self.Update()

	def ChangeModelStatus(self):
		if self.modelshow == 0:
			self.modelshow = 1
			renderTarget.SetVisibility(RENDER_INDEX, True)
			self.rtarget.Show()
			chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sklep z marmurami> Podgl¹d modelu: W³¹czony")
		else:
			renderTarget.SetVisibility(RENDER_INDEX, False)
			self.rtarget.Hide()
			self.modelshow = 0
			chat.AppendChat(chat.CHAT_TYPE_INFO, "<Sklep z marmurami> Podgl¹d modelu: Wy³¹czony")
		self.UpdateModel()

	def Update(self):
		if self.marmurLevel == 0:
			self.itemslots["required"][0].SetItemSlot(0,0,0)
			self.itemslots["required"][1].SetItemSlot(0,0,0)
			self.itemslots["required"][2].SetItemSlot(0,0,0)

		if self.marmurLevel == 1:
			self.itemslots["required"][0].SetItemSlot(0, REQUIRED_ITEMS[0], REQUIRED_ITEMS_AMOUNT_ST2*self.count)
			self.itemslots["required"][1].SetItemSlot(0, REQUIRED_ITEMS[1], REQUIRED_ITEMS_AMOUNT_ST2*self.count)
			self.itemslots["required"][2].SetItemSlot(0, REQUIRED_ITEMS[2], REQUIRED_ITEMS_AMOUNT_ST2*self.count)

		elif self.marmurLevel == 2:
			self.itemslots["required"][0].SetItemSlot(0, REQUIRED_ITEMS[0], REQUIRED_ITEMS_AMOUNT_ST3*self.count)
			self.itemslots["required"][1].SetItemSlot(0, REQUIRED_ITEMS[1], REQUIRED_ITEMS_AMOUNT_ST3*self.count)
			self.itemslots["required"][2].SetItemSlot(0, REQUIRED_ITEMS[2], REQUIRED_ITEMS_AMOUNT_ST3*self.count)

		if self.index == 0 and self.marmurLevel == 0:
			price = 30000000*self.count
		else:
			price = PRICE[self.marmurLevel]*self.count
			
		self.IloscText.SetText("Iloœæ: x"+str(self.count))
		if self.marmurLevel == 1:
			self.MoneyText.SetText("|cFFb8b8b8|H|h"+localeInfo.MoneyFormat(price)+"|r {}".format(emoji.AppendEmoji("icon/emoji/cheque_icon.png")))
		elif self.marmurLevel == 2:
			self.MoneyText.SetText("|cFFb8b8b8|H|h"+localeInfo.MoneyFormat(price)+"|r {}".format(emoji.AppendEmoji("icon/emoji/cheque_icon.png")))
		else:
			self.MoneyText.SetText("|cFFc99f36|H|h"+localeInfo.MoneyFormat(price)+"|r {}".format(emoji.AppendEmoji("icon/emoji/money_icon.png")))

	def UpdateModel(self):
		renderTarget.SelectModel(RENDER_INDEX, MOBS[self.index])
		self.MainHeaderText.SetText(MOBS_NAMES[self.index])

		self.Update()

	def BuyMarmur(self):
		if self.marmurLevel == 0:
			net.SendChatPacket("/buy_marble_item {} {} 0".format(MOBS[self.index], self.count))
		elif self.marmurLevel == 1:
			net.SendChatPacket("/buy_marble_item {} {} 1".format(MOBS[self.index], self.count))
		elif self.marmurLevel == 2:
			net.SendChatPacket("/buy_marble_item {} {} 2".format(MOBS[self.index], self.count))

	def Destroy(self):
		for i in xrange(1,5):
			self.toolTip[i].HideToolTip()
		self.Hide()
		self.ClearDictionary()

	def Close(self):
		for i in xrange(1,5):
			self.toolTip[i].HideToolTip()
		self.Hide()

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
	