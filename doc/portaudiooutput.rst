PortAudioOutput
===============
Audio output component using portaudio

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ portaudiooutput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Actual audio data output."

.. digraph:: comp

   rankdir=LR;
   PortAudioOutput [shape=Mrecord, label="PortAudioOutput"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> PortAudioOutput;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PortAudioOutput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "DelayCount", "Buffer length of audio output."
   "OutputChannelNumbers", "Number of audio channel."
   "OutputSampleByte", "Sample byte of audio output."
   "OutputSampleRate", "Sample rate of audio output."

