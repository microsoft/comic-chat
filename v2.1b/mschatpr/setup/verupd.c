/*
	Created : RegisB, 07/09/97

 This file is meant for preprocessing only to generate 
 a proper batch file with correct build version

*/

#include "version.h"

sed -e s/---VERSIONNUM---/VER_PRODUCTVERSION/g < CPrOcxOr.htm > MsChatPrOcx.htm
sed -e s/---VERSIONNUM---/VER_PRODUCTVERSION/g < CPrCabOr.htm > MsChatPrCab.htm
sed -e s/---VERSIONNUM---/VER_PRODUCTVERSION/g < CPrOr.inf > MsChatPr.inf

