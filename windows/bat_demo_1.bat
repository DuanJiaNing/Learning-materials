@echo current exe %0
@echo reciver args[1-3] max 9: %1 %2 %3

:: 条件语句
@ if "%1" == "1" goto :true
@ if not "%1" == "1" goto :false

:true
@echo true

:false
@echo false

