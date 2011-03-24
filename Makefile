TARGETS = compare netgen opus show tragen
TARGET_OBJS = $(addsuffix .o, $(TARGETS))

OBJS = analysis.o arguments.o arr_queue.o compare_args.o distro.o	\
edge_probs.o generate.o geometric.o graph.o graph_serialization.o	\
netgen_args.o nodistro.o packet.o poisson.o rand.o rou_order.o		\
show_args.o serializer.o simulation.o tabdistro.o test.o tragen_args.o	\
utils.o utils_ana.o utils_netgen.o utils_tragen.o utils_sim.o

#CXXFLAGS = -g -Wno-deprecated
CXXFLAGS = -O3 -Wno-deprecated

# Boost
#BOOSTPATH := /usr/local/boost-1.43
#CXXFLAGS := $(CXXFLAGS) -I $(BOOSTPATH)/include
#LDFLAGS := $(LDFLAGS) -L $(BOOSTPATH)/lib
LDFLAGS := $(LDFLAGS) -l boost_filesystem-mt
LDFLAGS := $(LDFLAGS) -l boost_graph-mt
LDFLAGS := $(LDFLAGS) -l boost_program_options-mt
LDFLAGS := $(LDFLAGS) -l boost_regex-mt
LDFLAGS := $(LDFLAGS) -l boost_serialization-mt

# GNU Scientific Library
LDFLAGS := $(LDFLAGS) -l gslcblas -l gsl

all: $(TARGETS)

compare: $(OBJS)

netgen: $(OBJS)

opus: $(OBJS)

show: $(OBJS)

tragen: $(OBJS)

.PHONY: clean count depend test

clean:
	rm -f *~
	rm -f $(OBJS)
	rm -f $(TARGET_OBJS)
	rm -f $(TARGETS)

count:
	wc -l *.hpp *.cc

depend:
	g++ -MM *.cc > dependencies

test: $(OBJS)
	cd test; make

include dependencies
