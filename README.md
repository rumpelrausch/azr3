# AZR3

## What's this?
AZR3 is a VST instrument.  
It resembles a tonewheel organ including vibrato, distortion effect and a rotating speaker simulation.

It's windows only (for the time being), but you're free to clone this and build your own VSTi.

## Isn't it pretty old?
Oh, yes, most definitely:  
I started developing it in 2000 and did the last update in 2005.

This is the first version available as 64 and 32 bit plugin. It's based upon
the latest VST SDK with major version 2 (2.4). Though VST SDK 2 is deprecated
for years now, most DAWs support this industry standard.

## Why no VST3 version?
One step after the other.  
I'm glad the port from 2.2 to 2.4 did succeed, and I did a lot of refactoring
in order to not look completely dinosaur when publishing the code today.

Anyway, I still like the simplicity VST 2 implements. It's not too hard to
understand, and it's footprint is smaller. My personal opinion: SDK 2 is
a much easier starting point for VST plugin creation, which itself is quite
a good kickoff into DSP stuff.

## Any special features?
The instrument itself is pretty straight forward, but it comes with some
non-obvious features:

- The two upper registers respond to midi channel 1 and 2, the pedal section to
channel 3. That's hard-coded.
- A switchable option enables speed selection for the rotating speaker simulation by using the sustain pedal.
- The plugin offers two audio inputs which are routed into the effects section.
You could use the AZR3 effects on other instruments if you get your DAW to
perform the correct routing (and probably disable midi input).

## How to compile?
I used "Microsoft Visual Studio Community 2019.
- The project expects the VST SDK 2.4 to exist at `..\vstsdk`.
If you obtained a license from Steinberg when it was still available
you can even use it legally, otherwise you will have to fetch the
sdk from *somewhere else* and break the Steinberg license rules.  
Yes, I got a license some 20 years ago (some years after the
rectification of the vuldronaii, but that's a different story).

- The current configuration creates a statically linked DLL.  
This means you need a local installation of the "C++-MFC"
matching your current toolchain version. It's available via
the Visual Studio Installer.

### Will it compile with other SDK versions?
- SDK 3  
No way without severe reconstruction.
- SDK 2 < 2.4
Would need refactoring: With 2.4 Steinberg changed the signatures of some of
the functions called by the host.
This is not detected automatically; The project would
probably build without exploding, but some of the functions would not
be called by the host.

## License?
No specific one, just these rules:
- Ask me if you plan to sell it.
- Please consider a donation as mentioned within the running plugin.
