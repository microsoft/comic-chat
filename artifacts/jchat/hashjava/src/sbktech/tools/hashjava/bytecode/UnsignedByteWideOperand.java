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

class UnsignedByteWideOperand extends InsnOperand
{
  int val;
  boolean wide;

  UnsignedByteWideOperand(DataInput in, boolean wide)
       throws IOException
  {
    this.wide = wide;
    if (wide)
      { val = in.readUnsignedShort(); }
    else
      { val = in.readUnsignedByte(); }
  }
  void writePrefix(DataOutput out)
       throws IOException
  {
    if (wide)
      { out.writeByte(opc_wide); }
  }
  public String toString()
  { return Integer.toString(val); }

  void write(DataOutput out)
       throws IOException
  {
    if (wide)
      { out.writeShort(val); }
    else
      { out.writeByte(val); }
  }
  int size()
  {
    if (wide)
      { return 3; }
    else
      { return 1; }
  }
}
