#!/usr/bin/env python

import subprocess
import time

langs = [
    ('C', ''),
    ('ja_JP.UTF-8', '-ja'),
]

comps = [
    ('pulseaudioinput', 'PulseAudioInput'),
    ('pulseaudiooutput', 'PulseAudioOutput'),
    ('portaudioinput', 'PortAudioInput'),
    ('portaudiooutput', 'PortAudioOutput'),
    ('noisereduction', 'NoiseReduction'),
    ('echosuppressor', 'EchoSuppressor'),
    ('echocanceler', 'EchoCanceler'),
    ('combfilter', 'CombFilter'),
    ('signalgeneration', 'SignalGeneration'),
    ('mixer', 'Mixer'),
    ('autogaincontrol', 'AutoGainControl'),
    ('dsarray', 'DSArray'),
]

tools = [
]

for c in comps:
    comppath = '/localhost/`hostname`.host_cxt/' + c[1] + '0.rtc'
    subprocess.call('rtexit ' + comppath, shell = True)
    time.sleep(1)
    for l in langs:
        subprocess.call('LANG=' + l[0] + ' ' + c[0] + ' &', shell = True)
        time.sleep(2)
        subprocess.call('rtdoc --format=rst --graph ' + comppath + ' > ' + c[0] + l[1] + '.rst', shell = True)
        subprocess.call('rtexit ' + comppath, shell = True)
        time.sleep(1)

for t in tools:
    for l in langs:
        subprocess.call('LANG=' + l[0] + ' ' + t + ' --help > ' + t + l[1] + '.rst', shell = True)
