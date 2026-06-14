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

class ClassCP extends CP
{
  int name_idx;                 // name index for class
  UnicodeCP name;               // current ref for name
  String Sname;                 // the original name
  String SnewName = null;
  boolean modified = false;     // has the name been altered?

  ClassCP(int nidx)
  { name_idx = nidx; }
  ClassCP(String n)
  { SnewName = Sname = n; }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_CLASS);
    out.writeShort(name.idx);
  }
  void setRefs(CP[] cps)
  {
    name = (UnicodeCP) cps[name_idx];
    Sname = name.val;
  }

  public String toString()
  { return ("Class " + Sname); }

  void resolve(ClassInfo cinfo)
  {
    name = (UnicodeCP) cinfo.addCP(new UnicodeCP(newName()));
  }

  void setNewName(Environment env, ClassInfo cinfo)
  {
    if (!modified)
      {
        modified = true;
        SnewName = env.newClassName(Sname);
      }
  }

                                // Merge identities of
                                // all classes with the same name
  public int hashCode()
  { return Sname.hashCode(); }

  public boolean equals(Object arg)
  {
    return ((arg instanceof ClassCP) &&
            ((ClassCP)arg).newName().equals(newName()));
  }
  String newName()
  {
    if (SnewName != null)
      return SnewName;
    return Sname;
  }
  String originalName()
  { return Sname; }
}
