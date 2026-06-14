/*
	Created : RegisB, 06/30/97

 This file is meant for preprocessing only to generate 
 a proper batch file with correct build version

*/

#include "chatver.h"

sed -e s/---VERSIONNUM---/VER_PRODUCTVERSION/g < CChat21o.inf > CChat21.inf
