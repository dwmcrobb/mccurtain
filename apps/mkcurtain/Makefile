include ../../Makefile.vars

mkcurtain: mkcurtain.o
	${CXX} ${LDFLAGS} -o $@ $< ${DWMLIBS}

mkcurtain.o: mkcurtain.cc
	${CXX} ${CXXFLAGS} ${DWMINCS} -c $<
