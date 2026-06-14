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

class SourceFileAttr extends Attr
{
  UnicodeCP source;
  String Ssource;

  public void corrupt()
  {

    //    Ssource = null; This would be nice, but VM's seem to like
    //    having this attribute around properly.
    Ssource =
 "\007\015WARNING: Decompiling this code may violate your licensing agreement\012\032\014";
  }

  /**
   * Create a new debugging attribute with this name.
   */
  public SourceFileAttr(String s)
  { Ssource = s; name = new UnicodeCP("SourceFile"); mysize = 2; }

  public String toString()
  {
    String ret = "SourceFile: index is ";
    if (source!=null)
      { ret += source.toString(); }
    else
      { ret += "null"; }
    return ret;
  }

  SourceFileAttr(DataInput in, UnicodeCP n, int size, CP cps[])
       throws IOException
  {
    name = n;
    Sname = n.val;
    this.mysize = size;
    source = (UnicodeCP) cps[in.readUnsignedShort()];
    if (source != null)
      { Ssource = source.val; }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (writeOut)
      {
        super.makeCP(env, cinfo);
        if (Ssource != null)
          { source = (UnicodeCP) cinfo.addCP(new UnicodeCP(Ssource)); }
        else
          { source = null; }
      }
  }

  void write(DataOutput out)
       throws IOException
  {
    if (writeOut)
      {
        super.write(out);
        if (source != null)
          { out.writeShort(source.idx); }
        else
          { out.writeShort(0); }
      }
  }
  int size()
  { return 6 + 2; }
}
