PulseAudioOutput
================
PulseAudioライブラリを使用した音声出力コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ pulseaudiooutput


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   PulseAudioOutput [shape=Mrecord, label="PulseAudioOutput"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> PulseAudioOutput;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   PulseAudioOutput -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "OutputChannelNumbers", "出力する音声データのチャンネル数設定"
   "OutputSampleByte", "出力する音声データのフォーマット設定"
   "BufferLength", "Length of buffer (in seconds)."
   "OutputSampleRate", "出力する音声データのサンプリング周波数設定"

