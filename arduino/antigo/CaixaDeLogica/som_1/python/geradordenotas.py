from __future__ import division
from pylab import*
from scipy.io import wavfile

import waveanalyzer
sampFreq, raw_sound = wavfile.read('testesom.wav')

#this means that the sound pressure values are mapped to integer values that can range from -2^15 to (2^15)-1.
#We can convert our sound array to floating point values ranging from -1 to 1 as follows:
snd = raw_sound / (2.**15)
print( "Shape:" ,snd.shape)
print( "Sampling Freq:" ,sampFreq)
size = snd.shape[0]
print( "Milis:" ,size / sampFreq)
s1 = snd #we’ll select and work with only one of the channels from now onwards

#timeArray = arange(0, size, 1)
#timeArray = timeArray / sampFreq
#timeArray = timeArray * 1000  #scale to milliseconds


#plot(timeArray, s1, color='k')
#ylabel('Amplitude')
#xlabel('Time (ms)')
sound_freq_list = []

step_freq = sampFreq/10
i = 0
while(i < size):
	clampstart = int(i * step_freq)
	clampend = int(min((i+1)*step_freq,size))
	slice = raw_sound[clampstart:clampend]
	freq_estimation = waveanalyzer.freq_from_hps(slice,sampFreq)
	sound_freq_list.append(freq_estimation)
	i += 1

plot(freq_estimation,range(0,count(freq_estimation)))