#!/bin//sh

ar crs src/libwebrtc.a ../webrtc/trunk/out/Debug/obj.target/signal_processing/src/common_audio/signal_processing/*.o ../webrtc/trunk/out/Debug/obj.target/vad/src/common_audio/vad/*.o

cp ../webrtc/trunk/src/typedefs.h ../webrtc/trunk/src/common_audio/vad/include/webrtc_vad.h src