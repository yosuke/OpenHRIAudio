// -*- C++ -*-
/*!
 * @file WavRecordComp.cpp
 * @brief Standalone component
 * @date $Date$
 *
 * $Id$
 */
#if defined(__linux)
#include <gtkmm.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "WavRecord.h"
#ifdef VERSION
#undef VERSION
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.00"
#endif
#include "intl.h"
//#include <intl.h>

#if defined(__linux)
static char WaveFileName[512*2]; 

class DialogWin : public Gtk::Window
{
    Gtk::Label m_label;

public:
  DialogWin() {
    Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_SAVE );
    // 開く、キャンセルボタン
    diag.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    diag.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    switch( diag.run() ){
    case Gtk::RESPONSE_OK:
      m_label.set_text( diag.get_filename() );
      strncpy(WaveFileName, (diag.get_filename()).c_str(), (diag.get_filename()).size());
      break;
    case Gtk::RESPONSE_CANCEL:
      m_label.set_text( "Cancel" );
      //strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
      break;
    }
    add( m_label );
    show_all_children();
    resize( 200, 100 );  
  };
};
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 

int OpenDiaog(HWND hwnd,LPCSTR Filter,char *FileName,DWORD Flags)
{
   OPENFILENAME OFN; 

   ZeroMemory(&OFN,sizeof(OPENFILENAME));
   OFN.lStructSize = sizeof(OPENFILENAME); 
   OFN.hwndOwner = hwnd;
   OFN.lpstrFilter =Filter;
   OFN.lpstrFile =FileName;  
   OFN.nMaxFile = MAX_PATH*2;
   OFN.Flags = Flags;    
   OFN.lpstrTitle = "File Open";
   return (GetOpenFileName(&OFN));
}
#endif


void MyModuleInit(RTC::Manager* manager)
{
  WavRecordInit(manager, WaveFileName);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("WavRecord");

  if (comp==NULL)
  {
    std::cerr << "Component create failed." << std::endl;
    abort();
  }

  // Example
  // The following procedure is examples how handle RT-Components.
  // These should not be in this function.

  // Get the component's object reference
//  RTC::RTObject_var rtobj;
//  rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));

  // Get the port list of the component
//  PortServiceList* portlist;
//  portlist = rtobj->get_ports();

  // getting port profiles
//  std::cout << "Number of Ports: ";
//  std::cout << portlist->length() << std::endl << std::endl;
//  for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
//  {
//    PortService_ptr port;
//    port = (*portlist)[i];
//    std::cout << "Port" << i << " (name): ";
//    std::cout << port->get_port_profile()->name << std::endl;
//
//    RTC::PortInterfaceProfileList iflist;
//    iflist = port->get_port_profile()->interfaces;
//    std::cout << "---interfaces---" << std::endl;
//    for (CORBA::ULong i(0), n(iflist.length()); i < n; ++i)
//    {
//      std::cout << "I/F name: ";
//      std::cout << iflist[i].instance_name << std::endl;
//      std::cout << "I/F type: ";
//      std::cout << iflist[i].type_name << std::endl;
//      const char* pol;
//      pol = iflist[i].polarity == 0 ? "PROVIDED" : "REQUIRED";
//      std::cout << "Polarity: " << pol << std::endl;
//    }
//    std::cout << "---properties---" << std::endl;
//    NVUtil::dump(port->get_port_profile()->properties);
//    std::cout << "----------------" << std::endl << std::endl;
//  }

  return;
}

int main (int argc, char** argv)
{
  RTC::Manager* manager;

#if defined(__linux)
  Gtk::Main kit(argc, argv);
  DialogWin dialogwin;
  Gtk::Main::run( dialogwin );
#elif defined(_WIN32)
  //HINSTANCE hInst = GetModuleHandle( NULL );
  HWND hwnd = GetWindow( NULL, GW_OWNER );
  OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName, OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);
  printf("Wave File Name:%s\n", WaveFileName);
#endif

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
