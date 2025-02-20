from sys import path as sys_path
from os.path import splitext as op_splitext

libpath = r'..\..\..\CRootLib\Workplace\Py2Lib\lib'
sys_path.append(libpath)
rootpath = r'..\..\..\CRootLib\Workplace'
rootpath = r'.'

capNameList = {
	"logininfo":"loginInfo",
    "acce_absorbwindow":"Acce_AbsorbWindow",
    "acce_combinewindow":"Acce_CombineWindow",
    "_pety_script":"_Pety_script",
    "artefaktsystemwindow":"ArtefaktSystemWindow",
    "atlaswindow":"AtlasWindow",
    "attachstonedialog":"AttachStoneDialog",
    "aura_absorbwindow":"Aura_AbsorbWindow",
    "aura_levelupwindow":"Aura_LevelUpWindow",
    "battlepassdialog":"BattlepassDialog",
    "beltinventorywindow":"BeltInventoryWindow",
    "bonus":"Bonus",
    "bonuschanger":"BonusChanger",
    "buffnpccreatewindow":"BuffNpcCreateWindow",
    "buffnpcrenamewindow":"BuffNpcRenameWindow",
    "buffnpcselectskillwindow":"BuffNpcSelectSkillWindow",
    "buffnpcwindow":"BuffNpcWindow",
    "buffnpcwindow_new":"BuffNpcWindow_new",
    "buildguildbuildingwindow":"BuildGuildBuildingWindow",
    "changegradenamedialog":"ChangeGradeNameDialog",
    "changepassworddialog":"ChangePasswordDialog",
    "characterwindow":"CharacterWindow",
    "chatsettingwindow":"ChatSettingWindow",
    "chestdropinfowindow":"ChestDropInfoWindow",
    "collectionswindow":"CollectionsWindow",
    "collectwindow":"CollectWindow",
    "connectingdialog":"ConnectingDialog",
    "costumewindow":"CostumeWindow",
    "createcharacterwindow":"CreateCharacterWindow",
    "cuberenewalwindow":"CubeRenewalWindow",
    "cuberesultwindow":"CubeResultWindow",
    "cubewindow":"CubeWindow",
    "declareguildwardialog":"DeclareGuildWarDialog",
    "depoiswindow":"DepoIsWindow",
    "dopalaczewindow":"DopalaczeWindow",
    "dragonsoulrefinewindow":"DragonSoulRefineWindow",
    "dragonsoulwindow":"DragonSoulWindow",
    "dungeoninfowindow":"DungeonInfoWindow",
    "dungeonrankingwindow":"DungeonRankingWindow",
    "equipmentdialog":"EquipmentDialog",
    "eventcalendar":"EventCalendar",
    "exchangedialog":"ExchangeDialog",
    "expandedmoneytaskbar":"ExpandedMoneyTaskBar",
    "expandedtaskbar":"ExpandedTaskBar",
    "extendedinventorywindow":"ExtendedInventoryWindow",
    "fastequip":"FastEquip",
    "fishingwindow":"FishingWindow",
    "gameoptiondialog":"GameOptionDialog",
    "gameoptiondialog_formobile":"GameOptionDialog_forMobile",
    "gamewindow":"GameWindow",
    "giftcodewindow":"GiftCodeWindow",
    "guildwindow":"GuildWindow",
    "guildwindow_baseinfopage":"GuildWindow_BaseInfoPage",
    "guildwindow_boardpage":"GuildWindow_BoardPage",
    "guildwindow_gradepage":"GuildWindow_GradePage",
    "guildwindow_guildinfopage":"GuildWindow_GuildInfoPage",
    "guildwindow_guildinfopage_eu":"GuildWindow_GuildInfoPage_Eu",
    "guildwindow_guildinfopage_jp":"GuildWindow_GuildInfoPage_Jp",
    "guildwindow_guildskillpage":"GuildWindow_GuildSkillPage",
    "guildwindow_memberpage":"GuildWindow_MemberPage",
    "helpwindow":"HelpWindow",
    "helpwindow2":"HelpWindow2",
    "horseshoe":"HorseShoe",
    "ikonki":"Ikonki",
    "imekor":"IMEKOR",
    "inputdialog":"InputDialog",
    "inputdialogwithdescription":"InputDialogWithDescription",
    "inputdialogwithdescription2":"InputDialogWithDescription2",
    "inventorywindow":"InventoryWindow",
    "inventorywindowex":"InventoryWindowEx",
    "itemshopeditorwindow":"ItemShopEditorWindow",
    "itemshoppopupdialog":"ItemShopPopUpDialog",
    "itemshopselectattributedialog":"ItemShopSelectAttributeDialog",
    "itemshopselectlist":"ItemShopSelectList",
    "itemshopwindow":"ItemShopWindow",
    "keychange_window":"KeyChange_Window",
    "loadingwindow":"LoadingWindow",
    "locationrendermapdialog":"LocationRenderMapDialog",
    "locationwindow":"LocationWindow",
    "mallwindow":"MallWindow",
    "mapasw":"MapaSw",
    "marklistwindow":"MarkListWindow",
    "marmur_window":"Marmur_Window",
    "messengerwindow":"MessengerWindow",
    "minigamerumigamepage":"MiniGameRumiGamePage",
    "minigamerumiwaitingpage":"MiniGameRumiWaitingPage",
    "minigamewindow":"MiniGameWindow",
    "minimap":"MiniMap1",
    "minimapdungeoninfo":"MiniMapDungeonInfo",
    "moneyinputdialog":"MoneyInputDialog",
    "mount_window":"Mount_Window",
    "mousebuttonwindow":"MouseButtonWindow",
    "movechanneldialog":"MoveChannelDialog",
    "musiclistwindow":"MusicListWindow",
    "odlamkiwindow":"OdlamkiWindow",
    "partymemberinfoboard":"PartyMemberInfoBoard",
    "passworddialog":"PasswordDialog",
    "picketcdialog":"PickEtcDialog",
    "picketcdialog2":"PickEtcDialog2",
    "pickmoneydialog":"PickMoneyDialog",
    "popupdialog":"PopUpDialog",
    "privateshopbuilder":"PrivateShopBuilder",
    "questdialog":"QuestDialog",
    "questiondialog":"QuestionDialog",
    "questiondialog2":"QuestionDialog2",
    "refinedialog":"RefineDialog",
    "removeitemdialog":"RemoveItemDialog",
    "respdialog":"RespDialog",
    "respminimapdialog":"RespMiniMapDialog",
    "resprendermapdialog":"RespRenderMapDialog",
    "resprendertargetdialog":"RespRenderTargetDialog",
    "restartdialog":"RestartDialog",
    "rightmousebuttonwindow":"RightMouseButtonWindow",
    "runewindow":"RuneWindow",
    "safeboxwindow":"SafeBoxWindow",
    "selectcharacterwindow":"SelectCharacterWindow",
    "selectempirewindow":"SelectEmpireWindow",
    "selectitemwindow":"SelectItemWindow",
    "shopdialog":"ShopDialog",
    "shopsearch":"ShopSearch",
    "skillpointresetdialog":"SkillPointResetDialog",
    "stonedetach":"StoneDetach",
    "systemdialog":"SystemDialog",
    "systemdialog_formall":"SystemDialog_forMall",
    "systemdialog_forportal":"SystemDialog_forPortal",
    "systemoptiondialog":"SystemOptionDialog",
    "systemswindow":"SystemsWindow",
    "systemswindow_new":"SystemsWindow_new",
    "taskbar":"TaskBar",
    "warteschleife":"WarteSchleife",
    "webwindow":"WebWindow",
    "weeklyrank":"WeeklyRank",
    "whisperdialog":"WhisperDialog",
    "wonexchangewindow":"WonExchangeWindow",
    "skillchoosewindow":"SkillChooseWindow",
    "queuewindow":"QueueWindow",
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
sourceFileName = sourceWriter.run(moduleNameLst, 'uiscriptlib')
print "%s successfully created." % sourceFileName

