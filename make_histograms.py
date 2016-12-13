#!/usr/bin/python2.7

import numpy as np
import subprocess

assert(subprocess.call("make histogram-stuff", shell=True) == 0)

have_slurm = True

try:
    subprocess.check_call("squeue > /dev/null", shell=True)
except (OSError, subprocess.CalledProcessError):
    print "Not using slurm..."
    have_slurm = False
    
ls_min = 7.0 # nm
ls_max = 7.0 # nm
ls_num = 1

lt_min = 7.075 # nm
lt_max = 7.075 # nm
lt_num = 1

k_b_min = 2080 # s^-1
k_b_max = 2080 # s^-1
k_b_num = 1

cb_min = 0.5 # s^-1
cb_max = 0.5 # s^-1
cb_num = 1

cm_min = 0.5 # s^-1
cm_max = 0.5 # s^-1
cm_num = 1

ct_min = 0.5 # s^-1
ct_max = 0.5 # s^-1
ct_num = 1



T_min = 810.15 # K
T_max = 810.15 # K
T_num = 1

ls_range = np.linspace(ls_min, ls_max, num=ls_num)
lt_range = np.linspace(lt_min, lt_max, num=lt_num)
k_b_range = np.linspace(k_b_min, k_b_max, num=k_b_num)
T_range = np.linspace(T_min, T_max, num=T_num)
cb_range = np.linspace(cb_min, cb_max, num=cb_num)
cm_range = np.linspace(cm_min, cm_max, num=cm_num)
ct_range = np.linspace(ct_min, ct_max, num=ct_num)

runtime = 0

for permutation in [{"ls": ls,"lt": lt,"k_b": k_b, "T": T, "cb": cb, "cm": cm, "ct": ct} for ls in ls_range for lt in lt_range
                    for k_b in k_b_range for T in T_range for cb in cb_range for cm in cm_range for ct in ct_range]:
    cmd = ["srun"] if have_slurm else []
    cmd.extend([
        "./generate_stepping_data",
        "--Ls", str(permutation["ls"]),
        "--Lt", str(permutation["lt"]),
        "--k_b", str(permutation["k_b"]),
        "--cb", str(permutation["cb"]),
        "--cm", str(permutation["cm"]),
        "--ct", str(permutation["ct"]),
        "--T", str(permutation["T"]),
        "--runtime", str(runtime),
        "--movie"
    ])
    print "Running: ", ' '.join(cmd)

    basename = 'ls-%.3g,lt-%.3g,k_b-%s,cb-%s,cm-%s,ct-%s,T-%s' % (permutation['ls'], permutation['lt'], permutation["k_b"], permutation["cb"],
                                                                  permutation["cm"], permutation["ct"], permutation['T'])
    out = open('runlogs/' + basename + '.out', 'w')
    subprocess.Popen(cmd, stdout=out, stderr=subprocess.STDOUT)
