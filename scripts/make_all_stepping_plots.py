#! /usr/bin/env python3

from __future__ import division
import numpy as np
import time, signal, sys, os, matplotlib, subprocess, re

if 'show' not in sys.argv:
    matplotlib.use('Agg')

import argparse
import datetime
import dynein.draw.cartoon as cartoon
import matplotlib.pyplot as plt
from matplotlib import gridspec
from matplotlib.patches import Rectangle

import dynein.data as datalib

import io

plt.rc('text', usetex=True)

EPSILON = 1e-7

def equal(f1, f2):
    return abs(f1-f2) < EPSILON

def get_stepping_data(args):
    data_files = []
    for fname in os.listdir(args.data_directory):
        if os.path.isfile(args.data_directory + "/" + fname):
            if (args.data_basename in fname and ".txt" in fname):
                if ("~" not in fname and "movie" not in fname and "config" not in fname):
                    data_files.append(args.data_directory + "/" + fname)

    if len(data_files) == 0:
        print("No files of form " + args.data_directory + "/*" + args.data_basename + "*.txt found. Exiting.")
        exit(1)

    data_objects = []

    for data_file in data_files:
        data_objects.append(datalib.SteppingData(data_file))

    stepping_data = {}

    stepping_data["onebound_times"] = np.concatenate([d.onebound_times for d in data_objects])
    stepping_data["bothbound_times"] = np.concatenate([d.bothbound_times for d in data_objects])
    stepping_data["step_times"] = stepping_data["onebound_times"] + stepping_data["bothbound_times"]
    stepping_data["step_lengths"] = np.concatenate([d.step_lengths for d in data_objects])
    stepping_data["initial_displacements"] = np.concatenate([d.initial_displacements for d in data_objects])

    stepping_data["alternating_passing"] = sum([d.alternating_passing for d in data_objects])
    stepping_data["alternating_not_passing"] = sum([d.alternating_not_passing for d in data_objects])
    stepping_data["not_alternating_passing"] = sum([d.not_alternating_passing for d in data_objects])
    stepping_data["not_alternating_not_passing"] = sum([d.not_alternating_not_passing for d in data_objects])

    stepping_data["leading_foot_steps"] = np.sum([d.leading_foot_steps for d in data_objects])
    stepping_data["trailing_foot_steps"] = np.sum([d.trailing_foot_steps for d in data_objects])

    stepping_data["num_steps"] = len(stepping_data["step_lengths"])
    stepping_data["initial_displacements"] = np.array(stepping_data["initial_displacements"])
    return stepping_data

def get_unbinding_probability_data(args):
    data_files = []
    for fname in os.listdir("data"):
        if os.path.isfile("data/" + fname):
            if ("paper_unbinding_probability__" in fname and ".txt" in fname):
                if ("~" not in fname):
                    data_files.append("data/" + fname)

    if len(data_files) == 0:
        print("No files of form data/*.txt found. Exiting.")
        exit(1)

    up_data = {}
    up_data["Ls"] = []
    up_data["mean_lagging_probability_per_L"] = []
    up_data["mean_leading_probability_per_L"] = []

    for data_file in data_files:
        data = np.loadtxt(data_file, dtype = np.float64)
        if len(data) == 15:
            continue
        start_L_idx = data_file.find('L-')+2
        end_L_idx = data_file[start_L_idx:].find(',') + start_L_idx
        L = float(data_file[start_L_idx:end_L_idx])

        times = data[:,0]
        near_unbinding_probabilities = data[:,1]
        far_unbinding_probabilities = data[:,2]

        near_binding_xs = data[:,5]
        far_binding_xs = data[:,9]

        lagging_unbinding_probabilities = np.array([far_unbinding_probabilities[s] if near_binding_xs[s] > far_binding_xs[s] else near_unbinding_probabilities[s] for s in range(len(times))])
        leading_unbinding_probabilities = np.array([near_unbinding_probabilities[s] if near_binding_xs[s] > far_binding_xs[s] else far_unbinding_probabilities[s] for s in range(len(times))])

        up_data["Ls"].append(L)
        up_data["mean_lagging_probability_per_L"].append(np.mean(lagging_unbinding_probabilities))
        up_data["mean_leading_probability_per_L"].append(np.mean(leading_unbinding_probabilities))

    up_data["mean_lagging_probability_per_L"] = np.array(up_data["mean_lagging_probability_per_L"])
    up_data["mean_leading_probability_per_L"] = np.array(up_data["mean_leading_probability_per_L"])
    return up_data

