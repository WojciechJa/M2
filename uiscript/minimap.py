#-*- coding: iso-8859-1 -*-
# import app
import emoji

ROOT = "d:/ymir work/ui/minimap/"
NEW_PATH = "d:/ymir work/ui/game/NEW_PATH/"
NEW_PATH = "d:/ymir work/ui/minimap/"

TEXT_POS_X = 60

window = {
	"name" : "MiniMap",

	"x" : SCREEN_WIDTH - 136,
	"y" : 0,

	"width" : 136,
	"height" : 150,

	"children" :
	(
		## OpenWindow
		{
			"name" : "OpenWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 136,
			"height" : 150,

			"children" :
			(
				{
					"name" : "OpenWindowBGI",
					"type" : "image",
					"x" : -15,
					"y" : 1,
					"image" : NEW_PATH + "minimapa_bg.png",
				},
				## MiniMapWindow
				{
					"name" : "MiniMapWindow",
					"type" : "window",

					"x" : 0,
					"y" : 20,

					"width" : 128,
					"height" : 128,
				},
				## ScaleU
				## ScaleUpButton
				{
					"name" : "ScaleUpButton",
					"type" : "button",

					"x" : 80,
					"y" : 128,

					"default_image" : NEW_PATH + "plus_btn0.png",
					"over_image" : NEW_PATH + "plus_btn1.png",
					"down_image" : NEW_PATH + "plus_btn2.png",
				},
				## ScaleDownButton
				{
					"name" : "ScaleDownButton",
					"type" : "button",

					"x" : 98,
					"y" : 114,

					"default_image" : NEW_PATH + "minus_btn0.png",
					"over_image" : NEW_PATH + "minus_btn1.png",
					"down_image" : NEW_PATH + "minus_btn2.png",
				},
				## MiniMapHideButton
				{
					"name" : "MiniMapHideButton",
					"type" : "button",

					"x" : 95,
					"y" : 27,

					"default_image" : NEW_PATH + "close_btn0.png",
					"over_image" : NEW_PATH + "close_btn1.png",
					"down_image" : NEW_PATH + "close_btn2.png",
				},
				## AtlasShowButton
				{
					"name" : "AtlasShowButton",
					"type" : "button",

					"x" : 5,
					"y" : 21,

					"default_image" : NEW_PATH + "open_atlas_btn.png",
					"over_image" : NEW_PATH + "open_atlas_btn1.png",
					"down_image" : NEW_PATH + "open_atlas_btn2.png",
				},
				## ch1
				{
					"name" : "Channel1",
					"type" : "radio_button",
					"x" : -10,
					"y" : 52,
					"default_image" : NEW_PATH + "ch1_0.png",
					"over_image" : NEW_PATH + "ch1_1.png",
					"down_image" : NEW_PATH + "ch1_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f1.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ch2
				{
					"name" : "Channel2",
					"type" : "radio_button",
					"x" : -14,
					"y" : 72,
					"default_image" : NEW_PATH + "ch2_0.png",
					"over_image" : NEW_PATH + "ch2_1.png",
					"down_image" : NEW_PATH + "ch2_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f2.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ch3
				{
					"name" : "Channel3",
					"type" : "radio_button",
					"x" : -11,
					"y" : 93,
					"default_image" : NEW_PATH + "ch3_0.png",
					"over_image" : NEW_PATH + "ch3_1.png",
					"down_image" : NEW_PATH + "ch3_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f3.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ch4
				{
					"name" : "Channel4",
					"type" : "radio_button",
					"x" : -1,
					"y" : 110,
					"default_image" : NEW_PATH + "ch4_0.png",
					"over_image" : NEW_PATH + "ch4_1.png",
					"down_image" : NEW_PATH + "ch4_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f4.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ch5
				{
					"name" : "Channel5",
					"type" : "radio_button",
					"x" : 14,
					"y" : 124,
					"default_image" : NEW_PATH + "ch5_0.png",
					"over_image" : NEW_PATH + "ch5_1.png",
					"down_image" : NEW_PATH + "ch5_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f5.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ch6
				{
					"name" : "Channel6",
					"type" : "radio_button",
					"x" : 33,
					"y" : 132,
					"default_image" : NEW_PATH + "ch6_0.png",
					"over_image" : NEW_PATH + "ch6_1.png",
					"down_image" : NEW_PATH + "ch6_2.png",

					"tooltip_text_new" : emoji.AppendEmoji("icon/emoji/key_alt.png")+"+"+emoji.AppendEmoji("icon/emoji/key_f6.png"),
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -40,
				},
				## ServerInfo
				{
					"name" : "ServerInfo",
					"type" : "text",
					"text_horizontal_align" : "center",
					"outline" : 1,
					"x" : TEXT_POS_X,
					"y" : 140+35,
					"text" : "",
                    # "fontname" : "Tahoma:11b",
				},
				## TimeInfo
				{
					"name": "dayInfo",
					"type": "text",
					"text_horizontal_align": "center",
					"outline": 1,
					"x": TEXT_POS_X,
					"y": 190,
					"text": "",
                 	# "fontname" : "Tahoma:11",
					"outline": True,
				},
				{
					"name" : "krecha",
					"type" : "text",
					"text_horizontal_align" : "center",
					"outline" : 1,
					"x" : TEXT_POS_X,
					"y" : 198,
					"text" : "______________",
					"color":0xff999999,
                    # "fontname" : "Tahoma:14b",
				},
				## PositionInfo
				{
					"name" : "PositionInfo",
					"type" : "text",
					"text_horizontal_align" : "center",
					"outline" : 1,
					"x" : TEXT_POS_X,
					"y" : 216,
					"text" : "",
				},
				## FPS Info
				{
					"name": "fpsInfo",
					"type": "text",
					"text_horizontal_align": "center",
					"outline": 1,
					"x": TEXT_POS_X,
					"y": 232,
					"text": "",
					"outline": True,
				},



				## ObserverCount
				{
					"name" : "ObserverCount",
					"type" : "text",

					"text_horizontal_align" : "center",

					"outline" : 1,

					"x" : 70,
					"y" : 180,

					"text" : "",
				},
			),
		},
		{
			"name" : "CloseWindow",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 132,
			"height" : 48,

			"children" :
			(
				## ShowButton
				{
					"name" : "MiniMapShowButton",
					"type" : "button",

					"x" : 100,
					"y" : 4,

					"default_image" : ROOT + "minimap_open_default.sub",
					"over_image" : ROOT + "minimap_open_default.sub",
					"down_image" : ROOT + "minimap_open_default.sub",
				},
			),
		},
	),
}