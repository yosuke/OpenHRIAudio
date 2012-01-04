;  This file is part of OpenHRI.
;  Copyright (C) 2010  AIST-OpenHRI Project
;

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

  !include "x64.nsh"
  !include "LogicLib.nsh"

!define PACKAGE_NAME "OpenHRIAudio"
!define PACKAGE_VERSION "2.00"
!define OUTFILE "${PACKAGE_NAME}-${PACKAGE_VERSION}-installer.exe"
!define TOP_SRCDIR "..\.."
!define TOP_BUILDDIR "..\.."
!define INSTDIR_REG_ROOT "HKLM"
!define INSTDIR_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}"
!define SCDIR "$SMPROGRAMS\OpenHRI\audio"

;Name and file
Name "${PACKAGE_NAME} ${PACKAGE_VERSION}"
OutFile "${OUTFILE}"
ShowInstDetails show
ShowUninstDetails show
InstallDir "$PROGRAMFILES\${PACKAGE_NAME}"
InstallDirRegKey ${INSTDIR_REG_ROOT} ${INSTDIR_REG_KEY} "InstallDir"

!include "AdvUninstLog.nsh"
!insertmacro UNATTENDED_UNINSTALL
;!insertmacro INTERACTIVE_UNINSTALL

;--------------------------------
;Interface Settings

;  !define MUI_ICON "${TOP_SRCDIR}\icons\openhriaudio.ico"
;  !define MUI_UNICON "${TOP_SRCDIR}\icons\openhriaudio.uninstall.ico"

  !define MUI_ABORTWARNING

  ;Start Menu Folder Page Configuration (for MUI_PAGE_STARTMENU)
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PACKAGE_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  ; These indented statements modify settings for MUI_PAGE_FINISH
  !define MUI_FINISHPAGE_NOAUTOCLOSE

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE $(MUILicense)
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "Japanese"

;--------------------------------
;License Language String

  LicenseLangString MUILicense ${LANG_ENGLISH} "${TOP_SRCDIR}\COPYING"
  LicenseLangString MUILicense ${LANG_JAPANESE} "${TOP_SRCDIR}\COPYING"

;--------------------------------
;Reserve Files

  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)

  !insertmacro MUI_RESERVEFILE_LANGDLL


;--------------------------------
;Installer Sections

Section $(TEXT_SecBase) SecBase

  SetOutPath "$INSTDIR"

  !insertmacro UNINSTALL.LOG_OPEN_INSTALL
  ; Main executables
  File "/oname=portaudioinput.exe" "${TOP_BUILDDIR}\win\components\PortAudioInput.exe"
  File "/oname=portaudiooutput.exe" "${TOP_BUILDDIR}\win\components\PortAudioOutput.exe"
  File "/oname=echocanceler.exe" "${TOP_BUILDDIR}\win\components\EchoCanceler.exe"
  File "/oname=echosuppressor.exe" "${TOP_BUILDDIR}\win\components\EchoSuppressor.exe"
  File "/oname=noisereduction.exe" "${TOP_BUILDDIR}\win\components\NoiseReduction.exe"
  File "/oname=samplingrateconverter.exe" "${TOP_BUILDDIR}\win\components\SamplingRateConverter.exe"
  File "/oname=combfilter.exe" "${TOP_BUILDDIR}\win\components\CombFilter.exe"
  File "/oname=mixer.exe" "${TOP_BUILDDIR}\win\components\Mixer.exe"
  File "/oname=signalgeneration.exe" "${TOP_BUILDDIR}\win\components\SignalGeneration.exe"
  File "/oname=dsarray.exe" "${TOP_BUILDDIR}\win\components\DSArray.exe"
  File "/oname=beamforming.exe" "${TOP_BUILDDIR}\win\components\BeamForming.exe"
  File "/oname=channelselector.exe" "${TOP_BUILDDIR}\win\components\ChannelSelector.exe"
  File "/oname=OpenHRIAudioManager.exe" "${TOP_BUILDDIR}\win\components\OpenHRIAudioManager.exe"
  ;;File "${TOP_BUILDDIR}\win\components\PortAudioRTC.dll" ;;by Irie Seisho
  File "${TOP_BUILDDIR}\win\components\PortAudioInput.dll";;by Irie Seisho
  File "${TOP_BUILDDIR}\win\components\PortAudioOutput.dll";;by Irie Seisho
  File "${TOP_BUILDDIR}\win\components\EchoCanceler.dll"
  File "${TOP_BUILDDIR}\win\components\EchoSuppressor.dll"
  File "${TOP_BUILDDIR}\win\components\NoiseReduction.dll"
  File "${TOP_BUILDDIR}\win\components\SamplingRateConverter.dll"
  File "${TOP_BUILDDIR}\win\components\CombFilter.dll"
  File "${TOP_BUILDDIR}\win\components\Mixer.dll"
  File "${TOP_BUILDDIR}\win\components\SignalGeneration.dll"
  File "${TOP_BUILDDIR}\win\components\DSArray.dll"
  File "${TOP_BUILDDIR}\win\components\BeamForming.dll"
  File "${TOP_BUILDDIR}\win\components\ChannelSelector.dll"
  File "${TOP_BUILDDIR}\win\components\AutoGainControl.dll"
  File "${TOP_BUILDDIR}\win\components\WavRecord.dll"
  File "${TOP_BUILDDIR}\win\components\WavPlayer.dll"
  File "${TOP_BUILDDIR}\win\components\micset.csv"
  File "${TOP_BUILDDIR}\win\libportaudio.dll"
  File "${TOP_BUILDDIR}\win\libspeex.dll"
