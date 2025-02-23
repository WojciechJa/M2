import uiScriptLocale

window = {
    "name" : "EventManageWindow",
    "style" : ("movable", "float",),
    "x" : SCREEN_WIDTH/2 - 175,
    "y" : SCREEN_HEIGHT/2 - 200,
    "width" : 350,
    "height" : 400,
    "children" :
    (
        {
            "name" : "board",
            "type" : "board_with_titlebar",
            "style" : ("attach",),
            "x" : 0,
            "y" : 0,
            "width" : 350,
            "height" : 400,
            "title" : "Event Manager",
            "children" :
            (
                {
                    "name" : "eventTypeLabel",
                    "type" : "text",
                    "x" : 15,
                    "y" : 35,
                    "text" : "Event Type:",
                },
                {
                    "name" : "eventTypeSlot",
                    "type" : "image",
                    "x" : 15,
                    "y" : 50,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_05.sub",
                    "children" :
                    (
                        {
                            "name" : "eventTypeCombo",
                            "type" : "dropdown",
                            "x" : 3,
                            "y" : 1,
                            "width" : 314,
                            "height" : 18,
                        },
                    ),
                },
                {
                    "name" : "startTimeLabel",
                    "type" : "text",
                    "x" : 15,
                    "y" : 80,
                    "text" : "Start Time:",
                },
                {
                    "name" : "startTimeSlot",
                    "type" : "image",
                    "x" : 15,
                    "y" : 95,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_05.sub",
                    "children" :
                    (
                        {
                            "name" : "startTimeInput",
                            "type" : "editline",
                            "x" : 3,
                            "y" : 3,
                            "width" : 94,
                            "height" : 14,
                            "input_limit" : 8,
                        },
                    ),
                },
                {
                    "name" : "endTimeLabel",
                    "type" : "text",
                    "x" : 125,
                    "y" : 80,
                    "text" : "End Time:",
                },
                {
                    "name" : "endTimeSlot",
                    "type" : "image",
                    "x" : 125,
                    "y" : 95,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_05.sub",
                    "children" :
                    (
                        {
                            "name" : "endTimeInput",
                            "type" : "editline",
                            "x" : 3,
                            "y" : 3,
                            "width" : 94,
                            "height" : 14,
                            "input_limit" : 8,
                        },
                    ),
                },
                {
                    "name" : "empireFlagSlot",
                    "type" : "image",
                    "x" : 15,
                    "y" : 140,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_05.sub",
                    "children" :
                    (
                        {
                            "name" : "empireFlagCombo",
                            "type" : "dropdown",
                            "x" : 3,
                            "y" : 1,
                            "width" : 94,
                            "height" : 18,
                        },
                    ),
                },
                {
                    "name" : "channelSlot",
                    "type" : "image",
                    "x" : 125,
                    "y" : 140,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_03.sub",
                    "children" :
                    (
                        {
                            "name" : "channelInput",
                            "type" : "editline",
                            "x" : 3,
                            "y" : 3,
                            "width" : 44,
                            "height" : 14,
                            "input_limit" : 3,
                            "only_number" : 1,
                        },
                    ),
                },
                # Values
                {
                    "name" : "valueLabel",
                    "type" : "text",
                    "x" : 15,
                    "y" : 170,
                    "text" : "Values:",
                },
            ) + tuple(
                {
                    "name": "valueSlot%d" % i,
                    "type": "image",
                    "x": 15 + (i * 70),
                    "y": 185,
                    "image": "d:/ymir work/ui/public/Parameter_Slot_03.sub",
                    "children":
                        (
                            {
                                "name": "valueInput%d" % i,
                                "type": "editline",
                                "x": 3,
                                "y": 3,
                                "width": 54,
                                "height": 14,
                                "input_limit": 8,
                                "only_number": 1,
                            },
                        ),
                } for i in range(4)
            ) + (
                {
                    "name" : "eventListSlot",
                    "type" : "image",
                    "x" : 15,
                    "y" : 215,
                    "image" : "d:/ymir work/ui/public/Parameter_Slot_05.sub",
                    "children" :
                    (
                        {
                            "name" : "eventList",
                            "type" : "listboxex",
                            "x" : 0,
                            "y" : 0,
                            "width" : 320,
                            "height" : 120,
                        },
                    ),
                },
                {
                    "name" : "addButton",
                    "type" : "button",
                    "x" : 15,
                    "y" : 350,
                    "text" : "Add",
                    "default_image" : "d:/ymir work/ui/public/small_button_01.sub",
                    "over_image" : "d:/ymir work/ui/public/small_button_02.sub",
                    "down_image" : "d:/ymir work/ui/public/small_button_03.sub",
                },
                {
                    "name" : "removeButton",
                    "type" : "button",
                    "x" : 125,
                    "y" : 350,
                    "text" : "Remove",
                    "default_image" : "d:/ymir work/ui/public/small_button_01.sub",
                    "over_image" : "d:/ymir work/ui/public/small_button_02.sub",
                    "down_image" : "d:/ymir work/ui/public/small_button_03.sub",
                },
                {
                    "name" : "modifyButton",
                    "type" : "button",
                    "x" : 235,
                    "y" : 350,
                    "text" : "Modify",
                    "default_image" : "d:/ymir work/ui/public/small_button_01.sub",
                    "over_image" : "d:/ymir work/ui/public/small_button_02.sub",
                    "down_image" : "d:/ymir work/ui/public/small_button_03.sub",
                },
            ),
        },
    ),
}