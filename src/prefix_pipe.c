
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

	if (prefix_pipe_set_nonblocking(fd[0]) < 0
	 	|| prefix_pipe_set_nonblocking(fd[1]) < 0
	 	|| prefix_pipe_set_closeonexec(fd[0]) < 0
	 	|| prefix_pipe_set_closeonexec(fd[1]) < 0)
	{
		close(fd[0]);
		close(fd[1]);
		fd[0] = fd[1] = -1;
		return ERROR;
	}

	return SUCCESS;
}

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
