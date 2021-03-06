CC=clang
CXX=clang++
LD=clang++

RM=rm -f
CXXFLAGS=-std=c++17 -g -O0 -Wall -Werror -pedantic
LDFLAGS=-std=c++17
LDLIBS=

SRCS=main.cc backend.cc shard-assignment.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: backend

backend: $(OBJS)
	$(CXX) $(LDFLAGS) -o backend $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
