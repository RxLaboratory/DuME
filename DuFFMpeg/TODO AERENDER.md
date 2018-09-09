# in ffmpeg.cpp

- [ ] In encodeNextItem(), if is AE, launch AE render (ignore if ae not installed)  
    set output to temp EXR (done)  
    set output path (done)  
    launch audio process too  
    if ae render queue, just render project
- [ ] In finishedAE()  
    update currentitem to be the rendered frames, set as non ae and relaunch  
    or if render queue, go to next item
- [X] In readyReadAE() process aerender output
- [ ] Implement multithreading (keep a 'QList<QProcess \*>')

# AE Installation

- [ ] Find the best way to install EXR output module

# in settingswidget.cpp

- [X] List installed AE (dropdown) with option for 'always use latest'
- [X] Custom aerender.exe path
- [X] Temp folder for ae render

# in main.cpp

- [X] Autodetect AE installation (latest) if set to always latest, or specified version

# Output widget

- [ ] No copy stream with aep
- [ ] Disable everything if use ae render queue is checked

# Input Widget

- [ ] add "Use Ae Render Queue" option

# Mainwindow

- [ ] update stop button to stop aerender
