// -*- C++ -*-
/*!
 * @file  Gate.cpp
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

#include "Gate.h"
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
static const char* gate_spec[] =
  {
    "implementation_id", "Gate",
    "type_name",         "Gate",
    "description",       N_("Gate component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.__doc__.usage", "\n  ::\n\n  $ gatertc\n",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Gate::Gate(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_inIn("AudioDataIn", m_in),
    m_gateIn("GateIn", m_gate),
    m_outOut("AudioDataOut", m_out)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Gate::~Gate()
{
}

RTC::ReturnCode_t Gate::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("Gate : Gate component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("AudioDataIn", m_inIn);
  m_inIn.setDescription(_("Audio data input."));
  addInPort("GateIn", m_gateIn);
  m_gateIn.setDescription(_("Gate data input."));

  // Set OutPort buffer
  addOutPort("AudioDataOut", m_outOut);
  m_outOut.setDescription(_("Audio data output."));

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>

  RTC_DEBUG(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Gate::onActivated(RTC::UniqueId ec_id)
{
  is_gated = false;
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Gate::onExecute(RTC::UniqueId ec_id)
{
  int i;
  if (m_gateIn.isNew()) {
    m_gateIn.read();
    is_gated = m_gate.data;
  }
  if (m_inIn.isNew()) {
    m_inIn.read();
    m_out.data.length(m_in.data.length());
    if (is_gated == false) {
      for (i = 0; i < m_in.data.length(); i++) {
        m_out.data[i] = m_in.data[i];
      }
    } else {
      for (i = 0; i < m_in.data.length(); i++) {
        m_out.data[i] = 0;
      }
    }
    setTimestamp(m_out);
    m_outOut.write();
  }
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Gate::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Gate::onFinalize()
{
  return RTC::RTC_OK;
}

extern "C"
{
  void GateInit(RTC::Manager* manager)
  {
    int i;
    for (i = 0; strlen(gate_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = (char *)_(gate_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                             RTC::Create<Gate>,
                             RTC::Delete<Gate>);
  }
};
