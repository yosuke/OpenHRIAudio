#!/bin/sh

echo "exiting existing components..."
rtexit /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtexit /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtexit /localhost/`hostname`.host_cxt/CombFilter0.rtc
rtexit /localhost/`hostname`.host_cxt/Mixer0.rtc
rtexit /localhost/`hostname`.host_cxt/SignalGeneration0.rtc
sleep 3

echo "launching components..."
gnome-terminal -x pulseaudioinput
gnome-terminal -x combfilter
gnome-terminal -x mixer
gnome-terminal -x signalgeneration
gnome-terminal -x pulseaudiooutput
sleep 3

echo "connecting components..."
rtcon /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/SignalGeneration0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/Mixer0.rtc:ReferenceAudioDataIn
rtcon /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn

echo "activating components..."
rtact /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtact /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtact /localhost/`hostname`.host_cxt/CombFilter0.rtc
rtact /localhost/`hostname`.host_cxt/Mixer0.rtc
rtact /localhost/`hostname`.host_cxt/SignalGeneration0.rtc

echo "listen to noise signal for 15 seconds..."
sleep 15

echo "connecting to filter..."
rtdis /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/Mixer0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/CombFilter0.rtc:AudioDataIn
rtcon /localhost/`hostname`.host_cxt/CombFilter0.rtc:AudioDataOut /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc:AudioDataIn

echo "listen to filtered signal for 15 seconds..."
sleep 15

echo "existing components..."
rtexit /localhost/`hostname`.host_cxt/PulseAudioOutput0.rtc
rtexit /localhost/`hostname`.host_cxt/PulseAudioInput0.rtc
rtexit /localhost/`hostname`.host_cxt/CombFilter0.rtc
rtexit /localhost/`hostname`.host_cxt/Mixer0.rtc
rtexit /localhost/`hostname`.host_cxt/SignalGeneration0.rtc
