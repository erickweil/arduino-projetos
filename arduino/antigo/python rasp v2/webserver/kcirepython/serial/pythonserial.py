import sys
import glob
import kcirepython.config as CONF
from kcirepython.tkapp.preferences import PREF
import serial
import serial.tools.list_ports

DEBUG = False
serial_log = None
conection_state = "Desconectado"
port_conected = None
ser = None
max_posicao =0
max_passos =0
velocidade_motor =0
aceleracao_motor =0

#apenas pra checar se esta aberta a conexao
def isOpen():
	global ser
	if (ser == None) or (not ser.isOpen()):
		return False
	else:
		return True
# checa se a conexao com arduino existe, se nao, fecha a antiga se existir e conecta denovo
def tryOpen():
	global ser
	if (ser == None) or (not ser.isOpen()):
		if ser != None:
			disconnect()
		connect()

# conecta ao arduino tentando se conectar a todas as portas retornadas por serial_ports()
def connect():
	global ser,conection_state,port_conected
	print("Conectando com o arduino, portas seriais disponiveis")
	portas_seriais = serial_ports()
	print(portas_seriais)
	# tenta as portas uma a uma se conectar com o arduino
	for port in portas_seriais:
		forceconnect(port)
# desconecta o arduino
def disconnect():
	global ser,conection_state,port_conected
	try:
		ser.close()
		if serial_log != None:
			serial_log("<Porta Serial Desconectada>\n")
	except Exception as e: 
		print(e)
	conection_state = "Desconectado"
	port_conected = None
	ser = None

def forceconnect(port):
	global ser,conection_state,port_conected,max_posicao,max_passos,velocidade_motor,aceleracao_motor
	print("Conectando com o arduino, na porta "+port)
	try:
	
		ser = serial.Serial(
			baudrate=CONF.BAUDRATE,\
			parity=serial.PARITY_NONE,\
			stopbits=serial.STOPBITS_ONE,\
			bytesize=serial.EIGHTBITS,\
			timeout=PREF.READ_TIMEOUT)
		ser.port = port
		if PREF.PREVENT_ARDUINO_RESET:
			ser.dtr = False
		ser.open()
		
		try:
			if serial_log != None:
				serial_log("<Porta Serial '"+port+"' Conectada>\n")
			if PREF.PREVENT_ARDUINO_RESET:
				writeline(CONF.COD_PING.decode())
				linha_resposta = readline();
				if linha_resposta != None and linha_resposta == "OK":
					conection_state = "Conectado"
					port_conected = port
					return
				else:
					print("Resposta incorreta na porta '"+port+"' ")
					print("E realmente um Arduino? ele esta com o codigo correto?")
					print("Esperando 'OK' recebeu:")
					print(linha_resposta)
			else:
				linha_resposta = readline()
				if linha_resposta != None and linha_resposta.split(" ")[0] == "OK":
					params = linha_resposta.split(" ")
					print("Conexao feita com sucesso na porta '"+port+"'")
					print("Versao Arduino:"+params[1]+"")
					print("Calibrado:"+params[2]+"")
					linha_resposta = readline()
					params = linha_resposta.split(" ")
					max_posicao =params[0]
					max_passos =params[1]
					velocidade_motor =params[2]
					aceleracao_motor =params[3]
					print("max_posicao :"+params[0]+"")
					print("max_passos :"+params[1]+"")
					print("velocidade_motor :"+params[2]+"")
					print("aceleracao_motor :"+params[3]+"")
					conection_state = "Conectado"
					port_conected = port
					return
				else:
					print("Resposta incorreta na porta '"+port+"' ")
					print("E realmente um Arduino? ele esta com o codigo correto?")
					print("Esperando 'OK' recebeu:")
					print(linha_resposta)
		except EOFError:
			print("A Conexao serial na porta '"+port+"' nao responde")
	except Exception as e: 
		print("")
		print(e)
		print("ocorreu um erro ao conectar a porta '"+port+"' ")
	# só chega aqui quando não conseguiu conectar
	disconnect()
	
def writeline(text):
	global ser
	if text != None and len(text) > 0:
		#limpa todo o input, descartando tudo que havia nele
		ser.reset_input_buffer()
		ser.reset_output_buffer()

		ser.write(text.encode())
		ser.write(CONF.COD_FIM)
		
		# espera ate todo o conteudo do buffer ser escrito
		ser.flush()

		if serial_log != None:
			serial_log(">"+text+"\n")

def readline():
	global ser
	rv = ""
	while len(rv) < 80:
		b = ser.read()
		if b == CONF.COD_FIM:
			print("LIDO: '"+rv+"'")
			if serial_log != None:
				serial_log(rv+"\n")
			return rv
		ch = b.decode()
		if ch!="":
			rv += ch
		else:
			print("Antes do erro LIDO: '"+rv+"'")
			raise EOFError()
	print("Excedeu 80 caracteres")
	if serial_log != None:
		serial_log(rv+"\n")
	return rv

# lista todas a portas seriais disponiveis e utilizando a informacao delas, encontra qual(is) 
# e(sao) porta(s) arduino
def serial_ports(all_ports = False): 
	ports = serial.tools.list_ports.comports()
	result = []
	for port in ports:
		try:
			if DEBUG:
				print("device:"+ str(port.device))
				#Full device name/path, e.g. /dev/ttyUSB0. This is also the information returned as first element when accessed by index.

				print("name:"+ str(port.name))
				#Short device name, e.g. ttyUSB0.

				print("description:"+ str(port.description))
				#Human readable description or n/a. This is also the information returned as second element when accessed by index.

				print("hwid:"+ str(port.hwid))
				#Technical description or n/a. This is also the information returned as third element when accessed by index.

				#USB specific data, these are all None if it is not an USB device (or the platform does not support extended info).

				print("vid:"+ str(port.vid))
				#USB Vendor ID (integer, 0...65535).

				print("pid:"+ str(port.pid))
				#USB product ID (integer, 0...65535).

				print("serial_number:"+ str(port.serial_number))
				#USB serial number as a string.

				print("location:"+ str(port.location))
				#USB device location string (“<bus>-<port>[-<port>]...”)

				print("manufacturer:"+ str(port.manufacturer))
				#USB manufacturer string, as reported by device.

				print("product:"+ str(port.product))
				#USB product string, as reported by device.

				print("interface:"+ str(port.interface))
				#Interface specific description, e.g. used in compound USB devices.
			# apenas adiciona a porta ao resultado se ela e a porta de um arduino mesmo
			# tem que ver se isso tem em todos as conexões USB, se todos os arduinos tem isso, se o linux vai detectar.
			if all_ports or "Arduino" in port.description: 
				result.append(port.device)
		except Exception:
			pass
	return result

# parei de usar essa funcao por ela tentar uma conexao para descobrir se a porta existe
# e na verdade uma tentativa frustada e o meio errado de se listar as portas seriais	
def serial_portsoold():
	""" Lists serial port names

		:raises EnvironmentError:
			On unsupported or unknown platforms
		:returns:
			A list of the serial ports available on the system
	"""
	if sys.platform.startswith('win'):
		ports = ['COM%s' % (i + 1) for i in range(256)]
	elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
		# this excludes your current terminal "/dev/tty"
		ports = glob.glob('/dev/tty[A-Za-z]*')
	elif sys.platform.startswith('darwin'):
		ports = glob.glob('/dev/tty.*')
	else:
		raise EnvironmentError('Unsupported platform')

	result = []
	for port in ports:
		try:
			s = serial.Serial(port)
			s.close()
			result.append(port)
		except (OSError, serial.SerialException):
			pass
	return result
