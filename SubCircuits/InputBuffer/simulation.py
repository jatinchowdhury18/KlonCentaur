import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

FS = 44100
R1 = 10e3
R2 = 1e6
C1 = 0.1e-6

b0s = C1 * R2
b1s = 0.0
a0s = C1 * (R1 + R2)
a1s = 1.0

w, h = signal.freqs([b0s, b1s], [a0s, a1s], worN=np.logspace(0, 4.3, 1000)*(2*np.pi))
plt.semilogx(w/(2*np.pi), 20 * np.log10(abs(h)))


K = 2.0 * FS
a0 = a0s * K + a1s
b0z = ( b0s * K + b1s) / a0
b1z = (-b0s * K + b1s) / a0
a0z = 1.0
a1z = (-a0s * K + a1s) / a0

w, h = signal.freqz([b0z, b1z], [a0z, a1z], worN=np.logspace(0, 4.3, 1000), fs=FS)
plt.semilogx(w, 20 * np.log10(abs(h)), '--')

plt.show()
