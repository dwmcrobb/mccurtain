include ../../Makefile.vars

ALLINCS  = -I../../classes/include ${DWMINCS}
OBJS     = mccdb.o
OBJDEPS      = $(OBJS:%.o=deps/%_deps)
TARTARGETS   = ${TARDIR}/bin/mccdb \
               ${TARDIR}/man/man1/mccdb.1

mccdb: mccdb.o ../../classes/lib/libDwmMcCurtain.a
	${CXX} ${LDFLAGS} -o $@ $^ ${DWMLIBS}

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINCS} $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINCS} -c $< -o $@

../../classes/lib/libDwmMcCurtain.a::
	${MAKE} -C ../../classes/src

tarprep: ${TARTARGETS}

${TARDIR}/bin/mccdb: mccdb
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

${TARDIR}/man/man1/mccdb.1: mccdb.1
	${LIBTOOL} --mode=install ../../install-sh -c -m 644 $< $@

distclean:: clean
	rm -f ${OBJDEPS}

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f mccdb ${OBJS}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARTARGETS}
