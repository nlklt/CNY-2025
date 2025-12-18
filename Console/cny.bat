@echo off
cls
setlocal

:: --- НАСТРОЙКИ ПУТЕЙ ---
:: Путь к папке с проектом
set "WORK_DIR=D:\KPO\CNY-2025"
:: Путь к компилятору
set "MY_COMPILER=.\x64\Debug\CNY-2025.exe"
:: Путь к исходному файлу
set "SOURCE_INPUT=in.cny"

set "LIB_DIR=.\Debug"
set "MSVC_BIN=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x86"

:: Настройка путей библиотек (в одну строку)
set "LIB=%LIB_DIR%;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\lib\x86;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x86;C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x86"

:: Переход на диск D и в рабочую папку
cd /d "%WORK_DIR%"

:: --- ШАГ 1: ГЕНЕРАЦИЯ ASM ФАЙЛА ---
echo [STEP 1] Running custom compiler...
if not exist "%SOURCE_INPUT%" (
    echo [ERROR] Input file %SOURCE_INPUT% not found!
    pause
    exit /b
)

:: Запуск вашего компилятора для создания in.cny.out.asm
"%MY_COMPILER%" -i %SOURCE_INPUT%

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Your compiler failed.
    pause
    exit /b
)

:: Проверяем, создал ли ваш компилятор файл
if not exist in.cny.out.asm (
    echo [ERROR] File in.cny.out.asm was not created by your compiler!
    pause
    exit /b
)

:: --- ШАГ 2: АССЕМБЛИРОВАНИЕ ---
echo [STEP 2] Assembling with MASM...
"%MSVC_BIN%\ml.exe" /c /coff /Zd /Zi in.cny.out.asm
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] ml.exe failed.
    pause
    exit /b
)

:: --- ШАГ 3: ЛИНКОВКА ---
echo [STEP 3] Linking...
"%MSVC_BIN%\link.exe" /subsystem:console /entry:main in.cny.out.obj Library.lib kernel32.lib libucrt.lib /out:example.exe
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] link.exe failed.
    pause
    exit /b
)

:: --- ШАГ 4: ЗАПУСК ---
if exist example.exe (
    echo.
    echo [SUCCESS] Running example.exe:
    echo -----------------------------------
    example.exe
    echo.
    echo -----------------------------------
) else (
    echo [ERROR] example.exe was not created.
)

echo.
echo Process finished.
pause