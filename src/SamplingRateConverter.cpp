// -*- C++ -*-
/*!
 * @file  SamplingRateConverter.cpp
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

#include "SamplingRateConverter.h"
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
static const char* samplingrateconverter_spec[] =
  {
    "implementation_id", "SamplingRateConverter",
    "type_name",         "SamplingRateConverter",
    "description",       N_("Sampling rate converter"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "100",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.InputSampleRate", "16000",
    "conf.default.OutputSampleRate", "16000",
    "conf.default.InputBitDepth", "16",
    "conf.default.OutputBitDepth", "16",
    "conf.__widget__.InputSampleRate", "spin",
    "conf.__constraints__.InputSampleRate", "x >= 1",
    "conf.__description__.InputSampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.OutputSampleRate", "spin",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__description__.OutputSampleRate", N_("Sample rate of audio output."),
    "conf.__widget__.InputBitDepth", "radio",
    "conf.__constraints__.InputBitDepth", "(8, 16, 24, 32, float32)",
    "conf.__description__.InputBitDepth", N_("Bit depth of audio input."),
    "conf.__widget__.OutputBitDepth", "radio",
    "conf.__constraints__.OutputBitDepth", "(8, 16, 24, 32, float32)",
    "conf.__description__.OutputBitDepth", N_("Bit depth of audio output."),
    "conf.__doc__.usage", "\n  ::\n\n  $ sampleingrateconverter\n",
    ""
  };
// </rtc-template>

enum {DEPTH8, DEPTH16, DEPTH24, DEPTH32, DEPTHFLOAT32};

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
    SamplingRateConverter *p = (SamplingRateConverter *)m_obj;
    p->RcvInBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
SamplingRateConverter::SamplingRateConverter(RTC::Manager* manager)
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
SamplingRateConverter::~SamplingRateConverter()
{
}

RTC::ReturnCode_t SamplingRateConverter::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("SamplingRateConverter : Sampling rate converter"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_inIn);
  m_inIn.setDescription(_("Audio data input."));

  /* setiing datalistener event */
  m_handle = NULL;
  m_inIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this), false);

  // Set OutPort buffer
  addOutPort("AudioDataOut", m_outOut);
  m_outOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("InputSampleRate", m_in_rate, "16000");
  bindParameter("OutputSampleRate", m_out_rate, "16000");
  bindParameter("InputBitDepth", m_in_formatstr, "16");
  bindParameter("OutputBitDepth", m_out_formatstr, "16");
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SamplingRateConverter::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  m_in_format = DEPTH8;
  if ( m_in_formatstr == "8" ) {
    m_in_format = DEPTH8;
  } else if ( m_in_formatstr == "16" ) {
    m_in_format = DEPTH16;
  } else if ( m_in_formatstr == "24" ) {
    m_in_format = DEPTH24;
  } else if ( m_in_formatstr == "32" ) {
    m_in_format = DEPTH32;
  } else if ( m_in_formatstr == "float32" ) {
    m_in_format = DEPTHFLOAT32;
  } else {
    RTC_INFO(("Unsupported input format %s", m_in_formatstr.c_str()));
  }

  m_out_format = DEPTH8;
  if ( m_out_formatstr == "8" ) {
    m_out_format = DEPTH8;
  } else if ( m_out_formatstr == "16" ) {
    m_out_format = DEPTH16;
  } else if ( m_out_formatstr == "24" ) {
    m_out_format = DEPTH24;
  } else if ( m_out_formatstr == "32" ) {
    m_out_format = DEPTH32;
  } else if ( m_out_formatstr == "float32" ) {
    m_out_format = DEPTHFLOAT32;
  } else {
    RTC_INFO(("Unsupported output format %s", m_out_formatstr.c_str()));
  }

  m_ratio = (double)m_out_rate / (double)m_in_rate;
  if ( !m_handle ) {
    m_handle = resample_open(1, m_ratio, m_ratio);
  }
  is_active = true;

  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void SamplingRateConverter::RcvInBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvInBuffer start"));
  if ( is_active == true ) {
    //! The queuing does input data.
    for ( int i = 0; i < (int)data.data.length(); i++ ) {
      m_queue.push_back((unsigned char)data.data[i]);  //!< enqueue
    }
#if 0
    unsigned long len = data.data.length();  //!< inport data length
    if ( m_ratio == 1 ) {
      m_out.data.length(len);  //!< set outport data length
      memcpy((void *)&(m_out.data[0]), (void *)&(data.data[0]), len);
      m_outOut.write();
    } else {
      m_mutex.lock();
      RTC_DEBUG(("RcvInBuffer:mutex lock"));
      int num = (int)len / 2;
      short *buffer = new short[num];
      memcpy((void *)&(buffer[0]), (void *)&(data.data[0]), len);

      for ( int i = 0; i < num; i++ ) {
        m_queue.push_back(buffer[i]);  //!< enqueue
      }
      delete [] buffer;
      m_mutex.unlock();
      RTC_DEBUG(("RcvInBuffer:mutex unlock"));
    }
#endif
  }
  RTC_DEBUG(("RcvInBuffer finish"));
  return;
}

