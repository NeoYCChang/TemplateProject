@echo off

SetLocal EnableDelayedExpansion


set QT_PATH=C:\Qt
set QT_Version=5.15.2
set Qt_Compiler=mingw81_64
set Compiler_Version=810_64
set argOption=0
for %%x in (%*) do (
	IF %%x==-h (
		set argOption=0
		GOTO helpFunction
	) ^
	ELSE IF %%x==-QT_PATH (
		set argOption=1
	) ^
	ELSE IF %%x==-QT_Version (
		set argOption=2
	) ^
	ELSE IF %%x==-Qt_Compiler (
		set argOption=3
	) ^
	ELSE IF %%x==-Compiler_Version (
		set argOption=4
	) ^
	ELSE (
		IF !argOption!==1 (
			set QT_PATH=%%x
			set argOption=0
		) ^
		ELSE IF !argOption!==2 (
			set QT_Version=%%x
			set argOption=0
		) ^
		ELSE IF !argOption!==3 (
			set Qt_Compiler=%%x
			set argOption=0
		) ^
		ELSE IF !argOption!==4 (
			set Compiler_Version=%%x
			set argOption=0
		) ^
		ELSE (
			set argOption=0
			echo "[Warning]: Unknown argument -> %%x"
		)
	)
)

echo Qt path: !QT_PATH!
echo Qt version: !QT_Version!
echo Qt Compiler: !Qt_Compiler!
echo Compiler version: !Compiler_Version!

set bat_dir=%~dp0
IF not exist !QT_PATH!\!QT_Version!\!Qt_Compiler!\bin (
  echo "[Error]: The path not exist.(!QT_PATH!\!QT_Version!\!Qt_Compiler!\bin)"
  GOTO helpFunction
) ^
ELSE IF not exist !QT_PATH!\Tools\mingw!Compiler_Version!\bin (
  echo "[Error]: The path not exist.(!QT_PATH!\Tools\mingw!Compiler_Version!\bin)"
  GOTO helpFunction
) ^
ELSE IF not exist !QT_PATH!\Tools\CMake_64\bin (
  echo "[Error]: The path not exist.(!QT_PATH!\Tools\CMake_64\bin)"
  GOTO helpFunction
) ^
ELSE IF not exist !QT_PATH!\Tools\Ninja (
  echo "[Error]: The path not exist.(!QT_PATH!\Tools\Ninja)"
  GOTO helpFunction
)
set PATH=!QT_PATH!\!QT_Version!\!Qt_Compiler!\bin;!QT_PATH!\Tools\mingw!Compiler_Version!\bin;!QT_PATH!\Tools\CMake_64\bin;!QT_PATH!\Tools\Ninja;%PATH%


set folders=
set c_count=0

for /D %%i in (%~dp0*) do (
	set folders=!folders!;%%i
)

for %%a in (!folders!) do (
	IF exist %%a\CMakeLists.txt (
		set /a c_count+=1
		MD %%a\win_build
		cmd /c "cmake.exe -S %%a -B %%a\win_build "-DCMAKE_GENERATOR:STRING=Ninja" "-DCMAKE_BUILD_TYPE:STRING=Release" "-DQT_QMAKE_EXECUTABLE:FILEPATH=qmake.exe" "-DCMAKE_C_COMPILER:FILEPATH=gcc.exe" "-DCMAKE_CXX_COMPILER:FILEPATH=g++.exe""
		cmd /c "cmake.exe --build %%a\win_build"
		for %%c in (%%a\win_build\*.exe) do (
			MD !bat_dir!%%~na_build_deploy_win64
			copy %%c !bat_dir!%%~na_build_deploy_win64\%%~xnc
			cmd /c "windeployqt.exe !bat_dir!%%~na_build_deploy_win64\%%~xnc"
		)
		rmdir /s /q %%a\win_build
	)
)

IF %c_count%==0 (
    echo [Warning]: CMakeLists.txt not found
) 
pause
goto:eof
:helpFunction
echo "-QT_PATH <path> -> Please enter Qt path (ex: -QT_PATH C:\Qt)"
echo.
echo "-QT_Version <version> -> Please enter Qt version (ex: -QT_Version 5.15.2)"
echo.
echo "-Qt_Compiler <Compiler> -> Please enter Qt Compiler (ex:-Qt_Compiler mingw81_64)"
echo.
echo "-Compiler_Version <version> -> Please enter Compiler version (ex:-Compiler_Version 810_64)"
echo.
pause