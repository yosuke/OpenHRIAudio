/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "critical_section_wrapper.h"
#include "monitor_module.h"

namespace webrtc  {

namespace voe  {

MonitorModule::MonitorModule() :
    _observerPtr(NULL),
    _callbackCritSect(*CriticalSectionWrapper::CreateCriticalSection()),
    _lastProcessTime(GET_TIME_IN_MS())
{
}

MonitorModule::~MonitorModule()
{
    delete &_callbackCritSect;
}

WebRtc_Word32 
MonitorModule::RegisterObserver(MonitorObserver& observer)
{
    CriticalSectionScoped lock(_callbackCritSect);
    if (_observerPtr)
    {
        return -1;
    }
    _observerPtr = &observer;
    return 0;
}

WebRtc_Word32 
MonitorModule::DeRegisterObserver()
{
    CriticalSectionScoped lock(_callbackCritSect);
    if (!_observerPtr)
    {
        return 0;
    }
    _observerPtr = NULL;
    return 0;
}

WebRtc_Word32 
MonitorModule::Version(WebRtc_Word8* version,
                       WebRtc_UWord32& remainingBufferInBytes,
                       WebRtc_UWord32& position) const
{
    return 0;
}
   
WebRtc_Word32 
MonitorModule::ChangeUniqueId(const WebRtc_Word32 id)
{
    return 0;
}

WebRtc_Word32 
MonitorModule::TimeUntilNextProcess()
{
    WebRtc_UWord32 now = GET_TIME_IN_MS();
    WebRtc_Word32 timeToNext =
        kAverageProcessUpdateTimeMs - (now - _lastProcessTime);
    return (timeToNext); 
}

WebRtc_Word32 
MonitorModule::Process()
{
    _lastProcessTime = GET_TIME_IN_MS();
    if (_observerPtr)
    {
        CriticalSectionScoped lock(_callbackCritSect);
        _observerPtr->OnPeriodicProcess();
    }
    return 0;
}

}  //  namespace voe

}  //  namespace webrtc
