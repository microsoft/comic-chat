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

class IincOperand extends InsnOperand implements VMConstants
{
  int vindex, constt;
  boolean wide;

  IincOperand(DataInput in, boolean wide)
       throws IOException
  {
    this.wide = wide;
    if (wide)
      {
        vindex = in.readUnsignedShort();
        constt = in.readShort();
      }
    else
      {
        vindex = in.readUnsignedByte();
        constt = in.readByte();
      }
  }
  void writePrefix(DataOutput out)
       throws IOException
  {
    if (wide)
      { out.writeByte(opc_wide); }
  }
  void write(DataOutput out)
       throws IOException
  {
    if (wide)
      {
        out.writeShort(vindex);
        out.writeShort(constt);
      }
    else
      {
        out.writeByte(vindex);
        out.writeByte(constt);
      }
  }

  public String toString()
  {
    return ("var["+vindex+"], " +constt);
  }
  int size()
  {
    if (wide)
      { return 5; }
    else
      { return 2; }
  }
}
