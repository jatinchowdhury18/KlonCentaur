import numpy as np
import os
import random
import librosa
import matplotlib.pyplot as plt
import tensorflow as tf

# Frequency smoothing
def freqSmooth(x, sm=1.0/24.0):
    s = sm if sm > 1.0 else np.sqrt(2.0**sm)
    N = len(x)
    y = np.zeros_like(x)
    for i in range(N):
        i1 = max(int(np.floor(i/s)), 0)
        i2 = min(int(np.floor(i*s)+1), N-1)
        if i2 > i1:
            y[i] = np.mean(x[i1:i2])
    return y

# FFT with frequency smoothing
def plot_fft(x, fs, sm=1.0/24.0):
    fft = freqSmooth(20 * np.log10(np.abs(np.fft.rfft(x) + 1.0e-9)), sm=sm)
    freqs = np.fft.rfftfreq(len(x), 1.0 / fs)
    return freqs, fft

# load file from FMA dataset
def load_fma_file(files, filepath, fs, N):
    subfolder = files[random.randrange(0, len(files))]
    subfolderpath = filepath + subfolder + '/'
    subfiles = os.listdir(subfolderpath)
    
    mp3path = subfolderpath + subfiles[random.randrange(0, len(subfiles))]
    x, sr = librosa.load(mp3path, sr=fs, mono=True)
        
    start_idx = int(random.uniform(0, len(x) - N))
    return x[start_idx:start_idx+N]

def plot_metric(history, key, val=True):
    loss = history.history[key]
    if val:
        val_loss = history.history['val_' + key]
    epochs = range(1, len(loss) + 1)

    plt.plot(epochs, loss, 'g.', label='Training ' + key)
    if val:
        plt.plot(epochs, val_loss, 'b', label='Validation ' + key)
    plt.title('Training and validation ' + key)
    plt.xlabel('Epochs')
    plt.ylabel(key)
    plt.legend()
