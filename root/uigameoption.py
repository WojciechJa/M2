import ui
import snd
import systemSetting
import net
import chat
import app
import localeInfo
import constInfo
# import chrmgr
import player
import uiPrivateShopBuilder
import background
import musicInfo
import uiSelectMusic
# import interfaceModule
if app.ENABLE_OFFLINE_SHOP_SYSTEM:
	import uiOfflineShopBuilder

blockMode = 0
viewChatMode = 0

MUSIC_FILENAME_MAX_LEN = 25

MOBILE = False

if localeInfo.IsYMIR():
	MOBILE = True

class OptionDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.__Load()
		self.RefreshViewChat()
		self.RefreshAlwaysShowName()
		self.RefreshShowDamage()
		self.RefreshShowSalesText()
		self.RefreshDesc0Btn()
		self.RefreshDesc1Btn()
		self.RefreshDesc2Btn()
		self.RefreshDesc3Btn()
		self.RefreshFontText()
		self.RefreshAutobuff()
		self.RefreshPety()
		self.RefreshWierzch()
		if app.WJ_SHOW_MOB_INFO:
			self.RefreshShowMobInfo()
		if app.ENABLE_DOG_MODE:
			self.RefreshDogMode()

	def __del__(self):
		ui.ScriptWindow.__del__(self)
		print " -------------------------------------- DELETE GAME OPTION DIALOG"

	def __Initialize(self):
		self.board = 0
		self.titleBar = 0
		self.page = 0
		self.nameColorModeButtonList = []
		self.viewTargetBoardButtonList = []
		self.pvpModeButtonDict = {}
		self.blockButtonList = []
		self.viewChatButtonList = []
		self.alwaysShowNameButtonList = []
		self.showDamageButtonList = []
		self.showsalesTextButtonList = []
		self.fonttype = []
		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList = []
		if app.ENABLE_DYNAMIC_FONTS:
			self.fonts = None
			self.FontsBoard = None
		if app.ENABLE_DOG_MODE:
			self.dogModeButtonList = []

		self.tilingMode = 0
		self.changeMusicButton = 0
		self.selectMusicFile = 0
		self.ctrlMusicVolume = 0
		self.ctrlSoundVolume = 0
		self.musicListDlg = 0
		self.tilingApplyButton = 0
		self.cameraModeButtonList = []
		self.fogModeButtonList = []
		self.tilingModeButtonList = []
		self.ctrlShadowQuality = 0
		if app.ENABLE_FOV_OPTION:
			self.fovController = None
			self.fovResetButton = None
			self.fovValueText = None

		self.sklepyoffline_controller = None

	def Destroy(self):
		self.ClearDictionary()
		if app.ENABLE_DYNAMIC_FONTS and self.FontsBoard:
			self.__CloseFonts()
		self.__Initialize()
		print " -------------------------------------- DESTROY GAME OPTION DIALOG"

	def __Load_LoadScript(self, fileName):
		try:
			pyScriptLoader = ui.PythonScriptLoader()
			pyScriptLoader.LoadScriptFile(self, fileName)
		except:
			import exception
			exception.Abort("OptionDialog.__Load_LoadScript")

	def __Load_BindObject(self):
		try:
			GetObject = self.GetChild
			self.board = GetObject("board")
			self.titleBar = GetObject("titlebar")
			self.nameColorModeButtonList.append(GetObject("name_color_normal"))
			self.nameColorModeButtonList.append(GetObject("name_color_empire"))
			self.viewTargetBoardButtonList.append(GetObject("target_board_no_view"))
			self.viewTargetBoardButtonList.append(GetObject("target_board_view"))
			self.pvpModeButtonDict[player.PK_MODE_PEACE] = GetObject("pvp_peace")
			self.pvpModeButtonDict[player.PK_MODE_REVENGE] = GetObject("pvp_revenge")
			self.pvpModeButtonDict[player.PK_MODE_GUILD] = GetObject("pvp_guild")
			self.pvpModeButtonDict[player.PK_MODE_FREE] = GetObject("pvp_free")
			self.blockButtonList.append(GetObject("block_exchange_button"))
			self.blockButtonList.append(GetObject("block_party_button"))
			self.blockButtonList.append(GetObject("block_guild_button"))
			self.blockButtonList.append(GetObject("block_whisper_button"))
			self.blockButtonList.append(GetObject("block_friend_button"))
			self.blockButtonList.append(GetObject("block_party_request_button"))
			self.viewChatButtonList.append(GetObject("view_chat_on_button"))
			self.viewChatButtonList.append(GetObject("view_chat_off_button"))
			self.alwaysShowNameButtonList.append(GetObject("always_show_name_on_button"))
			self.alwaysShowNameButtonList.append(GetObject("always_show_name_off_button"))
			self.showDamageButtonList.append(GetObject("show_damage_on_button"))
			self.showDamageButtonList.append(GetObject("show_damage_off_button"))
			self.showsalesTextButtonList.append(GetObject("salestext_on_button"))
			if app.ENABLE_DOG_MODE:
				self.dogModeButtonList.append(GetObject("dog_mode_open"))
				self.dogModeButtonList.append(GetObject("dog_mode_close"))
			if app.ENABLE_DYNAMIC_FONTS:
				self.fonts = GetObject("fonts_select")
			self.showsalesTextButtonList.append(GetObject("salestext_off_button"))
			self.fonttype.append(GetObject("font_type"))
			self.fonttype.append(GetObject("font_type2"))
			if app.WJ_SHOW_MOB_INFO:
				self.showMobInfoButtonList.append(GetObject("show_mob_level_button"))
				self.showMobInfoButtonList.append(GetObject("show_mob_AI_flag_button"))

			self.selectMusicFile = GetObject("bgm_file")
			self.changeMusicButton = GetObject("bgm_button")
			self.ctrlMusicVolume = GetObject("music_volume_controller")
			self.ctrlSoundVolume = GetObject("sound_volume_controller")
			self.cameraModeButtonList.append(GetObject("camera_short"))
			self.cameraModeButtonList.append(GetObject("camera_long"))
			self.fogModeButtonList.append(GetObject("fog_level0"))
			self.fogModeButtonList.append(GetObject("fog_level1"))
			self.fogModeButtonList.append(GetObject("fog_level2"))
			self.tilingModeButtonList.append(GetObject("tiling_cpu"))
			self.tilingModeButtonList.append(GetObject("tiling_gpu"))
			self.tilingApplyButton=GetObject("tiling_apply")
			if app.ENABLE_FOV_OPTION:
				self.fovController = GetObject("fov_controller")
				self.fovResetButton = GetObject("fov_reset_button")
				self.fovValueText = GetObject("fov_value_text")

			self.sklepyoffline_controller = GetObject("sklepyoffline_controller")

			self.autobuff_on = GetObject("autobuff_on")
			self.autobuff_off = GetObject("autobuff_off")

			self.pety_on = GetObject("pety_on")
			self.pety_off = GetObject("pety_off")

			self.wierzchowce_on = GetObject("wierzchowce_on")
			self.wierzchowce_off = GetObject("wierzchowce_off")

			global MOBILE
			if MOBILE:
				self.inputMobileButton = GetObject("input_mobile_button")
				self.deleteMobileButton = GetObject("delete_mobile_button")

			self.global_window = GetObject("global_window")
			self.system_window = GetObject("system_window")
			self.desc_window = GetObject("desc_window")
			self.ukrywanie_window = GetObject("ukrywanie_window")
			self.category_btn1 = GetObject("category_btn1")
			self.category_btn2 = GetObject("category_btn2")
			self.category_btn3 = GetObject("category_btn3")
			self.category_btn4 = GetObject("category_btn4")

			self.desc0_on = GetObject("desc0_on")
			self.desc0_off = GetObject("desc0_off")
			self.desc1_on = GetObject("desc1_on")
			self.desc1_off = GetObject("desc1_off")
			self.desc2_on = GetObject("desc2_on")
			self.desc2_off = GetObject("desc2_off")
			self.desc3_on = GetObject("desc3_on")
			self.desc3_off = GetObject("desc3_off")

			self.category_btn1.SetEvent(ui.__mem_func__(self.OpenOgolne))
			self.category_btn2.SetEvent(ui.__mem_func__(self.OpenSystemowe))
			self.category_btn3.SetEvent(ui.__mem_func__(self.OpenDesc))
			self.category_btn4.SetEvent(ui.__mem_func__(self.OpenUkrywanie))

			self.page = 0
			if self.page == 0:
				self.desc_window.Hide()
				self.global_window.Show()
				self.system_window.Hide()
				self.ukrywanie_window.Hide()
				self.category_btn1.Down()
			else:
				self.category_btn1.SetUp()

			if self.page == 1:
				self.desc_window.Hide()
				self.global_window.Hide()
				self.system_window.Show()
				self.ukrywanie_window.Hide()
				self.category_btn2.Down()
			else:
				self.category_btn2.SetUp()

			if self.page == 2:
				self.global_window.Hide()
				self.desc_window.Show()
				self.system_window.Hide()
				self.ukrywanie_window.Hide()
				self.category_btn3.Down()
			else:
				self.category_btn3.SetUp()
			
			if self.page == 3:
				self.global_window.Hide()
				self.desc_window.Hide()
				self.system_window.Hide()
				self.ukrywanie_window.Show()
				self.category_btn4.Down()
			else:
				self.category_btn4.SetUp()

		except:
			import exception
			exception.Abort("OptionDialog.__Load_BindObject")

	def OpenOgolne(self):
		self.page = 0
		self.category_btn1.Down()
		self.category_btn2.SetUp()
		self.category_btn3.SetUp()
		self.category_btn4.SetUp()
		self.global_window.Show()
		self.desc_window.Hide()
		self.system_window.Hide()
		self.ukrywanie_window.Hide()
	
	def OpenSystemowe(self):
		self.page = 1
		self.category_btn1.SetUp()
		self.category_btn2.Down()
		self.category_btn3.SetUp()
		self.category_btn4.SetUp()
		self.desc_window.Hide()
		self.global_window.Hide()
		self.system_window.Show()
		self.ukrywanie_window.Hide()

	def OpenDesc(self):
		self.page = 2
		self.category_btn1.SetUp()
		self.category_btn2.SetUp()
		self.category_btn3.Down()
		self.category_btn4.SetUp()
		self.desc_window.Show()
		self.global_window.Hide()
		self.system_window.Hide()
		self.ukrywanie_window.Hide()
	
	def OpenUkrywanie(self):
		self.page = 3
		self.category_btn1.SetUp()
		self.category_btn2.SetUp()
		self.category_btn3.SetUp()
		self.category_btn4.Down()
		self.desc_window.Hide()
		self.global_window.Hide()
		self.system_window.Hide()
		self.ukrywanie_window.Show()

	def __Load(self):
		global MOBILE
		if MOBILE:
			self.__Load_LoadScript("uiscript/gameoptiondialog_formobile.py")
		else:
			self.__Load_LoadScript("uiscript/gameoptiondialog.py")

		self.__Load_BindObject()

		self.SetCenterPosition()

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))

		self.cameraModeButtonList[0].SAFE_SetEvent(self.__OnClickCameraModeShortButton)
		self.cameraModeButtonList[1].SAFE_SetEvent(self.__OnClickCameraModeLongButton)

		if app.ENABLE_FOV_OPTION:
			if self.fovController:
				self.fovController.SetSliderPos(float(systemSetting.GetFOV()) / float(app.MAX_CAMERA_PERSPECTIVE))
				self.fovController.SetEvent(ui.__mem_func__(self.__OnChangeFOV))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

			if self.fovResetButton:
				self.fovResetButton.SetEvent(ui.__mem_func__(self.__OnClickFOVResetButton))
		
		#if self.sklepyoffline_controller:
		#	self.sklepyoffline_controller.SetSliderPos(float(systemSetting.GetShowOfflineShopRange()))
		#	self.sklepyoffline_controller.SetEvent(ui.__mem_func__(self.__OnChangeFOV0))

		self.autobuff_off.SAFE_SetEvent(self.onclickautobuffOFF)
		self.autobuff_on.SAFE_SetEvent(self.onclickautobuffON)

		self.pety_off.SAFE_SetEvent(self.onclickPetyOFF)
		self.pety_on.SAFE_SetEvent(self.onclickPetyON)

		self.wierzchowce_off.SAFE_SetEvent(self.onclickWierzchOFF)
		self.wierzchowce_on.SAFE_SetEvent(self.onclickWierzchON)

		self.nameColorModeButtonList[0].SAFE_SetEvent(self.__OnClickNameColorModeNormalButton)
		self.nameColorModeButtonList[1].SAFE_SetEvent(self.__OnClickNameColorModeEmpireButton)

		self.viewTargetBoardButtonList[0].SAFE_SetEvent(self.__OnClickTargetBoardViewButton)
		self.viewTargetBoardButtonList[1].SAFE_SetEvent(self.__OnClickTargetBoardNoViewButton)

		self.pvpModeButtonDict[player.PK_MODE_PEACE].SAFE_SetEvent(self.__OnClickPvPModePeaceButton)
		self.pvpModeButtonDict[player.PK_MODE_REVENGE].SAFE_SetEvent(self.__OnClickPvPModeRevengeButton)
		self.pvpModeButtonDict[player.PK_MODE_GUILD].SAFE_SetEvent(self.__OnClickPvPModeGuildButton)
		self.pvpModeButtonDict[player.PK_MODE_FREE].SAFE_SetEvent(self.__OnClickPvPModeFreeButton)

		self.blockButtonList[0].SetToggleUpEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleUpEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleUpEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleUpEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleUpEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleUpEvent(self.__OnClickBlockPartyRequest)

		self.blockButtonList[0].SetToggleDownEvent(self.__OnClickBlockExchangeButton)
		self.blockButtonList[1].SetToggleDownEvent(self.__OnClickBlockPartyButton)
		self.blockButtonList[2].SetToggleDownEvent(self.__OnClickBlockGuildButton)
		self.blockButtonList[3].SetToggleDownEvent(self.__OnClickBlockWhisperButton)
		self.blockButtonList[4].SetToggleDownEvent(self.__OnClickBlockFriendButton)
		self.blockButtonList[5].SetToggleDownEvent(self.__OnClickBlockPartyRequest)

		self.viewChatButtonList[0].SAFE_SetEvent(self.__OnClickViewChatOnButton)
		self.viewChatButtonList[1].SAFE_SetEvent(self.__OnClickViewChatOffButton)

		self.alwaysShowNameButtonList[0].SAFE_SetEvent(self.__OnClickAlwaysShowNameOnButton)
		self.alwaysShowNameButtonList[1].SAFE_SetEvent(self.__OnClickAlwaysShowNameOffButton)

		self.showDamageButtonList[0].SAFE_SetEvent(self.__OnClickShowDamageOnButton)
		self.showDamageButtonList[1].SAFE_SetEvent(self.__OnClickShowDamageOffButton)

		self.showsalesTextButtonList[0].SAFE_SetEvent(self.__OnClickSalesTextOnButton)
		self.showsalesTextButtonList[1].SAFE_SetEvent(self.__OnClickSalesTextOffButton)
		
		self.desc0_on.SAFE_SetEvent(self.desc0_OnButton)
		self.desc0_off.SAFE_SetEvent(self.desc0_OffButton)

		self.desc1_on.SAFE_SetEvent(self.desc1_OnButton)
		self.desc1_off.SAFE_SetEvent(self.desc1_OffButton)

		self.desc2_on.SAFE_SetEvent(self.desc2_OnButton)
		self.desc2_off.SAFE_SetEvent(self.desc2_OffButton)

		self.desc3_on.SAFE_SetEvent(self.desc3_OnButton)
		self.desc3_off.SAFE_SetEvent(self.desc3_OffButton)

		if app.ENABLE_DOG_MODE:
			self.dogModeButtonList[0].SAFE_SetEvent(self.__OnClickDogButton)
			self.dogModeButtonList[1].SAFE_SetEvent(self.__OffClickDogButton)

		self.fonttype[0].SAFE_SetEvent(self.__OnClickFontChangeSmallButton)
		self.fonttype[1].SAFE_SetEvent(self.__OnClickFontChangeBigButton)

		if app.WJ_SHOW_MOB_INFO:
			self.showMobInfoButtonList[0].SetToggleUpEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[0].SetToggleDownEvent(self.__OnClickShowMobLevelButton)
			self.showMobInfoButtonList[1].SetToggleUpEvent(self.__OnClickShowMobAIFlagButton)
			self.showMobInfoButtonList[1].SetToggleDownEvent(self.__OnClickShowMobAIFlagButton)

		self.fogModeButtonList[0].SAFE_SetEvent(self.__OnClickFogModeLevel0Button)
		self.fogModeButtonList[1].SAFE_SetEvent(self.__OnClickFogModeLevel1Button)
		self.fogModeButtonList[2].SAFE_SetEvent(self.__OnClickFogModeLevel2Button)

		self.changeMusicButton.SAFE_SetEvent(self.__OnClickChangeMusicButton)
		self.ctrlMusicVolume.SetSliderPos(float(systemSetting.GetMusicVolume()))
		self.ctrlMusicVolume.SetEvent(ui.__mem_func__(self.OnChangeMusicVolume))
		self.ctrlSoundVolume.SetSliderPos(float(systemSetting.GetSoundVolume()) / 5.0)
		self.ctrlSoundVolume.SetEvent(ui.__mem_func__(self.OnChangeSoundVolume))

		self.tilingModeButtonList[0].SAFE_SetEvent(self.__OnClickTilingModeCPUButton)
		self.tilingModeButtonList[1].SAFE_SetEvent(self.__OnClickTilingModeGPUButton)
		self.tilingApplyButton.SAFE_SetEvent(self.__OnClickTilingApplyButton)
		self.__SetCurTilingMode()

		self.__ClickRadioButton(self.fogModeButtonList, constInfo.GET_FOG_LEVEL_INDEX())
		self.__ClickRadioButton(self.cameraModeButtonList, constInfo.GET_CAMERA_MAX_DISTANCE_INDEX())

		self.__ClickRadioButton(self.nameColorModeButtonList, constInfo.GET_CHRNAME_COLOR_INDEX())
		self.__ClickRadioButton(self.viewTargetBoardButtonList, constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD())

		if musicInfo.fieldMusic==musicInfo.METIN2THEMA:
			self.selectMusicFile.SetText(uiSelectMusic.DEFAULT_THEMA)
		else:
			self.selectMusicFile.SetText(musicInfo.fieldMusic[:MUSIC_FILENAME_MAX_LEN])

		self.__SetPeacePKMode()

		#global MOBILE
		if MOBILE:
			self.inputMobileButton.SetEvent(ui.__mem_func__(self.__OnChangeMobilePhoneNumber))
			self.deleteMobileButton.SetEvent(ui.__mem_func__(self.__OnDeleteMobilePhoneNumber))
		if app.ENABLE_DYNAMIC_FONTS:		
			self.BuildFonts()	
			if self.fonts != None:
				self.fonts.SAFE_SetEvent(self.__OnClickFonts)

	def __ClickRadioButton(self, buttonList, buttonIndex):
		try:
			selButton=buttonList[buttonIndex]
		except IndexError:
			return

		for eachButton in buttonList:
			eachButton.SetUp()

		selButton.Down()

	def __OnClickTilingModeCPUButton(self):
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_1)
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_2)
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_CPU_TILING_3)
		self.__SetTilingMode(0)

	def __OnClickTilingModeGPUButton(self):
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_1)
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_2)
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_GPU_TILING_3)
		self.__SetTilingMode(1)

	def __SetTilingMode(self, index):
		self.__ClickRadioButton(self.tilingModeButtonList, index)
		self.tilingMode=index

	def __OnClickTilingApplyButton(self):
		self.__NotifyChatLine(localeInfo.SYSTEM_OPTION_TILING_EXIT)
		if 0==self.tilingMode:
			background.EnableSoftwareTiling(1)
		else:
			background.EnableSoftwareTiling(0)

		net.ExitGame()

	def __SetNameColorMode(self, index):
		constInfo.SET_CHRNAME_COLOR_INDEX(index)
		self.__ClickRadioButton(self.nameColorModeButtonList, index)

	def __SetTargetBoardViewMode(self, flag):
		constInfo.SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(flag)
		self.__ClickRadioButton(self.viewTargetBoardButtonList, flag)

	def __OnClickNameColorModeNormalButton(self):
		self.__SetNameColorMode(0)

	def __OnClickNameColorModeEmpireButton(self):
		self.__SetNameColorMode(1)

	def __OnClickTargetBoardViewButton(self):
		self.__SetTargetBoardViewMode(0)

	def __OnClickTargetBoardNoViewButton(self):
		self.__SetTargetBoardViewMode(1)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	def __OnClickFogModeLevel0Button(self):
		self.__SetFogLevel(0)

	def __OnClickFogModeLevel1Button(self):
		self.__SetFogLevel(1)

	def __OnClickFogModeLevel2Button(self):
		self.__SetFogLevel(2)

	def __OnClickBlockExchangeButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_EXCHANGE))
	def __OnClickBlockPartyButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY))
	def __OnClickBlockGuildButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_GUILD))
	def __OnClickBlockWhisperButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_WHISPER))
	def __OnClickBlockFriendButton(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_FRIEND))
	def __OnClickBlockPartyRequest(self):
		self.RefreshBlock()
		global blockMode
		net.SendChatPacket("/setblockmode " + str(blockMode ^ player.BLOCK_PARTY_REQUEST))

	def __OnClickViewChatOnButton(self):
		global viewChatMode
		viewChatMode = 1
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()
	def __OnClickViewChatOffButton(self):
		global viewChatMode
		viewChatMode = 0
		systemSetting.SetViewChatFlag(viewChatMode)
		self.RefreshViewChat()

	def __OnClickAlwaysShowNameOnButton(self):
		systemSetting.SetAlwaysShowNameFlag(True)
		self.RefreshAlwaysShowName()

	def __OnClickAlwaysShowNameOffButton(self):
		systemSetting.SetAlwaysShowNameFlag(False)
		self.RefreshAlwaysShowName()

	def __OnClickShowDamageOnButton(self):
		systemSetting.SetShowDamageFlag(True)
		self.RefreshShowDamage()

	def __OnClickShowDamageOffButton(self):
		systemSetting.SetShowDamageFlag(False)
		self.RefreshShowDamage()

	def __OnClickFontChangeSmallButton(self):
		f = open("_cfg/font.cfg", "r+")
		f.write("0")
		f.close()
		self.RefreshFontText()
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Zmiana tej opcji wymaga ponownego uruchomienia gry.")
		self.fonttype[1].SetUp()

	def __OnClickFontChangeBigButton(self):
		f = open("_cfg/font.cfg", "r+")
		f.write("1")
		f.close()
		self.RefreshFontText()
		chat.AppendChat(chat.CHAT_TYPE_INFO, "Zmiana tej opcji wymaga ponownego uruchomienia gry.")
		self.fonttype[0].SetUp()

	def desc0_OnButton(self):
		constInfo.ZNAJDZ_SKRZYNIE_TOOLTIP = 1
		self.RefreshDesc0Btn()

	def desc0_OffButton(self):
		constInfo.ZNAJDZ_SKRZYNIE_TOOLTIP = 0
		self.RefreshDesc0Btn()

	def desc1_OnButton(self):
		constInfo.ROZDZIEL_TOOLTIP = 1
		self.RefreshDesc1Btn()

	def desc1_OffButton(self):
		constInfo.ROZDZIEL_TOOLTIP = 0
		self.RefreshDesc1Btn()

	def desc2_OnButton(self):
		constInfo.WYCIAGNIJ_TOOLTIP = 1
		self.RefreshDesc2Btn()

	def desc2_OffButton(self):
		constInfo.WYCIAGNIJ_TOOLTIP = 0
		self.RefreshDesc2Btn()

	def desc3_OnButton(self):
		constInfo.DODAJ_DODATKOWE_TOOLTIP = 1
		self.RefreshDesc3Btn()

	def desc3_OffButton(self):
		constInfo.DODAJ_DODATKOWE_TOOLTIP = 0
		self.RefreshDesc3Btn()

	def __OnClickSalesTextOnButton(self):
		systemSetting.SetShowSalesTextFlag(True)
		self.RefreshShowSalesText()
		if app.ENABLE_OFFLINE_SHOP_SYSTEM:
			uiOfflineShopBuilder.UpdateADBoard()
		uiPrivateShopBuilder.UpdateADBoard()

	def __OnClickSalesTextOffButton(self):
		systemSetting.SetShowSalesTextFlag(False)
		self.RefreshShowSalesText()

	if app.ENABLE_DOG_MODE:
		def __OnClickDogButton(self):
			systemSetting.SetDogMode(True)
			self.RefreshDogMode()

		def __OffClickDogButton(self):
			systemSetting.SetDogMode(False)
			self.RefreshDogMode()

		def RefreshDogMode(self):
			if systemSetting.GetDogMode():
				self.dogModeButtonList[0].Down()
				self.dogModeButtonList[1].SetUp()
			else:
				self.dogModeButtonList[0].SetUp()
				self.dogModeButtonList[1].Down()

	if app.WJ_SHOW_MOB_INFO:
		def __OnClickShowMobLevelButton(self):
			systemSetting.SetShowMobLevel(not systemSetting.IsShowMobLevel())
			self.RefreshShowMobInfo()
		def __OnClickShowMobAIFlagButton(self):
			systemSetting.SetShowMobAIFlag(not systemSetting.IsShowMobAIFlag())
			self.RefreshShowMobInfo()

	def __CheckPvPProtectedLevelPlayer(self):
		if player.GetStatus(player.LEVEL)<constInfo.PVPMODE_PROTECTED_LEVEL:
			self.__SetPeacePKMode()
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_PROTECT % (constInfo.PVPMODE_PROTECTED_LEVEL))
			return 1

		return 0

	def __SetPKMode(self, mode):
		for btn in self.pvpModeButtonDict.values():
			btn.SetUp()
		if self.pvpModeButtonDict.has_key(mode):
			self.pvpModeButtonDict[mode].Down()

	def __SetPeacePKMode(self):
		self.__SetPKMode(player.PK_MODE_PEACE)

	def __RefreshPVPButtonList(self):
		self.__SetPKMode(player.GetPKMode())

	def __OnClickPvPModePeaceButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 0", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeRevengeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 1", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeFreeButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 2", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def __OnClickPvPModeGuildButton(self):
		if self.__CheckPvPProtectedLevelPlayer():
			return

		self.__RefreshPVPButtonList()

		if 0 == player.GetGuildID():
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_CANNOT_SET_GUILD_MODE)
			return

		if constInfo.PVPMODE_ENABLE:
			net.SendChatPacket("/pkmode 4", chat.CHAT_TYPE_TALKING)
		else:
			chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.OPTION_PVPMODE_NOT_SUPPORT)

	def OnChangePKMode(self):
		self.__RefreshPVPButtonList()

	def __OnChangeMobilePhoneNumber(self):
		global MOBILE
		if not MOBILE:
			return

		import uiCommon
		inputDialog = uiCommon.InputDialog()
		inputDialog.SetTitle(localeInfo.MESSENGER_INPUT_MOBILE_PHONE_NUMBER_TITLE)
		inputDialog.SetMaxLength(13)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OnInputMobilePhoneNumber))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseInputDialog))
		inputDialog.Open()
		self.inputDialog = inputDialog

	def __OnDeleteMobilePhoneNumber(self):
		global MOBILE
		if not MOBILE:
			return

		import uiCommon
		questionDialog = uiCommon.QuestionDialog()
		questionDialog.SetText(localeInfo.MESSENGER_DO_YOU_DELETE_PHONE_NUMBER)
		questionDialog.SetAcceptEvent(ui.__mem_func__(self.OnDeleteMobile))
		questionDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseQuestionDialog))
		questionDialog.Open()
		self.questionDialog = questionDialog

	def OnInputMobilePhoneNumber(self):
		global MOBILE
		if not MOBILE:
			return

		text = self.inputDialog.GetText()

		if not text:
			return

		text.replace('-', '')
		net.SendChatPacket("/mobile " + text)
		self.OnCloseInputDialog()
		return True

	def OnInputMobileAuthorityCode(self):
		global MOBILE
		if not MOBILE:
			return

		text = self.inputDialog.GetText()
		net.SendChatPacket("/mobile_auth " + text)
		self.OnCloseInputDialog()
		return True

	def OnDeleteMobile(self):
		global MOBILE
		if not MOBILE:
			return

		net.SendChatPacket("/mobile")
		self.OnCloseQuestionDialog()
		return True

	def OnCloseInputDialog(self):
		self.inputDialog.Close()
		self.inputDialog = None
		return True

	def OnCloseQuestionDialog(self):
		self.questionDialog.Close()
		self.questionDialog = None
		return True

	def OnPressEscapeKey(self):
		self.Close()
		return True

	def RefreshMobile(self):
		global MOBILE
		if not MOBILE:
			return

		if player.HasMobilePhoneNumber():
			self.inputMobileButton.Hide()
			self.deleteMobileButton.Show()
		else:
			self.inputMobileButton.Show()
			self.deleteMobileButton.Hide()

	def OnMobileAuthority(self):
		global MOBILE
		if not MOBILE:
			return

		import uiCommon
		inputDialog = uiCommon.InputDialogWithDescription()
		inputDialog.SetTitle(localeInfo.MESSENGER_INPUT_MOBILE_AUTHORITY_TITLE)
		inputDialog.SetDescription(localeInfo.MESSENGER_INPUT_MOBILE_AUTHORITY_DESCRIPTION)
		inputDialog.SetAcceptEvent(ui.__mem_func__(self.OnInputMobileAuthorityCode))
		inputDialog.SetCancelEvent(ui.__mem_func__(self.OnCloseInputDialog))
		inputDialog.SetMaxLength(4)
		inputDialog.SetBoardWidth(310)
		inputDialog.Open()
		self.inputDialog = inputDialog

	def RefreshBlock(self):
		global blockMode
		for i in xrange(len(self.blockButtonList)):
			if 0 != (blockMode & (1 << i)):
				self.blockButtonList[i].Down()
			else:
				self.blockButtonList[i].SetUp()

	def RefreshViewChat(self):
		if systemSetting.IsViewChat():
			self.viewChatButtonList[0].Down()
			self.viewChatButtonList[1].SetUp()
		else:
			self.viewChatButtonList[0].SetUp()
			self.viewChatButtonList[1].Down()

	def RefreshAlwaysShowName(self):
		if systemSetting.IsAlwaysShowName():
			self.alwaysShowNameButtonList[0].Down()
			self.alwaysShowNameButtonList[1].SetUp()
		else:
			self.alwaysShowNameButtonList[0].SetUp()
			self.alwaysShowNameButtonList[1].Down()

	def RefreshShowDamage(self):
		if systemSetting.IsShowDamage():
			self.showDamageButtonList[0].Down()
			self.showDamageButtonList[1].SetUp()
		else:
			self.showDamageButtonList[0].SetUp()
			self.showDamageButtonList[1].Down()

	def RefreshDesc0Btn(self):
		if constInfo.ZNAJDZ_SKRZYNIE_TOOLTIP == 1:
			self.desc0_on.Down()
			self.desc0_off.SetUp()
		else:
			self.desc0_off.Down()
			self.desc0_on.SetUp()

	def RefreshDesc1Btn(self):
		if constInfo.ROZDZIEL_TOOLTIP == 1:
			self.desc1_on.Down()
			self.desc1_off.SetUp()
		else:
			self.desc1_off.Down()
			self.desc1_on.SetUp()

	def RefreshDesc2Btn(self):
		if constInfo.WYCIAGNIJ_TOOLTIP == 1:
			self.desc2_on.Down()
			self.desc2_off.SetUp()
		else:
			self.desc2_off.Down()
			self.desc2_on.SetUp()

	def RefreshDesc3Btn(self):
		if constInfo.DODAJ_DODATKOWE_TOOLTIP == 1:
			self.desc3_on.Down()
			self.desc3_off.SetUp()
		else:
			self.desc3_off.Down()
			self.desc3_on.SetUp()

	def RefreshShowSalesText(self):
		if systemSetting.IsShowSalesText():
			self.showsalesTextButtonList[0].Down()
			self.showsalesTextButtonList[1].SetUp()
		else:
			self.showsalesTextButtonList[0].SetUp()
			self.showsalesTextButtonList[1].Down()

	def RefreshFontText(self):
		f = open("_cfg/font.cfg", "r+")
		hehe = f.read()
		if hehe == "1":
			self.fonttype[1].Down()
		elif hehe == "0":
			self.fonttype[0].Down()
		else:
			self.fonttype[0].Down()
			self.fonttype[1].Down()

	if app.WJ_SHOW_MOB_INFO:
		def RefreshShowMobInfo(self):
			if systemSetting.IsShowMobLevel():
				self.showMobInfoButtonList[0].Down()
			else:
				self.showMobInfoButtonList[0].SetUp()
			if systemSetting.IsShowMobAIFlag():
				self.showMobInfoButtonList[1].Down()
			else:
				self.showMobInfoButtonList[1].SetUp()

	def OnBlockMode(self, mode):
		global blockMode
		blockMode = mode
		self.RefreshBlock()

	def Show(self):
		self.RefreshMobile()
		self.RefreshBlock()
		ui.ScriptWindow.Show(self)

	def Close(self):
		self.__SetCurTilingMode()
		self.Hide()
		if app.ENABLE_DYNAMIC_FONTS and self.FontsBoard:
			self.__CloseFonts()		
	if app.ENABLE_DYNAMIC_FONTS:
		def __OnClickFonts(self):
			if not self.FontsBoard:
				return
				
			_obj = self.FontsBoard[0]
			_obj.Show() if not _obj.IsShow() else _obj.Hide()
			_obj.SetTop()

	def __SetCurTilingMode(self):
		if background.IsSoftwareTiling():
			self.__SetTilingMode(0)
		else:
			self.__SetTilingMode(1)

	if app.ENABLE_DYNAMIC_FONTS:
		def RefreshFonts(self, pos = None):
			if pos == None:
				pos = int(localeInfo.GetCurrentFont()[0])

			for f in self.FontsBoard[1]:
				f.SetUp()
					
			self.FontsBoard[1][pos].Down()				
	
		def ChangeFont(self, pos):
			s = len(localeInfo.FONTS) - 1

			if pos >= 0 and pos <= s:
				constInfo.set_reg("font", str(pos))
				localeInfo.LoadFont(True)
				self.RefreshFonts(pos)
				
		def __CloseFonts(self):
			if self.FontsBoard:
				self.FontsBoard[0].Hide()
	
		def BuildFonts(self):
			FONTS = localeInfo.FONTS
			buttons = []
			
			b = ui.BoardWithTitleBar()
			b.AddFlag("movable")
			b.AddFlag("float")
			b.SetPosition(0, 0)
			b.SetSize(120, 63 + len(FONTS) * 18)
			b.SetTitleName("Czcionka")
			b.SetCloseEvent(ui.__mem_func__(self.__CloseFonts))
			b.Hide()
			b.SetCenterPosition()
			
			pos = 0
			for font in FONTS:			
				f = ui.RadioButton()
				f.SetParent(b)
				f.SetUpVisual("d:/ymir work/ui/public/large_button_01.sub")
				f.SetOverVisual("d:/ymir work/ui/public/large_button_02.sub")
				f.SetDownVisual("d:/ymir work/ui/public/large_button_03.sub")	
				f.SetWindowHorizontalAlignCenter()
				f.SetPosition(0, 35 + 20 * pos)		
				f.SetText(font)
				f.SetEvent(ui.__mem_func__(self.ChangeFont), pos)
				f.Show()
					
				f.SetDynamicFont(True)
				f.SetFontName(font)
					
				buttons.append(f)
				pos += 1
			
			self.FontsBoard = (b, buttons)
			self.RefreshFonts()

	def __NotifyChatLine(self, text):
		chat.AppendChat(chat.CHAT_TYPE_INFO, text)

	if app.ENABLE_FOV_OPTION:
		def __OnChangeFOV(self):
			pos = self.fovController.GetSliderPos()
			systemSetting.SetFOV(pos * float(app.MAX_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

		def __OnClickFOVResetButton(self):
			self.fovController.SetSliderPos(float(app.DEFAULT_CAMERA_PERSPECTIVE) / float(app.MAX_CAMERA_PERSPECTIVE))
			systemSetting.SetFOV(float(app.DEFAULT_CAMERA_PERSPECTIVE))

			if self.fovValueText:
				self.fovValueText.SetText(str(int(systemSetting.GetFOV())))

	def __OnChangeFOV0(self):
		pos = self.sklepyoffline_controller.GetSliderPos()
		#systemSetting.SetShowOfflineShopRange(pos * float(5000))

	def onclickautobuffOFF(self):
		systemSetting.SetShowAutoBuffMode(1)
		self.RefreshAutobuff()

	def onclickautobuffON(self):
		systemSetting.SetShowAutoBuffMode(0)
		self.RefreshAutobuff()

	def RefreshAutobuff(self):
		if systemSetting.GetShowAutoBuffMode() == 1:
			self.autobuff_off.Down()
			self.autobuff_on.SetUp()
		else:
			self.autobuff_off.SetUp()
			self.autobuff_on.Down()

	def onclickPetyOFF(self):
		systemSetting.SetShowPetMode(1)
		self.RefreshPety()

	def onclickPetyON(self):
		systemSetting.SetShowPetMode(0)
		self.RefreshPety()

	def RefreshPety(self):
		if systemSetting.GetShowPetMode() == 1:
			self.pety_off.Down()
			self.pety_on.SetUp()
		else:
			self.pety_off.SetUp()
			self.pety_on.Down()

	def onclickWierzchOFF(self):
		systemSetting.SetShowMountMode(1)
		self.RefreshWierzch()

	def onclickWierzchON(self):
		systemSetting.SetShowMountMode(0)
		self.RefreshWierzch()

	def RefreshWierzch(self):
		if systemSetting.GetShowMountMode() == 1:
			self.wierzchowce_off.Down()
			self.wierzchowce_on.SetUp()
		else:
			self.wierzchowce_off.SetUp()
			self.wierzchowce_on.Down()

	def __SetFogLevel(self, index):
		constInfo.SET_FOG_LEVEL_INDEX(index)
		self.__ClickRadioButton(self.fogModeButtonList, index)

	def __SetCameraMode(self, index):
		constInfo.SET_CAMERA_MAX_DISTANCE_INDEX(index)
		self.__ClickRadioButton(self.cameraModeButtonList, index)

	def __OnClickCameraModeShortButton(self):
		self.__SetCameraMode(0)

	def __OnClickCameraModeLongButton(self):
		self.__SetCameraMode(1)

	def __OnChangeMusic(self, fileName):
		self.selectMusicFile.SetText(fileName[:MUSIC_FILENAME_MAX_LEN])

		if musicInfo.fieldMusic != "":
			snd.FadeOutMusic("BGM/"+ musicInfo.fieldMusic)

		if fileName==uiSelectMusic.DEFAULT_THEMA:
			musicInfo.fieldMusic=musicInfo.METIN2THEMA
		else:
			musicInfo.fieldMusic=fileName

		musicInfo.SaveLastPlayFieldMusic()

		if musicInfo.fieldMusic != "":
			snd.FadeInMusic("BGM/" + musicInfo.fieldMusic)

	def __OnClickChangeMusicButton(self):
		if not self.musicListDlg:

			self.musicListDlg=uiSelectMusic.FileListDialog()
			self.musicListDlg.SAFE_SetSelectEvent(self.__OnChangeMusic)

		self.musicListDlg.Open()

	def OnChangeMusicVolume(self):
		pos = self.ctrlMusicVolume.GetSliderPos()
		snd.SetMusicVolume(pos * net.GetFieldMusicVolume())
		systemSetting.SetMusicVolume(pos)

	def OnChangeSoundVolume(self):
		pos = self.ctrlSoundVolume.GetSliderPos()
		snd.SetSoundVolumef(pos)
		systemSetting.SetSoundVolumef(pos)