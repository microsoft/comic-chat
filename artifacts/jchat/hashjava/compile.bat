@echo off
c:\java\bin\javac -d .  src\sbktech\tools\hashjava\bytecode\*.java
rem c:\java\bin\javac -d .  src\sbktech\tools\hashjava\util\*.java
REM 
REM due to a bug in the 1.02 unicode implementation, you will
REM need to include the file src/sbktech/tools/util/UnicodeNameGenerator.java
REM explicitly in the command line for the compile in any class
REM that uses it if you don't want the JDK 1.02 compiler to fail
REM
c:\java\bin\javac -d .  src\sbktech\tools\hashjava\batch\*.java src\sbktech\tools\hashjava\util\*.java  src\sbktech\tools\hashjava\hashpplet\*.java
c:\java\bin\javac -d . src\Example.java src\DirTreeGenerator.java src\InvalidDirectoryError.java
c:\java\bin\javadoc -author -d doc sbktech.tools.hashjava.bytecode sbktech.tools.hashjava.util src\sbktech\tools\hashjava\bytecode\*.java src\sbktech\tools\hashjava\util\*.java
