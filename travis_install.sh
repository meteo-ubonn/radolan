#!/bin/bash
# Copyright (c) 2016, radolan developers.
# Distributed under the MIT License. See LICENSE.txt for more info.

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    wget http://repo.continuum.io/miniconda/Miniconda3-latest-MacOSX-x86_64.sh \
        -O miniconda.sh
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh \
        -O miniconda.sh
fi

chmod +x miniconda.sh
bash miniconda.sh -b -p $HOME/miniconda
export PATH=$HOME/miniconda/bin:$PATH
conda update --yes conda
conda update --yes conda

# Create a testenv with the correct Python version
conda create -n radolan --yes pip python=3.7
source activate radolan

# add conda-forge channel
conda config --add channels conda-forge
conda config --set channel_priority strict

# Install radolan dependencies
conda install --yes boost<1.70.0 hdf5 netcdf-cxx4 cmake shapelib

if [[ "$TRAVIS_OS_NAME" == "osx" ]]
then
    # for Mac OSX
    export CC=clang
    export CXX=clang++
    export MACOSX_VERSION_MIN="10.7"
    export MACOSX_DEPLOYMENT_TARGET="${MACOSX_VERSION_MIN}"
    export CXXFLAGS="${CXXFLAGS} -mmacosx-version-min=${MACOSX_VERSION_MIN}"
    export CXXFLAGS="${CXXFLAGS} -stdlib=libc++ -std=c++11"
    export LDFLAGS="${LDFLAGS} -mmacosx-version-min=${MACOSX_VERSION_MIN}"
    export LDFLAGS="${LDFLAGS} -stdlib=libc++ -lc++"
    export LINKFLAGS="${LDFLAGS}"
elif [[ "$TRAVIS_OS_NAME" == "linux" ]]
then
    # for Linux
    export CC=gcc
    export CXX=g++
    export CXXFLAGS="${CXXFLAGS} -DBOOST_MATH_DISABLE_FLOAT128"
    export LDFLAGS="${LDFLAGS}"
    export LINKFLAGS="${LDFLAGS}"
else
    echo "This system is unsupported by the toolchain."
    exit 1
fi

export CFLAGS="${CFLAGS} -m64"
export CXXFLAGS="${CXXFLAGS} -m64"