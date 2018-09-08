# in ffmpeg.cpp

- [ ] In encodeNextItem(), if is AE, launch AE render (ignore if ae not installed)
    set output to temp EXR (done)
    set output path
- [ ] In finishedAE()
    update currentitem to be the rendered frames, set as non ae and relaunch
- [X] In readyReadAE() process aerender output

# AE Installation

- [ ] Find the best way to install EXR output module

# in settingswidget.cpp

- [X] List installed AE (dropdown) with option for 'always use latest'
- [X] Custom aerender.exe path
- [X] Temp folder for ae render

# in main.cpp

- [ ] Autodetect AE installation (latest) if set to always latest, or specified version

# Output widget

- [ ] No copy stream with aep

# Mainwindow

- [ ] update stop button to stop aerender
