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

// Hold basic entry for symbol names
//

package sbktech.tools.hashjava.bytecode;
import java.io.*;

abstract class CP implements VMConstants, Writable
{
  int idx;                      // location for this
                                // entry in the CP


  // This is a way to set up the local references to the
  // other CP's used in defining a CP
  abstract void setRefs(CP[] cps);

  // This is a way to enable CP's to add dependent CPs
  // to the class if necessary
  abstract void resolve(ClassInfo cinfo);

  // And set things to the new name for the CP
  abstract void setNewName(Environment env, ClassInfo cinfo);

  static CP readCP(DataInput in) throws IOException
  {
    CP ret = null;

    int tag = in.readUnsignedByte();
    switch(tag)
      {
      case CONSTANT_CLASS:
        ret = new ClassCP(in.readUnsignedShort());
        break;
      case CONSTANT_METHOD:
        ret = new MethodCP(in.readUnsignedShort(), in.readUnsignedShort());
        break;
      case CONSTANT_FIELD:
        ret = new FieldCP(in.readUnsignedShort(), in.readUnsignedShort());
        break;
      case CONSTANT_INTERFACEMETHOD:
        ret = new InterfaceCP(in.readUnsignedShort(), in.readUnsignedShort());
        break;
      case CONSTANT_STRING:
        ret = new StringCP(in.readUnsignedShort());
        break;
      case CONSTANT_INTEGER:
        ret = new IntegerCP(in.readInt());
        break;
      case CONSTANT_FLOAT:
        ret = new FloatCP(in.readFloat());
        break;
      case CONSTANT_LONG:
        ret = new LongCP(in.readLong());
        break;
      case CONSTANT_DOUBLE:
        ret = new DoubleCP(in.readDouble());
        break;
      case CONSTANT_NAMEANDTYPE:
        ret = new NameTypeCP(in.readUnsignedShort(), in.readUnsignedShort());
        break;
      case CONSTANT_UNICODE:
      case CONSTANT_UTF8:
        ret = new UnicodeCP(in.readUTF());
        break;
      default:
        System.err.println("Wups, unknown tag " + (int)tag);
      }
    return ret;
  }
  static String setNewSig(String sig, Environment env)
  {
    StringBuffer buf = new StringBuffer();
    int previdx = 0;
    int curidx = sig.indexOf('L');
    while (curidx != -1)
      {
        buf.append(sig.substring(previdx, curidx+1));
        previdx = curidx;
        curidx = sig.indexOf(';', previdx);
        String oldClassName = sig.substring(previdx+1, curidx);
        String newClassName = env.newClassName(oldClassName);
        buf.append(newClassName);
        previdx = curidx;
        curidx = sig.indexOf('L', previdx);
      }
    buf.append(sig.substring(previdx, sig.length()));
    return new String(buf);
  }
}
