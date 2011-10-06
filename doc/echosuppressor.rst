EchoSuppressor
==============
Acoustic echo cancellation component

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ echosuppressor


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
   EchoSuppressor [shape=Mrecord, label="EchoSuppressor"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> EchoSuppressor;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> EchoSuppressor;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   EchoSuppressor -> AudioDataOut;