;  File "${TOP_BUILDDIR}\win\libspeexdsp.dll"
  File "${TOP_BUILDDIR}\win\libresample.dll"
  File "${TOP_BUILDDIR}\win\libsndfile-1.dll"
;${If} ${RunningX64}
  File "C:\Program Files (x86)\omniORB-4.1.4\bin\x86_win32\*.dll"
  File "C:\Program Files (x86)\OpenRTM-aist\1.0\bin\*.dll"
;${Else}
;  File "C:\Program Files\omniORB-4.1.4\bin\x86_win32\*.dll"
;  File "C:\Program Files\OpenRTM-aist\1.0\bin\*.dll"
;${EndIf}
  File "rtc.conf"

  ; Information/documentation files
  ;File "/oname=ChangeLog.txt" "${TOP_SRCDIR}\ChangeLog"
  File "/oname=Authors.txt" "${TOP_SRCDIR}\AUTHORS"
  File "/oname=License.txt" "${TOP_SRCDIR}\COPYING"

  !insertmacro UNINSTALL.LOG_CLOSE_INSTALL

  ;Store installation folder
  WriteRegStr HKLM "Software\${PACKAGE_NAME}" "" $INSTDIR

  ; Write the Windows-uninstall keys
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "DisplayName" "${PACKAGE_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "DisplayVersion" "${PACKAGE_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "Publisher" "AIST-OpenHRI Project"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "UninstallString" "$INSTDIR\uninstall.exe"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}" "NoRepair" 1

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ;Create shortcuts
  CreateDirectory "${SCDIR}"
  CreateShortCut "${SCDIR}\Uninstall Audio Components.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "${SCDIR}\portaudioinput.lnk" "$INSTDIR\portaudioinput.exe"
  CreateShortCut "${SCDIR}\portaudiooutput.lnk" "$INSTDIR\portaudiooutput.exe"
  CreateShortCut "${SCDIR}\echocanceler.lnk" "$INSTDIR\echocanceler.exe"
  CreateShortCut "${SCDIR}\echosuppressor.lnk" "$INSTDIR\echosuppressor.exe"
  CreateShortCut "${SCDIR}\noisereduction.lnk" "$INSTDIR\noisereduction.exe"
  CreateShortCut "${SCDIR}\samplingrateconverter.lnk" "$INSTDIR\samplingrateconverter.exe"
  CreateShortCut "${SCDIR}\combfilter.lnk" "$INSTDIR\combfilter.exe"
  CreateShortCut "${SCDIR}\mixer.lnk" "$INSTDIR\mixer.exe"
  CreateShortCut "${SCDIR}\signalgeneration.lnk" "$INSTDIR\signalgeneration.exe"
  CreateShortCut "${SCDIR}\dsarray.lnk" "$INSTDIR\dsarray.exe"
  CreateShortCut "${SCDIR}\beamforming.lnk" "$INSTDIR\beamforming.exe"
  CreateShortCut "${SCDIR}\channelselector.lnk" "$INSTDIR\channelselector.exe"
  CreateShortCut "${SCDIR}\OpenHRIAudioManager.lnk" "$INSTDIR\OpenHRIAudioManager.exe" -d

SectionEnd

;--------------------------------
;Installer Functions

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  !insertmacro UNINSTALL.LOG_PREPARE_INSTALL
FunctionEnd

Function .onInstSuccess
  !insertmacro UNINSTALL.LOG_UPDATE_INSTALL
FunctionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString TEXT_SecBase ${LANG_ENGLISH} "Standard installation"
  LangString DESC_SecBase ${LANG_ENGLISH} "Standard installation."

  LangString TEXT_SecBase ${LANG_JAPANESE} "Standard installation"
  LangString DESC_SecBase ${LANG_JAPANESE} "Standard installation"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecBase} $(DESC_SecBase)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END



;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  ;!insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL
  !insertmacro UNINSTALL.LOG_UNINSTALL "$INSTDIR"
  !insertmacro UNINSTALL.LOG_END_UNINSTALL

  Delete "$INSTDIR\uninstall.exe"

  Delete "${SCDIR}\Uninstall Audio Components.lnk"
  Delete "${SCDIR}\portaudioinput.lnk"
  Delete "${SCDIR}\portaudiooutput.lnk"
  Delete "${SCDIR}\echocanceler.lnk"
  Delete "${SCDIR}\echosuppressor.lnk"
  Delete "${SCDIR}\noisereduction.lnk"
  Delete "${SCDIR}\samplingrateconverter.lnk"
  Delete "${SCDIR}\combfilter.lnk"
  Delete "${SCDIR}\mixer.lnk"
  Delete "${SCDIR}\signalgeneration.lnk"
  Delete "${SCDIR}\dsarray.lnk"
  Delete "${SCDIR}\beamforming.lnk"
  Delete "${SCDIR}\channelselector.lnk"
  Delete "${SCDIR}\OpenHRIAudioManager.lnk"
  RMDir "${SCDIR}"

  DeleteRegValue HKLM "Software\${PACKAGE_NAME}" "Start Menu Folder"
  DeleteRegValue HKLM "Software\${PACKAGE_NAME}" ""
  DeleteRegKey /ifempty HKLM "Software\${PACKAGE_NAME}"

  ; Unregister with Windows' uninstall system
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE_NAME}"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  !insertmacro UNINSTALL.LOG_BEGIN_UNINSTALL
FunctionEnd

