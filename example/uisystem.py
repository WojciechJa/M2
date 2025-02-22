#Find in def __Initialize(self):
		self.gameOptionDlg = None

#Add
		if app.__BL_OFFICIAL_LOOT_FILTER__:
			self.interface = None

#Find
			self.gameOptionDlg = uiGameOption.OptionDialog()

#Add
			if app.__BL_OFFICIAL_LOOT_FILTER__:
				if self.interface:
					self.gameOptionDlg.BindInterface(self.interface)

#Add new func
	if app.__BL_OFFICIAL_LOOT_FILTER__:
		def BindInterface(self, interface):
			from _weakref import proxy
			self.interface = proxy(interface)