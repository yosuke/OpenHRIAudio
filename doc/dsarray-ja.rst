DSArray
=======
Sound localization component using microphone array.

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::
  $ dsarray


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "mic", "DataInPort", "TimedOctetSeq", "Audio data input."
   "result", "DataOutPort", "TimedDouble", "Audio data output."

.. digraph:: comp

   rankdir=LR;
   DSArray [shape=Mrecord, label="DSArray"];
   mic [shape=plaintext, label="mic"];
   mic -> DSArray;
   result [shape=plaintext, label="result"];
   DSArray -> result;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "Sample rate of audio input."
   "ChannelNumbers", "Number of audio input channels."

