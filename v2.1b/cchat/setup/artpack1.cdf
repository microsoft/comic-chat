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
InstallPrompt="Install Microsoft Comic Chat Art Pack 1?"
DisplayLicense="..\license.txt"
FinishMessage="Thanks for downloading Microsoft Art Pack 1!  Installation complete!"
TargetName=".\ArtPack1.exe"
FriendlyName="Microsoft Comic Chat Art Pack 1"
AppLaunched="ArtPack1.INF"
PostInstallCmd="<None>"

FILE0="ArtPack1.INF"

FILE1="license.txt"

FILE10="Bolo.avb"
FILE11="Cro.avb"
FILE12="Denise.avb"
FILE13="Kevin.avb"
FILE14="Kwensa.avb"
FILE15="Lynnea.avb"
FILE16="Maynard.avb"
FILE17="Rebecca.avb"
FILE18="Sage.avb"
FILE19="Scotty.avb"

FILE31="Den.bmp"
FILE32="Volcano.bmp"


[SourceFiles]
SourceFiles0=.\
SourceFiles1=..\
SourceFiles2=..\ArtPack1\

[SourceFiles0]
%FILE0%

[SourceFiles1]
%FILE1%

[SourceFiles2]
%FILE10%
%FILE11%
%FILE12%
%FILE13%
%FILE14%
%FILE15%
%FILE16%
%FILE17%
%FILE18%
%FILE19%
%FILE31%
%FILE32%
