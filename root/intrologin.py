#-*- coding: iso-8859-1 -*-
import ui
import net
import app
import wndMgr
import snd
import musicInfo
import systemSetting
import localeInfo
import constInfo
import ime
if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
	import uiGuild		
import _winreg
REG_PATH = r"SOFTWARE\Arezzo"

def set_reg(name, value):
	try:
		_winreg.CreateKey(_winreg.HKEY_CURRENT_USER, REG_PATH)
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_WRITE)
		_winreg.SetValueEx(registry_key, name, 0, _winreg.REG_SZ, value)
		_winreg.CloseKey(registry_key)
		return True
	except WindowsError:
		return False

def get_reg(name):
	try:
		registry_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, REG_PATH, 0, _winreg.KEY_READ)
		value, regtype = _winreg.QueryValueEx(registry_key, name)
		_winreg.CloseKey(registry_key)
		return str(value)
	except WindowsError:
		return None		
		
class LoginWindow(ui.ScriptWindow):
	PAGE1, PAGE2 = range(2)

	IS_TEST = net.IsTest()

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		self.stream = stream
		self.accountLabel = {0:None, 1:None, 2:None, 3:None, 4:None}
		self.positionLabel = [30, 70, 110, 150, 190]
		self.channelButton = None
		if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			self.LoadingProb = False
		
	def __del__(self):
		ui.ScriptWindow.__del__(self)
		
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)

	if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
		def BINARY_SetGuildBuildingList(self, obj):
			uiGuild.BUILDING_DATA_LIST = obj
			
		def GameLoaded(self):
			constInfo.isGameLoaded = True

			if self.LoadingProb == True:
				self.stream.popupWindow.Close()
				self.__OnClickLoginButton()

	def Open(self):
		self.loginFailureMsgDict={

			"ALREADY"	: localeInfo.LOGIN_FAILURE_ALREAY,
			"NOID"		: localeInfo.LOGIN_FAILURE_NOT_EXIST_ID,
			"WRONGPWD"	: localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"FULL"		: localeInfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN"	: localeInfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR"	: localeInfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK"		: localeInfo.LOGIN_FAILURE_BLOCK_ID,
			"WRONGMAT"	: localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER,
			"QUIT"		: localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER_TRIPLE,
			"BESAMEKEY"	: localeInfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL"	: localeInfo.LOGIN_FAILURE_NOT_AVAIL,
			"NOBILL"	: localeInfo.LOGIN_FAILURE_NOBILL,
			"BLKLOGIN"	: localeInfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK"	: localeInfo.LOGIN_FAILURE_WEB_BLOCK,
			"NOPIN"		: localeInfo.LOGIN_FAILURE_PIN,
		}

		if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			self.loginFailureMsgDict.update({"LOADING_IN_PROGRESS": localeInfo.LOGIN_FAILURE_LOADING_IN_PROGRESS,})

		self.loginFailureFuncDict = {
			"WRONGPWD"	: localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"WRONGMAT"	: localeInfo.LOGIN_FAILURE_WRONG_MATRIX_CARD_NUMBER,
			"QUIT"		: app.Exit,
		}

		if app.ENABLE_HWID_SYSTEM:
			self.loginFailureMsgDict.update({ "HWID"	: localeInfo.LOGIN_FAILURE_HWID, })
			self.loginFailureFuncDict.update({ "HWID"	: app.Exit, })

		if app.CHECK_CLIENT_VERSION:
			self.loginFailureMsgDict.update({"VERSION" : localeInfo.LOGIN_WRONG_VERSION})
			self.loginFailureFuncDict.update({"VERSION" : app.Exit})

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		self.__LoadScript("arezzo/ui/loginwindow.py")
		self.CheckAccount()
		
		if musicInfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		ime.AddExceptKey(91)
		ime.AddExceptKey(93)
		
		self.SetChannel("CH1")
		
		self.Show()
		app.ShowCursor()	

		if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			net.LoadResourcesInCache()

	def Close(self):
		if musicInfo.loginMusic != "" and musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/"+musicInfo.loginMusic)
	
		if self.stream.popupWindow:
			self.stream.popupWindow.Close()

		self.Hide()
		app.HideCursor()
		ime.ClearExceptKey()
		
	def SetIDEditLineFocus(self):
		if self.idEditLine != None:
			self.idEditLine.SetFocus()

	def SetPasswordEditLineFocus(self):
		if self.idEditLine != None:
			self.idEditLine.SetText("")
			self.idEditLine.SetFocus()
			
		if self.pwdEditLine != None:
			self.pwdEditLine.SetText("")
			
		if self.pinEditLine != None:
			self.pinEditLine.SetText("")
			
	def SetPinEditLineFocus(self):
		if self.pinEditLine != None:
			self.pinEditLine.SetFocus()

	def OnConnectFailure(self):
		snd.PlaySound("sound/ui/loginfail.wav")
		self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.EmptyFunc)

	def OnHandShake(self):
		snd.PlaySound("sound/ui/loginok.wav")
		self.PopupDisplayMessage(localeInfo.LOGIN_CONNECT_SUCCESS)

	def OnLoginStart(self):
		self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)

	def OnLoginFailure(self, error):
		try:
			loginFailureMsg = self.loginFailureMsgDict[error]
		except KeyError:
			
			if error == "INACTIVE":
				loginFailureMsg = "PotwierdŸ rejestracjê konta na skrzynce e-mail!"
			else:
				loginFailureMsg = localeInfo.LOGIN_FAILURE_UNKNOWN  + error

		loginFailureFunc = self.loginFailureFuncDict.get(error, self.EmptyFunc)

		self.PopupNotifyMessage(loginFailureMsg, loginFailureFunc)

		snd.PlaySound("sound/ui/loginfail.wav")

	def __LoadScript(self, fileName):
		# try:
		pyScrLoader = ui.PythonScriptLoader()
		pyScrLoader.LoadScriptFile(self, fileName)
		# except Exception:
		# 	import exception
		# 	exception.Abort("LoginWindow.__LoadScript.LoadObject")

		# try:
		gc = self.GetChild

		self.idEditLine = gc("id")
		self.pwdEditLine = gc("pwd")
		self.pinEditLine = gc("pin")
		self.loginButton = gc("login_button")
		self.exitButton = gc("exit_button")

		self.pagetext = gc("PageText")
		self.pageindex = "I"

		self.channelButton = {
			"CH1" : gc("ch1"),
			"CH2" :	gc("ch2"),
			"CH3" : gc("ch3"),
			"CH4" : gc("ch4"),
			"CH5" : gc("ch5"),
			"CH6" : gc("ch6")}
				
		self.pageDict = {
			self.PAGE1	: gc("Board1"),
			self.PAGE2	: gc("Board2"),
		}

		self.prevpagebtn = gc("PrevPageBtn")
		self.nextpagebtn = gc("NextPageBtn")

		self.bgDark = gc("dark_background")

		self.page = self.PAGE1
		self.SetPage(self.PAGE1)

		self.accountData = {
			0 : [[gc("load_button_0"), gc("save_button_0"), gc("delete_button_0")], gc("account_0_text")],
			1 : [[gc("load_button_1"), gc("save_button_1"), gc("delete_button_1")], gc("account_1_text")],
			2 : [[gc("load_button_2"), gc("save_button_2"), gc("delete_button_2")], gc("account_2_text")],
			3 : [[gc("load_button_3"), gc("save_button_3"), gc("delete_button_3")], gc("account_3_text")],
			4 : [[gc("load_button_4"), gc("save_button_4"), gc("delete_button_4")], gc("account_4_text")],
			5 : [[gc("load_button_5"), gc("save_button_5"), gc("delete_button_5")], gc("account_5_text")],
			6 : [[gc("load_button_6"), gc("save_button_6"), gc("delete_button_6")], gc("account_6_text")],
			7 : [[gc("load_button_7"), gc("save_button_7"), gc("delete_button_7")], gc("account_7_text")],
			8 : [[gc("load_button_8"), gc("save_button_8"), gc("delete_button_8")], gc("account_8_text")],
			9 : [[gc("load_button_9"), gc("save_button_9"), gc("delete_button_9")], gc("account_9_text")]
		}
			

		for (channelID, channelButtons) in self.channelButton.items():
			channelButtons.SetEvent(ui.__mem_func__(self.SetChannel), channelID)
			
		for (key, item) in self.accountData.iteritems():
			if isinstance(item[0], list):
				item[0][0].SetEvent(ui.__mem_func__(self.LoadAccount), key)
				item[0][1].SetEvent(ui.__mem_func__(self.SaveAccount), key)
				item[0][2].SetEvent(ui.__mem_func__(self.DeleteAccount), key)

		# except Exception:
		# 	import exception
		# 	exception.Abort("LoginWindow.__LoadScript.BindObject")

		self.loginButton.SetEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.exitButton.SetEvent(ui.__mem_func__(self.OnPressExitKey))
		
		self.prevpagebtn.SetEvent(ui.__mem_func__(self.PrevPage))
		self.nextpagebtn.SetEvent(ui.__mem_func__(self.NextPage))

		# self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		# self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		# self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		# self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))
		
		self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.pinEditLine.SetFocus))
		
		self.pinEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pinEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))
		
		
		self.pagetext.SetText(str(self.pageindex))

		self.idEditLine.SetFocus()
		self.bgDark.Hide()

	def SetPage(self, pageKey):
		self.page = pageKey

		if self.page == self.PAGE1:
			self.pageindex = "I"
		elif self.page == self.PAGE2:
			self.pageindex = "II"

		self.pagetext.SetText(self.pageindex)

		for pageValue in self.pageDict.itervalues():
			pageValue.Hide()

		self.pageDict[pageKey].Show()

	def PrevPage(self):
		if self.page == self.PAGE1:
			return
		elif self.page == self.PAGE2:
			self.SetPage(self.PAGE1)

	def NextPage(self):
		if self.page == self.PAGE2:
			return
		elif self.page == self.PAGE1:
			self.SetPage(self.PAGE2)

	def SetChannel(self, ch):
		for (channelID, channelButtons) in self.channelButton.items():
			channelButtons.SetUp()
			
		self.channelButton[ch].Down()
		
	# 	self.stream.SetConnectInfo("193.177.165.189", self.ChannelPort(ch, 0), "193.177.165.189", self.ChannelPort("LOGIN"))
	# 	net.SetMarkServer("193.177.165.189", self.ChannelPort("LOGO"))
	# 	app.SetGuildMarkPath("10.tga")
	# 	app.SetGuildSymbolPath("10")
	# 	net.SetServerInfo(self.ChannelPort(ch, 2))
		
	# def ChannelPort(self, ch='CH1', value=0):
	# 	channel = {
	# 		"CH1"	:	[30003, "Arezzo.pl", "Kana³ 1"],
	# 		"CH2"	:	[30007, "Arezzo.pl", "Kana³ 2"],
	# 		"CH3"	:	[30011, "Arezzo.pl", "Kana³ 3"],
	# 		"CH4"	:	[30015, "Arezzo.pl", "Kana³ 4"],
	# 		"CH5"	:	[30019, "Arezzo.pl", "Kana³ 5"],
	# 		"CH6"	:	[30023, "Arezzo.pl", "Kana³ 6"]}

		self.stream.SetConnectInfo("176.122.226.4", self.ChannelPort(ch, 0), "176.122.226.4", self.ChannelPort("LOGIN"))
		net.SetMarkServer("176.122.226.4", self.ChannelPort("LOGO"))

		#self.stream.SetConnectInfo("188.68.242.219", self.ChannelPort(ch, 0), "188.68.242.219", self.ChannelPort("LOGIN"))
		#net.SetMarkServer("188.68.242.219", self.ChannelPort("LOGO"))
		app.SetGuildMarkPath("10.tga")
		app.SetGuildSymbolPath("10")
		net.SetServerInfo(self.ChannelPort(ch, 2))
		
	def ChannelPort(self, ch='CH1', value=0):
		channel = {
			"CH1"	:	[30003, "Arezzo.pl", "Kana³ 1"],
			"CH2"	:	[30015, "Arezzo.pl", "Kana³ 2"],
			"CH3"	:	[30027, "Arezzo.pl", "Kana³ 3"],
			"CH4"	:	[30039, "Arezzo.pl", "Kana³ 4"],
			"CH5"	:	[30051, "Arezzo.pl", "Kana³ 5"],
			"CH6"	:	[30063, "Arezzo.pl", "Kana³ 6"]}
		
		if ch == "LOGIN":
			return 30001
		elif ch == "LOGO":
			return channel["CH1"][0]
		elif value == 2:
			return "%s, %s" % (channel[ch][1], channel[ch][2])
		else:
			return channel[ch][value]
				
	def Connect(self, id, pwd, pin):
		if constInfo.SEQUENCE_PACKET_ENABLE:
			net.SetPacketSequenceMode()

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(localeInfo.LOGIN_CONNETING, self.EmptyFunc, localeInfo.UI_CANCEL)
		
		net.SetVersionId(constInfo.CLIENT_VERSION)
		self.stream.SetLoginInfo(id, pwd, pin)
		self.stream.Connect()
		
	def PopupDisplayMessage(self, msg):
		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg)

	def PopupNotifyMessage(self, msg, func=0):
		if not func:
			func = self.EmptyFunc

		self.stream.popupWindow.Close()
		self.stream.popupWindow.Open(msg, func, localeInfo.UI_OK)

	def OnPressExitKey(self):
		if self.stream.popupWindow:
			self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return TRUE

	def EmptyFunc(self):
		pass

	def __OnClickLoginButton(self):
		if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
			if constInfo.isGameLoaded == False:
				self.PopupNotifyMessage(self.loginFailureMsgDict["LOADING_IN_PROGRESS"], self.EmptyFunc)
				self.LoadingProb = True
				return

		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()
		pin = self.pinEditLine.GetText()

		if len(id)==0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_ID, self.EmptyFunc)
			return

		if len(pwd)==0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_PASSWORD, self.EmptyFunc)
			return
			
		if len(pin)==0:
			self.PopupNotifyMessage(localeInfo.LOGIN_INPUT_PIN, self.SetPinEditLineFocus)
			return

		self.Connect(id, pwd, pin)
		
	def CheckAccount(self):
		for i in xrange(10):
			if get_reg("id_%d" % i):
				self.accountData[i][1].SetText(str(get_reg("id_%d" % i)))
				self.accountData[i][0][1].Hide()
				self.accountData[i][0][0].Show()
			else:
				self.accountData[i][1].SetText("Nie zapisano")
				self.accountData[i][0][1].Show()
				self.accountData[i][0][0].Hide()

	def OnUpdate(self):
		if not self.stream.popupWindow or not self.stream.popupWindow.IsShow():
			self.bgDark.Hide()
			
		if self.stream.popupWindow.IsShow() and not self.bgDark.IsShow():
			self.bgDark.Show()

	def DeleteAccount(self, key):
		if get_reg("id_%d" % key):
			set_reg("id_%d" % key, "")
			set_reg("pwd_%d" % key, "")
			set_reg("pin%d" % key, "")
			self.PopupNotifyMessage("Konto zosta³o usuniête.")
		else:
			self.PopupNotifyMessage("Aby usun¹æ konto musisz je zapisaæ.")
			
		self.CheckAccount()
		
	def LoadAccount(self, key):
		if get_reg("id_%d" % key):
			self.idEditLine.SetText(str(get_reg("id_%d" % key)))
			self.pwdEditLine.SetText(str(get_reg("pwd_%d" % key)))
			self.pinEditLine.SetText(str(get_reg("pin%d" % key)))
			self.pwdEditLine.SetFocus()
			ajdi = self.idEditLine.GetText()
			haselko = self.pwdEditLine.GetText()
			pinek = self.pinEditLine.GetText()
			if app.ENABLE_PERFORMANCE_IMPROVEMENTS_NEW:
				if constInfo.isGameLoaded == False:
					self.PopupNotifyMessage(self.loginFailureMsgDict["LOADING_IN_PROGRESS"], self.EmptyFunc)
					self.LoadingProb = True
					return
			self.Connect(ajdi, haselko,pinek)
		else:
			self.PopupNotifyMessage("Nie ma zapisanego konta.")

	def OnKeyDown(self, key):
		i = max(0, key - app.DIK_F1)
		self.LoadAccount(i)
		return True

	def SaveAccount(self, key):
		if get_reg("id_%d" % key):
			self.PopupNotifyMessage("Aby zapisaæ konto musisz je usun¹æ.")
			return
		
		if self.idEditLine.GetText() == "" or self.pwdEditLine.GetText() == "":
			self.PopupNotifyMessage("Podaj dane aby zapisaæ konto.")
			return
		
		set_reg("id_%d" % key, self.idEditLine.GetText())
		set_reg("pwd_%d" % key, self.pwdEditLine.GetText())
		set_reg("pin%d" % key, self.pinEditLine.GetText())
		self.PopupNotifyMessage("Konto zosta³o zapisane.")
		self.CheckAccount()
