#!/usr/bin/env python
"""Run the py->rst conversion and run all examples.

Steps are:
    analyze example index file for example py filenames
    check for any filenames in example directory not included
    do py to rst conversion, writing into build directory
    run
"""
# -----------------------------------------------------------------------------
# Library imports
# -----------------------------------------------------------------------------

# Stdlib imports
import os
import os.path as osp
import sys
import shutil
import io
from subprocess import check_call
from glob import glob
from time import time

# Third-party imports

from ex2rst import *

# We must configure the mpl backend before making any further mpl imports
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib._pylab_helpers import Gcf

docdir = '../UseCaseDocumentation/'
torem = 'torem.txt'
open(torem, 'w').write('')
for fil in os.listdir(docdir):
    if osp.isfile(fil) and (fil.endswith(".py") or fil.endswith(".zip")):
        shutil.copyfile(osp.join(docdir, fil), fil)
        open(torem, 'a+').write(fil + '\n')

rootp = osp.abspath('..')
EG_INDEX_FNAME = 'examples_index.rst'

# Copy the py files; check they are in the examples list and warn if not
with io.open(EG_INDEX_FNAME, 'rt', encoding="utf8") as f:
    eg_index_contents = f.read()

# Add a script to valid_examples.txt if you want it to be built
flist_name = 'valid_examples.txt'
with io.open(flist_name, "r", encoding="utf8") as flist:
    validated_examples = flist.readlines()

# Parse "#" in lines
validated_examples = [line.split("#", 1)[0] for line in validated_examples]
# Remove leading and trailing white space from example names
validated_examples = [line.strip() for line in validated_examples]
# Remove blank lines
validated_examples = list(filter(None, validated_examples))

# Run the conversion from .py to rst file
for example in validated_examples:
    print(example)
    if not example.endswith(".py"):
        print("%s not a python file, skipping." % example)
        continue
    elif not osp.isfile(example):
        print("Cannot find file, %s, skipping." % example)
        continue
    file_root = example[:-3]
    if file_root not in eg_index_contents:
        msg = "Example, %s, not in index file %s."
        msg = msg % (example, EG_INDEX_FNAME)
        print(msg)
        continue
    exfile2rstfile(example, '.')

# added the path so that scripts can import other scripts on the same directory
sys.path.insert(0, os.getcwd())

if not osp.isdir('fig'):
    os.mkdir('fig')

use_xvfb = os.environ.get('TEST_WITH_XVFB', False)
use_memprof = os.environ.get('TEST_WITH_MEMPROF', False)

if use_xvfb:
    try:
        from xvfbwrapper import Xvfb
    except ImportError:
        raise RuntimeError("You are trying to run a documentation build",
                           "with 'TEST_WITH_XVFB' set to True, but ",
                           "xvfbwrapper is not available. Please install",
                           "xvfbwrapper and try again")

    display = Xvfb(width=1920, height=1080)
    display.start()

if use_memprof:
    try:
        import memory_profiler
    except ImportError:
        raise RuntimeError("You are trying to run a documentation build",
                           "with 'TEST_WITH_MEMPROF' set to True, but ",
                           "memory_profiler is not available. Please install",
                           "memory_profiler and try again")

name = ''


def run_script():
    namespace = {}
    t1 = time()
    with io.open(script, encoding="utf8") as f:
        exec(f.read(), namespace)
    t2 = time()
    print("That took %.2f seconds to run" % (t2 - t1))
    plt.close('all')
    del namespace


# Execute each python script in the directory:
for script in validated_examples:
    figure_basename = osp.join('fig', osp.splitext(script)[0])
    if use_memprof:
        print("memory profiling ", script)
        memory_profiler.profile(run_script)()

    else:
        print('*************************************************************')
        print(script)
        print('*************************************************************')
        run_script()

if use_xvfb:
    display.stop()

# clean up stray images, pickles, npy files, etc
for globber in ('*.nii.gz', '*.dpy', '*.npy', '*.pkl', '*.mat', '*.img',
                '*.hdr'):
    for fname in glob(globber):
        os.unlink(fname)