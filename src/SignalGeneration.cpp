// -*- C++ -*-
/*!
 * @file  SignalGeneration.cpp
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

#include "SignalGeneration.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "../config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

// Module specification
// <rtc-template block="module_spec">
static const char* signalgeneration_spec[] =
  {
    "implementation_id", "SignalGeneration",
    "type_name",         "SignalGeneration",
    "description",       N_("Signal generation component"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.OutputSampleRate", "16000",
    "conf.default.Frequency", "100",
    "conf.default.Gain", "1000",
    "conf.default.Mode", "Square",
    "conf.__widget__.OutputSampleRate", "spin",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__description__.OutputSampleRate", N_("Sample rate of audio output."),
    "conf.__widget__.Frequency", "spin",
    "conf.__constraints__.Frequency", "x >= 1",
    "conf.__widget__.Gain", "spin",
    "conf.__constraints__.Gain", "x >= 1",
    "conf.__widget__.Mode", "radio",
    "conf.__constraints__.Mode", "(Square, Triangle, Sin)",
    "conf.__doc__.usage", "\n  ::\n\n  $ signalgeneration\n",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
SignalGeneration::SignalGeneration(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_out_dataOut("AudioDataOut", m_out_data)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
SignalGeneration::~SignalGeneration()
{
}

RTC::ReturnCode_t SignalGeneration::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("SignalGeneration : Signal generation component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers

  // Set OutPort buffer
  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Audio data in packet."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("OutputSampleRate", m_samplerate, "16000");
  bindParameter("Frequency", m_freq, "100");
  bindParameter("Gain", m_gain, "1000");
  bindParameter("Mode", m_mode, "Square");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SignalGeneration::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  try {
    m_pbuff = 0;
    m_flg = true;
    m_cnt = 0;
    m_timer = coil::gettimeofday() - 1.0;
  } catch (...) {
    RTC_WARN(("%s", "error onActivated."));
    return RTC::RTC_ERROR;
  }

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SignalGeneration::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  coil::TimeValue now = coil::gettimeofday();
  long bufferlen = long((now - m_timer) * m_samplerate);
  if ( bufferlen <= 0 ) return RTC::RTC_OK;
  m_timer = now;
  int len = (int)( m_samplerate / m_freq );
  short *buffer = new short[bufferlen];
  double d = 0;
  if ( m_mode == "Triangle" ) {
    d = m_gain / (len / 4);
  } else if ( m_mode == "Sin" ) {
    d = M_PI * 2 / len;
  }
  for ( int i = 0; i < bufferlen; i++) {
    if ( m_mode == "Square" ) {
      if ( m_cnt < len / 2 ) {
        m_pbuff = m_gain;
      } else {
        m_pbuff = ~m_gain;
      }
    } else if ( m_mode == "Triangle" ) {
      if ( m_flg == true ) {
        m_pbuff += d;
        if (m_pbuff > m_gain) {
          m_flg = false;
          m_pbuff = m_pbuff - (d * 2);
        }
      } else {
        m_pbuff -= d;
        if (m_pbuff < ~m_gain) {
          m_flg = true;
          m_pbuff = m_pbuff + (d * 2);
        }
      }
    } else if ( m_mode == "Sin" ) {
      m_pbuff = m_gain * sin( d * m_cnt );
    } else {
      m_pbuff = 0;
    }
    m_cnt++;
    if ( m_cnt >= len ) {
      m_cnt = 0;
      if ( m_mode != "Square" ) m_pbuff = 0;
    }
    buffer[i] = (short)m_pbuff;
  }
  m_out_data.data.length(bufferlen * 2);  //!< set outport data length
  memcpy((void *)&(m_out_data.data[0]), (void *)&(buffer[0]), bufferlen * 2);
  setTimestamp(m_out_data);
  m_out_dataOut.write();
  delete [] buffer;

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SignalGeneration::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SignalGeneration::onFinalize()
{
  return RTC::RTC_OK;
}

extern "C"
{
  void SignalGenerationInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(signalgeneration_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
    	spec_intl[j] = (char *)_(signalgeneration_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<SignalGeneration>,
                           RTC::Delete<SignalGeneration>);
  }
};