def get_force_data(args):
    data_files = []
    for fname in os.listdir("data"):
        if os.path.isfile("data/" + fname):
            if ("paper_force_stepping_data" in fname and ".txt" in fname):
                if ("~" not in fname):
                    data_files.append("data/" + fname)

    if len(data_files) == 0:
        print("No files of form data/stepping_data_paper_force__*.txt found. Exiting.")
        exit(1)

    force_data = {}
    force_data["forces"] = []
    force_data["velocities"] = []
    force_data["seeds"] = []

    for data_file in data_files:
        data = np.loadtxt(data_file, dtype = np.float64)
        if len(data) == 0 or len(data[1, :]) <= 5:
            continue
        start_F_idx = data_file.find('F-')+2
        end_F_idx = data_file[start_F_idx:].find(',') + start_F_idx
        F = float(data_file[start_F_idx:end_F_idx])

        start_s_idx = data_file.find(',s-')+3
        end_s_idx = data_file[start_s_idx:].find('.') + start_s_idx
        s = float(data_file[start_s_idx:end_s_idx])

        final_nbx = float(data[-1,2])
        final_fbx = float(data[-1,3])
        final_t   = float(data[-1,1])
        velocity = (final_nbx + final_fbx) / (2 * final_t)

        force_data["forces"].append([F])
        force_data["seeds"].append([s])
        force_data["velocities"].append([velocity])
    return force_data

def get_cli_arguments():
    parser = argparse.ArgumentParser(description = 'script to generate various histograms from stepping data.')

    parser.add_argument('-d', '--data-directory', dest = 'data_directory', action='store', type = str,
                        default="data/", help='data file directory', required = False)
    parser.add_argument('-b', '--data-basename', dest = 'data_basename', action='store', type = str,
                        default="", help='data file basename', required = True)
    parser.add_argument('-p', '--param-file', dest = 'parameters_filename', action='store', type = str,
                        default="", help='parameter filename (.tex)')
    parser.add_argument('-q', '--quick', dest='quick', action='store_true', default=False,
                        help='Only make the stepping length histogram')

    return parser.parse_args()

