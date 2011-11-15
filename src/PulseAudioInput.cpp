// -*- C++ -*-
/*!
 * @file  PulseAudioInput.cpp
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

#include "PulseAudioInput.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include <intl.h>

// Module specification
// <rtc-template block="module_spec">
static const char* pulseaudioinput_spec[] =
  {
    "implementation_id", "PulseAudioInput",
    "type_name",         "PulseAudioInput",
    "description",       N_("Audio input component using pulseaudio"),
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
    "conf.__constraints__.InputSampleByte", "(int8, int16, int24,int32)",
    "conf.__description__.InputSampleByte", N_("Sample byte of audio capture."),
    "conf.__widget__.InputChannelNumbers", "spin",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__description__.InputChannelNumbers", N_("Number of audio channel."),
    "conf.__widget__.InputSampleRate", "spin",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__description__.InputSampleRate", N_("Sample rate of audio capture."),
    "conf.__doc__.usage", "\n  ::\n\n  $ pulseaudioinput\n",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 */
DataListener::DataListener(const char *name, void* data)
{
  m_obj = data;
  m_name = name;
}

/*!
 * @brief destructor
 */
DataListener::~DataListener()
{
}

void DataListener::operator ()(const ConnectorInfo& info, const TimedLong& data)
{
  if ( m_name == "ON_BUFFER_WRITE" ) {
      PulseAudioInput *p = (PulseAudioInput *)m_obj;
      p->SetGain(data.data);
    }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PulseAudioInput::PulseAudioInput(RTC::Manager* manager)
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
PulseAudioInput::~PulseAudioInput()
{
}

RTC::ReturnCode_t PulseAudioInput::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("PulseAudioInput : Audio input component using pulseaudio api"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_in_dataIn);
  m_in_dataIn.setDescription(_("Audio data input."));

  /* setting datalistener event */
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_GAIN_WRITE", this), false);

  // Set OutPort buffer
  m_out_data.tm.nsec = 0;
  m_out_data.tm.sec = 0;
  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Audio data in packet."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("InputSampleRate", m_spec.rate, "16000");
  bindParameter("InputSampleByte", m_formatstr, "int16");
  bindParameter("InputChannelNumbers", m_channels, "1");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

pa_sample_format PulseAudioInput::getFormat(std::string str)
{
  if ( str == "int8" ) {
    return PA_SAMPLE_U8;
  } else if ( str == "int16" ) {
    return PA_SAMPLE_S16LE;
  } else if ( str == "int24" ) {
    return PA_SAMPLE_S24LE ;
  } else if ( str == "int32" ) {
    return PA_SAMPLE_S32LE ;
  } else {
    return PA_SAMPLE_S16LE;
  }
}

RTC::ReturnCode_t PulseAudioInput::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  try {
    pa_cvolume cv;
//    mp_vol = pa_cvolume_reset(&cv, 1);
//    pa_cvolume_init(mp_vol);
    m_spec.format = getFormat(m_formatstr);
    m_spec.channels = (uint8_t)m_channels;

    m_simple = pa_simple_new(
                  NULL,               //!< Server name, or NULL for default
                  "PulseAudioInput",  //!< A descriptive name for this client (application name, ...)
                  PA_STREAM_RECORD,   //!< Open this stream for recording or playback?
                  NULL,               //!< Sink (resp. source) name, or NULL for default
                  "record",           //!< A descriptive name for this client (application name, song title, ...)
                  &m_spec,            //!< The sample type to use
                  NULL,               //!< The channel map to use, or NULL for default
                  NULL,               //!< Buffering attributes, or NULL for default
                  &m_err );           //!< A pointer where the error code is stored when the routine returns NULL. It is OK to pass NULL here.
    if ( m_simple == NULL ) {
      throw m_err;
    }
  } catch (...) {
    std::string error_str = pa_strerror(m_err);
    RTC_WARN(("pa_simple_new() failed onActivated:%s", error_str.c_str()));
  }
  is_active = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void PulseAudioInput::SetGain(long m_gain)
{
  pa_cvolume v;
  v.values[0] = pa_sw_volume_from_linear(m_gain);
  pa_cvolume_set(&v, 2, v.values[0]);
  //      pa_volume_t gain = pa_sw_volume_from_linear(m_gain);
  //      pa_cvolume_set(mp_vol, 1, gain);
  return;
}

RTC::ReturnCode_t PulseAudioInput::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if( m_simple ) {
    int r;
    simple_recast *psimple = (simple_recast *)m_simple;
    pa_threaded_mainloop_lock( psimple->mainloop );
    RTC_DEBUG(("pa_threaded_mainloop_lock()"));

    while ( !psimple->read_data ) {
      r = pa_stream_peek( psimple->stream, &psimple->read_data, &psimple->read_length );
      if ( !psimple->read_data ) {
        RTC_DEBUG(("pa_stream_peek():no readable data. wait start."));
        pa_threaded_mainloop_wait(psimple->mainloop);
      }
    }

    m_out_data.data.length( psimple->read_length );  //!< set outport data length
    memcpy((void *)&(m_out_data.data[0]), (const uint8_t*) psimple->read_data, psimple->read_length);

    r = pa_stream_drop( psimple->stream );
    if ( r < 0 ) {
      RTC_WARN(("pa_stream_drop():capture stream drop failed."));
    }
    psimple->read_data = NULL;
    psimple->read_length = 0;
    psimple->read_index = 0;

    setTimestamp( m_out_data );
    m_out_dataOut.write();
    RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));

    pa_threaded_mainloop_unlock( psimple->mainloop );
    RTC_DEBUG(("pa_threaded_mainloop_unlock()"));
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PulseAudioInput::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( m_simple ) {
    RTC_DEBUG(("onDeactivated:simple connection object free start."));
    pa_simple_free( m_simple );
    RTC_DEBUG(("onDeactivated:simple connection object free finish."));
    m_simple = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t PulseAudioInput::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( m_simple ) {
    RTC_DEBUG(("onFinalize:simple connection object free start."));
    pa_simple_free( m_simple );
    RTC_DEBUG(("onFinalize:simple connection object free finish."));
   m_simple = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void PulseAudioInputInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(pulseaudioinput_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = _((char *)pulseaudioinput_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<PulseAudioInput>,
                           RTC::Delete<PulseAudioInput>);
  }
};
