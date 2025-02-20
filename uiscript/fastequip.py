#-*- coding: iso-8859-1 -*-
import uiScriptLocale

window = {
	"name" : "FastEquip",
	"x" : SCREEN_WIDTH - 580,
	"y" : SCREEN_HEIGHT - 550,
	"style" : ("movable", "float",),
	"width" : 310,
	"height" : 320,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 30,
			"width" : 140,
			"height" : 229,
			"children" :
			(
				{
					"name" : "Costume_Base",
					"type" : "image",
					"x" : 10,
					"y" : 10,
					"image" : uiScriptLocale.LOCALE_UISCRIPT_PATH + "costume/new_costume_bg.jpg",
				},
			),
		},
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 125,
			"y" : 0,
			"width" : 182,
			"height" : 290,
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 6,
					"width" : 170,
					"color" : "yellow",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":80, "y":0, "text":"Zmiana Ekwipunku", "outline":1, "text_horizontal_align":"center" },
					),
				},

				## Equipment Slot
				{
					"name" : "equip_Base",
					"type" : "image",
					"x" : 13,
					"y" : 38,
					"image" : "d:/ymir work/ui/equipment_bg_without_ring.tga",
				},		
				{
					"name" : "change_button",
					"type" : "button",
					"x" : 30,
					"y" : 258,
					"default_image" : "d:/ymir work/ui/weekly_rank/button-example-020.png",
					"over_image" : "d:/ymir work/ui/weekly_rank/button-example-021.png",
					"down_image" : "d:/ymir work/ui/weekly_rank/button-example-022.png",
					"children":
					(
						{
							"name" : "change_text",
							"type" : "text",
							"x":0,"y":-1,
							"text":"Zmie�",
							"outline":1,
							"fontname":"Tahoma:16",
							"all_align":"center",
						},
					),
				},
			),
		},
		{
			"name" : "equipslot",
			"type" : "slot",
			"x" : 16,
			"y" : 41,
			# "image" : "d:/ymir work/ui/public/Slot_Base.sub",
			"width" : 273,
			"height" : 200,
			"slot" : (
						{"index":1, "x":163, "y":35, "width":32, "height":64}, #zbroja
						{"index":2, "x":163, "y":2, "width":32, "height":32}, #he�m
						{"index":3, "x":163, "y":145, "width":32, "height":32}, #buty
						{"index":4, "x":200, "y":66, "width":32, "height":32}, #bransa
						{"index":5, "x":127, "y":2, "width":32, "height":96}, # bro�
						{"index":6, "x":239, "y":66, "width":32, "height":32}, #naszyjnik
						{"index":7, "x":239, "y":33, "width":32, "height":32}, #tarcza
						{"index":8, "x":200, "y":33, "width":32, "height":32}, #kolczyki
						{"index":9, "x":163, "y":105, "width":32, "height":32}, #pas
						{"index":10, "x":239, "y":144, "width":32, "height":32}, #rekawica
						#
						{"index":11, "x":10, "y":16, "width":32, "height":96}, #kostium bro�
						{"index":12, "x":59, "y":11, "width":32, "height":32}, #kostium fryzura
						{"index":13, "x":59, "y":47, "width":32, "height":64}, #kostium zbroja
						#
						{"index":14, "x":3, "y":128, "width":32, "height":32}, #emblemat
						{"index":15, "x":67, "y":128, "width":32, "height":32}, #nakladka szarfa
						{"index":16, "x":35, "y":128, "width":32, "height":32}, #szarfa
						#
						{"index":17, "x":35, "y":160, "width":32, "height":32}, #aura
					),
		},
		{
			"name" : "eq_btn_window",
			"type" : "window",
			"x" : 135,
			"y" : 230,
			"width" : 159,
			"height": 22,
			"children":
			(
				{
					"name" : "eq0",
					"type" : "radio_button",
					"x" : 2,
					"y" : 0,
					"text" : "I",
					"outline":1,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},				
				{
					"name" : "eq1",
					"type" : "radio_button",
					"x" : 33,
					"y" : 0,
					"text" : "II",
					"outline":1,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},				
				{
					"name" : "eq2",
					"type" : "radio_button",
					"x" : 33+31,
					"y" : 0,
					"text" : "III",
					"outline":1,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},				
				{
					"name" : "eq3",
					"type" : "radio_button",
					"x" : 33+31+31,
					"y" : 0,
					"text" : "IV",
					"outline":1,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},				
				{
					"name" : "eq4",
					"type" : "radio_button",
					"x" : 33+31+31+31,
					"y" : 0,
					"text" : "V",
					"outline":1,
					"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
					"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
					"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",
				},
			),
		},
	),
}