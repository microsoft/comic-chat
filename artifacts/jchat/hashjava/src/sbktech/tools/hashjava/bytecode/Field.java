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

/**
 * A Field is a reference to a field declared in a class.
 * @author $Author: kbs $
 * @version $Revision: 1.2 $
 */

class Field implements VMConstants, Modifiable
{
//////////////////////////////Public access/////////////////////////////
  /**
   * @return an integer containing the access permitted upon this field.
   * @see VMConstants
   */
  public int permissions()
  { return var_acc; }

  /**
   * @return the original name for this field
   */
  public String originalName()
  { return Sname; }
  /**
   * @return the new (possibly renamed) name for field
   */
  public String newName()
  {
    if (SnewName != null)
      { return SnewName; }
    return Sname;
  }

  /**
   * Set the new name for this field.
   * @param name new name for the field.
   */

  public void rename(String name)
  { SnewName = name; }
  public String toString()
  { return ("Defined variable " +name+ " with type "+sig); }


//////////////////////////////Package access////////////////////////////
  Field(DataInput in, CP[] cps, Statistics stats)
       throws IOException
  {
    var_acc = in.readUnsignedShort();
    name = (UnicodeCP) cps[in.readUnsignedShort()];
    sig = (UnicodeCP) cps[in.readUnsignedShort()];
    Sname = name.val;
    Ssig = sig.val;
    attrs = new Attr[in.readUnsignedShort()];
    for (int i=0; i<attrs.length; i++)
      { attrs[i] = Attr.readAttr(in, cps, stats); }
  }
  String getSig()
  { return Ssig; }
  void makeCP(Environment env, ClassInfo cinfo)
  {
    sig = (UnicodeCP) cinfo.addCP(new UnicodeCP(CP.setNewSig(Ssig, env)));
    name = (UnicodeCP) cinfo.addCP(new UnicodeCP(newName()));
    for (int i=0; i<attrs.length; i++)
      { attrs[i].makeCP(env, cinfo); }
  }

  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(var_acc);
    out.writeShort(name.idx);
    out.writeShort(sig.idx);
    out.writeShort(attrs.length);
    for (int i=0; i<attrs.length; i++)
      { attrs[i].write(out); }
  }

//////////////////////////////Private access////////////////////////////
  private int var_acc;
  private Attr attrs[];
  private UnicodeCP name, sig;
  private String Sname, Ssig;
  private String SnewName;
}
