EchoCanceler
============
適応フィルタ使用した音響エコー除去コンポーネント

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
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "マイクからの音声データ入力"
   "ReferenceAudioDataIn", "DataInPort", "TimedOctetSeq", "音声出力コンポーネントからの音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"
   "Parameter", "DataOutPort", "TimedDoubleSeq", "適応フィルタの設定値"

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
   
   "Gain", "適応フィルタ倍率"

