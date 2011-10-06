// -*- C++ -*-
/*!
 * @file  BeamForming.cpp
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

#include "BeamForming.h"
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
static const char* beamforming_spec[] =
  {
    "implementation_id", "BeamForming",
    "type_name",         "BeamForming",
    "description",       N_("Sound localization component using microphone array."),
    "version",           "0.0.1",
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.Mode", "Const",
    "conf.default.ConstAngle", "0",
    "conf.default.SampleRate", "16000",
    "conf.default.ChannelNumbers", "8",
    "conf.__widget__.Mode", "radio",
    "conf.__constraints__.Mode", "(Const, Variable)",
    "conf.__widget__.ConstAngle", "spin",
    "conf.__constraints__.ConstAngle", "x < 360",
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.ChannelNumbers", "spin",
    "conf.__constraints__.ChannelNumbers", "x >= 2",
    "conf.__description__.ChannelNumbers", N_("Number of audio input channels."),
    "conf.__doc__.usage", "\n  ::\n  $ beamforming\n",
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
  BeamForming *p = (BeamForming *) m_obj;
  if (m_name == "ON_BUFFER_WRITE") {
    p->RcvBuffer(data);
  }
}
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
BeamForming::BeamForming(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_micIn("mic", m_mic),
    m_angleIn("angle", m_angle),
    m_resultOut("result", m_result),
    m_paraOut("parameter", m_para)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
BeamForming::~BeamForming()
{
}

RTC::ReturnCode_t BeamForming::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("mic", m_micIn);
  /* setiing datalistener event */
  m_micIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this));
  registerInPort("angle", m_angleIn);
  // Set OutPort buffer
  registerOutPort("result", m_resultOut);
  registerOutPort("parameter", m_paraOut);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("Mode", m_mode, "Const");
  bindParameter("ConstAngle", m_anglepara, "0");
  bindParameter("SampleRate", m_rate, "16000");
  bindParameter("ChannelNumbers", m_channels, "8");

  return RTC::RTC_OK;
}

RTC::ReturnCode_t BeamForming::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  int i = 0;
  is_active = true;
  m_horizon = true;

  m_micinfo = new mic_info[m_channels];
  char fname[] = "micset.csv";
  std::ifstream stream(fname);
  if ( stream.is_open() )
  {
    float wk_x,wk_y,wk_z;
//    double wk_rad;
    char str[256];
    while ( stream.getline( str, 256 ) ) {
      if ( str[0] == '#' ) continue;
      if ( sscanf( str, "%f,%f,%f", &wk_x, &wk_y, &wk_z) == 3 ) {
        m_micinfo[i].used = true;
        m_micinfo[i].dt = 0;
        m_micinfo[i].x = (double)wk_x;
        m_micinfo[i].y = (double)wk_y;
        m_micinfo[i].z = (double)wk_z;
        if (( wk_y != 0.0 ) || ( wk_z != 0.0 )) {
          m_horizon = false;
        }
        if ( m_micinfo[i].x < 0 ) {
          if ( m_micinfo[i].y < 0 ) {
            m_micinfo[i].xy_rad = acos((double)wk_y) * -1;
          } else {
            m_micinfo[i].xy_rad = asin((double)wk_x);
          }
        } else {
          m_micinfo[i].xy_rad = acos((double)wk_y);
        }
        if ( m_micinfo[i].y < 0 ) {
          if ( m_micinfo[i].z < 0 ) {
            m_micinfo[i].yz_rad = acos((double)wk_z) * -1;
          } else {
            m_micinfo[i].yz_rad = asin((double)wk_y);
          }
        } else {
          m_micinfo[i].yz_rad = acos((double)wk_z);
        }
      }
      i++;
      if ( i>=m_channels ) break;
    }
    stream.close();
  }
  if ( m_horizon == true ) {
    for ( i = 0; i < m_channels; i++ ) {
      m_micinfo[i].xy_rad = 0;
      m_micinfo[i].yz_rad = 0;
    }
  }
  DelayFunc();
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void BeamForming::DelayFunc(void)
{
  int i = 0;
  double wk_rad;
  for ( i = 0; i < m_channels; i++ ) {
    if ( m_horizon == true ) {
      wk_rad = m_anglepara * M_PI / 180;
      double dt = sin(wk_rad) * m_micinfo[i].x;
      dt = dt / SONIC;
      dt = dt * m_rate;
      m_micinfo[i].dt = (int)dt;
    } else {
      wk_rad = abs(m_micinfo[i].xy_rad - (m_anglepara * M_PI / 180));
      if ( wk_rad > M_PI ) {
        wk_rad = (2 * M_PI) - wk_rad;
      }

      if ( wk_rad < M_PI/2 ) {
        double brank = sqrt(pow(m_micinfo[i].x, 2.0) + pow(m_micinfo[i].y, 2.0));
        double dt = (brank / cos(wk_rad) - brank) * m_rate / SONIC;
        m_micinfo[i].dt = (int)dt;
      } else {
        m_micinfo[i].used = false;
      }
    }
  }
  return;
}

