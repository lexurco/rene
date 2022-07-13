#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int interactive = 0;
int noact = 0;
int nooverride = 0;
int replaceall = 0;
int replacelast = 0;
int verbose = 0;

void
usage(void)
{
	fprintf(stderr, "usage: rene [-ailnov] from to file ...\n");
	exit(EXIT_FAILURE);
}

char *rene;

void
err(const char *fmt, ...)
{
	fprintf(stderr, "%s: ", rene);
	if (fmt != NULL) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr, fmt, argp);
		va_end(argp);
	}
	fprintf(stderr, "\n");
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
	if (replacelast || replaceall) {
		char *temp = p;
		while (temp) {
			temp = strstr(temp+fromlen, from);
			count = (replaceall && temp) ? count + 1 : count;
			p = (replacelast && temp) ? temp : p;
		}
	}

	if (!(*new = malloc(strlen(s) + strlen(to)*count - fromlen*count + 1))) {
		err("%s: couldn't allocate memory", s);
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
		p = replaceall ? strstr(s, from) : strchr(s, '\0');
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
	if ((nooverride || interactive) && access(new, F_OK) == 0)
		yes = nooverride ? 0 : ask(f, new);
	if (yes && !noact)
		yes -= rename(f, new);
	if (verbose && yes)
		printf("%s -> %s\n", f, new);
	free(new);
}

int
main(int argc, char *argv[])
{
	char *from, *to;
	rene = argv[0];

	int c;
	while ((c = getopt(argc, argv, "ailnov")) != -1) {
		switch (c) {
		case 'a':
			replaceall = 1;
			break;
		case 'i':
			interactive = 1;
			break;
		case 'l':
			replacelast = 1;
			break;
		case 'n':
			noact = 1;
			break;
		case 'o':
			nooverride = 1;
			break;
		case 'v':
			verbose = 1;
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
