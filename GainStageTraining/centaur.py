# %%
# Load dependencies
import tensorflow as tf
from tensorflow import keras
import librosa

import numpy as np
import matplotlib.pyplot as plt
import audio_dspy as adsp
import scipy.signal as signal
from tqdm import tqdm
import os
from pathlib import Path
import random
import sys

from utils import plot_fft
from model import Model
import losses as losses

# %%
# load files
# filepath = '../../KlonCentaur/GainStageML/'
pathlist = Path('DataIn').glob('*.wav')

clean_data = []
dist_data = []
gain_data = []
T_data = []

NUM_SAMPLES = 10000

for p in tqdm(pathlist):
    fs = np.random.uniform(48000, 96000)
    x, sr = librosa.load(str(p), sr=fs, mono=True)
    N = len(x)

    gain = 0.0
    for wav_dir in ['Data0/', 'Data25/', 'Data50/', 'Data75/', 'Data100/']:
        y, sr = librosa.load(wav_dir + p.name, sr=fs, mono=True)
        
        clean_data.append(x[N//2:N//2+NUM_SAMPLES])
        dist_data.append(y[N//2:N//2+NUM_SAMPLES])
        gain_data.append(gain * np.ones(NUM_SAMPLES))
        T_data.append(np.ones(NUM_SAMPLES) / fs)

        gain += 0.25

# %%
clean_data = np.asarray(clean_data)
dist_data = np.asarray(dist_data)
gain_data = np.asarray(gain_data)
T_data = np.asarray(T_data)

# %%
NUM_FILES = len(clean_data)

idx = 4
plt.figure()
plt.plot(clean_data[idx])
plt.plot(dist_data[idx])
plt.plot(gain_data[idx])
plt.plot(T_data[idx])

plt.figure()
freqs, x_fft = plot_fft(clean_data[idx], 1.0 / T_data[idx][0])
freqs, y_fft = plot_fft(dist_data[idx],  1.0 / T_data[idx][0])
plt.semilogx(freqs, x_fft)
plt.semilogx(freqs, y_fft)

# %%
NUM_TRAIN = int(NUM_FILES * 0.95)
NUM_VAL = NUM_FILES - NUM_TRAIN
NUM_INPUTS = 3

x_data = np.stack((clean_data, gain_data, T_data), axis=1).astype(np.float32)

x_train, x_val = np.split(x_data, [NUM_TRAIN])
y_train, y_val  = np.split(dist_data,  [NUM_TRAIN])

# %%
OUT_train  = np.reshape(y_train, (NUM_TRAIN, NUM_SAMPLES, 1))
OUT_val    = np.reshape(y_val, (NUM_VAL, NUM_SAMPLES, 1))
IN_train = np.reshape(x_train.transpose((0, 2, 1)), (NUM_TRAIN, NUM_SAMPLES, NUM_INPUTS))
IN_val   = np.reshape(x_val.transpose((0, 2, 1)), (NUM_VAL, NUM_SAMPLES, NUM_INPUTS))

# %%
plt.plot(IN_train[0, :, 0])
plt.plot(IN_train[0, :, 1])

print(IN_train.dtype)
print(OUT_train.dtype)

# %%
np.save("data/out_train.npy", OUT_train)
np.save("data/out_val.npy", OUT_val)
np.save("data/in_train.npy", IN_train)
np.save("data/in_val.npy", IN_val)

# %%
OUT_train = np.load("data/out_train.npy")
OUT_val   = np.load("data/out_val.npy")
IN_train  = np.load("data/in_train.npy")
IN_val    = np.load("data/in_val.npy")

NUM_SAMPLES = 10000
NUM_INPUTS = 3

# %%
model_file = 'models/centaur.json'
model_hist = 'models/centaur_history.txt'

# %%
def model_loss(target_y, predicted_y):
    return losses.esr_loss(target_y, predicted_y, losses.pre_emphasis_filter) + losses.dc_loss(target_y, predicted_y)

# construct model
model = Model(model_loss, optimizer=keras.optimizers.Adam(learning_rate=5.0e-4))
# model.model.add(keras.layers.InputLayer(input_shape=(None, NUM_INPUTS)))
# model.model.add(keras.layers.TimeDistributed(keras.layers.Dense(4, activation='tanh')))
# model.model.add(keras.layers.GRU(units=8, return_sequences=True))
# model.model.add(keras.layers.Dense(1))
model.load_model(model_file)
model.load_history(model_hist)

model.model.summary()

# %%
model.train(1000, IN_train, OUT_train, IN_val, OUT_val, save_model=model_file, save_hist=model_hist)

# %%
# plot metrics
plt.figure()
model.plot_loss()

# plt.figure()
# model.plot_error()

print(len(model.train_loss))
print(model.train_loss[-1])
print(model.val_loss[-1])

# %%
# Test prediction
idx = 42
predictions = model.model.predict(IN_train[idx].reshape(1, NUM_SAMPLES, NUM_INPUTS)).flatten()

# Plot the predictions along with the test data
plt.clf()
plt.title('Training data predicted vs actual values')
plt.plot(OUT_train[idx], 'c', label='Actual')
plt.plot(predictions, 'r--', label='Predicted')
plt.legend()
plt.xlim(0, 3000)
plt.xlabel('Time [samples]')

# %%
fs = 1.0 / IN_train[idx,0,-1]
freqs, pred_fft = plot_fft(predictions, fs)
freqs, target_fft = plot_fft(OUT_train[idx].flatten(), fs)

# Plot the predictions along with to the test data
plt.clf()
plt.title('Training data predicted vs actual values')
plt.semilogx(freqs, target_fft, 'b', label='Actual')
plt.semilogx(freqs, pred_fft, 'r--', label='Predicted')
plt.legend()
plt.xlim(50, 20000)
plt.ylim(-5)
plt.xlabel('Frequency [Hz]')
plt.ylabel('Magnitude [dB]')

# %%
print(losses.esr_loss(OUT_val, model.model.predict(IN_val)))

# %%
print(losses.esr_loss(OUT_train, model.model.predict(IN_train)))

# %%
model.save_model(model_file)
model.save_history(model_hist)

# %%
