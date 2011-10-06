// -*- C++ -*-
/*!
 * @file  WavRecord.cpp
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

#include "WavRecord.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "../config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

#define SAMPLE_RATE 44100
#define BIT_DEPTH 16

// Module specification
// <rtc-template block="module_spec">
static const char* wavrecord_spec[] =
  {
    "implementation_id", "WavRecord",
    "type_name",         "WavRecord",
    "description",       N_("Sound record component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.SampleRate", "16000",
    "conf.default.ChannelNumbers", "1",
    "conf.default.FileName", "",
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.ChannelNumbers", "spin",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__description__.ChannelNumbers", N_("Number of audio input channels."),
    "conf.__doc__.usage", "\n  ::\n  $ wavrecord\n",
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

void DataListener::operator ()(const ConnectorInfo& info,const TimedOctetSeq& data)
{
  WavRecord *p = (WavRecord *) m_obj;
  if (m_name == "ON_BUFFER_WRITE") {
    p->RcvBuffer(data);
  }
}
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WavRecord::WavRecord(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_in_dataIn("AudioDataIn", m_in_data)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WavRecord::~WavRecord()
{
}

RTC::ReturnCode_t WavRecord::onInitialize()
{
  RTC_INFO(("WavRecord : Audio record component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this));
  registerInPort("AudioDataIn", m_in_dataIn);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("SampleRate", m_rate, "16000");
  bindParameter("ChannelNumbers", m_channels, "1");
  bindParameter("FileName", m_filename, "");
  is_active = false;

  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  sfinfo.samplerate = m_rate;
  sfinfo.channels = m_channels;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  if (sf_format_check(&sfinfo) == 0) {
    RTC_DEBUG(("invalid format"));
    return RTC::RTC_ERROR;
  }
  sfw = sf_open(m_filename.c_str(), SFM_WRITE, &sfinfo);
  if (sfw == NULL) {
    RTC_DEBUG(("unable to open file: %s", m_filename.c_str()));
    return RTC::RTC_ERROR;
  }
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void WavRecord::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if (is_active == true) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    sf_count_t frames = data.data.length();
    sf_write_raw(sfw, &data.data[0], frames);
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t WavRecord::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onDeactivated(RTC::UniqueId ec_id)
{
  is_active = false;
  sf_close(sfw);
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onFinalize()
{
  return RTC::RTC_OK;
}

extern "C"
{
  void WavRecordInit(RTC::Manager* manager)
  {
    coil::Properties profile(wavrecord_spec);
    manager->registerFactory(profile,
                             RTC::Create<WavRecord>,
                             RTC::Delete<WavRecord>);
  }
};

