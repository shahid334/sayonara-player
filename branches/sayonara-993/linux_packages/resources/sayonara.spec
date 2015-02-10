%global svnrev  r992

Name:           sayonara
Version:        0.6.1
Release:        1.5.svn992%{?dist}
Summary:        A lightweight Qt Audio player

License:        GPLv3
URL:            http://code.google.com/p/sayonara-player/
# download instructions
# svn co -r992 http://sayonara-player.googlecode.com/svn/trunk/ sayonara-player-r992
# tar cfz sayonara-player-r992.tar.gz sayonara-player-r992
Source0:        sayonara-player-%{svnrev}.tar.gz
#Patch0:         sayonara-desktop.patch
#Patch1:         sayonara-libdir.patch

BuildRequires:  cmake
BuildRequires:  desktop-file-utils
BuildRequires:  qt-devel
BuildRequires:  gstreamer-devel
BuildRequires:  gstreamer-plugins-base-devel
BuildRequires:  libnotify-devel
Buildrequires:  curl-devel
BuildRequires:  taglib-devel
Requires:       svn

%description
%{name} is a small, clear, not yet platform-independent music player. Low 
CPU usage, low memory consumption and no long loading times are only three 
benefits of this player. Sayonara should be easy and intuitive to use and 
therefore it should be able to compete with the most popular music players.

%prep
%setup -q -n %{name}-player-%{svnrev}
%patch0 -p1
%patch1 -p1

# enabled debuging information due rpmlint error message debuginfo-without-sources
sed -i \
    -e 's|SET(CMAKE_BUILD_TYPE "Release")|#SET(CMAKE_BUILD_TYPE "Release")|'     \
    -e 's|SET(CMAKE_C_FLAGS "-Wall -O2 -s")|#SET(CMAKE_C_FLAGS "-Wall -O2 -s")|' \
    -e 's|#SET(CMAKE_BUILD_TYPE "Debug")|SET(CMAKE_BUILD_TYPE "Debug")|'         \
src/CMakeLists.txt

# correct rpmlint wrong-file-end-of-line-encoding warning
perl -i -pe 's/\r\n/\n/gs' README.txt

# correct svn path
perl -i -pe 's/svn/\/bin\/svn/g' scripts/setversion.sh

# set correct permission due rpmlint warning W: spurious-executable-permect
find -type f -exec chmod a-x {} \;

%build
%cmake
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}


for file in %{buildroot}/%{_datadir}/applications/*.desktop; do
    desktop-file-validate $file
done

# remove menu dir, because it's not necessary
rm -rf %{buildroot}/%{_datadir}/menu

find %{buildroot} -type f -o -type l|sort|sed ' 
s:'"%{buildroot}"':: 
s:\(.*/lang/\)\([^/_]\+\)\(.*\.qm$\):%lang(\2) \1\2\3: 
s:^\([^%].*\):: 
/^$/d' > %{name}.lang

#find_lang %{name}

%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
/usr/bin/update-desktop-database &> /dev/null || :

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi
/usr/bin/update-desktop-database &> /dev/null || :

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :


#files -f %{name}.lang
%files
%doc MANUAL README.txt VERSION
%{_bindir}/%{name}
%{_libdir}/%{name}
%{_datadir}/%{name}/
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}*
%{_datadir}/icons/%{name}*
%{_datadir}/icons/hicolor/*/*/*.png


%changelog
* Mon Feb 09 2015 Martin Gansser <martinkg@fedoraproject.org> - 0.4.1-1.5.svn992
- rebuild for new svn release

* Mon Sep 01 2014 Martin Gansser <martinkg@fedoraproject.org> - 0.4.1-1.4.svn878
- enabled debugging informations
- rebuild for new svn release
- set correct file permisson

* Fri Aug 29 2014 Martin Gansser <martinkg@fedoraproject.org> - 0.4.1-1.3.svn870
- rebuild for new svn release
- added more comments

* Tue Jun 10 2014 Martin Gansser <martinkg@fedoraproject.org> - 0.4.0-1.2.svn851
- removed unecessary BR glib2-devel
- removed unecessary BR alsa-lib-devel
- removed unecessary BR libxml2-devel

* Tue Jun 10 2014 Martin Gansser <martinkg@fedoraproject.org> - 0.4.0-1.2.svn851
- rebuild for new svn release
- added svn Requirement
- corrected svn path

* Mon Jun 09 2014 Martin Gansser <martinkg@fedoraproject.org> - 0.4.0-1.1.svn850
- added download instructions
- rebuild for new svn release

* Tue Oct 29 2013 Brendan Jones <brendan.jones.it@gmail.com> - 0.4.0-1.0.svn695
- Inital release.
