# The server API

!!! Note
    The server API is not yet implemented... Just a little more patience!

The server API will be an easy way to control a running instance of *DuME* from another application, from the same computer or over the network.

Once launched, *DuME* will start listenning on a specific port both on localhost and the network (depending on the settings). Another script or application will then be able to read and write on this port to communicate and control *DuME*, for example to add a job to the queue, launch a transcoding process, etc.

This communication will be done through queries and replies formatted in *JSON*, which will make it easy to implement in any programming language, and use *DuME* anywhere you need it, either in Python, JavaScript, or any other language.

![META](authors:Nicolas "Duduf" Dufresne;license:GNU-FDL;copyright:2019-2021;updated:2021/03/07)