[Version]
Class=IEXPRESS
CDFVersion=2.0

[Options]
PackagePurpose=CreateCAB
ShowInstallProgramWindow=0
HideExtractAnimation=0
UseLongFileName=0
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
SourceFiles=SourceFiles

[Strings]
InstallPrompt=""
DisplayLicense=""
FinishMessage=""
TargetName=".\MsChatPr.Cab"
FriendlyName="Microsoft Chat Protocol OCX"
AppLaunched=""
PostInstallCmd=""

FILE0="MSChatPr.ocx"
FILE1="MsChatPr.inf"

[SourceFiles]
SourceFiles0=.\

[SourceFiles0]
%FILE0%
%FILE1%
