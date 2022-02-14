echo off

rem build
call build.cmd

rem copy to local arduino libraries folder
del C:\Users\Jumangee\Documents\Arduino\libraries\HTTPClient32\*.* /y
copy ..\HTTPClient32\*.* C:\Users\Jumangee\Documents\Arduino\libraries\HTTPClient32\ /y

del e:\Projects\!DIY\ESP32\TelegramBot\lib\HTTPClient32\*.* /y
copy ..\HTTPClient32\*.* e:\Projects\!DIY\ESP32\TelegramBot\lib\HTTPClient32\ /y

del e:\Projects\!DIY\ESP32\SecurGramCam32\lib\HTTPClient32\*.* /y
copy ..\HTTPClient32\*.* e:\Projects\!DIY\ESP32\SecurGramCam32\lib\HTTPClient32\ /y
