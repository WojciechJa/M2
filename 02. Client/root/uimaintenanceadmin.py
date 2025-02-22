import ui
import net
import chat


class MaintenanceAdminWindow(ui.ScriptWindow):
	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.LoadDialog()

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def LoadDialog(self):
		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/maintenanceadminwindow.py")
		except:
			import exception
			exception.Abort("MaintenanceAdminWindow.LoadWindow")

		try:
			self.petfeed = self.GetChild("board")
			self.titleBar = self.GetChild("TitleBakim")

			self.editline1 = self.GetChild("InputValue1")
			self.editline2 = self.GetChild("InputValue2")
			self.editline3 = self.GetChild("InputValue3")

			self.okButton = self.GetChild("okButton")
			self.cancelButton = self.GetChild("cancelButton")
			self.whycancelButton = self.GetChild("whycancelButton")
			self.whyupdateButton = self.GetChild("whyupdateButton")
			self.maintenanceupdateButton = self.GetChild("maintenanceupdateButton")
			self.updateclientButton = self.GetChild("updateclientButton")

			self.okButton.SAFE_SetEvent(self.Ok)
			self.cancelButton.SAFE_SetEvent(self.Cancel)
			self.whycancelButton.SAFE_SetEvent(self.Whycancel)
			self.whyupdateButton.SAFE_SetEvent(self.Whyupdate)
			self.maintenanceupdateButton.SAFE_SetEvent(self.Maintenanceupdate)
			self.updateclientButton.SAFE_SetEvent(self.GetClientSet)
			self.titleBar.SetCloseEvent(ui.__mem_func__(self.Close))
		except:
			import exception
			exception.Abort("MaintenanceAdminWindow.LoadWindow")

	def OpenDialog(self):
		self.SetTop()
		self.SetCenterPosition()
		self.editline2.SetFocus()
		self.editline3.SetFocus()
		self.editline1.SetFocus()
		self.Show()

    def Ok(self):
        val1 = int(len(self.editline1.GetText()))
        yazi1 = str(self.editline1.GetText())
        val2 = int(len(self.editline2.GetText()))
        yazi2 = str(self.editline2.GetText())
        val3 = int(len(self.editline3.GetText()))
        yazi3 = str(self.editline3.GetText())

        if "" == yazi1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac powód konserwacji!")
            return

        if val1 < 5:
            chat.AppendChat(5, "Powód konserwacji jest zbyt krótki!")
            return

        if val1 > 30:
            chat.AppendChat(5, "Powód konserwacji jest zbyt dlugi!")
            return

        if yazi1.lower().find('%') != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (%) w powodzie konserwacji!")
            return

        if yazi1.lower().find("'") != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (') w powodzie konserwacji!")
            return

        if "" == yazi2:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac pozostaly czas do konserwacji!")
            return

        if val2 < 3:
            chat.AppendChat(5, "Pozostaly czas do konserwacji jest zbyt krótki!")
            return

        if val2 > 5:
            chat.AppendChat(5, "Pozostaly czas do konserwacji jest zbyt dlugi!")
            return

        if "" == yazi3:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac czasu trwania konserwacji!")
            return

        if val3 < 3:
            chat.AppendChat(5, "Czas trwania konserwacji jest zbyt krótki!")
            return

        if val3 > 5:
            chat.AppendChat(5, "Czas trwania konserwacji jest zbyt dlugi!")
            return

        net.SendChatPacket("/maintenance " + str(yazi2) + " " + str(yazi3))
        net.SendChatPacket("/maintenance_text enable " + str(yazi1))
        self.Close()

    def Cancel(self):
        net.SendChatPacket("/maintenance disable")
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Konserwacja zostala pomyslnie anulowana!")
        self.Close()

    def Whycancel(self):
        net.SendChatPacket("/maintenance_text disable")
        self.GetClientSet()
        self.Close()

    def Whyupdate(self):
        val1 = int(len(self.editline1.GetText()))
        yazi1 = str(self.editline1.GetText())

        if "" == yazi1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac powód konserwacji!")
            return

        if val1 < 5:
            chat.AppendChat(5, "Powód konserwacji jest zbyt krótki!")
            return

        if val1 > 30:
            chat.AppendChat(5, "Powód konserwacji jest zbyt dlugi!")
            return

        if yazi1.lower().find('%') != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (%) w powodzie konserwacji!")
            return

        if yazi1.lower().find("'") != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (') w powodzie konserwacji!")
            return

		net.SendChatPacket("/maintenance " + str(yazi2) + " " + str(yazi3))
		net.SendChatPacket("/maintenance_text enable " + str(yazi1))
		self.Close()

    def Cancel(self):
        net.SendChatPacket("/maintenance disable")
        chat.AppendChat(chat.CHAT_TYPE_INFO, "Konserwacja zostala pomyslnie anulowana!")
        self.Close()

    def Whycancel(self):
        net.SendChatPacket("/maintenance_text disable")
        self.GetClientSet()
        self.Close()

    def Whyupdate(self):
        val1 = int(len(self.editline1.GetText()))
        yazi1 = str(self.editline1.GetText())

        if "" == yazi1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac powód konserwacji!")
            return

        if val1 < 5:
            chat.AppendChat(5, "Powód konserwacji jest zbyt krótki!")
            return

        if val1 > 30:
            chat.AppendChat(5, "Powód konserwacji jest zbyt dlugi!")
            return

        if yazi1.lower().find('%') != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (%) w powodzie konserwacji!")
            return

        if yazi1.lower().find("'") != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (') w powodzie konserwacji!")
            return

		net.SendChatPacket("/maintenance_text enable " + str(yazi1))
		self.GetClientSet()
		self.Close()

    def Maintenanceupdate(self):
        val1 = int(len(self.editline1.GetText()))
        yazi1 = str(self.editline1.GetText())
        val2 = int(len(self.editline2.GetText()))
        yazi2 = str(self.editline2.GetText())
        val3 = int(len(self.editline3.GetText()))
        yazi3 = str(self.editline3.GetText())

        if "" == yazi1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac powód konserwacji!")
            return

        if val1 < 5:
            chat.AppendChat(5, "Powód konserwacji jest zbyt krótki!")
            return

        if val1 > 30:
            chat.AppendChat(5, "Powód konserwacji jest zbyt dlugi!")
            return

        if yazi1.lower().find('%') != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (%) w powodzie konserwacji!")
            return

        if yazi1.lower().find("'") != -1:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Nie mozesz uzywac znaku (') w powodzie konserwacji!")
            return

        if "" == yazi2:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac pozostaly czas do konserwacji!")
            return

        if val2 < 3:
            chat.AppendChat(5, "Pozostaly czas do konserwacji jest zbyt krótki!")
            return

        if val2 > 5:
            chat.AppendChat(5, "Pozostaly czas do konserwacji jest zbyt dlugi!")
            return

        if "" == yazi3:
            chat.AppendChat(chat.CHAT_TYPE_INFO, "Zapomniales podac czasu trwania konserwacji!")
            return

        if val3 < 3:
            chat.AppendChat(5, "Czas trwania konserwacji jest zbyt krótki!")
            return

        if val3 > 5:
            chat.AppendChat(5, "Czas trwania konserwacji jest zbyt dlugi!")
            return

		net.SendChatPacket("/maintenance disable")
		net.SendChatPacket("/maintenance " + str(yazi2) + " " + str(yazi3))
		net.SendChatPacket("/maintenance_text enable " + str(yazi1))
		self.Close()

	def GetClientSet(self):
		net.SendChatPacket("/maintenance_update")
		self.Close()

	def Close(self):
		self.Hide()
		return TRUE

	def OnPressEscapeKey(self):
		self.Close()
		return TRUE
