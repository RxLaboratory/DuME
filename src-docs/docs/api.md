# Controlling DuME

DuME is a stand-alone application, but it can easily be controlled from other applications and scripts.

There are two ways to control *DuME* from outside:

- [**Command line options**](cli-options.md), which can launch *DuME* with specific instructions.
- The [**server API**](server-api.md) provides an easy way to control *DuME* through a socket, from the same computer or over the network.

!!! Hint
    It is not yet possible to completely control every feature of *DuME* and automate everything.

    If you need deep control over the transcoding from another application, or if you'd like a headless version of *DuME*, it would be easier to use *FFmpeg* (and the *After Effects'* `aerender` command) by yourself, as *DuME* does not do a lot more than correctly handling these third-party command-line tools.

    In the future, we hope to provide more control over *DuME*, so it could be used in a completely automated workflow and provide renderfarm-like features. Some other important features (like the render queue) have to be implemented first.
