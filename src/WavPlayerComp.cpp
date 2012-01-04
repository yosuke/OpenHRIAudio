// -*- C++ -*-
/*!
 * @file WavPlayerComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "WavPlayer.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"


void MyModuleInit(RTC::Manager* manager)
{
  WavPlayerInit(manager);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("WavPlayer");

  if (comp==NULL)
  {
    std::cerr << "Component create failed." << std::endl;
    abort();
  }
  return;
}

int main (int argc, char** argv)
{
  RTC::Manager* manager;

  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);

  manager = RTC::Manager::init(argc, argv);

  // Initialize manager
  manager->init(argc, argv);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
  manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
  manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
  // manager->runManager(true);

  return 0;
}
