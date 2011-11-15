// -*- C++ -*-
/*!
 * @file  DSArray.cpp
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

#include "DSArray.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

// use speex's internal fft functions
extern "C" {
  void *spx_fft_init(int size);
  void spx_fft_destroy(void *table);
  void spx_fft_float(void *table, float *in, float *out);
  void spx_ifft_float(void *table, float *in, float *out);
}

// Module specification
// <rtc-template block="module_spec">
static const char* dsarray_spec[] =
  {
    "implementation_id", "DSArray",
    "type_name",         "DSArray",
    "description",       N_("Sound localization component using microphone array."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.SampleRate", "16000",
    "conf.default.ChannelNumbers", "4",
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.ChannelNumbers", "spin",
    "conf.__constraints__.ChannelNumbers", "x >= 2",
    "conf.__description__.ChannelNumbers", N_("Number of audio input channels."),
    "conf.__doc__.usage", "\n  ::\n\n  $ dsarray\n",
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
  DSArray *p = (DSArray *) m_obj;
  if (m_name == "ON_BUFFER_WRITE") {
    p->RcvBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
DSArray::DSArray(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_micIn ( "mic", m_mic ),
    m_resultOut ( "result", m_result )

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
DSArray::~DSArray()
{
}
int DSArray::ccf(short *base, short *data)
{
  int i;
  int cnt = 0;
  float max = 0;
  float *out0 = new float[WINLEN];
  float *out1 = new float[WINLEN];
  float *fft0 = new float[WINLEN*2];
  float *fft1 = new float[WINLEN*2];
  for ( i = 0; i < WINLEN; i++ ) {
    out0[i] = (float)base[i];
    out1[i] = (float)data[i];
  }
  ApplyWindowFloat(WINLEN, window, out0);
  ApplyWindowFloat(WINLEN, window, out1);
  spx_fft_float(fft, out0, fft0);
  spx_fft_float(fft, out1, fft1);
  for ( i = 0; i < WINLEN*2; i++ ) {
    fft0[i] = fft0[i] * fft1[i];
  }
  spx_ifft_float(fft, fft0, out0);

  for ( i = 0; i < SEARCHMAX; i++ ) {
    if ( max < out0[i] ) {
      max = out0[i];
      cnt = i;
    }
  }
#if 0
/* 平滑化処理 */
  out1[0] = 0;
  out1[1] = 0;
  for ( i = 2; i < WINLEN-2; i++ ) {
    out1[i] = 0;
    for ( j = -2; j < 3; j++) {
      out1[i] += out0[i+j];
    }
    out1[i] /= 5;
  }
/*  平滑化処理  */
/* ピーク検知：限定区間最大値 */
  for ( i = 0; i < 200; i++ ) {
    if ( max < out1[i] ) {
      max = out1[i];
      cnt = i;
    }
  }
/* ピーク検知：限定区間最大値 */
/* ピーク検知：符号反転
  for ( i = 1; i < WINLEN; i++ ) {
    if ( (out1[i] - out1[i-1] ) < 0 ) {
      cnt = i;
      break;
    }
  }
 ピーク検知：符号反転 */
#endif
  delete[] out0;
  delete[] out1;
  delete[] fft0;
  delete[] fft1;
  return cnt;
}

int DSArray::CrossCorrelation(short *base, short *data)
{
  int i,j;
  int cnt = SEARCHMAX;
  long max = WINLEN*2*700;
  long sum[SEARCHMAX];
  for ( i = 0; i < SEARCHMAX; i++ ) {
    sum[i] = 0;
    for ( j = 0; j+SEARCHMAX < WINLEN; j++ ) {
      sum[i] += abs(base[j] + data[i+j]);
    }
	if ( max < sum[i] ) {
      max = sum[i];
      cnt = i;
    }
  }
//  std::cout << "max = " << max <<  std::endl;
  return cnt;
}

