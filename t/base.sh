#!/bin/sh

tests="$(find . -name '*.t')"

for t in $tests; do
	base=${t%%.t}
	printf '%-10s ' "$(basename $base)"
	sh $t >$base.out 2>$base.err
	cmp $base.out $base.exp >/dev/null 2>&1
	res=$?
	cmp $base.err $base.err.exp >/dev/null 2>&1
	eres=$?
	if [ "$res" = 0 ] && [ "$eres" = 0 ]; then
		echo OK
	else
		echo FAIL
	fi
done
