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

class ExceptionsAttr extends Attr
{
  ClassCP exceptions[];
  String Sexceptions[];

  ExceptionsAttr(DataInput in, UnicodeCP n, int size, CP cps[])
       throws IOException
  {
    name = n;
    Sname = n.val;
    this.mysize = size;
    exceptions = new ClassCP[in.readUnsignedShort()];
    Sexceptions = new String[exceptions.length];
    for (int i=0; i<exceptions.length; i++)
      {
        exceptions[i] = (ClassCP) cps[in.readUnsignedShort()];
        Sexceptions[i] = exceptions[i].Sname;
      }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    super.makeCP(env, cinfo);
    for (int i=0; i<exceptions.length; i++)
      {
        exceptions[i].setNewName(env, cinfo);
        exceptions[i] = (ClassCP) cinfo.addCP(exceptions[i]);
      }
  }
  int size()
  {
    return 6 + 2 + 2*exceptions.length;
  }
  void write(DataOutput out)
       throws IOException
  {
    super.write(out);
    out.writeShort(exceptions.length);
    for (int i=0; i<exceptions.length; i++)
      {
        out.writeShort(exceptions[i].idx);
      }
  }
}
