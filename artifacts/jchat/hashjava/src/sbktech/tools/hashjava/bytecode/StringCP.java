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

package sbktech.tools.hashjava.bytecode;

import java.io.*;

class StringCP extends CP
{
  int string_idx;
  UnicodeCP string;
  String Sstring;

  StringCP(int sidx)
  { string_idx = sidx; }
  StringCP(String s)
  { Sstring = s; }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_STRING);
    out.writeShort(string.idx);
  }
  void setRefs(CP[] cps)
  { string = (UnicodeCP)cps[string_idx]; Sstring = string.val; }
  void resolve(ClassInfo cinfo)
  { string = (UnicodeCP) cinfo.addCP(new UnicodeCP(Sstring)); }
  void setNewName(Environment env, ClassInfo cinfo)
  {}
  public int hashCode()
  { return Sstring.hashCode(); }
  public boolean equals(Object arg)
  {
    return ((arg instanceof StringCP) &&
            ((StringCP) arg).Sstring.equals(Sstring));
  }
  public String toString()
  { return "String: " + Sstring; }
}
