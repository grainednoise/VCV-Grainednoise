# VCV Modules

At the moment there are just the two modules, barely justifying the plural 's', but still.  

## WARNING

This is alpha-quality code, in particular given how untested this code is on anything other than my own desktop Windows 10 machine. Suggestions to improve this situation are welcomed, in particular suggestions that don't require me to do a ton of work or invest a lot of money.

Feedback in general is also appreciated.

## Berlin

The module can be described as a _rough_ approximation of a Perlin noise
generator where you get to choose the X and Y coordinates of a landscape to
explore. Except, it's not a flat landscape but a _torus_, i.e. it wraps around.
As the landscape is 1.0 wide and 1.0 high, that means that an input voltages of,
for instance, 0.27V, 1.27V, 9.27V or -3.73V will result in the _exact_ same
output.

The noise landscape is displayed at the top of the module.

![Berlin module](/images/berlin-headshot.png)

### Inputs, outputs, controls

Inputs:

* `X-Coor` the X-coordinate
* `Y-Coor` the Y-coordinate

Outputs:

* `Out` value ranging from roughly 0.0 - 1.0. The nature of the interpolation adds a little ripple to the output, but it should be < 0.1.  

Controls:

* `X-Scale` a simple scaling constant for  `X-Coor`
* `Y-Scale` a simple scaling constant for  `Y-Coor`
* `X-Size` the size of the random landscape in the X-direction. It can range from 2-16.
* `Y-Size` the size of the random landscape in the Y-direction. It can range from 2-16.
* `Seed` (_plus button_) increases the seed number of the random number generator by 1.
* `Seed` (_text field_) allows you to edit the seed number as text



### Example Patches

#### Warble

Two LFOs are connected to the `X-coor` and `Y-coor` inputs respectively. Both
`X-scale` and `Y-scale` are kept low, leading to a signal at the output that is
random, but with some measure of periodicity. Feeding this output to the delay
control of the Delay module (base delay is a few tens of milliseconds) via an
attenuator leads a clear warble of whatever is routed through said delay.

The _depth_ of the warble is controlled by the attenuator, whereas the _nature_
(speed, how periodic or random) is determined by the LFOs and the Berlin
module. Experiment.


![Warble patch](/images/berlin-warble-ss.png)

#### Engine noise

Like in the previous patch, two LFOs drive this, but now Berlin's output is directly used as audio. One of the LFOs is set to about 20Hz, the other one can be kept much lower. A filter is added to control the harsher harmonics. The `X-scale` knob (if the 20Hz LFO is connected to `X-coor`) now is used to control the harmonic content of the output, and the other LFO together with `Y-scale` make the waveform be either less or more regular. You can vary this patch by connecting one of the LFOs to the filter CV.

![Warble patch](/images/berlin-engine-ss.png)

## Remarks

