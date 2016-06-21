CFLAGS += -I../ -g
LDFLAGS += -L../ -levent-lite

target = test_event_base.out test_event_add.out

all:
	cd ../ && make
	make test

test:$(target)
	@echo "make all test target success ! "

$(target):%.out:%.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	cd ../ && make clean
	rm -rf $(target) *.o

.PHONY:all test $(target)
