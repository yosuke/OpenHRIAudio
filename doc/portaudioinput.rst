PortAudioInput
==============
Audio input component using portaudio

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::

  $ portaudioinput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data in packet."

.. digraph:: comp

   rankdir=LR;
   PortAudioInput [shape=Mrecord, label="PortAudioInput"];
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PortAudioInput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "InputSampleRate", "Sample rate of audio capture."
   "InputChannelNumbers", "Number of audio channel."
   "InputSampleByte", "Sample byte of audio capture."

