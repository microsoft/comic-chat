/*
 * Copyright (C) 1997 KB Sriram
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

package sbktech.tools.hashjava.util;

/**
 * Interface defining mechanism to pick up a sequence
 * of names to use while obfuscating.
 * @author $Author: kbs $
 */

public abstract class NameGenerator
{
  /**
   * Generate a new name for this old symbol.
   */
  public abstract String nextName(String oldName);

  public String nextName()
  { return nextName(""); }

  /**
   * Return a name which does <em>not</em> belong in the stop
   * table.
   */
  public String checkedName(java.util.Hashtable stop, String oldName)
  {
    String ret = nextName(oldName);
    if (stop != null)
      {
        int i=0;
        while (stop.get(ret) != null)
          { ret = nextName(oldName + (i++)); }
      }
    return ret;
  }
}
