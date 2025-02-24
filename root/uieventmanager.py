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

        self.AddFlag("not_pick")

        self.dayIndex = 0
        self.currentYear = 0
        self.currentMonth = 0

        # Dodajemy zmienne do śledzenia wybranych wartości
        self.selectedEventIndex = 0
        self.selectedEmpire = 0
        self.selectedChannel = 0

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

            self.eventTypeCombo = ui.ComboBox()
            self.eventTypeCombo.SetParent(self)
            self.eventTypeCombo.SetPosition(15, 50)
            self.eventTypeCombo.SetSize(320, 20)
            self.eventTypeCombo.SetEvent(ui.__mem_func__(self.__OnSelectEventType))
            # Ustawiamy tekst domyślny
            self.eventTypeCombo.SetCurrentItem(localeInfo.UI_ITEM)
            self.eventTypeCombo.Show()

            # Time Inputs
            self.startTimeLabel = ui.TextLine()
            self.startTimeLabel.SetParent(self)
            self.startTimeLabel.SetPosition(15, 80)
            self.startTimeLabel.SetText("Start Time:")
            self.startTimeLabel.Show()

            self.startTimeSlot = ui.SlotBar()
            self.startTimeSlot.SetParent(self)
            self.startTimeSlot.SetPosition(15, 95)
            self.startTimeSlot.SetSize(100, 20)
            self.startTimeSlot.Show()

            self.startTimeInput = ui.EditLine()
            self.startTimeInput.SetParent(self.startTimeSlot)
            self.startTimeInput.SetPosition(3, 3)
            self.startTimeInput.SetSize(94, 14)
            self.startTimeInput.SetMax(8)
            self.startTimeInput.Show()

            self.endTimeLabel = ui.TextLine()
            self.endTimeLabel.SetParent(self)
            self.endTimeLabel.SetPosition(125, 80)
            self.endTimeLabel.SetText("End Time:")
            self.endTimeLabel.Show()

            self.endTimeSlot = ui.SlotBar()
            self.endTimeSlot.SetParent(self)
            self.endTimeSlot.SetPosition(125, 95)
            self.endTimeSlot.SetSize(100, 20)
            self.endTimeSlot.Show()

            self.endTimeInput = ui.EditLine()
            self.endTimeInput.SetParent(self.endTimeSlot)
            self.endTimeInput.SetPosition(3, 3)
            self.endTimeInput.SetSize(94, 14)
            self.endTimeInput.SetMax(8)
            self.endTimeInput.Show()

            # Empire ComboBox
            self.empireFlagLabel = ui.TextLine()
            self.empireFlagLabel.SetParent(self)
            self.empireFlagLabel.SetPosition(15, 125)
            self.empireFlagLabel.SetText("Empire:")
            self.empireFlagLabel.Show()

            self.empireFlagCombo = ui.ComboBox()
            self.empireFlagCombo.SetParent(self)
            self.empireFlagCombo.SetPosition(15, 140)
            self.empireFlagCombo.SetSize(100, 20)
            self.empireFlagCombo.SetEvent(ui.__mem_func__(self.__OnSelectEmpire))
            self.empireFlagCombo.Show()

            # Channel ComboBox
            self.channelLabel = ui.TextLine()
            self.channelLabel.SetParent(self)
            self.channelLabel.SetPosition(125, 125)
            self.channelLabel.SetText("Channel:")
            self.channelLabel.Show()

            self.channelCombo = ui.ComboBox()
            self.channelCombo.SetParent(self)
            self.channelCombo.SetPosition(125, 140)
            self.channelCombo.SetSize(50, 20)
            self.channelCombo.SetEvent(ui.__mem_func__(self.__OnSelectChannel))
            self.channelCombo.Show()

            # Values
            self.valueLabel = ui.TextLine()
            self.valueLabel.SetParent(self)
            self.valueLabel.SetPosition(15, 170)
            self.valueLabel.SetText("Values:")
            self.valueLabel.Show()

            self.valueSlots = []
            self.valueInputs = []
            for i in range(4):
                slot = ui.SlotBar()
                slot.SetParent(self)
                slot.SetPosition(15 + (i * 70), 185)
                slot.SetSize(60, 20)
                slot.Show()
                self.valueSlots.append(slot)

                input = ui.EditLine()
                input.SetParent(slot)
                input.SetPosition(3, 3)
                input.SetSize(54, 14)
                input.SetNumberMode()
                input.SetMax(8)
                input.Show()
                self.valueInputs.append(input)

            # Event List
            self.eventListBar = ui.Bar()
            self.eventListBar.SetParent(self)
            self.eventListBar.SetPosition(15, 215)
            self.eventListBar.SetSize(320, 120)
            self.eventListBar.SetColor(0xFF000000)
            self.eventListBar.Show()

            self.eventList = ui.ListBoxEx()
            self.eventList.SetParent(self.eventListBar)
            self.eventList.SetPosition(2, 2)
            self.eventList.SetSize(316, 116)
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

            # Wypełniamy listy
            for id, data in events_default_data.items():
                self.eventTypeCombo.InsertItem(id, data[1])

            empireNames = ["All", "Shinsoo", "Chunjo", "Jinno"]
            for i, name in enumerate(empireNames):
                self.empireFlagCombo.InsertItem(i, name)

            channelNames = ["All"] + [str(i) for i in range(1, 7)]
            for i, name in enumerate(channelNames):
                self.channelCombo.InsertItem(i, name)

        except:
            import exception
            exception.Abort("EventManageWindow.LoadWindow")

    def __OnSelectEventType(self, index):
        self.selectedEventIndex = index
        # Zmiana z GetText() na GetCurrentText()
        name = self.eventTypeCombo.GetCurrentText()  # ComboBox używa GetCurrentText zamiast GetText

    def __OnSelectEmpire(self, index):
        self.selectedEmpire = index
        name = self.empireFlagCombo.GetCurrentText()

    def __OnSelectChannel(self, index):
        self.selectedChannel = index
        name = self.channelCombo.GetCurrentText()

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

        # Reset selection
        self.selectedEventIndex = 0
        self.selectedEmpire = 0
        self.selectedChannel = 0

        # Set default items - używamy indeksu zamiast tekstu
        self.eventTypeCombo.SetCurrentItem(0)  # Zmiana z tekstu na indeks
        self.empireFlagCombo.SetCurrentItem(0)
        self.channelCombo.SetCurrentItem(0)

        # Czyszczenie listy eventów
        if self.eventList:
            self.eventList.ClearItems()

        self.SetTop()

    def Close(self):
        self.Hide()

    def OnPressEscapeKey(self):
        self.Close()
        return True

    def LoadEvents(self, events):
        self.eventList.ClearItem()
        for eventID, eventData in events.items():
            self.eventList.AppendItem(events_default_data[eventData[EVENT_INDEX]][1])

    def OnTop(self):
        self.SetTop()

    def Show(self):
        ui.BoardWithTitleBar.Show(self)
        self.OnTop()

    def Close(self):
        self.Hide()

    def __OnAdd(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

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

        values = []
        for inp in self.valueInputs:
            values.append(inp.GetText() or "0")

        # Format: add <eventIndex> <startDate> <startTime> <endDate> <endTime> <empireFlag> <channelFlag> [value0] [value1] [value2] [value3] <dayIndex>
        cmd = "/event_manager add %d %s %s %s %s %d %d %s %s %s %s %d" % (
            self.selectedEventIndex,  # eventIndex
            startTime.split()[0],  # startDate
            startTime.split()[1],  # startTime
            endTime.split()[0],  # endDate
            endTime.split()[1],  # endTime
            self.selectedEmpire,  # empireFlag
            self.selectedChannel,  # channelFlag
            values[0],  # value0
            values[1],  # value1
            values[2],  # value2
            values[3],  # value3
            self.dayIndex  # dayIndex
        )

        net.SendChatPacket(cmd)

    def __OnRemove(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

        # Używamy wybranego eventu z ComboBox
        if self.selectedEventIndex == 0:  # Zakładając że 0 to wartość domyślna/nieprawidłowa
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Please select an event type.")
            return

        # Format: remove <eventIndex> <dayIndex>
        cmd = "/event_manager remove %d %d" % (
            self.selectedEventIndex,  # Używamy bezpośrednio wybranego indexu z ComboBox
            self.dayIndex
        )
        net.SendChatPacket(cmd)

    def __OnModify(self):
        if not IsGM():
            chat.AppendChat(chat.CHAT_TYPE_INFO, localeInfo.NOT_GM_PERMISSION)
            return

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

        values = []
        for inp in self.valueInputs:
            values.append(inp.GetText() or "0")

        # Format: modify <eventIndex> <startDate> <startTime> <endDate> <endTime> <empireFlag> <channelFlag> [value0] [value1] [value2] [value3] <dayIndex>
        cmd = "/event_manager modify %d %s %s %s %s %d %d %s %s %s %s %d" % (
            self.selectedEventIndex,  # eventIndex
            startTime.split()[0],  # startDate
            startTime.split()[1],  # startTime
            endTime.split()[0],  # endDate
            endTime.split()[1],  # endTime
            self.selectedEmpire,  # empireFlag
            self.selectedChannel,  # channelFlag
            values[0],  # value0
            values[1],  # value1
            values[2],  # value2
            values[3],  # value3
            self.dayIndex  # dayIndex
        )

        net.SendChatPacket(cmd)