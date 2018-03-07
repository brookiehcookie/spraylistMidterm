@echo off
cls

echo Running test 1...
del MyTest01.exe
gcc SkipList.c MyTest01.c -o MyTest01
MyTest01 > MyOutput01.txt

echo Running test 2...
del MyTest02.exe
gcc SkipList.c MyTest02.c -o MyTest02
MyTest02

echo Running test 3...
del MyTest03.exe
gcc SkipList.c MyTest03.c -o MyTest03
MyTest03 > MyOutput03.txt

echo Tests complete
