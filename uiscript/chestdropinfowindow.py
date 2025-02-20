#-*- coding: iso-8859-1 -*-
#blackdragonx61 (28.11.21)
# import uiScriptLocale

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
CHATTING_PATH			= "d:/ymir work/ui/chat/"
UPDATE_2_5 = 60
WINDOW_WIDTH			= 285
WINDOW_HEIGHT			= 275 + UPDATE_2_5
PATTERN_WINDOW_WIDTH	= WINDOW_WIDTH - 22
PATTERN_WINDOW_HEIGHT	= WINDOW_HEIGHT - 46

PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

MIDDLE_BUTTON_WIDTH 	= 65
MIDDLE_BUTTON_HEIGHT 	= 25

window = {
	"name" : "ChestDropInfoWindow",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
					"width" : WINDOW_WIDTH-15,
					"color" : "yellow",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":-3, "text":"...", "all_align":"center","outline":"1" },
						{
							"name":"BackBtn",
							"type":"button",
							"x":1,"y":1,
							"default_image":"d:/ymir work/ui/back_btn0.png",
							"over_image":"d:/ymir work/ui/back_btn1.png",
							"down_image":"d:/ymir work/ui/back_btn2.png",
							"tooltip_text_new" : "Powr�t",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -34,
						},
					),
				},
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					"x" : 10,
					"y" : 32,
					"width" : PATTERN_WINDOW_WIDTH,
					"height" :PATTERN_WINDOW_HEIGHT,
					"children" :
					(
						## LeftTop 1
						{
							"name" : "pattern_left_top_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "pattern_right_top_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "pattern_left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "pattern_right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "pattern_top_cetner_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "pattern_left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "pattern_right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "pattern_bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "pattern_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
                {
                    "name":"chestboard",
                    "type":"image",
                    "x":12,
                    "y":34,
                    "image":"d:/ymir work/ui/chest_board.png",
				},
				## main slot bg
				{
					"name" : "regist_slot_img",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 118,
					"y" : 103 - 60,
					
					"image" : "d:/ymir work/ui/minigame/fish_event/fish_special_slot.sub",
					
					"children" :
					(
						## regist slot
						{
								"name" : "main_item_slot",
								"type" : "slot",

								"x" : 0,
								"y" : 0,

								"width" : 44,
								"height" : 107,
								
								"slot" : ( {"index":0, "x":7, "y":7, "width":32, "height":96}, )
						},
					),
				},
                {
					"name" : "ThinBoard",
					"type" : "thinboard_circle",

					"x" : 12,
					"y" : 93,
					"width" : 259,
					"height" : 227,

					"children" : (
						{
							"name" : "drop_item_slot",
							"type" : "grid_table",

							"x" : 17,
							"y" : 5,

							"start_index" : 0,
							"x_count" : 7,
							"y_count" : 6,
							"x_step" : 32,
							"y_step" : 32,

							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
						},
					),
				},
				{
					"name":"PageSlotbar", "type":"image",
					"x": 0, "y": 294,
					"horizontal_align":"center",
					"image":"d:/ymir work/ui/location_window/slotbar-84x23.png",
					"children": 
					(
						{
							"name":"prev_button", "type":"button",
							"x": 10, "y": 5,
							"default_image":"d:/ymir work/ui/location_window/btn-left-0.png",
							"over_image":"d:/ymir work/ui/location_window/btn-left-1.png",
							"down_image":"d:/ymir work/ui/location_window/btn-left-2.png",
						},
						{
							"name":"CurrentPage","type":"text",
							"x":0,"y":0,
							"all_align":"center",
							"text":"0",
							"fontname":"Tahoma:14b",
							"outline":"1",
						},
						{
							"name":"next_button", "type":"button",
							"x": 60, "y": 5,
							"default_image":"d:/ymir work/ui/location_window/btn-right-0.png",
							"over_image":"d:/ymir work/ui/location_window/btn-right-1.png",
							"down_image":"d:/ymir work/ui/location_window/btn-right-2.png",
						},  
					),
				},
			),
		},
	),
}