long *SamplingRateConverter::InputDataSet(int *length)
{
  RTC_DEBUG(("Output data start"));
  unsigned char val[4];
  int i;
  int size = m_queue.size();
  long *data;

  switch ( m_in_format ) {
  case DEPTH8:
    *length = size;
    data = new long[size];
    for ( i = 0; i < size; i++ ) {
      data[i] = m_queue.front();
      m_queue.pop_front();
    }
    break;
  case DEPTH16:
    *length = size/2;
    data = new long[size/2];
    for ( i = 0; i < size/2; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      short wk = (short)val[0] + (short)(val[1] << 8);
      data[i] = wk;
    }
    break;
  case DEPTH24:
    *length = size/3;
    data = new long[size/3];
    for ( i = 0; i < size/3; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
      if ( wk > 8388607 ) {
        wk = wk - 16777215;
      }
      data[i] = wk;
    }
    break;
  case DEPTH32:
    *length = size/4;
    data = new long[size/4];
    for ( i = 0; i < size/4; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      val[3] = m_queue.front();
      m_queue.pop_front();
//      int wk = (int)val[0] + (int)(val[1] << 8) + (int)(val[2] << 16) + (int)(val[3] << 24);
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);
//      if ( ( wk & 0x80000000 ) != 0 ) {
//        wk = ~(wk & 0x7fffffff);
//      }
      data[i] = wk;
//      std::string str("data %ld :%x %x %x %x", data[i], val0, val1, val2, val3);
//      std::cout << str << std::endl;
    }
    break;
  case DEPTHFLOAT32:
    data = NULL;
    break;
  default:
    data = NULL;
    break;
  }
  return data;
}

float *SamplingRateConverter::InputDataSetFloat(int *length)
{
  RTC_DEBUG(("Output data start"));
  unsigned char val[4];
  int i;
  int size = m_queue.size();
  float *data;

  switch ( m_in_format ) {
  case DEPTH8:
    *length = size;
    data = new float[size];
    for ( i = 0; i < size; i++ ) {
      data[i] = (float)m_queue.front();
      m_queue.pop_front();
    }
    break;
  case DEPTH16:
    *length = size/2;
    data = new float[size/2];
    for ( i = 0; i < size/2; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      short wk = (short)val[0] + (short)(val[1] << 8);
      data[i] = (float)wk;
    }
    break;
  case DEPTH24:
    *length = size/3;
    data = new float[size/3];
    for ( i = 0; i < size/3; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16);
      if ( ( wk & 0x00800000 ) != 0 ) {
        wk = ~(wk & 0x007fffff);
      }
      data[i] = (float)wk;
    }
    break;
  case DEPTH32:
    *length = size/4;
    data = new float[size/4];
    for ( i = 0; i < size/4; i++ ) {
      val[0] = m_queue.front();
      m_queue.pop_front();
      val[1] = m_queue.front();
      m_queue.pop_front();
      val[2] = m_queue.front();
      m_queue.pop_front();
      val[3] = m_queue.front();
      m_queue.pop_front();
      long wk = (long)val[0] + (long)(val[1] << 8) + (long)(val[2] << 16) + (long)(val[3] << 24);
      if ( ( wk & 0x80000000 ) != 0 ) {
        wk = ~(wk & 0x7fffffff);
      }
      data[i] = (float)wk;
    }
    break;
  case DEPTHFLOAT32:
    data = NULL;
    break;
  default:
    data = NULL;
    break;
  }
  return data;
}