def make_behavior_plot(args, stepping_data, up_data):
    fig = plt.figure(figsize=(4,8))
    gs = gridspec.GridSpec(7, 2)
    ax0 = fig.add_subplot(gs[0:2, 0:2])
    ax1 = fig.add_subplot(gs[2:4, 0:2])
    ax2 = fig.add_subplot(gs[4, 0])
    ax3 = fig.add_subplot(gs[4, 1], sharey=ax2)
    ax4 = fig.add_subplot(gs[5:7, 0:2])

    data_files = []
    for fname in os.listdir(args.data_directory):
        if os.path.isfile(args.data_directory + "/" + fname):
            if (args.data_basename in fname and ".txt" in fname):
                if ("~" not in fname and "movie" not in fname and "config" not in fname):
                    data_files.append(args.data_directory + "/" + fname)

    if len(data_files) == 0:
        print("No files of form " + args.data_directory + "/*" + args.data_basename + "*.txt found. Exiting.")
        exit(1)

    # model position trace
    for i in [1, 2, 3, 4, 5, 6, 7, 8]:
        df = "data/paper_main_stepping_data-{}.txt".format(i)
        if df not in data_files:
            continue
        data = np.loadtxt(df, dtype = np.float64)
        bind_times = np.array(data[:,1])
        near_foot_positions = np.around(np.array(data[:,2]), decimals=12)
        far_foot_positions = np.around(np.array(data[:,3]), decimals=12)

        bind_times_duplicated = np.zeros(len(bind_times)*2-1)
        near_positions_duplicated = np.zeros(len(bind_times)*2-1)
        far_positions_duplicated = np.zeros(len(bind_times)*2-1)

        for t in range(1, len(bind_times)):
            bind_times_duplicated[2*t-1] = bind_times[t]
            bind_times_duplicated[2*t] = bind_times[t]
            near_positions_duplicated[2*t-1] = near_foot_positions[t-1]
            near_positions_duplicated[2*t] = near_foot_positions[t]
            far_positions_duplicated[2*t-1] = far_foot_positions[t-1]
            far_positions_duplicated[2*t] = far_foot_positions[t]

            bind_times_duplicated[0] = bind_times[0]
            near_positions_duplicated[0] = near_foot_positions[0]
            far_positions_duplicated[0] = far_foot_positions[0]

        ax0.plot(bind_times, near_foot_positions, 'o-', label="Model "+str(i), markersize=0, linewidth=0.5)

    yildiz_data = np.loadtxt("data/yildiz_tracking_data.txt", dtype = np.float64)
    ax0.plot(yildiz_data[:,0]/1e3, yildiz_data[:,1], 'o-', label="Yildiz dynein", markersize=0, linewidth=0.5)

    ax0.set_xlabel("time (s)")
    ax0.set_ylabel("Position (nm)")

    ax0.legend()

    ax0.spines["top"].set_visible(False)
    ax0.spines["right"].set_visible(False)

    #step length histogram
    yildiz_step_lengths = np.concatenate(([-37]*1, [-35]*1, [-34]*1, [-33]*2, [-31]*2, [-30]*3, [-29]*1, [-28]*1, [-27]*4, [-26]*4, [-25]*2, [-24]*3, [-23]*4, [-21]*4, [-20]*3,
                                          [-19]*3, [-18]*5, [-17]*3, [-16]*3, [-15]*7, [-14]*5, [-13]*7, [-12]*12, [-11]*16, [-10]*14, [-9]*20, [-8]*14, [-7]*10, [-6]*9, [-5]*11,
                                          [-4]*8, [-3]*2, [4]*6, [5]*7, [6]*12, [7]*20, [8]*19, [9]*22, [10]*30, [11]*34, [12]*26, [13]*21, [14]*23, [15]*22, [16]*30, [17]*29,
                                          [18]*23, [19]*22, [20]*26, [21]*12, [22]*21, [23]*16, [24]*7, [25]*8, [26]*7, [27]*8, [28]*5, [29]*9, [30]*7, [31]*8, [32]*6, [33]*2,
                                          [34]*2, [35]*9, [36]*4, [37]*9, [38]*5, [39]*1, [40]*2, [41]*1, [42]*3, [43]*2, [44]*4, [45]*4, [46]*1, [47]*1))

    if len(stepping_data["step_lengths"]) == 0:
        print("No steps to put in histogram!")

    bins = np.histogram(np.hstack((yildiz_step_lengths, stepping_data["step_lengths"])), bins=20)[1]

    ax1.hist(yildiz_step_lengths, bins, alpha=0.5, label="Yildiz 2012", normed=True, stacked=True)
    ax1.hist(stepping_data["step_lengths"], bins, alpha=0.5, label="Model", normed=True, stacked=True)

    ax1.scatter([np.mean(stepping_data["step_lengths"])], [0], label=r'$\overline{\Delta x} = ' + str(np.around(np.mean(stepping_data["step_lengths"]), decimals=2)) + r'$ \textit{nm}')

    ax1.legend(loc="upper right")
    ax1.set_xlabel("Step length (nm)")
    ax1.set_ylabel("Frequency")

    ax1.spines["top"].set_visible(False)
    ax1.spines["right"].set_visible(False)

    # OB time histogram
    if len(stepping_data["step_times"]) > 0:
        ax2.hist(stepping_data["onebound_times"], bins=np.logspace(np.log10(1e-9),np.log10(1e-3), 50))
    else:
        print("Error, no step_times")
        exit(1)

    ax2.set_ylabel("Frequency")
    ax2.set_xlabel("Onebound time (s)")
    ax2.set_xscale('log')

    ob_theory_avg = 5.2e-4
    ax2.axvline(ob_theory_avg, color='red', linestyle='dashed', linewidth=1)
    ax2.spines["top"].set_visible(False)
    ax2.spines["right"].set_visible(False)

    # BB (dwell) time histogram
    if len(stepping_data["step_times"]) > 0:
        ax3.hist(stepping_data["bothbound_times"], bins=np.logspace(np.log10(1e-6),np.log10(1e-0), 50))

    bb_theory_avg = 6.4e-2
    ax3.axvline(bb_theory_avg, color='red', linestyle='dashed', linewidth=1)
    plt.setp([ax3.get_yticklabels()], visible=False)
    ax3.tick_params(axis='y', which="both", left=False, right=False, labelbottom=False)
    ax3.set_xlabel("Bothbound time (s)")
    ax3.set_xscale('log')

    ax3.spines["top"].set_visible(False)
    ax3.spines["right"].set_visible(False)

    # unbinding probability vs L plot
    yildiz_displacements = [10, 20, 30, 40, 50]
    yildiz_lagging_fractions = [0.525, 0.545, 0.61, 0.59, 0.67]
    yildiz_lagging_uncertainty = [0.06, 0.04, 0.035, 0.045, 0.075]

    ax4.errorbar(yildiz_displacements, yildiz_lagging_fractions, yerr=yildiz_lagging_uncertainty, label="Yildiz 2012", fmt='o-', c='b', markersize=4, linestyle='', capsize=1, elinewidth=0.3, markeredgewidth=0.3)

    ax4.scatter(up_data["Ls"], up_data["mean_lagging_probability_per_L"] / (up_data["mean_lagging_probability_per_L"] + up_data["mean_leading_probability_per_L"]), c='r', label="Model", zorder=2, s=4**2)
    ax4.set_xlabel("Binding domain separation (nm)")
    ax4.set_ylabel("P(lagging step)")
    ax4.legend()

    ax4.spines["top"].set_visible(False)
    ax4.spines["right"].set_visible(False)

    plt.tight_layout(w_pad=2, h_pad=0.5)
    plt.savefig("plots/model_behavior.pdf", format="pdf")

