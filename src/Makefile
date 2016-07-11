REACTOR := select
CFLAGS += -shared -fPIC -Wall -g
LDFLAGS += #-DDEBUG

obj = prefix_event_base.o prefix_event.o prefix_pipe.o prefix_min_heap.o \
	prefix_base.o prefix_log.o prefix_event_signal.o prefix_bufferevent.o
target = libevent-lite.so

ifeq ("$(REACTOR)","select")
reactor := prefix_event_op_select.o
LDFLAGS += -D_USE_SELECT
endif

all:$(obj) $(reactor)
	$(CC) -o $(target) $^ $(CFLAGS) $(LDFLAGS)
	@echo "make all objs success"

$(obj):%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

$(reactor):%.o:%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(target)

.PHONY:all clean
