#-*- coding: iso-8859-1 -*-
# import uiScriptLocale

SIZE_X = 365
SIZE_Y = 310
WIDTH = (SCREEN_WIDTH/10)
HEIGHT = (SCREEN_HEIGHT/2) - (SIZE_Y/2)

locationHorizontalWindow_Width = 355
locationHorizontalWindow_Height = 50

ZAPIS_POS_Y = 10
ZAPIS_BORDER_MINUS = 18

window = {
	"name" : "UiLocationWindow",
	"style" : ("movable", "float",),
	"x" : WIDTH,
	"y" : HEIGHT,	
	"width" : SIZE_X,
	"height" : SIZE_Y+50,
	"children" :
	(
		{
			"name" : "board_logs",
			"type" : "board",
			"x": 0, "y": SIZE_Y-25,
			"horizontal_align":"center",
			"width": 130,"height": 40,
			"children" :
			(
				{
					"name":"PageSlotbar", "type":"image",
					"x": 0, "y": 28,
					"horizontal_align":"center",
					"image":"d:/ymir work/ui/location_window/slotbar-84x23.png",
					"children": 
					(
						{
							"name":"PrevPageBtn", "type":"button",
							"x": 10, "y": 5,
							"default_image":"d:/ymir work/ui/location_window/btn-left-0.png",
							"over_image":"d:/ymir work/ui/location_window/btn-left-1.png",
							"down_image":"d:/ymir work/ui/location_window/btn-left-2.png",
						},
						{
							"name":"PageText","type":"text",
							"x":0,"y":0,
							"all_align":"center",
							"text":"I",
							"fontname":"Tahoma:14b",
							"outline":"1",
						},
						{
							"name":"NextPageBtn", "type":"button",
							"x": 60, "y": 5,
							"default_image":"d:/ymir work/ui/location_window/btn-right-0.png",
							"over_image":"d:/ymir work/ui/location_window/btn-right-1.png",
							"down_image":"d:/ymir work/ui/location_window/btn-right-2.png",
						},  
					),
				},
			),
		},
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : SIZE_X,
			"height" : SIZE_Y,
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 6,
					"y" : 7,
					"width" : SIZE_X - 13,
					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",
							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"x" : 0,
							"y" : 0,
							"text" : "Zapisane pozycje",
							"outline":"1",
						},
					),
				},
				{
					"name" : "SaveLastPageButton",
					"type" : "toggle_button",
					"x" : 12,
					"y" : 7,
					"tooltip_text_new" : "Zapamiêtaj ostatni¹ stronê",
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
					"default_image" : "d:/ymir work/ui/off_btn.png",
					"over_image" : "d:/ymir work/ui/off_btn.png",
					"down_image" : "d:/ymir work/ui/on_btn.png",
				},
				{
					"name" : "OpenWhenTeleportButton",
					"type" : "toggle_button",
					"x" : 30,
					"y" : 7,
					"tooltip_text_new" : "Otwieraj automatycznie (przy zmianie mapy/channela)",
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -35,
					"default_image" : "d:/ymir work/ui/off_btn.png",
					"over_image" : "d:/ymir work/ui/off_btn.png",
					"down_image" : "d:/ymir work/ui/on_btn.png",
				},
                {
					"name" : "Thinboard",
                    "type" : "border_a",
                    "x":0,"y":ZAPIS_POS_Y,
                    "vertical_align":"center",
                    "horizontal_align":"center",
                    "width":SIZE_X-ZAPIS_BORDER_MINUS, "height":SIZE_Y-45,
                    "children":
                    (
						{
							"name":"Zapis_0",
							"type":"image",
							"x":-1,"y":10,
                            "horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/background-0.png",
							"children":
							(
								{
									"name" : "Zapis_Board",
									"type" : "thinboard_new",
									"x": 0, "y": 0,
									"width":335,"height":45,
								},  
							),
						},
						{
							"name":"Zapis_1",
							"type":"image",
							"x":-1,"y":60,
                            "horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/background-1.png",
							"children":
							(
								{
									"name" : "Zapis_Board",
									"type" : "thinboard_new",
									"x": 0, "y": 0,
									"width":335,"height":45,
								},  
							),
						},
						{
							"name":"Zapis_2",
							"type":"image",
							"x":-1,"y":110,
                            "horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/background-2.png",
							"children":
							(
								{
									"name" : "Zapis_Board",
									"type" : "thinboard_new",
									"x": 0, "y": 0,
									"width":335,"height":45,
								},  
							),
						},
						{
							"name":"Zapis_3",
							"type":"image",
							"x":-1,"y":160,
                            "horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/background-3.png",
							"children":
							(
								{
									"name" : "Zapis_Board",
									"type" : "thinboard_new",
									"x": 0, "y": 0,
									"width":335,"height":45,
								},  
							),
						},
						{
							"name":"Zapis_4",
							"type":"image",
							"x":-1,"y":210,
                            "horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/background-4.png",
							"children":
							(
								{
									"name" : "Zapis_Board",
									"type" : "thinboard_new",
									"x": 0, "y": 0,
									"width":335,"height":45,
								},  
							),
						},
					),
				},
				{
					"name" : "Page1", "type" : "window",
					"x" : 0, "y" : 25,
					"width" : SIZE_X, "height" : SIZE_Y-50,
					"children":
					(
						{
							"name":"locationHorizontalWindow0",
							"type":"window",
							"x":0,"y":10,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow1",
							"type":"window",
							"x":0,"y":60,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow2",
							"type":"window",
							"x":0,"y":110,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow3",
							"type":"window",
							"x":0,"y":160,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow4",
							"type":"window",
							"x":0,"y":210,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
					),
				},
				{
					"name" : "Page2", "type" : "window",
					"x" : 0, "y" : 25,
					"width" : SIZE_X, "height" : SIZE_Y-50,
					"children":
					(
						{
							"name":"locationHorizontalWindow5",
							"type":"window",
							"x":0,"y":10,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow6",
							"type":"window",
							"x":0,"y":60,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow7",
							"type":"window",
							"x":0,"y":110,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow8",
							"type":"window",
							"x":0,"y":160,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow9",
							"type":"window",
							"x":0,"y":210,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
					),
				},
				{
					"name" : "Page3", "type" : "window",
					"x" : 0, "y" : 25,
					"width" : SIZE_X, "height" : SIZE_Y-50,
					"children":
					(
						{
							"name":"locationHorizontalWindow10",
							"type":"window",
							"x":0,"y":10,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow11",
							"type":"window",
							"x":0,"y":60,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow12",
							"type":"window",
							"x":0,"y":110,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow13",
							"type":"window",
							"x":0,"y":160,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow14",
							"type":"window",
							"x":0,"y":210,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
					),
				},
				{
					"name" : "Page4", "type" : "window",
					"x" : 0, "y" : 25,
					"width" : SIZE_X, "height" : SIZE_Y-50,
					"children":
					(
						{
							"name":"locationHorizontalWindow15",
							"type":"window",
							"x":0,"y":10,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow16",
							"type":"window",
							"x":0,"y":60,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow17",
							"type":"window",
							"x":0,"y":110,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow18",
							"type":"window",
							"x":0,"y":160,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow19",
							"type":"window",
							"x":0,"y":210,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
					),
				},
				{
					"name" : "Page5", "type" : "window",
					"x" : 0, "y" : 25,
					"width" : SIZE_X, "height" : SIZE_Y-50,
					"children":
					(
						{
							"name":"locationHorizontalWindow20",
							"type":"window",
							"x":0,"y":10,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow21",
							"type":"window",
							"x":0,"y":60,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow22",
							"type":"window",
							"x":0,"y":110,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow23",
							"type":"window",
							"x":0,"y":160,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
						{
							"name":"locationHorizontalWindow24",
							"type":"window",
							"x":0,"y":210,
							"width" : locationHorizontalWindow_Width, "height" : locationHorizontalWindow_Height,
						},
					),
				},
			),
		},
	),
}