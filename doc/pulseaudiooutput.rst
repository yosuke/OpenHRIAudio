PulseAudioOutput
================
Audio output component using pulseaudio

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ pulseaudiooutput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Actual audio data output."

.. digraph:: comp

   rankdir=LR;
   PulseAudioOutput [shape=Mrecord, label="PulseAudioOutput"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> PulseAudioOutput;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PulseAudioOutput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "OutputChannelNumbers", "Number of audio channel."
   "OutputSampleByte", "Sample byte of audio output."
   "BufferLength", "Length of buffer (in seconds)."
   "OutputSampleRate", "Sample rate of audio output."

