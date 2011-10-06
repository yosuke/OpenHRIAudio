CombFilter
==========
Comb filter noise reduction component.

:Vendor: AIST
:Version: 1.08
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
   
   "SampleRate", "Sample rate of audio input."
   "Frequency", "Target frequency to reduce the noise."
   "Gain", "Degree of gain to add or subtract [1>=x>=-1]."
   "ChannelNumbers", "Number of audio channel."

