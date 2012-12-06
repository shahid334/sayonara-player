
Sayonara


Libraries

    - Qt >= 4.6: Gui, Core, Xml, Sqlite http://qt.nokia.com/products/
    - libcurl http://curl.haxx.se/libcurl/
    - Taglib http://developer.kde.org/~wheeler/taglib.html
    - Gstreamer development files, GStreamer plugins 

1. Linux

	You need 
	* cmake
	* Qt >= 4.6 + development files (qt4-core, qt4-gui-, qt4-xml, qt4-sql, qt4-sqlite)
	* libqtcore4 (on some distributions it's not installed by default)
	* libtaglib
	* Gstreamer 0.10 + development files (libgstreamer0.10, libgstreamer-plugins-base0.10)
	* libcurl

	* OPTIONAL: For notifications: libnotify + development files

	= Build =

	* cmake .
	* make
	* make install (as root)
	* sayonara
	
	1.1 apt-get
	
		* sudo apt-get install libqt4-dev libqtcore4 libtaglib2.0-cil libcurl4-gnutls-dev \
                  libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev cmake

		* for notifications
		  sudo apt-get install libnotify-dev


2. Windows

Not supported.


3. MAC

Same as for Linux, try to fetch all packages with MacPorts
