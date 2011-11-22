// -*- C++ -*-
/*!
 * @file  PortAudioInput.cpp
 * @author Yosuke Matsusaka <yosuke.matsusaka@aist.go.jp> and OpenHRI development team
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date $Date$
 *
 * $Id$
 */

#include "PortAudioInput.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"
#if defined(_WIN32)
#define _WIN32_WINNT 0x0601
#define PA_USE_WASAPI
#endif

#ifdef HAVE_LIBPORTMIXER
// use portmixer"s internal volume control functions
extern "C" {
  void *Px_OpenMixer( void *pa_stream, int i );
  void Px_CloseMixer(void *mixer);
  float Px_GetInputVolume( void *mixer );
  void Px_SetInputVolume( void *mixer, float volume );
}
#endif

//extern coil::Mutex m_pa_mutex; //by Irie Seisho

// Module specification
// <rtc-template block="module_spec">
const char* portaudioinput_spec[] =
  {
    "implementation_id", "PortAudioInput",
    "type_name",         "PortAudioInput",
    "description",       N_("Audio input component using portaudio"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.InputSampleRate", "16000",
    "conf.default.InputSampleByte", "int16",
    "conf.default.InputChannelNumbers", "1",
    "conf.__widget__.InputSampleByte", "radio",
    "conf.__constraints__.InputSampleByte", "(int8, int16, int24, int32)",
    "conf.__description__.InputSampleByte", N_("Sample byte of audio capture."),
    "conf.__widget__.InputChannelNumbers", "spin",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__description__.InputChannelNumbers", N_("Number of audio channel."),
    "conf.__widget__.InputSampleRate", "spin",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__description__.InputSampleRate", N_("Sample rate of audio capture."),
    "conf.__doc__.usage", "\n  ::\n\n  $ portaudioinput\n",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 */
PortAudioInputDataListener::PortAudioInputDataListener(const char *name, void* data)
{
  m_obj = data;
  m_name = name;
}

/*!
 * @brief destructor
 */
PortAudioInputDataListener::~PortAudioInputDataListener()
{
}

void PortAudioInputDataListener::operator ()(const ConnectorInfo& info, const TimedLong& data)
{
  if ( m_name == "ON_BUFFER_WRITE" ) {
    PortAudioInput *p = (PortAudioInput *)m_obj;
    p->SetGain(data.data);
  }
}

static int StreamCB( const void *inputBuffer,
		     void *outputBuffer,
		     unsigned long framesPerBuffer,
		     const PaStreamCallbackTimeInfo *timeInfo,
		     PaStreamCallbackFlags statusFlags,
		     void *userData )
{
  PortAudioInput *p = (PortAudioInput *)userData;
  unsigned long nbytes = p->m_totalframes;
  if ( FRAMES_PER_BUFFER != framesPerBuffer ) {
    nbytes = framesPerBuffer * p->m_totalframes / FRAMES_PER_BUFFER;
  }
  p->ReadBufferCB(inputBuffer, nbytes);
  return 0;
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PortAudioInput::PortAudioInput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_in_dataIn("GainDataIn", m_in_data),
    m_out_dataOut("AudioDataOut", m_out_data)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PortAudioInput::~PortAudioInput()
{
}

RTC::ReturnCode_t PortAudioInput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PortAudioInput : Audio input component using portaudio library"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("GainDataIn", m_in_dataIn);
  m_in_dataIn.setDescription(_("Gain."));

  /* setting datalistener event */
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new PortAudioInputDataListener("ON_BUFFER_WRITE", this), false);

  // Set OutPort buffer
  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Audio data in packet."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("InputSampleRate", m_samplerate, "16000");
  bindParameter("InputSampleByte", m_formatstr, "int16");
  bindParameter("InputChannelNumbers", m_channels, "1");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

PaSampleFormat PortAudioInput::getFormat(std::string str)
{
  if ( str == "int8" ) {
    return paInt8;
  } else if ( str == "int16" ) {
    return paInt16;
  } else if ( str == "int24" ) {
    return paInt24;
  } else if ( str == "int32" ) {
    return paInt32;
  } else {
    return paInt16;
  }
}

#if defined(_WIN32)
bool PortAudioInput::InitMixer(void)
{
  MIXERLINE         mxl;
  MIXERCONTROL      mxc;
  MIXERLINECONTROLS mxlc;
  MMRESULT          mmresult;
  DWORD dwLineID = -1;

  m_isOpen = false;
  m_mixerNums = mixerGetNumDevs();
  if ( m_mixerNums < 1 ) return FALSE;
  m_mixerNums = min(m_mixerNums, HMIXER_NUM_MAX);

  mxl.cbStruct = sizeof(MIXERLINE);
  mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

  /* Mixer Init */
  for ( int i = 0; i < m_mixerNums; i++ ) {
    mmresult = mixerOpen( &m_mixer[i], (UINT)i, 0, 0, MIXER_OBJECTF_MIXER );
    if ( mmresult == MMSYSERR_NOERROR ) {
      m_barrOpened[i] = true;
      m_isOpen = true;
      mmresult = mixerGetLineInfo( (HMIXEROBJ)m_mixer[i], &mxl,
				   MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
      if ( mmresult == MMSYSERR_NOERROR ) {
        m_mixerMic = m_mixer[i];
        break;
      }
    }
  }
  if ( m_mixerMic == 0 ) return false;
  /* Volume Control Init */
  for ( DWORD i = 0; i < mxl.cConnections; i++ ) {
    mxl.dwSource = i;
    mmresult = mixerGetLineInfo( (HMIXEROBJ)m_mixerMic, &mxl,
				 MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);
    if ( mmresult != MMSYSERR_NOERROR ) {
      break;
    }
    if ( mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE ) {
      dwLineID = mxl.dwLineID;
      break;
    }
  }
  if ( dwLineID == -1 ) {
    return false;
  }

  mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
  mxlc.dwLineID = dwLineID;
  mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
  mxlc.cControls = 1;
  mxlc.cbmxctrl = sizeof(MIXERCONTROL);
  mxlc.pamxctrl = &mxc;
  mmresult = mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				  &mxlc,
				  MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
  if ( mmresult != MMSYSERR_NOERROR ) return false;
  m_dwCntlID = mxc.dwControlID;
  m_dwMax = mxc.Bounds.dwMaximum;
  m_dwMin = mxc.Bounds.dwMinimum;
  return true;
}

void PortAudioInput::CloseMixer(void)
{
  m_dwCntlID = 0;
  if ( m_isOpen ) {
    for (int i = 0; i < m_mixerNums; i++ ) {
      if ( m_barrOpened[i] ) {
        mixerClose(m_mixer[i]);
        m_barrOpened[i] = false;
      }
    }
    m_isOpen = false;
  }
}

bool PortAudioInput::GetMicrophoneLevel(DWORD* dwLevel)
{
  MMRESULT mmresult;
  MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
  MIXERCONTROLDETAILS   mxcd;

  mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
  mxcd.dwControlID = m_dwCntlID;
  mxcd.cChannels = 1;
  mxcd.cMultipleItems = 0;
  mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
  mxcd.paDetails = &mxcdVolume;
  mmresult = mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				    &mxcd,
				    MIXER_GETCONTROLDETAILSF_VALUE);
  if (mmresult != MMSYSERR_NOERROR) return false;
  *dwLevel = mxcdVolume.dwValue;
  return true;
}

bool PortAudioInput::SetMicrophoneLevel(DWORD dwLevel)
{
  MMRESULT mmresult;
  MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set = {
    dwLevel
  };
  MIXERCONTROLDETAILS mxcd_Set;
  mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);
  mxcd_Set.dwControlID = m_dwCntlID;
  mxcd_Set.cChannels = 1;
  mxcd_Set.cMultipleItems = 0;
  mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
  mxcd_Set.paDetails = &mxcdVolume_Set;
  mmresult = mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(m_mixerMic),
				    &mxcd_Set,
				    MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE);
  return ( mmresult == MMSYSERR_NOERROR );
}
#elif defined(__linux)

