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
import java.util.*;

class LocalVariableEntry
{
  int pc, len, nidx, sigidx, slot;
  UnicodeCP name, nsig;
  String Sname, Ssig;
  LocalVariableEntry(String n, String s,
                     int pc, int len, int slot)
  {
    Sname = n;
    Ssig = s;
    this.pc = pc;
    this.len = len;
    this.slot = slot;
  }

  LocalVariableEntry(DataInput in, CP cps[])
       throws IOException
  {
    pc = in.readUnsignedShort();
    len = in.readUnsignedShort();
    nidx = in.readUnsignedShort();
    name = (UnicodeCP)(cps[nidx]);
    if (name != null)
      { Sname = name.val; }
    sigidx = in.readUnsignedShort();
    nsig = (UnicodeCP)(cps[sigidx]);
    if (nsig != null)
      { Ssig = nsig.val; }
    slot = in.readUnsignedShort();
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (Sname != null)
      { name = (UnicodeCP)(cinfo.addCP(new UnicodeCP(Sname))); }
    else
      { name = null; }
    if (Ssig != null)
      { nsig = (UnicodeCP)(cinfo.addCP(new UnicodeCP(Ssig))); }
    else
      { nsig = null; }
  }

  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(pc);
    out.writeShort(len);
    if (name != null)
      { out.writeShort(name.idx); }
    else
      { out.writeShort(0); }
    if (nsig != null)
      { out.writeShort(nsig.idx); }
    else
      { out.writeShort(0); }
    out.writeShort(slot);
  }
}
