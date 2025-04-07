#include <stddef.h>
#include <stdio.h>
#include "interface.h"
#include "string.h"
#include "common.h"
#include <libexplain/pipe.h>
#include <libexplain/fdopen.h>
#include <libexplain/fread.h>

PROVIDER((struct, Pipe), createPipe)


size_t format_struct_Pipe(char *to, size_t n, const struct Pipe thing[restrict static 1])
{
	(void)thing;
	return snprintf(to, n, "<pipe>");
}

size_t createPipe(size_t cap, struct Pipe buf[restrict static cap])
{
	if(cap)
	{
		explain_pipe_or_die(buf->fds);
		buf->out = explain_fdopen_or_die(buf->fds[0], "r");
		buf->in = explain_fdopen_or_die(buf->fds[1], "w");
	}

	return 1;
}
