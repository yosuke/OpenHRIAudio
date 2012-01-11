#!/bin/sh

rtcwd /localhost/`hostname`.host_cxt/

echo "exiting existing components..."
rtexit WebRTCVAD0.rtc 
rtexit PulseAudioInput0.rtc 
rtexit PulseAudioOutput0.rtc 
sleep 2

echo "launching components..."
gnome-terminal -x pulseaudioinput
gnome-terminal -x pulseaudiooutput
gnome-terminal -x webrtcvad
sleep 3

echo "connecting components..."
rtcon PulseAudioInput0.rtc:AudioDataOut WebRTCVAD0.rtc:AudioDataIn 
rtcon WebRTCVAD0.rtc:AudioDataOut PulseAudioOutput0.rtc:AudioDataIn

echo "activating components..."
rtact WebRTCVAD0.rtc 
rtact PulseAudioInput0.rtc 
rtact PulseAudioOutput0.rtc 
