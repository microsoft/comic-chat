@echo off
echo Setting Environment Variables
set path=.;..\jdk114\bin;..\sdk20\bin;%windir%;%windir%\command
set classpath=.;..\jdk114\lib\classes.zip;..\hashjava
echo Deleting Class, Jar, Cab Files
attrib *.class -r
del *.class
cd ob
attrib *.class -r
del *.class
attrib *.jar -r
del *.jar
attrib *.cab -r
del *.cab
cd ..
echo Compiling with JAVAC
javac *.java
echo Running Hashjava
java sbktech.tools.hashjava.batch.Main mschat.conf
echo Creating Jar File
jar cvf ob\mschat.jar ob\*.class >> nul
dir *.jar /b
echo Creating Cab File
cabarc n ob\mschat.cab ob\*.class >> nul
dir *.cab /b
echo Deleting Class Files
del ob\*.class
del *.class
echo Compiling with JVC
jvc /w0 *.java
echo Running Hashjava
java sbktech.tools.hashjava.batch.Main mschat.conf
echo Done