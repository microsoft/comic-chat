#
# makefile for Comic Chat, rossb, 4/4/96
#

all: init ssync bumpexe exe setup pack drop cleanup

init:
		echo Performing daily build > ..\build\buildlog.txt
		-showtime >> ..\build\buildlog.txt

ssync:
		cd ..
		echo SSync'ing >> build\buildlog.txt
		ssync -rf >> build\buildlog.txt
		cd build

bumpexe:
		cd ..\client
		echo Incrementing CChat version number >> ..\build\buildlog.txt
        sadmin setpv ..+1 >> ..\build\buildlog.txt
		cd ..\build

exe:
        cd ..\client
		echo Building client application (debug version) >> ..\build\buildlog.txt
        nmake CFG="chat - Win32 Debug" -f chat.mak >> ..\build\buildlog.txt
		echo Building client application (release version) >> ..\build\buildlog.txt
        nmake CFG="chat - Win32 Release" -f chat.mak >> ..\build\buildlog.txt
		cd ..\build

setup:
		cd ..
		echo Creating drop directory and files >> build\buildlog.txt
		-deltree -y c:\cchat\drop
		-md drop
		-md drop\ComicArt
		-md drop\ComicArt\Avatars
		-md drop\ComicArt\Backdrop
		-copy client\chat.exe drop\chat.exe
		-copy client\debug\chat.exe drop\chatd.exe
                -copy client\chat.pdb drop\chat.pdb
		-copy client\license.txt drop\license.txt
		-copy client\titles.txt drop\titles.txt
                -copy client\readme.htm drop\readme.htm
                -copy client\readme.gif drop\readme.gif
		-copy client\cchat.hlp drop\cchat.hlp
		-copy client\cchat.cnt drop\cchat.cnt
		-xcopy /s /i client\comicart drop\comicart
                -xcopy /s /i shared drop
		cd build

pack:
		cd ..\setup
		echo Packing self-extracting .exe (cchat10.exe) >> ..\build\buildlog.txt
		iexpress /n cchat.cdf
		echo Packing debug version >> ..\build\buildlog.txt
		iexpress /n cchatd.cdf
                echo Packing heavy version >> ..\build\buildlog.txt
                iexpress /n cchata.cdf
		cd ..\build

drop:
		cd ..\drop
		echo Copying binaries to \\curlew\vwgpub >> ..\build\buildlog.txt
		-net use x: /d
		-net use x: \\curlew\vwgpub
		-attrib -r x:\cchat\previous\*.*
        -attrib -r x:\cchat\current\*.*
		-deltree -y x:\cchat\previous\.
		-copy x:\cchat\current x:\cchat\previous
		-copy cchat10.exe x:\cchat\current\cchat10.exe
		-copy cchat10d.exe x:\cchat\current\cchat10d.exe
                -copy cchat10a.exe x:\cchat\current\cchat10a.exe
        -deltree -y x:\cchat\flat
        -md x:\cchat\flat
		-copy chat.exe x:\cchat\flat
		-copy *.txt x:\cchat\flat
                -copy ..\setup\cchat.inf x:\cchat\flat
		-copy *.hlp x:\cchat\flat
		-copy *.cnt x:\cchat\flat
		-copy comicart\avatars\*.* x:\cchat\flat
		-copy comicart\backdrop\*.* x:\cchat\flat
        -attrib +r x:\cchat\current\*.*
        -attrib +r x:\cchat\previous\*.*
		-net use x: /d
		cd ..\build

cleanup:
		echo Daily build completed! >> buildlog.txt

		
