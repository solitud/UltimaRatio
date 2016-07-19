![](http://www.gezeiten-modular.com/img/gezeiten_logo.png)

#Ultima Ratio - Manual
*Ultima Ratio* is an intuitive playable Clock Divider, Clock Generator and Dual AR Envelope.

You can use it

* to synchronize sequencers with independent time signatures
* as a performance-like instrument when you play the divider ratios
* to provide and distribute a stable system clock
* to fire envelopes in a [live performance](https://www.youtube.com/watch?v=JzxdrqcDII0) to rhythmically control filters or VCAs in musically meaningful ratios
* to mult triggers

##Modes
The Ultima Ratio operates in two modi depending on a plug is inserted in the *IN* jack.

###Mode 1: Cascading Divider
If a cable is plugged into the *IN* jack Ultima Ratios Masterclock is set by the incoming signal (Slave mode).
The Ultima Ratio accepts most signals like a square wave or some kind of periodic gate signal.

The signal on *IN* will be multed to the *THRU* jack, great if you are short on trigger mults.

The *CLK1 Knob* now works as Divider 1 which divides the incoming clocks by ratios of {1,2,3,4,6,8,12,16,24,32,48,64,96}.
The divided signal can be obtained at the *CLK1* output.
The divided clock also triggers an Attack-Release Envelope which is present at jack *ENV1*

The *CLK2 Knob* acts similar like the *CLK1 Knob*.
The source signal for Clock Divider 2 can be set by the *Source Switch*. Clock Divider 2 divides either the Masterclock or the already divided signal of Clock Divider 1.

###Mode 2: Clock Generator
If no cable is plugged into the *IN* jack Ultima Ratio works as a Clock Generator.
In this case the *CLK1 Knob* sets the systems Masterclock which can be obtained from the *THRU* jack.
Values are scaled in a practical range to drive sequencers which rely on a MidiClock type of input in typically speeds of ca. 60 - 200 BPM.

The Masterclock can be divided by *CLK2 Knob* and is available on *CLK2* jack.

When you use the Ultima Ratio as a Clock Generator you lose the ability to control Clock Divider 1 independently.
Since Masterclock and Divider 1 share the same knob in this mode, *CLK1 Knob* still sets the ratio of clock Divider 1.
That means, the faster you set the Masterclock tempo the higher is the Clock Divider ratio on Clock Divider 1.

The clock signal on *CLK1* jack is still available; but since it is connected with the tempo of the Masterclock it may be not that useful.
Except you strive for an unpredictable reign of chaos.

##Envelopes
Attack and Release times are set with the *Attack* and *Release* Knobs and will be simultaneous applied to both envelopes *ENV1* and *ENV2*.
The *Envelopes* are optimized for short running times. They are ideally suited to rapidly trigger VCAs and Filters on punchy sequencer lines.

##Reset
If you apply a gate signal to the *RESET* input the next incoming Clock Signal will reset the internal counter of the Ultima Ratio.
Ultima Ratio will always be in sync with the Masterclock, even if a Reset occurs. The Reset signal will just shift the sequence in sync to the start point, e.g. Beat 1.
This might be useful to define a reference point for synchronizing the Ultima Ratio to sequencers.

##CV
You can control the ratio of Divider 1 with Control Voltages.
Depending if the Source Switch is set to Cascade mode this will also influence the Ratio of Divider 2.
If the Ultima Ratio operates in Mode 2, CV also influences the Tempo of the Masterclock.

#Patch Ideas
##Animated Filter
Trigger the filter cutoff with both envelopes and play with the ratio knobs and the *Source Switch*.
Be careful, you might get unwanted wobble bass like Dubstep effects.

##Simple Envelope
Plug a gate signal in to *IN* and use the UR as a simple envelope generator.

##1 to 3 active Clock Mult
Plug in a clock signal in *IN* jack, turn all knobs left, get 3 refreshed clock signals from *THRU*, *CLK1*, and *CLK2*

##Sync Device
Sync sequencers which rely on different clock inputs: If you twist the divider knobs while the sequence plays you will get great syncopic results.

#Specifications
Width: 8 HP | Depth: 36 mm | Current Draw: 61 mA +12V

******
www.gezeiten-modular.com