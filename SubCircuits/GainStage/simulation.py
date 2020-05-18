import numpy as np
from numpy.polynomial import polynomial as poly
import scipy.signal as signal
import matplotlib.pyplot as plt

# Component values
GAIN = 1.0

R6 = 10e3
Ra = 100e3 * GAIN
R10b = 2e3 + 100e3 * (1-GAIN)
R11 = 15e3
R12 = 422e3

C3 = 0.1e-6
C5 = 68e-9
C7 = 82e-9
C8 = 390e-12


a0s = C7 * C8 * R10b * R11 * R12
a1s = C7 * R10b * R11 + C8 * R12 * (R10b + R11)
a2s = R10b + R11
b0s = a0s
b1s = C7 * R11 * R12 + a1s
b2s = R12 + a2s

w, h = signal.freqs([b0s, b1s, b2s], [a0s, a1s, a2s], worN=np.logspace(1.3, 4.3, 1000)*(2*np.pi))

plt.semilogx(w/(2*np.pi), 20*np.log10(np.abs(h+np.finfo(float).eps)))
plt.show()

# Create impedances
# z1Num = R6 # poly.Polynomial((1, R6 * (C3 + C5)))
# z1Den = poly.Polynomial((0, C3, R6 * C3 * C5))
# z2Num = R10b + R11 # poly.Polynomial((R10b + R11, C7 * R10b * R11))
# z2Den = 1.0 # poly.Polynomial((1, C7 * R11))
# z3Num = R12
# z3Den = 1 # poly.Polynomial((1, C8 * R12))

# # Simplify
# b_s = z1Den * Ra * (z3Den * z2Num + z2Den * z3Num)
# a_s = z2Den * z3Den * (Ra * z1Den + z1Num)

# print(b_s.coef)
# print(a_s.coef)

# w, h = signal.freqs(b_s.coef, a_s.coef, worN=np.logspace(0, 2, 1000)*(2*np.pi))

# plt.semilogx(w/(2*np.pi), 20*np.log10(np.abs(h+np.finfo(float).eps)))
# plt.show()
