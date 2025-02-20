import uiScriptLocale

window = {
	"name" : "IkonkiWindow",
	"x" : SCREEN_WIDTH - 480,
	"y" : 0,
	"width" : 300,
	"height" : 120,
	"children" :
	(
		{
			"name" : "expander",
			"type" : "button",
			"x":90,
			"y":1,
			"default_image" : "d:/ymir work/ui/btn_expand_normal.png",
			"over_image" : "d:/ymir work/ui/btn_expand_over.png",
			"down_image" : "d:/ymir work/ui/btn_expand_down.png",
		},
		{
			"name" : "ItemShopButton",
			"type" : "button",
			"x":200,
			"y":20,
			"default_image" : "arezzo/is_1.png",
			"over_image" : "arezzo/is_2.png",
			"down_image" : "arezzo/is_3.png",
		},
		{
			"name" : "EventButton",
			"type" : "button",
			"x":92,
			"y":19,
			"default_image" : "arezzo/kalendarz_1.png",
			"over_image" : "arezzo/kalendarz_2.png",
			"down_image" : "arezzo/kalendarz_3.png",
		},
		{
			"name" : "BattlepassButton",
			"type" : "button",
			"x":0,
			"y":19,
			"default_image" : "arezzo/battlepass_1.png",
			"over_image" : "arezzo/battlepass_2.png",
			"down_image" : "arezzo/battlepass_3.png",
		},
	),
}