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

class InvokeinterfaceOperand extends InsnOperand implements VMConstants
{
  CP cp;
  int nargs;
  int reserved;

  InvokeinterfaceOperand(DataInput in, CP cps[])
       throws IOException
  {
    cp = cps[in.readUnsignedShort()];
    nargs = in.readUnsignedByte();
    reserved = in.readByte();
  }
  void makeCP(Environment env, ClassInfo cinfo)
  {
    cp.setNewName(env, cinfo);
    cp = cinfo.addCP(cp);
  }
  public String toString()
  { return cp.toString() + "("+nargs + " args)"; }

  void writePrefix(DataOutput out)
  {}
  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(cp.idx);
    out.writeByte(nargs);
    out.writeByte(reserved);
  }
  int size()
  { return 4; }
}
