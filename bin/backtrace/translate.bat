@echo off
if not exist backtrace_translate.log goto translate
@del /F /Q backtrace_translate.log
:translate
php backtrace.php
if not exist backtrace_translate.log goto end
echo Ouverture de Notepad
notepad backtrace_translate.log
:end
pause