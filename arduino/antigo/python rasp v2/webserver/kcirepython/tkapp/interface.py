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
import traceback
from kcirepython.tkapp.customwidgets import *
from kcirepython.tkapp.manager import *
from kcirepython.tkapp.dialog import *
from kcirepython.tkapp.monitor import *
from kcirepython.tkapp.preferences import PREF,show_preferences
import kcirepython.config as CONF
import kcirepython.serial.managePost as managePost
if CONF.FAKECONNECTION:
	import kcirepython.serial.fakePost as pythonserial
	from kcirepython.serial.fakePost import sendArduino
else:
	import kcirepython.serial.pythonserial as pythonserial
	from kcirepython.serial.sendComand import sendArduino
import serial
import time

def stepScale(widget,step,onchange):
	scaleValue = widget.get()
	newvalue = int(round(scaleValue/step))
	#print("scale:"+str(scaleValue)+" -> "+str(newvalue))
	# isso e pra evitar um stackoverflow
	# talvez deva fazer algo mais 'seguro'
	if scaleValue != newvalue:
		widget.set(newvalue)
		onchange(newvalue)

def startApp():
	try:
		app = ExampleApp(tk.Tk())
	except Exception as e:
		print(e)
		print_last_error("Erro Fatal ao Iniciar a Interface:"+str(e))
	try:
		app.run()
	except Exception as e:
		print(e)
		print_last_error("Erro Fatal Durante a Execucao:"+str(e))

