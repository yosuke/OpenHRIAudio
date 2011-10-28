// -*- C++ -*-
/*!
 * @file  PortAudioInput.h
 * @author Yosuke Matsusaka <yosuke.matsusaka@aist.go.jp> and OpenHRI development team
 *
 * Copyright (C) 2010
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * @date  $Date$
 *
 * $Id$
 */

#ifndef PORTAUDIOINPUT_H
#define PORTAUDIOINPUT_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <math.h>
#include <portaudio.h>
//#include <pa_win_wasapi.h>
#if defined(_WIN32)
#include <windows.h>
#include <mmsystem.h>
#elif defined(__linux)
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#endif
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/SystemLogger.h>
#include <coil/Mutex.h>

#include "DescriptablePort.h"

#ifdef HAVE_CONFIG_H
#include "../config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

#define FRAMES_PER_BUFFER (512)
#define HMIXER_NUM_MAX	(10)

using namespace RTC;

/*!
 * @class DataListener
 * @brief
 */
class PortAudioInputDataListener
  : public ConnectorDataListenerT<RTC::TimedLong>
{
public:
  /*!
   * @brief constructor
   *
   * @param name DataListener event name
   * @param data PortAudio Object
   */
  PortAudioInputDataListener(const char* name, void *data);

  /*!
   * @brief destructor
   */
  virtual ~PortAudioInputDataListener();
  virtual void operator()(const ConnectorInfo& info,
                          const TimedLong& data);
  void *m_obj;
  std::string m_name;
};

/*!
 * @class PortAudioInput
 * @brief Periodic Console Out Component
 *
 */
class PortAudioInput
  : public RTC::DataFlowComponentBase
{
 public:
  void SetGain(long data);
  void ReadBufferCB(const void *, unsigned long );
  unsigned long m_totalframes;      //!< Max frames to buffer size

  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  PortAudioInput(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~PortAudioInput();

  // <rtc-template block="public_attribute">

  // </rtc-template>

  // <rtc-template block="public_operation">

  // </rtc-template>

  /***
   *
   * The initialize action (on CREATED->ALIVE transition)
   * formaer rtc_init_entry()
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onInitialize();

  /***
   *
   * The finalize action (on ALIVE->END transition)
   * formaer rtc_exiting_entry()
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onFinalize();

  /***
   *
   * The startup action when ExecutionContext startup
   * former rtc_starting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  /***
   *
   * The shutdown action when ExecutionContext stop
   * former rtc_stopping_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  /***
   *
   * The activated action (Active state entry action)
   * former rtc_active_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  /***
   *
   * The deactivated action (Active state exit action)
   * former rtc_active_exit()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  /***
   *
   * The execution action that is invoked periodically
   * former rtc_active_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
   virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  /***
   *
   * The aborting action when main logic error occurred.
   * former rtc_aborting_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  /***
   *
   * The error action in ERROR state
   * former rtc_error_do()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  /***
   *
   * The reset action that is invoked resetting
   * This is same but different the former rtc_init_entry()
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);

  /***
   *
   * The state update action that is invoked after onExecute() action
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  /***
   *
   * The action that is invoked when execution context's rate is changed
   * no corresponding operation exists in OpenRTm-aist-0.2.0
   *
   * @param ec_id target ExecutionContext Id
   *
   * @return RTC::ReturnCode_t
   *
   *
   */
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);

 protected:
  // <rtc-template block="protected_attribute">

  // </rtc-template>

  // <rtc-template block="protected_operation">

  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
   TimedLong m_in_data;
   DescriptableInPort<TimedLong> m_in_dataIn;

  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  TimedOctetSeq m_out_data;
  DescriptableOutPort<TimedOctetSeq> m_out_dataOut;

  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">

  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">

  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">

  // </rtc-template>

 private:
  /*!
   * @brief get PortAudio sample format
   *
   * @param str InputSampleByte
   * @return PaSampleFormat
   */
  PaSampleFormat getFormat(std::string str);
#if 0
  void AutoGainControl(void *data, int length);
#endif

  coil::Mutex m_mutex;
  PaStream *m_stream;          //!< PortAudio Stream Object
  PaError m_err;               //!< Error codes returned by PortAudio functions
  PaSampleFormat m_format;     //!< Sample format type (paInt16, paInt24, paInt8)
  std::string m_formatstr;     //!< config parameter(InputSampleByte)
  std::string m_gainstr;
  double m_samplerate;         //!< config parameter(InputSampleRate)
  float m_volume;
  int m_channels;              //!< config parameter(InputChannelNumbers)
  bool is_active;
  bool syncflg;
#ifdef HAVE_LIBPORTMIXER
  void *m_mixer;
#else
#if defined(_WIN32)
  bool InitMixer(void);
  void CloseMixer(void);
  bool GetMicrophoneLevel(DWORD* dwLevel);
  bool SetMicrophoneLevel(DWORD dwLevel);
  HMIXER m_mixer[HMIXER_NUM_MAX];
  BOOL m_barrOpened[HMIXER_NUM_MAX];
  HMIXER m_mixerMic;
  int m_mixerNums;
  bool m_isOpen;
  DWORD m_dwCntlID,m_dwMin,m_dwMax;
#elif defined(__linux)
  int m_device,m_fd;
#endif
#endif
  // <rtc-template block="private_attribute">

  // </rtc-template>

  // <rtc-template block="private_operation">

  // </rtc-template>

};
extern "C"
{
  /*!
   * @brief PortAudioInput initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void PortAudioInputInit(RTC::Manager* manager);
};


#endif // PORTAUDIOINPUT_H
