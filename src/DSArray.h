// -*- C++ -*-
/*!
 * @file  DSArray.h
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

#ifndef DSARRAY_H
#define DSARRAY_H

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <list>
#include <fstream>
//#include <fftw3.h>
//#include <speex/speex.h>
//#include <speex/speex_preprocess.h>
//#include <speex/speex_echo.h>

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>
#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/SystemLogger.h>

#include <coil/Mutex.h>

#include "DescriptablePort.h"
#include "windowing.h"

// Service implementation headers
// <rtc-template block="service_impl_h">

// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">

// </rtc-template>

struct mic_info {
  double x,y,z;
  double xy_rad,yz_rad;
};

struct delay_rank {
  int mic_num,trigger_cnt;
};

using namespace RTC;
#define SONIC 340.29
#define WINLEN 2048
#define SEARCHMAX 100

/*!
 * @class DataListener
 * @brief
 */
class DataListener
  : public ConnectorDataListenerT<RTC::TimedOctetSeq>
{
public:
  /*!
   * @brief constructor
   *
   * @param name DataListener event name
   * @param data PortAudio Object
   */
  DataListener(const char* name, void *data);

  /*!
   * @brief destructor
   */
  virtual ~DataListener();
  virtual void operator()(const ConnectorInfo& info,
                          const TimedOctetSeq& data);
  void *m_obj;
  std::string m_name;
};

/*!
 * @class DSArray
 * @brief Periodic Console Out Component
 *
 */
class DSArray
  : public RTC::DataFlowComponentBase
{
 public:
  /*!
   * @brief receive inport buffer
   */
  void RcvBuffer(TimedOctetSeq data);
  /*!
   * @brief constructor
   * @param manager Maneger Object
   */
  DSArray(RTC::Manager* manager);

  /*!
   * @brief destructor
   */
  ~DSArray();

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
  TimedOctetSeq m_mic;
  DescriptableInPort<TimedOctetSeq> m_micIn;

  // </rtc-template>


  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  TimedDouble m_result;
  DescriptableOutPort<TimedDouble> m_resultOut;

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
  // <rtc-template block="private_attribute">
  void BufferClr(void);
  std::list<short> m_data; //!< receive buffer queue
  coil::Mutex m_mutex;
  mic_info* m_micinfo;
  void *fft;
  float *window;
  double m_rate;
  int m_channels;
  bool is_active;
  bool m_horizon;
  // </rtc-template>

  // <rtc-template block="private_operation">
  int ccf(short *base, short *data);
  int CrossCorrelation(short *base, short *data);
  // </rtc-template>

};

extern "C"
{
  /*!
   * @brief DSArray initialize
   *
   * @param manager Maneger Object
   */
  DLL_EXPORT void DSArrayInit(RTC::Manager* manager);
};

#endif // DSARRAY_H
