# The libraries for pythia, fastjet, and root ( "LIB_TRI" )
PY8PATH=/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.5
LIB_PYTH=-I${PY8PATH}/include -L${PY8PATH}/lib -lpythia8
LIB_FASTJET=`/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.5/bin/fastjet-config --cxxflags --libs`
LIB_ROOT=`root-config --cflags --glibs`
LIB_TRI=${LIB_PYTH} ${LIB_FASTJET} ${LIB_ROOT}

# compilation option
CC=g++
CFLAGS=-std=c++11 -O3 -Wno-deprecated

# some other test comment
bin/main: \
	obj/main.o \
	obj/BkgGen.o \
	obj/P8Gen.o  \
	obj/JetClusterer.o \
	obj/JetIndicesMatcher.o \
	obj/RhoMedianBkgClusterer.o
	${CC} ${CFLAGS} -o $@ $^ ${LIB_TRI}

bin/bkup: src/main_bkup.cc
	${CC} ${CFLAGS} -o $@ $^ ${LIB_TRI}

obj/main.o: src/main.cc src/BkgGen.h src/P8Gen.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@

obj/BkgGen.o: src/BkgGen.cc src/BkgGen.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@

obj/P8Gen.o: src/P8Gen.cc src/P8Gen.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@

obj/JetClusterer.o: src/JetClusterer.cc src/JetClusterer.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@

obj/JetIndicesMatcher.o: src/JetIndicesMatcher.cc src/JetIndicesMatcher.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@

obj/RhoMedianBkgClusterer.o: src/RhoMedianBkgClusterer.cc src/RhoMedianBkgClusterer.h
	${CC} ${CFLAGS} ${LIB_TRI} -c $< -o $@
