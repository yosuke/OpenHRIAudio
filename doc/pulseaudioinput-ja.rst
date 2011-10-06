PulseAudioInput
===============
PulseAudioライブラリを使用した音声入力コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ pulseaudioinput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "GainDataIn", "DataInPort", "TimedLong", "音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "パケット化した音声データ"

.. digraph:: comp

   rankdir=LR;
   PulseAudioInput [shape=Mrecord, label="PulseAudioInput"];
   GainDataIn [shape=plaintext, label="GainDataIn"];
   GainDataIn -> PulseAudioInput;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PulseAudioInput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "InputSampleRate", "録音する音声のサンプリング周波数設定"
   "InputChannelNumbers", "出力する音声データのチャンネル数設定"
   "InputSampleByte", "録音する音声のフォーマット設定"

