import os
from os import path
import numpy as np
from numpy.linalg import matrix_power
import matplotlib.pyplot as plt
from matplotlib import gridspec
from statistics import mean
from mpl_toolkits.mplot3d import Axes3D
import scipy.constants
import sys
sys.path.append("../data")
import importlib
import argparse
import subprocess
import bb_energy_distribution
from glob import glob


params = importlib.import_module("params")

parser = argparse.ArgumentParser()
parser.add_argument("-k", "--kb", type=float, help="Binding const", default=params.for_simulation['k_b'])
parser.add_argument("-s", "--ks", type=float, help="Sticky const", default=params.for_simulation['k_stk'])
args = parser.parse_args()

k_b = float(args.kb)        # Binding Rate Constant
k_stk = float(args.ks)      # Sticky Rate Constant


basepath = '../data/mc_data_{0:.2e}_{1:.2e}/'.format(k_b, k_stk)
datapath = '../data/compressed_mc_data/mc_data_file_{0:.2e}_{1:.2e}'.format(k_b, k_stk)
plottingdatapath = '../data/mc_plotting_data/mc_plotting_data_{0:.2e}_{1:.2e}'.format(k_b, k_stk)
leading_files = glob('{}/l_*.txt'.format(basepath))

initial_disp = []
final_disp_dict = {}
ob_time_dict = {}

# probability of being a leading step
P_leading_dict = {}
P_leading = []

for leading in leading_files:
    leading = leading[len(basepath):]
    leading_data = {'L': [], 't': []}
    trailing_data = {'L': [], 't': []}
    trailing = leading.replace('l_', 't_')
    first_ = leading.find('_',2)
    second_ = leading.find('_', first_+1)
    third_ = leading.find('_', second_+1)
    fourth_ = leading.find('_', third_+1)
    fifth_ = leading.find('_', fourth_+1)
    sixth_ = leading.find('_', fifth_+1)
    seventh_ = leading.find('_', sixth_+1)
    eigth_ = leading.find('_', seventh_+1)
    iL = float(leading[2:first_])

    if iL in initial_disp:
        print('woopsies, we have two files with the same L', iL, 'one of them is', leading)
        exit(1)
    N = leading[second_+1:third_]
    k_b = leading[third_+1:fourth_]
    dt = leading[fourth_+1:fifth_]
    cb = leading[fifth_+1:sixth_]
    cm = leading[sixth_+1:seventh_]
    ct = leading[seventh_+1:eigth_]
    C = leading[eigth_+1:leading.rfind('.')]
    leading_data['L'] = np.loadtxt(basepath+leading)[0]
    leading_data['t'] = np.loadtxt(basepath+leading)[1]
    try:
        trailing_data['L'] = np.loadtxt(basepath+trailing)[0]
        trailing_data['t'] = np.loadtxt(basepath+trailing)[1]
        initial_disp.append(iL)
        initial_disp.append(-iL)
        final_disp_dict[iL] = leading_data['L']
        final_disp_dict[-iL] = trailing_data['L']

        ob_time_dict[iL] = leading_data['t']
        ob_time_dict[-iL] = trailing_data['t']

        # calculate probability for step to be leading or trailing
        leading_data_length = len(leading_data['L'])
        trailing_data_length = len(trailing_data['L'])
        P_leading_dict[iL] = leading_data_length / (leading_data_length + trailing_data_length)
    except:
        if not path.exists(leading.replace('l', 't', 1)):
            print('unable to load trailing data for ', leading)

for key in sorted(P_leading_dict.keys()):
    P_leading.append(P_leading_dict[key])

# Probability of Leading and Trailing Steps Based on Data
P_leading = np.array(P_leading)
P_trailing = 1-P_leading
P_unbinding = {'leading': P_leading, 'trailing': P_trailing}

if len(P_leading) == 0:
    print("we have no data!!! :(")
    exit(1)

# make bin center the data point (for pcolor)
initial_disp = np.array(sorted(initial_disp)) # -50 to 50 array
final_bin_center = initial_disp*1.0 # set final_bin_center to initial_disp
final_bin_edges = np.zeros(len(final_bin_center)+1)
for i in range(1,len(final_bin_edges)-1):
    final_bin_edges[i] = (final_bin_center[i-1] + final_bin_center[i])*0.5
final_bin_edges[0] = 2*final_bin_center[0] - final_bin_edges[1]
final_bin_edges[-1] = 2*final_bin_center[-1] - final_bin_edges[-2]

# obtain meshgrid for pcolor
initial_disp_center, final_disp_center = np.meshgrid(initial_disp, final_bin_center)

final_disp_bin_width = final_bin_edges[1:] - final_bin_edges[:-1]    # a 1D array giving final displacement bin width


hist = np.zeros_like(initial_disp_center)
normalized_hist = np.zeros_like(initial_disp_center)    # Dimensions: 1/distance

for i_disp in initial_disp:
    i_disp_index = 0
    for i in range(1,len(final_bin_edges)):
        if i_disp < final_bin_edges[i]:
            i_disp_index = i-1
            break
    total_counts = len(final_disp_dict[i_disp])
    for f_disp in final_disp_dict[i_disp]:
        f_disp_index = None
        for i in range(1,len(final_bin_edges)):
            if f_disp < final_bin_edges[i]:
                f_disp_index = i-1
                break
        if f_disp_index is None or f_disp < final_bin_edges[0] or f_disp > final_bin_edges[-1]:
            # Data that is outside of range goes into the bin edges.
            if f_disp < final_bin_edges[0]:
                normalized_hist[0, i_disp_index] += 1/total_counts/final_disp_bin_width[0]
                hist[0, i_disp_index] += 1/total_counts
            if f_disp > final_bin_edges[-1]:
                normalized_hist[-1, i_disp_index] += 1/total_counts/final_disp_bin_width[-1]
                hist[-1, i_disp_index] += 1/total_counts
            continue
            # print("crazasges", f_disp, 'vs', final_bin_edges[0], 'and', final_bin_edges[-1])
            # Possibly think about making a infinite bin for final_L that goes outside plot

        else:
            # Collect unnormalized counts in hist for Transition Matrix
            hist[f_disp_index, i_disp_index] += 1/total_counts  # Dimensionless

            # Normalized by bin width (length)
            normalized_hist[f_disp_index, i_disp_index] += 1/total_counts/final_disp_bin_width[f_disp_index]     # Dimensions: 1/distance


np.savez_compressed(datapath,
                    final_disp_dict=final_disp_dict,
                    ob_time_dict=ob_time_dict,
                    P_unbinding=P_unbinding)
np.savez_compressed(plottingdatapath,
                    initial_disp=initial_disp,
                    hist=hist,
                    normalized_hist=normalized_hist,
                    P_unbinding=P_unbinding)