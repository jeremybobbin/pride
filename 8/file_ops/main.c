#include <sys/resource.h>
#include <sys/time.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <sys/fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include "fileops.h"

#define PERMS 0666

int _fillbuf(FILE *fp)
{
	int bufsize;
	if ((fp->flag&(_READ|_EOF|_ERR)) != _READ)
		return EOF;

	bufsize = (fp->flag & _UNBUF) ? 1 : BUFSIZ;
	if (fp->base == NULL) 	/* there is no buffer */
		if ((fp->base = (char *) malloc(bufsize)) == NULL)
			return EOF; /* can't alloc */

	fp->ptr = fp->base;
	fp->cnt = read(fp->fd, fp->ptr, bufsize);
	if (--fp->cnt < 0) {
		if (fp->cnt == -1)
			fp->flag |= _EOF;
		else
			fp->flag |= _ERR;

		fp->cnt = 0;
		return EOF;
	}
	return (unsigned char) *fp->ptr++;
}

FILE _iob[OPEN_MAX] = {
	{ 0, (char *) 0, (char *) 0, _READ,          0 },
	{ 0, (char *) 0, (char *) 0, _WRITE,         1 },
	{ 0, (char *) 0, (char *) 0, _WRITE | _UNBUF, 2 },
};

FILE *fopen(char *name, char *mode)
{
	int fd;
	FILE *fp;

	if (*mode != 'r' && *mode != 'w' && *mode != 'a')
		return NULL;
	for (fp = _iob; fp < _iob + OPEN_MAX; fp++)
		if ((fp-> flag & (_READ | _WRITE)) == 0)
			break;

	if (fp >= _iob + OPEN_MAX)
		return NULL;

	if (*mode == 'w')
		fd = creat(name, PERMS);
	else if (*mode == 'a') {
		if ((fd = open(name, O_WRONLY, 0)) == -1)
			fd = creat(name, PERMS);
		lseek(fd, 0L, 2);
	} else
		fd = open(name, O_RDONLY, 0);

	if (fd == -1)
		return NULL;
	fp->fd = fd;
	fp->cnt = 0;
	fp->base = NULL;
	fp->flag = (*mode == 'r') ? _READ : _WRITE;
	return fp;
}

double get_time()
{
    struct timeval t;
    struct timezone tzp;
    gettimeofday(&t, &tzp);
    return t.tv_sec + t.tv_usec*1e-6;
}

int cat_by_char(FILE *ifp)
{
	int c, n;
	while ((c = getc(ifp)) != EOF)
		n += c;
}

//#define MAX_ITER 1000
int main(int argc, char *argv[])
{
	int i;
	FILE *iop;
	for (i = 1; i < argc; i++)
		if ((iop = fopen(argv[i], "r")) != NULL)
			cat_by_char(iop);
		else exit(1);
	//t = get_time();
	//printf("Cat by char: %lf\n", get_time() - t);

	//for (i = 0; i < MAX_ITER; i++)
	//	isupper_fn(i);



}
