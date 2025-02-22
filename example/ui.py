#Change ImageBox class(from 2018 official root)
class ImageBox(Window):
	def __init__(self, layer = "UI"):
		Window.__init__(self, layer)

		self.eventDict={}
		self.eventFunc = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}
		self.eventArgs = {"mouse_click" : None, "mouse_over_in" : None, "mouse_over_out" : None}

	def __del__(self):
		Window.__del__(self)
		self.eventFunc = None
		self.eventArgs = None

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterImageBox(self, layer)

	def LoadImage(self, imageName):
		self.name=imageName
		wndMgr.LoadImage(self.hWnd, imageName)

		if len(self.eventDict)!=0:
			print "LOAD IMAGE", self, self.eventDict

	def SetAlpha(self, alpha):
		wndMgr.SetDiffuseColor(self.hWnd, 1.0, 1.0, 1.0, alpha)

	def GetWidth(self):
		return wndMgr.GetWidth(self.hWnd)

	def GetHeight(self):
		return wndMgr.GetHeight(self.hWnd)

	def OnMouseOverIn(self):
		try:
			self.eventDict["MOUSE_OVER_IN"]()
		except KeyError:
			pass

	def OnMouseOverOut(self):
		try:
			self.eventDict["MOUSE_OVER_OUT"]()
		except KeyError:
			pass

	def SAFE_SetStringEvent(self, event, func):
		self.eventDict[event]=__mem_func__(func)

	def SetEvent(self, func, *args) :
		result = self.eventFunc.has_key(args[0])		
		if result :
			self.eventFunc[args[0]] = func
			self.eventArgs[args[0]] = args
		else :
			print "[ERROR] ui.py SetEvent, Can`t Find has_key : %s" % args[0]

	def OnMouseLeftButtonUp(self) :
		if self.eventFunc["mouse_click"] :
			apply(self.eventFunc["mouse_click"], self.eventArgs["mouse_click"])

	def OnMouseOverIn(self) :
		if self.eventFunc["mouse_over_in"] :
			apply(self.eventFunc["mouse_over_in"], self.eventArgs["mouse_over_in"])
		else:
			try:
				self.eventDict["MOUSE_OVER_IN"]()
			except KeyError:
				pass

	def OnMouseOverOut(self) :
		if self.eventFunc["mouse_over_out"] :
			apply(self.eventFunc["mouse_over_out"], self.eventArgs["mouse_over_out"])
		else :
			try:
				self.eventDict["MOUSE_OVER_OUT"]()
			except KeyError:
				pass

#Change ToggleButton class(from 2018 official root)
class ToggleButton(Button):
	def __init__(self):
		Button.__init__(self)

		self.eventUp = None
		self.eventDown = None

		self.eventUpArgs = None
		self.eventDownArgs = None

	def __del__(self):
		Button.__del__(self)

		self.eventUp = None
		self.eventDown = None

	def SetToggleUpEvent(self, event, *args):
		self.eventUp = event
		self.eventUpArgs = args

	def SetToggleDownEvent(self, event, *args):
		self.eventDown = event
		self.eventDownArgs = args

	def RegisterWindow(self, layer):
		self.hWnd = wndMgr.RegisterToggleButton(self, layer)

	def OnToggleUp(self):
		if self.eventUp:
			if self.eventUpArgs:
				apply(self.eventUp, self.eventUpArgs)
			else:
				self.eventUp()

	def OnToggleDown(self):
		if self.eventDown:
			if self.eventDownArgs:
				apply(self.eventDown, self.eventDownArgs)
			else:
				self.eventDown()