RTC::ReturnCode_t DSArray::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO((" DSArray : Sound localization component using microphone array"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("mic", m_micIn);
  m_micIn.setDescription(_("Audio data input."));
  /* setiing datalistener event */
  m_micIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this));
  // Set OutPort buffer
  registerOutPort("result", m_resultOut);
  m_resultOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("SampleRate", m_rate, "16000");
  bindParameter("ChannelNumbers", m_channels, "4");

  fft = NULL;
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t DSArray::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  is_active = true;
  m_horizon = true;
  int i = 0;
  BufferClr();

  m_micinfo = new mic_info[m_channels];
  char fname[] = "micset.csv";
  std::ifstream stream(fname);
  if ( stream.is_open() ) {
    float wk_x,wk_y,wk_z;
    double dwk;
    char str[256];
    while ( stream.getline( str, 256 ) ) {
      if ( str[0] == '#' ) continue;
      if ( sscanf( str, "%f,%f,%f", &wk_x, &wk_y, &wk_z ) == 3 ) {
        m_micinfo[i].x = (double)wk_x;
        m_micinfo[i].y = (double)wk_y;
        m_micinfo[i].z = (double)wk_z;
	if (( wk_y != 0.0 ) || ( wk_z != 0.0 )) {
          m_horizon = false;
        }
        dwk = sqrt( wk_x * wk_x + wk_y * wk_y );
        if ( m_micinfo[i].x < 0 ) {
          if ( m_micinfo[i].y < 0) {
            m_micinfo[i].xy_rad = acos( (double)wk_y/dwk ) * -1;
          } else {
            m_micinfo[i].xy_rad = asin( (double)wk_x/dwk );
           }
        } else {
          m_micinfo[i].xy_rad = acos( (double)wk_y/dwk );
        }
        dwk = sqrt( wk_y * wk_y + wk_z * wk_z );
        if ( m_micinfo[i].y < 0 ) {
          if ( m_micinfo[i].z < 0 ) {
            m_micinfo[i].yz_rad = acos( (double)wk_z/dwk ) * -1;
          } else {
            m_micinfo[i].yz_rad = asin( (double)wk_y/dwk );
          }
        } else {
          m_micinfo[i].yz_rad = acos( (double)wk_z/dwk );
        }
      }
      RTC_INFO(("mic %i angle = %f [deg]", i, m_micinfo[i].xy_rad * 180 / M_PI));
      std::cout << "mic " << i << " : (" << m_micinfo[i].x << "," << m_micinfo[i].y << "," << m_micinfo[i].z << ") angle " << m_micinfo[i].xy_rad * 180 / M_PI <<  "[deg]" << std::endl;
      i++;
      if ( i >= m_channels ) break;
    }
    stream.close();
  }
  if ( m_horizon == true ) {
    for ( i = 0; i < m_channels; i++ ) {
      m_micinfo[i].xy_rad = 0;
      m_micinfo[i].yz_rad = 0;
    }
  }
  window = CreateWindowFloat(WINLEN, Hamming);
//  windowd = CreateWindowDouble(WINLEN, Hamming);
  fft = spx_fft_init(WINLEN);
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void DSArray::BufferClr(void)
{
  RTC_DEBUG(("BufferClr start"));
  m_mutex.lock();
  RTC_DEBUG(("BufferClr:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if ( !m_data.empty() ) {
    m_data.clear(); //!< queue buffer clear
  }
  m_mutex.unlock();
  RTC_DEBUG(("BufferClr:mutex unlock"));
  RTC_DEBUG(("BufferClr finish"));
}

void DSArray::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if ( is_active == true ) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    int length = data.data.length();
    short wk;
    unsigned char wk0, wk1;
    for (int i = 0; i < length/2; i++) {
      wk0 = (unsigned char)data.data[i*2];
      wk1 = (unsigned char)data.data[i*2+1];
      wk = (short)(wk1 << 8) + (short)wk0;
      m_data.push_back(wk);
    }

#if 0
    if ( m_channels == 8 ) {
      int center_mic[5];
      for ( int k = 0; k < 5 ; k++ ) {
        center_mic[k] = (int)(m_angle + k);
        if(center_mic[k] > 7) center_mic[k] = center_mic[k] - 8;
      }
      unsigned long len = data.data.length() ; //!< inport data length
      int num = (int) len / 2;
      short *buffer = new short[num];
      memcpy( (void *) &(buffer[0]), (void *) &(data.data[0]), len );
      for ( int i = 0; i < num; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
          if ( j == center_mic[2] ) {
            m_data.push_back( buffer[i] );
          } else if ( ( j == center_mic[1] ) || ( j == center_mic[3] ) ) {
            if ( i < 13 ) {
              m_data.push_back(0);
            } else {
              m_data.push_back(buffer[i-13]);
            }
          } else if ( ( j == center_mic[0] ) || ( j == center_mic[4] ) ) {
            if ( i < 47 ) {
              m_data.push_back(0);
            } else {
              m_data.push_back(buffer[i-47]);
            }
          } else {
            m_data.push_back(0);
          }
        }
      }
      delete[] buffer;
    } else if ( m_channels == 2 ) {
      unsigned long len = data.data.length() ; //!< inport data length
      int num = (int) len / 2;
      short *buffer = new short[num];
      memcpy( (void *) &(buffer[0]), (void *) &(data.data[0]), len );
      for ( int i = 0; i < num; i++ ) {
        m_data.push_back( buffer[i] );
        m_data.push_back( buffer[i] );
      }
      delete[] buffer;
    }
