#Find in def __Initialize(self):
		self.showsalesTextButtonList = []

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.interface = None
			self.looting_system_button = None

#Find in def __Load_BindObject(self):
			self.showsalesTextButtonList.append(GetObject("salestext_off_button"))

#Add
			if app.__BL_OFFICIAL_LOOT_FILTER__:
				self.looting_system_button = GetObject("looting_system_button")

#Find in def __Load(self):
		self.__SetPeacePKMode()

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.looting_system_button.SetEvent(ui.__mem_func__(self.__OnClickLootingSystemButton))

#Add new func
	if app.__BL_OFFICIAL_LOOT_FILTER__:
		def BindInterface(self, interface):
			self.interface = interface

		def __OnClickLootingSystemButton(self):
			if self.interface:
				self.interface.OpenLootingSystemWindow()