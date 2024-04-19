include ../../Makefile.vars

ALLINCS = -I../../classes/include ${DWMINCS}

mkcurtain: mkcurtain.o ../../classes/lib/libDwmMcCurtain.a
	${CXX} ${LDFLAGS} -o $@ $^ ${DWMLIBS}

../../classes/lib/libDwmMcCurtain.a::
	${MAKE} -C ../../classes/src

mkcurtain.o: mkcurtain.cc
	${CXX} ${CXXFLAGS} ${ALLINCS} -c $<

clean::
	rm -f mkcurtain mkcurtain.o
