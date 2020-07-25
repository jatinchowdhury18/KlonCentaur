import os
import numpy as np
import matplotlib.pyplot as plt
from scipy import interpolate
from scipy.io import savemat, loadmat

from PyLTSpice.LTSpiceBatch import LTCommander
from PyLTSpice.LTSpice_RawRead import LTSpiceRawRead

def clean_up_SPICE(file):
    for ext in ['.asc', '.masterlog', '.net', '_run.net', '_run.op.raw', '_run.raw', '1.log']:
        os.system(f'rm {file}{ext}')

def set_SPICE_params(file, freq, amp, N_cycles, gainVal=0.5):
    rTop = max(100e3 * gainVal, 1)
    rBot = max(100e3 * (1 - gainVal), 1)
    with open(file, "a+") as file_object:
        # Move read cursor to the start of file.
        file_object.seek(0)
        data = file_object.read(100)
        # Append text at the end of file
        file_object.write(f'TEXT -928 552 Left 2 !.tran 0.2\\n.param fr={freq} N={N_cycles} G={amp} RVaTop={rTop} RVaBot={rBot} RVbTop={rTop} RVbBot={rBot}\\n.probe v(Vi) v(Vout)')
        file_object.write("\n")

def createSample(freq, amp, gainVal, N):
    filename = f'{meAbsPath}\\SPICE\\GainStage_samp{N}.asc'
    # os.system(f'cp {meAbsPath}\\SPICE\\GainStage2_zzz3.asc {filename}')
    # set_SPICE_params(filename, freq, amp, int(freq/10), gainVal)

    # LTC = LTCommander(filename)
    # raw, log = LTC.run()
    raw = 'D:/Documents/CCRMA/Research/Klon_Centaur/GainStageML/SPICE/GainStage2_zzz2.raw'
    LTR = LTSpiceRawRead(raw)

    # try:
    #     LTR = LTSpiceRawRead(raw)
    # except:
    #     LTR = LTSpiceRawRead(f'{meAbsPath}\\SPICE\\GainStage_samp{N}_run.raw')

    Vo = LTR.get_trace("V(vout)")
    Vi = LTR.get_trace("V(vi)")
    x = LTR.get_trace('time') # Gets the time axis
    step = LTR.get_steps()[0]
    # plt.plot(x.get_time_axis(step), Vi.get_wave(step))
    # plt.plot(x.get_time_axis(step), Vo.get_wave(step))

    FS = 44100.0
    t = np.arange(0.0, 0.2, 1.0 / FS)
    x_interp = interpolate.interp1d(x.get_time_axis(step), Vi.get_wave(step))
    y_interp = interpolate.interp1d(x.get_time_axis(step), Vo.get_wave(step))
    x = x_interp(t)
    y = y_interp(t)

    # plt.plot(t, x, '--')
    # plt.plot(t, y, '--')
    # plt.show()
    g = np.ones_like(x) * gainVal

    samples = [np.array([x, g, y])]
    savemat(f'Samples\\sample{N}.mat', { 'samples': np.asarray(samples) })

    # clean_up_SPICE(f'{meAbsPath}\\SPICE\\GainStage_samp{N}')


# get script absolute path
meAbsPath = os.path.dirname(os.path.realpath(__file__))

N = 338
freqs = [50, 75, 100, 150, 200, 250, 300, 400, 600, 800, 1000, 2000, 4000]
gains = [0.1, 0.25, 0.5, 0.75, 0.9, 1.0]
paramVals = [0.99] #[0.01, 0.25, 0.5, 0.75, 0.99]

# for freq in freqs:
#     for G in gains:
#         for p in paramVals:
#             print(f'{freq}, {G}, {p}')
#             try:
#                 createSample(freq, G, p, N)
#             except:
#                 pass
#             N += 1

N = 76
createSample(100, 1.0, 0.99, N)

# mat_dict = loadmat('Samples\\sample0.mat')
# samples = mat_dict['samples']
# print(np.shape(samples))

# x_values = samples[0][0]
# y_values = samples[0][2]

# plt.plot(x_values)
# plt.plot(y_values)
# print(np.shape(x_values))
# print(np.shape(y_values))
# plt.show()
