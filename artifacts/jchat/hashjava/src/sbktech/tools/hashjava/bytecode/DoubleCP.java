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

class DoubleCP extends CP
{
  double val;
  Double Cval;
  DoubleCP(double d)
  { val = d; Cval = new Double(d); }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_DOUBLE);
    out.writeDouble(val);
  }
  void setRefs(CP[]cps)
  {}
  void resolve(ClassInfo c)
  {}
  void setNewName(Environment env, ClassInfo cinfo)
  {}
  public boolean equals(Object arg)
  {
    return ((arg instanceof DoubleCP) &&
            ((DoubleCP) arg).Cval.equals(Cval));
  }
  public int hashCode()
  { return Cval.hashCode(); }
}
