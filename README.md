# r2d2
Some code inspired by a facebook conversation - a hormonal robot which uses R2D2-esque beeps to indicate its status. See the "notes" textfile for more information.

## Synth system
This is a jackd client implementing a blocking monosynth (since R2 is
monophonic).
It works by parsing commands from the user, which typically create
and edit SynthDef objects (synthesizer definitions), which contain 
GenDefs (generator definitions) and the links between them. 
The GenDefs describe things like oscillators, envelope generators etc.
and their parameters.

Issuing a note command causes a Synth (containing Gens) to be created
and added to the note command queue, along with which frequency to play.
When the audio thread has finished playing a note, it pulls a new command
off the end of the queue. Certain gens (typically envelope generators)
are nominated as "done monitors", and these all have to be in the "done"
state for the note to have finished.

The projected usage here is that the main program will send UDP messages to the synth, which it will ignore if it's currently making a sound, or queue up - it may be necessary
for complex beep sequences to be made up of multiple queued commands.

An example of some commands:
```
# start a new synth
newsynth foo

# create a sin osc called sin1 with the default params

g sin sin1

# create a sin osc called sin2 with a freq mult of 1.01

g sin sin2 p freq 1.01

# create an env generator called env1, and make it a "done monitor" - the
# sound is finished when all donemons are complete.
# Envs use a special syntax without the "g" keyword, but their params
# can still be modified with "p" etc.

env env1 0 0, 0.01 1, 0.7 0; done

# create a mixer called "mix" and make it the output, and set the
# amplitudes of its outputs to 0.5

g mix2 mix out p ampa 0.5,ampb 0.5

# link env1 to the amp input of sin1

- env1 sin1 amp

# link env1 to the amp input of sin2

- env1 sin2 amp

# link the sine oscillators to the mixer

- sin1 mix a
- sin2 mix b

# play a note
+ 440;


# this creates a new synth

newsynth bar
g sin sin1 p pm 19,amp 0.1; out
g sin sin2 p freq 1.7 # phase modulator
- sin2 sin1 pm
env e 0 0, 0.01 1, 0.08 0.7, 0.1 0; done
- e sin1 amp
env emod 0 0, 0.02 1, 0.07 0.2, 0.08 0.01, 0.1 0; 
- emod sin2 amp

# selects it and plays notes on it

s bar +743,300,873,674,384,500

# this demonstrates how to modify a synthdef with "m"
# Change the sin1 PM and amp values:
#   m sin1 p pm 20,amp 0.1;
# Change the time of the final stage of the envelope:
#   m e p t3 0.2;
```

