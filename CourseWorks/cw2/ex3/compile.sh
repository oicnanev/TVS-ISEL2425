#!/bin/bash

# Compile the shared library
gcc -shared -o mylib.so mylib.c -fPIC

# Compile the main program
gcc -o prog prog.c -ldl
