CombFilter
==========
ノイズ除去コンポーネント

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ combfilter


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   CombFilter [shape=Mrecord, label="CombFilter"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> CombFilter;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   CombFilter -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "Delay", ""
   "SampleRate", "入力音声データのサンプリング周波数設定"
   "ChannelNumbers", "出力する音声データのチャンネル数設定"

