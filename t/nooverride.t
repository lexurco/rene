tmp=/tmp/.rene.tes
mkdir -p $tmp
trap 'rm -rf $tmp' EXIT HUP INT
touch $tmp/bar
./rene -nvo foo bar $tmp/foo
