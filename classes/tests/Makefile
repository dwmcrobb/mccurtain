include ../../Makefile.vars

OBJFILES = TestIpv4Net2ASDb.o \
           TestAS2Ipv4NetDb.o
OBJDEPS  = $(OBJFILES:%.o=deps/%_deps)
TESTS    = $(OBJFILES:%.o=%)
ALLINCS  = -I../../classes/include ${DWMINCS}

all: ${TESTS}

runtests: all
	@ for tp in ${TESTS} ; do \
		printf "%-36s " $$tp ; \
		out=`./$$tp` ; \
		if [ $$? -eq 0 ]; then \
		  printf "%25s\n" "$$out" ; \
		else \
		  printf "\n%s" "$$out" ; \
		fi ; \
	done

#  dependency rule
deps/%_deps: %.cc 
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${ALLINCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

%.o: %.cc
	${CXX} ${CXXFLAGS} ${ALLINCS} -c $<

%: %.o ../lib/libDwmMcCurtain.a
	${CXX} ${LDFLAGS} -o $@ $^ ${DWMLIBS}

../lib/libDwmMcCurtain.a::
	${MAKE} -C ../src

clean::
	rm -f ${TESTS} ${OBJFILES}

distclean:: clean
	rm -f ${OBJDEPS}

