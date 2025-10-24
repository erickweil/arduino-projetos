import sys
from sys import version_info
if version_info.major == 2:
	# We are using Python 2.x
	import Tkinter as tk
	import ttk
elif version_info.major == 3:
	# We are using Python 3.x
	from tkinter import ttk
	import tkinter as tk
import kcirepython.config as CONF
import traceback
def print_last_error(msg):
	err = str(traceback.format_exception(*sys.exc_info()))
	if err != None and "\\n" in err:
		err = err.replace("\\n","\n")
	print(err)
	#sys.exc_clear()
	show_warning(msg+"\n\ndetalhes:"+err)	

def show_warning(message,img = CONF.IMG["sign_warning"]):
	app = WarningDialog(message,img)
	app.run()
	
def show_kcire():
	app = WarningDialog(
		"--------------------------------------------------------------------------------\n"+
		"Programa Desenvolvido Por:\n"+
		"--------------------------------------------------------------------------------\n"+
		"--------------------------------------------------------------------------------\n"+
		"-------------------------- Erick Leonardo Weil ---------------------------------\n"+
		"--------------------------------------------------------------------------------\n"+
		"--------------------------------------------------------------------------------\n"+
		"\n\n\n\n Isto é parte de um trabalho do IFRO. colocar mais informações\n"+
		"--------------------------------------------------------------------------------\n"
		,CONF.IMG["kcire"])
	app.run()

class WarningDialog(tk.Frame):
	''' An example application for TkInter.  Instantiate
		and call the run method to run. '''
	def __init__(self, message, img ):
		# Initialize window using the parent's constructor
		self.master = tk.Tk()
		tk.Frame.__init__(self,
						  self.master,
						  width=450,
						  height=200)
		# Set the title
		self.master.title('Alerta')
		
		# This allows the size specification to take effect
		#self.pack_propagate(0)
		
		# We'll use the flexible pack layout manager
		self.pack()
		
		try:
			self.img_warning = tk.PhotoImage(master = self.master,file=img)
			self.warning_labelimg = tk.Label(self,image=self.img_warning)
			self.warning_labelimg.pack(side=tk.TOP)
		except Exception as e: 
			print(e)
		
		self.main_frame = tk.Frame(self)
		
		if message != None and "\\n" in message:
			message = message.replace("\\n","\n")
		
		self.msglabel = tk.Label(self.main_frame,wraplength=800,justify=tk.LEFT,text=message)
		self.msglabel.pack(side=tk.TOP)
		
		self.okbutton = ttk.Button(self.main_frame,text="OK",command = self.close_dialog)
		self.okbutton.pack(side=tk.TOP)
		
		self.main_frame.pack(side=tk.TOP)
		
		try:
			self.master.iconbitmap(CONF.IMG["icone"])
		except Exception as e: 
			print(e)
			
		#self.master.protocol("WM_DELETE_WINDOW", self.close_dialog)
	def run(self):
		pass
		#self.mainloop()
	def close_dialog(self):
		self.master.destroy()
		#self.master.destroy()
		