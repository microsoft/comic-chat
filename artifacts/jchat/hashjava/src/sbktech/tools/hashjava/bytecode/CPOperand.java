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

class CPOperand extends InsnOperand
{
  CP target;
  boolean wide;
  Environment env;
  ClassInfo cinfo;

  CPOperand(DataInput in, boolean wide, CP cps[])
       throws IOException
  {
    this.wide = wide;
    if (wide)
      { target = cps[in.readUnsignedShort()]; }
    else
      { target = cps[in.readUnsignedByte()]; }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    target.setNewName(env, cinfo);
    target = cinfo.addCP(target);
    this.env = env;
    this.cinfo = cinfo;
  }

  void writePrefix(DataOutput out)
  {}
  void write(DataOutput out)
       throws IOException
  {
    if (wide)
      { out.writeShort(target.idx); }
    else
      { out.writeByte(target.idx); }
    if (target instanceof MethodCP)
      {
                                // Check if this is a Class.forName()
                                // call
        MethodCP chk = (MethodCP)target;
        if (("java/lang/Class".equals(chk.Scname)) &&
            ("forName".equals(chk.Sname)))
          {
            env.stats.info
              (cinfo.originalName() + " contains a Class.forName() call");
          }
      }
  }
  public String toString()
  { return target.toString(); }
  int size()
  {
    if (wide)
      { return 2; }
    else
      { return 1; }
  }
}
