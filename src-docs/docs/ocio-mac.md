# LUT tools and Mac OS

The LUT tools in *DuME* rely on `ociobakelut`, a tool included in [*OpenColorIO*](http://opencolorio.org). For technical reasons on *Mac OS*, *DuME* does not include this tool (yet), and you have to install it on your own.

The easiest way to get it is to use [*Homebrew*](http://brew.sh), and install *OpenColorIO* with the command `brew install opencolorio`.

## Detailed instructions

Here is a simple step-by-step tutorial to help you get *OpenColorIO* on *Mac OS*.

1. Open the *Terminal* application, where you can copy and paste the following commands.
2. Install *XCode* command line tools with `xcode-select --install`
3. Install *Homebrew* with `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"`
4. Install *OpenColorIO* with `brew install opencolorio`

*OpenColorIO* is now available on your system, along with the tools needed by *DuME*.

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)