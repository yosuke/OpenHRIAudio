Mixer
=====
Audio mixer component

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ mixer


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "Audio data input (from mic)."
   "ReferenceAudioDataIn", "DataInPort", "TimedOctetSeq", "Referenct audio data input (from AudioOuput component)."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "Audio data output."

.. digraph:: comp

   rankdir=LR;
   Mixer [shape=Mrecord, label="Mixer"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> Mixer;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> Mixer;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   Mixer -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "OutputChannelNumbers", "Number of output audio channels."
   "MixGains", "Gains for mixing signals."

