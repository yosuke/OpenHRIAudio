// -*- C++ -*-
/*!
 * @file  WavPlayer.cpp
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

#if defined(__linux)
static char WaveFileName[512*2]; 
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 
#endif

// Module specification
// <rtc-template block="module_spec">
static const char* waveplayer_spec[] =
  {
    "implementation_id", "WavPlayer",
    "type_name",         "WavPlayer",
    "description",       N_("Wave player component"),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.OutputSampleRate", "16000",
    "conf.default.ChannelNumbers", "1",
#if defined(__linux)
	"conf.default.FileName", "wavrecord-default.wav",
#elif defined(_WIN32)
	"conf.default.FileName", "c:\\work\\wavrecord-default.wav",
#endif
    "conf.__widget__.OutputSampleRate", "spin",
    "conf.__constraints__.OutputSampleRate", "x >= 1",
    "conf.__description__.OutputSampleRate", N_("Sample rate of audio output."),
    "conf.__widget__.Frequency", "spin",
    "conf.__constraints__.Frequency", "x >= 1",
    "conf.__description__.Frequency", N_("Frequency of the signal to genarate."),
    "conf.__widget__.Gain", "spin",
    "conf.__constraints__.Gain", "x >= 1",
    "conf.__description__.Gain", N_("Amplitude of the signal to generate."),
    "conf.__widget__.Mode", "radio",
    "conf.__constraints__.Mode", "(Square, Triangle, Sin)",
    "conf.__description__.Mode", N_("Mode (Square, Triangle or Sin)."),
    "conf.__doc__.usage", "\n  ::\n\n  $ waveplayer\n",
    ""
  };

#if defined(__linux)
//nothing
#elif defined(_WIN32)
#ifdef SHARED_LIB
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
   OFN.lpstrTitle = "ファイルを開く";
   return (GetOpenFileName(&OFN));
}
#endif//SHARED_LIB
#endif//defined(_WIN32)


// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WavPlayer::WavPlayer(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_out_dataOut("AudioDataOut", m_out_data)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WavPlayer::~WavPlayer()
{
}

RTC::ReturnCode_t WavPlayer::onInitialize()
{
  RTC_DEBUG(("onInitialize start"));
  RTC_INFO(("WavPlayer : Wave player component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));

  registerOutPort("AudioDataOut", m_out_dataOut);
  m_out_dataOut.setDescription(_("Audio data out packet."));

  bindParameter("OutputSampleRate", m_samplerate, "16000");
  bindParameter("ChannelNumbers", m_channels, "1");
#if defined(__linux)
	bindParameter("FileName", m_filename, "wavrecord-default.wav");
#ifdef SHARED_LIB
	Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_OPEN );
	// 開く、キャンセルボタン
	diag.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	diag.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	switch( diag.run() ){
	case Gtk::RESPONSE_OK:
	  strncpy(WaveFileName, (diag.get_filename()).c_str(), (diag.get_filename()).size());
	  break;
        case Gtk::RESPONSE_CANCEL:
	  strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
	  break;
	}
	Gtk::MessageDialog( WaveFileName ).run();
#endif //SHARED_LIB
#elif defined(_WIN32)
	bindParameter("FileName", m_filename, "c:\\work\\wavrecord-default.wav");
#ifdef SHARED_LIB
	HWND hwnd = GetWindow( NULL, GW_OWNER );

	ZeroMemory(WaveFileName,MAX_PATH*2);
	strncpy(WaveFileName, m_filename.c_str(), m_filename.size());
	//printf("m_filename.c_str: %s\n", m_filename.c_str());
	//printf("m_filename.size: %d\n", m_filename.size());
	//printf("Wave File Name: %s\n", WaveFileName);
	if (OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName,OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY))
	            
	//MessageBox(hwnd,strcat(WaveFileName,"\nを選択しました。"),"情報",MB_OK);
#endif//SHARED_LIB
#endif//defined(_WIN32)

  RTC_DEBUG(("onInitialize finish"));
  RTC_INFO(("onInitialize finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavPlayer::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  sfinfo.samplerate = (int)m_samplerate;
  sfinfo.channels = m_channels;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  try {
    sfr = sf_open(WaveFileName, SFM_READ, &sfinfo);
    RTC_INFO(("Wave File Name: %s\n", WaveFileName));
	if (sf_format_check(&sfinfo) == 0) {
		RTC_DEBUG(("invalid format"));
		RTC_INFO(("Wave file invalid format"));
		return RTC::RTC_ERROR;
	}
    if (sfr == NULL) {
      //RTC_DEBUG(("unable to open file: %s", m_filename.c_str()));
      RTC_DEBUG(("unable to open file: %s", WaveFileName));
      return RTC::RTC_ERROR;
    }
    m_timer = coil::gettimeofday() - 1.0;
  } catch (...) {
    RTC_WARN(("%s", "error onActivated."));
    return RTC::RTC_ERROR;
  }

  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavPlayer::onExecute(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onExecute start"));
  coil::TimeValue now = coil::gettimeofday();
  long bufferlen = long((now - m_timer) * m_samplerate);
  if ( bufferlen <= 0 ) return RTC::RTC_OK;
  m_timer = now;
  short *buffer = new short[bufferlen];
  sf_readf_short(sfr, buffer, bufferlen) ;
  m_out_data.data.length(bufferlen * 2);  //!< set outport data length
  memcpy((void *)&(m_out_data.data[0]), (void *)&(buffer[0]), bufferlen * 2);
  setTimestamp(m_out_data);
  m_out_dataOut.write();
  delete [] buffer;

  RTC_DEBUG(("onExecute finish"));
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavPlayer::onDeactivated(RTC::UniqueId ec_id)
{
	sf_close( sfr );
	return RTC::RTC_OK;
}

RTC::ReturnCode_t WavPlayer::onFinalize()
{
  return RTC::RTC_OK;
}

extern "C"
{
  void WavPlayerInit(RTC::Manager* manager, char * wave_file_name)
  {
    int i;
#ifdef SHARED_LIB
    //nothing
#else
    strcpy(WaveFileName, wave_file_name);
#endif
    for (i = 0; strlen(waveplayer_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
    	spec_intl[j] = (char *)_(waveplayer_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                           RTC::Create<WavPlayer>,
                           RTC::Delete<WavPlayer>);
  }
};
