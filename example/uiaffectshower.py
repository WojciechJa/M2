# Search
	if app.ENABLE_DRAGON_SOUL_SYSTEM:
		# ¿ëÈ¥¼® Ãµ, Áö µ¦.
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK1] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK1, "d:/ymir work/ui/dragonsoul/buff_ds_sky1.tga")
		AFFECT_DATA_DICT[chr.NEW_AFFECT_DRAGON_SOUL_DECK2] = (localeInfo.TOOLTIP_DRAGON_SOUL_DECK2, "d:/ymir work/ui/dragonsoul/buff_ds_land1.tga")

# Add below
	if app.__BL_OFFICIAL_LOOT_FILTER__ and app.ENABLE_PREMIUM_LOOT_FILTER:
		AFFECT_DATA_DICT[chr.NEW_AFFECT_LOOTING_SYSTEM] = (localeInfo.TOOLTIP_AFFECT_LOOTING_SYSTEM, "d:/ymir work/ui/skill/common/affect/looting_system.sub")
