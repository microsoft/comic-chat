[Version]
Class=IEXPRESS
SEDVersion=3

[Options]
PackagePurpose=InstallApp
ShowInstallProgramWindow=0
HideExtractAnimation=0
UseLongFileName=0
InsideCompressed=0
CAB_FixedSize=0
CAB_ResvCodeSigning=6144
RebootMode=I
InstallPrompt=%InstallPrompt%
DisplayLicense=%DisplayLicense%
FinishMessage=%FinishMessage%
TargetName=%TargetName%
FriendlyName=%FriendlyName%
AppLaunched=%AppLaunched%
PostInstallCmd=%PostInstallCmd%
AdminQuietInstCmd=%AdminQuietInstCmd%
UserQuietInstCmd=%UserQuietInstCmd%
SourceFiles=SourceFiles

[Strings]
InstallPrompt=Install Microsoft Chat Protocol 2.0 beta 1 and Chat Control 1.1?
DisplayLicense=
FinishMessage=Installation complete!
TargetName=\nm4\chat\sdk\Chatcntl.exe
FriendlyName=Microsoft Chat Protocol Control 2.0 beta 1 & Chat Control 1.1
AppLaunched=chatmsdn.inf
PostInstallCmd=<None>
AdminQuietInstCmd=
UserQuietInstCmd=
FILE0="license.txt"
FILE1="chatmsdn.inf"
FILE2="chatsock.dll"
FILE3="mschat.ocx"
FILE4="Mschatpr.ocx"

[SourceFiles]
SourceFiles0=.\
SourceFiles1=.\bin\



[SourceFiles0]
%FILE0%=
%FILE1%=

[SourceFiles1]
%FILE2%=
%FILE3%=
%FILE4%=