// -*- C++ -*-
/*!
 * @file  CombFilter.cpp
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

#include "CombFilter.h"
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
static const char* combfilter_spec[] =
  {
    "implementation_id", "CombFilter",
    "type_name",         "CombFilter",
    "description",       N_("Comb filter noise reduction component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.SampleRate", "16000",
    "conf.default.ChannelNumbers", "1",
    "conf.default.Frequency", "100",
    "conf.default.Gain", "-1",
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.ChannelNumbers", "spin",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__description__.ChannelNumbers", N_("Number of audio channel."),
    "conf.__widget__.Frequency", "spin",
    "conf.__constraints__.Frequency", "x >= 0",
    "conf.__widget__.Gain", "spin",
    "conf.__doc__.usage", "\n  ::\n\n  $ combfilter\n",
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
    CombFilter *p = (CombFilter *)m_obj;
    p->RcvInBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
CombFilter::CombFilter(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ninIn("AudioDataIn", m_nin),
    m_foutOut("AudioDataOut", m_fout)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
CombFilter::~CombFilter()
{
}

RTC::ReturnCode_t CombFilter::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("CombFilter : Comb filter noise reduction component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_ninIn);
  m_ninIn.setDescription(_("Audio data input."));

  /* setiing datalistener event */
  m_ninIn.addConnectorDataListener(ON_BUFFER_WRITE,
                        new DataListener("ON_BUFFER_WRITE", this));

  // Set OutPort buffer
  addOutPort("AudioDataOut", m_foutOut);
  m_foutOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("SampleRate", m_rate, "16000");
  bindParameter("ChannelNumbers", m_channels, "1");
  bindParameter("Frequency", m_freq, "100");
  bindParameter("Gain", m_gain, "-1");
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t CombFilter::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( !m_dbuff.empty() ) {
    m_dbuff.clear();
  }

  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void CombFilter::RcvInBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));
    //! The queuing does input data.
    unsigned char wk0, wk1;
    short wk;
    int length = data.data.length(); //!< inport data length
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_queue.push_back(wk);  //!< enqueue
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

//! order : dft->pre-emphasis->idft->repetition addition
RTC::ReturnCode_t CombFilter::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  int delay = (int)( m_rate / m_freq );
  int num = m_queue.size();
  if ( num > delay ) {
    int i,j;
    short *buffer = new short[num];
    short *dbuffer = new short[num];
    for ( j = 0; j < delay; j++ ) {
      if( !m_dbuff.empty() ) {
        dbuffer[j] = m_dbuff.front();
        m_dbuff.pop_front();  //!< dequeue
      } else {
        dbuffer[j] = 0;
       }
    }
    for ( i = 0; i < num; i++ ) {
      //! takes buffer
      buffer[i] = m_queue.front();
      m_queue.pop_front();  //!< dequeue
      if( j < num ) {
        dbuffer[j] = buffer[i];
      } else {
        m_dbuff.push_back(buffer[i]);
      }
      j++;
    }
    for ( i = 0; i < num; i++ ) {
      buffer[i] += dbuffer[i] * m_gain;
    }
    //! output the added forequarter.
    m_fout.data.length(num*2);  //!< set outport data length
    for ( i = 0; i < num; i++ ) {
      short val = buffer[i];
      m_fout.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_fout.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    setTimestamp( m_fout );
    m_foutOut.write();
    delete[] buffer;
    delete[] dbuffer;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t CombFilter::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t CombFilter::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void CombFilterInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(combfilter_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
    	spec_intl[j] = (char *)_(combfilter_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<CombFilter>,
                           RTC::Delete<CombFilter>);
  }
};
