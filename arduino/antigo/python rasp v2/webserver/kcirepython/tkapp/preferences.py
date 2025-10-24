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

import json
import kcirepython.config as CONF

class PreferencesManager():
	def __init__(self):
		self.prefs = {}
		self.pref_order = 0
		
		self.pref_aba = "Sistema"
		self.add_pref("INICIAR_COM_SISTEMA","Iniciar com Sistema",False,"CHECKBOX")
		
		self.pref_aba = "Conexão Serial"
		
		self.add_pref("CONECTAR_AUTO","Conectar Automatimente",True,"CHECKBOX")
		
		# Prevenir que o arduino resete ao conectar na porta serial
		# seguindo essa resposta 
		# http://stackoverflow.com/questions/15460865/disable-dtr-in-pyserial-from-code
		self.add_pref("PREVENT_ARDUINO_RESET","Desativar Auto-Reset",False,"CHECKBOX")
		# Retirado da documentacao do pyserial:
		#Possible values for the parameter timeout which controls the behavior of read():

		#timeout = None: wait forever / until requested number of bytes are received
		#timeout = 0: non-blocking mode, return immediately in any case, returning zero or more, up to the requested number of bytes
		#timeout = x: set timeout to x seconds (float allowed) returns immediately when the requested number of bytes are available,
		#	otherwise wait until the timeout expires and return all bytes that were received until then.
		# um read timeout alto por causa que o arduino pode demorar um pouco para mandar o OK inicial
		self.add_pref("READ_TIMEOUT","Read Timeout",8.0,"NUM_ENTRY")
		
		
		self.load_preferences()
	def __getattr__(self, name):
		return self.prefs[name].value
	def add_pref(self,name,label,value,widget):
		self.prefs[name] = Pref(label,value,self.pref_aba,self.pref_order,widget)
		self.pref_order += 1
	def get_abas(self):
		items = self.prefs.items()
		items = sorted(items, key=lambda k: k[1].order)
		abas = []
		for i in items:
			if i[1].aba not in abas:
				abas.append(i[1].aba)
		return abas
	def get_items(self,aba):
		items = self.prefs.items()
		items = sorted(items, key=lambda k: k[1].order)
		items = [i for i in items if i[1].aba == aba]
		return items
	def load_preferences(self):
		try:
			f = open('preferences.txt', 'r')
		except IOError:
			print("Não pôde carregar as preferencias")
			return
		
		with f:
			s = f.read()
			savedprefs = json.loads(s)
			keys = self.prefs.keys()
			for k in keys:
				self.prefs[k].value = savedprefs[k]
	def save_preferences(self):
		try:
			f = open('preferences.txt', 'w')
		except IOError:
			print("Não pôde escrever as preferencias")
			return
		
		with f:
			savedprefs = {}
			keys = self.prefs.keys()
			for k in keys:
				savedprefs[k] = self.prefs[k].value
			json.dump(savedprefs, f)
class Pref():
	def __init__(self,label,value,aba,order,widget):
		self.label = label
		self.value = value
		self.aba = aba
		self.widget = widget
		self.order = order	

PREF = PreferencesManager()
preferences_app = None

def show_preferences(app):
	global preferences_app
	if preferences_app == None:
		preferences_app = PreferencesWindow(tk.Toplevel(app))
	else:
		preferences_app.focus_force()
		
class PreferencesWindow(tk.Frame):

	def __init__(self, master):
		global PREF
		# Initialize window using the parent's constructor
		tk.Frame.__init__(self,
						  master,
						  width=550,
						  height=410)
		# Set the title
		self.master.title('Preferências')
		# This allows the size specification to take effect
		self.pack_propagate(0)
		
		# We'll use the flexible pack layout manager
		self.pack()
		
		self.topo = ttk.Notebook(self)
		self.button_salvar = ttk.Button(self,text="Salvar",command=self.salvar_prefs)
		padd = 5
		self.button_salvar.pack(side=tk.BOTTOM,anchor=tk.SE,padx=padd,pady=padd)

		abas = PREF.get_abas()
		self.prefs_var = {}
		#adciona as abas
		for aba in abas:
			frame = tk.Frame(self.topo)
			self.topo.add(frame, text=aba)
			items = PREF.get_items(aba)
			# adiciona os items
			for item in items:
				pref_name = item[0]
				pref_item = item[1]
				pref_label = pref_item.label
				pref_value = pref_item.value
				pref_type = pref_item.widget
				if pref_type == "CHECKBOX":
					self.prefs_var[pref_name] = tk.IntVar()
					
					ttk.Checkbutton(frame,text=pref_label, variable=self.prefs_var[pref_name])\
					.pack(side=tk.TOP,anchor=tk.NW,padx=padd,pady=padd)
				elif pref_type == "NUM_ENTRY":
					self.prefs_var[pref_name] = tk.StringVar()
					
					t = tk.Frame(frame)
					ttk.Label(t,text=pref_label).pack(side=tk.LEFT)
					ttk.Entry(t,textvariable=self.prefs_var[pref_name]).pack(side=tk.LEFT)
					t.pack(side=tk.TOP,anchor=tk.NW,padx=padd,pady=padd)
				self.prefs_var[pref_name].set(pref_value)
		
		self.topo.pack(fill=tk.BOTH,expand=1,side=tk.TOP)
		
		self.pack(side=tk.TOP,fill=tk.BOTH, expand=1)
		self.master.protocol("WM_DELETE_WINDOW", self.on_quit)
		
		try:
			self.master.iconbitmap(CONF.IMG["icone"])
		except Exception as e: 
			print(e)
	def salvar_prefs(self):
		items = self.prefs_var.items()
		for i in items:
			pref_key = i[0]
			pref_var = i[1]
			PREF.prefs[pref_key].value = pref_var.get()
		PREF.save_preferences()
	def on_quit(self):
		global preferences_app
		preferences_app.master.destroy()
		preferences_app = None