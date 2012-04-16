all: gst sayonara
	
install: sayonara
	make -f Makefile.sayonara install

sayonara: gst Makefile.sayonara
	make -f Makefile.sayonara

sayonara_clean: Makefile.sayonara
	make -f Makefile.sayonara clean


gst: Makefile.gstreamer
	make -f Makefile.gstreamer

gst_clean:
	make -f Makefile.gstreamer clean

phonon: Makefile.phonon
	make -f Makefile.phonon

phonon-install: phonon
	make -f Makefile.phonon install

phonon_clean: Makefile.phonon
	make -f Makefile.phonon clean


Makefile.sayonara: Sayonara.pro
	qmake Sayonara.pro -o Makefile.sayonara

Makefile.gstreamer: Gstreamer.pro
	qmake Gstreamer.pro -o Makefile.gstreamer

Makefile.phonon: Phonon.pro
	qmake Phonon.pro -o Makefile.phonon

clean: sayonara_clean phonon_clean gst_clean
	rm -rf Makefile.*



	
