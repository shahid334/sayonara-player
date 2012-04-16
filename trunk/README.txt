
Sayonara


Libraries

    - Qt >= 4.7: Gui, Core, Phonon, Xml, Sqlite http://qt.nokia.com/products/
    - libcurl http://curl.haxx.se/libcurl/
    - Taglib http://developer.kde.org/~wheeler/taglib.html
    - Gstreamer development files, GStreamer plugins 

1. Linux

    Due to the plugin-based design there are several makefiles now: Makefile, Makefile.sayonara, Makefile.gstreamer, Makefile.phonon. Makefile is just a wrapper for the three smaller ones. This wrapper Makefile also builds its dependency-Makefiles out of the Qt project files automatically (Sayonara.pro, Gstreamer.pro, Phonon.pro). Gstreamer is mandatory and will be linked static to sayonara while Phonon is build as a complete optional plugin which brings its own installation routine. 

    For building just Sayonara type 
      make sayonara

    For building the Phonon backend 
      make phonon

    For building the Gstreamer backend 
      make gst

    For building everything 
      make (all)

    Installing sayonara is done by 
      make install

    Installing the phonon plugin is done by 
      make phonon-install

    Do not use qmake anymore!!! If you have used qmake, you can restore the Makefile by renaming the Makefile.bak 



2. Windows

On Windows you have to set environment variables: There is a Visual Studio 2008 solution, but not updated every time, so maybe the project will miss a few files

    Qt: QTDIR
    Taglib: LIBTAG
    libcurl: CURL 

    Known issues:
        Some MP3 files are not played due to some Phonon issue (the standard Windows DirectX backend fails, so the next step will contain compilation of an alternative backend e.g. gstreamer under Windows) 



3. MAC

Same as for Linux, try to fetch all packages with MacPorts
