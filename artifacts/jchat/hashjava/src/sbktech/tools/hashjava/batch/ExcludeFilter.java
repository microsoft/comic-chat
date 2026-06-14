/*
 * Copyright (C) 1996 KB Sriram
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
 * MA 02139, USA.
 */
package sbktech.tools.hashjava.batch;

import sbktech.tools.hashjava.util.*;

/**
 * This class is used to provide a way for the obfuscator
 * to determine if a particular type/name can be renamed.
 */

class ExcludeFilter
{
  private Matcher patterns[];
  private int types[];

  ExcludeFilter(Matcher p[], int t[])
  { patterns = p; types = t; }

  boolean matches(int type, String name)
  {
    for (int i=0; i<patterns.length; i++)
      {
        boolean tmatched = false;
        if (types[i] > 0)
          { tmatched = ((types[i] & type) == types[i]); }
        else if (types[i] == 0)
          { tmatched = (type == 0); }
        else
          { tmatched = true; }

        if (tmatched && patterns[i].match(name))
          return true;
      }
    return false;
  }
}