#endif

RTC::ReturnCode_t PortAudioInput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  PaStreamParameters inputParameters;
//  PaWasapiStreamInfo wasapiinfo;

  try {
    //m_pa_mutex.lock(); //by Irie Seisho

    m_format = getFormat(m_formatstr);
    m_totalframes = FRAMES_PER_BUFFER * m_channels;
    m_err = Pa_GetSampleSize(m_format);
    if( m_err > 0 ) {
      m_totalframes *= m_err;
    }

#if 0
    /* Find all WASAPI devices */
    const PaDeviceInfo *device;
    for ( int i = 0; i < Pa_GetDeviceCount(); ++i ) {
      device = Pa_GetDeviceInfo(i);
      if ( Pa_GetDeviceInfo(i)->hostApi == Pa_HostApiTypeIdToHostApiIndex(paWASAPI) ) {
        std::cout << "Device Index " << i << " : " << device->name << ", inch " << device->maxInputChannels << ", outch " << device->maxOutputChannels << std::endl;
      }
    }

//#if 0
    PaDeviceIndex dnum = Pa_GetDeviceCount();
    for (int i = 0; i < (int)dnum; i++) {
      std::cout << "Device Index " << i << " : " << Pa_GetDeviceInfo(i)->name << ", inch " << Pa_GetDeviceInfo(i)->maxInputChannels << ", outch " << Pa_GetDeviceInfo(i)->maxOutputChannels << std::endl;
    }
//#endif
    void *pFormat;
    unsigned int nFormatSize;
    PaDeviceIndex nDevice;
    int r = PaWasapi_GetDeviceDefaultFormat(pFormat, nFormatSize, nDevice);
#endif

    inputParameters.device = Pa_GetDefaultInputDevice(); //!< default input device
    if ( inputParameters.device < 0 ) {
      throw (paNotInitialized);
    }
    if ( m_channels > Pa_GetDeviceInfo(inputParameters.device)->maxInputChannels )
      m_channels = Pa_GetDeviceInfo(inputParameters.device)->maxInputChannels;
    inputParameters.channelCount = m_channels;
    inputParameters.sampleFormat = m_format;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;

/*
    wasapiinfo.size = sizeof(PaWasapiStreamInfo);
    wasapiinfo.hostApiType = paWASAPI;
    wasapiinfo.version = 1;
    wasapiinfo.flags = paWasapiUseChannelSelectors;
    wasapiinfo.hostProcessorInput
    wasapiinfo.threadPriority
    wasapiinfo.channelMask = outputChannelSelectors;

    inputParameters.hostApiSpecificStreamInfo = wasapiinfo;
*/
    inputParameters.hostApiSpecificStreamInfo = NULL;
    m_err = Pa_OpenStream(
          &m_stream,         //!< PortAudioStream
          &inputParameters,  //!< InputParameters
          NULL,              //!< outputParameters
          m_samplerate,      //!< sampleRate
          FRAMES_PER_BUFFER, //!< framesPerBuffer
          paClipOff,         //!< streamFlags:we won't output out of range samples so don't bother clipping
//          StreamCB,          //!< streamCallback
//          this );            //!< callback userData
          NULL,              //!< streamCallback:no callback, use blocking API
          NULL );            //!< no callback, so no callback userData
    if( m_err != paNoError ) {
      throw m_err;
    }
#ifdef HAVE_LIBPORTMIXER
    m_mixer = Px_OpenMixer( m_stream, 0 );
    m_volume = Px_GetInputVolume( m_mixer );
#else
#if defined(_WIN32)
    if ( InitMixer() == true ) {
      DWORD vol;
      GetMicrophoneLevel(&vol);
    } else {
      CloseMixer();
    }
#elif defined(__linux)
    const char* sound_device_names[] = SOUND_DEVICE_NAMES;
    m_device = -1;
    m_fd = -1;
    for ( int i = 0; i < SOUND_MIXER_NRDEVICES; i ++ ) {
      std::cout << " device name : " << sound_device_names[i] << std::endl;
      if ( strcmp( "mic", sound_device_names[i] ) == 0 ) {
        m_device = i;
        break;
      }
    }
    if ( ( m_fd = open( "/dev/mixer", O_RDONLY ) ) == -1 ) {
      perror( "open" );
    }
#endif
#endif

    m_err = Pa_StartStream( m_stream );
    if( m_err != paNoError ) {
      throw m_err;
    }

    //m_pa_mutex.unlock(); //by Irie Seisho
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio failed onActivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }

  syncflg = true;
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void PortAudioInput::ReadBufferCB(const void *data, unsigned long fream_len)
{
  m_mutex.lock();
  m_out_data.data.length(fream_len); //!< set outport data length
  memcpy((void *)&(m_out_data.data[0]), data, fream_len);
  m_out_dataOut.write();
  m_mutex.unlock();
  return;
}

