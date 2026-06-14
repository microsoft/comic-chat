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

class NameTypeCP extends CP
{
  int name_idx;
  int sig_idx;
  UnicodeCP name, sig;
  String Sname, Ssig;

  NameTypeCP(int n, int s)
  { name_idx = n; sig_idx = s; }
  NameTypeCP(String n, String s)
  { Sname = n; Ssig = s; }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_NAMEANDTYPE);
    out.writeShort(name.idx);
    out.writeShort(sig.idx);
  }
  void setRefs(CP[] cps)
  {
    name = (UnicodeCP) cps[name_idx];
    sig = (UnicodeCP) cps[sig_idx];
    Sname = name.val;
    Ssig = sig.val;
  }
  void setNewName(Environment env, ClassInfo cinfo)
  {}
  void resolve(ClassInfo cinfo)
  {
    name = (UnicodeCP) cinfo.addCP(new UnicodeCP(Sname));
    sig = (UnicodeCP) cinfo.addCP(new UnicodeCP(Ssig));
  }
  public boolean equals(Object arg)
  {
    return ((arg instanceof NameTypeCP) &&
            ((NameTypeCP)arg).Sname.equals(Sname) &&
            ((NameTypeCP)arg).Ssig.equals(Ssig));
  }
  public int hashCode()
  {
    return((Ssig.hashCode() << 16) |
           (Sname.hashCode() & 0xffff));
  }
  public String toString()
  { return (Sname + " is of type " + Ssig); }
}
