#!/bin/bash
function compile {
    # g++ -ggdb -g3 -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) -L$CONDA_PREFIX/lib ${1}.cpp -o ${1}$(python3-config --extension-suffix) ${2}
    g++ -O2 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) -L$CONDA_PREFIX/lib ${1}.cpp -o ${1}$(python3-config --extension-suffix) ${2}
}

export LD_LIBRARY_PATH=$CONDA_PREFIX/lib
compile sortings "-I$CONDA_PREFIX/include -lmkl_rt"
compile random_bits "-I$CONDA_PREFIX/include -lmkl_rt"
compile zp ""
python test.py
