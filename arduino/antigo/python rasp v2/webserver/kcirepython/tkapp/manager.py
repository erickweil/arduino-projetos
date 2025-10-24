from kcirepython.tkapp.interface import *
from kcirepython.tkapp.dialog import *
import kcirepython.config as CONF
class InterfaceManager:

	def readFloat(self,strvalue):
		try:
			if strvalue != None:
				if '.' in strvalue:
					return float(strvalue)
				else:
					return float(int(strvalue))
			else:
				return 0
		except Exception as e:
			print(e)
			print_last_error("Erro ao ler o valor '"+str(strvalue)+"': "+str(e))
			return 0

	def __init__(self, app):
		self.app = app
		#Variaveis de controle
		self.posicao_atual = 0
		self.posicao_alvo = 0
		self.girovel = 0
		self.girodir = "Horário"
		
	# controle Posicionador
	def add_posicao(self,value):
		self.update_posicao_alvo(self.posicao_alvo + value)
	
	def setar_posicao(self):
		self.update_posicao_alvo(int(self.readFloat(self.app.var_pos_input.get())*10.0))
	
	def update_posicao_alvo(self,val):
		val = min(max(val,0),3000)
		#if self.posicao_alvo != val:
		self.posicao_alvo = val
		self.app.var_pos_input.set(float(val)/10.0)
		self.app.var_target_slider.set(val)
		#decimos de milimetros
		self.app.var_mmtargetpos_label.set('Posição alvo: '+str(val/10)+'mm')
		self.app.trySendArduino(CONF.COD_SETAR_POSICAO,[val])
		print('position:',self.posicao_alvo)
	
	def update_posicao_atual(self):
		resposta = self.app.trySendArduino(CONF.COD_LER_POSICAO,[])
		if resposta != None and resposta.status == "OK" and resposta.args != None and len(resposta.args) == 2:
			val = int(resposta.args[0])
			val_passos = int(resposta.args[1])
			if self.posicao_atual != val:
				self.posicao_atual = val
				self.app.var_current_slider.set(val)
				#decimos de milimetros
				self.app.var_mmcurrentpos_label.set('Posição atual: '+str(val/10)+'mm')
				#voltas
				self.app.var_voltascurrentpos_label.set('voltas atual: '+str(float(val_passos)/float(CONF.PASSOS_POR_VOLTA)))

	def stop(self):
		self.app.trySendArduino(CONF.COD_PARAR_MOTOR,None)
		
	def update_initial_values(self):
		print("Atualizando Valores Iniciais")
		self.update_posicao_atual()
		resposta = self.app.trySendArduino(CONF.COD_LER_ALVO,[])
		if resposta != None and resposta.status == "OK" and resposta.args != None and len(resposta.args) > 0:
			val = int(resposta.args[0])
			self.update_posicao_alvo(val)
		
		resposta = self.app.trySendArduino(CONF.COD_LER_GIROVEL,[])
		if resposta != None and resposta.status == "OK" and resposta.args != None and len(resposta.args) > 0:
			val = int(resposta.args[0])
			self.update_girovel(val)
		
	# controle girador amostra
	def girodir_combo(self,*args):
		val = self.app.var_giradorsentido_combo.get()
		#print("girodir_combo: "+str(val))
		if self.girodir != val:
			self.girodir = val
			self.set_girovel(abs(self.girovel))
	def set_girovel(self,val):
		#print("set_girovel: "+str(val))
		if self.girodir == "Horário":
			self.update_girovel(val)
		else:
			self.update_girovel(-val)
	def parargiro(self):
		self.update_girovel(0)
	def update_girovel(self,val):
		val = min(max(val,-3),3)
		#print("update_girovel: "+str(val))
		#if self.girovel != val:
		self.girovel = val
		if val < 0:
			dirgirovel = -val
			self.girodir = "Anti-Horário"
		elif val > 0:
			dirgirovel = val
			self.girodir = "Horário"
		else:
			dirgirovel = 0
		
		if self.girodir != self.app.var_giradorsentido_combo.get():
			self.app.var_giradorsentido_combo.set(self.girodir)
		
		if dirgirovel == 0:
			self.app.var_girovel_label.set('Velocidade atual: Parado')
		else:
			self.app.var_girovel_label.set('Velocidade atual: '+str(dirgirovel))
		self.app.var_giradorvel_slider.set(dirgirovel)
		self.app.trySendArduino(CONF.COD_SETAR_GIROVEL,[val])
		#print('girovel:',val)
	def calibrar_rosca(self):
		#passos_por_volta * voltas_ate_topo -> (altura_rosca)
		#passos                             -> x
		#
		#max_passos -> max_posicao
		#passos     -> x
		#
		#resumindo
		#
		#passos_por_volta = 200
		#voltas_ate_topo = 41
		#altura_rosca = 32.35
		#
		#max_passos = passos_por_volta * voltas_ate_topo
		#max_posicao = (altura_rosca)
		
		voltas_ate_topo = self.readFloat(self.app.var_calibramaxvoltas.get())
		# input em mm, calculo em decimos de mm
		altura_rosca = self.readFloat(self.app.var_calibramaxpos.get())*10.0
		
		max_passos = int(float(CONF.PASSOS_POR_VOLTA) * voltas_ate_topo)
		max_posicao = int(altura_rosca)
		
		self.app.trySendArduino(CONF.COD_CALIBRA_MAX,[max_posicao,max_passos])
		self.update_initial_values()
	def calibrar_motor(self):
		vel_motor = int(self.readFloat(self.app.var_calibramotorvel.get()))
		acel_motor = int(self.readFloat(self.app.var_calibramotoracel.get()))
		self.app.trySendArduino(CONF.COD_CALIBRA_MOTOR,[vel_motor,acel_motor])
	def calibrar_fimdecurso(self):
		self.app.trySendArduino(CONF.COD_CALIBRA_MIN,[])
		self.update_initial_values()
	def calibrar_salvar(self):
		self.app.trySendArduino(CONF.COD_SALVAR_CALIBRA,[])
	def add_passos(self,val):
		self.app.trySendArduino(CONF.COD_ADD_PASSOS,[val])
	def setar_voltas(self):
		val = int(self.readFloat(self.app.var_voltas_input.get())*float(CONF.PASSOS_POR_VOLTA))
		self.app.trySendArduino(CONF.COD_SETAR_PASSOS,[val])
		#self.update_initial_values()