void PortAudioInput::SetGain(long m_gain)
{
#ifdef USE_PORTMIXER
  Px_SetInputVolume( m_mixer, float(m_volume * m_gain) );
#else
#if defined(_WIN32)
  if ( m_mixerMic != NULL ) {
    DWORD vol;
    GetMicrophoneLevel(&vol);
    vol = vol * m_gain;
    SetMicrophoneLevel(vol);
  }
#elif defined(__linux)
  int level = (int)m_gain;
  if ( ( m_fd > 0 ) && ( m_device > 0 ) ) {
    if ( ioctl( m_fd, MIXER_WRITE( m_device ), &level ) == -1 ) {
      perror( "ioctl(MIXER_WRITE())" );
    }
  }
#endif
#endif
  return;
}

#if 0
void PortAudioInput::AutoGainControl(void *data, int length)
{
  RTC_DEBUG(("AutoGainControl start"));
  int i,len;

  if ( m_formatstr == "int8" ) {
    len = length;
    char *buffer = (char *)data;
    for ( i = 0; i < len; i++ ) {
      buffer[i] = (char)(buffer[i] * m_gain);
    }
  } else if ( m_formatstr == "int24" ) {
    len = length / 3;
    long *buffer = (long *)data;
    for ( i = 0; i < len; i++ ) {
      buffer[i] = (long)(buffer[i] * m_gain);
    }
  } else {
    len = length / 2;
    short *buffer = (short *)data;
    for ( i = 0; i < len; i++ ) {
      buffer[i] = (short)(buffer[i] * m_gain);
    }
  }
  return;
}
#endif

