:: 输出命令
echo oo 1

:: @ 与 echo off 相似，但 @ 只影响当前行
:: 不输出命令，只有结果
@rem 也是注释
@echo oo

:: 调用并执行另一个 bat，执行完后回到当前 bat，继续执行
call bat_demo_1.bat a b c
@echo back
@echo current exe bat: %0

:: 暂停
pause

:: 此命令后的所有命令都不显示命令本身
echo off

choice /C abq /M "aa,bb,quit"
if errorlevel 1 goto a
if errorlevel 2 goto b
if errorlevel 3 goto q

:a
@echo aa

:b
@echo bb

:q
echo quit