import ui
import app
import net
import player
import chat
import time
import wndMgr
import localeInfo
import constInfo
import chr
from uiToolTip import ItemToolTip
from uiEventCalendar import events_default_data, EVENT_INDEX


def IsGM():
    return chr.IsGameMaster(player.GetMainCharacterIndex())


class EventManageWindow(ui.BoardWithTitleBar):
    def __init__(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

        ui.BoardWithTitleBar.__init__(self)
        self.SetSize(350, 400)
        self.SetCenterPosition()
        self.AddFlag("movable")
        self.AddFlag("float")
        self.SetTitleName("Event Manager")

        self.dayIndex = 0
        self.currentYear = 0
        self.currentMonth = 0

        self.LoadWindow()

    def LoadWindow(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            self.Close()
            return

        try:
            # Event Type ComboBox
            self.eventTypeLabel = ui.TextLine()
            self.eventTypeLabel.SetParent(self)
            self.eventTypeLabel.SetPosition(15, 35)
            self.eventTypeLabel.SetText("Event Type:")
            self.eventTypeLabel.Show()

            # Reszta kodu pozostaje taka sama, usuwamy tylko część z TitleBar
            self.eventTypeCombo = ui.ComboBox()
            self.eventTypeCombo.SetParent(self)
            self.eventTypeCombo.SetPosition(15, 50)
            self.eventTypeCombo.SetSize(320, 20)
            self.eventTypeCombo.Show()

            for id, data in events_default_data.items():
                self.eventTypeCombo.InsertItem(id, data[1])

            # Time Inputs
            self.startTimeLabel = ui.TextLine()
            self.startTimeLabel.SetParent(self)
            self.startTimeLabel.SetPosition(15, 80)
            self.startTimeLabel.SetText("Start Time:")
            self.startTimeLabel.Show()

            self.startTimeInput = ui.EditLine()
            self.startTimeInput.SetParent(self)
            self.startTimeInput.SetPosition(15, 95)
            self.startTimeInput.SetSize(100, 20)
            self.startTimeInput.SetMax(8)
            self.startTimeInput.Show()

            self.endTimeLabel = ui.TextLine()
            self.endTimeLabel.SetParent(self)
            self.endTimeLabel.SetPosition(125, 80)
            self.endTimeLabel.SetText("End Time:")
            self.endTimeLabel.Show()

            self.endTimeInput = ui.EditLine()
            self.endTimeInput.SetParent(self)
            self.endTimeInput.SetPosition(125, 95)
            self.endTimeInput.SetSize(100, 20)
            self.endTimeInput.SetMax(8)
            self.endTimeInput.Show()

            # Empire and Channel
            self.empireFlagCombo = ui.ComboBox()
            self.empireFlagCombo.SetParent(self)
            self.empireFlagCombo.SetPosition(15, 140)
            self.empireFlagCombo.SetSize(100, 20)
            self.empireFlagCombo.Show()

            empireNames = ["All Empires", "Red Empire", "Yellow Empire", "Blue Empire"]
            for i, name in enumerate(empireNames):
                self.empireFlagCombo.InsertItem(i, name)

            self.channelInput = ui.EditLine()
            self.channelInput.SetParent(self)
            self.channelInput.SetPosition(125, 140)
            self.channelInput.SetSize(50, 20)
            self.channelInput.SetNumberMode()
            self.channelInput.SetMax(3)
            self.channelInput.Show()

            # Values
            self.valueInputs = []
            for i in range(4):
                input = ui.EditLine()
                input.SetParent(self)
                input.SetPosition(15 + (i * 70), 185)
                input.SetSize(60, 20)
                input.SetNumberMode()
                input.SetMax(8)
                input.Show()
                self.valueInputs.append(input)

            # Event List
            self.eventList = ui.ListBox()
            self.eventList.SetParent(self)
            self.eventList.SetPosition(15, 215)
            self.eventList.SetSize(320, 120)
            self.eventList.Show()

            # Buttons
            self.addButton = ui.Button()
            self.addButton.SetParent(self)
            self.addButton.SetPosition(15, 350)
            self.addButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
            self.addButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
            self.addButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
            self.addButton.SetText("Add")
            self.addButton.SetEvent(ui.__mem_func__(self.__OnAdd))
            self.addButton.Show()

            self.removeButton = ui.Button()
            self.removeButton.SetParent(self)
            self.removeButton.SetPosition(125, 350)
            self.removeButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
            self.removeButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
            self.removeButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
            self.removeButton.SetText("Remove")
            self.removeButton.SetEvent(ui.__mem_func__(self.__OnRemove))
            self.removeButton.Show()

            self.modifyButton = ui.Button()
            self.modifyButton.SetParent(self)
            self.modifyButton.SetPosition(235, 350)
            self.modifyButton.SetUpVisual("d:/ymir work/ui/public/small_button_01.sub")
            self.modifyButton.SetOverVisual("d:/ymir work/ui/public/small_button_02.sub")
            self.modifyButton.SetDownVisual("d:/ymir work/ui/public/small_button_03.sub")
            self.modifyButton.SetText("Modify")
            self.modifyButton.SetEvent(ui.__mem_func__(self.__OnModify))
            self.modifyButton.Show()

        except:
            import exception
            exception.Abort("EventManageWindow.LoadWindow")

    def Open(self, dayIndex):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, "You need GM privileges to use this feature.")
            return

        self.dayIndex = dayIndex
        self.Show()
        self.SetCenterPosition()

        now = time.localtime()
        self.currentYear = now.tm_year
        self.currentMonth = now.tm_mon

        self.startTimeInput.SetText("00:00:00")
        self.endTimeInput.SetText("23:59:59")

    def Close(self):
        self.Hide()

    def OnPressEscapeKey(self):
        self.Close()
        return True

    def LoadEvents(self, events):
        self.eventList.ClearItem()
        for eventID, eventData in events.items():
            self.eventList.AppendItem(events_default_data[eventData[EVENT_INDEX]][1])

    def __OnAdd(self):
        if not player.IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, "You need GM privileges to use this feature.")
            return

        eventIndex = self.eventTypeCombo.GetSelectedItem()
        startTime = "%04d-%02d-%02d %s" % (
            self.currentYear,
            self.currentMonth,
            self.dayIndex,
            self.startTimeInput.GetText()
        )
        endTime = "%04d-%02d-%02d %s" % (
            self.currentYear,
            self.currentMonth,
            self.dayIndex,
            self.endTimeInput.GetText()
        )

        empireFlag = self.empireFlagCombo.GetSelectedItem()
        channelFlag = int(self.channelInput.GetText() or "0")

        values = []
        for inp in self.valueInputs:
            values.append(inp.GetText() or "0")

        cmd = "/event_manager add %d %s %s %d %d %s" % (
            eventIndex,
            startTime,
            endTime,
            empireFlag,
            channelFlag,
            " ".join(values)
        )

        net.SendChatPacket(cmd)

    def __OnRemove(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

        selectedID = self.eventList.GetSelectedItem()
        if selectedID == -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Please select an event to remove.")
            return

        cmd = "/event_manager remove %d" % selectedID
        net.SendChatPacket(cmd)

    def __OnModify(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

        selectedID = self.eventList.GetSelectedItem()
        if selectedID == -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Please select an event to modify.")
            return

        eventIndex = self.eventTypeCombo.GetSelectedItem()
        startTime = "%04d-%02d-%02d %s" % (
            self.currentYear,
            self.currentMonth,
            self.dayIndex,
            self.startTimeInput.GetText()
        )
        endTime = "%04d-%02d-%02d %s" % (
            self.currentYear,
            self.currentMonth,
            self.dayIndex,
            self.endTimeInput.GetText()
        )

        empireFlag = self.empireFlagCombo.GetSelectedItem()
        channelFlag = int(self.channelInput.GetText() or "0")

        values = []
        for inp in self.valueInputs:
            values.append(inp.GetText() or "0")

        cmd = "/event_manager modify %d %d %s %s %d %d %s" % (
            selectedID,
            eventIndex,
            startTime,
            endTime,
            empireFlag,
            channelFlag,
            " ".join(values)
        )

        net.SendChatPacket(cmd)