def make_analysis_plot(args, stepping_data):
    fig = plt.figure(figsize=(8*.6, 6*.6), dpi=300)
    plt.rc('text', usetex=True)

    gs = gridspec.GridSpec(1, 3, width_ratios=[1, 1, 1])
    ax1 = fig.add_subplot(gs[0])
    ax2 = fig.add_subplot(gs[1])
    ax3 = fig.add_subplot(gs[2])

    width = 0.5

    ax1.bar([0, 1], [stepping_data["alternating_passing"] + stepping_data["alternating_not_passing"], stepping_data["not_alternating_passing"] + stepping_data["not_alternating_not_passing"],], width)
    ax1.set_ylabel('frequency')
    ax1.set_xticks([0, 1])
    ax1.set_xticklabels(('alternating', 'not\nalternating'), rotation=45)

    ax2.bar([0, 1], [stepping_data["alternating_passing"] + stepping_data["not_alternating_passing"], stepping_data["alternating_not_passing"] + stepping_data["not_alternating_not_passing"],], width)
    ax2.set_ylabel('frequency')
    ax2.set_xticks([0, 1])
    ax2.set_xticklabels(('passing', 'not passing'), rotation=45)

    ax3.bar([0, 1, 2, 3], [stepping_data["alternating_passing"], stepping_data["alternating_not_passing"], stepping_data["not_alternating_passing"], stepping_data["not_alternating_not_passing"],], width)
    ax3.set_ylabel('frequency')
    ax3.set_xticks([0, 1, 2, 3])
    ax3.set_xticklabels(('alternating,\n passing', 'alternating,\n not passing', 'not alternating,\n passing', 'not alternating,\n not passing'), rotation=45)

    plt.tight_layout()
    plt.gca().spines["top"].set_visible(False)
    plt.gca().spines["right"].set_visible(False)
    plt.savefig("plots/stepping_analysis.pdf", format="pdf")
    plt.close(fig)