void BeamForming::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
//  delete [] m_micinfo;
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void BeamForming::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    int i,j = 0;
    int length = data.data.length();
    int size = length / ( 2 * m_channels );
    if ( size < 1 ) return;

    short **buffer = new short*[m_channels];
    for ( i = 0; i < m_channels; i++ ) {
      buffer[i] = new short[size];
    }

    short wk;
    unsigned char wk0, wk1;
    for ( i = 0; i < size; i++) {
      for ( j = 0; j < m_channels; j++) {
        wk0 = (unsigned char)data.data[i*2];
        wk1 = (unsigned char)data.data[i*2+1];
        wk = (short)(wk1 << 8) + (short)wk0;
        buffer[j][i] = wk;
      }
    }
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));

    int cnt = 0;
    short *result = new short[size];
    for ( i = 0; i < size; i++) {
      double dwk = 0.0;
      for ( j = 0; j < m_channels; j++) {
        if ( m_micinfo[j].used == false ) continue;
        cnt = m_micinfo[j].dt + i;
        if ( ( cnt < 0 ) || ( cnt >= size ) ) continue;
        dwk += buffer[j][cnt];
      }
//      result[i] = (short)( dwk / ( m_channels / 2 ) );
      result[i] = (short)( dwk );
    }
    for ( i = 0; i < m_channels; i++ ) {
      delete [] buffer[i];
    }
    delete [] buffer;
    m_result.data.length(size * 2); //!< set outport data length

    for (i = 0; i < size; i++) {
      m_result.data[i*2] = (unsigned char)(result[i] & 0x00ff);
      m_result.data[i*2+1] = (unsigned char)((result[i] & 0xff00) >> 8);
    }
    setTimestamp( m_result );
    m_resultOut.write();
    delete [] result;
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t BeamForming::onExecute(RTC::UniqueId ec_id)
{
  if( m_mode == "Variable" ) {
    m_angleIn.read();
    m_anglepara = m_angle.data;
    DelayFunc();
  }
  return RTC::RTC_OK;
}

RTC::ReturnCode_t BeamForming::onDeactivated(RTC::UniqueId ec_id)
{
  try {
    is_active = false;
    delete [] m_micinfo;
    BufferClr();
  } catch (...) {
    RTC_WARN(("%s", "onDeactivated error"));
  }
  return RTC::RTC_OK;
}

RTC::ReturnCode_t BeamForming::onFinalize()
{
  is_active = false;
  BufferClr();
  return RTC::RTC_OK;
}

extern "C"
{
  void BeamFormingInit(RTC::Manager* manager)
  {
    coil::Properties profile(beamforming_spec);
    manager->registerFactory(profile,
                           RTC::Create<BeamForming>,
                           RTC::Delete<BeamForming>);
  }
};

