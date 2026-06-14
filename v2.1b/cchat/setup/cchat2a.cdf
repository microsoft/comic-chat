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
VersionInfo=VersionSection

[VersionSection] 
FromFile="..\cchat.exe"
Internalname=%FileName% 
OriginalFilename=%FileName% 
FileDescription=%FileDesc% 
ProductName=%ProdName%

[Strings]
InstallPrompt="Install Microsoft Chat 2.0?"
DisplayLicense="..\license.txt"
FinishMessage="Thanks for downloading Microsoft Chat 2.0!  Installation complete!"
TargetName=".\MsChat2A.exe"
FriendlyName="Microsoft Chat 2.0"
AppLaunched="cchat2a.inf"
PostInstallCmd="<None>"

FileName = "MsChat2A.exe"
FileDesc = "Self Extracting Microsoft Chat 2.0"
ProdName = "Microsoft Chat"

FILE0="cchat2a.inf"

FILE1="license.txt"

FILE2="CChat.exe"
FILE3="Cchat.hlp"
FILE4="Cchat.cnt"
FILE5="readme.htm"
FILE6="readme.gif"
FILE7="readme.txt"

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
%FILE7%

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