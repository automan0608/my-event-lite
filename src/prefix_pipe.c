
#include <fcntl.h>
#include <unistd.h>

#include "prefix_core.h"

#include "prefix_log.h"
#include "prefix_pipe.h"

static int prefix_pipe_set_nonblocking(int fd);
static int prefix_pipe_set_closeonexec(int fd);

int prefix_pipe_init(int fd[2])
{
	if (0 != pipe(fd))
	{
		prefix_log("error", "pipe create error");
		return ERROR;
	}

	if ( /*prefix_pipe_set_nonblocking(fd[0]) < 0
	 	|| prefix_pipe_set_nonblocking(fd[1]) < 0
	 	|| */ prefix_pipe_set_closeonexec(fd[0]) < 0
	 	|| prefix_pipe_set_closeonexec(fd[1]) < 0)
	{
		close(fd[0]);
		close(fd[1]);
		fd[0] = fd[1] = -1;
		return ERROR;
	}

	return SUCCESS;
}

#if 0
static int prefix_pipe_set_nonblocking(int fd)
{
	if (0 >= fd)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		prefix_log("error", "set nonblocking error");
		return -ERROR;
	}
	return SUCCESS;
}
#endif

static int prefix_pipe_set_closeonexec(int fd)
{
	if (0 >= fd)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1)
	{
		prefix_log("error", "set closeonexec error");
		return ERROR;
	}
	return SUCCESS;
}

int prefix_pipe_write(int fd, void *buf, size_t n)
{
	prefix_log("debug", "in");

	if (0 >= fd || NULL == buf || 0 >= n)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	ssize_t result = 0;
	result = write(fd, buf, n);
	if (n != result)
	{
		prefix_log("error", "write pipe error");
		return ERROR;
	}
	prefix_log("debug", "write pipe success:%p:%2s", buf, (char *)buf);

	prefix_log("debug", "out");
	return SUCCESS;
}

int prefix_pipe_read(int fd, void *buf, size_t n)
{
	prefix_log("debug", "in");

	if (0 >= fd || NULL == buf || 0 >= n)
	{
		prefix_log("error", "parameter error");
		return ERROR;
	}

	ssize_t result = 0;
	result = read(fd, buf, n);
	if (n != result)
	{
		prefix_log("error", "read pipe error");
		return ERROR;
	}

	prefix_log("debug", "out");
	return SUCCESS;
}
