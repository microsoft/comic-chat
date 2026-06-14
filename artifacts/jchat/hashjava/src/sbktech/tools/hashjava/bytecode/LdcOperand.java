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

class LdcOperand extends InsnOperand
{
  CP target;
  boolean wide, modified=false;
  Insn source;

  LdcOperand(DataInput in, boolean wide, Insn source, CP cps[])
       throws IOException
  {
    this.wide = wide;
    this.source = source;

    if (wide)
      { target = cps[in.readUnsignedShort()]; }
    else
      { target = cps[in.readUnsignedByte()]; }
  }

  public String toString()
  { return "index val is " + target.idx + target.toString(); }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    target = cinfo.addCP(target);
    if (source.opc != opc_ldc2_w) // Flag for potential opcode widening
      { modified = true; }
  }

  void writePrefix(DataOutput out)
  {}
  void write(DataOutput out)
       throws IOException
  {
    if (wide)
      {
        out.writeShort(target.idx); }
    else
      { out.writeByte(target.idx); }
  }

  int size()
  {
    if (modified)
      {
        wide = (target.idx > 255);
        modified= false;
                                // set up apropriate opcode
        if (wide &&
            (source.opc == opc_ldc))
          { source.opc = opc_ldc_w; }
        else if (!wide &&
                 (source.opc == opc_ldc_w))
          { source.opc = opc_ldc; }
      }
    if (wide)
      { return 2; }
    else
      { return 1; }
  }
}
