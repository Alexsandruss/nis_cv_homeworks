#!/bin/bash
function compile {
    g++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) -L$CONDA_PREFIX/lib ${1}.cpp -o ${1}$(python3-config --extension-suffix) ${2}
}

compile sortings ""
compile random_bits "-I$CONDA_PREFIX/include -lmkl_rt"
python test.py
