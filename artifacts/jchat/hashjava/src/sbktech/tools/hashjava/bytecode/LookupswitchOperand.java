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

class LookupswitchOperand extends InsnOperand
{
  int dflt;
  int[] jump;
  int[] match;
  Insn source, dfltI, jumpI[];

  LookupswitchOperand(DataInput in, Insn source)
       throws IOException
  {
    this.source = source;
    int pad;
    int curpc = source.pc;

    if (((curpc+1) % 4) != 0)
      {
        pad = (4 - ((curpc+1) % 4));
        for (int x=0; x<pad; x++) in.readByte();
      }

    dflt = in.readInt();
    jump = new int[in.readInt()];
    match = new int[jump.length];

    for (int x=0; x<jump.length; x++)
      {
        match[x] = in.readInt();
        jump[x] = in.readInt();
      }
  }
  void writePrefix(DataOutput out)
  {}
  void write(DataOutput out)
       throws IOException
  {
    int pad;
    int curpc = source.pc;

    if (((curpc+1) % 4) != 0)
      {
        pad = (4 - ((curpc+1) % 4));
        for (int x=0; x<pad; x++) out.writeByte(0);
      }
    out.writeInt(dfltI.pc - curpc);
    out.writeInt(jump.length);
    for (int x=0; x<jump.length; x++)
      {
        out.writeInt(match[x]);
        out.writeInt(jumpI[x].pc - curpc);
      }
  }

  void fixup(CodeAttr code)
  {
    int mypc = source.pc;
    dfltI = code.getInsn(mypc + dflt);
    jumpI = new Insn[jump.length];
    for (int i=0; i<jump.length; i++)
      { jumpI[i] = code.getInsn(mypc + jump[i]); }
  }

  int size()
  {
    int sz = 8;

    int curpc = source.pc;
    if (((curpc+1) %4)!= 0)
      {
        sz += (4 - ((curpc+1) % 4));
      }
    sz += 8*jump.length;
    return sz;
  }
}
