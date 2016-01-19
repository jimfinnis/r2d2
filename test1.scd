(
SynthDef(\beep,{|out= 0, pan= 0, freq= 400, amp= 1, dur= 2, t_trig= 1, gate=1|
	var env = EnvGen.kr(Env.adsr(0.01,0.3,0.7,0.2),gate,amp,doneAction:2);
	var son = SinOsc.ar(freq,mul:env);
	Out.ar(out,son);
}).add;

)

Pbind(\instrument,\beep,\freq,Pgauss(800,500).max(100),\dur,Pgauss(0.1,0.05).max(0.1)).play;