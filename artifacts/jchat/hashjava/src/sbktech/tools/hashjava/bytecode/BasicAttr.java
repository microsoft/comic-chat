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


class BasicAttr extends Attr
{
  byte data[];

  BasicAttr(String n, byte[] data)
  {
    name = new UnicodeCP(n);
    mysize = data.length;
    this.data = data;
  }

  BasicAttr(DataInput in, UnicodeCP n, int sz)
       throws IOException
  {
    name = n;
    Sname = n.val;
    mysize = sz;
    data = new byte[sz];
    if (sz > 0) in.readFully(data);
  }
  int size()
  {
    return 6 + data.length;
  }
  void write(DataOutput out)
       throws IOException
  {
    if (writeOut)
      {
        super.write(out);
        out.write(data);
      }
  }
}
