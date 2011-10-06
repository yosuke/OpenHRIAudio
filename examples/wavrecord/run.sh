#!/bin/sh

echo "exiting existing components..."
killall openhriaudiomanager
sleep 1

echo "launching components..."
gnome-terminal -x openhriaudiomanager -d
sleep 1
rtmgr --create=PulseAudioInput /localhost/`hostname`.host_cxt/manager.mgr
rtmgr --create=WavRecord /localhost/`hostname`.host_cxt/manager.mgr
sleep 1

echo "connecting components..."
rtcon /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/WavRecord0.rtc:AudioDataIn

echo "configureing components..."
rtconf /localhost/`hostname`.host_cxt/WavRecord0.rtc set FileName record0.wav

sleep 1
echo "activating components..."
rtact /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtact /localhost/`hostname`.host_cxt/WavRecord0.rtc

echo "recording input signal for 15 seconds..."
sleep 15

echo "existing components..."
rtexit /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtexit /localhost/`hostname`.host_cxt/WavRecord0.rtc
sleep 1
killall openhriaudiomanager

echo "playing recorded signal..."
paplay record0.wav