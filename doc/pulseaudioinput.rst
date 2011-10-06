PulseAudioInput
===============
Audio input component using pulseaudio

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ pulseaudioinput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data in packet."

.. digraph:: comp

   rankdir=LR;
   PulseAudioInput [shape=Mrecord, label="PulseAudioInput"];
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PulseAudioInput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "InputSampleRate", "Sample rate of audio capture."
   "InputChannelNumbers", "Number of audio channel."
   "InputSampleByte", "Sample byte of audio capture."

