# import uiScriptLocale
# import app

ROOT = "d:/ymir work/ui/game/"

BOARD_X = SCREEN_WIDTH - (350)
BOARD_WIDTH = (350)
BOARD_HEIGHT = 40

window = {
	"name" : "ExpandedMoneyTaskbar",
	
	"x" : BOARD_X,
	"y" : SCREEN_HEIGHT - 65,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"style" : ("float",),

	"children" :
	[
		{
			"name" : "ExpanedMoneyTaskBar_Board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			[
				## Print
				{
					"name":"Money_Icon",
					"type":"button",
					
					"x":194,
					"y":9,

					"default_image":"icon/emoji/money_icon.png",
                    "over_image":"icon/emoji/money_icon1.png",
                    "down_image":"icon/emoji/money_icon2.png",
				},
				{
					"name":"Money_Slot",
					"type":"button",

					"x":223,
					"y":8,

					#"horizontal_align":"center",

					"default_image" : "d:/ymir work/ui/public/parameter_slot_04.sub",
					"over_image" : "d:/ymir work/ui/public/parameter_slot_04.sub",
					"down_image" : "d:/ymir work/ui/public/parameter_slot_04.sub",

					"children" :
					(
						{
							"name" : "Money",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "9,999,999,999",
						},
					),
				},
			],
		},
		{
			"name" : "expander",
			"type" : "button",
			"x":15,
			"y":-6,
			"default_image" : "d:/ymir work/ui/btn_expand_normal1.png",
			"over_image" : "d:/ymir work/ui/btn_expand_over1.png",
			"down_image" : "d:/ymir work/ui/btn_expand_down1.png",
		},
	],
}

window["children"][0]["children"] = window["children"][0]["children"] + [
				{
					"name":"Cheque_Icon",
					"type":"button",
					
					"x":94,
					"y":10,

					"default_image":"icon/emoji/cheque_icon.png",
                    "over_image":"icon/emoji/cheque_icon1.png",
                    "down_image":"icon/emoji/cheque_icon2.png",
				},					
				{
					"name":"Cheque_Slot",
					"type":"button",

					"x":113,
					"y":8,

					"default_image" : "d:/ymir work/ui/public/parameter_slot_77x18.sub",
					"over_image" : "d:/ymir work/ui/public/parameter_slot_77x18.sub",
					"down_image" : "d:/ymir work/ui/public/parameter_slot_77x18.sub",

					"children" :
					(
						{
							"name" : "Cheque",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "999,999",
						},
					),
				},
				]
window["children"][0]["children"] = window["children"][0]["children"] + [
				{
					"name":"Pkt_Osiag_Icon",
					"type":"image",
					
					"x":10,
					"y":10,

					"image":"d:/ymir work/ui/stone_point/stone_point.tga",
				},					
				{
					"name":"Pkt_Osiag_Slot",
					"type":"image",

					"x":28,
					"y":8,
					
					"image" : "d:/ymir work/ui/public/Parameter_Slot_02.sub",

					"children" :
					(
						{
							"name" : "Pkt_Osiag",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "999,999",
						},
					),
				},
				]