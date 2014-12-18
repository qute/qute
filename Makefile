
CFLAGS += -Ideps -Iinclude -Wall -std=c99
SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)
DEPS = $(wildcard deps/*/*.c)
DEP_OBJS = $(DEPS:.c=.o)
TESTS = $(wildcard test/*.c)
EXAMPLES = $(wildcard examples/*.c)

libqute.a: $(DEP_OBJS) $(OBJS)
	ar crus $@ $^

test: $(TESTS:.c=)
	$(foreach TEST, $^, ./$(TEST);)

examples: $(EXAMPLES:.c=)

test/ast: test/ast.o $(DEP_OBJS) $(OBJS)
test/lex: test/lex.o $(DEP_OBJS) $(OBJS)
test/parser: test/parser.o $(DEP_OBJS) $(OBJS)
test/simple: test/simple.o $(DEP_OBJS) $(OBJS)

examples/basic: examples/basic.o $(DEP_OBJS) $(OBJS)
examples/json: examples/json.o $(DEP_OBJS) $(OBJS)
examples/json.o: CFLAGS += -DTEST
examples/math: examples/math.o $(DEP_OBJS) $(OBJS)
examples/program: examples/program.o $(DEP_OBJS) $(OBJS)

clean:
	rm -f libqute.a
	rm -f $(OBJS) $(DEP_OBJS)
	rm -f $(TESTS:.c=.o) $(TESTS:.c=)
	rm -f $(EXAMPLES:.c=.o) $(EXAMPLES:.c=)

install: libqute.a
	install $^ $(PREFIX)/lib
	cp -rf include/* $(PREFIX)/include

uninstall:
	rm -f $(PREFIX)/lib/$(LIB)
	rm -rf $(PREFIX)/include/qute*

.PHONY: test examples clean install uninstall
