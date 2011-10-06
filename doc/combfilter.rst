CombFilter
==========
Noise reduction component.

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ combfilter


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data output."

.. digraph:: comp

   rankdir=LR;
   CombFilter [shape=Mrecord, label="CombFilter"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> CombFilter;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   CombFilter -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "Delay", ""
   "SampleRate", "Sample rate of audio input."
   "ChannelNumbers", "Number of audio channel."

