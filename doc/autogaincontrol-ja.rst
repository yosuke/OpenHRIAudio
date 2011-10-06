AutoGainControl
===============
ゲイン調整コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ autogaincontrol


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedLong", "ゲイン"

.. digraph:: comp

   rankdir=LR;
   AutoGainControl [shape=Mrecord, label="AutoGainControl"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> AutoGainControl;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   AutoGainControl -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "GainCtlSampleByte", "ゲイン調整のバイト数"
   "InputSampleByte", "録音する音声のフォーマット設定"
   "AutomaticGainControl", "ゲイン調整パラメータ"
   "InputChannelNumbers", "出力する音声データのチャンネル数設定"

