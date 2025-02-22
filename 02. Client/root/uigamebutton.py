#Add

import chr

#Search:

				"EXIT_OBSERVER" : self.GetChild("ExitObserver"),

#Add below:

				"ADMPANEL" : self.GetChild("AdmPanel"),
                
#Search:

			self.gameButtonDict["EXIT_OBSERVER"].SetEvent(ui.__mem_func__(self.__OnClickExitObserver))

#Add below:

			self.gameButtonDict["ADMPANEL"].SetEvent(ui.__mem_func__(self.MaintenanceAdmin))
            
#Search:

	def __OnClickExitObserver(self):
		net.SendChatPacket("/observer_exit")

#Add below:

	def MaintenanceAdmin(self):
		import constInfo
		constInfo.MAINTENANCEADMIN_OPEN = 1
        
#Search:

	def CheckGameButton(self):

		if not self.IsShow():
			return

		statusPlusButton=self.gameButtonDict["STATUS"]
		skillPlusButton=self.gameButtonDict["SKILL"]
		helpButton=self.gameButtonDict["HELP"]

#Add below:

		admpanel1=self.gameButtonDict["ADMPANEL"]
        
#Search:

		if self.__IsSkillStat():
			skillPlusButton.Show()
		else:
			skillPlusButton.Hide()

#Add below:

		if chr.IsGameMaster(player.GetMainCharacterIndex()):
			admpanel1.Show()
			ronark.Show()
			pm.Show()
		else:
			admpanel1.Hide()
			ronark.Hide()
			pm.Hide()
        
        