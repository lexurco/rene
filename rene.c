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

uint8_t opts;

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

int
strrep(char *from, char *to, char *s, char **new)
{
	char *p = strrchr(s, '/');
	if (p) {
		if (*++p == '\0')
			return 0;
	} else
		p = s;
	p = strstr(p, from);
	if (!p)
		return 0;
	int fromlen = strlen(from);
	int count = 1;
	if (opts & O_REPLACELAST || opts & O_REPLACEALL) {
		char *temp = p;
		while (temp) {
			temp = strstr(temp+fromlen, from);
			count = (opts & O_REPLACEALL && temp) ? count + 1 :
			    count;
			p = (opts & O_REPLACELAST && temp) ? temp : p;
		}
	}

	if (!(*new =
	    malloc(strlen(s) + strlen(to)*count - fromlen*count + 1))) {
		warn("malloc");
		return 0;
	}

	char *newp = *new;
	char *top = to;
	while (p) {
		for (; s != p; *newp++ = *s++)
			;
		for (; *top != '\0'; *newp++ = *top++)
			;
		s += fromlen;
		p = opts & O_REPLACEALL ? strstr(s, from) : strchr(s, '\0');
		p = p ? p : strchr(s, '\0');
		for (; s != p; *newp++ = *s++)
			;
		p = (*p == '\0') ? NULL : p;
		top = to;
	}
	*newp = '\0';
	return 1;
}

int
ask(char *from, char *to)
{
	fprintf(stderr, "replace %s with %s? ", from, to);
	return getchar() == 'y';
}

void
ren(char *from, char *to, char *f)
{
	int yes = 1;
	char *new = NULL;
	if (!strrep(from, to, f, &new))
		return;
	if ((opts & O_NOOVERRIDE || opts & O_INTERACTIVE) &&
	    access(new, F_OK) == 0)
		yes = opts & O_NOOVERRIDE ? 0 : ask(f, new);
	if (yes && !(opts & O_NOACT) && !(yes += rename(f, new)))
		warn("rename");
	if (opts & O_VERBOSE && yes)
		printf("%s -> %s\n", f, new);
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
	char *from, *to;

#ifdef __OpenBSD__
	if (pledge("stdio cpath rpath", NULL) == -1)
		err(1, "pledge");
#endif

	int c;
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

	if (argc < 3)
		usage();
	from = argv[0];
	to = argv[1];
	for (int i = 2; i < argc; i++)
		ren(from, to, argv[i]);
	return 0;
}
