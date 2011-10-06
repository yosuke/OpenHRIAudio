PortAudioInput
==============
Audio input component using portaudio

:Vendor: AIST
:Version: 1.08
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
   
   "GainDataIn", "DataInPort", "TimedLong", "Gain."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data in packet."

.. digraph:: comp

   rankdir=LR;
   PortAudioInput [shape=Mrecord, label="PortAudioInput"];
   GainDataIn [shape=plaintext, label="GainDataIn"];
   GainDataIn -> PortAudioInput;
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

