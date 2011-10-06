NoiseReduction
==============
ノイズ除去コンポーネント

:Vendor: AIST
:Version: 1.06
:Category: communication

Usage
-----

  ::
  $ noisereduction


Ports
-----
.. csv-table:: Ports
   :header: "Name", "Type", "DataType", "Description"
   :widths: 8, 8, 8, 26
   
   "AudioDataIn", "DataInPort", "TimedOctetSeq", "音声データ入力"
   "AudioDataOut", "DataOutPort", "TimedOctetSeq", "音声データ出力"

.. digraph:: comp

   rankdir=LR;
   NoiseReduction [shape=Mrecord, label="NoiseReduction"];
   AudioDataIn [shape=plaintext, label="AudioDataIn"];
   AudioDataIn -> NoiseReduction;
   AudioDataOut [shape=plaintext, label="AudioDataOut"];
   NoiseReduction -> AudioDataOut;

Configuration parameters
------------------------
.. csv-table:: Configuration parameters
   :header: "Name", "Description"
   :widths: 12, 38
   
   "FrequencyMin", "バンドパスフィルタモードでの最小周波数"
   "Power", "強調倍率"
   "ModeSelect", "フィルタモード選択 「premp」=プリエンファシスモード、「deemp」=ディエンファシスモード、「bpf」=バンドパスフィルタモード"
   "FrequencyMax", "バンドパスフィルタモードでの最大周波数"
   "CenterFrequency", "プリエンファシスモード及びディエンファシスモードでの中心周波数"
   "SampleRate", "入力音声データのサンプリング周波数設定"

