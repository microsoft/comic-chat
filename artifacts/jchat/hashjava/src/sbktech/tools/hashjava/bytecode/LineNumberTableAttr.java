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

class LineNumberTableAttr extends Attr
{
  LineNumberEntry lines[];

  /**
   * Create an empty line table debugging attribute
   */
  public LineNumberTableAttr()
  {
    name = new UnicodeCP("LineNumberTable");
    lines = new LineNumberEntry[0];
    mysize = 2;
  }

  public void corrupt()
  {
                                // Just remove the line table.
    lines = new LineNumberEntry[0];
    mysize = 2;
  }

  LineNumberTableAttr(DataInput in, UnicodeCP n, int size)
       throws IOException
  {
    name = n;
    Sname = n.val;
    this.mysize = size;
    lines = new LineNumberEntry[in.readUnsignedShort()];
    for (int i=0; i<lines.length; i++)
      { lines[i] = new LineNumberEntry(in); }
  }

  int size()
  {
    if (mysize != 4*lines.length + 2)
      {
        System.err.println("Mismatched size in line number table");
      }
    return 6 + 4*lines.length + 2;
  }

  void write(DataOutput out)
       throws IOException
  {
    if (writeOut)
      {
                                // Determine total size
        int sz = 4*lines.length + 2;

                                // Now for everything else
        out.writeShort(name.idx);
        out.writeInt(sz);
        out.writeShort(lines.length);
        for (int i=0; i<lines.length; i++)
          { lines[i].write(out); }
      }
  }
}
