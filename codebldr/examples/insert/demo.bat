@echo off
if not exist im.exe goto :error
if not exist console.exe goto :error
:run
        echo on
        console "* Starting demo ..." > demo.msg
        copy demo.dat demo.c
        rse icc -c demo.c > demo.msg
        im edlin < demo.msg > visit.bat
        visit
        echo off
        echo * Demo done.
        goto :done
:error
        echo * Please build the programs IM.EXE and CONSOLE.EXE first.
        echo * See the read-me file INSERT.RME for details.
:done
