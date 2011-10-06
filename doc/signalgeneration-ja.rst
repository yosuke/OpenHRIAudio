SignalGeneration
================
信号生成コンポーネント

:Vendor: AIST
:Version: 1.08
:Category: communication

Usage
-----

  ::

  $ signalgeneration


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "パケット化した音声データ"

.. digraph:: comp

   rankdir=LR;
   SignalGeneration [shape=Mrecord, label="SignalGeneration"];
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   SignalGeneration -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "Mode", ""
   "Frequency", ""
   "Gain", ""
   "OutputSampleRate", "出力する音声データのサンプリング周波数設定"

