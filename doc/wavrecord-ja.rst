WavRecord
=========
録音コンポーネント

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
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "音声データ入力"

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
   
   "SampleRate", "入力音声データのサンプリング周波数設定"
   "FileName", "録音したデータを保存するファイル名"
   "ChannelNumbers", "入力する音声データのチャンネル数設定"

