#Add in imports:

import uimaintenance

#Search:

		self.affectShower = uiAffectShower.AffectShower()

#Add below:

		self.wndMaintenance = uimaintenance.MaintenanceClass()

#Search:

		if self.console:
			self.console.BindGameClass(0)
			self.console.Close()
			self.console=None

#Add above:
		if self.wndMaintenance.IsShow():
			self.wndMaintenance.Hide()

#Search
    def OnUpdate(self):
        app.UpdateGame()
    
#Add below:


        if constInfo.MAINTENANCEADMIN_OPEN == 1:
            self.interface.MaintenanceAdmin()
            constInfo.MAINTENANCEADMIN_OPEN = 0
            
#Search

			"mall"			: self.__InGameShop_Show,
    
#Add below:
     
            
            
            "BINARY_Close_Maintenance" : self.BINARY_Close_Maintenance,
			"BINARY_Update_Maintenance" : self.BINARY_Update_Maintenance,
 #Search         
            
    	if app.__BL_MULTI_LANGUAGE_ULTIMATE__:
		def LanguageChangeAnonymous(self):
			if self.interface:
				self.interface.LanguageChangeAnonymous()        
            
#Add below:           
            
    def BINARY_Close_Maintenance(self):
		self.wndMaintenance.Close()

	def BINARY_Update_Maintenance(self, iTime, iDuration, iReason):
		sTime = int(iTime)
		sDuration = int(iDuration)
		sReason = str(iReason)

		if sTime != 0 and sDuration != 0:
			self.wndMaintenance.OpenMaintenance(int(iTime), int(iDuration), str(iReason))