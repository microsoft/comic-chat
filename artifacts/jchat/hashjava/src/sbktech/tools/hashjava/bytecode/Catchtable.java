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

// Record a catch table
// $Author: kbs $
// $Revision: 1.5 $

package sbktech.tools.hashjava.bytecode;
import java.io.*;

class Catchtable
{
  CatchEntry entries[];

  Catchtable(DataInput in, CP cps[])
       throws IOException
  {
    entries = new CatchEntry[in.readUnsignedShort()];
    for (int i=0; i<entries.length; i++)
      { entries[i] = new CatchEntry(in, cps); }
  }

  void fixup(CodeAttr code)
  {
    for (int i=0; i<entries.length; i++)
      { entries[i].fixup(code); }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    for (int i=0; i<entries.length; i++)
      { entries[i].makeCP(env, cinfo); }
  }
  int size()
  {
    return 2+ 8*entries.length;
  }
  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(entries.length);
    for (int i=0; i<entries.length; i++)
      { entries[i].write(out); }
  }
}

class CatchEntry
{
  ClassCP catchcp;
  int start_pc, end_pc, handler_pc;
  Insn startI, endI, handlerI;

  CatchEntry(DataInput in, CP cps[])
       throws IOException
  {
    start_pc = in.readUnsignedShort();
    end_pc = in.readUnsignedShort();
    handler_pc = in.readUnsignedShort();
    catchcp = (ClassCP)cps[in.readUnsignedShort()];
  }
  void fixup(CodeAttr code)
  {
    startI = code.getInsn(start_pc);
    endI = code.getInsn(end_pc);
    handlerI = code.getInsn(handler_pc);
  }
  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (catchcp != null)
      {
        catchcp.setNewName(env, cinfo);
        catchcp = (ClassCP) cinfo.addCP(catchcp);
      }
  }
  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(startI.pc);
    out.writeShort(endI.pc);
    out.writeShort(handlerI.pc);
    if (catchcp != null)
      { out.writeShort(catchcp.idx); }
    else
      { out.writeShort(0); }
  }
}
