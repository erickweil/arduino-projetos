from sys import version_info
if version_info.major == 2:
	# We are using Python 2.x
	import Tkinter as tk
	import ttk
elif version_info.major == 3:
	# We are using Python 3.x
	from tkinter import ttk
	import tkinter as tk

class ToggledFrame(tk.Frame):
	
	def __init__(self, parent, showvariable=None, *args, **options):
		tk.Frame.__init__(self, parent, *args, **options)
		
		if showvariable == None:
			self.show = tk.IntVar()
		else:
			self.show = showvariable
		self.show.trace("w", self.toggle)
		
		#self.title_frame = ttk.Frame(self)
		#self.title_frame.pack(fill="x", expand=1)
		
		#ttk.Label(self.title_frame, text=text).pack(side="left", fill="x", expand=1)

		#self.toggle_button = ttk.Checkbutton(self.title_frame, width=2, text='+', command=self.toggle,
		#									variable=self.show, style='Toolbutton')
		#self.toggle_button.pack(side="left")
		
		self.sub_frame = tk.Frame(self)
		self.toggle()
	
	def toggle(self,*args):
		if bool(self.show.get()):
			self.sub_frame.pack(fill=tk.X, expand=1)
			#self.toggle_button.configure(text='-')
		else:
			self.sub_frame.forget()
			#self.toggle_button.configure(text='+')
	
class StepScale(ttk.Scale):
	def __init__(self, parent, sliderstep=1, changedcommand=None, releasecommand= None, stepvariable = None, *args, **options):
		ttk.Scale.__init__(self, parent, command=self.controlStep, *args, **options)
		self.sliderstep = sliderstep
		self.changedcommand = changedcommand
		self.releasecommand = releasecommand
		self.is_controlingStep = False
		self.is_onchangevar = False
		self.stepvariable = stepvariable
		self.old_value = self.stepvariable.get()
		self.stepvariable.trace("w", self.onchangevar)
		self.bind("<Button-1>",lambda x:self.controlPress(1))
		self.bind("<Button-3>",lambda x:self.controlPress(3))
		self.bind("<ButtonRelease-1>",lambda x:self.controlRelease(1))
		self.bind("<ButtonRelease-3>",lambda x:self.controlRelease(3))
		self.value_onpress = self.old_value
		
	def controlPress(self,button):
		self.value_onpress = self.old_value
		
	def controlRelease(self,button):
		if self.old_value != self.value_onpress and self.releasecommand != None:
			self.releasecommand(self.old_value)
		
	def onchangevar(self,*args):
		#print("onchangevar")
		# para prevenir um stackoverflow
		# uma escrita na variavel chama a funcao que escreve na variavel
		# que escreve na variavel etc...
		if not self.is_onchangevar and not self.is_controlingStep:
			self.is_onchangevar = True
			scaleValue = self.stepvariable.get()
			newvalue = int(round(scaleValue/self.sliderstep))
			# segundo jeito louco de prevenir stackoverflow
			if self.get() != newvalue:
				self.set(newvalue)
				if newvalue != self.old_value:
					if self.changedcommand != None:
						self.changedcommand(newvalue)
			self.old_value = newvalue
			self.is_onchangevar = False
		
	def controlStep(self,*args):
		#print("controlStep")
		# para prevenir um stackoverflow
		# uma escrita na variavel chama a funcao que escreve na variavel
		# que escreve na variavel etc...
		if not self.is_onchangevar and not self.is_controlingStep:
			self.is_controlingStep = True
			scaleValue = float(args[0])
			newvalue = int(round(scaleValue/self.sliderstep))
			# segundo jeito louco de prevenir stackoverflow
			if scaleValue != newvalue:
				self.stepvariable.set(newvalue)
				self.set(newvalue)
				if newvalue != self.old_value:
					if self.changedcommand != None:
						self.changedcommand(newvalue)
			self.old_value = newvalue
			self.is_controlingStep = False