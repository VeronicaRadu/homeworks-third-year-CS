@echo off

set SRC=tema
set TESTS=tests
set OUT=out
set STATS=stats

if exist %OUT% (
    del %OUT%
)

if exist %STATS% (
    del %STATS%
)

python -tt %SRC%/tester.py -t test0 -o %OUT% -i 1
python -tt %SRC%/tester.py -f %TESTS%/test1 -o %OUT% -i 5

echo ""
echo "-----------------------------------------------------------------------"
echo ""

if exist %OUT% (
    type %OUT%
) else (
    echo "Tests failed"
)

pause
