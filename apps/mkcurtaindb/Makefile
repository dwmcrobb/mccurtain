include ../../Makefile.vars

ALLINCS = -I../../classes/include ${DWMINCS}
OBJS         = mkcurtaindb.o
OBJDEPS      = $(OBJS:%.o=deps/%_deps)

mkcurtaindb: mkcurtaindb.o ../../classes/lib/libDwmMcCurtain.a
	${CXX} ${LDFLAGS} -o $@ $^ ${DWMLIBS}

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINCS} -c $< | \
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

clean::
	rm -f mkcurtaindb ${OBJS}

distclean:: clean
	rm -f ${OBJDEPS}
