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
 * A Method is a reference to a method defined in a class. You just
 * get to examine and alter its name through the Modifiable interface
 * @author $Author: kbs $
 * @version $Revision: 1.4 $
 */

public class Method implements VMConstants, Modifiable
{

//////////////////////////////Public access/////////////////////////////
  /**
   * @return the access permissions for this method
   * @see VMConstants
   */
  public int permissions()
  { return acc; }

  /**
   * @return the method's original name
   */
  public String originalName()
  { return Sname; }

  /**
   * @return the method's new (possibly modified) name
   */
  public String newName()
  {
    if (SnewName == null)
      return Sname;
    return SnewName;
  }

  /**
   * set the new name for this method
   * @param name new name for method
   */
  public void rename(String name)
  { SnewName = name; }

  /**
   * @return true if the Method being compared has the same
   *         name and signature
   */
  public boolean equals(Object x)
  {
    if (x instanceof Method)
      {
        Method target = (Method) x;
        return (target.name.val.equals(name.val) &&
                target.sig.val.equals(sig.val));
      }
    return false;
  }

  public String toString()
  {
    return ("Defined method " + name + " with type " + sig);
  }

//////////////////////////////Package access////////////////////////////
  Method(DataInput in, CP[] cps, Statistics stats)
       throws IOException
  {
    acc = in.readUnsignedShort();
    name = (UnicodeCP) cps[in.readUnsignedShort()];
    sig = (UnicodeCP) cps[in.readUnsignedShort()];
    Ssig = sig.val;
    Sname = name.val;
                                // Make up a dummy name for name/type
                                // for later use.
    ntype = new NameTypeCP(Sname, Ssig);
    attrs = new Attr[in.readUnsignedShort()];
    for (int i=0; i<attrs.length; i++)
      { attrs[i] = Attr.readAttr(in, cps, stats); }
  }

  String getSig()
  { return Ssig; }

  NameTypeCP getNtcp()
  { return ntype; }

  void makeCP(Environment env, ClassInfo cinfo)
  {
                                // The new name is checked from the parent
                                // so that any alterations by a superclass
                                // are pulled in
    name = (UnicodeCP)(cinfo.addCP(new UnicodeCP(cinfo.newMethodName(ntype))));

    sig = (UnicodeCP)(cinfo.addCP(new UnicodeCP(CP.setNewSig(Ssig, env))));
    for (int i=0; i<attrs.length; i++)
      { attrs[i].makeCP(env, cinfo); }
  }

  void write(DataOutput out)
       throws IOException
  {
    out.writeShort(acc);
    out.writeShort(name.idx);
    out.writeShort(sig.idx);

    out.writeShort(attrs.length);
    for (int i=0; i<attrs.length; i++)
      { attrs[i].write(out); }
  }

//////////////////////////////Private access////////////////////////////

  private int acc;
  private UnicodeCP name, sig;
  private NameTypeCP ntype;
  private String Sname, Ssig, SnewName;
  private Attr attrs[];
}
