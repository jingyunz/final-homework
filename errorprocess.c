/*
 * (C) 2014 HuangJinDong
 *
 * src/errorprocess.c
 * 
 */

#include "errorprocess.h"
#include "globals.h"
#include <stdio.h>
#include <errno.h>

int   errno;             /* from errno.h to finger out the error */

void error_process()
{
	switch(errno)
	{
	case E2BIG:fprintf(stderr, "jysh: Argument list too long\n");
		   break;
	case EACCES:fprintf(stderr, "jysh: %s: Permission denied\n", commands);
		    break;
	case EBUSY:fprintf(stderr, "jysh: Device or resource busy\n");
		   break;
	case EEXIST:fprintf(stderr, "jysh: File exists\n");
		    break;
	case EFBIG:fprintf(stderr, "jysh: File too large\n");
		   break;
	case EINVAL:fprintf(stderr, "jysh: Invalid argument\n");
		    break;
	case EIO:fprintf(stderr, "jysh: Input/output error\n");
		 break;
	case EISDIR:fprintf(stderr, "jysh: %s: Is a dirextory\n", commands);
		    break;
	case ENODEV:fprintf(stderr, "jysh: No such device\n");
		    break;
	case ENOENT:fprintf(stderr, "jysh: %s: command not found\n", commands);
		    break;
	}

	/* resume */
	errno = 0;
}

