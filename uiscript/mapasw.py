import uiScriptLocale

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH

window = {
	"name" : "MapaSwWindow",
	"style" : ("float",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,
	"children" :
	(	
		{	
			"name" : "chui",
			"type" : "bar",
        
			"x" : 0,
			"y" : 0,
			"width" : SCREEN_WIDTH,
			"height" : SCREEN_HEIGHT,
			"children" :
			(
				{
					"name" : "mapa_image",
					"type" : "image",
					"x" : ((SCREEN_WIDTH) / 2) - (800/2),
					"y" : ((SCREEN_HEIGHT) / 2) - (584/2),
					"image" : 'locale/faris/mapa/map.png',
					"children" :
					(	
						{"name":"jinno",			"type":"button",	"x":150,	"y":370,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/jinno_m1_0.png",			"over_image":"locale/faris/mapa/jinno_m1_1.png",			"down_image":"locale/faris/mapa/jinno_m1_2.png",		"text":"",},
						{"name":"jinno2",			"type":"button",	"x":200,	"y":410,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/jinno_m2_0.png",			"over_image":"locale/faris/mapa/jinno_m2_1.png",			"down_image":"locale/faris/mapa/jinno_m2_2.png",		"text":"",},	
						{"name":"shinso",			"type":"button",	"x":75,	"y":240,		"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/shinsoo_m1_0.png",			"over_image":"locale/faris/mapa/shinsoo_m1_1.png",		"down_image":"locale/faris/mapa/shinsoo_m1_2.png",		"text":"",},
						{"name":"shinso2",			"type":"button",	"x":150,	"y":275,		"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/shinsoo_m2_0.png",			"over_image":"locale/faris/mapa/shinsoo_m2_1.png",		"down_image":"locale/faris/mapa/shinsoo_m2_2.png",		"text":"",},
						{"name":"dalsze_zakatki",	"type":"button",	"x":120,	"y":320,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/dalszezakatki_0.png",	"over_image":"locale/faris/mapa/dalszezakatki_1.png",	"down_image":"locale/faris/mapa/dalszezakatki_2.png",		"text":"",},
						{"name":"sohan",				"type":"button",	"x":368,	"y":230,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/sohan_0.png",		"over_image":"locale/faris/mapa/sohan_1.png",		"down_image":"locale/faris/mapa/sohan_2.png",		"text":"",},
						{"name":"orki",				"type":"button",	"x":215,	"y":220,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/dolina_orkow_0.png",		"over_image":"locale/faris/mapa/dolina_orkow_1.png",		"down_image":"locale/faris/mapa/dolina_orkow_2.png",		"text":"",},
						{"name":"lasduchow",				"type":"button",	"x":300,	"y":275,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/lasduchow_0.png",		"over_image":"locale/faris/mapa/lasduchow_1.png",		"down_image":"locale/faris/mapa/lasduchow_2.png",		"text":"",},
						{"name":"loch_paj2",				"type":"button",	"x":325,	"y":322,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/loch_pajakow_0.png",		"over_image":"locale/faris/mapa/loch_pajakow_1.png",		"down_image":"locale/faris/mapa/loch_pajakow_2.png",		"text":"",},
                        {"name":"dolina_cyklopow",				"type":"button",	"x":320,	"y":370,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/dolina_cyklopow_0.png",		"over_image":"locale/faris/mapa/dolina_cyklopow_1.png",		"down_image":"locale/faris/mapa/dolina_cyklopow_2.png",		"text":"",},
                        {"name":"pustykowie_faraona",				"type":"button",	"x":513,	"y":340,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/pustkowie_0.png",		"over_image":"locale/faris/mapa/pustkowie_1.png",		"down_image":"locale/faris/mapa/pustkowie_2.png",		"text":"",},
                        {"name":"mrozna_kraina",				"type":"button",	"x":470,	"y":285,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/mroznakraina_0.png",		"over_image":"locale/faris/mapa/mroznakraina_1.png",		"down_image":"locale/faris/mapa/mroznakraina_2.png",		"text":"",},
                        {"name":"ognista_ziemia",				"type":"button",	"x":510,	"y":235,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/ognistaziemia_0.png",		"over_image":"locale/faris/mapa/ognistaziemia_1.png",		"down_image":"locale/faris/mapa/ognistaziemia_2.png",		"text":"",},
                        {"name":"zaczarowany_las",				"type":"button",	"x":420,	"y":183,	"tooltip_text":"",	"tooltip_y":3,		"default_image":"locale/faris/mapa/zaczarowanylas_0.png",		"over_image":"locale/faris/mapa/zaczarowanylas_1.png",		"down_image":"locale/faris/mapa/zaczarowanylas_2.png",		"text":"",},
					),
				},
				
			),
		},
	),
}