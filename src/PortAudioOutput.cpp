// -*- C++ -*-
/*!
 * @file  PortAudioOutput.cpp
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

#include "PortAudioOutput.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

extern coil::Mutex m_pa_mutex;

// Module specification
// <rtc-template block="module_spec">
const char* portaudiooutput_spec[] =
  {
    "implementation_id", "PortAudioOutput",
    "type_name",         "PortAudioOutput",
    "description",       N_("Audio output component using portaudio"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.OutputSampleRate", "16000",
    "conf.default.OutputSampleByte", "int16",
    "conf.default.OutputChannelNumbers", "1",
    "conf.default.DelayCount", "0",
    "conf.__widget__.OutputSampleByte", "radio",
    "conf.__constraints__.OutputSampleByte", "(int8, int16, int24, int32)",
    "conf.__description__.OutputSampleByte", N_("Sample byte of audio output."),
    "conf.__widget__.OutputChannelNumbers", "spin",
    "conf.__constraints__.OutputChannelNumbers", "x >= 1",
    "conf.__description__.OutputChannelNumbers", N_("Number of audio channel."),
    "conf.__widget__.OutputSampleRate", "spin",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__description__.OutputSampleRate", N_("Sample rate of audio output."),
    "conf.__widget__.DelayCount", "spin",
    "conf.__constraints__.DelayCount", "x >= 0",
    "conf.__description__.DelayCount", N_("Buffer length of audio output."),
    "conf.__doc__.usage", "\n  ::\n\n  $ portaudiooutput\n",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 */
PortAudioOutputDataListener::PortAudioOutputDataListener(const char *name, void* data)
{
  m_obj = data;
  m_name = name;
}

/*!
 * @brief destructor
 */
PortAudioOutputDataListener::~PortAudioOutputDataListener()
{
}

void PortAudioOutputDataListener::operator ()(const ConnectorInfo& info, const TimedOctetSeq& data)
{
  if ( m_name == "ON_BUFFER_WRITE" ) {
    PortAudioOutput *p = (PortAudioOutput *)m_obj;
    p->RcvBuffer(data);
  }
}

static int StreamCB( const void *inputBuffer,
		     void *outputBuffer,
		     unsigned long framesPerBuffer,
		     const PaStreamCallbackTimeInfo *timeInfo,
		     PaStreamCallbackFlags statusFlags,
		     void *userData )
{
  PortAudioOutput *p = (PortAudioOutput *)userData;
  unsigned long nbytes = p->m_totalframes;
  if ( FRAMES_PER_BUFFER != framesPerBuffer ) {
    nbytes = framesPerBuffer * p->m_totalframes / FRAMES_PER_BUFFER;
  }
  p->WriteBufferCB(outputBuffer, nbytes);
  return 0;
}

/*!
 * @brief constructor
 */
PortAudioOutput::PortAudioOutput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_in_dataIn("AudioDataIn", m_in_data),
    m_out_dataOut("AudioDataOut", m_out_data)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PortAudioOutput::~PortAudioOutput()
{
}

RTC::ReturnCode_t PortAudioOutput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PortAudioOutput : Audio output component using portaudio library"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_in_dataIn);
  //  registerInPort("SampleInPort0", m_in_dataIn);
  m_in_dataIn.setDescription(_("Audio data input."));

  /* setiing datalistener event */
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new PortAudioOutputDataListener("ON_BUFFER_WRITE", this), false);

  // Set OutPort buffer
  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Actual audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("OutputSampleRate", m_samplerate, "16000");
  bindParameter("OutputSampleByte", m_formatstr, "int16");
  bindParameter("OutputChannelNumbers", m_channels, "1");
  bindParameter("DelayCount", m_delay, "0");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

PaSampleFormat PortAudioOutput::getFormat(std::string str)
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

