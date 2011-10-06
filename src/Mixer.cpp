// -*- C++ -*-
/*!
 * @file  Mixer.cpp
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

#include "Mixer.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "../config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

#include <coil/stringutil.h>

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v)
{
  std::string s;
  std::vector<std::string> sv;
  is >> s;
  sv = coil::split(s ,",");
  v.resize(sv.size());
  for (int i(0), len(sv.size()); i < len; ++i) {
    T tv;
    if (coil::stringTo(tv, sv[i].c_str())) {
      v[i] = tv;
    }
  }
  return is;
}

// Module specification
// <rtc-template block="module_spec">
static const char* mixer_spec[] =
  {
    "implementation_id", "Mixer",
    "type_name",         "Mixer",
    "description",       N_("Audio mixer component"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.OutputChannelNumbers", "1",
    "conf.__widget__.OutputChannelNumbers", "spin",
    "conf.__constraints__.OutputChannelNumbers", "x >= 1",
    "conf.__description__.OutputChannelNumbers", N_("Number of output audio channels."),
    "conf.default.MixGains", "0.5,0.5",
    "conf.__description__.MixGains", N_("Gains for mixing signals."),
    "conf.__doc__.usage", "\n  ::\n\n  $ mixer\n",
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
  Mixer *p = (Mixer *)m_obj;
  if( m_name == "ON_BUFFER_WRITE_N" ) {
    p->RcvInBuffer(data);
  } else if( m_name == "ON_BUFFER_WRITE_F" ) {
    p->RcvOutBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Mixer::Mixer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_ninIn("AudioDataIn", m_nin),
    m_finIn("ReferenceAudioDataIn", m_fin),
    m_foutOut("AudioDataOut", m_fout)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Mixer::~Mixer()
{
}

RTC::ReturnCode_t Mixer::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("Mixer : Audio mixer component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_ninIn);
  m_ninIn.setDescription(_("Audio data input (from mic)."));
  addInPort("ReferenceAudioDataIn", m_finIn);
  m_finIn.setDescription(_("Referenct audio data input (from AudioOuput component)."));

  /* setiing datalistener event */
  m_ninIn.addConnectorDataListener(ON_BUFFER_WRITE,
                        new DataListener("ON_BUFFER_WRITE_N", this));
  m_finIn.addConnectorDataListener(ON_BUFFER_WRITE,
                        new DataListener("ON_BUFFER_WRITE_F", this));

  // Set OutPort buffer
//  registerOutPort("foutput", m_foutOut);
  addOutPort("AudioDataOut", m_foutOut);
  m_foutOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("OutputChannelNumbers", m_channels, "1");
  bindParameter("MixGains", m_gains, "0.5,0.5");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Mixer::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  BufferClr();
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void Mixer::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if ( !m_indata.empty() ) {
    m_indata.clear();  //!< queue buffer clear
  }
  if ( !m_outdata.empty() ) {
    m_outdata.clear();  //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void Mixer::RcvInBuffer(TimedOctetSeq data)
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
      m_indata.push_back(wk);
    }
    RTC_DEBUG(("Input1:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_indata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

void Mixer::RcvOutBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvOutBuffer start"));
  if ( is_active == true ) {
    //! The queuing does input data.
    m_mutex.lock();
    RTC_DEBUG(("RcvOutBuffer:mutex lock"));
    unsigned char wk0, wk1;
    short wk;
    int length = data.data.length(); //!< inport data length
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_outdata.push_back(wk);
    }
    RTC_DEBUG(("Input2:size %d, time %d:%d, buffer %d", length/2, data.tm.sec, data.tm.nsec, m_outdata.size()));
    m_mutex.unlock();
    RTC_DEBUG(("RcvOutBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvOutBuffer finish"));
  return;
}

RTC::ReturnCode_t Mixer::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if( ( m_indata.size() > BUFFER_MAX ) || ( m_outdata.size() > BUFFER_MAX ) ) {
    RTC_INFO(("One of buffers exceeded the maximum value. Start clear buffers."));
    BufferClr();
  }
  if (!m_indata.empty() && !m_outdata.empty()) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int num = (m_indata.size() < m_outdata.size()) ? m_indata.size() : m_outdata.size();
    double *buffer;
    buffer = new double[num * m_channels];
    int cnt = 0;
    for (int i = 0; i < num; i++) {
      for (int j = 0; j < m_channels; j++) {
        if (m_gains.size() > 2*j+1)
          buffer[cnt++] = m_gains[2*j] * (double)m_indata.front() + m_gains[2*j+1] * (double)m_outdata.front();
        else
          buffer[cnt++] = 0.0;
      }
      m_indata.pop_front();
      m_outdata.pop_front();
    }
    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
    m_fout.data.length(num * m_channels * 2);  //!< set outport data length
    for ( int i = 0; i < num * m_channels; i++ ) {
      short val = buffer[i];
      m_fout.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_fout.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    delete [] buffer;
    setTimestamp( m_fout );
    m_foutOut.write();
    RTC_DEBUG(("onExecute:writing %d samples", m_fout.data.length() / 2));
  } else {
    RTC_DEBUG(("onExecute:either or both the buffer is empty"));
  }
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Mixer::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Mixer::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void MixerInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(mixer_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(mixer_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                             RTC::Create<Mixer>,
                             RTC::Delete<Mixer>);
  }
};
