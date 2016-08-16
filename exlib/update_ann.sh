#!/bin/sh

rm -r -f libANN/*

if [ ! -f ann_1.1.2.zip ]; then
    echo "File not found!"
    wget https://www.cs.umd.edu/~mount/ANN/Files/1.1.2/ann_1.1.2.zip
fi

unzip -q ann_1.1.2.zip
cp -r ann_1.1.2/Copyright.txt ann_1.1.2/include ann_1.1.2/License.txt ann_1.1.2/ReadMe.txt ann_1.1.2/src libANN
rm -r ann_1.1.2

# Use static linkage
sed -i -e '62,84s\^\//\' -e '86s\^\//\' libANN/include/ANN/ANN.h

cat <<EOF > libANN/Makefile
all: library

library:
	mkdir -p lib/
	cd src; \$(MAKE) targets

clean:
	\$(RM) -R lib/
	cd src; \$(MAKE) clean
EOF

cat <<EOF > libANN/Make-config
ANNLIB = libANN.a
C++ = \$(CXX)
CFLAGS = -O2
MAKELIB = \$(AR) rcs
RANLIB = true
EOF
