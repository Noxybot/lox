#!/bin/bash

mkdir build

cmake -B build -G "Ninja"

cmake --build build
