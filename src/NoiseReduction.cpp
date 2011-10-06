// -*- C++ -*-
/*!
 * @file  NoiseReduction.cpp
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

#include "NoiseReduction.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "../config.h"
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
static const char* noisereduction_spec[] =
  {
    "implementation_id", "NoiseReduction",
    "type_name",         "NoiseReduction",
    "description",       N_("FFT/IFFT based noise reduction component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.SampleRate", "16000",
    "conf.default.CenterFrequency", "500",
    "conf.default.FrequencyMax", "700",
    "conf.default.FrequencyMin", "300",
    "conf.default.Power", "1",
    "conf.default.ModeSelect", "premp",
    "conf.__widget__.ModeSelect", "radio",
    "conf.__constraints__.ModeSelect", "(premp, deemp, bpf)",
    "conf.__description__.ModeSelect", N_("Specify filtering method ['premp': Pre-emphasis mode, 'deemp': De-emphasis mode, 'bpf': Bandpass filter mode]."),
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.CenterFrequency", "spin",
    "conf.__constraints__.CenterFrequency", "x >= 1",
    "conf.__description__.CenterFrequency", N_("Center frequency (effective in 'premp' and 'deemp' mode."),
    "conf.__widget__.FrequencyMax", "spin",
    "conf.__constraints__.FrequencyMax", "x >= 1",
    "conf.__description__.FrequencyMax", N_("Max frequency (effective in 'bpf' mode."),
    "conf.__widget__.FrequencyMin", "spin",
    "conf.__constraints__.FrequencyMin", "x >= 1",
    "conf.__description__.FrequencyMin", N_("Min frequency (effective in 'bpf' mode."),
    "conf.__widget__.Power", "spin",
    "conf.__constraints__.Power", "x >= 1",
    "conf.__description__.Power", N_("Magnification rate of the emphasis."),
    "conf.__doc__.usage", "\n  ::\n\n  $ noisereduction\n",
    ""
  };
// </rtc-template>

//! Band-pass filter
void NoiseReduction::bpf(float *fdata)
{
  float df = ((float)m_rate) / ((float)WINLEN);

  if (m_freqmax < m_freqmin) return;

  for (int i = 0; i < WINLEN; i++) {
    if (((i * df) > (float)m_freqmin) && ((i * df) < (float)m_freqmax)) {
      fdata[i*2+0] *= m_power;
      fdata[i*2+1] *= m_power;
    }
  }

}

//! pre-emphasis
void NoiseReduction::pre_emp(float *fdata)
{
  if (m_freqcenter > m_rate) return;
  for (int i = 0; i < WINLEN; i++) {
    fdata[i*2+0] *= gauss[i] * m_power;
    fdata[i*2+1] *= gauss[i] * m_power;
  }
}

//! de-emphasis
void NoiseReduction::de_emp(float *fdata)
{
  if (m_freqcenter > m_rate) return;
  for ( int i = 0; i < WINLEN; i++ ) {
    fdata[i*2+0] /= gauss[i] * m_power;
    fdata[i*2+1] /= gauss[i] * m_power;
  }
}

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
    NoiseReduction *p = (NoiseReduction *)m_obj;
    p->RcvInBuffer(data);
  }
}

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
NoiseReduction::NoiseReduction(RTC::Manager* manager)
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
NoiseReduction::~NoiseReduction()
{
}

RTC::ReturnCode_t NoiseReduction::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("NoiseReduction : FFT/IFFT based noise reduction component"));
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
  bindParameter("CenterFrequency", m_freqcenter, "500");
  bindParameter("FrequencyMax", m_freqmax, "700");
  bindParameter("FrequencyMin", m_freqmin, "300");
  bindParameter("Power", m_power, "1");
  bindParameter("ModeSelect", m_mode, "premp");
  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t NoiseReduction::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));

  m_mutex.lock();
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();

  window = CreateWindowFloat(WINLEN, Hamming);
  gauss = CreateGaussWindowFloat(WINLEN, m_rate, m_freqcenter);
  fft = spx_fft_init(WINLEN);
  for (int i = 0; i < WINLEN/2; i++) {
    m_outbuffer.push_back(0);
  }
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void NoiseReduction::RcvInBuffer(TimedOctetSeq data)
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

//! order : dft->pre-emphasis->idft->repetition addition
RTC::ReturnCode_t NoiseReduction::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  m_mutex.lock();
  RTC_DEBUG(("onExecute:mutex lock"));
  if (m_inbuffer.size() >= WINLEN) {
    int i;
    float data[WINLEN];
    float fdata[WINLEN*2];
    std::list<short>::iterator pbuffer;

    // sliding window with half overlap
    for (i = 0; i < WINLEN/2; i++) {
      data[i] = (float)m_inbuffer.front();
      m_inbuffer.pop_front();
    }
    pbuffer = m_inbuffer.begin();
    for (i = WINLEN/2; i < WINLEN; i++) {
      data[i] = (float)*pbuffer;
      pbuffer++;
    }

    // apply window function
    ApplyWindowFloat(WINLEN, window, data);

    // convert to frequency domain
    spx_fft_float(fft, data, fdata);

    // sampling frequency 16000Hz, number 1024, resolution 16000/1024=15.625
    // voice frequency male：300～550Hz,female：400～700Hz
    if ( m_mode == "premp" ) {
      //! pre-emphasis
      pre_emp(fdata);
    } else if ( m_mode == "deemp" ) {
      //! de-emphasis
      de_emp(fdata);
    } else if ( m_mode == "bpf" ) {
      //! band-pass
      bpf(fdata);
    }

    // return back to time domain
    spx_ifft_float(fft, fdata, data);
    ApplyWindowFloat(WINLEN, window, data);

    // sum it up (half overlap)
    pbuffer = m_outbuffer.begin();
    for (i = 0; i < WINLEN/2; i++) {
      *pbuffer += (short)(data[i]);
      pbuffer++;
    }
    for (i = WINLEN/2; i < WINLEN; i++) {
      m_outbuffer.push_back((short)(data[i]));
    }
    // output the resulting signal
    m_fout.data.length(WINLEN);
    short val;
    for (i = 0; i < WINLEN/2; i++) {
      val = m_outbuffer.front();
      m_fout.data[i*2] = (unsigned char)(val & 0x00ff);
      m_fout.data[i*2+1] = (unsigned char)((val & 0xff00) >> 8);
      m_outbuffer.pop_front();
    }
    setTimestamp(m_fout);
    m_foutOut.write();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onExecute:mutex unlock"));
  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t NoiseReduction::onDeactivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onDeactivated start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onDeactivated:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onDeactivated:mutex unlock"));
  RTC_DEBUG(("onDeactivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t NoiseReduction::onFinalize()
{
  RTC_DEBUG(("onFinalize start"));
  is_active = false;
  m_mutex.lock();
  RTC_DEBUG(("onFinalize:mutex lock"));
  if ( fft != NULL ) {
    spx_fft_destroy(fft);
    fft = NULL;
  }
  if (!m_inbuffer.empty()) {
    m_inbuffer.clear();
  }
  if (!m_outbuffer.empty()) {
    m_outbuffer.clear();
  }
  m_mutex.unlock();
  RTC_DEBUG(("onFinalize:mutex unlock"));
  RTC_DEBUG(("onFinalize finish"));
  return RTC::RTC_OK;
}

extern "C"
{
  void NoiseReductionInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(noisereduction_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(noisereduction_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<NoiseReduction>,
                           RTC::Delete<NoiseReduction>);
  }
};
