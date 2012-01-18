// -*- C++ -*-
/*!
 * @file  WebRTCVAD.cpp
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

#include "WebRTCVAD.h"
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
static const char* webrtcvad_spec[] =
  {
    "implementation_id", "WebRTCVAD",
    "type_name",         "WebRTCVAD",
    "description",       N_("WebRTC based noise reduction component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.__doc__.usage", "\n  ::\n\n  $ noisereduction\n",
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
    WebRTCVAD *p = (WebRTCVAD *)m_obj;
    p->RcvInBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WebRTCVAD::WebRTCVAD(RTC::Manager* manager)
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
WebRTCVAD::~WebRTCVAD()
{
}

RTC::ReturnCode_t WebRTCVAD::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WebRTCVAD : WebRTC based noise reduction component"));
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

  WebRtcVad_Create(&handle);
  WebRtcVad_Init(handle);
  WebRtcVad_set_mode(handle, 2); // agressive adaptation mode

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WebRTCVAD::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  m_mutex.lock();

  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }

  m_mutex.unlock();

  is_active = true;

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void WebRTCVAD::RcvInBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvInBuffer:mutex lock"));

    int length = data.data.length();
    short wk;
    unsigned char wk0, wk1;
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_inbuffer.push_back(wk);
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvInBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

RTC::ReturnCode_t WebRTCVAD::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if (m_inbuffer.size() >= WINLEN) {
    int i;
    WebRtc_Word16 data[WINLEN];
    std::list<short>::iterator pbuffer;

    // sliding window with half overlap
    for (i = 0; i < WINLEN/2; i++) {
      data[i] = m_inbuffer.front();
      m_inbuffer.pop_front();
    }
    pbuffer = m_inbuffer.begin();
    for (i = WINLEN/2; i < WINLEN; i++) {
      data[i] = *pbuffer;
      pbuffer++;
    }

    WebRtc_Word16 vad = WebRtcVad_Process(handle, 16000, data, WINLEN);
    RTC_INFO(("vad: %i", vad));
    
    // output the resulting signal
    m_fout.data.length(WINLEN);
    if (vad > 0) {
      for (i = 0; i < WINLEN/2; i++) {
        m_fout.data[i*2] = (unsigned char)(data[i] & 0x00ff);
        m_fout.data[i*2+1] = (unsigned char)((data[i] & 0xff00) >> 8);
      }
    } else {
      for (i = 0; i < WINLEN/2; i++) {
        m_fout.data[i*2] = i % 2; // avoid julius zero stripping problem
        m_fout.data[i*2+1] = 0;
      }
    }
    setTimestamp(m_fout);
    m_foutOut.write();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WebRTCVAD::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WebRTCVAD::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void WebRTCVADInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(webrtcvad_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(webrtcvad_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                             RTC::Create<WebRTCVAD>,
                             RTC::Delete<WebRTCVAD>);
  }
};
