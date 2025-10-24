import time
import json
import kcirepython.config as CONF
import kcirepython.serial.pythonserial as pythonserial

class ReturnStatus:
	def __init__(self,status,args = None):
		self.status = status
		if args != None and len(args) > 0:
			self.args = args
		else:
			self.args = None

def sendArduino(request,args):
	#json_array = json.loads(json_string)
	#request = str(json_array["request"])
	#args = json_array["args"]
	
	line = request.decode()
	
	if(args != None):
		for arg in args:
			line += CONF.COD_ESPACO.decode()
			line += str(arg)
	
	pythonserial.writeline(line);
	# espera ate chegar algum byte de resposta
	#while not ser.in_waiting > 0:
	#	time.sleep (1.0 / 1000.0);
		
	# le a resposta ate o \n 
	resposta = pythonserial.readline();
	params = resposta.split(" ");
	return ReturnStatus(params[0],params[1:])
	#status = params[0];
	#content = "null"
	#if len(params) == 2:
	#	content = "[\""+params[1]+"\"]";
	#if len(params) == 3:
	#	content = "[\""+params[1]+"\",\""+params[2]+"\"]";
	#
	#return "{\"status\":\""+status+"\",\"content\":"+content+"}";
