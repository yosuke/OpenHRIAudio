AutoGainControl
===============
Automatic gain control component

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ autogaincontrol


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."
   "AudioDataOut", "DataOutPort", "TimedLong", "Gain."

.. digraph:: comp

   rankdir=LR;
   AutoGainControl [shape=Mrecord, label="AutoGainControl"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> AutoGainControl;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   AutoGainControl -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "GainCtlSampleByte", "Sample byte of gain control."
   "InputSampleByte", "Sample byte of audio capture."
   "AutomaticGainControl", "Automatic gain control parameter."
   "InputChannelNumbers", "Number of audio channel."

