#Search:

if app.ENABLE_TARGET_INFORMATION_SYSTEM:
	MONSTER_INFO_DATA = {}

#Add above:

def GetFlag(flagname):
	global py_Flag
	return py_Flag.get(flagname, None)
def SetFlag(flagname,value):
	global py_Flag
	py_Flag[flagname] = value
MAINTENANCEADMIN_OPEN = 0