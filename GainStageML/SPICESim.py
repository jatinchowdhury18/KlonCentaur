import os
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy.io import savemat

from PyLTSpice.LTSpiceBatch import LTCommander
from PyLTSpice.LTSpice_RawRead import LTSpiceRawRead

# TEXT -224 760 Left 2 !.tran 0.2\n.probe v(Vout)\n.probe v(Vi)

# # get script absolute path
# meAbsPath = os.path.dirname(os.path.realpath(__file__))
# # select spice model
# LTC = LTCommander(meAbsPath + "\\SPICE\\GainStage2_zzz.asc")

# raw, log = LTC.run()

# print(raw)
raw = 'D:\\Documents\\CCRMA\\Research\\Klon_Centaur\\GainStageML\\SPICE\\GainStage2_zzz_run.raw'

LTR = LTSpiceRawRead(raw)

print(LTR.get_trace_names())
# print(LTR.get_raw_property())

Vo = LTR.get_trace("V(vout)")
Vi = LTR.get_trace("V(vi)")
x = LTR.get_trace('time') # Gets the time axis
step = LTR.get_steps()[0]
plt.plot(x.get_time_axis(step), Vi.get_wave(step))
plt.plot(x.get_time_axis(step), Vo.get_wave(step))

# print(1.0 / 44100)
# print(np.diff(x.get_time_axis(step)))

FS = 44100.0
t = np.arange(0.0, 0.2, 1.0 / FS)
x_interp = interpolate.interp1d(x.get_time_axis(step), Vi.get_wave(step))
y_interp = interpolate.interp1d(x.get_time_axis(step), Vo.get_wave(step))
x = x_interp(t)
y = y_interp(t)

plt.plot(t, x)
plt.plot(t, y)

samples = []

samples.append(np.array([x, y]))
samples = np.asarray(samples)
savemat('sample.txt', { 'samples': samples })

# plt.legend() # order a legend
# plt.show()
