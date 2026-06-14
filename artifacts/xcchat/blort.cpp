// Had to move the innards that were in stdafx.cpp to here so they would compile. NM's build
// system doesn't compile stdafx.cpp when it makes a precompiled header out of stdafx.h

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlimpl.cpp>
#include <atlctl.cpp>
#include <atlwin.cpp>