class ExampleApp(tk.Frame):

	def dummy(self):
		print("lol")
		serial_log("LOL")
	''' An example application for TkInter.  Instantiate
		and call the run method to run. '''
	def __init__(self, master):
		# Initialize window using the parent's constructor
		tk.Frame.__init__(self,
						  master,
						  width=600,
						  height=480)
						  
		#pega as excecoes
		self.oldreport_callback_exception = master.report_callback_exception
		master.report_callback_exception = self.show_error
		# Set the title
		self.master.title('Controle Posicionador de Amostra - by Erick Leonardo Weil')
		# This allows the size specification to take effect
		self.pack_propagate(0)
		
		# We'll use the flexible pack layout manager
		self.pack()
		
		self.manager = InterfaceManager(self)
		
		self.conectou_arduino = CONF.FAKECONNECTION

		# tenta iniciar conexao com arduino
		pythonserial.serial_log = serial_log
		pythonserial.connect()
		self.portconnected = False
		
		# create a menu
		#Without it, each of your menus (on Windows and X11) will start with what looks like a dashed line, and allows you to "tear off" the menu 
		#so it appears in its own window. You really don't want that there.
		self.master.option_add('*tearOff', tk.FALSE)
		
		menubar = tk.Menu(self.master)
		
		menu_file = tk.Menu(menubar)
		menu_file.add_command(label='Fechar', command=self.master.quit)
		menubar.add_cascade(menu=menu_file, label='Arquivo')
		
		menu_opcoes = tk.Menu(menubar)
		menu_opcoes.add_command(label='Preferências', command=lambda: show_preferences(self))
		menubar.add_cascade(menu=menu_opcoes, label='Opções')
		
		menu_arduino = tk.Menu(menubar)
		menu_arduino.add_command(label='Monitor Serial', command=lambda: show_monitor(self))
		menu_arduino.add_command(label='Enviar Ping', command=lambda: self.trySendArduino(CONF.COD_PING,[]))
		menu_arduino.add_command(label='Abrir IDE Arduino')
		menubar.add_cascade(menu=menu_arduino, label='Arduino')
		
		menu_ajuda = tk.Menu(menubar)
		menu_ajuda.add_command(label='Sobre...', command=show_kcire)
		menubar.add_cascade(menu=menu_ajuda, label='Ajuda')
		
		self.master.config(menu=menubar)
		
		# depois tem q tirar esse menu besta
		menu = tk.Menu(self.master)
		for i in ('One', 'Two', 'Three'):
			menu.add_command(label=i)
		if (self.master.tk.call('tk', 'windowingsystem')=='aqua'):
			self.master.bind('<2>', lambda e: menu.post(e.x_root, e.y_root))
			self.master.bind('<Control-1>', lambda e: menu.post(e.x_root, e.y_root))
		else:
			self.master.bind('<3>', lambda e: menu.post(e.x_root, e.y_root))
		
		#menu = tk.Menu(self.master)
		#self.master.config(menu=menu)
		#filemenu = tk.Menu(menu)
		#menu.add_cascade(label="File", menu=filemenu)
		#filemenu.add_command(label="New", command=self.dummy)
		#filemenu.add_command(label="Open...", command=self.dummy)
		#filemenu.add_command(label="Save", command=self.dummy)
		#filemenu.add_separator()
		#filemenu.add_command(label="Exit", command=self.dummy)
		#helpmenu = tk.Menu(menu)
		#menu.add_cascade(label="Help", menu=helpmenu)
		#helpmenu.add_command(label="About...", command=self.dummy)
		# end of menu creation
		
		
		# controle conexao arduino
		# consiste de um texto dizendo estado da conexao e porta conectada
		# bem como a opcao de conectar manualmente por selecionar uma porta do combobox
		
		self.frame_conexao = tk.Frame(self,padx=10,pady=10)
		
		t = tk.Frame(self.frame_conexao)
		
		try:
			self.img_desconectado = tk.PhotoImage(master = self.master,file=CONF.IMG["sign_warning"])
			self.img_conectado = tk.PhotoImage(master = self.master,file=CONF.IMG["ok"])
			self.conexaoestado_labelimg = tk.Label(t,image=self.img_desconectado)
			self.conexaoestado_labelimg.pack(side=tk.LEFT)
		except Exception as e: 
			print(e)
		
		ttk.Label(t,text="Conexão Arduino:").pack(side=tk.LEFT)
		
		self.var_conexaoestado_label = tk.StringVar()
		self.conexaoestado_label = ttk.Label(t,textvariable=self.var_conexaoestado_label)
		self.conexaoestado_label.pack(side=tk.LEFT)
		self.var_conexaoestado_label.set(pythonserial.conection_state)
		
		#t.pack(side=tk.TOP,anchor=tk.W)
		#t = tk.Frame(self.frame_conexao)
		ttk.Label(t,text="Porta:").pack(side=tk.LEFT)
		
		self.var_conexaoporta_label = tk.StringVar()
		self.conexaoporta_label = ttk.Label(t,textvariable=self.var_conexaoporta_label)
		self.conexaoporta_label.pack(side=tk.LEFT)
		self.var_conexaoporta_label.set(pythonserial.port_conected)
		
		self.var_showcombo_check = tk.IntVar()
		self.var_showcombo_check.set(0)
		t.pack(side=tk.TOP,anchor=tk.W)
		t = ToggledFrame(self.frame_conexao, showvariable=self.var_showcombo_check)
		
		self.var_conautomatica_check = tk.IntVar()
		self.conautomatica_check = ttk.Checkbutton(t,text="Conectar Automaticamente",variable=self.var_conautomatica_check,command=self.check_conectarauto)
		self.conautomatica_check.pack(side=tk.LEFT)
		self.var_conautomatica_check.set(PREF.CONECTAR_AUTO)
		
		self.var_selecionarPorta_combo = tk.StringVar()
		self.selecionarPorta_combo = ttk.Combobox(t.sub_frame,postcommand=self.populate_ports,textvariable=self.var_selecionarPorta_combo)
		self.selecionarPorta_combo.pack(side=tk.LEFT)
		if pythonserial.port_conected != None:
			self.var_selecionarPorta_combo.set(pythonserial.port_conected)
		else:
			allports = pythonserial.serial_ports(True)
			if allports != None and len(allports) > 0:
				self.var_selecionarPorta_combo.set(allports[0])
			else:
				self.var_selecionarPorta_combo.set("Selecione uma Porta")
		
		self.conexao_button = ttk.Button(t.sub_frame,text='Conectar',command=self.conexaobutton)
		self.conexao_button.pack(side=tk.LEFT)
		
		t.pack(side=tk.TOP,anchor=tk.W)
		self.frame_conexao.pack(fill=tk.X,side=tk.TOP)
		# abas topo
		# aqui as janelas da aplicacao sao separadas por tres abas
		# o posicionador, e onde sobe e desce a prateleira
		# o girador, onde controla o giro do prato da amostra
		# calibracao, para calibrar o posicionador
		
		self.topo = ttk.Notebook(self)
		#width=600,height=460
		self.frame_posicionador = tk.Frame(self.topo)   # first page, which would get widgets gridded into it
		self.frame_girador = tk.Frame(self.topo)   # second page
		self.frame_calibracao = tk.Frame(self.topo)
		
		self.topo.add(self.frame_posicionador, text='Posicionador')
		self.topo.add(self.frame_girador, text='Girador')
		self.topo.add(self.frame_calibracao, text='Calibração')
		
		
		#Janela Posicionador


		# Slider
		
		self.slider_frame = tk.Frame(self.frame_posicionador,padx=10,pady=10)
		
		self.var_target_slider = tk.IntVar()
		self.target_slider = StepScale(self.slider_frame,releasecommand=self.manager.update_posicao_alvo,stepvariable=self.var_target_slider,length=250, from_=3000, to=0, orient=tk.VERTICAL)
		#self.target_slider = ttk.Scale(self.slider_frame,variable=self.var_target_slider,length=250, from_=3000, to=0, orient=tk.VERTICAL)
		#						command=lambda x: self.manager.update_posicao_alvo(int(float(x))))
		#self.target_slider.state(["disabled"])
		self.target_slider.pack(side=tk.LEFT,padx=10)
		
		self.var_current_slider = tk.IntVar()
		self.current_slider = ttk.Scale(self.slider_frame,variable=self.var_current_slider,length=250, from_=3000, to=0, orient=tk.VERTICAL)
		self.current_slider.state(["disabled"])
		self.current_slider.pack(side=tk.LEFT,padx=10)
		
		# botoes de incremento
		self.sliderbutton_frame = tk.Frame(self.slider_frame)
		
		add_button = ttk.Button(self.sliderbutton_frame,text='++',command=lambda: self.manager.add_posicao(400))
		add_button.pack(side=tk.TOP,pady=5)
		
		add_button = ttk.Button(self.sliderbutton_frame,text='+',command=lambda: self.manager.add_posicao(100))
		add_button.pack(side=tk.TOP,pady=5)
		
		add_button = ttk.Button(self.sliderbutton_frame,text='P',command=self.manager.stop)
		add_button.pack(side=tk.TOP,pady=5)
		
		add_button = ttk.Button(self.sliderbutton_frame,text='-',command=lambda: self.manager.add_posicao(-100))
		add_button.pack(side=tk.TOP,pady=5)
		
		add_button = ttk.Button(self.sliderbutton_frame,text='--',command=lambda: self.manager.add_posicao(-400))
		add_button.pack(side=tk.TOP,pady=5)
		
		self.sliderbutton_frame.pack(side=tk.LEFT)
		
		# texto posicao atual e alvo
		self.slidertext_frame = tk.Frame(self.slider_frame)
		
		self.var_mmcurrentpos_label = tk.StringVar()
		self.mmcurrentpos_label = ttk.Label(self.slidertext_frame,textvariable=self.var_mmcurrentpos_label)
		self.mmcurrentpos_label.pack(side=tk.TOP)
		self.var_mmcurrentpos_label.set('posição atual: 0mm')
		
		self.var_mmtargetpos_label = tk.StringVar()
		self.mmtargetpos_label = ttk.Label(self.slidertext_frame,textvariable=self.var_mmtargetpos_label)
		self.mmtargetpos_label.pack(side=tk.TOP)
		self.var_mmtargetpos_label.set('posição alvo: 0mm')
		
		self.slidertext_frame.pack(side=tk.LEFT)
		
		
		self.slider_frame.pack(fill=tk.X)
		
		self.input_frame = tk.Frame(self.frame_posicionador)
		
		# The recipient text entry control and its StringVar
		self.var_pos_input = tk.StringVar()
		self.pos_input = ttk.Entry(self.input_frame,textvariable=self.var_pos_input)
		self.pos_input.bind("<Return>",lambda event: self.manager.setar_posicao())
		self.var_pos_input.set('0')
		self.pos_input.pack(side=tk.LEFT)
		
		# The go button
		self.pos_inputbutton = ttk.Button(self.input_frame,text='Setar Posição',command=self.manager.setar_posicao)
		self.pos_inputbutton.pack(side=tk.LEFT)
		
		self.input_frame.pack(anchor=tk.W)
		
		#Tela Girador Amostra
		
		# Slider
		self.giradorslider_frame = tk.Frame(self.frame_girador,padx=10,pady=10)
		
		
		# botoes de incremento
		t = tk.Frame(self.giradorslider_frame, width=300,height=70)
		t.grid_propagate(0)
		add_button = ttk.Button(t,width=4,text='P',command=self.manager.parargiro)
		add_button.grid(row=0,column=0)
		
		add_button = ttk.Button(t,width=4,text='+',command=lambda: self.manager.set_girovel(1))
		add_button.grid(row=0,column=1)
		
		add_button = ttk.Button(t,width=4,text='++',command=lambda: self.manager.set_girovel(2))
		add_button.grid(row=0,column=2)
		
		add_button = ttk.Button(t,width=4,text='+++',command=lambda: self.manager.set_girovel(3))
		add_button.grid(row=0,column=3)
		
		self.var_giradorvel_slider = tk.IntVar()
		self.giradorvel_slider = StepScale(t,
										length=280, from_=0, to=3,orient=tk.HORIZONTAL,
										stepvariable=self.var_giradorvel_slider,releasecommand=self.manager.set_girovel)
		#self.giradorvel_slider = ttk.Scale(self.giradorslider_frame,variable=self.var_giradorvel_slider,length=250, from_=-3, to=3,orient=tk.HORIZONTAL,
		#					command=lambda x: stepScale(self.giradorvel_slider,1,lambda x: self.manager.update_girovel(x)))
		#self.giradorvel_slider.state(["disabled"])
		self.giradorvel_slider.grid(row=1,column=0,columnspan=4)
		self.var_giradorvel_slider.set(0)
		
		t.pack(side=tk.TOP,anchor=tk.W)
		t = tk.Frame(self.giradorslider_frame )
		
		ttk.Label(t,text="Sentido do giro:",width=20).pack(side=tk.LEFT)
		
		self.var_giradorsentido_combo = tk.StringVar()
		self.var_giradorsentido_combo.trace('w',self.manager.girodir_combo)
		self.giradorsentido_combo = ttk.Combobox(t,state='readonly',values=["Horário","Anti-Horário"],textvariable=self.var_giradorsentido_combo)
		self.giradorsentido_combo.pack(side=tk.LEFT)
		self.var_giradorsentido_combo.set("Horário")
		
		t.pack(side=tk.TOP,anchor=tk.W,pady=10)
		
		self.var_girovel_label = tk.StringVar()
		self.girovel_label = ttk.Label(self.giradorslider_frame,textvariable=self.var_girovel_label)
		self.girovel_label.pack(side=tk.TOP,anchor=tk.W,pady=10)
		self.var_girovel_label.set('Velocidade atual: Parado')
		
		self.giradorslider_frame.pack(fill=tk.X,side=tk.TOP)
		
		#Tela Calibracao
		
		group_width = 250
		group_height = 50
		
		group = ttk.Labelframe(self.frame_calibracao, text="Calibrar Rosca")
		
		#ttk.Label(group,wraplength=250,
		#	text="Altura da rosca: é a altura que o fuso percorreu ao girar pela rosca \n Voltas do Fuso: o número de voltas que o fuso deu"
		#	).pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		ttk.Label(group,wraplength=250,
		text="Altura Medida em milímetros \n 1 volta -> 360º"
		).pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)
		ttk.Label(t,text="Altura percorrida mm:",width=20).pack(side=tk.LEFT)
		self.var_calibramaxpos = tk.StringVar()
		self.calibramaxpos_input = ttk.Entry(t,textvariable=self.var_calibramaxpos)
		self.calibramaxpos_input.pack(side=tk.LEFT)
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)
		ttk.Label(t,text="Voltas do Fuso:",width=20).pack(side=tk.LEFT)
		self.var_calibramaxvoltas = tk.StringVar()
		self.calibramaxvoltas_input = ttk.Entry(t,textvariable=self.var_calibramaxvoltas)
		self.calibramaxvoltas_input.pack(side=tk.LEFT)
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		ttk.Label(group,text="Controle Por Voltas:").pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)

		self.var_voltas_input = tk.StringVar()
		self.voltas_input = ttk.Entry(t,textvariable=self.var_voltas_input)
		self.voltas_input.pack(side=tk.LEFT)
		self.var_voltas_input.set('0.0')

		self.voltas_inputbutton = ttk.Button(t,text='Setar Voltas',command=self.manager.setar_voltas)
		self.voltas_inputbutton.pack(side=tk.LEFT)
		
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		self.var_voltascurrentpos_label = tk.StringVar()
		self.voltascurrentpos_label = ttk.Label(group,textvariable=self.var_voltascurrentpos_label)
		self.voltascurrentpos_label.pack(side=tk.TOP,anchor=tk.W,padx=20)
		self.var_voltascurrentpos_label.set('voltas atual: 0')
		
		self.button_calibrarrosca = ttk.Button(group,text="Calibrar",command=self.manager.calibrar_rosca)
		self.button_calibrarrosca.pack(side=tk.BOTTOM,anchor=tk.SE,padx=20,pady=(0,10))
		
		group.grid(row=0,column=0,sticky=tk.N+tk.E+tk.S+tk.W)
		
		group = ttk.Labelframe(self.frame_calibracao, text="Calibrar Motor")
		
		ttk.Label(group,wraplength=250,
			text="Velocidade Medida em passos/s \nobs: "+str(CONF.PASSOS_POR_VOLTA)+" passos -> 1 volta(360º)\n Não colocar valores acima de 1000 passos/s"
			).pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)
		ttk.Label(t,text="Velocidade do motor:",width=20).pack(side=tk.LEFT)
		self.var_calibramotorvel = tk.StringVar()
		self.calibramotorvel_input = ttk.Entry(t,textvariable=self.var_calibramotorvel)
		self.calibramotorvel_input.pack(side=tk.LEFT)
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)
		ttk.Label(t,text="Aceleração do motor:",width=20).pack(side=tk.LEFT)
		self.var_calibramotoracel = tk.StringVar()
		self.calibramotoracel_input = ttk.Entry(t,textvariable=self.var_calibramotoracel)
		self.calibramotoracel_input.pack(side=tk.LEFT)
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		self.button_calibrarmotor = ttk.Button(group,text="Calibrar",command=self.manager.calibrar_motor)
		self.button_calibrarmotor.pack(side=tk.BOTTOM,anchor=tk.SE,padx=20,pady=(0,10))
		
		group.grid(row=0,column=1,sticky=tk.N+tk.E+tk.S+tk.W)
		
		group = ttk.Labelframe(self.frame_calibracao,text="Controle Motor")
		
		t = tk.Frame(group)
		
		ttk.Label(t,text="Setar Fim de curso:").pack(side=tk.LEFT)
		
		self.button_calibra_fimdecurso = ttk.Button(t,text="Ok",command=self.manager.calibrar_fimdecurso)
		self.button_calibra_fimdecurso.pack(side=tk.LEFT)
		
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		t = tk.Frame(group)
		
		ttk.Label(t,text="Girar Motor:").pack(side=tk.TOP)
		
		addframe = tk.Frame(t)
		
		add_volta = ttk.Button(addframe,width=5,text="+360°",command=lambda : self.manager.add_passos(CONF.PASSOS_POR_VOLTA))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="+180°",command=lambda : self.manager.add_passos(CONF.PASSOS_180))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="+90°",command=lambda : self.manager.add_passos(CONF.PASSOS_90))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="+45°",command=lambda : self.manager.add_passos(CONF.PASSOS_45))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="+9°",command=lambda : self.manager.add_passos(CONF.PASSOS_9))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="+1,8°",command=lambda : self.manager.add_passos(CONF.PASSOS_1_8))
		add_volta.pack(side=tk.LEFT,padx=2)
		
		addframe.pack(side=tk.TOP)
		addframe = tk.Frame(t)
		
		add_volta = ttk.Button(addframe,width=5,text="-360°",command=lambda : self.manager.add_passos(-CONF.PASSOS_POR_VOLTA))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="-180°",command=lambda : self.manager.add_passos(-CONF.PASSOS_180))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="-90°",command=lambda : self.manager.add_passos(-CONF.PASSOS_90))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="-45°",command=lambda : self.manager.add_passos(-CONF.PASSOS_45))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=5,text="-9°",command=lambda : self.manager.add_passos(-CONF.PASSOS_9))
		add_volta.pack(side=tk.LEFT,padx=2)
		add_volta = ttk.Button(addframe,width=6,text="-1,8°",command=lambda : self.manager.add_passos(-CONF.PASSOS_1_8))
		add_volta.pack(side=tk.LEFT,padx=2)
		
		addframe.pack(side=tk.TOP)
		
		t.pack(side=tk.TOP,anchor=tk.W,padx=20)
		
		group.grid(row=1,column=0,sticky=tk.N+tk.E+tk.S+tk.W)
		
		
		self.button_calibra_salvar = ttk.Button(self.frame_calibracao,text="Salvar",command=self.manager.calibrar_salvar)
		self.button_calibra_salvar.grid(row=1,column=1,sticky=tk.S+tk.E)
		
		self.frame_calibracao.grid_propagate(0)
		#self.frame_calibracao.grid_rowconfigure(0,minsize = group_height,weight=1)
		#self.frame_calibracao.grid_rowconfigure(1,minsize = group_height,weight=1)
		self.frame_calibracao.grid_rowconfigure(0)
		self.frame_calibracao.grid_rowconfigure(1,weight=1)
		
		self.frame_calibracao.grid_columnconfigure(0,minsize = group_width,weight=1)
		self.frame_calibracao.grid_columnconfigure(1,minsize = group_width,weight=1)
		
		self.topo.pack(fill=tk.BOTH,expand=1,side=tk.TOP)
		self.updating_status = False
		self.updatePortConStatus()
		
		try:
			master.iconbitmap(CONF.IMG["icone"])
		except Exception as e: 
			print(e)
		
		self.after(500,self.onupdate)
	def run(self):
		self.mainloop()

	# You would normally put that on the App class
	def show_error(self, *args):
		err = traceback.format_exception(*args)
		self.oldreport_callback_exception(*args)
		show_warning("Erro Durante Execucao:"+str(args)+"\n\ndetalhes:"+str(err))
	def show_last_error(self, msg = ""):
		err = traceback.format_exception(*sys.exc_info())
		self.oldreport_callback_exception(*sys.exc_info())
		#sys.exc_clear()
		show_warning(msg+"\n\ndetalhes:"+str(err))
	def onupdate(self):
		if pythonserial.isOpen():
			self.after(500,self.onupdate)
			self.manager.update_posicao_atual()
		else:
			self.after(5000,self.onupdate)
			if self.var_conautomatica_check.get():
				#faz um ping, aqui vai tentar conectar com arduino
				# desse modo automaticamente estara detectando o arduino assim que for colocado
				self.trySendArduino(CONF.COD_PING,None)
		
	# quando clica no combobox das portas seriais
	def check_conectarauto(self):
		if self.var_conautomatica_check.get() == 1:
			self.var_showcombo_check.set(0)
			#self.selecionarPorta_combo.configure(state=tk.DISABLED)
			#self.conexao_button.configure(state=tk.DISABLED)
		else:
			self.var_showcombo_check.set(1)
			#self.selecionarPorta_combo.configure(state=tk.NORMAL)
			#self.conexao_button.configure(state=tk.NORMAL)
	def populate_ports(self):
		self.selecionarPorta_combo['values'] = pythonserial.serial_ports(True)
	def conexaobutton(self):
		self.updatePortConStatus()
		if pythonserial.isOpen():
			print("Desconectando")
			pythonserial.disconnect()
		else:
			portSelec = self.selecionarPorta_combo.get()
			self.var_conexaoestado_label.set("Conectando")
			self.var_conexaoporta_label.set(portSelec)
			pythonserial.forceconnect(portSelec)
		self.updatePortConStatus()
	
	def updatePortConStatus(self):
		try:
			# porque ao atualizar o status faz conexao serial que causa outra atualizacao do status
			if not self.updating_status:
				self.updating_status = True
				self.var_conexaoestado_label.set(pythonserial.conection_state)
				self.var_conexaoporta_label.set(pythonserial.port_conected)
				# se a porta foi conectada Agora
				if self.portconnected != pythonserial.isOpen():
					self.portconnected = pythonserial.isOpen()
					if pythonserial.isOpen():
						self.manager.update_initial_values()
						self.conexaoestado_labelimg.configure(image = self.img_conectado)
						self.conexao_button['text'] = "Desconectar"
						try:
							self.var_calibramaxpos.set(str(float(int(pythonserial.max_posicao))/10.0))
							self.var_calibramaxvoltas.set(str(float(int(pythonserial.max_passos))/200.0))
							self.var_calibramotorvel.set(pythonserial.velocidade_motor)
							self.var_calibramotoracel.set(pythonserial.aceleracao_motor)
						except Exception as e: 
							print(e)
					else:
						self.conexaoestado_labelimg.configure(image = self.img_desconectado)
						self.conexao_button['text'] = "Conectar"
				self.updating_status = False
		except Exception as e: 
			self.updating_status = False
			self.show_last_error("Erro ao atualizar Status da Conexao:"+str(e))
		
	def trySendArduino(self,request,args):
		print("Request: "+request.decode()+" "+str(args))
		try:
			# so tenta abrir conexao se esta marcado para isso
			if pythonserial.isOpen() or self.var_conautomatica_check.get():
				pythonserial.tryOpen()
				if pythonserial.isOpen():
					response = sendArduino(request,args)
					if response.status == "OK":
						print("resposta '"+response.status+"' "+str(response.args))
						return response
					else:
						print("resposta '"+response.status+"' "+str(response.args))
						show_warning("Erro no Arduino: status não ok \n status:"+response.status+" args:"+str(response.args))
				else:
					print("A porta serial não pôde ser aberta, conecte seu arduino")
			return None
		except (serial.SerialException,serial.SerialTimeoutException,EOFError) as e:
			pythonserial.disconnect()
			return None
		except Exception as e:
			pythonserial.disconnect()
			self.show_last_error("Erro grave na Comunicação:"+str(e))
			return None
		finally:
			self.updatePortConStatus()