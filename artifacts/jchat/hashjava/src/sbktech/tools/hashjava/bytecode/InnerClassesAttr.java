/*
 * Copyright (C) 1997 KB Sriram
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
import java.util.*;

class InnerClassesAttr extends Attr
{
  ClassCP inner[], outer[];
  UnicodeCP nameS[];
  short accessFlg[];

  InnerClassesAttr(DataInput in, UnicodeCP n, int size, CP cps[])
       throws IOException
  {
    name = n;
    Sname = n.val;
    this.mysize = size;
    int tlen = in.readUnsignedShort();
    //    System.out.println(tlen + " entries");
    inner = new ClassCP[tlen]; outer = new ClassCP[tlen];
    nameS = new UnicodeCP[tlen]; accessFlg = new short[tlen];
    for (int i=0; i<tlen; i++)
      {
        inner[i] = (ClassCP) cps[in.readUnsignedShort()];
        outer[i] = (ClassCP) cps[in.readUnsignedShort()];
        nameS[i] = (UnicodeCP)cps[in.readUnsignedShort()];
        accessFlg[i] = in.readShort();
        /*
        System.out.print("["+i+"]");
        if (inner[i]!= null)
          { System.out.print(" inner:"+inner[i]); }
        if (outer[i]!=null)
          { System.out.print(" outer:"+outer[i]); }
        if (nameS[i] != null)
          { System.out.print(" name:" + nameS[i]); }
        System.out.println();
        */
      }
    /*
    for (int i=0; i<cps.length; i++)
      {
        if (cps[i] != null)
          { System.out.println("cp["+i+"] " + cps[i]); }
      }
      */
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    super.makeCP(env, cinfo);
    for (int i=0; i<nameS.length; i++)
      {
        ClassInfo ci = env.forName(inner[i].originalName());
        String newInnerName = ci.newInnerName();
        inner[i].setNewName(env, cinfo);
        inner[i] = (ClassCP) cinfo.addCP(inner[i]);
        if (outer[i] != null)
          {
            outer[i].setNewName(env, cinfo);
            outer[i] = (ClassCP) cinfo.addCP(outer[i]);
          }
        if (newInnerName != null)
          { nameS[i] = (UnicodeCP) cinfo.addCP(new UnicodeCP(newInnerName)); }
        else
          { nameS[i] = null; }
      }
  }

  int size()
  { return 6 + 2 + 8*nameS.length;  }
  void write(DataOutput out)
       throws IOException
  {
    super.write(out);
    out.writeShort(nameS.length);
    for (int i=0; i<nameS.length; i++)
      {
        out.writeShort(inner[i].idx);
        if (outer[i]!=null)
          { out.writeShort(outer[i].idx); }
        else
          { out.writeShort(0); }
        if (nameS[i] != null)
          { out.writeShort(nameS[i].idx); }
        else
          { out.writeShort(0); }
        out.writeShort(accessFlg[i]);
      }
  }
}
