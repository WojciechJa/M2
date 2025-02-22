#Add
import uimaintenanceadmin

#Search:

	def __MakeGuildWindow(self):
		self.wndGuild = uiGuild.GuildWindow()

#Add below:

	def MaintenanceAdmin(self):
		self.dlgMaintenanceAdmin.OpenDialog()
        
#Search:

		self.dlgShop.Hide()

#Add below:

		self.dlgMaintenanceAdmin = uimaintenanceadmin.MaintenanceAdminWindow()
		self.dlgMaintenanceAdmin.LoadDialog()
		self.dlgMaintenanceAdmin.Hide()
#Option self.dlgRestart = uiRestart.RestartDialog()

#Search:

		if self.dlgShop:
			self.dlgShop.Destroy()

#Add below:

		if self.dlgMaintenanceAdmin:
			self.dlgMaintenanceAdmin.Destroy()
            
#Search:

		del self.dlgShop

#Add below:

		del self.dlgMaintenanceAdmin
        
#Search:

	def HideAllWindows(self):
		if self.wndTaskBar:
			self.wndTaskBar.Hide()
		
		if self.wndEnergyBar:
			self.wndEnergyBar.Hide()

		if app.ENABLE_DETAILS_UI:
			if self.wndCharacter:
				self.wndCharacter.Close()
		else:
			if self.wndCharacter:
				self.wndCharacter.Hide()

		if self.wndInventory:
			self.wndInventory.Hide()
			
		if app.ENABLE_DRAGON_SOUL_SYSTEM:
			self.wndDragonSoul.Hide()
			self.wndDragonSoulRefine.Hide()

		if self.wndChat:
			self.wndChat.Hide()

#Add below:

		if self.dlgMaintenanceAdmin:
			self.dlgMaintenanceAdmin.Hide()
            
#Search:

	def __HideWindows(self):
		hideWindows = self.wndTaskBar,\
						self.wndCharacter,\
						self.wndInventory,\
						self.wndMiniMap,\
						self.wndGuild,\
						self.wndMessenger,\
						self.wndChat,\
						self.wndParty,\
						self.wndGameButton,

		if self.wndEnergyBar:
			hideWindows += self.wndEnergyBar,

		if self.wndExpandedTaskBar:
			hideWindows += self.wndExpandedTaskBar,

#Add below:

		if self.dlgMaintenanceAdmin:
			hideWindows += self.dlgMaintenanceAdmin,