#-*- coding: iso-8859-1 -*-
import item
import app

BOARD_WIDTH = 300
BOARD_HEIGHT = 325
IMG_PATH = "d:/ymir work/ui/mount_window/"

if app.ENABLE_MOUNT_COSTUME_SYSTEM:
    window = {
        "name" : "Pet_Window",
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
                                "x":0,"y":-3,
                                "text": "Zwierzak",
                                "outline":"1",
                                "all_align":"center",
                            },
                        ),
                    },
                    {
                        "name" : "ExtraBonus",
                        "type" : "image",
                        "x" : 17, 
                        "y" : 7,
                        "horizontal_align":"left",
                        "image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
                    },
                    { 
                        "name" : "info_board", "type" : "image", "x" : 57, "y" : 35, "image" : IMG_PATH + "base-image.png",
                        "children" :
                        (
                            {
                                "name" : "window",
                                "type" : "window",
                                "x": 58, "y": 3,
                                "width":172,"height":12,
                                "children" :
                                (
                                    { "name" : "mount_level", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center" },
                                ),
                            },
                            { "name" : "exp_hover_info", "type" : "window", "x" : 109, "y" : 24, "width" : 75, "height" : 18},
                            { "name" : "exp_gauge_01", "type" : "expanded_image", "x" : 109 + 0, "y" : 24, "image" : IMG_PATH + "exp_full.png" },
                            { "name" : "exp_gauge_02", "type" : "expanded_image", "x" : 109 + 18, "y" : 24, "image" : IMG_PATH + "exp_full.png" },
                            { "name" : "exp_gauge_03", "type" : "expanded_image", "x" : 109 + 36, "y" : 24, "image" : IMG_PATH + "exp_full.png" },
                            { "name" : "exp_gauge_04", "type" : "expanded_image", "x" : 109 + 54, "y" : 24, "image" : IMG_PATH + "exp_full.png" },
                        ),
                    },
                    {
                        "name" : "MountSlotImage",
                        "type" : "image",
                        "x" : 12,
                        "y" : 36,
                        "image":"d:/ymir work/ui/detach_slot2.png",
                        "children":
                        (
                            {
                                "name" : "islot",
                                "type" : "slot",

                                "x" : 5,
                                "y" : 4,

                                "width" : 32,
                                "height" : 32,

                                "slot" : (
                                            {"index":item.SLOT_ITEM_NEW_PET, "x":0, "y":0, "width":32, "height":32},
                                        ),
                            },
                        ),
                    },
                    {
                        "name":"header",
                        "type":"image",
                        "x" : 20,
                        "y" : 85,
                        "image":"d:/ymir work/ui/offline_shop/test_image_020.tga",
                        "children":
                        (
                            {
                                "name":"header_text",
                                "type":"text",
                                "x":0,"y":-1,
                                "all_align":"center",
                                "outline":"1",
                                "text":"Umiejêtnoœci pasywne",
                            },
                            {
                                "name":"um",
                                "type":"image",
                                "x" : 7,
                                "y" : 3,
                                "image":IMG_PATH+"um.png",
                                "children":
                                (
                                    {
                                        "name":"skill_points",
                                        "type":"text",
                                        "x":0,"y":-1,
                                        "all_align":"center",
                                        "outline":"1",
                                        "text":"0",
                                    },
                                ),
                            },
                        ),
                    },
                    {
                        "name":"skill_window",
                        "type":"thinboard_circle",
                        "x":12,"y":108,
                        "width":272,"height":109,
                        "children":
                        (
                            {
                                "name":"skill_board",
                                "type":"image",
                                "x":9,"y":10,
                                "image":IMG_PATH+"item-image0.png",
                            },
                            {
                                "name" : "skills", "type" : "grid_table",

                                "x" : 40, "y" : 22,

                                "start_index" : 0,
                                "x_count" : 6,
                                "y_count" : 2,
                                "x_step" : 0+32,
                                "y_step" : 32,
                                # "start_index" : item.SLOT_ITEM_NEW_PET_EQ_START,

                                "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                            },		
                        ),
                    },
                    {
                        "name":"header",
                        "type":"image",
                        "x" : 20,
                        "y" : 223,
                        "image":"d:/ymir work/ui/offline_shop/test_image_020.tga",
                        "children":
                        (
                            {
                                "name":"header_text",
                                "type":"text",
                                "x":0,"y":-1,
                                "all_align":"center",
                                "outline":"1",
                                "text":"Ekwipunek",
                            },
                        ),
                    },
                    {
                        "name":"horseshoe_window",
                        "type":"thinboard_circle",
                        "x":12,"y":245,
                        "width":272,"height":65,
                        "children":
                        (
                            {
                                "name":"horseshoe_board",
                                "type":"image",
                                "x":9,"y":10,
                                "image":IMG_PATH+"item-image-base.png",
                                "children":
                                (
                                    {
                                        "name" : "eqs",
                                        "type" : "grid_table",

                                        "start_index" : item.SLOT_ITEM_NEW_PET_EQ_START,
                                        "x" : 70,
                                        "y" : 6,
                                        "x_count" : 3,
                                        "y_count" : 1,
                                        "x_step" : 45,
                                        "y_step" : 32,
                                        "image" : "d:/ymir work/ui/public/Slot_Base.sub",
                                    },
                                ),
                            },
                        ),
                    },
                ),
            },
        ),
    }