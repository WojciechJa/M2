#Add to end
	if app.__BL_OFFICIAL_LOOT_FILTER__:
		def LootingSystemProcess(self):
			if self.interface:
				self.interface.LootingSystemProcess()