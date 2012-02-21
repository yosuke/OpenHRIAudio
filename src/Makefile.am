bin_PROGRAMS = openhriaudiomanager portaudiooutput portaudioinput pulseaudiooutput pulseaudioinput echocanceler echosuppressor samplingrateconverter noisereduction combfilter signalgeneration mixer dsarray channelselector beamforming autogaincontrol wavrecord wavplayer webrtcvad gatertc
lib_LTLIBRARIES = libportaudiortc.la libpulseaudiooutput.la libpulseaudioinput.la libechocanceler.la libechosuppressor.la libsamplingrateconverter.la libnoisereduction.la libcombfilter.la libsignalgeneration.la libmixer.la libdsarray.la libchannelselector.la libbeamforming.la libautogaincontrol.la libwavrecord.la libwavplayer.la libwebrtcvad.la libgate.la

LIBS = @LDFLAGS@ @LIBS@ $(OPENRTM_LIBS) $(PULSEAUDIO_LIBS) -lm @LTLIBINTL@
AM_CPPFLAGS = -g -Wall @CFLAGS@ $(OPENRTM_CFLAGS) $(PULSEAUDIO_CFLAGS) @DEFS@ -DLOCALEDIR=\"$(localedir)\" -I ../thirdparty/webrtc -I ../thirdparty/webrtc/common_audio/vad/include

include_HEADERS = DescriptablePort.h PortAudioOutput.h PortAudioInput.h PulseAudioOutput.h PulseAudioInput.h EchoCanceler.h EchoSuppressor.h SamplingRateConverter.h NoiseReduction.h CombFilter.h SignalGeneration.h Mixer.h DSArray.h ChannelSelector.h BeamForming.h AutoGainControl.h WavRecord.h WavPlayer.h Gate.h windowing.h arch.h os_support.h fftwrap.h smallft.h

openhriaudiomanager_SOURCES = OpenHRIAudioManager.cpp
openhriaudiomanager_CPPFLAGS = $(AM_CPPFLAGS) $(GTKMM_CFLAGS)
openhriaudiomanager_LDADD = $(GTKMM_LIBS)

libportaudiortc_la_SOURCES = PortAudioInput.cpp PortAudioOutput.cpp PortAudioRTC.cpp

portaudioinput_SOURCES = PortAudioInputComp.cpp
portaudioinput_LDADD = libportaudiortc.la

portaudiooutput_SOURCES = PortAudioOutputComp.cpp
portaudiooutput_LDADD = libportaudiortc.la

libpulseaudiooutput_la_SOURCES = PulseAudioOutput.cpp
pulseaudiooutput_SOURCES = PulseAudioOutputComp.cpp
pulseaudiooutput_LDADD = libpulseaudiooutput.la

libpulseaudioinput_la_SOURCES = PulseAudioInput.cpp
pulseaudioinput_SOURCES = PulseAudioInputComp.cpp
pulseaudioinput_LDADD = libpulseaudioinput.la

libechocanceler_la_SOURCES = EchoCanceler.cpp
echocanceler_SOURCES = EchoCancelerComp.cpp
echocanceler_LDADD = libechocanceler.la

libechosuppressor_la_SOURCES = EchoSuppressor.cpp
echosuppressor_SOURCES = EchoSuppressorComp.cpp
echosuppressor_LDADD = libechosuppressor.la

libnoisereduction_la_SOURCES = NoiseReduction.cpp windowing.cpp fftwrap.c smallft.c
noisereduction_SOURCES = NoiseReductionComp.cpp
noisereduction_LDADD = libnoisereduction.la

libsamplingrateconverter_la_SOURCES = SamplingRateConverter.cpp
samplingrateconverter_SOURCES = SamplingRateConverterComp.cpp
samplingrateconverter_LDADD = libsamplingrateconverter.la

libcombfilter_la_SOURCES = CombFilter.cpp
combfilter_SOURCES = CombFilterComp.cpp
combfilter_LDADD = libcombfilter.la

libsignalgeneration_la_SOURCES = SignalGeneration.cpp
signalgeneration_SOURCES = SignalGenerationComp.cpp
signalgeneration_LDADD = libsignalgeneration.la

libmixer_la_SOURCES = Mixer.cpp
mixer_SOURCES = MixerComp.cpp
mixer_LDADD = libmixer.la

libdsarray_la_SOURCES = DSArray.cpp windowing.cpp fftwrap.c smallft.c
dsarray_SOURCES = DSArrayComp.cpp
dsarray_LDADD = libdsarray.la

libchannelselector_la_SOURCES = ChannelSelector.cpp
channelselector_SOURCES = ChannelSelectorComp.cpp
channelselector_LDADD = libchannelselector.la

libbeamforming_la_SOURCES = BeamForming.cpp windowing.cpp fftwrap.c smallft.c
beamforming_SOURCES = BeamFormingComp.cpp
beamforming_LDADD = libbeamforming.la

libautogaincontrol_la_SOURCES = AutoGainControl.cpp
autogaincontrol_SOURCES = AutoGainControlComp.cpp
autogaincontrol_LDADD = libautogaincontrol.la

AM_D_SHARED_LIB = -DSHARED_LIB

libwavrecord_la_SOURCES = WavRecord.cpp
libwavrecord_la_CPPFLAGS = $(AM_D_SHARED_LIB) $(AM_CPPFLAGS) $(GTKMM_CFLAGS)
libwavrecord_la_LIBADD = $(GTKMM_LIBS)
wavrecord_SOURCES = WavRecordComp.cpp WavRecord.cpp
wavrecord_CPPFLAGS = $(AM_CPPFLAGS) $(GTKMM_CFLAGS)
wavrecord_LDADD = $(GTKMM_LIBS)

libwavplayer_la_SOURCES = WavPlayer.cpp
libwavplayer_la_CPPFLAGS = $(AM_D_SHARED_LIB) $(AM_CPPFLAGS) $(GTKMM_CFLAGS)
libwavplayer_la_LIBADD = $(GTKMM_LIBS)
wavplayer_SOURCES = WavPlayerComp.cpp WavPlayer.cpp
wavplayer_CPPFLAGS = $(AM_CPPFLAGS) $(GTKMM_CFLAGS)
wavplayer_LDADD = $(GTKMM_LIBS)

libwebrtcvad_la_SOURCES = WebRTCVAD.cpp
libwebrtcvad_la_LIBADD = libwebrtccommonaudio.la
#libwebrtcvad_la_LDFLAGS =  -L. -lwebrtc
webrtcvad_SOURCES = WebRTCVADComp.cpp
#webrtcvad_LDFLAGS = -L. -lwebrtc
webrtcvad_LDADD = libwebrtcvad.la libwebrtccommonaudio.la

libgate_la_SOURCES = Gate.cpp
gatertc_SOURCES = GateComp.cpp
gatertc_LDADD = libgate.la
