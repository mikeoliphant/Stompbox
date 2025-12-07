<img width="400" alt="Image" src="https://github.com/user-attachments/assets/4e4ae15e-2f02-4046-99bd-34736ca5a4b1" />

Stompbox is a guitar amplification and effects application, arranged as a digital version of a guitar pedalboard.

It can be run as a Windows VST plugin, or as stanalone headless [JACK Audio](https://github.com/jackaudio) client with a remote-controlled user interface. 

I run it as both a VST3 plugin in a DAW on my PC, and (mostly) headless on my Raspberry Pi pedalboard.

This is what the VST and Windows/Linux remote interface looks like:

![stompbox](https://github.com/mikeoliphant/StompboxUI/assets/6710799/dd6e9349-ff0d-4437-af42-ef62f1096496)

This is what it looks like running as an Android remote:

![StompboxAndroid](https://github.com/mikeoliphant/StompboxUI/assets/6710799/3189e769-a28c-4e3b-8629-6846fb32de6c)

Here is a video of it running on my pedalboard:

[![Neural Amp Modeler on Raspberry Pi](https://img.youtube.com/vi/2I_bxxzQs2s/0.jpg)](https://www.youtube.com/watch?v=2I_bxxzQs2s)

This is the hardware I'm using:
- Raspberry Pi 4
- Hotone Jogg audio interface
- Hotone Ampero Control MIDI Controller
- Wio Terminal (used for a display using my [SerialTFT](https://github.com/mikeoliphant/SerialTFT) app)

# Features

* Headless operation with a separate UI app that can be run remotely or locally
* MIDI support for switching presets, toggling effects
* [Neural Amp Modeler](https://github.com/sdatkinson/neural-amp-modeler) and [RTNeural](https://github.com/jatinchowdhury18/RTNeural) model support
* Cabinet impulse responses
* Tonestack/EQ
* Noise Gate
* Drive (clean boost, tube screamer)
* Time-based effects
  - Delay  
  - Reverb (algorithmic and convolution)
  - Compressor
  - Chorus
  - Phaser
  - Flanger
  - Tremolo
  - Vibrato
  - Wah/Auto-Wah
* Tuner
* Audio file player

# Running as a Windows VST

To run it as a VST3, you can simply download it from [the releases section of this repo](https://github.com/mikeoliphant/Stompbox/releases/latest).

Simply extract the .zip file and copy the resulting folder to "C:\Program Files\Common Files\VST3".

## VST user data location

Files, such as NAM models, impulse responses, etc. go in your local user roaming AppData folder. Go to "%APPDATA%" in file explorer, and you should see a "stompbox" folder after the VST has been loaded at least once.

# Building

First clone the repository:
```bash
git clone --recurse-submodules -j4 https://github.com/mikeoliphant/Stompbox
```

## Building on Windows

```bash
cmake.exe -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config=release -j4
```

Note - you'll have to change the Visual Studio version if you are using a different one.

If you want to use Jack audio, you need to have Jack for windows installed before building:

https://jackaudio.org/downloads/

## Building on Liunux/MacOS

To build the stombox-jack client, before building you need to have the jack development libraries installed:

```bash
sudo apt-get install libjack-jackd2-dev
```

Build with:

```bash
cd stompbox/build
cmake .. -DCMAKE_BUILD_TYPE="Release"
make
```

## Building the remote app on Linux/MacOS

To build StompboxRemoteGL on Linux, do the following:

Build StompboxImageProcessor:
```
dotnet build -c Release StompboxImageProcessor
```

Run it:
```
StompboxImageProcessor/bin/Release/net8.0/StompboxImageProcessor
```

Build StompboxRemoteGL:
```
dotnet build -c Release StompboxRemoteGL
```

and your executable will be: ```StompboxRemoteGL/bin/Release/net8.0/StompboxRemoteGL```

**Note:** For file browser input to work, you need to have [Zenity](https://help.gnome.org/users/zenity/stable/index.html.en) installed.

# Running headless with stompbox-jack

After building, the binary will be in build/stompbox-jack.

You must have [jack](https://github.com/jackaudio) installed and configured to run on your audio device. If jack is running (as the same user you run stompbox with), the running instance will be used, otherwise jack will be started using your default configuration.

stompbox-jack takes a single optional command-line argument - the name of the preset to start with.

Currently it will connect to the first available jack audio input, and the first two available jack audio outputs. It will also connect to any available jack midi inputs.

The following folders are epected to exist in the same folder as the stompbox-jack executable is run from:

**Presets** - folder to store configuration presets

**NAM** - folder containing .nam amp models

**Cabinets** - folder containing .wav format cabinet impulse responses

**Reverb** - folder containing .wav format convolution reverb files

**Music** - folder containing .wav audio files for the built-in audio looper

## MIDI Support

If you have MIDI enabled in jack, MIDI devices can be used to control stompbox behavior.

Via the remote user interface, you can configure ("..." -> "MIDI CC Map" in the UI) any MIDI CC to control whether an effect is enabled, or to control any effect parameter.

In addition, if you send a MIDI patch change event, stompbox will attempt to load a preset starting with the patch number (ie: MIDI patch change to 2 would load "02MyCoolPreset").

Some more complicated MIDI stomp-based UI is also available if you are running the serial display interface - it gives interactive access to patch changes, tuner, stomps, etc.

# Using a microcontroller dispay

Stompbox has built-in support for talking to a USB-connected microcontroller with display. I'm using a [Wio Terminal ](https://www.seeedstudio.com/Wio-Terminal-p-4509.html), but any Arduino-compatible microcontroller with an SPI display should work. It requires installing my [SerialTFT](https://github.com/mikeoliphant/SerialTFT) Arduino sketch on the microcontroller.


