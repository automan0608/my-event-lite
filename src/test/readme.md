
## These are all tests of the event-lite
    to run test, should "export LD_LIBRARY_PATH=`pwd`/.." or run env.sh


### test_event_io_echo_serv_01.c
	a echo server which uses iterate module, it can only serve 1 client at the same time

### test_event_io_echo_serv_02.c
	a echo server which uses non-blocking module, it can serve multiple clients at the same time

### test_event_io_echo_cli.c
	a echo client just like the telnet client, it should be used along with echo server.
	use ctrl+D to exit

### test_event_time_io_echo_cli.c
	almost the same with test_event_io_echo_cli.c, it add a time event as an extra.

### test_event_time.c
	test the mix of several different types of time events.

### test_min_heap.c
	test of min heap
