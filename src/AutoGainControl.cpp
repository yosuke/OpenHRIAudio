// -*- C++ -*-
/*!
 * @file  AutoGainControl.cpp
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

#include "AutoGainControl.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"
//#include <intl.h>

// Module specification
// <rtc-template block="module_spec">
static const char* autogaincontrol_spec[] =
  {
    "implementation_id", "AutoGainControl",
    "type_name",         "AutoGainControl",
    "description",       N_("Automatic gain control component"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.InputSampleByte", "int16",
    "conf.default.InputChannelNumbers", "1",
    "conf.default.AutomaticGainControl", "100",
    "conf.default.GainCtlSampleByte", "int16",
    "conf.__widget__.InputSampleByte", "radio",
    "conf.__constraints__.InputSampleByte", "(int8, int16, int24,int32)",
    "conf.__description__.InputSampleByte", N_("Sample byte of audio capture."),
    "conf.__widget__.InputChannelNumbers", "spin",
    "conf.__constraints__.InputChannelNumbers", "x >= 1",
    "conf.__description__.InputChannelNumbers", N_("Number of audio channel."),
    "conf.__widget__.AutomaticGainControl", "spin",
    "conf.__constraints__.AutomaticGainControl", "x >= 0",
    "conf.__description__.AutomaticGainControl", N_("Automatic gain control parameter."),
    "conf.__widget__.GainCtlSampleByte", "radio",
    "conf.__constraints__.GainCtlSampleByte", "(int8, int16, int24, int32)",
    "conf.__description__.GainCtlSampleByte", N_("Sample byte of gain control."),
    "conf.__doc__.usage", "\n  ::\n\n  $ autogaincontrol\n",
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

void DataListener::operator ()(const ConnectorInfo& info, const TimedOctetSeq& data)
{
  if ( m_name == "ON_BUFFER_WRITE" ) {
      AutoGainControl *p = (AutoGainControl *)m_obj;
      p->RcvBuffer(data);
    }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
AutoGainControl::AutoGainControl(RTC::Manager* manager)
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
AutoGainControl::~AutoGainControl()
{
}

RTC::ReturnCode_t AutoGainControl::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("AutoGainControl : Audio input component using pulseaudio api"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_in_dataIn);
  m_in_dataIn.setDescription(_("Audio data input."));

  /* setting datalistener event */
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this), false);

  // Set OutPort buffer
  m_out_data.tm.nsec = 0;
  m_out_data.tm.sec = 0;
  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Gain."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("InputSampleByte", m_formatstr, "int16");
  bindParameter("InputChannelNumbers", m_channels, "1");
  bindParameter("AutomaticGainControl", m_agc, "100");
  bindParameter("GainCtlSampleByte", m_gainstr, "int16");

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t AutoGainControl::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  m_gain = 1;
  m_div1 = m_div2 = 0;
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

#if 0
void AutoGainControl::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( ( is_active == true ) && ( m_agc != 0) ) {
    //! The queuing does input data.
    int i,num;
    double wk = 0;
    unsigned long len = data.data.length();  //!< inport data length

    if ( m_formatstr == "int8" ) {
      RTC_DEBUG(("int8 input data process start."));
      num = (int)len;
      char *buffer = new char[num];
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);
      for ( i = 0; i < num; i++ ) {
        wk += pow( (double)buffer[i], 2.0);
      }
      delete [] buffer;
      RTC_DEBUG(("int8 input data process finish."));

    } else if ( m_formatstr == "int16" ) {
      RTC_DEBUG(("int16 input data process start."));
      num = (int)(len / 2);
      short *buffer = new short[num];
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);
      for ( i = 0; i < num; i++ ) {
        wk += pow( (double)buffer[i], 2.0);
      }
      delete [] buffer;
      RTC_DEBUG(("int16 input data process finish."));

    } else if ( m_formatstr == "int24" ) {
      RTC_DEBUG(("int24 input data process start."));
      num = (int)(len / 3);
      char *buffer = new char[len];
      int cnt = 0;
      long lwk;
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);
      for ( i = 0; i < num; i++ ) {
        memcpy((void *)&(lwk), (void *)&(buffer[cnt]), 3);
        wk += pow( (double)lwk, 2.0);
        cnt += 3;
      }
      delete [] buffer;