#endif
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t DSArray::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( (int)m_data.size() > ( m_channels * WINLEN ) ) {
//  if ( ( !m_data.empty() ) && ( m_data.size() > ( m_channels * WINLEN ) ) ) {
    m_mutex.lock();
    double dwk;
    double angle,angle0,angle1,deg;
    int cnt = 0;
    int i,j = 0;
    int dt,dtmin,micnum;
    short **buffer = new short*[m_channels];

    for ( i = 0; i < m_channels; i++ ) {
      buffer[i] = new short[WINLEN];
    }

    for ( i = 0; i < WINLEN; i++ ) {
      for ( j = 0; j < m_channels; j++ ) {
        buffer[j][i] = m_data.front();
        m_data.pop_front();
      }
    }
    m_mutex.unlock();

    deg = 0;
    cnt = 0;
    dtmin = SEARCHMAX;
    for ( i = 0; i < m_channels; i++ ) {
      angle0 = m_micinfo[i].xy_rad;

//      for ( j = i + 1; j < m_channels; j++ ) {
      for ( j = 0; j < m_channels; j++ ) {
        if ( i == j ) continue;
        angle1 = m_micinfo[j].xy_rad;
        if ( fabs( angle0 ) > fabs( angle1 ) ) {
          angle = angle0;
        } else {
          angle = angle1;
        }
/*        dt = ccf(buffer[i], buffer[j]);*/
        dt = CrossCorrelation(buffer[i], buffer[j]);
//        if ( ( dt == 0 ) || ( dt == SEARCHMAX ) ) continue;
        if ( dt == SEARCHMAX ) continue;
        if ( dtmin > dt ) {
          dtmin = dt;
          micnum = j;
        }
//        std::cout << "mic = " << i << " - " << j << " dt = " << dt << std::endl;
        /* 理論値：ch0=47,ch1=13,ch2=0,ch3=13,ch4=47 */
        dwk = sqrt(pow((m_micinfo[i].x - m_micinfo[j].x), 2.0) + pow((m_micinfo[i].y - m_micinfo[j].y), 2.0));
        dwk = dt * SONIC / dwk / m_rate;
        if ( (dwk > 1 ) || ( dwk < -1 ) ) continue;
        angle = asin(dwk);

        if ( ( angle0 == 0 ) && ( angle1 == 0 ) ) {
          if ( m_micinfo[i].x < m_micinfo[j].x ) angle *= -1;
        } else {
          if ( ( abs(angle0) > M_PI/2 ) || ( abs(angle1) > M_PI/2 ) ) {
            if ( ( angle0 > 0 ) && ( angle1 < 0 ) ) {
              angle1 = angle1 + 2*M_PI;
            } else if ( ( angle0 < 0 ) && ( angle1 > 0 ) ) {
              angle1 = angle1 - 2*M_PI;
            }
          }
          if ( angle0 < angle1 ) {
            angle = angle0 + (angle1 - angle0)/2 - angle;
          } else {
            angle = angle1 + (angle0 - angle1)/2 + angle;
          }
        }
//		std::cout << "mic = " << i << " - " << j << ": dt = " << dt << ", angle = " << angle * 180 / M_PI << "[°]" << std::endl;
        deg += angle;
        cnt++;
      }
    }
//    if ( ( micnum <  m_channels - 1 ) && ( m_horizon == true ) ) {
//      deg *= -1;
//    }
//    if ( cnt > 2 ) {
    if ( cnt > 0 ) {
      deg = deg / cnt;
      deg = deg * 180 / M_PI;
      std::cout << " angle = " << deg << "[°]" << std::endl;
      m_result.data = deg;
      setTimestamp( m_result );
      m_resultOut.write();
    }

    for ( i = 0; i < m_channels; i++ ) {
      delete[] buffer[i];
    }
    delete[] buffer;
  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

#if 0
RTC::ReturnCode_t DSArray::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( !m_data.empty() ) {
    m_mutex.lock();
    int i,j,k = 0;
    int fastmic = m_channels;
    int *trgcnt = new int[m_channels];
    int size = m_data.size();
    int num = size / m_channels;
    short **buffer = new short*[m_channels];
    short *result = new short[size];

    for ( i = 0; i < m_channels; i++ ) {
      buffer[i] = new short[num];
      trgcnt[i] = num;
    }

    for ( i = 0; i < num; i++ ) {
      for ( j = 0; j < m_channels; j++ ) {
        buffer[j][i] = result[k++] = m_data.front();
        m_data.pop_front();
        if ( ( buffer[j][i] > TRIGGER_VOL ) && ( trgcnt[j] == num ) ) {
          trgcnt[j] = i;
          if ( fastmic == m_channels ) fastmic = j;
        }
      }
    }
    m_mutex.unlock();

    for ( i = 0; i < m_channels; i++ ) {
      if ( ( trgcnt[i] == num ) || ( i == fastmic ) ) continue;
      int r;
      r = ccf(buffer[fastmic], buffer[i]);
      RTC_INFO(("mic %i-%i diff = %d, ccf = %d", fastmic, i, trgcnt[i], r));
    }

    if ( fastmic != m_channels ) {
      int cnt = 0;
      double dt = 0;
      double brank = 0;
      double angle = 0;
      double deg = 0;

      for ( i = 0; i < m_channels; i++ ) {
        if ( ( trgcnt[i] == num ) || ( i == fastmic ) ) continue;
        brank = sqrt(pow((m_micinfo[fastmic].x - m_micinfo[i].x), 2.0) + pow((m_micinfo[fastmic].y - m_micinfo[i].y), 2.0));
//      angle = asin(1700*dt/16000);/* sinΘ=Δt・c/d （c=340m/s,d=0.2）*/
//      angle = asin(dt * SONIC / brank / m_rate);
        dt = trgcnt[i] * SONIC / brank / m_rate;
        if ( (dt > 1 ) || ( dt < -1 ) ) continue;
        angle = asin(dt);
        double angle0 = m_micinfo[fastmic].xy_rad;
        double angle1 = m_micinfo[i].xy_rad;
        if ( ( abs(angle0) > M_PI/2 ) || ( abs(angle1) > M_PI/2 ) ) {
          if ( ( angle0 > 0 ) && ( angle1 < 0 ) ) {
            angle1 = angle1 + 2*M_PI;
          } else if ( ( angle0 < 0 ) && ( angle1 > 0 ) ) {
            angle1 = angle1 - 2*M_PI;
          }
        }
        if ( angle0 < angle1 ) {
          angle = angle0 + (angle1 - angle0)/2 - angle;
        } else {
          angle = angle1 + (angle0 - angle1)/2 + angle;
        }
        deg += angle;
        RTC_DEBUG(("mic %i-%i angle = %f [deg]", fastmic, i, angle * 180 / M_PI));
        cnt++;
      }
      if ( cnt == 0 ) {
        angle = m_micinfo[fastmic].xy_rad;
        deg = angle * 180 / M_PI;
      } else {
        deg = deg / cnt;
        deg = deg * 180 / M_PI;
      }
      RTC_DEBUG(("angle = %f [deg]", deg));
    }

    m_result.data.length(size * 2); //!< set outport data length
    memcpy((void *) &(m_result.data[0]), (void *) &(result[0]), size * 2);
    setTimestamp( m_result );
    m_resultOut.write();

    for ( i = 0; i < m_channels; i++ ) {
      delete[] buffer[i];
    }
    delete[] buffer;
    delete[] result;
    delete[] trgcnt;
  }

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}
#endif

RTC::ReturnCode_t DSArray::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  try {
    is_active = false;
    BufferClr();
    delete[] m_micinfo;
  } catch (...) {
    RTC_WARN(("%s", "onDeactivated error"));
  }
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t DSArray::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  BufferClr();
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void DSArrayInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(dsarray_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = _((char *)dsarray_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<DSArray>,
                           RTC::Delete<DSArray>);
  }
};

