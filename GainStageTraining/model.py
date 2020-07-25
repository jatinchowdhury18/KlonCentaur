import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
import time
import json
from json import JSONEncoder

class NumpyArrayEncoder(JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return JSONEncoder.default(self, obj)

class Model():
    def __init__(self, loss_func, optimizer=keras.optimizers.Adam()):
        self.model = keras.Sequential()
        self.opt = optimizer
        self.loss_func = loss_func

        self.train_loss = []
        self.train_err = []
        self.val_loss = []
        self.val_err = []

    def run_training(self, in_train, out_train, epoch_loss, epoch_error, N_skip, N_block):
        N_samples = in_train.shape[1]
        self.model.reset_states() # clear existing state
        self.model(in_train[:, :N_skip, :]) # process some samples to build up state

        # iterate over blocks
        for n in range(N_skip, N_samples-N_block, N_block):
            # compute loss
            with tf.GradientTape() as tape:
                y_pred = self.model(in_train[:, n:n+N_block, :])
                loss = self.loss_func(out_train[:, n:n+N_block, :], y_pred)

            # apply gradients
            grads = tape.gradient(loss, self.model.trainable_variables)
            self.opt.apply_gradients(zip(grads, self.model.trainable_variables))

            # update training metrics
            epoch_loss.update_state(loss)
            epoch_error.update_state(out_train[:, n:n+N_block, :], y_pred)

    def run_validation(self, in_val, out_val, val_loss, val_error):
        self.model.reset_states()
        y_val = self.model(in_val)
        loss = self.loss_func(out_val, y_val)
        val_loss.update_state(loss)
        val_error.update_state(out_val, y_val)

    def run_epoch(self, epoch, in_train, out_train, in_val=None, out_val=None, N_skip=1000, N_block=2048):
        start = time.perf_counter()
        epoch_loss_avg = keras.metrics.Mean()
        epoch_error = keras.metrics.MeanSquaredError()
        val_loss_avg = keras.metrics.Mean()
        val_error = keras.metrics.MeanSquaredError()

        self.run_training(in_train, out_train, epoch_loss_avg, epoch_error, N_skip, N_block)
        if in_val is not None:
            self.run_validation(in_val, out_val, val_loss_avg, val_error)
            
        # end epoch
        self.train_loss.append(epoch_loss_avg.result())
        self.train_err.append(epoch_error.result())
        self.val_loss.append(val_loss_avg.result())
        self.val_err.append(val_error.result())
        dur = time.perf_counter() - start

        print("Epoch {:03d} - Time: {:.4f}s, Loss: {:.4f}, Error: {:.3%}, Val_Loss: {:.4f}, Val_Error: {:.3%}".format(
            epoch+1, dur, epoch_loss_avg.result(), epoch_error.result(), val_loss_avg.result(), val_error.result()))

    def train_until(self, loss_stop, in_train, out_train, in_val=None, out_val=None, N_skip=1000, N_block=2048):
        epoch = 0
        while True:
            self.run_epoch(epoch, in_train, out_train, in_val, out_val, N_skip, N_block)

            if self.train_loss[-1] < loss_stop:
                break

            epoch += 1

    def train(self, num_epochs, in_train, out_train, in_val=None, out_val=None, N_skip=1000, N_block=2048, save_model=None, save_hist=None):
        for epoch in range(num_epochs):
            self.run_epoch(epoch, in_train, out_train, in_val, out_val, N_skip, N_block)

            if save_model is not None:
                self.save_model(save_model)
            if save_hist is not None:
                self.save_history(save_hist)
        
        print("DONE!")


    def plot_loss(self):
        epochs = range(1, len(self.train_loss) + 1)
        plt.plot(epochs, self.train_loss, 'g.', label='Training Loss')
        plt.plot(epochs, self.val_loss, 'b', label='Validation Loss')
        plt.title('Training and validation Loss')
        plt.xlabel('Epochs')
        plt.ylabel('Loss')
        plt.legend()

    def plot_error(self):
        epochs = range(1, len(self.train_loss) + 1)
        plt.plot(epochs, self.train_err, 'g.', label='Training Error')
        plt.plot(epochs, self.val_err, 'b', label='Validation Error')
        plt.title('Training and validation Error')
        plt.xlabel('Epochs')
        plt.ylabel('Error')
        plt.legend()

    def save_model_json(self):
        def get_layer_type(layer):
            if isinstance(layer, keras.layers.TimeDistributed):
                return 'time-distributed-dense'

            if isinstance(layer, keras.layers.GRU):
                return 'gru'

            if isinstance(layer, keras.layers.Dense):
                return 'dense'

            return 'unknown'

        def save_layer(layer):
            layer_dict = {
                "type"    : get_layer_type(layer),
                "shape"   : layer.output_shape,
                "weights" : layer.get_weights()
            }

            return layer_dict


        model_dict = {}
        model_dict["in_shape"] = self.model.input_shape
        layers = []
        for layer in self.model.layers:
            layer_dict = save_layer(layer)
            layers.append(layer_dict)

        model_dict["layers"] = layers
        return model_dict

    def save_model(self, filename):
        model_dict = self.save_model_json()
        with open(filename, 'w') as outfile:
            json.dump(model_dict, outfile, cls=NumpyArrayEncoder)

    def load_model_json(self, json):
        in_shape = json["in_shape"][1:]
        self.model.add(keras.layers.InputLayer(input_shape=in_shape))

        for layer in json["layers"]:
            print(in_shape)
            weights = layer["weights"]
            np_weights = []
            for w in weights:
                np_weights.append(np.array(w))

            if layer["type"] == 'time-distributed-dense':
                d_layer = keras.layers.Dense(layer["shape"][-1], activation='tanh')
                d_layer.build(input_shape=in_shape)
                d_layer.set_weights(np_weights)
                m_layer = keras.layers.TimeDistributed(d_layer)

            elif layer["type"] == 'gru':
                m_layer = keras.layers.GRU(units=layer["shape"][-1], return_sequences=True)
                m_layer.build(input_shape=(None, None, in_shape[-1]))
                m_layer.set_weights(np_weights)

            elif layer["type"] == 'dense':
                m_layer = keras.layers.Dense(layer["shape"][-1])
                m_layer.build(input_shape=in_shape)
                m_layer.set_weights(np_weights)

            else:
                continue
            
            in_shape = tuple(layer["shape"])
            self.model.add(m_layer)
    
    def load_model(self, filename):
        with open(filename, 'r') as json_file:
            model_json = json.load(json_file)
        self.load_model_json(model_json)

    def save_history(self, filename):
        history = np.array([self.train_loss, self.train_err, self.val_loss, self.val_err])
        np.savetxt(filename, history)

    def load_history(self, filename):
        history = np.loadtxt(filename)
        self.train_loss = history[0].tolist()
        self.train_err = history[1].tolist()
        self.val_loss = history[2].tolist()
        self.val_err = history[3].tolist()
