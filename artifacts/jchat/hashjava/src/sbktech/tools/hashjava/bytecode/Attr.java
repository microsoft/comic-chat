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
import java.util.*;

abstract class Attr
{

  /**
   * Control whether or not to include this attribute when writing
   * out the file. The default setting is to write out the attribute.
   * @param b if true, include this attribute when writing out.
   */
  void include(boolean b)
  { writeOut = b; }

  /**
   * Corrupt the attribute in some way. This is typically appropriate
   * to do only for debugging attributes.
   */
  void corrupt()
  { }

  public String toString()
  { return "Attribute " + name.val; }


  static Attr readAttr(DataInput in, CP[] cps, Statistics stats)
       throws IOException
  {
    UnicodeCP name = (UnicodeCP) cps[in.readUnsignedShort()];
    int size = in.readInt();
    String tmp = name.val;
    if (tmp.equals("ConstantValue"))
      {
        return (new ConstAttr(in, name, size, cps));
      }
    else if (tmp.equals("SourceFile"))
      {
        return (new SourceFileAttr(in, name, size, cps));
      }
    else if (tmp.equals("Exceptions"))
      {
        return (new ExceptionsAttr(in, name, size, cps));
      }
    else if (tmp.equals("Code"))
      {
        return (new CodeAttr(in, name, size, cps, stats));
      }
    else if (tmp.equals("LocalVariableTable"))
      {
        return (new LocalVariableTableAttr(in, name, size, cps));
      }
    else if (tmp.equals("LineNumberTable"))
      {
        return (new LineNumberTableAttr(in, name, size));
      }
    else if (tmp.equals("InnerClasses"))
      {
        return (new InnerClassesAttr(in, name, size, cps));
      }
    else if ((tmp.equals("Synthetic")) ||
             (tmp.equals("Deprecated")))
      {
        return (new BasicAttr(in, name, size));
      }
    else
      {
        stats.verboseMessage("Warning: unknown attribute named>>");
        stats.verboseMessage(name.val);
        return (new BasicAttr(in, name, size));
      }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (writeOut)
      { name = (UnicodeCP) cinfo.addCP(name); }
  }

  void write(DataOutput out)
       throws IOException
  {
    if (writeOut)
      {
        out.writeShort(name.idx);
        out.writeInt(mysize);
      }
  }

  boolean includeP()
  { return writeOut; }

  abstract int size();

//////////////////////////////Subclass access/////////////////////////
  protected UnicodeCP name;
  protected String Sname;
  protected int mysize;
  protected boolean writeOut = true;
}
