import os
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy.io import wavfile
from PyLTSpice.LTSpice_RawRead import LTSpiceRawRead

raw = 'D:/Documents/CCRMA/Research/Klon_Centaur/SubCircuits/FeedForward1/SPICE/FF1.raw'
LTR = LTSpiceRawRead(raw)

LTR

Vo = LTR.get_trace("V(vout)")
Vi = LTR.get_trace("V(vin)")
x = LTR.get_trace('time') # Gets the time axis
step = LTR.get_steps()[0]
# plt.plot(x.get_time_axis(step), Vi.get_wave(step))
# plt.plot(x.get_time_axis(step), Vo.get_wave(step))

FS = 44100.0
t = np.arange(0.0, 2.0, 1.0 / FS)
x_interp = interpolate.interp1d(x.get_time_axis(step), Vi.get_wave(step))
y_interp = interpolate.interp1d(x.get_time_axis(step), Vo.get_wave(step))
x = x_interp(t)
y = y_interp(t)

# plt.plot(t, x, '--')
# plt.plot(t, y, '--')
# plt.show()

# wavfile.write('square.wav', int(FS), (x * 0.999 * 2**15).astype(np.int16))
fs, y_wdf = wavfile.read('wdf_out2.wav')


y_wdf = y_wdf[:len(t),0] / 2**15

si = 3000
ei = -1000
plt.plot(t[si:ei], y[si:ei], label='Analog')
plt.plot(t[si:ei], -2.15*y_wdf[si:ei], '--', label='WDF')
plt.legend()
plt.xlabel('Time [s]')
plt.ylabel('Voltage [V]')
plt.title('Feed-Forward Network 1 Response')
plt.show()
