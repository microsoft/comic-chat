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

class MethodCP extends CP
{
  int cname_idx;
  int name_type_idx;
  ClassCP cname;
  NameTypeCP name_type;
  String Scname, Sname, Ssig;
  boolean modified = false;

  MethodCP(int c, int n)
  { cname_idx = c; name_type_idx = n; }
  MethodCP(String c, String n, String s)
  { Scname = c; Sname = n; Ssig = s; }
  public void write(DataOutput out)
       throws IOException
  {
    out.writeByte(CONSTANT_METHOD);
    out.writeShort(cname.idx);
    out.writeShort(name_type.idx);
  }
  void setRefs(CP[] cps)
  {
    cname = (ClassCP)(cps[cname_idx]);
    Scname = ((UnicodeCP)(cps[cname.name_idx])).val;
    name_type = (NameTypeCP)(cps[name_type_idx]);
    Sname = ((UnicodeCP)(cps[name_type.name_idx])).val;
    Ssig  = ((UnicodeCP)(cps[name_type.sig_idx])).val;
  }

  public String toString()
  {
    return ("Method from class " + Scname + " is " +Sname+ " type "+Ssig);
  }
  public int hashCode()
  {
    return ((Scname.hashCode() << 16) |
            (Sname.hashCode() & 0xff00) |
            (Ssig.hashCode() & 0xff));
  }
  public boolean equals(Object arg)
  {
    return ((arg instanceof MethodCP) &&
            ((MethodCP)arg).Scname.equals(Scname) &&
            ((MethodCP)arg).Sname.equals(Sname) &&
            ((MethodCP)arg).Ssig.equals(Ssig));
  }
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

        tmp =  env.newMethodName(Scname, name_type);
        if (tmp == null)
          {
            env.stats.info
              ("Warning: did not find method " + Sname +
               " in class " + Scname);
            env.stats.info
              ("This usually means that some of your classes are not fully compiled.");
            env.stats.info
              ("Please try recompiling your entire source code before obfuscating.");
            throw new InterfaceMatchFailedException(Sname);
          }
        else
          { Sname = tmp; }

        Scname = env.newClassName(Scname);
        Ssig = setNewSig(Ssig, env);
        modified = true;
      }
  }
}
