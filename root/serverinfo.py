#-*- coding: iso-8859-1 -*-
import app
# import localeInfo
app.ServerName = None

SRV1 = {
	"name":"Arezzo.pl",

	"host":"146.59.52.209",
	#"host":"188.68.242.219",
	"auth1":30001,
	"ch1":30003,
	"ch2":30015,
	"ch3":30027,
	"ch4":30039,
	"ch5":30051,
	"ch6":30063,

	# "host":"193.177.165.189",
	# "auth1":30001,
	# "ch1":30003,
	# "ch2":30007,
	# "ch3":30011,
	# "ch4":30015,
	# "ch5":30019,
	# "ch6":30023,
}


STATE_NONE = "..."

STATE_DICT = {
	0 : "....",
	1 : "NORM",
	2 : "BUSY",
	3 : "FULL"
}

SERVER1_CHANNEL_DICT = {
	1:{"key":11,"name":"Kana³ 1","ip":SRV1["host"],"tcp_port":SRV1["ch1"],"udp_port":SRV1["ch1"],"state":STATE_NONE,},
	2:{"key":12,"name":"Kana³ 2","ip":SRV1["host"],"tcp_port":SRV1["ch2"],"udp_port":SRV1["ch2"],"state":STATE_NONE,},
	3:{"key":13,"name":"Kana³ 3","ip":SRV1["host"],"tcp_port":SRV1["ch3"],"udp_port":SRV1["ch3"],"state":STATE_NONE,},
	4:{"key":14,"name":"Kana³ 4","ip":SRV1["host"],"tcp_port":SRV1["ch4"],"udp_port":SRV1["ch4"],"state":STATE_NONE,},
	5:{"key":15,"name":"Kana³ 5","ip":SRV1["host"],"tcp_port":SRV1["ch5"],"udp_port":SRV1["ch5"],"state":STATE_NONE,},
	6:{"key":16,"name":"Kana³ 6","ip":SRV1["host"],"tcp_port":SRV1["ch6"],"udp_port":SRV1["ch6"],"state":STATE_NONE,},
}

REGION_NAME_DICT = {
	0 : SRV1["name"],
}

REGION_AUTH_SERVER_DICT = {
	0 : {
		1 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
		2 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
		3 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
		4 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
		5 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
		6 : { "ip":SRV1["host"], "port":SRV1["auth1"], },
	}
}

REGION_DICT = {
	0 : {
		1 : { "name" :SRV1["name"], "channel" : SERVER1_CHANNEL_DICT, },
	},
}

MARKADDR_DICT = {
	10 : { "ip" : SRV1["host"], "tcp_port" : SRV1["ch1"], "mark" : "10.tga", "symbol_path" : "10", },
}

TESTADDR = { "ip" : SRV1["host"], "tcp_port" : SRV1["ch1"], "udp_port" : SRV1["ch1"], }

#DONE


