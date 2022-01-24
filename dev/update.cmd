@echo off

rem build
call grunt.cmd

rem copy to local arduino libraries folder
copy ..\HTTPClient32\*.* C:\Users\Jumangee\Documents\Arduino\libraries\HTTPClient32 /y