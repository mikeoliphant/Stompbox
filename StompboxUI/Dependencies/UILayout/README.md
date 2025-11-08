# UILayout

UILayout is a simple, peformant, layout-based, cross-platform user interface library for .NET languages.

# Platforms Supported

UILayout currently supports SkiaSharp and MonoGame as underlying rendering systems. As such, it has the ability to target the same platforms they do - so most platforms are potentially supported.

Tested platforms:
- Windows x64
- Linux x64

# Functionality

<img width="626" height="458" alt="Image" src="https://github.com/user-attachments/assets/f4e99b70-37ac-4496-9bfc-d6a92c869cde" />

UILayout provides the following:

- Layout structure (dock, horizontal/vertical stacks, etc.)
- UI controls (buttons, text blocks, sliders, menus, dialogs, scrollbars, etc.) 
- Image rendering (images, ninepatch images, batching, image atlases)
- Font rendering (skia text rendering, bitmap fonts)
- Mouse/Keyboard input handling
- File dialog support

# Getting Started

Have a look at the included test UI for examples of how to use it:

https://github.com/mikeoliphant/UILayout/blob/master/UILayout.Test/LayoutTest.cs

# Current Status

UILayout is currently under active development and everything is very much subject to change.

That said, it already provides considerable functionality and is being used in several large projects.

Projects using UILayout include:

- [ChartPlayer](https://github.com/mikeoliphant/ChartPlayer): Visual player for song charts synchronized to music recordings.
- [ChartConverter](https://github.com/mikeoliphant/ChartConverter): Convert Rocksmith PSARC and Rock Band files to OpenSongChart format.
- [StompboxUI](https://github.com/mikeoliphant/StompboxUI): Remote GUI and VST3 plugin for Stompbox guitar simulation.
- [FaustVst](https://github.com/mikeoliphant/FaustVst): Load/edit Faust dsp effects in a VST.