The **implementation** is pretty straightforward: it's no more than an X by Y
two-dimensional array of random numbers, and the X and Y input interpolate over
this 'random landscape' using approximated order 2 [Lanczos
resampling](https://en.wikipedia.org/wiki/Lanczos_resampling). Having said that,
finding an approximation to the Lanczos 2 kernel proved to be somewhat
time-consuming, and the results are far from perfect. I feel I might want to
revisit this in future, armed with a bit more maths.

I've also implemented a **random number generator**. No, don't worry, I've not
gone mad, nor have I re-invented the wheel, I've just used
[this tiny alorithm](https://en.wikipedia.org/wiki/Linear_congruential_generator)
as to allow me to have full and exclusive control over the state of the RNG.
This allows me to reconstruct the full state of the random terrain from just a
single number. Predictable random numbers, how's that for an oxymoron?


**CPU usage** is somewhat higher than I would like it to be, but it's lower
than Vacuumba (a nice sounding tube-like distortion generator). There already
has been some fairly hefty optimization going on to get it to this point. More
_is_ possible, but at the expense of code clarity and memory usage, so I'm
calling it 'good enough' for now.


## Nevada

**WARNING**: For some settings of the controls, the module can produce a
surprising amount of high-frequency signal. Listening to those sounds at high
levels, especially on headphones, is **not good for your hearing**. It might even
blow your tweeters if you're using speakers. Please be careful.

![Nevada module](/images/nevada-headshot.png)

Inspired by the sound of those old
[Geiger–Müller counters](https://en.wikipedia.org/wiki/Geiger_counter), this
module definitely can produce _that_ particular sound, but it doesn't end there,
as it is able to produce all kinds of noisy, eh, well, noises. None of them are
all that pretty. You can get get sawtooth-like tones out of it, but they'll
always be somewhat glitchy and irregular.


### Inputs, outputs, controls

Inputs / controls:

* `RAD` (or _rand_, rather) is a random trigger. The higher this control is set,
  the more likely the trigger is to occur during any unit of time. When turned
  all the way down it will result in no output.
* `Shield` counteracts the randomness of the `RAD` control somewhat, suppressing
  the trigger when the output is high, and doing the opposite when it's low.
  High values lead to outputs that resemble a normal ocsillator, although it
  never result in fully predictable periods.
* `Decay` determines how quickly the output goes down again after a trigger.
  Higher values both add more high frequencies in general, and determine the
  average frequency of the signal in general if `shield` is used.
* `Energy` how much of an impuls a trigger provides. When turned all the way
  down it will result in no output.
* `Shape` how quickly the energy imparted by a trigger is transfered to the
  output. It acts somewhat like a low-pass filter .
* `Saturate` sets the internal saturation level. When turned all the way down
  it will result in no output.

Outputs:

* `Radiation symbol`  the noisy stuff comes out here.


##### Notes

The controls are nothing if not finicky, mostly because there's so much
interaction between them. I wouldn't go so far as to claim this is _by design_,
but I do embrace the nature of this stochastic module. It gives rise to a range
of strange noise tones, like white and coloured noise, scratchy vinyl, analog
sounding bass tones, screeches, ripping cloth, bad electrical contacts and more.
There are lots of control settings where there will be no output. In particular,
`rad`, `energy` and `saturate` all result in no output when turned all the way
down. Now, that _is_ by design.

Given how easy it is for the module to saturate at some level, I've decided to go use AC coupling for the output. For some inputs and input ranges, this will not prevent a fair amount of that input making its way to the output.


Also be aware that connecting inputs can have an effect on how the control
works. In particular, the `rad`, `energy` and `saturation` inputs are
multiplicative with their respecive controls, and that will kill any output if
you, for instance, connect an envelope generator to them. Just trigger the
envelope generator to make it work.


### Example Patches

#### Geiger counter

Turn `shield` all the way down, set `decay` from 12 to 4 o'clock, and `shape`
from 3 o'clock to maximum. `Energy` and `saturate` are not very important, just
put them at 12 o'clock. Now use the `rad` control or input to vary the pulses
per second from none to too much. Note that the module easily saturates as `rad`
goes up, but where exactly depends for a large part on `decay` as well as other
controls. Note that setting both `decay` and `shape` to maximum can lead to
uncomforatble levels of treble output. You _did_ read the warning above, didn't
you? Good.

#### Talking drum

A single envelope generator drives both the `rad` and `shape` inputs. The
velocity output from `MIDI-CV` (i.e. your keyboard) can bee connected to the
`energy` input. Route Nevada's output through a format filter, and control the
filter by connecting the mod-wheel from `MIDI-CV` to it. Tweak the controls to
your liking, you can use the screenshot as a jumping off point. 

Keep in mind that way the module works can result in a lot of envelope voltage getting through to the output, but in this patch that works to our advantage adding an extra transient.

The velocity gives this patch some extra dynamics, but it would work better if
it would be routed through an exponentiator.

![Talking drum patch](/images/nevada-talking-drum.png)


# Getting it running

The code is developed and tested under Windows 10, using MinGW64. With one
exception, there is nothing specific to that platform, or even the processor
it's running on, so it should 'just work' on other platforms as well. That one
exception is a link flag in the `Makefile` that was needed to find the OpenGL
libraries in Windows.


# Licence

The source code is released under the [MIT licence](LICENCE.txt).

# Credits
The Berlin UI uses a font called [SchoonSquare](https://www.dafont.com/schoonsquare.font).

The Nevada UI uses two fonts: [Atomic Age](www.sorkintype.com) and  [ShareTech](www.carrois.com).

All fonts are available available under the [SIL Open Font Licence](http://scripts.sil.org/OFL)