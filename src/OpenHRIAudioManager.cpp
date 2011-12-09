// -*- C++ -*-
/*!
 * @file OpenHRIAudioManager.cpp
 * @brief RT component manager for OpenHRIAudio
 */

#include <iostream>
#if defined(__linux)
#include <dlfcn.h>
#include <link.h>
#elif defined(_WIN32)
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "resource.h"
#endif
#include <rtm/Manager.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __linux
char *getsopath(std::string soname)
{
  void *h;
  struct link_map *map;
  char *fullpath;
  h = dlopen(soname.c_str(), RTLD_LAZY);
  if (!h) {
     return NULL;
  }
  dlinfo(h, RTLD_DI_LINKMAP, &map);
  fullpath = realpath(map->l_name, NULL);
  return fullpath;
}
#endif

#ifdef HAVE_GTKMM
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>

class MenuWindow : public Gtk::Window
{
public:
  MenuWindow() {
    set_title("OpenHRIAudio Manager");
    set_border_width(10);
    add(m_box);
    m_box.show();
  };

  virtual ~MenuWindow() {};

  void setManager(RTC::Manager* _manager) {
    manager = _manager;
  };

  void setComponents(std::vector<std::string> components) {
    for (unsigned int i = 0; i < components.size(); i++) {
      Gtk::Button* button;
      button = new Gtk::Button(components[i]);
      button->signal_clicked().connect(sigc::bind<Glib::ustring>(
				       sigc::mem_fun(*this, &MenuWindow::on_button_clicked), components[i]));
      m_box.pack_start(*button);
      button->show();
    }
  };
  
protected:

  void on_button_clicked(Glib::ustring data) {
    manager->createComponent(data.c_str());
  };

  Gtk::VBox m_box;
  RTC::Manager* manager;
};
#endif

#if defined(_WIN32)
#define COMPNUM 14
RTC::Manager* g_manager;
bool loadlist[COMPNUM];

BOOL CALLBACK DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch( msg ){
  case WM_INITDIALOG:
    for (int i = 0; i < COMPNUM; i++) {
      if ( loadlist[i] == false ) {
	EnableWindow( GetDlgItem( hWnd, (1000 + i) ), false);
      }
    }
    //g_manager->createComponent("PortAudioInput");
    //g_manager->createComponent("PortAudioOutput");
    break;
  case WM_CLOSE:
    EndDialog(hWnd , ID_OK);
    return TRUE;
  case WM_COMMAND:
    switch( LOWORD( wParam ) ){
    case ID_PORTAUDIOINPUT:
      g_manager->createComponent("PortAudioInput");
      break;
    case ID_PORTAUDIOOUTPUT:
      g_manager->createComponent("PortAudioOutput");
      break;
    case ID_ECHOCANCELER:
      g_manager->createComponent("EchoCanceler");
      break;
    case ID_ECHOSUPPRESSOR:
      g_manager->createComponent("EchoSuppressor");
      break;
    case ID_SAMPLERATECNV:
      g_manager->createComponent("SamplingRateConverter");
      break;
    case ID_NOISEREDUCTION:
      g_manager->createComponent("NoiseReduction");
      break;
    case ID_COMBFILTER:
      g_manager->createComponent("CombFilter");
      break;
    case ID_SIGNALGENRATION:
      g_manager->createComponent("SignalGeneration");
      break;
    case ID_MIXER:
      g_manager->createComponent("Mixer");
      break;
    case ID_DSARRAY:
      g_manager->createComponent("DSArray");
      break;
    case ID_CHANNELSELECTOR:
      g_manager->createComponent("ChannelSelector");
      break;
    case ID_BEAMFORMING:
      g_manager->createComponent("BeamForming");
      break;
    case ID_AUTOGAINCONTROL:
      g_manager->createComponent("AutoGainControl");
      break;
    case ID_WAVEREC:
      g_manager->createComponent("WavRecord");
      break;
    }
  }
  
  return FALSE;
}
#endif

int main (int argc, char** argv)
{
  char *path;
  RTC::Manager* manager;
  std::vector<std::string> components;
#if defined(__linux)
  components.push_back("PulseAudioOutput");
  components.push_back("PulseAudioInput");
#elif defined(_WIN32)
  //components.push_back("PortAudioRTC"); //by Irie Seisho
  components.push_back("PortAudioInput");   //by Irie Seisho
  components.push_back("PortAudioOutput");   //by Irie Seisho
#endif
  components.push_back("EchoCanceler");
  components.push_back("EchoSuppressor");
  components.push_back("SamplingRateConverter");
  components.push_back("NoiseReduction");
  components.push_back("CombFilter");
  components.push_back("SignalGeneration");
  components.push_back("Mixer");
  components.push_back("DSArray");
  components.push_back("ChannelSelector");
  components.push_back("BeamForming");
  components.push_back("AutoGainControl");
  components.push_back("WavRecord");

  if(argc != 2) {
    argc = 2;
    char argv1[] = "-d";
    argv[1] = argv1;
  }
  else { // argc == 2
    if(strcmp(argv[1], "-d") != 0) {
      strcpy(argv[1],"-d");
    }
  }
  manager = RTC::Manager::init(argc, argv);
  manager->activateManager();

#if defined(__linux)
  for (unsigned int i = 0; i < components.size(); i++) {
    std::string lower = components[i];
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);   
    std::string soname = "lib" + lower + ".so";
    if ((path = getsopath(soname)) != NULL) {
      printf("loading %s: %s\n", components[i].c_str(), path);
      manager->load(path, (components[i] + "Init").c_str());
      free(path);
    }
  }
#elif defined(_WIN32)
  HANDLE hFind;
  WIN32_FIND_DATA fd;
  for (unsigned int i = 0; i < components.size(); i++) {
    loadlist[i] = false;
    std::string lower = components[i];
    std::string dllname = lower + ".dll";
    hFind = FindFirstFile(dllname.c_str(), &fd);
    if ( hFind != INVALID_HANDLE_VALUE ) {
      path = (char *)dllname.c_str();
      printf("loading %s: %s\n", lower.c_str(), path);
      manager->load(path, (lower + "Init").c_str());
      loadlist[i] = true;
    } else {
      printf("not find file %s: %s\n", lower.c_str(), path);
    }
    FindClose(hFind);
  }
#endif
#if defined(HAVE_GTKMM)
  manager->runManager(true);
  Gtk::Main kit(argc, argv);
  MenuWindow menuwindow;
  menuwindow.setManager(manager);
  menuwindow.setComponents(components);
  Gtk::Main::run(menuwindow);
  manager->terminate();
#elif defined(_WIN32)
  manager->runManager(true);
  HINSTANCE hInst = GetModuleHandle( NULL );
  g_manager = manager;
  DialogBox( hInst, "MENUDLG", NULL, ( DLGPROC )DlgProc );
  manager->terminate();
#endif

  return 0;
}
