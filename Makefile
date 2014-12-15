
SRC = $(wildcard src/*.c)
LIB ?= libqute.a
OBJS ?= $(SRC:.c=.o)

PREFIX ?= /usr/local

DEP_SRC ?= $(wildcard deps/*/*.c)

EXAMPLES_SRC ?= $(wildcard examples/*.c)
EXAMPLES ?= $(EXAMPLES_SRC:.c=)

TESTS_SRC ?= $(wildcard test/*.c)
TESTS ?= $(TESTS_SRC:.c=)

.PHONY: $(LIB)
$(LIB): $(OBJS)
	ar crus $(@) $(OBJS)

.PHONY: $(OBJS)
$(OBJS):
	$(CC) -Iinclude -c $(@:.o=.c) -o $(@)

clean:
	rm -f $(LIB) $(OBJS)
	rm -f $(TESTS)
	rm -f $(EXAMPLES)

install: uninstall $(LIB)
	install $(LIB) $(PREFIX)/lib
	cp -rf include/* $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(LIB)
	rm -rf $(PREFIX)/include/qute*

examples/: examples
examples: $(EXAMPLES)

test/: test
test: $(TESTS)

.PHONY: $(EXAMPLES)
$(EXAMPLES):
	$(CC) -Iinclude -Ideps $(LIB) $(@).c $(DEP_SRC) -o $(@)

.PHONY: $(TESTS)
$(TESTS):
	$(CC) -Iinclude -Ideps $(LIB) $(@).c $(DEP_SRC) -o $(@)
	./$(@)

