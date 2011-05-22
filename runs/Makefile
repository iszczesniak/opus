TARGETS = \
benes16.A benes16.S \
pionier.A pionier.S \
six_node_net.A six_node_net.S

all: $(TARGETS)

.SUFFIXES: .A .S .dot .run .tm

%.A: %.run %.tm %.dot ../opus
	time ./run_opus -A $<

%.S: %.run %.tm %.dot ../opus
	time ./run_opus -S $<

clean:
	rm -f $(TARGETS)
