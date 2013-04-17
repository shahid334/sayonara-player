Name: sayonara
#Version: TODO get version
#Release: TODO get release
#Summary: TODO get from resources

Group: Applications/Multimedia
License: GPLv3
URL: http://sayonara.luciocarreras.de
Source0: ../../sayonara-blablabla.tar.gz
BuildRoot: ./build

BuildRequires: cmake gcc-c++
BuildRequires: curl-devel
BuildRequires: taglib-devel
BuildRequires: qt4-devel  
BuildRequires: gstreamer-devel gstreamer-plugins-base-devel


%ifarch x86_64
Requires: gstreamer0.10(decoder-audio/x-vorbis)()(64bit)
Requires: gstreamer0.10(decoder-audio/x-flac)()(64bit)
Requires: gstreamer0.10(decoder-audio/x-speex)()(64bit)
Requires: gstreamer0.10(decoder-audio/x-wav)()(64bit)
%else
Requires: gstreamer0.10(decoder-audio/x-vorbis)
Requires: gstreamer0.10(decoder-audio/x-flac)
Requires: gstreamer0.10(decoder-audio/x-speex)
Requires: gstreamer0.10(decoder-audio/x-wav)
%endif

%description
#TODO 
#get from resources

%build
#TODO

%install
#TODO

%clean
#TODO

%files
#TODO

%changelog
#TODO
