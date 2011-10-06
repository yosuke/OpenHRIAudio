EchoSuppressor
==============
音響エコー除去コンポーネント

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
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "マイクからの音声データ入力"
   "ReferenceAudioDataIn", "DataInPort", "TimedOctetSeq", "音声出力コンポーネントからの音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   EchoSuppressor [shape=Mrecord, label="EchoSuppressor"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> EchoSuppressor;
   ReferenceAudioDataIn [shape=plaintext, label="ReferenceAudioDataIn"];
   ReferenceAudioDataIn -> EchoSuppressor;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   EchoSuppressor -> AudioDataOut;

