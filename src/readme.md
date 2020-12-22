# VCV Modules

At the moment there's just the one module, so the plural is a bit premature at
the very least. 

## WARNING

This is alpha-quality code, in particular given how untested this code is on anything other than my own desktop Windows 10 machine. Suggestions to improve this situation are welcomed, in particular suggestions that don't require me to do a ton of work or invest a lot of money.

Feedback in general is also appreciated.

## Berlin

The module can be described as a _rough_ approximation of Perlin noise where
you get to choose the X and Y coordinates of a landscape to explore. Except,
it's not a flat landscape but a _torus_, i.e. it wraps around. As the landscape
is 1.0 wide and 1.0 high, that means that an input voltages of, for instance,
0.27V, 1.27V, 9.27V or -3.73V will result in the _exact_ same output.

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

## Getting it running

The code is developed and tested under Windows 10, using MinGW64. With one
exception, there is nothing specific to that platform, or even the processor
it's running on, so it should 'just work' on other platforms as well. That one
exception is a link flag in the `Makefile` that was needed to find the OpenGL
libraries in Windows.

## Remarks

The **implementation** is pretty straightforward: it's no more than an X by Y
two-dimensional array of random numbers, and the X and Y input interpolate over
this 'random landscape' using approximated order 2 [Lanczos
resampling](https://en.wikipedia.org/wiki/Lanczos_resampling). Having said that, finding an approximation to the Lanczos 2 kernel proved to be somewhat time-consuming, and the results are far from perfect. I feel I might want to revisit this in future, armed with a bit more maths.

I've also implemented a **random number generator**. No, don't worry, I've not gone mad, nor have I re-invented the wheel, I've just used [this tiny alorithm](https://en.wikipedia.org/wiki/Linear_congruential_generator) as to allow me to have full and exclusive control over the state of the RNG. This allows me to reconstruct the full state of the random terrain from just a single number. Predictable random numbers, how's that for an oxymoron?


**CPU usage** is somewhat higher than I would like it to be, but it's lower
than Vacuumba (a nice sounding tube-like distortion generator). There already
has been some fairly hefty optimization going on to get it to this point. More
_is_ possible, but at the expense of code clarity and memory usage, so I'm
calling it 'good enough' for now.

## Credits
The Berlin UI uses a font called [SchoonSquare](https://www.dafont.com/schoonsquare.font) available under the [SIL Open Font Licence](http://scripts.sil.org/OFL).