BOARD_WIDTH = 420
BOARD_HEIGHT = 270

window = {
    "name" : "Marmur_Window",
	"style" : ("movable", "float",),
    "x" : 0, "y" : 0,
    "width": BOARD_WIDTH,"height": BOARD_HEIGHT,
    "children" :
    (
        {
            "name" : "board",
            "type" : "board",
            "x": 0, "y": 0,
            "width": BOARD_WIDTH,"height": BOARD_HEIGHT,
            "children" :
            (
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,"y" : 7,
					"width" : BOARD_WIDTH-15,
					"color" : "yellow",
					"children" :
					(
						{ 
							"name": "TitleName",
							"type":"text",
							"x":0,"y":-4,
							"text": "Sklep z Marmurami",
                            "outline":"1",
							"all_align":"center",
						},
					),
				},
                {
                    "name" : "ThinBoard",
                    "type" : "thinboard_new",
                    "x": 208, "y": 32,
                    "width":201,"height":235,
                    "children" :
                    (
                        {
                            "name" : "MainHeader",
                            "type" : "image",
                            "x": 1, "y": 0,
                            "image" : "d:/ymir work/ui/separator_big.tga",
                            "children" : 
                            (
                                {
                                    "name" : "MainHeader_Text",
                                    "type" : "text",
                                    "x" : 0,"y" : 0,
                                    "all_align" : "center",
                                    "text" : "Dziki Pies","outline" : "1",
                                },
                                {
                                    "name" : "RenderModel",
                                    "type" : "render_target",
                                    "x" : 0, "y" : 20,
                                    "width" : 197,
                                    "height" : 210,
                                    "index" : 8,
                                },
                            ),
                        },
                    ),
                },
                {
                    "name" : "ThinBoard1",
                    "type" : "thinboard_new",
                    "x": 10, "y": 32,
                    "width":201,"height":133,
                    "children" :
                    (
                        {
                            "name" : "MainHeader1",
                            "type" : "image",
                            "x": 1, "y": 0,
                            "image" : "d:/ymir work/ui/separator_big.tga",
                            "children" : 
                            (
                                {
                                    "name" : "MainHeader1_Text",
                                    "type" : "text",
                                    "x" : 0,"y" : 0,
                                    "all_align" : "center",
                                    "text" : "Wymagane Przedmioty","outline" : "1",
                                },
                                {
                                    "name" : "RequiredItemBackground",
                                    "type" : "image",
                                    "style" : ("attach",),
                                    "x" : 2,"y" : 23,
                                    "image" : "d:/ymir work/ui/marmur_window/item_slots_bg.png",
                                    "children" : (
                                        {
                                            "name" : "RequiredItemSlot",
                                            "type" : "slot", 
                                            "x" : 13,"y" : 10,
                                            "width" : 130,"height" : 32,
                                            "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub",
                                            "slot" : (
                                                # { "index" : 0, "x" : 0, "y" : 0, "width" : 32, "height" : 32 },
                                                # { "index" : 1, "x" : 32 + 12, "y" : 0, "width" : 32, "height" : 32 },
                                                # { "index" : 2, "x" : 64 + 23, "y" : 0, "width" : 32, "height" : 32 },
                                                # { "index" : 3, "x" : 132, "y" : 0, "width" : 32, "height" : 32 },
                                                { "index" : 4, "x" : 0, "y" : 52, "width" : 32, "height" : 32 },
                                                { "index" : 5, "x" : 32 + 12, "y" : 52, "width" : 32, "height" : 32 },
                                                { "index" : 6, "x" : 64 + 23, "y" : 52, "width" : 32, "height" : 32 },
                                                { "index" : 7, "x" : 132, "y" : 52, "width" : 32, "height" : 32 },
                                            ),
                                        },
                                    )
                                },
                            ),
                        },
                        {
                            "name" : "Real_Slot0",
                            "type" : "slot",
                            "x" : 16,"y" : 33.5,
                            "width" : 32,"height" : 32,
                            "slot" : (
                                {"index":0, "x":0, "y":0, "width":32, "height":32},
                            ),
                        },
                        {
                            "name" : "Real_Slot1",
                            "type" : "slot",
                            "x" : 60,"y" : 33.5,
                            "width" : 32,"height" : 32,
                            "slot" : (
                                {"index":0, "x":0, "y":0, "width":32, "height":32},
                            ),
                        },
                        {
                            "name" : "Real_Slot2",
                            "type" : "slot",
                            "x" : 104,"y" : 33.5,
                            "width" : 32,"height" : 32,
                            "slot" : (
                                {"index":0, "x":0, "y":0, "width":32, "height":32},
                            ),
                        },
                    ),
                },
                {
                    "name" : "ThinBoard2",
                    "type" : "thinboard_new",
                    "x": 10, "y": 168,
                    "width":201,"height":50,
                    "children" :
                    (
                        {
                            "name" : "MainHeader2",
                            "type" : "image",
                            "x": 1, "y": 0,
                            "image" : "d:/ymir work/ui/separator_big.tga",
                            "children" : 
                            (
                                {
                                    "name" : "MainHeader2_Text",
                                    "type" : "text",
                                    "x" : 0,"y" : 0,
                                    "all_align" : "center",
                                    "text" : "Wymagana Waluta","outline" : "1",
                                },
                                {
                                    "name" : "MoneyText",
                                    "type" : "text",
                                    "x" : 0,"y" : 24,
                                    "all_align" : "center",
                                    "text" : "","outline" : "1",
                                },
                            ),
                        },
                    ),
                },
                {
                    "name" : "ThinBoard3",
                    "type" : "thinboard_new",
                    "x": 10, "y": 220,
                    "width":201,"height":40,
                    "children" :
                    (
                        {
                            "name" : "IloscText",
                            "type" : "text",
                            "x" : 15,"y" : 13,
                            "text" : "xxx","outline" : "1",
                        },
                        {
                            "name" : "PlusButton",
                            "type" : "button",
                            "x" : 155,"y" : 8,
                            "default_image" : "d:/ymir work/ui/location_window/add.png",
                            "over_image" : "d:/ymir work/ui/location_window/add1.png",
                            "down_image" : "d:/ymir work/ui/location_window/add2.png",
                        },
                        {
                            "name" : "MinusButton",
                            "type" : "button",
                            "x" : 130,"y" : 8,
                            "default_image" : "d:/ymir work/ui/location_window/del.png",
                            "over_image" : "d:/ymir work/ui/location_window/del1.png",
                            "down_image" : "d:/ymir work/ui/location_window/del2.png",
                        },
                        {
                            "name" : "status",
                            "type" : "toggle_button",
                            "x": 111, "y": 16,
                            "default_image" : "d:/ymir work/ui/pattern/visible_mark_01.tga",
                            "over_image" : "d:/ymir work/ui/pattern/visible_mark_02.tga",
                            "down_image" : "d:/ymir work/ui/pattern/visible_mark_03.tga",
                        },
                    ),
                },
                {
                    "name" : "obramowka",
                    "type" : "image",
                    "x": 200, "y": 40,
                    "image" : "d:/ymir work/ui/marmur_window/obramowka.png",
                },
                {
                    "name" : "PrevButton",
                    "type" : "button",
                    "x" : 207,"y" : 30,
                    "default_image" : "d:/ymir work/ui/marmur_window/left_btn.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/left_btn1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/left_btn2.png",
                },
                {
                    "name" : "NextButton",
                    "type" : "button",
                    "x" : 390,"y" : 30,
                    "default_image" : "d:/ymir work/ui/marmur_window/right_btn.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/right_btn1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/right_btn2.png",
                },
                {
                    "name" : "stopien_low",
                    "type" : "radio_button",
                    "x" : 270,"y" : 60,
                    "default_image" : "d:/ymir work/ui/marmur_window/stopien.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/stopien1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/stopien2.png",
                },
                {
                    "name" : "stopien_medium",
                    "type" : "radio_button",
                    "x" : 300,"y" : 60,
                    "default_image" : "d:/ymir work/ui/marmur_window/stopien.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/stopien1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/stopien2.png",
                },
                {
                    "name" : "stopien_high",
                    "type" : "radio_button",
                    "x" : 330,"y" : 60,
                    "default_image" : "d:/ymir work/ui/marmur_window/stopien.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/stopien1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/stopien2.png",
                },
                {
                    "name" : "BuyButton",
                    "type" : "button",
                    "x" : 263,"y" : 230,
                    "default_image" : "d:/ymir work/ui/marmur_window/buy_btn.png",
                    "over_image" : "d:/ymir work/ui/marmur_window/buy_btn1.png",
                    "down_image" : "d:/ymir work/ui/marmur_window/buy_btn2.png",
                },
            ),
        },
    ),
}