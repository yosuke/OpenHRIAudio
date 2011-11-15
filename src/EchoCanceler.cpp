// -*- C++ -*-
/*!
 * @file  EchoCanceler.cpp
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

#include "EchoCanceler.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

// Module specification
// <rtc-template block="module_spec">
static const char* echocanceler_spec[] =
  {
    "implementation_id", "EchoCanceler",
    "type_name",         "EchoCanceler",
    "description",       N_("Acoustic echo cancellation component using adaptive filter"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.__doc__.usage", "\n  ::\n\n  $ echocanceler\n",
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
  EchoCanceler *p = (EchoCanceler *) m_obj;
  if ( m_name == "ON_BUFFER_WRITE_N" ) {
    p->RcvInBuffer(data);
  } else if ( m_name == "ON_BUFFER_WRITE_F" ) {
    p->RcvOutBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
EchoCanceler::EchoCanceler(RTC::Manager* manager)
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
EchoCanceler::~EchoCanceler()
{
}

RTC::ReturnCode_t EchoCanceler::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("EchoCanceler : Acoustic echo cancellation component using adaptive filter"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_ninIn);
  addInPort("ReferenceAudioDataIn", m_finIn);

  /* setiing datalistener event */
  m_ninIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE_N", this));
  m_ninIn.setDescription(_("Audio data input (from mic)."));
  m_finIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE_F", this));
  m_finIn.setDescription(_("Referenct audio data input (from AudioOuput component)."));
  // Set OutPort buffer
  registerOutPort("AudioDataOut", m_foutOut);
  m_foutOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  mp_sest = NULL;
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t EchoCanceler::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  BufferClr();
  if ( mp_sest == NULL ) {
    int sampleRate = 16000;
    mp_sest = speex_echo_state_init(ECHOLEN, sampleRate * 2);
    speex_echo_ctl(mp_sest, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
  }
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void EchoCanceler::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if (!m_outdata.empty()) {
    m_outdata.clear(); //!< queue buffer clear
  }
  if (!m_indata.empty()) {
    m_indata.clear(); //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void EchoCanceler::RcvInBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if (is_active == true) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));
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

void EchoCanceler::RcvOutBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvOutBuffer start"));
  if (is_active == true) {
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

RTC::ReturnCode_t EchoCanceler::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if((m_indata.size() > BUFFER_MAX) || (m_outdata.size() > BUFFER_MAX)) {
    RTC_INFO(("One of buffers exceeded the maximum value. Start clear buffers."));
    BufferClr();
  }
  if (( m_indata.size() >= ECHOLEN) && (m_outdata.size() >= ECHOLEN)) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int i;
    short *inbuffer = new short[ECHOLEN];
    short *outbuffer = new short[ECHOLEN];
    short *result = new short[ECHOLEN];

    for ( i = 0; i < ECHOLEN; i++ ) {
      inbuffer[i] = m_indata.front();
      m_indata.pop_front();
      outbuffer[i] = m_outdata.front();
      m_outdata.pop_front();
      result[i] = 0;
    }

    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
    speex_echo_cancellation(mp_sest, inbuffer, outbuffer, result);

    delete[] inbuffer;
    delete[] outbuffer;
    m_fout.data.length(ECHOLEN * 2);
    for ( i = 0; i < ECHOLEN; i++ ) {
      short val = result[i];
      m_fout.data[i*2]   = (unsigned char)(val & 0x00ff);
      m_fout.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
    }
    delete[] result;
    setTimestamp( m_fout );
    m_foutOut.write();
    RTC_DEBUG(("onExecute:writing %d samples", m_fout.data.length() / 2));
  } else {
    RTC_DEBUG(("onExecute:either or both the buffer is under limit"));
  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t EchoCanceler::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  try {
    is_active = false;
    BufferClr();
    if ( mp_sest != NULL ) {
      speex_echo_state_destroy(mp_sest);
      mp_sest = NULL;
    }
  } catch (...) {
    RTC_WARN(("ec_deinit() failed onDeactivated"));
  }
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t EchoCanceler::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void EchoCancelerInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(echocanceler_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(echocanceler_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<EchoCanceler>,
                           RTC::Delete<EchoCanceler>);
  }
};
