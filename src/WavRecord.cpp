// -*- C++ -*-
/*!
 * @file  WavRecord.cpp
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

#if defined(__linux)
static char WaveFileName[512*2]; 
#elif defined(_WIN32)
static char WaveFileName[MAX_PATH*2]; 
#endif

#define SAMPLE_RATE 44100
#define BIT_DEPTH 16

// Module specification
// <rtc-template block="module_spec">
static const char* wavrecord_spec[] =
  {
    "implementation_id", "WavRecord",
    "type_name",         "WavRecord",
    "description",       N_("Sound record component."),
    "version",           VERSION,
    "vendor",            "AIST",
    "category",          "communication",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "script",
    "conf.default.SampleRate", "16000",
    "conf.default.ChannelNumbers", "1",
#if defined(__linux)
	"conf.default.FileName", "wavrecord-default.wav",
#elif defined(_WIN32)
	"conf.default.FileName", "c:\\work\\wavrecord-default.wav",
#endif
    "conf.__widget__.SampleRate", "spin",
    "conf.__constraints__.SampleRate", "x >= 1",
    "conf.__description__.SampleRate", N_("Sample rate of audio input."),
    "conf.__widget__.ChannelNumbers", "spin",
    "conf.__constraints__.ChannelNumbers", "x >= 1",
    "conf.__description__.ChannelNumbers", N_("Number of audio input channels."),
    "conf.__description__.FileName", N_("Name of file to save the recorded data."),
    "conf.__doc__.usage", "\n  ::\n\n  $ wavrecord\n",
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
  WavRecord *p = (WavRecord *) m_obj;
  if (m_name == "ON_BUFFER_WRITE") {
    p->RcvBuffer(data);
  }
}
/*!
 * @brief constructor
 * @param manager Maneger Object
 */
WavRecord::WavRecord(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_in_dataIn("AudioDataIn", m_in_data)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
WavRecord::~WavRecord()
{
}

RTC::ReturnCode_t WavRecord::onInitialize()
{
  RTC_INFO(("WavRecord : Audio record component"));
  RTC_INFO((" Copyright (C) 2010-2011 Yosuke Matsusaka and AIST-OpenHRI development team"));
  RTC_INFO((" Version %s", VERSION));
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  m_in_dataIn.addConnectorDataListener(ON_BUFFER_WRITE, new DataListener("ON_BUFFER_WRITE", this));
  m_in_dataIn.setDescription(_("Audio data input."));
  registerInPort("AudioDataIn", m_in_dataIn);

  // Set service provider to Ports

  // Set service consumers to Ports

  // Set CORBA Service Ports

  // </rtc-template>
  bindParameter("SampleRate", m_rate, "16000");
  bindParameter("ChannelNumbers", m_channels, "1");
  //bindParameter("FileName", m_filename, "");
#if defined(__linux)
	bindParameter("FileName", m_filename, "wavrecord-default.wav");
#ifdef SHARED_LIB
	Gtk::FileChooserDialog diag( "ファイル選択", Gtk::FILE_CHOOSER_ACTION_SAVE );
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
	OpenDiaog(hwnd,"Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0\0",
					WaveFileName,OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT);
	            
#endif//SHARED_LIB
#endif//defined(_WIN32)

  RTC_INFO(("onInitialize finish"));

  is_active = false;

  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onActivated(RTC::UniqueId ec_id)
{
  RTC_DEBUG(("onActivated start"));
  RTC_INFO(("Wave File Name: %s\n", WaveFileName));
  sfinfo.samplerate = m_rate;
  sfinfo.channels = m_channels;
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  if (sf_format_check(&sfinfo) == 0) {
    RTC_DEBUG(("invalid format"));
    return RTC::RTC_ERROR;
  }
  sfw = sf_open(WaveFileName, SFM_WRITE, &sfinfo);
  if (sfw == NULL) {
    RTC_DEBUG(("unable to open file: %s", WaveFileName));
    return RTC::RTC_ERROR;
  }
  is_active = true;
  RTC_DEBUG(("onActivated finish"));
  return RTC::RTC_OK;
}

void WavRecord::RcvBuffer(TimedOctetSeq data)
{
  RTC_DEBUG(("RcvBuffer start"));
  if (is_active == true) {
    m_mutex.lock();
    RTC_DEBUG(("RcvBuffer:mutex lock"));
    sf_count_t frames = data.data.length();
    sf_write_raw(sfw, &data.data[0], frames);
    m_mutex.unlock();
    RTC_DEBUG(("RcvBuffer:mutex unlock"));
  }
  RTC_DEBUG(("RcvBuffer finish"));
  return;
}

RTC::ReturnCode_t WavRecord::onExecute(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onDeactivated(RTC::UniqueId ec_id)
{
  is_active = false;
  sf_close(sfw);
  return RTC::RTC_OK;
}

RTC::ReturnCode_t WavRecord::onFinalize()
{
  return RTC::RTC_OK;
}

extern "C"
{
  void WavRecordInit(RTC::Manager* manager, char * wave_file_name)
  {
    int i;
#ifdef SHARED_LIB
    //nothing
#else
    strcpy(WaveFileName, wave_file_name);
#endif
    for (i = 0; strlen(wavrecord_spec[i]) != 0; i++);
    char** spec_intl = new char*[i + 1];
    for (int j = 0; j < i; j++) {
      spec_intl[j] = _((char *)wavrecord_spec[j]);
    }
    spec_intl[i] = (char *)"";
    coil::Properties profile((const char **)spec_intl);
    manager->registerFactory(profile,
                             RTC::Create<WavRecord>,
                             RTC::Delete<WavRecord>);
  }
};