void SamplingRateConverter::OutputDataWriteFloat(float *data, int length)
{
  RTC_DEBUG(("Output data start"));
  int i;
  long val;

  switch ( m_out_format ) {
  case DEPTH8:
    m_out.data.length(length * 1);
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val < 0 ) {
        val *= -1;
      }
      if ( val > 255 ) {
        val = 255;
      }
/* 8bit format used unsigned char
      if ( fwk < -124.0 ) {
        fwk = -127.0;
      }
      if ( fwk > 127.0 ) {
        fwk = 127.0;
      }
 */
      m_out.data[i] = (unsigned char)val;
    }
    break;
  case DEPTH16:
    m_out.data.length(length * 2);
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val > 32767 ) {
        val = 32767;
      } else if ( val < -32767 ) {
        val = -32767;
      }
      m_out.data[i*2]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*2+1] = (unsigned char)((val & 0x0000ff00) >> 8);
    }
/*
    m_out.data.length(length * 2);
    for ( i = 0; i < length; i++ ) {
      fwk = data[i];
      if ( fwk > 32767.0 ) {
        fwk = 32767.0;
      } else if ( fwk < -32767.0 ) {
        fwk = -32767.0;
      }
      m_out.data[i*2] = ((unsigned char)fwk & 0x00ff);
      m_out.data[i*2+1] = (((unsigned char)fwk & 0xff00) >> 8);
    }
*/
    break;
  case DEPTH24:
    m_out.data.length(length * 3);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      if ( val > 8388607 ) {
        val = 8388607;
      } else if ( val < -8388607 ) {
        val = -8388607;
      }
      m_out.data[i*3]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*3+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_out.data[i*3+2] = (unsigned char)((val & 0x00ff0000) >> 16);
    }
/*
    m_out.data.length(length * 3);
    for ( i = 0; i < length; i++ ) {
      fwk = data[i];
      if ( fwk > 8388607.0 ) {
        fwk = 8388607.0;
      } else if ( fwk < -8388607.0 ) {
        fwk = -8388607.0;
      }
      m_out.data[i*3] = ((unsigned long)fwk & 0x0000ff);
      m_out.data[i*3+1] = (((unsigned long)fwk & 0x00ff00) >> 8);
      m_out.data[i*3+2] = (((unsigned long)fwk & 0xff0000) >> 16);
    }
*/
    break;
  case DEPTH32:
    m_out.data.length(length * 4);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = (long)data[i];
      m_out.data[i*4]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*4+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_out.data[i*4+2] = (unsigned char)((val & 0x00ff0000) >> 16);
      m_out.data[i*4+3] = (unsigned char)((val & 0xff000000) >> 24);
    }
    break;
  case DEPTHFLOAT32:
    break;
  }
  setTimestamp( m_out );
  m_outOut.write();
  return;
}

double SamplingRateConverter::DeprhGain(void)
{
  double wk0,wk1;
  switch ( m_in_format ) {
  case DEPTH8:
    wk0 = 255.0;
    break;
  case DEPTH16:
    wk0 = 32767.0;
    break;
  case DEPTH24:
    wk0 = 8388607.0;
    break;
  case DEPTH32:
    wk0 = 2147483520.0;
    break;
  default:
    wk0 = 1.0;
    break;
  }
  switch ( m_out_format ) {
  case DEPTH8:
    wk1 = 255.0;
    break;
  case DEPTH16:
    wk1 = 32767.0;
    break;
  case DEPTH24:
    wk1 = 8388607.0;
    break;
  case DEPTH32:
    wk1 = 2147483520.0;
    break;
  default:
    wk1 = 1.0;
    break;
  }
  wk0 = wk1/wk0;
//  if ( wk0 < 0.00001 ) wk0 = 0.004;
  return wk0;
}
void SamplingRateConverter::OutputDataWrite(long *data, int length)
{
  RTC_DEBUG(("Output data start"));
  double gain = DeprhGain();
  int i;
  long val;

  switch ( m_out_format ) {
  case DEPTH8:
    m_out.data.length(length * 1);
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      if ( val < 0 ) {
        val *= -1;
      }
      val = (long)(val * gain);
      if ( val > 255 ) {
        val = 255;
      }
      m_out.data[i] = (unsigned char)val;
    }
    break;
  case DEPTH16:
    m_out.data.length(length * 2);
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      if ( val > 32767 ) {
        val = 32767;
      } else if ( val < -32767 ) {
        val = -32767;
      }
      m_out.data[i*2]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*2+1] = (unsigned char)((val & 0x0000ff00) >> 8);
    }
    break;
  case DEPTH24:
    m_out.data.length(length * 3);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      val = (long)data[i];
      if ( val > 8388607 ) {
        val = 8388607;
      } else if ( val < -8388607 ) {
        val = -8388607;
      }
      m_out.data[i*3]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*3+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_out.data[i*3+2] = (unsigned char)((val & 0x00ff0000) >> 16);
    }
    break;
  case DEPTH32:
    m_out.data.length(length * 4);  //!< set outport data length
    for ( i = 0; i < length; i++ ) {
      val = data[i];
      val = (long)(val * gain);
      m_out.data[i*4]   = (unsigned char)(val & 0x000000ff);
      m_out.data[i*4+1] = (unsigned char)((val & 0x0000ff00) >> 8);
      m_out.data[i*4+2] = (unsigned char)((val & 0x00ff0000) >> 16);
      m_out.data[i*4+3] = (unsigned char)((val & 0xff000000) >> 24);
    }
    break;
  case DEPTHFLOAT32:
    break;
  }
  setTimestamp( m_out );
  m_outOut.write();
  return;
}

