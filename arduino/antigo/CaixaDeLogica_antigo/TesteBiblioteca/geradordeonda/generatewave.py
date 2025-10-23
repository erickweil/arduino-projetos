import math

def inline_nop_delay(nops):
	str_command = ""
	
	if nops >= 400:
		str_command += "nop400();"
		nops = nops - 400
		
	if nops >= 400:
		str_command += "nop400();"
		nops = nops - 400
		
	if nops >= 400:
		str_command += "nop400();"
		nops = nops - 400
		
	if nops >= 400:
		str_command += "nop400();"
		nops = nops - 400
	
	if nops >= 200:
		str_command += "nop200();"
		nops = nops - 200
		
	if nops >= 200:
		str_command += "nop200();"
		nops = nops - 200
		
	if nops >= 100:
		str_command += "nop100();"
		nops = nops - 100
		
	if nops >= 100:
		str_command += "nop100();"
		nops = nops - 100
		
	if nops >= 50:
		str_command += "nop50();"
		nops = nops - 50
	
	if nops >= 20:
		str_command += "nop20();"
		nops = nops - 20
	
	if nops >= 20:
		str_command += "nop20();"
		nops = nops - 20
	
	if nops >= 10:
		str_command += "nop10();"
		nops = nops - 10
	
	if nops >= 10:
		str_command += "nop10();"
		nops = nops - 10
	
	if nops != 0:
		str_command += "NOP"+str(nops)+";"
		
	return str_command

def range_radian(r):
	return math.radians(360.0*r)

NOPDELAY = 0.0625	
	
TONEDELAY = 75.0
	
freq = 262.0;
microsdelay  = 500000.0/freq;
microsperiod = 1000000.0/freq;
samples = int(round(microsperiod/TONEDELAY))


f = open('generated.cpp','w')

for i in range(0,samples):
	# calculate pwm percentage based on sine wave
	r = i/samples
	wave_amplitude = math.sin(range_radian(r))
	normalized_amplitude = wave_amplitude + 1.0
	percent_amplitude = (normalized_amplitude / 2.0)
	
	# calculate pwm on and off times based on calculated percentage
	time_on = TONEDELAY*percent_amplitude
	time_off = TONEDELAY - time_on
	
	#nops_on = int(round(time_on/NOPDELAY))
	#nops_off = int(round(time_off/NOPDELAY))
	
	f.write("TONEON;\n")
	f.write("delayMicroseconds("+str(int(round(time_on)))+");\n")
	#f.write(inline_nop_delay(nops_on)+"\n")
	f.write("TONEOFF;\n")
	f.write("delayMicroseconds("+str(int(round(time_off)))+");\n")
	#f.write(inline_nop_delay(nops_off)+"\n")

f.close() # you can omit in most cases as the destructor will call it