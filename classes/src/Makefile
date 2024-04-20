include ../../Makefile.vars

ALLINC       = -I../include ${DWMINCS}
OBJS         = DwmMcCurtainASInfo.o \
               DwmMcCurtainASes.o \
               DwmMcCurtainAS2Ipv4NetDb.o \
               DwmMcCurtainIpv4Net2ASDb.o \
               DwmMcCurtainRipeAsnTxt.o
OBJDEPS      = $(OBJS:%.o=deps/%_deps)
OBJFILES     = $(OBJS:%.o=../obj/%.o)

all: ../lib/libDwmMcCurtain.a

../lib/libDwmMcCurtain.a: ${OBJFILES}
	ar rv $@ $^

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINC} -c $< -o $@

clean::
	rm -f ../lib/libDwmMcCurtain.a ${OBJFILES}

distclean:: clean
	rm -f deps/*_deps
