@echo off
set croot=C:\progra~1\cchat
if %1n==n goto setup
set croot=%1

:setup


CHOICE.COM /C:DR "change TO Debug or Retail "
if errorlevel 1 set vers=.
if errorlevel 2 set vers=..\retail


echo ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
echo ³ Usage: CHANGE CChatdir
echo ³ Example: CHANGE C:\Progra~1\cchat
echo ³
echo ³ This will copy %vers% comic chat components to your system
echo ³ The CChat directory is currently set to:
echo ³ 
echo ³          %croot%
echo ³ 
echo ³ If this is not correct, ctrl-c to exit
echo ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

pause

:GUI
copy %vers%\CChat.exe                %croot%
copy %vers%\CChat.dbg                %croot%
copy %vers%\CChat.pdb                %croot%
copy %vers%\CChat.sym                %croot%

:END
set vers=
