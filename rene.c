#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define O_INTERACTIVE 1
#define O_NOACT (1<<1)
#define O_NOOVERRIDE (1<<2)
#define O_REPLACEALL (1<<3)
#define O_REPLACELAST (1<<4)
#define O_VERBOSE (1<<5)

#define MAX_FILENAME 4096

uint8_t opts;
char new[MAX_FILENAME], *newe = new + MAX_FILENAME-1;

void
warn(const char *fmt, ...)
{
	char *w = strerror(errno);
	fputs("rene: ", stderr);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fprintf(stderr, ": %s\n", w);
}

void
err(int eval, const char *fmt, ...)
{
	char *w = strerror(errno);
	fputs("rene: ", stderr);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fprintf(stderr, ": %s\n", w);
	exit(eval);
}

void
errx(int eval, const char *fmt, ...)
{
	fputs("rene: ", stderr);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fputc('\n', stderr);
	exit(eval);
}

int
ren(char *from, char *to, char *f)
{
	int y = !(opts & O_NOOVERRIDE);

	char *p = strrchr(f, '/');
	if (p) {
		if (*++p == '\0')
			return 1;
	} else
		p = f;
	if (!(p = strstr(p, from)))
		return 1;
	int fromlen = strlen(from);
	if (opts & O_REPLACELAST)
		for(char *x; (x = strstr(p+fromlen, from)); p = x)
			;

	char *fp = f;
	char *newp = new;
	char *top = to;
	for (;;) {
		for (; fp != p; *newp++ = *fp++)
			if (newp == newe)
toolong:
				errx(1, "%s: final file name is too long", f);
		if (!*fp)
			break;
		for (top = to; *top != '\0'; *newp++ = *top++)
			if (newp == newe)
				goto toolong;
		fp += fromlen;
		if (!(opts & O_REPLACEALL) || !(p = strstr(fp, from)))
			p = strchr(fp, '\0');
	}
	*newp = '\0';

	if (opts & O_INTERACTIVE && access(new, F_OK) == 0) {
		fprintf(stderr, "replace %s with %s? ", from, to);
		y = getchar() == 'y';
	}
	if (y && !(opts & O_NOACT) && !(y += rename(f, new))) {
		warn("rename");
		return 0;
	}
	if (opts & O_VERBOSE && y)
		printf("%s -> %s\n", f, new);
	return 1;
}

void
usage(void)
{
	fputs("usage: rene [-ailnov] from to file ...\n", stderr);
	exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
	int c, ret = 0;
#ifdef __OpenBSD__
	if (pledge("stdio cpath rpath", NULL) == -1)
		err(1, "pledge");
#endif
	while ((c = getopt(argc, argv, "ailnov")) != -1) {
		switch (c) {
		case 'a':
			opts |= O_REPLACEALL;
			break;
		case 'i':
			opts |= O_INTERACTIVE;
			break;
		case 'l':
			opts |= O_REPLACELAST;
			break;
		case 'n':
			opts |= O_NOACT;
			break;
		case 'o':
			opts |= O_NOOVERRIDE;
			break;
		case 'v':
			opts |= O_VERBOSE;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 3 || !argv[0] || !argv[1] || !argv[0][0])
		usage();
	for (int i = 2; i < argc; i++)
		if (!ren(argv[0], argv[1], argv[i]))
			ret = 1;
	return ret;
}
