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

class CodeAttr extends Attr
{
  int stack_size, num_locals;
  Insn insns[];
  Catchtable ctable;
  Attr attrs[], extra[];
  Statistics stats;
  Hashtable indexByOldPc = new Hashtable();


  CodeAttr(DataInput in, UnicodeCP n, int sz, CP cps[], Statistics stats)
       throws IOException
  {
    name = n;
    Sname = n.val;
    mysize = sz;
    this.stats = stats;
    stack_size = in.readUnsignedShort();
    num_locals = in.readUnsignedShort();

    int size = in.readInt();
    Vector code = new Vector();

    int pc = 0;
    while (pc < size)
      {
        Insn insn = new Insn(in, pc, cps);
        indexByOldPc.put(new Integer(pc), insn);
        pc += insn.size();
        code.addElement(insn);
      }
    insns = new Insn[code.size()];
    code.copyInto(insns);
                                // Now fix up control transfers
    for (int i=0; i<insns.length; i++)
      {
        if (insns[i].operand != null)
          { insns[i].operand.fixup(this); }
      }

                                // read catch table
                                // read extra attributes
    ctable = new Catchtable(in, cps);
                                // and fix up *its* control transfers
    ctable.fixup(this);
    attrs = new Attr[in.readUnsignedShort()];
    for (int i=0; i<attrs.length; i++)
      { attrs[i] = Attr.readAttr(in, cps, stats); }
  }

  void makeCP(Environment env, ClassInfo cinfo)
  {
    super.makeCP(env, cinfo);

    for (int i=0; i<insns.length; i++)
      { insns[i].makeCP(env, cinfo); }
    ctable.makeCP(env, cinfo);

    boolean sawvar = false;
    boolean sawline = false;

    for (int i=0; i<attrs.length; i++)
      {
        int mode = env.DEBUG_RETAIN;
        if (attrs[i] instanceof LineNumberTableAttr)
          { mode = env.lineNumberDebugging; sawline = true; }
        else if (attrs[i] instanceof LocalVariableTableAttr)
          {
            if (cinfo.varCorrupted())
              { mode = env.DEBUG_REMOVE; }
            else
              {
                cinfo.setVarCorrupted(true);
                mode = env.DEBUG_CORRUPT;
                sawvar = true;
              }
          }
        switch(mode)
          {
          case env.DEBUG_REMOVE:
            attrs[i].include(false);
            break;
          case env.DEBUG_CORRUPT:
            attrs[i].corrupt();
            break;
          default:
            break;
          }
        if (attrs[i].includeP())
          { attrs[i].makeCP(env, cinfo); }
      }

    int nextra = 0;
    if (!sawvar && (env.localVariableDebugging == env.DEBUG_CORRUPT))
      {
        if (!cinfo.varCorrupted())
          {
            nextra++;
            cinfo.setVarCorrupted(true);
          }
        else
          { sawvar = true; }
      }
    if (!sawline && (env.lineNumberDebugging == env.DEBUG_CORRUPT))
      { nextra++; }

    extra = new Attr[nextra];
    nextra = 0;
    if (!sawvar && (env.localVariableDebugging == env.DEBUG_CORRUPT))
      {
        extra[nextra] = new LocalVariableTableAttr();
        extra[nextra].corrupt();
        extra[nextra++].makeCP(env, cinfo);
      }
    if (!sawline && (env.lineNumberDebugging == env.DEBUG_CORRUPT))
      {
        extra[nextra] = new LineNumberTableAttr();
        extra[nextra].makeCP(env, cinfo);
      }
  }

  Insn getInsn(int pc)
  {
    Integer key = new Integer(pc);
    Insn ret = (Insn)(indexByOldPc.get(key));
    if (ret == null)
      {
        throw new RuntimeException("Woops, no pc at " + pc);
      }
    return ret;
  }

  private int determineCodeSize()
  {
    int code_size = 0;
    for (int i=0; i<insns.length; i++)
      {
        insns[i].pc = code_size;
        code_size += insns[i].size();
      }
    return code_size;
  }
    
  private int determineRestSize()
  {
    int total_size = 10;
    total_size +=  ctable.size();
    for (int i=0; i<attrs.length; i++)
      {
        if (attrs[i].includeP())
          { total_size += attrs[i].size(); }
      }
    for (int i=0; i<extra.length; i++)
      { total_size += extra[i].size(); }
    return total_size;
  }

  int size()
  {
    System.err.println("Warning: size called from CodeAttr");
    return 6 + determineCodeSize() + determineRestSize();
  }

  void write(DataOutput out)
       throws IOException
  {
                                // First determine total size,
    int code_size = determineCodeSize();
    int total_size = code_size + determineRestSize();
                                // Now ready to write things out
    out.writeShort(name.idx);
    out.writeInt(total_size);
    out.writeShort(stack_size);
    out.writeShort(num_locals);
    out.writeInt(code_size);

    for (int i=0; i<insns.length; i++)
      { insns[i].write(out); }

    ctable.write(out);
    int alen = extra.length;
    for (int i=0; i<attrs.length; i++)
      {
        if (attrs[i].includeP())
          { alen++; }
      }
    out.writeShort(alen);
    for (int i=0; i<attrs.length; i++)
      {
        if (attrs[i].includeP())
          { attrs[i].write(out); }
      }
    for (int i=0; i<extra.length; i++)
      { extra[i].write(out); }
  }
}