def make_force_plot(args, force_data):
    fig = plt.figure(figsize=(4, 2.2), dpi=300)
    gennerich_forces = [4, 0, -1, -2, -3, -4, -5, -6, -7, -8, -10]
    gennerich_velocities = [50.76, 45.38, 43.07, 36.92, 22.3, 8.46, 4.6, 3.07, -.6, -5.38, -15.38]
    gennerich_errors = [2.3, 1.8, 3, 3, 1.6, 1.4, 1.5, 1.5, 1.4, 1.4, 2.3]
    plt.errorbar(gennerich_forces, gennerich_velocities, yerr=gennerich_errors, label="Gennerich 2007", fmt='o-', c='b', markersize=4, linestyle='', capsize=1, elinewidth=0.3, markeredgewidth=0.3)
    plt.scatter(force_data["forces"], force_data["velocities"], c='r', label="Model", zorder=2, s=4**2)
    plt.xlabel("$\hat{x}$ Force (pN)")
    plt.ylabel("Velocity (nm/s)")
    plt.legend()
    plt.tight_layout()
    plt.savefig("plots/force_vs_velocity.pdf", format="pdf")

def initialize():
    plt.rcParams.update({'font.size': 8})

def main():
    initialize()
    args = get_cli_arguments()
    stepping_data = get_stepping_data(args)
    unbinding_probability_data = get_unbinding_probability_data(args)
    force_data = get_force_data(args)
    make_behavior_plot(args, stepping_data, unbinding_probability_data)
    if args.quick:
        exit()
    make_analysis_plot(args, stepping_data)
    force_data = get_force_data(args)
    make_force_plot(args, force_data)

if __name__ == "__main__":
    main()

# #step time histogram
# gs = gridspec.GridSpec(4, 1, height_ratios=[1, 1, 1, 1])
# ax0 = fig.add_subplot(gs[0])
# ax1 = fig.add_subplot(gs[1])
# ax2 = fig.add_subplot(gs[2])
# ax3 = fig.add_subplot(gs[3])

# if len(step_times) > 0:
#     ax0.hist(step_times, bins=np.logspace(np.log10(1e-10),np.log10(1e-2), 50))
#     ax1.hist(onebound_times, bins=np.logspace(np.log10(1e-10),np.log10(1e-2), 50))
#     ax2.hist(bothbound_times, bins=np.logspace(np.log10(1e-10),np.log10(1e-2), 50))
#     # ax3.hist(run_velocities, bins=50)

# ax0.set_ylabel("Frequency")
# ax0.set_xscale('log')
# # ax0.ticklabel_format(style='sci', axis='x', scilimits=(0,0))

# ax1.set_ylabel("Frequency")
# ax1.set_xscale('log')
# # ax1.ticklabel_format(style='sci', axis='x', scilimits=(0,0))

# ax2.set_xlabel("Step time (s)")
# ax2.set_ylabel("Frequency")
# ax2.set_xscale('log')
# # ax2.ticklabel_format(style='sci', axis='x', scilimits=(0,0))

# ax3.set_xlabel("velocity (nm/s)")
# ax3.set_ylabel("Frequency")

