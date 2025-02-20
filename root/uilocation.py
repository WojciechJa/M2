import ui
import wndMgr
import localeInfo
import net
import background
import chat
import app
import constInfo

savedLocationData = {
	"elements" : {"info" : {}}
}

MAX_POS = 25
MAX_LEN = 16

class UiLocationWindow(ui.ScriptWindow):
	PAGE1, PAGE2, PAGE3, PAGE4, PAGE5 = range(5)

	def __init__(self):
		ui.ScriptWindow.__init__(self)

		self.BoardWindow = None

		self.boardSaveWindow = None
		self.boardSaveText = None
		self.boardSaveInput = None
		self.boardSaveInputLine = None
		self.boardSaveButtonAccept = None
		self.boardSaveButtonCancel = None
		self.test1 = 0
		self.test2 = 0

		self.boardSaveWindowChilds = {}
		self.boardSaveChilds = []
		self.__wndRenderMapWnd = None

		self.locationHorizontalWindow = {}
		self.locationNameBar = {}
		self.locationNameText = {}
		self.locationPosText = {}
		self.locationButtonChild = {}
		self.locationMapBtn = {}
		self.locationMapindex = {}
		self.locationButton = {
			"childrens" : {"info" : {}}
		}

		self.page = self.PAGE1
		self.pageindex = "I"

		self.positionIndex = None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

		self.BoardWindow = None

		self.boardSaveWindow = None
		self.boardSaveText = None
		self.boardSaveInput = None
		self.boardSaveInputLine = None
		self.boardSaveButtonAccept = None
		self.boardSaveButtonCancel = None
		self.test1 = 0
		self.test2 = 0

		self.boardSaveWindowChilds = {}
		self.boardSaveChilds = []

		self.locationHorizontalWindow = {}
		self.locationNameBar = {}
		self.locationNameText = {}
		self.locationPosText = {}
		self.locationButtonChild = {}
		self.locationMapBtn = {}
		self.locationMapindex = {}
		self.locationButton = {
			"childrens" : {"info" : {}}
		}

		self.positionIndex = None

	def Close(self):
		self.OnCloseBoardSaveWindow()
		wndMgr.Hide(self.hWnd)

	def OnPressEscapeKey(self):
		self.OnCloseBoardSaveWindow()
		self.Hide()
		return True

	def OnPressExitKey(self):
		self.OnCloseBoardSaveWindow()
		self.Hide()
		return True

	def OnCloseBoardSaveWindow(self):
		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == True:
			constInfo.LASTPAGE = self.page
		for i in range(len(self.boardSaveChilds)):
			self.boardSaveChilds[i].Hide()

		self.boardSaveInputLine.OnKillFocus()
		self.boardSaveInputLine.SetText("")

		if self.__wndRenderMapWnd:
			self.__wndRenderMapWnd.Hide()
		
	def LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/locationwindow.py")
			
		except:
			import exception
			exception.Abort("UiLocationWindow.LoadWindow.Elements")

		try:
			self.BoardWindow			= self.GetChild("board")
			self.prevpagebtn			= self.GetChild("PrevPageBtn")
			self.nextpagebtn			= self.GetChild("NextPageBtn")
			self.pagetext				= self.GetChild("PageText")
			self.SaveLastPageButton		= self.GetChild("SaveLastPageButton")
			self.OpenWhenTeleportButton = self.GetChild("OpenWhenTeleportButton")

			self.boardSaveWindow = ui.Board()
			self.boardSaveWindow.SetParent(self)
			self.boardSaveWindow.SetPosition(70, 120)
			self.boardSaveWindow.SetSize(210, 95)
			self.boardSaveWindow.Hide()

			self.boardSaveText = ui.TextLine()
			self.boardSaveText.SetParent(self.boardSaveWindow)
			self.boardSaveText.SetWindowHorizontalAlignCenter()
			self.boardSaveText.SetHorizontalAlignCenter()
			self.boardSaveText.SetPosition(0, 15)
			self.boardSaveText.SetText("Nazwa zapisywanej pozycji:")
			self.boardSaveText.Hide()

			self.boardSaveInput = ui.ImageBox()
			self.boardSaveInput.SetParent(self.boardSaveWindow)
			self.boardSaveInput.SetPosition(25, 35)
			self.boardSaveInput.LoadImage("d:/ymir work/location_window/name_input.tga")
			self.boardSaveInput.Hide()

			self.boardSaveInputLine = ui.EditLine()
			self.boardSaveInputLine.SetParent(self.boardSaveInput)
			self.boardSaveInputLine.SetPosition(5, 2)
			self.boardSaveInputLine.SetSize(155, 16)
			self.boardSaveInputLine.SetMax(MAX_LEN)
			self.boardSaveInputLine.Hide()

			self.boardSaveButtonAccept = ui.Button()
			self.boardSaveButtonAccept.SetParent(self.boardSaveWindow)
			self.boardSaveButtonAccept.SetPosition(50, 60)
			self.boardSaveButtonAccept.SetUpVisual("d:/ymir work/ui/accept0.tga")
			self.boardSaveButtonAccept.SetOverVisual("d:/ymir work/ui/accept1.tga")
			self.boardSaveButtonAccept.SetDownVisual("d:/ymir work/ui/accept2.tga")
			self.boardSaveButtonAccept.SetEvent(ui.__mem_func__(self.OnClickCreateButton))
			self.boardSaveButtonAccept.Hide()

			self.boardSaveButtonCancel = ui.Button()
			self.boardSaveButtonCancel.SetParent(self.boardSaveWindow)
			self.boardSaveButtonCancel.SetPosition(118, 60)
			self.boardSaveButtonCancel.SetUpVisual("d:/ymir work/ui/cancel0.tga")
			self.boardSaveButtonCancel.SetOverVisual("d:/ymir work/ui/cancel1.tga")
			self.boardSaveButtonCancel.SetDownVisual("d:/ymir work/ui/cancel2.tga")
			self.boardSaveButtonCancel.SetEvent(ui.__mem_func__(self.OnCloseBoardSaveWindow))
			self.boardSaveButtonCancel.Hide()

			self.boardSaveChilds = [self.boardSaveWindow, self.boardSaveText, self.boardSaveInput, self.boardSaveInputLine, self.boardSaveButtonAccept, self.boardSaveButtonCancel]
			
			self.SetTop()
			if constInfo.ZAPAMIETAJ_OKNO_ZAPISU == False:
				self.SetCenterPosition()

		except:
			import exception
			exception.Abort("UiLocationWindow.LoadWindow.BindElements")

		self.GetChild("TitleBar").SetCloseEvent(ui.__mem_func__(self.Close))
		self.prevpagebtn.SetEvent(ui.__mem_func__(self.PrevPage))
		self.nextpagebtn.SetEvent(ui.__mem_func__(self.NextPage))

		self.SaveLastPageButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableLastPage))
		self.SaveLastPageButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableLastPage))

		self.OpenWhenTeleportButton.SetToggleUpEvent(ui.__mem_func__(self.__OnClickEnableFastOpen))
		self.OpenWhenTeleportButton.SetToggleDownEvent(ui.__mem_func__(self.__OnClickEnableFastOpen))

		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == True:
			self.SaveLastPageButton.Down()

		if constInfo.ZAPAMIETAJ_OKNO_ZAPISU == True:
			self.OpenWhenTeleportButton.Down()

		self.BindObjects()
		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == True:
			self.SetPage(constInfo.LASTPAGE)
		else:
			self.SetPage(self.PAGE1)
		self.pagetext.SetText(str(self.pageindex))

		for i in range(MAX_POS):
			savedLocationData["elements"][i] = []
			savedLocationData["elements"][i].append([0, 0, "None", 0, 0])

		self.SetTop()

	def __OnClickEnableLastPage(self):
		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == False:
			constInfo.ZAPAMIETAJ_OSTATNIA_STRONE = True
		else:
			constInfo.ZAPAMIETAJ_OSTATNIA_STRONE = False

	def __OnClickEnableFastOpen(self):
		if constInfo.ZAPAMIETAJ_OKNO_ZAPISU== False:
			constInfo.ZAPAMIETAJ_OKNO_ZAPISU = True
		else:
			constInfo.ZAPAMIETAJ_OKNO_ZAPISU = False

	def BindObjects(self):
		buttonMap = {
			0: [localeInfo.UI_LOCATION_02, 3],
			1: [localeInfo.UI_LOCATION_01, 2],
			2: [localeInfo.UI_LOCATION_03, 1],
		}

		self.__wndRenderMapWnd = RenderMapDialog()
		self.__wndRenderMapWnd.Hide()

		self.pageDict = {
			self.PAGE1	: self.GetChild("Page1"),
			self.PAGE2	: self.GetChild("Page2"),
			self.PAGE3  : self.GetChild("Page3"),
			self.PAGE4  : self.GetChild("Page4"),
			self.PAGE5	: self.GetChild("Page5"),
		}

		for i in range(MAX_POS):
			self.locationHorizontalWindow[i] = self.GetChild("locationHorizontalWindow%s" % str(i))
			self.locationHorizontalWindow[i].Show()

			self.locationNameText[i] = ui.TextLine()
			self.locationNameText[i].SetParent(self.locationHorizontalWindow[i])
			self.locationNameText[i].SetPosition(125, 12)
			self.locationNameText[i].SetHorizontalAlignCenter()
			self.locationNameText[i].SetFontName("Tahoma:14b")
			self.locationNameText[i].SetOutline(True)
			self.locationNameText[i].SetText("Pozycja niezapisana")
			self.locationNameText[i].Show()

			self.locationPosText[i] = ui.TextLine()
			self.locationPosText[i].SetParent(self.locationHorizontalWindow[i])
			self.locationPosText[i].SetPosition(125, 33)
			self.locationPosText[i].SetHorizontalAlignCenter()
			self.locationPosText[i].SetFontName("Tahoma:14")
			self.locationPosText[i].SetOutline(True)
			self.locationPosText[i].SetText("")
			self.locationPosText[i].Hide()

			self.locationButton["childrens"][i] = []

			for j in range(3):
				self.locationButtonChild[j] = ui.Button()
				self.locationButtonChild[j].SetParent(self.locationHorizontalWindow[i])
				self.locationButtonChild[j].SetPosition(227, 10+20*j)
				self.locationButtonChild[j].SetUpVisual("d:/ymir work/ui/location_window/button_example_001.png")
				self.locationButtonChild[j].SetOverVisual("d:/ymir work/ui/location_window/button_example_002.png")
				self.locationButtonChild[j].SetDownVisual("d:/ymir work/ui/location_window/button_example_003.png")
				self.locationButtonChild[j].SetText(buttonMap[j][0])
				self.locationButtonChild[j].SetFontName("Tahoma:14b")
				self.locationButtonChild[j].SetOutline(True)
				self.locationButtonChild[j].SetEvent(ui.__mem_func__(self.OnClickButton), i, buttonMap[j][1])
				self.locationButtonChild[j].Show()

				self.locationButton["childrens"][i].append(self.locationButtonChild[j])

			self.locationButton["childrens"][i][2].SetPosition(227, 10+20*1)

			self.boardSaveWindowChilds[i] = [self.locationHorizontalWindow[i]]
			self.locationHorizontalWindow[i].Hide()

			self.locationMapBtn[i] = ui.ImageBox()
			self.locationMapBtn[i].SetParent(self.locationHorizontalWindow[i])
			self.locationMapBtn[i].SetPosition(20, 20)
			self.locationMapBtn[i].LoadImage("d:/ymir work/ui/pattern/q_mark_01.tga")
			self.locationMapBtn[i].SetEvent(ui.__mem_func__(self.EventProgressIn), "mouse_over_in", i)
			self.locationMapBtn[i].SetEvent(ui.__mem_func__(self.EventProgressOut), "MOUSE_OVER_OUT")
			self.locationMapBtn[i].Show()

			self.OnMoveWindow(*self.GetGlobalPosition())

	def SetPage(self, pageKey):
		self.page = pageKey
		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == True:
			constInfo.LASTPAGE = self.page
		for i in range(MAX_POS):
			self.locationHorizontalWindow[i].Show()
		if self.page == self.PAGE1:
			self.pageindex = "I"
		elif self.page == self.PAGE2:
			self.pageindex = "II"
		elif self.page == self.PAGE3:
			self.pageindex = "III"
		elif self.page == self.PAGE4:
			self.pageindex = "IV"
		elif self.page == self.PAGE5:
			self.pageindex = "V"

		self.pagetext.SetText(self.pageindex)

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		self.pageDict[pageKey].Show()

	def PrevPage(self):
		if self.page == self.PAGE1:
			return
		elif self.page == self.PAGE2:
			self.SetPage(self.PAGE1)
		elif self.page == self.PAGE3:
			self.SetPage(self.PAGE2)
		elif self.page == self.PAGE4:
			self.SetPage(self.PAGE3)
		elif self.page == self.PAGE5:
			self.SetPage(self.PAGE4)

	def NextPage(self):
		if self.page == self.PAGE5:
			return
		elif self.page == self.PAGE1:
			self.SetPage(self.PAGE2)
		elif self.page == self.PAGE2:
			self.SetPage(self.PAGE3)
		elif self.page == self.PAGE3:
			self.SetPage(self.PAGE4)
		elif self.page == self.PAGE4:
			self.SetPage(self.PAGE5)

	def BindNameString(self):
		string = ""

		for i in range(MAX_POS):
			info = savedLocationData["elements"][i][0]

			if info[2] != "None":
				string += "%s" % info[0]
				string += "#"
				string += info[2].replace(" ", "_")
				string += "#"

		return string

	def BindNameStringNew(self, pos):
		string = self.BindNameString()
		string_split = string.split("#")
		string_new = ""

		for i in range(len(string_split) - 1):
			if i % 2 - 1:
				if int(string_split[i]) != int(pos):
					string_new += "%s" % string_split[i]
					string_new += "#"
					string_new += string_split[i + 1]
					string_new += "#"

		return string_new

	def OnUpdate(self):
	#	for i in range(MAX_POS):
	#		info = savedLocationData["elements"][i][0]
	#		if self.locationMapBtn[i]:
			# test = localeInfo.GetMiniMapZoneNameByIdx2(info[1])
			# chat.AppendChat(chat.CHAT_TYPE_INFO, str(test))

		if constInfo.ZAPAMIETAJ_OSTATNIA_STRONE == True:
			self.SaveLastPageButton.Down()
		else:
			self.SaveLastPageButton.SetUp()

	def RefreshData(self):
		for i in range(len(savedLocationData["elements"]) - 1):
			info = savedLocationData["elements"][i][0]
			x=savedLocationData["elements"][i][0][3]
			y=savedLocationData["elements"][i][0][4]
			(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(int(x), int(y))
			valuex = (int(x - xBase)/100)
			valuey = (int(y - yBase)/100)
			# chat.AppendChat(chat.CHAT_TYPE_INFO, "%d, %d" % (int(x - xBase)/100, int(y - yBase)/100))

			for j in range(0, 3):
				self.locationButton["childrens"][i][1].Show()
				self.locationButton["childrens"][i][j].SetUp()
				self.locationButton["childrens"][i][j].Enable()

			if info[2] == "None":
				self.locationButton["childrens"][i][0].Down()
				self.locationButton["childrens"][i][0].Disable()
				self.locationButton["childrens"][i][2].Hide()
				self.locationMapBtn[i].Hide()

				self.locationNameText[i].SetText(localeInfo.UI_LOCATION_04)

				self.locationPosText[i].SetText("")
				self.locationPosText[i].Hide()
			else:
				self.locationButton["childrens"][i][1].Hide()
				self.locationButton["childrens"][i][2].Show()

				self.locationNameText[i].SetText(info[2].replace("_", " "))

				self.locationPosText[i].Show()
				self.locationPosText[i].SetText("("+str(valuex)+", "+str(valuey)+")")

				self.locationMapBtn[i].Show()

	# def EventProgress(self):
	#	self.__wndRenderMapWnd.Open(self.locationMapindex)

	def EventProgressIn(self, event_type, pos):
		if "mouse_over_in" == str(event_type):
			self.__OverInButton(pos)
		# elif "mouse_over_out" == str(event_type):
		# 	self.__OverOutButton()
		else:
			return

	def EventProgressOut(self, event_type):
		if "MOUSE_OVER_OUT" == str(event_type):
			self.__OverOutButton()
		else:
			return

	def __OverInButton(self, pos):
		info = savedLocationData["elements"][pos][0][1]
		x=savedLocationData["elements"][pos][0][3]
		y=savedLocationData["elements"][pos][0][4]
		if self.__wndRenderMapWnd.IsShow():
			self.__wndRenderMapWnd.Hide()
		else:
			self.__wndRenderMapWnd.Open(info, pos, x, y)

	def __OverOutButton(self):
		self.__wndRenderMapWnd.Hide()

	def ClearData(self):
		for i in range(MAX_POS):
			savedLocationData["elements"][i][0][0] = 0
			savedLocationData["elements"][i][0][1] = 0
			savedLocationData["elements"][i][0][2] = "None"

		self.RefreshData()

	def AppendPosition(self, pos, index, posx, posy):
		savedLocationData["elements"][pos][0][0] = pos
		savedLocationData["elements"][pos][0][1] = index
		savedLocationData["elements"][pos][0][3] = posx
		savedLocationData["elements"][pos][0][4] = posy

	def AppendName(self, name):
		name_split = name.split("#")

		for i in range(len(name_split) - 1):
			if i % 2 - 1:
				savedLocationData["elements"][int(name_split[i])][0][2] = name_split[i + 1]

		self.RefreshData()

	def OnClickButton(self, pos, action):
		if action == 2:
			if savedLocationData["elements"][pos][0][1] != 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.UI_LOCATION_05)

			elif self.boardSaveWindow.IsShow():
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.UI_LOCATION_06)

			else:
				self.positionIndex = pos
				self.OnClickSaveButton()
		else:
			if savedLocationData["elements"][pos][0][1] == 0:
				chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.UI_LOCATION_07)
			else:
				if action == 1:
					string = self.BindNameStringNew(pos)
					net.SendLocationManagerPacket(action, pos, string)
				else:
					net.SendLocationManagerPacket(action, pos, "")
		
	def OnMoveWindow(self, x, y):
		self.__wndRenderMapWnd.SetPosition(x-270,
									y + (self.GetHeight() - self.__wndRenderMapWnd.GetHeight()) / 2)

	def OnClickSaveButton(self):
		for i in range(len(self.boardSaveChilds)):
			self.boardSaveChilds[i].Show()

		self.boardSaveInputLine.SetFocus()
		self.boardSaveInputLine.OnSetFocus()

	def OnClickCreateButton(self):
		if len(self.boardSaveInputLine.GetText()) > MAX_LEN:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.UI_LOCATION_08 % MAX_LEN)
		else:
			string = self.BindNameString()

			string += "%s" % self.positionIndex
			string += "#"
			string += self.boardSaveInputLine.GetText().replace(" ", "_")
			string += "#"

			self.OnCloseBoardSaveWindow()
			net.SendLocationManagerPacket(2, self.positionIndex, string)

