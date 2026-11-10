BIN_OBJ=test.o
LIB_OBJS=test_url.o test_system.o test_github.o
LIB_OBJS+=../src/lib/github.o ../src/lib/system.o ../src/lib/url.o
OBJS=$(BIN_OBJ) $(LIB_OBJS) # Does a static build libs
TARGET=run_test

LD_FLAGS?=-lcurl -lcjson -lcunit

FLAGS=-fPIC -Wall -Werror --std=gnu99

DEFS+=_GNU_SOURCE

FLAGS+=$(CFLAGS)
EXTRA_FLAGS=$(addprefix -D,$(DEFS))

all: $(TARGET)

test: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(FLAGS) $(EXTRA_FLAGS) $(LD_FLAGS) -o $(TARGET)

%.o: %.c
	$(CC) -c -o $@ $(FLAGS) $(EXTRA_FLAGS) $(LD_FLAGS) $<

clean:
	rm -f $(OBJS) *.bak *~

fclean: clean
	rm -f $(TARGET)

.PHONY: all test clean fclean