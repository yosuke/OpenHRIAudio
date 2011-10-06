EchoCanceler
============
適応フィルタ使用した音響エコー除去コンポーネント

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
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "マイクからの音声データ入力"
   "ReferenceAudioDataIn", "DataInPort", "TimedOctetSeq", "音声出力コンポーネントからの音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   EchoCanceler [shape=Mrecord, label="EchoCanceler"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> EchoCanceler;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> EchoCanceler;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   EchoCanceler -> AudioDataOut;

