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

class LocalVariableTableAttr extends Attr
{
  LocalVariableEntry lvars[];

  /**
   * Create an empty local variable debugging table
   */
  public LocalVariableTableAttr()
  {
    name = new UnicodeCP("LocalVariableTable");
    lvars = new LocalVariableEntry[0];
    mysize = 2;
  }

  public void corrupt()
  {
                                // Add a single dummy bad entry
    lvars = new LocalVariableEntry[1];
    lvars[0] = new
      LocalVariableEntry(null, null, 0, 1, 0);
    mysize = 2 + 10;
  }

  LocalVariableTableAttr(DataInput in, UnicodeCP n, int size, CP cps[])
       throws IOException
  {
    name = n;
    Sname = n.val;
    this.mysize = size;
    lvars = new LocalVariableEntry[in.readUnsignedShort()];
    for (int i=0; i<lvars.length; i++)
      { lvars[i] = new LocalVariableEntry(in, cps); }
  }

  int size()
  {
    if (mysize != 10 *lvars.length + 2)
      { System.err.println("Mismatched size in local var table"); }
    return 6 + 2 + 10*lvars.length;
  }
  void write(DataOutput out)
       throws IOException
  {
    if (writeOut)
      {
                                // Determine total size
        int sz = 10*lvars.length + 2;

                                // Now for everything else
        out.writeShort(name.idx);
        out.writeInt(sz);
        out.writeShort(lvars.length);
        for (int i=0; i<lvars.length; i++)
          { lvars[i].write(out); }
      }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (writeOut)
      {
        super.makeCP(env, cinfo);
        for (int i=0; i<lvars.length; i++)
          { lvars[i].makeCP(env, cinfo); }
      }
  }
}
