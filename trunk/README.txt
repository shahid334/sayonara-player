
Sayonara


Libraries

    - Qt >= 4.6: Gui, Core, Xml, Sqlite http://qt.nokia.com/products/
    - libcurl http://curl.haxx.se/libcurl/
    - Taglib http://developer.kde.org/~wheeler/taglib.html
    - Gstreamer development files, GStreamer plugins 

1. Linux

	You need 
	* g++ >= 4.8 (important due to C++x11 standard)
	* cmake
	* Qt >= 4.6 + development files (qt4-core, qt4-gui-, qt4-xml, qt4-sql, qt4-sqlite)
	* libqt4 (on some distributions it's not installed by default)
	* libtaglib
	* Gstreamer 1.0 + development files (libgstreamer1.0, libgstreamer-plugins-base1.0)
	  (will run and compile with gstreamer 0.10, too. In order to do so, install the
	   corresponding gstreamer 0.10 packages)
	* libcurl

	* OPTIONAL: For notifications: libnotify + development files

	= Build =

	* cmake .
	* make
	* make install (as root)
	* sayonara
	
	1.1 Debian/Ubuntu/Mint: apt-get
	
		* sudo apt-get install libqt4-dev libqt4-sql-sqlite libqt4-sql libqt4-gui libqt4-sql libqt4-core libtag1-dev libcurl4-gnutls-dev \
                  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-good1.0-dev cmake

		* for notifications
		  sudo apt-get install libnotify-dev


2. Windows

Not supported.


3. MAC

Same as for Linux, try to fetch all packages with MacPorts
