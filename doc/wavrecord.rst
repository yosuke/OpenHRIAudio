WavRecord
=========
Sound record component.

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ wavrecord


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input."

.. digraph:: comp

   rankdir=LR;
   WavRecord [shape=Mrecord, label="WavRecord"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> WavRecord;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "Sample rate of audio input."
   "FileName", "Name of file to save the recorded data."
   "ChannelNumbers", "Number of audio input channels."

