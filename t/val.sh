res() {
	printf '%-10s ' "$1"
	if [ $2 = $err ]; then
		echo FAIL
	else
		echo OK
	fi
}
err=1

echo AVERAGE
set -- foo bar foobar barfoo barfoobar foobarfoo foofoofoo foo bar
valgrind --error-exitcode=$err ./rene "$@" >/dev/null 2>&1
res normal $?
valgrind --error-exitcode=$err ./rene -l "$@" >/dev/null 2>&1
res last $?
valgrind --error-exitcode=$err ./rene -a "$@" >/dev/null 2>&1
res all $?

echo
echo SLASH
set -- foo bar foobar/ barfoo/ barfoobar/ foobarfoo/ foofoofoo/ foo/ bar/
valgrind --error-exitcode=$err ./rene "$@" >/dev/null 2>&1
res normal $?
valgrind --error-exitcode=$err ./rene -l "$@" >/dev/null 2>&1
res last $?
valgrind --error-exitcode=$err ./rene -a "$@" >/dev/null 2>&1
res all $?

echo
echo NOFROM
set -- foo bar bar abracadebra lol abcd
valgrind --error-exitcode=$err ./rene "$@" >/dev/null 2>&1
res normal $?
valgrind --error-exitcode=$err ./rene -l "$@" >/dev/null 2>&1
res last $?
valgrind --error-exitcode=$err ./rene -a "$@" >/dev/null 2>&1
res all $?
