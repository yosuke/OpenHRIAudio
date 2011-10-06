DSArray
=======
マイクロフォンアレイを用いた音源定位コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ dsarray


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "mic", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "result", "DataOutPort", "TimedDouble", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   DSArray [shape=Mrecord, label="DSArray"];
   mic [shape=plaintext, label="mic"];
   mic -> DSArray;
   result [shape=plaintext, label="result"];
   DSArray -> result;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "入力音声データのサンプリング周波数設定"
   "ChannelNumbers", "入力する音声データのチャンネル数設定"

