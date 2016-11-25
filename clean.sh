#!/bin/sh

[ -e Makefile ] && make clean
rm -rf include
rm -rf build
rm -f Makefile
rm -f DoxyAPI
rm -f src/scclust_types.h
