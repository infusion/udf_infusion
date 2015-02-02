import numpy as np
from numpy import ma
import scipy as sp


def load_table(filename):
    data = np.loadtxt(filename)
    return ma.masked_equal(data, 1e+20)


def example_table():
    if example_table.data is None:
        example_table.data = load_table('example_table')
    return example_table.data
example_table.data = None
