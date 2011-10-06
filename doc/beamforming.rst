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
   
   "mic", "DataInPort", "TimedOctetSeq", "Audio data input."
   "angle", "DataInPort", "TimedDouble", "Angle data input."
   "result", "DataOutPort", "TimedOctetSeq", "Audio data output."

.. digraph:: comp

   rankdir=LR;
   BeamForming [shape=Mrecord, label="BeamForming"];
   mic [shape=plaintext, label="mic"];
   mic -> BeamForming;
   angle [shape=plaintext, label="angle"];
   angle -> BeamForming;
   result [shape=plaintext, label="result"];
   BeamForming -> result;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "Sample rate of audio input."
   "ConstAngle", "Direction of the beam in constant mode [deg]."
   "Mode", "Whether to direct the beam against constant or variable angle."
   "ChannelNumbers", "Number of audio input channels."

