@echo off
echo Setting Environment Variables
set path=.;..\jdk114\bin;..\sdk20\bin;%windir%;%windir%\command
set classpath=.;..\jdk114\lib\classes.zip;..\hashjava
echo Deleting Class Files
attrib *.class -r
del *.class
cd ob
attrib *.class -r
del *.class
cd ..
echo Compiling with JVC
jvc /w0 *.java
echo Running Hashjava
java sbktech.tools.hashjava.batch.Main script.conf
echo Done
