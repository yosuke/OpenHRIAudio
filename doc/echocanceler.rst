EchoCanceler
============
Acoustic echo cancellation component using adaptive filter

:Vendor: AIST
:Version: 1.06
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
   "Parameter", "DataOutPort", "TimedDoubleSeq", "Current parameter of adaptive filter."

.. digraph:: comp

   rankdir=LR;
   EchoCanceler [shape=Mrecord, label="EchoCanceler"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> EchoCanceler;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> EchoCanceler;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   EchoCanceler -> AudioDataOut;
   Parameter [shape=plaintext, label="Parameter"];
   EchoCanceler -> Parameter;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "Gain", "Gain of the adaptive filter"

