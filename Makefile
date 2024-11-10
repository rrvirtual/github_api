
BIN_OBJ=src/github.o src/params.o
LIB_OBJS=src/lib/github.o src/lib/system.o src/lib/url.o src/lib/help.o 
OBJS=$(BIN_OBJ) $(LIB_OBJS) # Does a static build libs
TARGET=build/github

ifeq ($(DEBUG), 1)
DEFS=DEBUG
endif

LD_FLAGS?=-lcurl -lcjson

FLAGS=-fPIC -Wall -Werror --std=gnu99

DEFS+=_GNU_SOURCE

FLAGS+=$(CFLAGS)
EXTRA_FLAGS=$(addprefix -D,$(DEFS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(EXTRA_FLAGS) $(LD_FLAGS) -o $(TARGET)

%.o: %.c
	$(CC) -c -o $@ $(FLAGS) $(EXTRA_FLAGS) $(LD_FLAGS) $<

#run_test:
#	$(MAKE) -C test

clean:
	rm -f src/*.o $(TARGET) $(OBJS) *.bak *~
#	$(MAKE) clean -C test

