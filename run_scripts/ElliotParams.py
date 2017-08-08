#!/usr/bin/python
import simrunner
import numpy as np

l = "Elliott-Parameters"

simrunner.run_sim(**{"k_b": 10e18, "k_ub": 10e20, "cb": 2.0, "cm": 2.0, "ct": 1.0, "dt": 1e-10, "label": l, "constant-write": True, "runtime": 1e-4, })