RTC::ReturnCode_t PortAudioInput::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  //! The data read from the device is output.
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if ( Pa_IsStreamActive(m_stream) ) {
    m_out_data.data.length(m_totalframes); //!< set outport data length
    m_err = Pa_ReadStream( m_stream, (void *)&(m_out_data.data[0]), FRAMES_PER_BUFFER );
//    if( ( m_gain != 0 ) && ( m_gain != 1 ) && ( m_agc != 0 ) ) {
//      AutoGainControl((void *)&(m_out_data.data[0]), (int)m_totalframes );
//    }

    if ( m_err != paNoError ) {
      std::string error_str = Pa_GetErrorText(m_err);
      RTC_WARN(("PortAudio ReadStream failed:%s", error_str.c_str()));
    }
    setTimestamp(m_out_data);
    m_out_dataOut.write();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PortAudioInput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  try {
    if ( m_stream ) {
      if ( !Pa_IsStreamStopped( m_stream ) ) {
        m_err = Pa_AbortStream( m_stream );
        if ( m_err != paNoError ) {
          throw m_err;
        }
      }
#ifdef HAVE_LIBPORTMIXER
      Px_CloseMixer(m_mixer);
#elif defined(__linux)
      if ( m_fd > 0 ) {
        close( m_fd );
        m_fd = -1;
      }
#endif

      m_err = Pa_CloseStream( m_stream );
      if ( m_err != paNoError ) {
          throw m_err;
      }
      m_stream = NULL;
    }
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio Stream close failed onDeactivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PortAudioInput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  try {
    m_out_dataOut.deactivateInterfaces();
    if ( m_stream ) {
      if ( !Pa_IsStreamStopped( m_stream ) ) {
        m_err = Pa_AbortStream( m_stream );
        if ( m_err != paNoError ) {
          throw m_err;
        }
      }

      m_err = Pa_CloseStream( m_stream );
      if ( m_err != paNoError ) {
        throw m_err;
      }
      m_stream = NULL;
    }
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio Stream close failed onFinalize:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void PortAudioInputInit(RTC::Manager* manager)
  {
    int i, j;
    PaError m_err;
    
    m_err = Pa_Initialize();
    if(m_err != paNoError) {
      printf("PortAudio failed: %s\n", Pa_GetErrorText(m_err));
      return;
    }

    for (i = 0; strlen(portaudioinput_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(portaudioinput_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
			     RTC::Create<PortAudioInput>,
			     RTC::Delete<PortAudioInput>);
  }
};
