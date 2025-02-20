# import uiScriptLocale
import item
import player

EQUIPMENT_START_INDEX = player.EQUIPMENT_SLOT_START

window = {
	"name" : "RuneWindow",
	"x" : 0,
	"y" : SCREEN_HEIGHT - 37 - 575,
	"style" : ("movable", "float",),
	"width" : 462,
	"height" : 430,
	"children" :
	(
		## Inventory, Equipment Slots
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : 462,
			"height" : 430,
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
					"width" : 447,
					"color" : "yellow",
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":224, "y":0, "text": "Runy", "outline":"1","text_horizontal_align":"center" },
					),
				},
                {
					"name" : "InfoBtn",
					"type" : "button",
					"x" : 20,
					"y" : 6,
					"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
				{
					"name" : "GageBoard",
					"type" : "window",
					"x" : 5,
					"y" : 27,
					"width" : 462, 
					"height": 432,
					"children" :
					(
						{
							"name" : "BackGage",
							"type" : "ani_image",
							"x" : 0,
							"y" : 0,
							"delay" : 5,
							"images" :
							(
								"arezzo/runes/runes.png",
								"arezzo/runes/runes1.png",
								"arezzo/runes/runes2.png",
								"arezzo/runes/runes3.png",
								"arezzo/runes/runes4.png",
								"arezzo/runes/runes5.png",
								"arezzo/runes/runes6.png",
								"arezzo/runes/runes7.png",
								"arezzo/runes/runes8.png",
								"arezzo/runes/runes9.png",
								"arezzo/runes/runes8.png",
								"arezzo/runes/runes7.png",
								"arezzo/runes/runes6.png",
								"arezzo/runes/runes5.png",
								"arezzo/runes/runes4.png",
								"arezzo/runes/runes3.png",
								"arezzo/runes/runes2.png",
								"arezzo/runes/runes1.png",
								"arezzo/runes/runes.png",
							)
						},
						{
							"name" : "RuneSlot",
							"type" : "slot",
							"x" : 3,
							"y" : 3,
							"width" : 462,
							"height" : 432,
							# "image" : "d:/ymir work/ui/public/Slot_Base.sub",
							"slot" : (
								#	EQ_RUNE = WHITE
								{"index":item.EQUIPMENT_RUNE, "x":204, "y":22, "width":32, "height":32},
								{"index":item.EQUIPMENT_RUNE_RED, "x":106, "y":64, "width":32, "height":32},
								{"index":item.EQUIPMENT_RUNE_GREEN, "x":307, "y":64, "width":32, "height":32},
								
								{"index":item.EQUIPMENT_RUNE_BLUE, "x":109, "y":284, "width":32, "height":32},
								{"index":item.EQUIPMENT_RUNE_YELLOW, "x":205, "y":324, "width":32, "height":32},
								{"index":item.EQUIPMENT_RUNE_BLACK, "x":298, "y":287, "width":32, "height":32},
							),
						},
					),
				},
			),
		},
	),
}

