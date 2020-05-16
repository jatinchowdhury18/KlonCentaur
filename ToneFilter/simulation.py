import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp

# treble = 0.0
FS = 44100.0

def plot_shelf_freqs(treble):
    Rpot = 10e3
    C = 3.9e-9
    G1 = 1.0 / 100e3
    G2 = 1.0 / (1.8e3 + (1-treble)*Rpot)
    G3 = 1.0 / (4.7e3 + treble*Rpot)
    G4 = 1.0 /  100e3

    b0 = C * (G1 + G2)
    b1 = G1 * (G2 + G3)
    a0 = C * (G3 - G4)
    a1 = -G4 * (G2 + G3)

    w, h = signal.freqs([b0, b1], [a0, a1], worN=np.logspace(1.3, 4.3, 1000))

    print(np.roots([a0, a1]))

    plt.semilogx(w, 20 * np.log10(abs(h)))


def plot_shelf_freqz(treble, fs):
    Rpot = 10e3
    C = 3.9e-9
    G1 = 1.0 / 100e3
    G2 = 1.0 / (1.8e3 + (1-treble)*Rpot)
    G3 = 1.0 / (4.7e3 + treble*Rpot)
    G4 = 1.0 /  100e3

    b0s = C * (G1 + G2)
    b1s = G1 * (G2 + G3)
    a0s = C * (G3 - G4)
    a1s = -G4 * (G2 + G3)

    T = 1.0 / fs
    wc = G1 / C
    c = wc / np.tan(wc * T / 2.0)

    b, a = signal.bilinear([b0s, b1s],  [a0s, a1s], fs=fs)

    # bilinear
    # a = np.zeros(2)
    # b = np.zeros(2)
    # a0 = a0s * c + a1s

    # a[0] = a0 / a0
    # a[1] = (-a0s * c + a1s) / a0
    # b[0] =  (b0s * c + b1s) / a0
    # b[1] = (-b0s * c + b1s) / a0

    print(np.roots(a))

    w, h = signal.freqz(b, a, worN=np.logspace(1.3, 4.3, 1000), fs=fs)
    plt.semilogx(w, 20 * np.log10(abs(h)))

# float = 0.287394
# treb = [1.0] # [0.0, 0.2, 0.4, 0.6, 0.8, 1.0]

hf_gain = [-8, -4, 0, 4, 8, 12, 16]

for g in hf_gain:
    b, a = adsp.design_high_low_shelf(1.0, 10**(g/20), 480.0, FS)
    adsp.plot_magnitude_response(b, a, fs=FS)


# for t in treb:
#     plot_shelf_freqs(t)
#     plot_shelf_freqz(t, 44100.0)

plt.xlabel('Frequency')
plt.ylabel('Amplitude response [dB]')
plt.xlim(1, 20000)
plt.grid()
plt.show()
