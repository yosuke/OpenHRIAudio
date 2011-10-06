PortAudioInput
==============
PulseAudioライブラリを使用した音声入力コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ portaudioinput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "GainDataIn", "DataInPort", "TimedLong", "Gain."
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "パケット化した音声データ"

.. digraph:: comp

   rankdir=LR;
   PortAudioInput [shape=Mrecord, label="PortAudioInput"];
   GainDataIn [shape=plaintext, label="GainDataIn"];
   GainDataIn -> PortAudioInput;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PortAudioInput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "InputSampleRate", "録音する音声のサンプリング周波数設定"
   "InputChannelNumbers", "出力する音声データのチャンネル数設定"
   "InputSampleByte", "録音する音声のフォーマット設定"

