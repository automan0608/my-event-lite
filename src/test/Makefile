CFLAGS += -I../ -g
LDFLAGS += -L../
LDFLAGS += -lpthread -levent-lite
LDFLAGS += #-DDEBUG

target = test_event_base.out 				\
	     test_event_add.out 				\
	     test_bufferevent_add.out 			\
		 test_base.out 						\
		 test_min_heap.out 					\
		 test_event_time.out 				\
		 test_event_time_use_thread.out 	\
		 test_event_io_echo_serv_01.out 	\
		 test_event_io_echo_serv_02.out 	\
		 test_event_io_echo_serv_03.out 	\
		 test_event_io_echo_cli.out 		\
		 test_event_mixed_echo_cli.out      \
		 test_event_signal.out 	\
		 test_bufferevent_echo_cli.out

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
