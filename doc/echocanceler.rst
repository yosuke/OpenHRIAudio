EchoCanceler
============
Acoustic echo cancellation component using adaptive filter

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ echocanceler


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
   EchoCanceler [shape=Mrecord, label="EchoCanceler"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> EchoCanceler;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> EchoCanceler;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   EchoCanceler -> AudioDataOut;

