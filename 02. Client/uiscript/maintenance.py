import uiScriptLocale

window = {
	"name" : "MaintenanceWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 60,

	"width" : 264,
	"height" : 85,

	"children" :
	(
		{
			"name" : "Thinboard",
			"type" : "thinboard",

			"x" : 0,
			"y" : 0,
			"width" : 264,
			"height" : 85,

			"children" :
			(
				{
					"name" : "maintitle",
					"type" : "text",

					"x" : 0,
					"y" : 17,

					"color" : 0xfff8d090,

					"text" : "Karbantartási rendszer",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "maintitleimage",
					"type" : "image",

					"x" : 0,
					"y" : 10,

					"image" : "d:/ymir work/ui/center.tga",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message1",
					"type" : "text",

					"x" : 0,
					"y" : 50,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message2",
					"type" : "text",

					"x" : 0,
					"y" : 65,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
			),
		},
	),
}