# plt.subplots_adjust(hspace=0.6)

# plt.show()
# plt.gca().spines["top"].set_visible(False)
# plt.gca().spines["right"].set_visible(False)
# # plt.tight_layout()
# plt.savefig("plots/stepping_time_histogram.pdf", format="pdf")
# plt.close(fig)

# # OB_time vs step_length scatter
# assert len(onebound_times) == len(step_lengths)
# fig = plt.figure()
# plt.scatter(onebound_times, step_lengths, alpha=0.1)
# plt.gca().set_xscale('log')
# plt.xlabel("Onebound time (s)")
# plt.ylabel("Step length (nm)")

# plt.gca().set_xlim((1e-7, 1e-2))

# plt.tight_layout()
# plt.gca().spines["top"].set_visible(False)
# plt.gca().spines["right"].set_visible(False)
# plt.savefig("plots/ob-vs-length-scatter.pdf", format="pdf")
# plt.close(fig)

# # BB_time vs step_length scatter
# assert len(bothbound_times) == len(step_lengths)
# fig = plt.figure()
# plt.scatter(bothbound_times, step_lengths, alpha=0.1)
# plt.gca().set_xscale('log')
# plt.xlabel("Bothbound time (s), theory= 5e-3s")
# plt.ylabel("Step length (nm)")

# plt.gca().set_xlim((1e-5, 1))

# plt.gca().spines["top"].set_visible(False)
# plt.gca().spines["right"].set_visible(False)
# plt.tight_layout()
# plt.savefig("plots/bb-vs-length-scatter.pdf", format="pdf")
# plt.close(fig)

# # initial displacement vs motor step length scatter
# fig = plt.figure()
# plt.plot(initial_displacements, initial_displacements+step_lengths, '.', alpha=0.3)
# plt.xlabel("Initial displacement (nm)")
# plt.ylabel("Step length (nm)")
# plt.ylabel("Final displacement (nm)")
# plt.axes().set_aspect('equal')

# plt.gca().spines["top"].set_visible(False)
# plt.gca().spines["right"].set_visible(False)
# plt.tight_layout()
# plt.savefig("plots/displacement_vs_step_length.pdf", format="pdf")
# plt.close(fig)


# fig = plt.figure()

# initial_displacements = np.array(initial_displacements)
# indices = np.argsort(np.abs(initial_displacements))
# sorted_displacements = initial_displacements[indices]

# Nbin = 50
# L = np.zeros(int((len(sorted_displacements)-1)/Nbin)+1)
# ntrailing = np.zeros_like(L)
# nleading = np.zeros_like(L)
# for i in range(len(L)):
#     bunch = sorted_displacements[i*Nbin:(i+1)*Nbin]
#     ntrailing[i] = (bunch < 0).sum()
#     nleading[i] = (bunch > 0).sum()
#     L[i] = np.abs(bunch).mean()

# fraction_trailing = ntrailing / (ntrailing + nleading)

# yildiz_displacements = [10, 20, 30, 40, 50]
# yildiz_fractions = [0.525, 0.545, 0.61, 0.59, 0.67]
# yildiz_uncertainty = [0.06, 0.04, 0.035, 0.045, 0.075]

# plt.plot(L, fraction_trailing, 'o-', label="Model")
# plt.errorbar(yildiz_displacements, yildiz_fractions, yerr=yildiz_uncertainty, label="Experimental (Yildiz 2012)", fmt='o-',)
# plt.ylim(0,1)

# plt.xlabel("FIXME Initial foot x-displacement (unstepping - stepping) (nm)")
# plt.ylabel("Fraction trailing")

# plt.ylim([0,1.1])

# plt.legend()

# plt.gca().spines["top"].set_visible(False)
# plt.gca().spines["right"].set_visible(False)
# plt.tight_layout()
# plt.savefig("plots/displacement_histogram.pdf", format="pdf")
# plt.close(fig)
