// -*- C++ -*-
/*!
 * @file  PortAudioInput.cpp
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

#include "PortAudioInput.h"
#include "PortAudioOutput.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"
#if defined(_WIN32)
#define _WIN32_WINNT 0x0601
#define PA_USE_WASAPI
#endif

coil::Mutex m_pa_mutex;

extern const char** portaudioinput_spec;
extern const char** portaudiooutput_spec;

extern "C"
{
  DLL_EXPORT void PortAudioRTCInit(RTC::Manager* manager)
  {
    int i, j;
    PaError m_err;
    
    m_err = Pa_Initialize();
    if(m_err != paNoError) {
      printf("PortAudio failed: %s\n", Pa_GetErrorText(m_err));
      return;
    }

    for (i = 0; strlen(portaudioinput_spec[i]) != 0; i++);
    char** spec_intl_in = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl_in[j] = (char *)_(portaudioinput_spec[j]);
    }
    spec_intl_in[i] = (char *)"";
    coil::Properties pain_profile((const char **)spec_intl_in);
    manager->registerFactory(pain_profile,
			     RTC::Create<PortAudioInput>,
			     RTC::Delete<PortAudioInput>);

    for (i = 0; strlen(portaudiooutput_spec[i]) != 0; i++);
    char** spec_intl_out = new char*[i + 1];
    for (j = 0; j < i; j++) {
      spec_intl_out[j] = (char *)_(portaudiooutput_spec[j]);
    }
    spec_intl_out[i] = (char *)"";
    coil::Properties paout_profile((const char **)spec_intl_out);
    manager->registerFactory(paout_profile,
			     RTC::Create<PortAudioOutput>,
			     RTC::Delete<PortAudioOutput>);
  }
};
