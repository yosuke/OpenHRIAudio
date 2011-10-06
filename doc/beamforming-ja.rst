BeamForming
===========
マイクロフォンアレイを用いた音源定位コンポーネント

:Vendor: AIST
:Version: 0.0.1
:Category: communication

Usage
-----

  ::

  $ beamforming


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "mic", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "angle", "DataInPort", "TimedDouble", "角度データ入力"
   "result", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   BeamForming [shape=Mrecord, label="BeamForming"];
   mic [shape=plaintext, label="mic"];
   mic -> BeamForming;
   angle [shape=plaintext, label="angle"];
   angle -> BeamForming;
   result [shape=plaintext, label="result"];
   BeamForming -> result;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "SampleRate", "入力音声データのサンプリング周波数設定"
   "ConstAngle", "固定角度モードにおけるビームを向ける角度の設定 [deg]"
   "Mode", "ビームを固定角度に向けるか可変角度に向けるかの設定"
   "ChannelNumbers", "入力する音声データのチャンネル数設定"

