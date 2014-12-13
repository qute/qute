
SRC = $(wildcard *.c)
LIB ?= libqute.a
OBJS ?= $(SRC:.c=.o)
TESTS ?= $(wildcard test/*.c)

DEP_SRC ?= $(wildcard deps/*/*.c)

.PHONY: $(LIB)
$(LIB): $(OBJS)
	ar crus $(@) $(OBJS)

.PHONY: $(OBJS)
$(OBJS):
	$(CC) -c $(@:.o=.c) -o $(@)

clean:
	rm -f $(LIB) $(OBJS)
	rm -f $(TESTS:.c=)

test: test/simple

test/simple: $(LIB)
	$(CC) -I. -Ideps $(LIB) $(@).c $(DEP_SRC) -o $(@)
	./$(@)
