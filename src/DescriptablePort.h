// -*- C++ -*-
/*!
 * @file  DescriptablePort.h
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

#ifndef DESCRIPTABLEPORT_H
#define DESCRIPTABLEPORT_H

#include <rtm/RTC.h>
#include <rtm/Typename.h>
#include <rtm/OutPort.h>
#include <rtm/InPort.h>

namespace RTC
{
  template <class DataType>
  class DescriptableOutPort : public OutPort<DataType>
  {
  public:
    DescriptableOutPort(const char* name, DataType& value)
      : OutPort<DataType>(name, value){
    }
    void setDescription(const char* text) {
      this->appendProperty("description", text);
    }
  };
  
  template <class DataType>
  class DescriptableInPort : public InPort<DataType>
  {
  public:
    DescriptableInPort(const char* name, DataType& value)
      : InPort<DataType>(name, value){
    }
    void setDescription(const char* text) {
      this->appendProperty("description", text);
    }
  };
};

#endif
