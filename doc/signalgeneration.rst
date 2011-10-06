SignalGeneration
================
Signal generation component

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ signalgeneration


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data in packet."

.. digraph:: comp

   rankdir=LR;
   SignalGeneration [shape=Mrecord, label="SignalGeneration"];
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   SignalGeneration -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "Mode", "Mode (Square, Triangle or Sin)."
   "Frequency", "Frequency of the signal to genarate."
   "Gain", "Amplitude of the signal to generate."
   "OutputSampleRate", "Sample rate of audio output."

