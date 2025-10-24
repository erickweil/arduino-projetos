import time
import json
import kcirepython.config as CONF
import kcirepython.serial.pythonserial

def get(ser,json_string):
	json_array = json.loads(json_string)
	request = str(json_array["request"])
	args = json_array["args"]
	
	#limpa todo o input, descartando tudo que havia nele
	ser.reset_input_buffer()
	ser.reset_output_buffer()
	
	ser.write(request.encode())
	
	if(args != None):
		for arg in args:
			ser.write(CONF.COD_ESPACO)
			ser.write(str(arg).encode())
	
	ser.write(CONF.COD_FIM)
	
	# espera ate todo o conteudo do buffer ser escrito
	ser.flush()
	
	# espera ate chegar algum byte de resposta
	#while not ser.in_waiting > 0:
	#	time.sleep (1.0 / 1000.0);
		
	# le a resposta ate o \n 
	resposta = pythonserial.readline(ser);
	params = resposta.split(" ");
	status = params[0];
	content = "null"
	if len(params) == 2:
		content = "[\""+params[1]+"\"]";
	if len(params) == 3:
		content = "[\""+params[1]+"\",\""+params[2]+"\"]";
	
	return "{\"status\":\""+status+"\",\"content\":"+content+"}";
