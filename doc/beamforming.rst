BeamForming
===========
Sound localization component using microphone array.

:Vendor: AIST
:Version: 0.0.1
:Category: communication

Usage
-----

  ::
  $ beamforming


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "mic", "DataInPort", "TimedOctetSeq", ""
   "angle", "DataInPort", "TimedDouble", ""
   "result", "DataOutPort", "TimedOctetSeq", ""
   "parameter", "DataOutPort", "TimedDoubleSeq", ""

.. digraph:: comp

   rankdir=LR;
   BeamForming [shape=Mrecord, label="BeamForming"];
   mic [shape=plaintext, label="mic"];
   mic -> BeamForming;
   angle [shape=plaintext, label="angle"];
   angle -> BeamForming;
   result [shape=plaintext, label="result"];
   BeamForming -> result;
   parameter [shape=plaintext, label="parameter"];
   BeamForming -> parameter;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "Sample rate of audio input."
   "ConstAngle", ""
   "Mode", ""
   "ChannelNumbers", "Number of audio input channels."

