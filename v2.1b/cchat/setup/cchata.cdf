[Version]
Class=IEXPRESS
CDFVersion=2.0

[Options]
PackagePurpose=InstallApp
ShowInstallProgramWindow=0
HideExtractAnimation=0
UseLongFileName=0
RebootMode=I
InstallPrompt=%InstallPrompt%
DisplayLicense=%DisplayLicense%
FinishMessage=%FinishMessage%
TargetName=%TargetName%
FriendlyName=%FriendlyName%
AppLaunched=%AppLaunched%
PostInstallCmd=%PostInstallCmd%
SourceFiles=SourceFiles
CAB_FixedSize=0
CAB_ResvCodeSigning=6144

[Strings]
InstallPrompt="Install Microsoft Comic Chat 1.1?"
DisplayLicense="..\license.txt"
FinishMessage="Thanks for downloading Microsoft Comic Chat 1.1!  Installation complete!"
TargetName=".\ComicChat11A.exe"
FriendlyName="Microsoft Comic Chat 1.1"
AppLaunched="cchat11a.inf"
PostInstallCmd="<None>"

FILE0="cchat11a.inf"

FILE1="license.txt"

FILE2="CChat.exe"
FILE3="Cchat.hlp"
FILE4="Cchat.cnt"
FILE5="readme.htm"
FILE6="readme.gif"

FILE10="field.bmp"
FILE11="pastoral.bmp"
FILE12="room.bmp"

FILE13="clouds.bmp"
FILE14="space.bmp"

FILE20="buck.avb"
FILE21="kirby.avb"
FILE22="veronica.avb"

FILE30="anna.avb"
FILE31="armando.avb"
FILE32="dan.avb"
FILE33="hugh.avb"
FILE34="jordan.avb"
FILE35="lance.avb"
FILE36="margaret.avb"
FILE37="mike.avb"
FILE38="susan.avb"
FILE39="tiki.avb"
FILE40="tongtyed.avb"
FILE41="xeno.avb"

FILE50="chatsock.dll"
FILE51="Comic.ttf"
FILE52="Riched32.dll"
FILE53="Msvcrt40.dll"
FILE54="MFC40.dll"

[SourceFiles]
SourceFiles0=.\
SourceFiles1=..\
SourceFiles2=..\ComicArt\


[SourceFiles0]
%FILE0%
%FILE50%
%FILE51%
%FILE53%
%FILE54%

[SourceFiles1]
%FILE1%
%FILE2%
%FILE3%
%FILE4%
%FILE5%
%FILE6%

[SourceFiles2]
%FILE10%
%FILE11%
%FILE12%
%FILE30%
%FILE31%
%FILE32%
%FILE33%
%FILE34%
%FILE35%
%FILE36%
%FILE37%
%FILE38%
%FILE39%
%FILE40%
%FILE41%

[SourceFiles3]
%FILE13%
%FILE14%
%FILE20%
%FILE21%
%FILE22%