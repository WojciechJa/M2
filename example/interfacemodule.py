#Add
if app.__BL_OFFICIAL_LOOT_FILTER__:
	import uilootingsystem

#Find
		self.wndGuild = None

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.wndLootFilter = None

#Find
		self.wndChatLog = wndChatLog

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.wndLootFilter = uilootingsystem.LootingSystem()

#Find
		self.dlgSystem.SetOpenHelpWindowEvent(ui.__mem_func__(self.OpenHelpWindow))

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.dlgSystem.BindInterface(self)

#Find
		del self.wndItemSelect

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			if self.wndLootFilter:
				del self.wndLootFilter

#Find
	def ChangePartyParameter(self, distributionMode):
		self.wndParty.ChangePartyParameter(distributionMode)

#Add
	if app.__BL_OFFICIAL_LOOT_FILTER__:
		def OpenLootingSystemWindow(self):
			if self.wndLootFilter:
				self.wndLootFilter.Open()

		def LootingSystemProcess(self):
			if self.wndLootFilter:
				self.wndLootFilter.LootingSystemProcess()