class RenderMapDialog(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadWindow()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadWindow(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/locationrendermapdialog.py")

		except:
			import exception
			exception.Abort("RenderMapDialog.__LoadWindow.LoadObject")

		self.board = self.GetChild("map_board")
		self.header = self.GetChild("header_map")
		self.mapImage = self.GetChild("map_image")
		self.headerText = self.GetChild("header_map_text")
		self.mapPoint = self.GetChild("map_point")

		self.headerWidth = self.header.GetWidth()

	def Destroy(self):
		self.ClearDictionary()

		self.board = None
		self.header = None
		self.mapImage = None
		self.headerText = None

	def Open(self, mapindex, pos, x, y):
		test = localeInfo.GetMiniMapZoneNameByIdx2(mapindex)
		# chat.AppendChat(chat.CHAT_TYPE_INFO, "mapindex:"+str(mapindex)+" uiPos:"+str(pos)+" name:"+str(test))
		localeMapName = localeInfo.MINIMAP_ZONE_NAME_DICT.get(test, "")
		fileName = "d:/ymir work/ui/atlas/{}/atlas.sub".format(test)

		if not app.IsExistFile(fileName):
			return False

		self.mapImage.LoadImage(fileName)

		xSize = 8 + self.mapImage.GetWidth()
		ySize = 25 + 3 + self.mapImage.GetHeight()
		self.SetSize(xSize, ySize)
		self.board.SetSize(xSize, ySize)
		self.header.SetScale(float(self.mapImage.GetWidth()) / self.headerWidth, 1.0)

		self.headerText.SetText(localeMapName)

		self.mapImage.Show()

		# #test
		(mapName, xBase, yBase) = background.GlobalPositionToMapInfo(int(x), int(y))
		# valuex = (int(x - xBase)/100)
		# valuey = (int(y - yBase)/100)
		xMap, yMap = background.GlobalPositionToMapSize(int(x), int(y))
		xPoint = int(int(x) - xBase)
		yPoint = int(int(y) - yBase)

		xPoint /= float(xMap) / float(self.mapImage.GetWidth())
		yPoint /= float(yMap) / float(self.mapImage.GetHeight())

		self.mapPoint.SetPosition(xPoint - (self.mapPoint.GetWidth() / 4), yPoint - self.mapPoint.GetHeight())
		#end
	

		self.Show()
		self.UpdateRect()

		return True