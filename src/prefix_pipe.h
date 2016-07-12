
#ifndef _PREFIX_PIPE_H_
#define _PREFIX_PIPE_H_

int prefix_pipe_init(int fd[2]);

int prefix_pipe_write(int fd, void *buf, size_t n);

int prefix_pipe_read(int fd, void *buf, size_t n);

#endif
