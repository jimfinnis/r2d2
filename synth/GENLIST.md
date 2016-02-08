# List of generators and their parameters

## common to all
### params
|name|function|default|
|---|---|---|
|amp|amplitude|1|

## sin (SinOsc.h)
Generates a sine wave which can be separately phase, frequency or
amplitude modulates. Both FM and PM inputs are divided by 100.
### params
|name|function|default|
|---|---|---|
|pm|amount of phase mod|0.1|
|fm|amount of frequency mod|0.1|
|freq|frequency (see mode, below)|1|
|mode|fixed or relative, defining how frequency generated:|key|
||if **fixed**, freq=frequency*keyfreq, 
||otherwise freq=frequency
### inputs
|name|function|
|---|---|
|amp|amp modulation|
|pm|phase modulation|
|fm|freq modulation|

        
## wave (WaveTableOsc.*)
Uses a wave table oscillator (modified from
http://www.earlevel.com/main/2012/05/04/a-wavetable-oscillator%E2%80%94part-1/)
to generate saw, square and triangle waves correctly over a wide range
of frequencies.
### params
|name|function|default|
|---|---|---|
|freq|frequency (see mode, below)|1|
|fm|amount of FM|
|type|saw, triangle or square|triangle|
|mode|fixed or relative, defining how frequency generated:|key|
||if **fixed**, freq=frequency*keyfreq,|
||otherwise freq=frequency|
|recalc|FM recalculation interval|256 samples|

### inputs
|name|function|
|---|---|
|amp|amp modulation|
|fm|frequency modulation (calculated every **recalc** samples)
        
## env (Env.h)
Envelope generator, able to store up to 10 points. Before the first
time, the envelope's value is that of the first level; after the last
time, the value is that of the last level. The levels are linearly 
interpolated.
### params
|name|function|default|
|---|---|---|
|tN|time of stage N (0-9)|none|
|lN|level of stage N (0-9)|none|

        
## mix2 (Utils.h)
Simple 2-input mixer.
### params
|name|function|default|
|---|---|---|
|ampa|amplitude of input A|0.5|
|ampb|amplitude of input B|0.5|
### inputs
|name|function|
|---|---|
|a| input A
|b|input B|

## add (Utils.h)
Signal adder
### inputs
|name|function|
|---|---|
|a| input A
|b|input B|
## mul (Utils.h)
Signal multiplier
### inputs
|name|function|
|---|---|
|a| input A
|b|input B|
## const (Utils.h)
Outputs a constant
### params
|name|function|default|
|---|---|---|
|val|value of constant|1|
        
## noise (Noise.h)
White noise generator.
### Inputs
|name|function|
|---|---|
|amp|amplitude|

## perlin (Perlin.h)
Perlin noise generator.
### Params
|name|function|default|
|---|---|---|
|freq|"frequency" (this is a vague concept with perlin noise)|1|
### Inputs
|name|function|
|---|---|
|amp|amplitude|

## lpf (LPF.h)
Resonant low pass filter. Quite slow, and the modulation
only occurs at the control rate (every 1024 samples) because it's so
computationally expensive.
### Params
|name|function|default|
|---|---|---|
freq|cutoff|8000|
q|resonance|1|
gain|filter gain|1|
qmod|amount of Q modulation|0.1|
fmod|amount of freq modulation|1|
### Inputs
|name|function|
|---|---|---|
|in|signal to be filtered|
|qm|resonance modulation|
|fm|frequency modulation|

