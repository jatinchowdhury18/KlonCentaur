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

    w, h = signal.freqs([b0, b1], [a0, a1], worN=np.logspace(0, 4.3, 1000)*(2*np.pi))

    print(np.roots([a0, a1]))

    plt.semilogx(w/(2*np.pi), 20 * np.log10(abs(h)))


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

    # bilinear
    aU = np.zeros(2)
    bU = np.zeros(2)
    a0 = a0s * c + a1s

    aU[0] = a0 / a0
    aU[1] = (-a0s * c + a1s) / a0
    bU[0] =  (b0s * c + b1s) / a0
    bU[1] = (-b0s * c + b1s) / a0

    a = np.array([1.0, 1.0 / aU[1]])
    b = np.array([bU[0] / aU[1], bU[1] / aU[1]])

    print(np.abs(np.roots(a)))
    print(np.abs(np.roots(b)))
    # adsp.plotting.zplane(b, a)
    # plt.show()

    w, h = signal.freqz(bU, aU, worN=np.logspace(0, 4.3, 1000), fs=fs)
    plt.semilogx(w, 20 * np.log10(abs(h)), '--')

# float = 0.287394
treb = [0.0, 0.3, 0.6, 1.0]

# hf_gain = [-8, -4, 0, 4, 8, 12, 16]

# for g in hf_gain:
#     b, a = adsp.design_high_low_shelf(1.0, 10**(g/20), 480.0, FS)
#     adsp.plot_magnitude_response(b, a, fs=FS) 


for t in treb:
    plot_shelf_freqs(t)
    plot_shelf_freqz(t, 2*44100.0)

    # G = (4.7e3 + t*10e3) / (1.8e3 + (1-t)*10e3)
    # print(G)
    # b, a = adsp.design_high_low_shelf(1.0, G, 480.0, FS)
    # adsp.plot_magnitude_response(b, a, fs=FS)


plt.title('Tone Control Frequency Response')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Amplitude response [dB]')
plt.legend(['0 analog', '0 digital', '0.3 analog', '0.3 digital', '0.6 analog', '0.6 digital', '1.0 analog', '1.0 digital'])
plt.xlim(20, 20000)
plt.grid()
plt.show()
