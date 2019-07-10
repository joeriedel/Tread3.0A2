del /S /Q /F *.aps
del /S /Q /F *.SCC
del /S /Q /F *.ini
del /S /Q /F *.ncb
del /S /Q /F /AH *.suo
del /S /Q /F *.user
del /S /Q /F *.vspscc
del /S /Q /F *.vssscc
del logfile.txt
del /S /Q /F base\plugins\*.*
rd base\plugins
del /Q /F debug\*.*
rd debug
del /Q /F release\*.*
rd release
del /Q /F CJ60\lib\*.*
rd CJ60\lib
del /Q /F CJ60\CJ60Lib\release\*.*
del /Q /F CJ60\CJ60Lib\debug\*.*
del /Q /F CJ60\CJ60Lib\staticdebug\*.*
del /Q /F CJ60\CJ60Lib\staticrelease\*.*
rd CJ60\CJ60Lib\release
rd CJ60\CJ60Lib\debug
rd CJ60\CJ60Lib\staticdebug
rd CJ60\CJ60Lib\staticrelease
del /Q /F plugins\qgames\debug\*.*
del /Q /F plugins\qgames\release\*.*
rd plugins\qgames\debug
rd plugins\qgames\release
pause