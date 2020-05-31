# Klon Centaur Model

[![Build Status](https://travis-ci.com/jatinchowdhury18/KlonCentaur.svg?token=Ub9niJrqG1Br1qaaxp7E&branch=master)](https://travis-ci.com/jatinchowdhury18/KlonCentaur)

This repository contains a digital model of the Klon Centaur 
guitar pedal. The goal is to use this circuit as a test
bench for comparing/combining nodal analysis, wave 
digital filters, and neural network circuit modelling
approaches. The model is implemented as an audio plugin 
(Standalone/VST/AU), and as a guitar pedal-style effect
embedded on a Teensy microcontroller.

For a full technical paper summarizing this project see
[here](https://ccrma.stanford.edu/~jatin/papers/Klon_Model.pdf).
Original circuit schematic and analysis on
[ElectroSmash](https://www.electrosmash.com/klon-centaur-analysis).


This work began as part of a class project for
[EE 292D](https://ee292d.github.io/) at Stanford University.

## Audio Plugin

Download links for the audio plugin will be available soon...

<img src="./Paper/Figures/Plugin.png" width=50%>

### Building from Source

The audio plugin is built using
[JUCE](https://github.com/juce-framework/JUCE),
[FRUT](https://github.com/McMartin/FRUT), CMake, and
[PluginGUIMagic](https://github.com/ffAudio/PluginGUIMagic).
These dependencies are primarily managed by the `setup.sh` script,
except for CMake, which the user must install on their own.

To build from source, use the following steps:
```bash
# clone repository
$ git clone https://github.com/jatinchowdhury18/KlonCentaur.git
$ cd KlonCentaur

# run setup script
$ ./setup.sh

# Build ChowCentaur plugin
$ cd ChowCentaur
$ mkdir build && cd build/
$ cmake ..
$ cmake --build . --config Release
```

## Teensy Pedal

For information on the Teensy pedal-style implementation, see the
[`TeensyCentaur/`](./TeensyCentaur/) subfolder.

## Circuit Modelling

The circuit model is constructed using nodal analysis and wave digital
filters. For more information see:

- Julius Smith, [Physical Audio Signal Processing](https://ccrma.stanford.edu/~jos/pasp/pasp.html)
- Kurt Werner, [Virtual Analog Modelling of Audio Circuitry Using Wave Digital Filters](https://www.semanticscholar.org/paper/Virtual-Analog-Modeling-of-Audio-Circuitry-Using-Werner/4df7106aa5581a607ac88e559a05c71efc73497b)

The wave digital filters are implemented using a WDF library, available
[here](https://github.com/jatinchowdhury18/WaveDigitalFilters).

## Neural Network Modelling

In the neural network version of the emulation, a recurrent neural network
is used to emulate the gain stage circuit of the original pedal. The
RNN architecture used is derived from the one presented by Wright et. al.
in their 2019 DAFx paper ["Real-Time Black-Box Modelling with Recurrent Neural Networks"](http://dafx2019.bcu.ac.uk/papers/DAFx2019_paper_43.pdf).
Training data consists of ~4 minutes of Direct In (DI) recordings of 
electric guitar, chopped into 0.5 second segments. The data is then 
processed through a SPICE model to create a "ground truth" version of the
effect to train against. The training data, SPICE model, and `Python` code
for training the networks can be found in the
[`GainStageML/`](./GainStageML/) subfolder.

## License

This repository is licensed under the 
BSD-3-Clause license. Enjoy!
