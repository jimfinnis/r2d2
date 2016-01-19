# r2d2
Some code inspired by a facebook conversation - a hormonal robot which uses R2D2-esque beeps to indicate its status. See the "notes" textfile for more information.

## Synth system
This is a jackd client implementing a blocking monosynth (since R2 is monophonic). It works by using a cmd() function in the main thread
to send commands to the process thread. This causes the process thread to lock a mutex (using a nonblocking trylock), which keeps the lock
until the command (i.e. the sound) has completed. The cmd() function needs this mutex, and so will block or return false if a command is running.

The projected usage here is that the main program will send UDP messages to the synth, which it will ignore if it's currently making a sound, or queue up - it may be necessary
for complex beep sequences to be made up of multiple queued commands.

Currently the only command uses an exponential envelope to go "bing".
