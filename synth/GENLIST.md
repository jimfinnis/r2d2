# List of generators and their parameters

## common to all
### params
|name|function|
|---|---|
|amp|amplitude|

## sin (SinOsc.h)
Generates a sine wave which can be separately phase, frequency or
amplitude modulates. Both FM and PM inputs are divided by 100.
### params
|name|function|
|---|---|
|pm|amount of phase mod|
|fm|amount of frequency mod|
|freq|frequency (see mode, below)|
|mode|fixed or relative, defining how frequency generated:|
||if **fixed**, freq=frequency*keyfreq, 
||otherwise freq=frequency
### inputs
|name|function|
|---|---|
|amp|amp modulation|
pm|phase modulation|
|fm|freq modulation|

        
## wave (WaveTableOsc.*)
Uses a wave table oscillator (modified from
http://www.earlevel.com/main/2012/05/04/a-wavetable-oscillator%E2%80%94part-1/)
to generate saw, square and triangle waves correctly over a wide range
of frequencies.
### params
|name|function|
|---|---|
|freq|frequency (see mode, below)|
|type|saw, triangle or square (default is triangle)|
|mode|fixed or relative, defining how frequency generated:|
||if **fixed**, freq=frequency*keyfreq, 
||otherwise freq=frequency
### inputs
|name|function|
|---|---|
|amp|amp modulation|
        
## env (Env.h)
Envelope generator, able to store up to 10 points. Before the first
time, the envelope's value is that of the first level; after the last
time, the value is that of the last level. The levels are linearly 
interpolated.
### params
|name|function|
|---|---|
|tN|time of stage N (0-9)|
|lN|level of stage N (0-9)|

        
## mix2 (Utils.h)
Simple 2-input mixer.
### params
|name|function|
|---|---|
|ampa|amplitude of input A|
|ampb|amplitude of input B|
### inputs
|name|function|
|---|---|
|a| input A
|b|input B|
        
## noise (Noise.h)
White noise generator.
Has no params or inputs, just generates noise.

