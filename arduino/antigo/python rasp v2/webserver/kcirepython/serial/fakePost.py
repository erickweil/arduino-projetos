import time
import json
import kcirepython.config as CONF
from kcirepython.serial.sendComand import ReturnStatus
posicao_atual = 0
posicao_alvo = 0
class MyFakeSerial:
	def __init__(self):
		self.closed = False
	def close(self):
		#do nothing
		self.closed = True
	def isOpen(self):
		return (not self.closed)

		
# checa se a conexao com arduino existe, se nao, fecha a antiga se existir e conecta denovo
def tryOpen(ser):
	if (ser == None) or (not ser.isOpen()):
		if ser != None:
			try:
				ser.close()
			except Exception: 
				pass
		return connect()
	else:
		return ser

# conecta ao arduino tentando se conectar a todas as portas retornadas por serial_ports()
def connect():
	return MyFakeSerial()
	
def serial_ports(all_ports = False):
	return ["COM3","COM4","COM10"]

def sendArduino(ser,request,args):
	global posicao_atual,posicao_alvo
	#json_array = json.loads(json_string)
	#request = str(json_array["request"]).encode()
	#args = json_array["args"]
	
	if(request == CONF.COD_SETAR_POSICAO):
		posicao_alvo = int(args[0])
		return ReturnStatus("OK")
	elif(request == CONF.COD_LER_POSICAO):		
		if posicao_atual < posicao_alvo:
			posicao_atual += 1;
		elif posicao_atual > posicao_alvo:
			posicao_atual -= 1;
		return ReturnStatus("OK",[str(int(posicao_atual)),str(posicao_atual*128)])
	elif(request == CONF.COD_LER_ALVO):
		return ReturnStatus("OK",[str(int(posicao_alvo)),str(posicao_alvo*128)])
	elif(request == CONF.COD_CALIBRA_MIN):
		posicao_atual = 0
		posicao_alvo = 0
		return ReturnStatus("OK")
	elif(request == CONF.COD_PARAR_MOTOR):
		posicao_alvo = posicao_atual
		return ReturnStatus("OK")
