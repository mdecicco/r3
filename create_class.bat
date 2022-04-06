@echo off
setlocal enabledelayedexpansion

set /p class_name="Enter class name: "
set /p dir_name="Enter dir name: "

set header_name="%class_name%.h"
set src_name="%class_name%.cpp"
set "cmakefile=./CMakeLists.txt"

if not exist "./include/r3/%dir_name%/" (
	mkdir "./include/r3/%dir_name%"
	mkdir "./src/%dir_name%"
	set "doadd="
	for /f "delims=" %%i in ('type "%cmakefile%" ^| find /v /n "" ^& break ^> "%cmakefile%"') do (
		set "line=%%i"
		set "line=!line:*]=!"
		if "!line!" == "# auto_add_sources_below_this_line" (set "doadd=1")
		if not defined line (
			if "!doadd!" == "1" (>>"%cmakefile%" echo add_sources("src/%dir_name%" "%dir_name%"^) && set "doadd=")
		)
		>>"%cmakefile%" echo(!line!
	)
)

copy default_class.h "./include/r3/%dir_name%/%header_name%" >nul
cd "./src/%dir_name%"

set printf="C:\Program Files\Git\usr\bin\printf.exe"

%printf% "#include <r3/%dir_name%/%header_name%>\n\nnamespace r3 {\n};" >> %src_name%

cd ../..
copy /b CMakeLists.txt +,,
pause