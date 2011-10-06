#!/bin/sh

echo "exiting existing components..."
rtexit /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtexit /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtexit /localhost/`hostname`.host_cxt/EchoCanceler0.rtc
rtexit /localhost/`hostname`.host_cxt/Mixer0.rtc
sleep 3

echo "launching components..."
gnome-terminal -x pulseaudioinput
gnome-terminal -x echocanceler
gnome-terminal -x mixer
gnome-terminal -x pulseaudiooutput
sleep 3

echo "connecting components..."
rtcon /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/Mixer0.rtc:ReferenceAudioDataIn

#echo "configureing components..."
rtconf /localhost/`hostname`.host_cxt/Mixer0.rtc set MixGains "0.8,0.2"

echo "activating components..."
rtact /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtact /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtact /localhost/`hostname`.host_cxt/EchoCanceler0.rtc
rtact /localhost/`hostname`.host_cxt/Mixer0.rtc

echo "listen to sound with echo for 15 seconds..."
sleep 15

echo "connecting to filter..."
rtdis /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/EchoCanceler0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/EchoCanceler0.rtc:ReferenceAudioDataIn
rtcon /localhost/`hostname`.host_cxt/EchoCanceler0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn

echo "listen to filtered signal for 15 seconds..."
sleep 15

echo "existing components..."
rtexit /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtexit /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtexit /localhost/`hostname`.host_cxt/EchoCanceler0.rtc
rtexit /localhost/`hostname`.host_cxt/Mixer0.rtc
