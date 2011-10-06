NoiseReduction
==============
Noise reduction component.

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ noisereduction


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data output."

.. digraph:: comp

   rankdir=LR;
   NoiseReduction [shape=Mrecord, label="NoiseReduction"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> NoiseReduction;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   NoiseReduction -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "FrequencyMin", "Min frequency (effective in 'bpf' mode."
   "Power", "Magnification rate of the emphasis."
   "ModeSelect", "Specify filtering method ['premp': Pre-emphasis mode, 'deemp': De-emphasis mode, 'bpf': Bandpass filter mode]."
   "FrequencyMax", "Max frequency (effective in 'bpf' mode."
   "CenterFrequency", "Center frequency (effective in 'premp' and 'deemp' mode."
   "SampleRate", "Sample rate of audio input."

