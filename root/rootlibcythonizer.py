from sys import path as sys_path
from os.path import splitext as op_splitext

libpath = r'..\Py2Lib\lib'
sys_path.append(libpath)
rootpath = r'.'

capNameList = {
	"colorinfo":"colorInfo",
	"consolemodule":"consoleModule",
	"constinfo":"constInfo",
	"debuginfo":"debugInfo",
	"dragon_soul_refine_settings":"dragon_soul_refine_settings",
	"emotion":"emotion",
	"exception":"exception",
	"game":"game",
	"interfacemodule":"interfaceModule",
	"introcreate":"introCreate",
	"introempire":"introEmpire",
	"introloading":"introLoading",
	"intrologin":"introLogin",
	"intrologo":"introLogo",
	"introselect":"introSelect",
	"localeinfo":"localeInfo",
	"mousemodule":"mouseModule",
	"musicinfo":"musicInfo",
	"networkmodule":"networkModule",
	"new_uiweeklyrank":"new_uiWeeklyRank",
	"playersettingmodule":"playerSettingModule",
	"prototype":"Prototype",
	"rootlibcythonizer":"RootLibCythonizer",
	"servercommandparser":"serverCommandParser",
	"serverinfo":"serverInfo",
	"stringcommander":"stringCommander",
	"system":"system",
	"test_affect":"test_affect",
	"ui":"ui",
	"uiacce":"uiAcce",
	"uiaffectshower":"uiAffectShower",
	"uiartefaktsystem":"uiArtefaktSystem",
	"uiattachmetin":"uiAttachMetin",
	"uiaura":"uiAura",
	"uiautoban":"uiAutoban",
	"uibattlepass":"uiBattlePass",
	"uiblend":"uiBlend",
	"uibonus":"uiBonus",
	"uibonuschanger":"uiBonusChanger",
	"uibuffnpc":"uiBuffNpc",
	"uicandidate":"uiCandidate",
	"uicards":"uiCards",
	"uicharacter":"uiCharacter",
	"uichat":"uiChat",
	"uichestdropinfo":"uiChestDropInfo",
	"uicollections":"uiCollections",
	"uicollectwindow":"uiCollectWindow",
	"uicommon":"uiCommon",
	"uicube":"uiCube",
	"uicuberenewal":"uiCubeRenewal",
	"uidepois":"uiDepoIs",
	"uidragonsoul":"uiDragonSoul",
	"uidungeoninfo":"uiDungeonInfo",
	"uiequipmentdialog":"uiEquipmentDialog",
	"uieventcalendar":"uiEventCalendar",
	"uiex":"uiEx",
	"uiexchange":"uiExchange",
	"uigamebutton":"uiGameButton",
	"uigameoption":"uiGameOption",
	"uigiftcodewindow":"uiGiftCodeWindow",
	"uiguild":"uiGuild",
	"uihelp":"uiHelp",
	"uiikonki":"uiIkonki",
	"uiinventory":"uiInventory",
	"uiitemshop":"uiItemShop",
	"uikeychange":"uiKeyChange",
	"uilegenddamagewindow":"uiLegendDamageWindow",
	"uilocation":"uiLocation",
	"uimapasw":"uiMapaSw",
	"uimapnameshower":"uiMapNameShower",
	"uimarbleshop":"uiMarbleShop",
	"uimessenger":"uiMessenger",
	"uiminimap":"uiMiniMap",
	"uiminimapdungeoninfo":"uiMiniMapDungeonInfo",
	"uimount":"uiMount",
	"uiodlamki":"uiOdlamki",
	"uiofflineshop":"uiOfflineShop",
	"uiofflineshopbuilder":"uiOfflineShopBuilder",
	"uioption":"uiOption",
	"uiparty":"uiParty",
	"uiphasecurtain":"uiPhaseCurtain",
	"uipicketc":"uiPickEtc",
	"uipickmoney":"uiPickMoney",
	"uiplayergauge":"uiPlayerGauge",
	"uipointreset":"uiPointReset",
	"uipotions":"uiPotions",
	"uiprivateshopbuilder":"uiPrivateShopBuilder",
	"uiquest":"uiQuest",
	"uirefine":"uiRefine",
	"uiremoveitem":"uiRemoveItem",
	"uiresp":"uiResp",
	"uirestart":"uiRestart",
	"uisafebox":"uiSafebox",
	"uiscriptlocale":"uiScriptLocale",
	"uiselectitem":"uiselectitem", #
	"uiselectmusic":"uiSelectMusic",
	"uishop":"uiShop",
	"uishopsearch":"uiShopSearch",
	"uiskillchoose":"uiSkillChoose",
	"uistonedetach":"uiStoneDetach",
	"uiswitchbot":"uiSwitchBot",
	"uisystem":"uiSystem",
	"uisystemoption":"uiSystemOption",
	"uitarget":"uiTarget",
	"uitaskbar":"uiTaskBar",
	"uitip":"uiTip",
	"uitooltip":"uiToolTip",
	"uiuploadmark":"uiUploadMark",
	"uiwarpshower":"uiWarpShower",
	"uiweb":"uiWeb",
	"uiwhisper":"uiWhisper",
	"uiwonexchange":"uiWonExchange",
	"utils":"utils",
	"zmianaeq":"zmianaEq",
	"_pety":"_Pety",
}

def checkCapName(x):
	base, ext = op_splitext(x)
	try:
		return capNameList[base.lower()] + ext
	except KeyError:
		return x

#import utils
import imp
fp, pathname, description = imp.find_module('utils', [libpath])
utils = imp.load_module('utils', fp, pathname, description)

pys = utils.findMatchedFiles(rootpath, "*.py")
if __file__ in pys:
	pys.remove(__file__)
pys = [checkCapName(x) for x in pys]

import cythonizer
moduleLst = cythonizer.run(pys, forceRecompile=True)
moduleNameLst = []
sourceFileLst = []

for m in moduleLst:
	for source in m.sources:
		base, ext = op_splitext(source)
		moduleName = base.split('/')[-1]
		moduleNameLst.append(moduleName)
		sourceFileLst.append(base + (".cpp" if "c++" == m.language else ".c"))

import sourceWriter
sourceFileName = sourceWriter.run(moduleNameLst, 'rootlib')
print "%s successfully created." % sourceFileName

