#!/bin/sh

MSI=OpenRTM-aist-1.0.0-RELEASE_vc9_100212.msi
ProgFile="$HOME/.wine/drive_c/Program Files"
MingwDir="/usr/i586-mingw32msvc"

if [ ! -f $MSI ]; then
wget http://www.openrtm.org/pub/Windows/OpenRTM-aist/cxx/$MSI
fi

#md5sum --check setupwindows.md5
#if [ $? = 0 ]; then
#wget http://www.openrtm.org/pub/Windows/OpenRTM-aist/cxx/$MSI
#fi

if [ ! -d "$ProgFile/OpenRTM-aist" ]; then
wine msiexec /i $MSI
fi

sudo cp -r "$ProgFile/OpenRTM-aist/1.0/rtm" $MingwDir/include

sudo cp -r "$ProgFile/OpenRTM-aist/1.0/coil" $MingwDir/include

sudo cp "$ProgFile/omniORB/4.1.4/include/"*.h $MingwDir/include
sudo cp -r "$ProgFile/omniORB/4.1.4/include/COS" $MingwDir/include
sudo cp -r "$ProgFile/omniORB/4.1.4/include/omniORB4" $MingwDir/include
sudo cp -r "$ProgFile/omniORB/4.1.4/include/omniVms" $MingwDir/include
sudo cp -r "$ProgFile/omniORB/4.1.4/include/omnithread" $MingwDir/include

gendef "$ProgFile/OpenRTM-aist/1.0/bin/RTC100.dll"
sudo cp "$ProgFile/OpenRTM-aist/1.0/bin/RTC100.dll" $MingwDir/bin
sudo i586-mingw32msvc-dlltool -d RTC100.def -l $MingwDir/lib/libRTC.a

gendef "$ProgFile/OpenRTM-aist/1.0/bin/coil.dll"
sudo cp "$ProgFile/OpenRTM-aist/1.0/bin/coil.dll" $MingwDir/bin
sudo i586-mingw32msvc-dlltool -d coil.def -l $MingwDir/lib/libcoil.a

gendef "$ProgFile/omniORB/4.1.4/bin/x86_win32/omniORB414_vc9_rt.dll"
sudo cp "$ProgFile/omniORB/4.1.4/bin/x86_win32/omniORB414_vc9_rt.dll" $MingwDir/bin
sudo i586-mingw32msvc-dlltool -d omniORB414_vc9_rt.def -l $MingwDir/lib/libomniORB4.a

gendef "$ProgFile/omniORB/4.1.4/bin/x86_win32/omnithread34_vc9_rt.dll"
sudo cp "$ProgFile/omniORB/4.1.4/bin/x86_win32/omnithread34_vc9_rt.dll" $MingwDir/bin
sudo i586-mingw32msvc-dlltool -d omnithread34_vc9_rt.def -l $MingwDir/lib/libomnithread.a

gendef "$ProgFile/omniORB/4.1.4/bin/x86_win32/omniDynamic414_vc9_rt.dll"
sudo cp "$ProgFile/omniORB/4.1.4/bin/x86_win32/omniDynamic414_vc9_rt.dll" $MingwDir/bin
sudo i586-mingw32msvc-dlltool -d omniDynamic414_vc9_rt.def -l $MingwDir/lib/libomniDynamic4.a

# other libs

# apt-get source libsndfile
# cd sndfile
# ./configure --host=i586-mingw32msvc --prefix=/usr/i586-mingw32msvc --disable-external-libs --disable-sqlite
# make
# sudo make install

# apt-get source libportaudio2
# cd portaudio
# ./configure --host=i586-mingw32msvc --prefix=/usr/i586-mingw32msvc --with-winapi
# make
# sudo make install

# apt-get source speex
# cd speex
# ./configure --host=i586-mingw32msvc --prefix=/usr/i586-mingw32msvc --with-winapi
# make
# sudo make install