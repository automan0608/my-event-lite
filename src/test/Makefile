CFLAGS += -I../
LDFLAGS += -L../ -levent-lite

target = test_event_base.out

all:
	$(MAKE) -C ../
	make test

test:$(target)
	@echo "make all test target success ! "

$(target):%.out:%.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

