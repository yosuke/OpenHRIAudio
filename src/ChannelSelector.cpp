// -*- C++ -*-
/*!
 * @file  ChannelSelector.cpp
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

#include "ChannelSelector.h"
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
static const char* channelselector_spec[] =
  {
    "implementation_id", "ChannelSelector",
    "type_name",         "ChannelSelector",
    "description",       N_("Comb filter noise reduction component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.InputChannelNumbers", "4",
    "conf.default.SelectOutputChannel", "1",
    "conf.default.InputSampleByte", "int32",
    "conf.default.ReverseEndian", "OFF",
    "conf.__widget__.InputChannelNumbers", "spin",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__description__.InputChannelNumbers", N_("Number of input audio channels."),
    "conf.__widget__.SelectOutputChannel", "spin",
    "conf.__constraints__.SelectOutputChannel", "x >= 1",
    "conf.__description__.SelectOutputChannel", N_("Select output audio channel(ch1~)."),
    "conf.__widget__.InputSampleByte", "radio",
    "conf.__constraints__.InputSampleByte", "(int8, int16, int24, int32)",
    "conf.__description__.InputSampleByte", N_("Sample byte of audio capture."),
    "conf.__widget__.ReverseEndian", "radio",
    "conf.__constraints__.ReverseEndian", "(ON, OFF)",
    "conf.__doc__.usage", "\n  ::\n\n  $ channelselector\n",
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
  if ( m_name == "ON_BUFFER_WRITE" ) {
    ChannelSelector *p = (ChannelSelector *)m_obj;
    p->RcvInBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ChannelSelector::ChannelSelector(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("AudioDataIn", m_in),
    m_outOut("AudioDataOut", m_out)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ChannelSelector::~ChannelSelector()
{
}

RTC::ReturnCode_t ChannelSelector::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("ChannelSelector : Comb filter noise reduction component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_inIn);
  m_inIn.setDescription(_("Audio data input."));

  /* setiing datalistener event */
  m_inIn.addConnectorDataListener(ON_BUFFER_WRITE,
                        new DataListener("ON_BUFFER_WRITE", this));

  // Set OutPort buffer
  addOutPort("AudioDataOut", m_outOut);
  m_outOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("InputChannelNumbers", m_channels, "4");
  bindParameter("SelectOutputChannel", m_selectch, "1");
  bindParameter("InputSampleByte", m_formatstr, "int32");
  bindParameter("ReverseEndian", m_endian, "OFF");
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t ChannelSelector::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  if ( m_formatstr == "int8" ) {
    m_bytesize = 1;
  } else if ( m_formatstr == "int16" ) {
    m_bytesize = 2;
  } else if ( m_formatstr == "int24" ) {
    m_bytesize = 3;
  } else if ( m_formatstr == "int32" ) {
    m_bytesize = 4;
  } else {
    m_bytesize = 0;
  }

  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void ChannelSelector::RcvInBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( ( is_active == true ) && ( m_selectch <= m_channels ) && (m_bytesize != 0) ) {
    //! The queuing does input data.
    int skip = m_bytesize * m_channels;
    int cnt = 0;
    int length = (int)data.data.length();
//    std::cout << "input byte size = " << length << std::endl;
    m_out.data.length(length / m_channels);  //!< set outport data length
    for ( int i = m_bytesize * ( m_selectch - 1); i < length; i = i + skip ) {
      if ( m_endian == "ON" ) {
        for ( int j = 0; j < m_bytesize; j++ ) {
          m_out.data[cnt + j] = data.data[i + (m_bytesize - 1) - j];
        }
      } else {
        memcpy((void *)&(m_out.data[cnt]), (void *)&(data.data[i]), m_bytesize);
      }
      cnt += m_bytesize;
    }
    setTimestamp( m_out );
    m_outOut.write();
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

//! order : dft->pre-emphasis->idft->repetition addition
RTC::ReturnCode_t ChannelSelector::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

RTC::ReturnCode_t ChannelSelector::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t ChannelSelector::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void ChannelSelectorInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(channelselector_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
    	spec_intl[j] = (char *)_(channelselector_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<ChannelSelector>,
                           RTC::Delete<ChannelSelector>);
  }
};
