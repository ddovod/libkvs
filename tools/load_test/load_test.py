#!/usr/bin/env python

import argparse
import ruamel.yaml
import subprocess
import shutil
import os
import matplotlib.pyplot as plt


def runTestCase(testConfig, executable):
    name = ''
    runs = 1
    cmd = [executable]
    testDataDir = 'load_test_data'
    for k, v in testConfig.items():
        if k == 'Name':
            name = v
        elif k == 'TotalRuns':
            runs = v
        else:
            cmd.append('--' + k + '=' + str(v))

    totalOps = []
    for i in range(0, runs):
        if os.path.exists(testDataDir):
            shutil.rmtree(testDataDir)
        proc = subprocess.Popen(' '.join(cmd), shell=True,
                                stdout=subprocess.PIPE)
        out, _ = proc.communicate()
        ops = []
        for line in out.splitlines():
            if len(line) != 0:
                els = line.split(',')
                if float(els[0]) / float(testConfig['window']) > 0.9:
                    ops.append(els[1])
        totalOps.append(ops)
    result = []
    for el in totalOps:
        if len(result) == 0:
            result = [int(x) for x in el]
        else:
            for i in range(0, len(el)):
                result[i] += int(el[i])
    for i in range(0, len(result)):
        result[i] /= len(totalOps)
    return (name, result)


parser = argparse.ArgumentParser()
parser.add_argument('--config', required=True,
                    help='Path to the config file')
parser.add_argument('--executable', required=True,
                    help='Path to the load_test executable')
args = parser.parse_args()

config = ruamel.yaml.load(open(args.config), Loader=ruamel.yaml.Loader)
results = []
for el in config['Scenarios']:
    results.append(runTestCase(el, args.executable))

labels = []
for el in results:
    (name, data) = el
    plt.plot(data)
    labels.append(name)

plt.axis([None, None, 0, None])
plt.legend(labels, loc='best')
plt.savefig('test_result.png')
