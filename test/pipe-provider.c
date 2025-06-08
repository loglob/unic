#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "interface.h"
#include "string.h"
#include "common.h"

PROVIDER((struct, Pipe), createPipe)


size_t format_struct_Pipe(char *to, size_t n, const struct Pipe thing[restrict static 1])
{
	(void)thing;
	return snprintf(to, n, "<pipe>");
}

static FILE *fdopen_or_die(int fd, const char *mode)
{
	FILE *f = fdopen(fd, mode);

	if(! f)
	{
		fprintf(stderr, "fdopen(): %s\n", strerror(errno));
		exit(1);
	}

	return f;
}

size_t createPipe(size_t cap, struct Pipe buf[restrict static cap])
{
	if(cap)
	{
		if(pipe(buf->fds))
		{
			fprintf(stderr, "pipe(): %s\n", strerror(errno));
			exit(1);
		}

		buf->out = fdopen_or_die(buf->fds[0], "r");
		buf->in = fdopen_or_die(buf->fds[1], "w");
	}

	return 1;
}
