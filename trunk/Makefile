all: phonon gst sayonara
	

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

phonon_clean: Makefile.phonon
	make -f Makefile.phonon clean


Makefile.sayonara:
	qmake Sayonara.pro -o Makefile.sayonara



Makefile.gstreamer:
	qmake Gstreamer.pro -o Makefile.gstreamer

Makefile.phonon:
	qmake Phonon.pro -o Makefile.phonon

clean: sayonara_clean phonon_clean gst_clean
	rm -rf Makefile.*



	
