#-*- coding: iso-8859-1 -*-
import uiScriptLocale

PATH = "d:/ymir work/ui/itemshop/"

window = {
	"name": "ItemShopDialog",
	"style": ("movable", "float",),
	"x": 0,
	"y": -80,
	"width": 711,
	"height": 490,
	"children" :
	(
		{
			"name": "Board",
			"type": "image",
			"style": ("attach",),
			"x": 0,"y": 0,
			"width": 711,
			"height": 490,
			"image": PATH + "bg.png",
			"children" :
			(
				{
					"name" : "ExitButton",
					"type" : "button",
					"x" : 670, "y" : 30,
					"default_image" : "d:/ymir work/ui/battle_pass/exit0.png",
					"over_image" : "d:/ymir work/ui/battle_pass/exit1.png",
					"down_image" : "d:/ymir work/ui/battle_pass/exit2.png",
				},
				{
					"name" : "wheel_button",
					"type" : "button",
					"x" : 470, "y" : 30,
					"default_image" : PATH+"kolo.png",
					"over_image" : PATH+"kolo1.png",
					"down_image" : PATH+"kolo2.png",
				},
				{
					"name" : "VoucherButton",
					"type" : "button",
					"x" : 570, "y" : 30,
					"default_image" : PATH+"voucher.png",
					"over_image" : PATH+"voucher1.png",
					"down_image" : PATH+"voucher2.png",
				},
				{
					"name":"stankonta",
					"type":"image",
					"x":290,
					"y":45,
					"tooltip_text_new" : "Wciœnij, aby do³adowaæ SM.",
					"tooltip_text_color" : 0xfff1e6c0,
					"tooltip_x" : 0,
					"tooltip_y" : -27,
					"image":PATH+"stankonta.png",
					"children":
					(
						{
							"name": "money_value",
							"type": "text",

							"x": 0,
							"y": -8,
							"all_align":"center",

							"fontname": "Tahoma:14",
							#"color": 0xFFFFCF48,
							"outline": True,
							"text": "0",
						},
					),
				},
				{
					"name": "all_window",
					"type": "window",

					"x": 50,
					"y": 130,

					"width": 650,
					"height": 360,

					"children": (
						{
							"name": "categories_board",
							"type": "window",
							"x": 12,
							"y": 9,
							"width": 145,
							"height": 300,
							"children":
							(
								{
									"name" : "category_window",
									"type" : "image",
									"x" : 0,
									"y" : 0,
									"image" : PATH + "category.png",
									"children":
									(
										{
											"name": "discount_button",
											"type": "radio_button",
											"x": 12,
											"y": 10,
											"default_image": PATH + "oferty_btn.png",
											"over_image": PATH + "oferty_btn1.png",
											"down_image": PATH + "oferty_btn2.png",
										},
									),
								},
							),
						},
						{
							"name": "items_board",
							"type": "window",

							"x": 206,
							"y": 0,

							"width": 400,
							"height": 308,

							"children" : (
								{
									"name": "no_discount_items_text",
									"type": "text",

									"x": 0,
									"y": 0,

									"all_align" : "center",

									"fontname": "Tahoma:14b",
									"outline": True,
									"text": uiScriptLocale.ITEMSHOP_NO_DISCOUNT_ITEMS,
								},
							),
						},
					),
				},
				{
					"name": "wheel_window",
					"type": "window",
					"x": 0,
					"y": 148,
					"width": 792,
					"height": 407,
					"children": (
						{
							"name": "wheel_category_list_bg",
							"type": "image",
							"x": 62,
							"y": -9,
							"image": PATH + "category.png",
							"children":
							(
								{
									"name": "wheel_auto_button",
									"type": "toggle_button",
									"x": 0,
									"y": 275,
									"default_image": PATH + "category_btn.png",
									"over_image": PATH + "category_btn1.png",
									"down_image": PATH + "category_btn2.png",
									"horizontal_align":"center",
									"text": "",
									"children": (
										{
											"name": "wheel_auto_button_text_01",
											"type": "text",
											"x": 0,
											"y": -2,
											"all_align": "center",
											"fontname": "Tahoma:12",
											"outline": True,
											"text": "Automatyczne krêcenie",
										},
									),
								},
								{
									"name": "wheel_price_bg",
									"type": "image",
									"x": 0,
									"y": 247,
									"horizontal_align":"center",
									"image": PATH + "promo_slot.png",
									"children": (
										{
											"name": "wheel_price_value",
											"type": "text",
											"x": 0,
											"y": 0,
											"fontname": "Tahoma:12",
											"outline": True,
											"all_align":"center",
											"text": "",
										},
									),
								},
							),
						},
						{
							"name": "wheel_item_list_bg",
							"type": "image",
							"x": 600,
							"y": -14,
							"image": PATH + "category2.png",
						},
						{
							"name": "wheel_bg",
							"type": "image",
							"x": 265,
							"y": -15,
							"image": PATH + "wheel/bg.tga",
							"children": (
								{
									"name": "wheel_animation_image",
									"type": "image",
									"x": -10,
									"y": -10,
									"image": PATH + "wheel/1.tga",
								},
								{
									"name": "wheel_item_slot",
									"type": "slot",
									"x": -21,
									"y": -43,
									"width": 329,
									"height": 329,

									"slot": (
										{"index": 0, "x": 161, "y": 53, "width": 32, "height": 32},
										{"index": 1, "x": 207, "y": 63, "width": 32, "height": 32},
										{"index": 2, "x": 248, "y": 93, "width": 32, "height": 32},
										{"index": 3, "x": 277, "y": 135, "width": 32, "height": 32},
										{"index": 4, "x": 290, "y": 182, "width": 32, "height": 32},
										{"index": 5, "x": 277, "y": 228, "width": 32, "height": 32},
										{"index": 6, "x": 248, "y": 271, "width": 32, "height": 32},
										{"index": 7, "x": 207, "y": 298, "width": 32, "height": 32},
										{"index": 8, "x": 160, "y": 307, "width": 32, "height": 32},
										{"index": 9, "x": 116, "y": 296, "width": 32, "height": 32},
										{"index": 10, "x": 72, "y": 269, "width": 32, "height": 32},
										{"index": 11, "x": 44, "y": 229, "width": 32, "height": 32},
										{"index": 12, "x": 30, "y": 183, "width": 32, "height": 32},
										{"index": 13, "x": 41, "y": 133, "width": 32, "height": 32},
										{"index": 14, "x": 70, "y": 92, "width": 32, "height": 32},
										{"index": 15, "x": 112, "y": 63, "width": 32, "height": 32},
									),
								},
								{
									"name": "wheel_start_button",
									"type": "button",
									"x": 114,
									"y": 114,
									"default_image": PATH + "wheel/button.tga",
									"over_image": PATH + "wheel/button_down.tga",
									"down_image": PATH + "wheel/button_hover.tga",

									"text": "",
								},
							),
						},
					),
				},
				{
					"name": "promo_window",
					"type": "window",
					"x": 261,
					"y": 139,
					"width": 415,
					"height": 312,
					"children": (
						{
							"name" : "particles",
							"type" : "image",
							"x":0,"y":100,
							"image":PATH+"particles.png",
						},
						{
							"name" : "promo_input",
							"type" : "image",
							"x":0,"y":0,
							"image":PATH+"promo_input.png",
							"children":
							(
								{
									"name" : "promo_text",
									"type" : "text",
									"x" : 0, "y" : -35,
									"text" : "WprowadŸ kod promocjny",
									"fontname":"Tahoma:14b",
									"color":0xfff0e2ec,
									"outline":"1",
									"all_align":"center",
								},
								{
									"name" : "CodeFrameSlot",
									"type" : "image",
									"horizontal_align":"center",
									"x":0, "y":46,
									"image" : PATH+"promo_slot.png",
									"children" :
									(
										{
											"name" : "CodeFrameSlotText",
											"type" : "editline",
											"x" : 4,
											"y" : 2,
											"width" : 100,
											"height" : 17,
											"input_limit" : 10,
											"secret_flag" : 0,
											"text" : "",
										},
									),
								},
								{
									"name" : "rectangle",
									"type" : "image",
									"x":86,"y":20,
									"vertical_align":"center",
									"image":PATH+"rectangle.png",
								},
								{
									"name" : "promo_text2",
									"type" : "text",
									"x" : 0, "y" : 35,
									"text" : "Kody promocyjne mo¿esz znaleŸæ na naszym fanpage oraz discordzie.",
									"fontname":"Tahoma:14",
									"color":0xfff0e2ec,
									"outline":"1",
									"all_align":"center",
								},
							),
						},
						{
							"name" : "promo_btn",
							"type" : "button",
							"x" : 128, "y" : 120,
							"all_align":"center",
							"default_image":PATH+"category_btn.png",
							"over_image":PATH+"category_btn1.png",
							"down_image":PATH+"category_btn2.png",
							"children":
							(
								{
									"name" : "promo_btnText",
									"type" : "text",
									"x" : 0, "y" : 0,
									"text" : "Akceptuj",
									"fontname":"Tahoma:14b",
									# "color":0xffffffff,
									"outline":"1",
									"all_align":"center",
								},
							),
						},
					),
				},
				{
					"name": "item_loading",
					"type": "ani_image",

					"x": (830 - 16) / 2,
					"y": (608 - 16) / 2,

					"delay": 6,

					"images": (
						"d:/ymir work/ui/loading_ani/00.tga",
						"d:/ymir work/ui/loading_ani/01.tga",
						"d:/ymir work/ui/loading_ani/02.tga",
						"d:/ymir work/ui/loading_ani/03.tga",
						"d:/ymir work/ui/loading_ani/04.tga",
						"d:/ymir work/ui/loading_ani/05.tga",
						"d:/ymir work/ui/loading_ani/06.tga",
						"d:/ymir work/ui/loading_ani/07.tga",
					),
				},
			),
		},
	),
}
