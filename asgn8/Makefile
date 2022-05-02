CC=c++
CFLAGS=-g -O0 -std=c++14
OFILES=Simulator.o main.o buffer.o server.o event.o generator.o samples.o

all: sim

sim: $(OFILES)
	$(CC) $(OFILES) $(CFLAGS) -o $@

%.o: %.cpp 
	$(CC) -c $(CFLAGS) $<

# This rules is to automatically re-generate a dependency file to
# make sure that make will recompile a given object file if one of its
# headers changes.

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	rm -rf *.o *~ sim

# This imports the dependency header specs.

include $(OFILES:.o=.d)