RTC::ReturnCode_t SamplingRateConverter::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  if ( !m_queue.empty() ) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int len;
//    float *src = InputDataSetFloat(&len);
    long *data = InputDataSet(&len);
    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
//    if ( src == NULL ) return RTC::RTC_ERROR;
    if ( data == NULL ) return RTC::RTC_ERROR;

    if ( m_ratio == 1 ) {
      OutputDataWrite(data, len);
      delete [] data;
    } else {
      float *src = InputDataSetFloat(&len);
      if ( src == NULL ) return RTC::RTC_ERROR;
      int dnum = (int)(len * m_ratio);
      float *dst = new float[dnum];

      int o = 0, srcused = 0, srcpos = 0, out = 0;
      for(;;) {
        int srcBlock = len - srcpos;
        o = resample_process(  m_handle,
                               m_ratio,
                               &src[srcpos],
                               srcBlock,
                                   0,
                               &srcused,
                               &dst[out],
                               dnum - out);
        srcpos += srcused;
        if ( o >= 0 ) {
          out += o;
        }
        if ( o < 0 || (o == 0 && srcpos == len) ) {
          break;
        }
      }
      delete [] src;
      OutputDataWriteFloat(dst, dnum);
      delete [] dst;
    }

  }

#if 0
  if ( (!m_queue.empty()) && ( m_ratio != 1 ) ) {
    m_mutex.lock();
    RTC_DEBUG(("onExecute:mutex lock"));
    int num = m_queue.size();
    int dnum = (int)(num * m_ratio);
    int i = 0;
    float *src = new float[num];
    float *dst = new float[dnum];
    for ( i = 0; i < num; i++ ) {
      src[i] = (float)m_queue.front();
      m_queue.pop_front();
    }

    int o = 0, srcused = 0, srcpos = 0, out = 0;

    for(;;) {
      int srcBlock =num - srcpos;
      o = resample_process(  m_handle,
                             m_ratio,
                             &src[srcpos],
                             srcBlock,
                                 0,
                             &srcused,
                             &dst[out],
                             dnum - out);
      srcpos += srcused;
      if ( o >= 0 ) {
        out += o;
      }
      if ( o < 0 || (o == 0 && srcpos == num) ) {
        break;
      }
    }
    delete [] src;

    short *dbuffer = new short[dnum];
    for ( i = 0; i < dnum; i++ ) {
      dbuffer[i] = (short)dst[i];
    }
    delete [] dst;

    int dlen = dnum * 2;
    m_out.data.length(dlen);  //!< set outport data length
    memcpy((void *)&(m_out.data[0]), (void *)&(dbuffer[0]), dlen);
    setTimestamp( m_out );
    m_outOut.write();
    delete [] dbuffer;
    m_mutex.unlock();
    RTC_DEBUG(("onExecute:mutex unlock"));
  }
#endif
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SamplingRateConverter::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( m_handle ) {
    resample_close( m_handle );
    m_handle = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t SamplingRateConverter::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( !m_queue.empty() ) {
    m_queue.clear();
  }
  if ( m_handle ) {
    resample_close( m_handle );
    m_handle = NULL;
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void SamplingRateConverterInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(samplingrateconverter_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(samplingrateconverter_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<SamplingRateConverter>,
                           RTC::Delete<SamplingRateConverter>);
  }
};