RTC::ReturnCode_t PortAudioOutput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  PaStreamParameters outputParameters;

  try {
    m_pa_mutex.lock();

    m_format = getFormat(m_formatstr);
    m_totalframes = FRAMES_PER_BUFFER * m_channels;
    m_err = Pa_GetSampleSize(m_format);
    if ( m_err > 0 ) {
      m_totalframes *= m_err;
    }
    m_data.clear();  //!< queue buffer clear

    /* stream setiing */
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if ( outputParameters.device < 0 ) {
      throw (paNotInitialized);
    }
    outputParameters.channelCount = m_channels;
    outputParameters.sampleFormat = m_format;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowInputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    m_err = Pa_OpenStream(
          &m_stream,         //!< PortAudioStream
          NULL,              //!< InputParameters
          &outputParameters, //!< outputParameters
          m_samplerate,      //!< sampleRate
          FRAMES_PER_BUFFER, //!< framesPerBuffer
          paClipOff,         //!< streamFlags:we won't output out of range samples so don't bother clipping
//          StreamCB,          //!< streamCallback
//          this );            //!< callback userData
          NULL,              //!< streamCallback:no callback, use blocking API
          NULL );            //!< no callback, so no callback userData

    if ( m_err != paNoError ) {
      throw m_err;
    }

    m_err = Pa_StartStream( m_stream );
    if ( m_err != paNoError ) {
      throw m_err;
    }

    m_pa_mutex.unlock();
  } catch (...) {
    std::string error_str = Pa_GetErrorText(m_err);
    RTC_WARN(("PortAudio failed onActivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }

  if ( !m_data.empty() ) {
    m_data.clear();  //!< queue buffer clear
  }
  is_active = true;
  m_waitflg = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PortAudioOutput::onExecute(RTC::UniqueId ec_id)
{
  WriteBuffer();

  return RTC::RTC_OK;
}

void PortAudioOutput::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( is_active == true ) {
    //! The queuing does input data.
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    unsigned long i = 0;
    if ( m_waitflg == true ) {
      RTC_DEBUG(("Accumulation waiting process start."));
      m_waitflg = false;
      if ( m_delay > 0 ) {
        for ( i = 0; i < (m_delay * m_totalframes); i++ ) {
          m_data.push_back(0);  //!< enqueue
        }
      }
      RTC_DEBUG(("Accumulation waiting process finish."));
    }
    RTC_DEBUG(("Queuing input data process start."));
    unsigned long len = data.data.length();  //!< inport data length
    for ( i = 0; i < len; i++ ) {
      m_data.push_back(data.data[i]);  //!< enqueue
    }
    RTC_DEBUG(("Queuing input data process finish."));
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  return;
}

void PortAudioOutput::WriteBufferCB(void *data, unsigned long fream_len)
{
  m_mutex.lock();
  char *buff = (char *)data;
  for ( unsigned long i = 0; i < fream_len; i++ ) {
    if ( !m_data.empty() ) {
      *buff++ = m_data.front();
      m_data.pop_front();  //!< dequeue
    } else {
      *buff++ = 0;
      m_waitflg = true;
    }
  }
  m_mutex.unlock();

  m_out_data.data.length(fream_len);  //!< set outport data length
  memcpy((void *)&(m_out_data.data[0]), data, fream_len);
  setTimestamp( m_out_data );
  m_out_dataOut.write();

  return;
}

int PortAudioOutput::WriteBuffer(void)
{
  //! Input data(queue buffer) is output to the device
  m_pa_mutex.lock();
  m_mutex.lock();
  RTC_DEBUG(("WriteBuffer:mutex lock"));
  if (Pa_IsStreamActive(m_stream)) {
    if (FRAMES_PER_BUFFER <= (unsigned long)(Pa_GetStreamWriteAvailable(m_stream))) {
      unsigned char *buffer;  //!< Temporary buffer
      buffer = new unsigned char[m_totalframes];  //!< Temporary buffer

      RTC_DEBUG(("WriteBuffer:audio buffer dequeue start."));
      for ( unsigned long i = 0; i < m_totalframes; i++ ) {
        if ( !m_data.empty() ) {
          buffer[i] = m_data.front();
          m_data.pop_front();  //!< dequeue
        } else {
          buffer[i] = 0;
          m_waitflg = true;
        }
      }
      RTC_DEBUG(("WriteBuffer:audio buffer dequeue finish."));

      m_out_data.data.length(m_totalframes);  //!< set outport data length
      memcpy((void *)&(m_out_data.data[0]), buffer, m_totalframes);
      RTC_DEBUG(("WriteBuffer:audio buffer copy complete."));
      setTimestamp( m_out_data );
      m_out_dataOut.write();
      RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));

      //! write stream
      m_err = Pa_WriteStream( m_stream, buffer, FRAMES_PER_BUFFER );
      if ( m_err != paNoError ) {
        std::string error_str = Pa_GetErrorText(m_err);
        RTC_WARN(("PortAudio WriteStream failed:%s", error_str.c_str()));
        return RTC::RTC_ERROR;
      }
      delete [] buffer;
    }
  } else {
    if ( !m_data.empty() ) {
      m_data.clear();  //!< queue buffer clear
    }
  }
  m_mutex.unlock();
  m_pa_mutex.unlock();
  RTC_DEBUG(("WriteBuffer:mutex unlock"));
  return 0;
}

RTC::ReturnCode_t PortAudioOutput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  try {
    if ( !m_data.empty() ) {
      m_data.clear();  //!< queue buffer clear
    }

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
    RTC_WARN(("PortAudio Stream close failed onDeactivated:%s", error_str.c_str()));
    return RTC::RTC_ERROR;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PortAudioOutput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  try {
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
  void PortAudioOutputInit(RTC::Manager* manager)
  {
    int i, j;
    PaError m_err;
    
    m_err = Pa_Initialize();
    if(m_err != paNoError) {
      printf("PortAudio failed: %s\n", Pa_GetErrorText(m_err));
      return;
    }

    for (i = 0; strlen(portaudiooutput_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(portaudiooutput_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                             RTC::Create<PortAudioOutput>,
                             RTC::Delete<PortAudioOutput>);
  }
};
