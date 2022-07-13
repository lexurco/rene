dir="$(dirname $0)"

echo ---- BASE ----
sh "$dir"/base.sh

echo
echo -- VALGRIND --
sh "$dir"/val.sh
