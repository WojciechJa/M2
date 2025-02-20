#-*- coding: iso-8859-1 -*-
ROOT_PATH = "d:/ymir work/ui/game/itemshop/"

window = {
	"name" : "ItemShopPopupDialog",

	"x" : (SCREEN_WIDTH - 390) / 2,
	"y" : (SCREEN_HEIGHT - 68) / 2,

	"width": 390,
	"height": 68,

	"children" : (
		{
			"name" : "board",
			"type" : "image",

			"x" : -20,
			"y" : -20,

			"image" : ROOT_PATH + "popup_board.png",
		},
		{
			"name" : "main_board",
			"type" : "window",

			"x": 0,
			"y": 0,

			"width": 434,
			"height": 70,

			"children" :
			(
				{
					"name": "popup_text1",
					"type": "text",
					"x": 0,
					"y": -18,
					"all_align": "center",
					"fontname": "Tahoma:14",
					"outline": True,
					"text": "Miecz +9 jest teraz na promocji, kliknij, aby zobaczy�",
				},
				{
					"name": "popup_text2",
					"type": "text",
					"x": 0,
					"y": 9,
					"all_align": "center",
					"fontname": "Tahoma:14",
					"outline": True,
					"text": "Miecz +9 jest teraz na promocji, kliknij, aby zobaczy�",
				},
			),
		},
	),
}