/*
      long *buffer = new long[num];
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);
       for ( i = 0; i < num; i++ ) {
        wk += pow( (double)buffer[i], 2.0);
      }
      delete [] buffer;
*/
      RTC_DEBUG(("int24 input data process finish."));

    } else {
      RTC_DEBUG(("int16 input data process start."));
      num = (int)(len / 2);
      short *buffer = new short[num];
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);
      for ( i = 0; i < num; i++ ) {
        wk += pow( (double)buffer[i], 2.0);
      }
      delete [] buffer;
      RTC_DEBUG(("int16 input data process finish."));
    }

    wk = sqrt( wk / num );
    wk = wk - m_agc;
//    double d = 0.0001 * ( wk - m_div1 ) + 0.0001 * wk + 0.00003 * ( ( wk - m_div1 ) - ( m_div1 - m_div2 ) );
    double d = 0.0001 * ( wk - m_div1 );
    m_gain = m_gain + d;
    m_div2 = m_div1;
    m_div1 = wk;

//    std::cout << "div = " << wk << ", gain = " << m_gain << std::endl;
//    RTC_DEBUG(("AutomaticGainControl:value = %8.3lf, gain = %8.3lf", wk, m_gain));
  }

  return;
}
#endif
void AutoGainControl::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("AudioDataIn port:ON_BUFFER_WRITE"));
  if ( ( is_active == true ) && ( m_agc != 0) ) {
    //! The queuing does input data.
    unsigned char val[4];
    int length = (int)data.data.length();
    int i;
    double sum = 0;
    
    if ( m_gainstr == "int8" ) {
      RTC_DEBUG(("int8 input data process start."));
      for ( i = 0; i < length; i++ ) {
        val[0] = (unsigned char)data.data[i];
        sum += pow( (double)val[0], 2.0);
      }
      RTC_DEBUG(("int8 input data process finish."));
      
    } else if ( m_gainstr == "int16" ) {
      RTC_DEBUG(("int16 input data process start."));
      for ( i = 0; i < length/2; i++ ) {
        val[0] = (unsigned char)data.data[i*2];
        val[1] = (unsigned char)data.data[i*2+1];
        short wk = (short)val[0] + (short)(val[1] << 8);
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int16 input data process finish."));
      
    } else if ( m_gainstr == "int24" ) {
      RTC_DEBUG(("int24 input data process start."));
      for ( i = 0; i < length/3; i++ ) {
        val[0] = (unsigned char)data.data[i*3];
        val[1] = (unsigned char)data.data[i*3+1];
        val[2] = (unsigned char)data.data[i*3+2];
        long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
        if ( wk > 8388607 ) {
          wk = wk - 16777215;
        }
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int24 input data process finish."));
      
    } else if ( m_gainstr == "int32" ) {
      RTC_DEBUG(("int24 input data process start."));
      for ( i = 0; i < length/4; i++ ) {
        val[0] = (unsigned char)data.data[i*4];
        val[1] = (unsigned char)data.data[i*4+1];
        val[2] = (unsigned char)data.data[i*4+2];
        val[3] = (unsigned char)data.data[i*4+3];
        long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);
        sum += pow( (double)wk, 2.0);
      }
      RTC_DEBUG(("int24 input data process finish."));
      
    } else {
      sum = 0;
    }
    sum = sqrt( sum / length );
    sum = sum - m_agc;
    double d = 0.0001 * ( sum - m_div1 );
    m_gain = m_gain + d;
    m_div2 = m_div1;
    m_div1 = sum;
  }
  return;
}

RTC::ReturnCode_t AutoGainControl::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  m_out_data.data = m_gain;
  setTimestamp( m_out_data );
  m_out_dataOut.write();
  RTC_DEBUG(("AudioDataOut port:ON_BUFFER_WRITE"));
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t AutoGainControl::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t AutoGainControl::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void AutoGainControlInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(autogaincontrol_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = _((char *)autogaincontrol_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<AutoGainControl>,
                           RTC::Delete<AutoGainControl>);
  }
};
