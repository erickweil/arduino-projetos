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
if CONF.FAKECONNECTION:
	import kcirepython.serial.fakePost as pythonserial
else:
	import kcirepython.serial.pythonserial as pythonserial

last_inputs = []
last_inputs_index = -1
monitor_app = None

def show_monitor(app):
	global monitor_app
	if monitor_app == None:
		monitor_app = MonitorSerial(tk.Toplevel(app))
	else:
		monitor_app.focus_force()
		
def serial_log(line):
	global monitor_app
	try:
		if monitor_app == None:
			pass
		else:
			monitor_app.serial_log(line)
	except Exception as e:
		print("Erro ao fazer o Serial Log:"+str(e))

class MonitorSerial(tk.Frame):

	def __init__(self, master):
		# Initialize window using the parent's constructor
		tk.Frame.__init__(self,
						  master,
						  width=550,
						  height=410)
		# Set the title
		self.master.title('Monitor Serial')
		# This allows the size specification to take effect
		self.pack_propagate(0)
		
		# We'll use the flexible pack layout manager
		self.pack()
		
		topo = tk.Frame(self)
		

		self.var_send_input = tk.StringVar()
		self.send_input = ttk.Entry(topo,textvariable=self.var_send_input)
		self.send_input.pack(side=tk.LEFT,fill=tk.X,padx=4,expand=1)
		self.send_input.bind("<Return>",lambda event: self.enviar())
		self.send_input.bind("<Up>",lambda event: self.select_last(1))
		self.send_input.bind("<Down>",lambda event: self.select_last(-1))
		self.var_send_input.set("")
		
		self.send_button = ttk.Button(topo,text='Enviar',command=self.enviar)
		self.send_button.pack(side=tk.RIGHT,padx=4)
		
		topo.pack(side=tk.TOP,fill=tk.X, expand=1,pady=4)
		
		opcoes = tk.Frame(self)
		
		self.var_rolautomatica_check = tk.IntVar()
		self.rolautomatica_check = ttk.Checkbutton(opcoes,text="Rolagem Automática",variable=self.var_rolautomatica_check,command=self.check_rolauto)
		self.rolautomatica_check.pack(side=tk.LEFT,padx=4)
		self.var_rolautomatica_check.set(1)
		
		self.var_filtrar_check = tk.IntVar()
		self.filtrar_check = ttk.Checkbutton(opcoes,text="Filtrar",variable=self.var_filtrar_check,command=self.check_filtrar)
		self.filtrar_check.pack(side=tk.LEFT,padx=4)
		self.var_filtrar_check.set(0)
		
		opcoes.pack(side=tk.BOTTOM,fill=tk.X, expand=1, pady=4)
		
		console = tk.Frame(self)
		
		#edit_space = tkst.ScrolledText(
		#	master = console,
		#	wrap   = 'no',  # wrap text at full words only
		#	width  = 80,      # characters
		#	bg='white'        # background color of edit area
		#	)
		
		# the padx/pady space will form a frame
		#edit_space.pack(fill=tk.BOTH,side= tk.TOP, expand=1)
		
		self.console_text = tk.Text(console)
		self.console_text.grid(column=0, row=0, sticky=(tk.N,tk.W,tk.E,tk.S))
		
		self.console_text.configure(state="disabled")

		# make sure the widget gets focus when clicked
		# on, to enable highlighting and copying to the
		# clipboard.
		self.console_text.bind("<1>", lambda event: self.console_text.focus_set())
		
		self.console_scroll = ttk.Scrollbar(console, orient=tk.VERTICAL, command=self.console_text.yview)
		self.console_scroll.grid(column=1, row=0, sticky=(tk.N,tk.S))
		self.console_text['yscrollcommand'] = self.console_scroll.set
		#ttk.Sizegrip(console).grid(column=1, row=1, sticky=(tk.S,tk.E))
		console.grid_columnconfigure(0, weight=1)
		console.grid_rowconfigure(0, weight=1)
		
		console.pack(side=tk.TOP,fill=tk.BOTH, expand=1)
		

		
		self.pack(side=tk.TOP,fill=tk.BOTH, expand=1)
		self.master.protocol("WM_DELETE_WINDOW", self.on_quit)
		
		try:
			self.master.iconbitmap(CONF.IMG["icone"])
		except Exception as e: 
			print(e)
		
	def enviar(self):
		global last_inputs_index,last_inputs
		line = self.var_send_input.get()
		self.var_send_input.set("")
		
		if line != None and len(line) > 0 and pythonserial.isOpen():
			if self.var_filtrar_check.get():
				self.console_insert(">"+line+"\n")
			
			last_inputs.append(line)
			last_inputs_index = len(last_inputs)
			
			pythonserial.writeline(line);
			resposta = pythonserial.readline();
			
			if self.var_filtrar_check.get():
				self.console_insert(resposta+"\n")
				
	def select_last(self,offset):
		global last_inputs_index,last_inputs
		if len(last_inputs) > 0:
			last_inputs_index = min(len(last_inputs)-1,max(0,last_inputs_index-offset))
			line = last_inputs[last_inputs_index]
			self.var_send_input.set(line)
			
	def check_rolauto(self):
		pass
	def check_filtrar(self):
		pass
	def serial_log(self,message):
		if not self.var_filtrar_check.get():
			self.console_insert(message)
	def console_insert(self,message):
		self.console_text.config(state=tk.NORMAL)
		self.console_text.insert(tk.END, message)
		self.console_text.config(state=tk.DISABLED)
		if self.var_rolautomatica_check.get():
			self.console_text.see(tk.END)
	def on_quit(self):
		global monitor_app
		monitor_app.master.destroy()
		monitor_app = None