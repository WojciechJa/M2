import ui
import localeInfo
import chr
# import item
import app
import skill
import player
import uiToolTip
import math
# import constInfo
import uiCommon
import net
import chat
import emoji

# WEDDING
class LovePointImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/LovePoint/"
	FILE_DICT = {
		0 : FILE_PATH + "01.dds",
		1 : FILE_PATH + "02.dds",
		2 : FILE_PATH + "02.dds",
		3 : FILE_PATH + "03.dds",
		4 : FILE_PATH + "04.dds",
		5 : FILE_PATH + "05.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetLoverInfo(self, name, lovePoint):
		self.loverName = name
		self.lovePoint = lovePoint
		self.__Refresh()

	def OnUpdateLovePoint(self, lovePoint):
		self.lovePoint = lovePoint
		self.__Refresh()

	def __Refresh(self):
		self.lovePoint = max(0, self.lovePoint)
		self.lovePoint = min(100, self.lovePoint)

		if 0 == self.lovePoint:
			loveGrade = 0
		else:
			loveGrade = self.lovePoint / 25 + 1
		fileName = self.FILE_DICT.get(loveGrade, self.FILE_PATH+"00.dds")

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("LovePointImage.SetLoverInfo(lovePoint=%d) - LoadError %s" % (self.lovePoint, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()
		self.toolTip.SetTitle(self.loverName)
		self.toolTip.AppendTextLine(localeInfo.AFF_LOVE_POINT % (self.lovePoint))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
# END_OF_WEDDING


class HorseImage(ui.ExpandedImageBox):

	FILE_PATH = "d:/ymir work/ui/pattern/HorseState/"

	FILE_DICT = {
		00 : FILE_PATH+"00.dds",
		01 : FILE_PATH+"00.dds",
		02 : FILE_PATH+"00.dds",
		03 : FILE_PATH+"00.dds",
		10 : FILE_PATH+"10.dds",
		11 : FILE_PATH+"11.dds",
		12 : FILE_PATH+"12.dds",
		13 : FILE_PATH+"13.dds",
		20 : FILE_PATH+"20.dds",
		21 : FILE_PATH+"21.dds",
		22 : FILE_PATH+"22.dds",
		23 : FILE_PATH+"23.dds",
		30 : FILE_PATH+"30.dds",
		31 : FILE_PATH+"31.dds",
		32 : FILE_PATH+"32.dds",
		33 : FILE_PATH+"33.dds",
	}

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		#self.textLineList = []
		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __GetHorseGrade(self, level):
		if 0 == level:
			return 0

		return (level-1)/10 + 1

	def SetState(self, level, health, battery):
		#self.textLineList=[]
		self.toolTip.ClearToolTip()

		if level>0:

			try:
				grade = self.__GetHorseGrade(level)
				self.__AppendText(localeInfo.LEVEL_LIST[grade])
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			try:
				healthName=localeInfo.HEALTH_LIST[health]
				if len(healthName)>0:
					self.__AppendText(healthName)
			except IndexError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - Unknown Index" % (level, health, battery)
				return

			if health>0:
				if battery==0:
					self.__AppendText(localeInfo.NEEFD_REST)

			try:
				fileName=self.FILE_DICT[health*10+battery]
			except KeyError:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - KeyError" % (level, health, battery)

			try:
				self.LoadImage(fileName)
			except:
				print "HorseImage.SetState(level=%d, health=%d, battery=%d) - LoadError %s" % (level, health, battery, fileName)

		self.SetScale(0.7, 0.7)

	def __AppendText(self, text):

		self.toolTip.AppendTextLine(text)
		# self.toolTip.ResizeToolTip()

		#x=self.GetWidth()/2
		#textLine = ui.TextLine()
		#textLine.SetParent(self)
		#textLine.SetSize(0, 0)
		#textLine.SetOutline()
		#textLine.Hide()
		#textLine.SetPosition(x, 40+len(self.textLineList)*16)
		#textLine.SetText(text)
		#self.textLineList.append(textLine)

	def OnMouseOverIn(self):
		#for textLine in self.textLineList:
		#	textLine.Show()

		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		#for textLine in self.textLineList:
		#	textLine.Hide()

		self.toolTip.HideToolTip()


# AUTO_POTION
class AutoPotionImage(ui.ExpandedImageBox):

	FILE_PATH_HP = "d:/ymir work/ui/pattern/auto_hpgauge/"
	FILE_PATH_SP = "d:/ymir work/ui/pattern/auto_spgauge/"

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.loverName = ""
		self.lovePoint = 0
		self.potionType = player.AUTO_POTION_TYPE_HP
		self.filePath = ""

		self.toolTip = uiToolTip.ToolTip(100)
		self.toolTip.HideToolTip()

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetPotionType(self, type):
		self.potionType = type

		if player.AUTO_POTION_TYPE_HP == type:
			self.filePath = self.FILE_PATH_HP
		elif player.AUTO_POTION_TYPE_SP == type:
			self.filePath = self.FILE_PATH_SP


	def OnUpdateAutoPotionImage(self):
		self.__Refresh()

	def __Refresh(self):
		print "__Refresh"

		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(self.potionType)

		amountPercent = (float(currentAmount) / totalAmount) * 100.0
		grade = math.ceil(amountPercent / 20)

		if 5.0 > amountPercent:
			grade = 0

		if 80.0 < amountPercent:
			grade = 4
			if 90.0 < amountPercent:
				grade = 5

		fmt = self.filePath + "%.2d.dds"
		fileName = fmt % grade

		print self.potionType, amountPercent, fileName

		try:
			self.LoadImage(fileName)
		except:
			import dbg
			dbg.TraceError("AutoPotionImage.__Refresh(potionType=%d) - LoadError %s" % (self.potionType, fileName))

		self.SetScale(0.7, 0.7)

		self.toolTip.ClearToolTip()

		if player.AUTO_POTION_TYPE_HP == type:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_HP)
		else:
			self.toolTip.SetTitle(localeInfo.TOOLTIP_AUTO_POTION_SP)

		self.toolTip.AppendTextLine(localeInfo.TOOLTIP_AUTO_POTION_REST	% (amountPercent))
		self.toolTip.ResizeToolTip()

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
# END_OF_AUTO_POTION


class AffectImage(ui.ExpandedImageBox):

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)

		self.toolTip = uiToolTip.ToolTip()
		self.toolTip.HideToolTip()

		self.isSkillAffect = True
		self.description = None
		self.endTime = 0
		self.affect = None
		self.isClocked = True
		self.polymorphQuestionDialog = None
		# if app.ENABLE_SKILL_AFFECT_REMOVE:
		self.skillAffectQuestionDialog = None
		self.buffQuestionDialog = None
		self.extraQuestionDialog = None
		self.skillIndex = None
		# self.SetEvent(ui.__mem_func__(self.OnBuffQuestionDialog), "MOUSE_LEFT_BUTTON_UP")

	def SetAffect(self, affect):
		self.affect = affect

	def GetAffect(self):
		return self.affect

	def FormatTime(self, time):
		text = ""

		d = time // (24 * 3600)
		time = time % (24 * 3600)
		h = time // 3600
		time %= 3600
		m = time // 60
		time %= 60
		s = time

		if d:
			text += "%dd " % d
		if text or h:
			text += "%dg " % h
		if text or m:
			text += "%dm " % m
		if text or s:
			text += "%ds " % s

		return text[:-1]

	def SetToolTipText(self, text, x = 0, y = -19):
		self.toolTip.ClearToolTip()
		self.toolTip.AppendSpace(-5)
		self.toolTip.AppendDescription(text, 26)

	def SetDescription(self, description):
		self.description = description
		self.__UpdateDescription2()

	def SetDuration(self, duration):
		self.endTime = 0
		if duration > 0:
			self.endTime = app.GetGlobalTimeStamp() + duration
			leftTime = self.FormatTime(self.endTime - app.GetGlobalTimeStamp())
			self.toolTip.AppendHorizontalLine()
			self.toolTip.AppendTextLine("|cFFBEB47DPozosta퀉 czas:")
			self.toolTip.AppendTextLine(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+leftTime)
	
	def SetExtraDescriptions(self, affect):
		if affect == chr.AFFECT_RUNE_DECK5:
			self.toolTip.AppendHorizontalLine()
			self.toolTip.AppendTextLine("Warto쒏 Ataku +20%", self.toolTip.POSITIVE_COLOR)
			self.toolTip.AppendTextLine("똱ednie Obra풽nia +15%", self.toolTip.POSITIVE_COLOR)
			self.toolTip.AppendTextLine("Silny przeciwko Ludziom +25%", self.toolTip.POSITIVE_COLOR)
		elif affect == chr.AFFECT_RUNE_DECK9:
			self.toolTip.AppendHorizontalLine()
			self.toolTip.AppendTextLine("Warto쒏 Ataku +30%", self.toolTip.POSITIVE_COLOR)
			self.toolTip.AppendTextLine("똱ednie Obra풽nia +25%", self.toolTip.POSITIVE_COLOR)
			self.toolTip.AppendTextLine("Silny przeciwko Ludziom +40%", self.toolTip.POSITIVE_COLOR)

	def UpdateAutoPotionDescription(self):
		potionType = player.AUTO_POTION_TYPE_HP if self.affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY\
			else player.AUTO_POTION_TYPE_SP
		isActivated, currentAmount, totalAmount, slotIndex = player.GetAutoPotionInfo(potionType)

		try:
			amountPercent = (float(currentAmount) / totalAmount) * 100.0
		except:
			amountPercent = 100.0

		self.toolTip.childrenList[-1].SetText(self.description % amountPercent)

	def SetClock(self, isClocked):
		self.isClocked = isClocked
		self.SetDescription(self.description)
		
	def UpdateDescription(self):
		if not self.isClocked:
			return
	
		if not self.description:
			return

		if self.endTime > 0:
			leftTime = self.FormatTime(self.endTime - app.GetGlobalTimeStamp())

			self.toolTip.childrenList[-1].SetText(emoji.AppendEmoji("icon/emoji/image-example-007.png")+" "+leftTime)

	def __UpdateDescription2(self):
		if not self.description:
			return

		toolTip = self.description
		self.SetToolTipText(toolTip, 0, 40)

	def SetSkillAffectFlag(self, flag):
		self.isSkillAffect = flag
	def SetSkillIndex(self, skillIndex):
		self.skillIndex = skillIndex

	def IsSkillAffect(self):
		return self.isSkillAffect
		
	def OnPolymorphQuestionDialog(self):
		import uiCommon
		self.polymorphQuestionDialog = uiCommon.QuestionDialog()
		self.polymorphQuestionDialog.SetText(localeInfo.POLYMORPH_AFFECT_REMOVE_QUESTION)
		self.polymorphQuestionDialog.SetWidth(350)
		self.polymorphQuestionDialog.SetAcceptEvent(lambda arg = TRUE: self.OnClosePolymorphQuestionDialog(arg))
		self.polymorphQuestionDialog.SetCancelEvent(lambda arg = FALSE: self.OnClosePolymorphQuestionDialog(arg))
		self.polymorphQuestionDialog.Open()
		
	def OnClosePolymorphQuestionDialog(self, answer):

		if not self.polymorphQuestionDialog:
			return

		self.polymorphQuestionDialog.Close()
		self.polymorphQuestionDialog = None
				
		if not answer:
			return

		net.SendChatPacket("/remove_polymorph")
		return TRUE
		
	def OnBuffQuestionDialog(self):
		skillIndex = self.skillIndex
		if not skillIndex or skillIndex == 66:
			return
		self.buffQuestionDialog = uiCommon.QuestionDialog()
		self.buffQuestionDialog.SetWidth(350)
		self.buffQuestionDialog.SetText(localeInfo.BUFF_AFFECT_REMOVE_QUESTION % (skill.GetSkillName(skillIndex)))
		self.buffQuestionDialog.SetAcceptEvent(lambda arg = skillIndex: self.OnCloseBuffQuestionDialog(arg))
		self.buffQuestionDialog.SetCancelEvent(lambda arg = 0: self.OnCloseBuffQuestionDialog(arg))
		self.buffQuestionDialog.Open()
		
	def OnCloseBuffQuestionDialog(self, answer):
		if not self.buffQuestionDialog:
			return

		self.buffQuestionDialog.Close()
		self.buffQuestionDialog = None

		if not answer:
			return

		net.SendChatPacket("/remove_skill_affect %d" % answer)
		return TRUE

	def OnExtraQuestionDialog(self):
		affect = self.affect
		if not affect:
			return
		self.extraQuestionDialog = uiCommon.QuestionDialog()
		self.extraQuestionDialog.SetWidth(350)
		self.extraQuestionDialog.SetText("Czy na pewno chcesz usun방 ten bonus?")
		self.extraQuestionDialog.SetAcceptEvent(lambda arg = affect: self.OnCloseExtraQuestionDialog(arg))
		self.extraQuestionDialog.SetCancelEvent(lambda arg = 0: self.OnCloseExtraQuestionDialog(arg))
		self.extraQuestionDialog.Open()
		
	def OnCloseExtraQuestionDialog(self, answer):
		if not self.extraQuestionDialog:
			return

		self.extraQuestionDialog.Close()
		self.extraQuestionDialog = None

		if not answer:
			return

		affect_map = {
			1116: {510},
		}

		if answer not in affect_map:
			return

		net.SendChatPacket("/remove_skill_affect %d" % next(iter(affect_map[answer])))
		return True

	def OnMouseOverIn(self):
		self.toolTip.ShowToolTip()

	def OnMouseOverOut(self):
		self.toolTip.HideToolTip()
	
	def OnMouseLeftButtonUp(self):
		if self.affect == chr.NEW_AFFECT_POLYMORPH:
			self.OnPolymorphQuestionDialog()

		if self.affect == 1116:
			self.OnExtraQuestionDialog()

		skillIndex = self.skillIndex
		if skillIndex == 3 or skillIndex == 4:
			self.OnBuffQuestionDialog()

class AffectShower(ui.Window):

	MALL_DESC_IDX_START = 1000
	# if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
	DEW_DESC_IDX_START = 1200
	FISH_DESC_IDX_START = 800
	WATER_DESC_IDX_START = 1400
	DRAGON_GOD_DESC_IDX_START = 1600
	IMAGE_STEP = 25
	AFFECT_MAX_NUM = 32

	INFINITE_AFFECT_DURATION = 0x1FFFFFFF

	AFFECT_DATA_DICT =	{
			chr.AFFECT_POISON : (localeInfo.SKILL_TOXICDIE, "d:/ymir work/ui/skill/common/affect/poison.sub"),
			chr.AFFECT_SLOW : (localeInfo.SKILL_SLOW, "d:/ymir work/ui/skill/common/affect/slow.sub"),
			chr.AFFECT_STUN : (localeInfo.SKILL_STUN, "d:/ymir work/ui/skill/common/affect/stun.sub"),

			chr.AFFECT_ATT_SPEED_POTION : (localeInfo.SKILL_INC_ATKSPD, "d:/ymir work/ui/skill/common/affect/Increase_Attack_Speed.sub"),
			chr.AFFECT_MOV_SPEED_POTION : (localeInfo.SKILL_INC_MOVSPD, "d:/ymir work/ui/skill/common/affect/Increase_Move_Speed.sub"),
			chr.AFFECT_FISH_MIND : (localeInfo.SKILL_FISHMIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub"),

			chr.AFFECT_JEONGWI : (localeInfo.SKILL_JEONGWI, "d:/ymir work/ui/skill/warrior/jeongwi_03.sub",),
			chr.AFFECT_GEOMGYEONG : (localeInfo.SKILL_GEOMGYEONG, "d:/ymir work/ui/skill/warrior/geomgyeong_03.sub",),
			chr.AFFECT_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			chr.AFFECT_GYEONGGONG : (localeInfo.SKILL_GYEONGGONG, "d:/ymir work/ui/skill/assassin/gyeonggong_03.sub",),
			chr.AFFECT_EUNHYEONG : (localeInfo.SKILL_EUNHYEONG, "d:/ymir work/ui/skill/assassin/eunhyeong_03.sub",),
			chr.AFFECT_GWIGEOM : (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/sura/gwigeom_03.sub",),
			chr.AFFECT_GONGPO : (localeInfo.SKILL_GONGPO, "d:/ymir work/ui/skill/sura/gongpo_03.sub",),
			chr.AFFECT_JUMAGAP : (localeInfo.SKILL_JUMAGAP, "d:/ymir work/ui/skill/sura/jumagap_03.sub"),
			chr.AFFECT_HOSIN : (localeInfo.SKILL_HOSIN, "d:/ymir work/ui/skill/shaman/hosin_03.sub",),
			chr.AFFECT_BOHO : (localeInfo.SKILL_BOHO, "d:/ymir work/ui/skill/shaman/boho_03.sub",),
			chr.AFFECT_KWAESOK : (localeInfo.SKILL_KWAESOK, "d:/ymir work/ui/skill/shaman/kwaesok_03.sub",),
			chr.AFFECT_HEUKSIN : (localeInfo.SKILL_HEUKSIN, "d:/ymir work/ui/skill/sura/heuksin_03.sub",),
			chr.AFFECT_MUYEONG : (localeInfo.SKILL_MUYEONG, "d:/ymir work/ui/skill/sura/muyeong_03.sub",),
			chr.AFFECT_GICHEON : (localeInfo.SKILL_GICHEON, "d:/ymir work/ui/skill/shaman/gicheon_03.sub",),
			chr.AFFECT_JEUNGRYEOK : (localeInfo.SKILL_JEUNGRYEOK, "d:/ymir work/ui/skill/shaman/jeungryeok_03.sub",),

			chr.AFFECT_PABEOP : (localeInfo.SKILL_PABEOP, "d:/ymir work/ui/skill/sura/pabeop_03.sub",),
			chr.AFFECT_FALLEN_CHEONGEUN : (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/warrior/cheongeun_03.sub",),
			28 : (localeInfo.SKILL_FIRE, "d:/ymir work/ui/skill/sura/hwayeom_03.sub",),
			chr.AFFECT_CHINA_FIREWORK : (localeInfo.SKILL_POWERFUL_STRIKE, "d:/ymir work/ui/skill/common/affect/powerfulstrike.sub",),

			#64 - END
			chr.NEW_AFFECT_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_STATIC, "d:/ymir work/ui/game/affectshower/pdvip.tga",),
			chr.NEW_AFFECT_ITEM_BONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS_STATIC_NEW, "d:/ymir work/ui/game/affectshower/itemvip.tga",),

			chr.NEW_AFFECT_SAFEBOX : (localeInfo.TOOLTIP_MALL_SAFEBOX, "d:/ymir work/ui/skill/common/affect/safebox.sub",),
			chr.NEW_AFFECT_AUTOLOOT : (localeInfo.TOOLTIP_MALL_AUTOLOOT, "d:/ymir work/ui/skill/common/affect/autoloot.sub",),
			chr.NEW_AFFECT_FISH_MIND : (localeInfo.TOOLTIP_MALL_FISH_MIND, "d:/ymir work/ui/skill/common/affect/fishmind.sub",),
			chr.NEW_AFFECT_MARRIAGE_FAST : (localeInfo.TOOLTIP_MALL_MARRIAGE_FAST, "d:/ymir work/ui/skill/common/affect/marriage_fast.sub",),
			chr.NEW_AFFECT_GOLD_BONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),
			chr.AFFECT_PICKUP : (localeInfo.TOOLTIP_MALL_PICKUP, "d:/ymir work/ui/skill/common/affect/item_bonus_p_on.sub",),

			chr.NEW_AFFECT_NO_DEATH_PENALTY : (localeInfo.TOOLTIP_APPLY_NO_DEATH_PENALTY, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			chr.NEW_AFFECT_SKILL_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada.tga"),
			chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY : (localeInfo.TOOLTIP_APPLY_SKILL_BOOK_NO_DELAY, "d:/ymir work/ui/game/affectshower/egzo.tga"),
			
			chr.NEW_AFFECT_SKILE_PAS_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_PAS_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada0.tga"),
			chr.NEW_AFFECT_SKILE_PAS_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_PAS_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo0.tga"),
			
			chr.NEW_AFFECT_SKILE_PET_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_PET_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada2.tga"),
			chr.NEW_AFFECT_SKILE_PET_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_PET_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo2.tga"),
			
			chr.NEW_AFFECT_SKILE_BUFF_BOOK_BONUS : (localeInfo.TOOLTIP_APPLY_SKILE_BUFF_BOOK_BONUS, "d:/ymir work/ui/game/affectshower/rada1.tga"),
			chr.NEW_AFFECT_SKILE_BUFF_NO_BOOK_DELAY : (localeInfo.TOOLTIP_APPLY_SKILE_BUFF_NO_BOOK_DELAY, "d:/ymir work/ui/game/affectshower/egzo1.tga"),

			chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_hpgauge/05.dds"),
			chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/pattern/auto_spgauge/05.dds"),

			551 : (localeInfo.TOOLTIP_PASYWKI_MASTER, "d:/ymir work/ui/game/affectshower/pasywki.tga"),

			#chr.NEW_AFFECT_AUTO_HP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			#chr.NEW_AFFECT_AUTO_SP_RECOVERY : (localeInfo.TOOLTIP_AUTO_POTION_REST, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub"),

			MALL_DESC_IDX_START+player.POINT_MALL_ATTBONUS : (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/att_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_MALL_DEFBONUS : (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/skill/common/affect/def_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_MALL_EXPBONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/exp.tga",),
			MALL_DESC_IDX_START+player.POINT_MALL_ITEMBONUS : (localeInfo.TOOLTIP_MALL_ITEMBONUS, "d:/ymir work/ui/skill/common/affect/item_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_MALL_GOLDBONUS : (localeInfo.TOOLTIP_MALL_GOLDBONUS, "d:/ymir work/ui/skill/common/affect/gold_bonus.sub",),
			MALL_DESC_IDX_START+player.POINT_CRITICAL_PCT : (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT,"d:/ymir work/ui/skill/common/affect/critical.sub"),
			MALL_DESC_IDX_START+player.POINT_PENETRATE_PCT : (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_HP_PCT : (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),
			MALL_DESC_IDX_START+player.POINT_MAX_SP_PCT : (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/skill/common/affect/gold_premium.sub"),

			MALL_DESC_IDX_START+player.POINT_PC_BANG_EXP_BONUS : (localeInfo.TOOLTIP_MALL_EXPBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/EXP_Bonus_p_on.sub",),
			MALL_DESC_IDX_START+player.POINT_PC_BANG_DROP_BONUS: (localeInfo.TOOLTIP_MALL_ITEMBONUS_P_STATIC, "d:/ymir work/ui/skill/common/affect/Item_Bonus_p_on.sub",),
	}
	if app.ENABLE_DRAGON_SOUL_SYSTEM:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga")
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga")

	if app.ENABLE_AFFECT_POLYMORPH_REMOVE:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_POLYMORPH] = (localeInfo.POLYMORPH_AFFECT_TOOLTIP, "d:/ymir work/ui/polymorph_marble_icon.tga")

	if app.__AUTO_QUQUE_ATTACK__:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_AUTO_METIN_FARM] =  (localeInfo.NEW_AFFECT_AUTO_METIN_FARM, "d:/ymir work/ui/game/affectshower/61400.tga")

	if app.ENABLE_USE_FISH_SYSTEM:
		AFFECT_DATA_DICT[812] = (localeInfo.TOOLTIP_STR, "d:/ymir work/ui/game/affectshower/80400.tga")
		AFFECT_DATA_DICT[848] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_DEVIL, "d:/ymir work/ui/game/affectshower/80401.tga")
		AFFECT_DATA_DICT[815] = (localeInfo.TOOLTIP_INT, "d:/ymir work/ui/game/affectshower/80402.tga")
		AFFECT_DATA_DICT[814] = (localeInfo.TOOLTIP_DEX, "d:/ymir work/ui/game/affectshower/80403.tga")
		AFFECT_DATA_DICT[853] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "d:/ymir work/ui/game/affectshower/80404.tga")
		AFFECT_DATA_DICT[922] = (localeInfo.TOOLTIP_NORMAL_HIT_DAMAGE_BONUS, "d:/ymir work/ui/game/affectshower/80405.tga")
		AFFECT_DATA_DICT[883] = (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/80406.tga")
		AFFECT_DATA_DICT[895] = (localeInfo.TOOLTIP_ATT_GRADE, "d:/ymir work/ui/game/affectshower/80407.tga")
		AFFECT_DATA_DICT[952] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS, "d:/ymir work/ui/game/affectshower/80408.tga")

	AFFECT_DATA_DICT[746] = (localeInfo.RUNE_DECK5_TOOLTIP, "d:/ymir work/ui/game/affectshower/runy.tga")
	AFFECT_DATA_DICT[747] = (localeInfo.RUNE_DECK9_TOOLTIP, "d:/ymir work/ui/game/affectshower/runy.tga")

	# if constInfo.ENABLE_POTIONS_AFFECTSHOWER:
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, 	"d:/ymir work/ui/game/affectshower/50813.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, 	"d:/ymir work/ui/game/affectshower/50814.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, 			"d:/ymir work/ui/game/affectshower/50817.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, 			"d:/ymir work/ui/game/affectshower/50818.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_MAGIC_DEF_GRADE, 		"d:/ymir work/ui/game/affectshower/50819.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, 			"d:/ymir work/ui/game/affectshower/50820.tga")
	
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, 	"d:/ymir work/ui/game/affectshower/50821.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, 	"d:/ymir work/ui/game/affectshower/50822.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_SPEED] = (localeInfo.TOOLTIP_ATT_SPEED, 			"d:/ymir work/ui/game/affectshower/50823.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.RESIST_MAGIC] = (localeInfo.TOOLTIP_RESIST_MAGIC, 			"d:/ymir work/ui/game/affectshower/50824.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.ATT_BONUS] = (localeInfo.TOOLTIP_ATT_GRADE, 			"d:/ymir work/ui/game/affectshower/50825.tga")
	AFFECT_DATA_DICT[WATER_DESC_IDX_START+player.DEF_BONUS] = (localeInfo.TOOLTIP_DEF_GRADE, 			"d:/ymir work/ui/game/affectshower/50826.tga")
	
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+player.ENERGY] = (localeInfo.TOOLTIP_ENERGY, 			"d:/ymir work/ui/game/affectshower/51002.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_HP_PCT] = (localeInfo.TOOLTIP_MAX_HP_PCT, "d:/ymir work/ui/game/affectshower/71027.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MAX_SP_PCT] = (localeInfo.TOOLTIP_MAX_SP_PCT, "d:/ymir work/ui/game/affectshower/71029.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+132] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71028.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+93] = (localeInfo.TOOLTIP_MALL_ATTBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71028.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_MALL_DEFBONUS] = (localeInfo.TOOLTIP_MALL_DEFBONUS_STATIC, "d:/ymir work/ui/game/affectshower/71030.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_CRITICAL_PCT] = (localeInfo.TOOLTIP_APPLY_CRITICAL_PCT, "d:/ymir work/ui/game/affectshower/71044.tga")
	AFFECT_DATA_DICT[DRAGON_GOD_DESC_IDX_START+player.POINT_PENETRATE_PCT] = (localeInfo.TOOLTIP_APPLY_PENETRATE_PCT, "d:/ymir work/ui/game/affectshower/71045.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+152] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_BOSS, "d:/ymir work/ui/game/affectshower/50839.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+53] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_MONSTER, "d:/ymir work/ui/game/affectshower/50837.tga")
	AFFECT_DATA_DICT[DEW_DESC_IDX_START+154] = (localeInfo.TOOLTIP_APPLY_ATTBONUS_STONE, "d:/ymir work/ui/game/affectshower/50838.tga")

	AFFECT_DATA_DICT[1217] = (localeInfo.TOOLTIP_MOV_SPEED, "d:/ymir work/ui/game/affectshower/27109.tga")
	AFFECT_DATA_DICT[1219] = (localeInfo.TOOLTIP_ATT_SPEED, "d:/ymir work/ui/game/affectshower/27105.tga")

	AFFECT_DATA_DICT[750] = (localeInfo.TOOLTIP_MALL_EXPBONUS, "d:/ymir work/ui/game/affectshower/71153.tga")

	if app.ENABLE_WOLFMAN_CHARACTER:
		AFFECT_DATA_DICT[chr.AFFECT_BLEEDING] = (localeInfo.SKILL_BLEEDING, "d:/ymir work/ui/skill/common/affect/poison.sub")
		AFFECT_DATA_DICT[chr.AFFECT_RED_POSSESSION] = (localeInfo.SKILL_GWIGEOM, "d:/ymir work/ui/skill/wolfman/red_possession_03.sub")
		AFFECT_DATA_DICT[chr.AFFECT_BLUE_POSSESSION] = (localeInfo.SKILL_CHEONGEUN, "d:/ymir work/ui/skill/wolfman/blue_possession_03.sub")

	if app.ENABLE_PREMIUM_PRIVATE_SHOP_OFFICIAL:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_PREMIUM_PRIVATE_SHOP] = (localeInfo.TOOLTIP_AFFECT_PREMIUM_PRIVATE_SHOP, "d:/ymir work/ui/skill/common/affect/premium_private_shop.sub")

	def __init__(self):
		ui.Window.__init__(self)

		self.serverPlayTime=0
		self.clientPlayTime=0

		self.lastUpdateTime=0
		self.affectImageDict={}
		self.horseImage=None
		self.lovePointImage=None
		self.autoPotionImageHP = AutoPotionImage()
		self.autoPotionImageSP = AutoPotionImage()
		self.SetPosition(10, 10)
		self.Show()
		
	def ClearAllAffects(self):
		self.horseImage=None
		self.lovePointImage=None
		self.affectImageDict={}
		self.__ArrangeImageList()

	def ClearAffects(self):
		self.living_affectImageDict={}
		for key, image in self.affectImageDict.items():
			if not image.IsSkillAffect():
				self.living_affectImageDict[key] = image
		self.affectImageDict = self.living_affectImageDict
		self.__ArrangeImageList()


	def BINARY_NEW_AddAffect(self, type, pointIdx, value, duration):

		print "BINARY_NEW_AddAffect", type, pointIdx, value, duration

		if type < 499 and type != chr.NEW_AFFECT_POLYMORPH:
			return

		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif type == chr.AFFECT_BLEND_FISH:
			affect = self.FISH_DESC_IDX_START + pointIdx
		else:
			affect = type

		if chr.IsGameMaster(0):
			chat.AppendChat(chat.CHAT_TYPE_INFO, "BINARY_NEW_AddAffect - type: "+str(type)+" =):"+str(affect)+" pointIdx:"+str(pointIdx)+" value:"+str(value)+" duration:"+str(duration))
		
		if self.affectImageDict.has_key(affect):
			return

		if not self.AFFECT_DATA_DICT.has_key(affect):
			return

		## 용신의 가호, 선인의 교훈은 Duration 을 0 으로 설정한다.
		if affect == chr.NEW_AFFECT_NO_DEATH_PENALTY or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_BONUS or\
		   affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or\
		   affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY or\
		   affect == chr.AFFECT_RUNE_DECK5 or\
		   affect == chr.AFFECT_RUNE_DECK9 or\
		   affect == chr.NEW_AFFECT_SKILL_BOOK_NO_DELAY:
			duration = 0

		affectData = self.AFFECT_DATA_DICT[affect]
		description = affectData[0]
		filename = affectData[1]

		# if pointIdx == player.POINT_MALL_ITEMBONUS or\
		#    pointIdx == player.POINT_MALL_GOLDBONUS:
		# 	value = 1 + float(value) / 100.0

		trashValue = 123
		#if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
		if trashValue == 1:
			try:
				#image = AutoPotionImage()
				#image.SetParent(self)
				image = None
				
				if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
					image.SetPotionType(player.AUTO_POTION_TYPE_SP)
					image = self.autoPotionImageSP
					#self.autoPotionImageSP = image;
				else:
					image.SetPotionType(player.AUTO_POTION_TYPE_HP)
					image = self.autoPotionImageHP
					#self.autoPotionImageHP = image;
				
				image.SetParent(self)
				image.Show()
				image.OnUpdateAutoPotionImage()
				
				self.affectImageDict[affect] = image
				self.__ArrangeImageList()
				
			except Exception, e:
				print "except Aff auto potion affect ", e
				pass				
			
		else:
			if affect != chr.NEW_AFFECT_AUTO_SP_RECOVERY and affect != chr.NEW_AFFECT_AUTO_HP_RECOVERY and affect != 551 and affect != chr.AFFECT_RUNE_DECK5 and affect != chr.AFFECT_RUNE_DECK9:
				description = description(float(value))

			try:
				print "Add affect %s" % affect
				image = AffectImage()
				image.SetParent(self)
				image.LoadImage(filename)
				image.SetDescription(description)
				image.SetDuration(duration)
				image.SetAffect(affect)
				image.SetExtraDescriptions(affect)
				if affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE or\
					affect == chr.NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 or\
					affect == 551 or\
					self.INFINITE_AFFECT_DURATION < duration:
					image.SetClock(False)
					image.UpdateDescription()

				elif affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY or affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
					image.UpdateAutoPotionDescription()
				else:
					image.UpdateDescription()

				if affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK1 or affect == chr.NEW_AFFECT_DRAGON_SOUL_DECK2:
					image.SetScale(1, 1)
				else:
					image.SetScale(0.7, 0.7)
				
				image.SetScale(0.7, 0.7)

				# image.SAFE_SetStringEvent("MOUSE_LEFT_UP",self.__OnSelect, affect)
				image.SetSkillAffectFlag(False)
				image.Show()
				self.affectImageDict[affect] = image
				self.__ArrangeImageList()
			except Exception, e:
				print "except Aff affect ", e
				pass

	def BINARY_NEW_RemoveAffect(self, type, pointIdx):
		if type == chr.NEW_AFFECT_MALL:
			affect = self.MALL_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_BLEND:
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif (app.ENABLE_EXTENDED_BLEND and type == chr.NEW_AFFECT_BLEND_EX):
			affect = self.DEW_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_WATER:
			affect = self.WATER_DESC_IDX_START + pointIdx
		elif type == chr.NEW_AFFECT_MALL_EX:
			affect = self.DRAGON_GOD_DESC_IDX_START + pointIdx
		elif type == chr.AFFECT_BLEND_FISH:
			affect = self.FISH_DESC_IDX_START + pointIdx
		else:
			affect = type
	
		print "Remove Affect %s %s" % ( type , pointIdx )
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	# def OnSkillAffectQuestionDialog(self, affect):
		# import uiCommon
		# self.skillAffectQuestionDialog = uiCommon.QuestionDialog()
		# self.skillAffectQuestionDialog.SetText(localeInfo.SKILL_AFFECT_REMOVE_QUESTION)
		# self.skillAffectQuestionDialog.SetWidth(350)
		# self.skillAffectQuestionDialog.SetAcceptEvent(lambda arg = True: self.OnCloseSkillAffectQuestionDialog(arg))
		# self.skillAffectQuestionDialog.SetCancelEvent(lambda arg = False: self.OnCloseSkillAffectQuestionDialog(arg))
		# self.skillAffectQuestionDialog.affect = affect
		# self.skillAffectQuestionDialog.Open()
		
	# def OnCloseSkillAffectQuestionDialog(self, answer):
		# if not self.skillAffectQuestionDialog:
			# return

		# if not answer:
			# self.skillAffectQuestionDialog.Close()
			# self.skillAffectQuestionDialog = None
			# return
				
		# net.SendChatPacket("/remove_skill_affect {0}".format(int(self.skillAffectQuestionDialog.affect+1)))
		# net.SendChatPacket("/remove_skill_affect "+ str(self.affect + 1))
		# "/remove_affect " + str(self.affect + 1)
		# self.skillAffectQuestionDialog.Close()
		# self.skillAffectQuestionDialog = None
		# return True

	if app.ENABLE_AFFECT_FIX:
		def BINARY_NEW_RefreshAffect(self):
			self.__ArrangeImageList()

	def SetAffect(self, affect):
		self.__AppendAffect(affect)
		self.__ArrangeImageList()

	def ResetAffect(self, affect):
		self.__RemoveAffect(affect)
		self.__ArrangeImageList()

	def SetLoverInfo(self, name, lovePoint):
		image = LovePointImage()
		image.SetParent(self)
		image.SetLoverInfo(name, lovePoint)
		self.lovePointImage = image
		self.__ArrangeImageList()

	def ShowLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Show()
			self.__ArrangeImageList()

	def HideLoverState(self):
		if self.lovePointImage:
			self.lovePointImage.Hide()
			self.__ArrangeImageList()

	def ClearLoverState(self):
		self.lovePointImage = None
		self.__ArrangeImageList()

	def OnUpdateLovePoint(self, lovePoint):
		if self.lovePointImage:
			self.lovePointImage.OnUpdateLovePoint(lovePoint)

	def SetHorseState(self, level, health, battery):
		if level==0:
			self.horseImage=None
		else:
			image = HorseImage()
			image.SetParent(self)
			image.SetState(level, health, battery)
			image.Show()

			self.horseImage=image
			self.__ArrangeImageList()

	def SetPlayTime(self, playTime):
		self.serverPlayTime = playTime
		self.clientPlayTime = app.GetTime()

		if localeInfo.IsVIETNAM():
			image = PlayTimeImage()
			image.SetParent(self)
			image.SetPlayTime(playTime)
			image.Show()

			self.playTimeImage=image
			self.__ArrangeImageList()

	def __AppendAffect(self, affect):

		if self.affectImageDict.has_key(affect):
			return

		try:
			affectData = self.AFFECT_DATA_DICT[affect]
		except KeyError:
			return

		name = affectData[0]
		filename = affectData[1]

		skillIndex = player.AffectIndexToSkillIndex(affect)
		if 0 != skillIndex:
			name = skill.GetSkillName(skillIndex)
			

		image = AffectImage()
		image.SetParent(self)
		image.SetSkillAffectFlag(True)
		image.SetSkillIndex(skillIndex)

		try:
			image.LoadImage(filename)
		except:
			pass

		image.SetToolTipText(name, 0, 40)
		image.SetScale(0.7, 0.7)
		image.Show()
		self.affectImageDict[affect] = image

	def __RemoveAffect(self, affect):
		"""
		if affect == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
			self.autoPotionImageSP.Hide()

		if affect == chr.NEW_AFFECT_AUTO_HP_RECOVERY:
			self.autoPotionImageHP.Hide()
		"""
			
		if not self.affectImageDict.has_key(affect):
			print "__RemoveAffect %s ( No Affect )" % affect
			return

		print "__RemoveAffect %s ( Affect )" % affect
		del self.affectImageDict[affect]
		
		self.__ArrangeImageList()

	def __ArrangeImageList(self):
		xPos = 0
		yPos = 0

		xMax = 0

		countRow = 0

		if self.lovePointImage:
			if self.lovePointImage.IsShow():
				self.lovePointImage.SetPosition(xPos, yPos)
				xPos += self.IMAGE_STEP
				countRow += 1

		if self.horseImage:
			self.horseImage.SetPosition(xPos, yPos)
			xPos += self.IMAGE_STEP
			countRow += 1

		for image in self.affectImageDict.values():
			image.SetPosition(xPos, yPos)
			xPos += self.IMAGE_STEP
			countRow += 1

			if xMax < xPos:
				xMax = xPos

			if countRow == 10:
				xPos = 0
				yPos += self.IMAGE_STEP
				countRow = 0

		self.SetSize(xMax, yPos + 26)


	def OnUpdate(self):
		try:
			if app.GetGlobalTime() - self.lastUpdateTime > 500:
			#if 0 < app.GetGlobalTime():
				self.lastUpdateTime = app.GetGlobalTime()

				for image in self.affectImageDict.values():
					if image.GetAffect() == chr.NEW_AFFECT_AUTO_HP_RECOVERY or image.GetAffect() == chr.NEW_AFFECT_AUTO_SP_RECOVERY:
						image.UpdateAutoPotionDescription()
						continue

					if not image.IsSkillAffect():
						image.UpdateDescription()
		except Exception, e:
			print "AffectShower::OnUpdate error : ", e

