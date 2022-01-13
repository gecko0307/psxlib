@echo off
echo -- Setting SDK variables...
set PATH=C:\psyq\bin;
set PSX_PATH=C:\psyq\bin
set LIBRARY_PATH=C:\psyq\lib
set C_PLUS_INCLUDE_PATH=C:\psyq\include
set C_INCLUDE_PATH=C:\psyq\include
set PSYQ_PATH=C:\psyq\bin
set COMPILER_PATH=C:\psyq\bin
set GO32=DPMISTACK 1000000 
set G032TMP=C:\WINDOWS\TEMP
set TMPDIR=C:\WINDOWS\TEMP

echo -- Building CPE...
ccpsx -Xo$80010000 -O0 -I..\src ..\src\sys.s ..\src\pad.c ..\src\gpu.c src\cd.c src\main.c -ocpe\main.cpe

echo -- Converting to PSX EXE...
cpe2psx cpe/main.cpe iso/PSX.EXE

echo -- Building ISO...
mkpsxiso -y iso.xml
