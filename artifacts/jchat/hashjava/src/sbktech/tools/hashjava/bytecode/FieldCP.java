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

class FieldCP extends CP
{
  int cname_idx;
  int name_type_idx;
  ClassCP cname;
  NameTypeCP name_type;
  String Scname, Sname, Ssig;
  boolean modified = false;

  void resolve(ClassInfo cinfo)
  {
    cname = (ClassCP) cinfo.addCP(new ClassCP(Scname));
    name_type = (NameTypeCP) cinfo.addCP(new NameTypeCP(Sname, Ssig));
  }

  void setNewName(Environment env, ClassInfo cinfo)
  {
    if (!modified)
      {
        String tmp;
        tmp =  env.newFieldName(Scname, name_type);
        if (tmp == null)
          {
            env.stats.info
              ("Warning: " + cinfo + " uses field " + Sname + " in class "+
               Scname);
            env.stats.info
              ("         but "+Sname+" was not found. This may be because there are");
            env.stats.info
              ("         some classes that havent been recompiled after changes.");
            env.stats.info
              ("         Please recompile all your source code to prevent errors.");
          }
        else
          { Sname = tmp; }
        Scname = env.newClassName(Scname);
        Ssig = setNewSig(Ssig, env);
        modified = true;
      }
  }

  public int hashCode()
  {
    return ((Scname.hashCode() << 16) |
            (Sname.hashCode() & 0xff00) |
            (Ssig.hashCode() & 0xff));
  }
  public boolean equals(Object arg)
  {
    return ((arg instanceof FieldCP) &&
            ((FieldCP)arg).Scname.equals(Scname) &&
            ((FieldCP)arg).Sname.equals(Sname) &&
            ((FieldCP)arg).Ssig.equals(Ssig));
  }

  FieldCP(int c, int n)
  { cname_idx = c; name_type_idx = n; }
  FieldCP(String c, String n, String s)
  { Scname = c; Sname = n; Ssig = s; }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_FIELD);
    out.writeShort(cname.idx);
    out.writeShort(name_type.idx);
  }
  void setRefs(CP[] cps)
  {
    cname = (ClassCP) cps[cname_idx];
    Scname = ((UnicodeCP)(cps[cname.name_idx])).val;
    name_type = (NameTypeCP) cps[name_type_idx];
    Sname = ((UnicodeCP)(cps[name_type.name_idx])).val;
    Ssig = ((UnicodeCP)(cps[name_type.sig_idx])).val;
  }
  public String toString()
  { return ("Field from " +Scname+ " is " + Sname + " type " + Ssig); }
}

