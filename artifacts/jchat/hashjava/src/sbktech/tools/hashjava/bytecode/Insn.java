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

// Wrapper around a single instruction
// $Author: kbs $
// $Revision: 1.2 $

package sbktech.tools.hashjava.bytecode;
import java.io.*;

class Insn implements VMConstants
{
  int opc;
  int pc;
  InsnOperand operand;

  Insn(DataInput in, int pc, CP cps[])
       throws IOException
  {
    boolean wide_mode = false;
    this.pc = pc;

    opc = in.readUnsignedByte();
    if (opc == opc_wide)
      {
        wide_mode = true;
        opc = in.readUnsignedByte();
      }
    switch(opc)
      {
      case opc_bipush:
        operand = new ByteOperand(in);
        break;
      case opc_sipush:
        operand = new ShortOperand(in);
        break;
      case opc_newarray:
        operand = new UnsignedByteOperand(in);
        break;

      case opc_ret:
      case opc_iload:
      case opc_lload:
      case opc_fload:
      case opc_dload:
      case opc_aload:
      case opc_istore:
      case opc_lstore:
      case opc_fstore:
      case opc_dstore:
      case opc_astore:
        operand = new UnsignedByteWideOperand(in, wide_mode);
        break;
      case opc_jsr:
      case opc_goto:
      case opc_if_acmpne:
      case opc_if_acmpeq:
      case opc_if_icmpge:
      case opc_if_icmple:
      case opc_if_icmpgt:
      case opc_if_icmplt:
      case opc_if_icmpne:
      case opc_if_icmpeq:
      case opc_ifge:
      case opc_ifgt:
      case opc_ifne:
      case opc_ifle:
      case opc_iflt:
      case opc_ifeq:
      case opc_ifnull:
      case opc_ifnonnull:
        operand = new LabelOperand(in, wide_mode, this);
        break;
      case opc_goto_w:
      case opc_jsr_w:
        operand = new LabelOperand(in, true, this);
        break;
      case opc_anewarray:
      case opc_invokenonvirtual:
      case opc_invokestatic:
      case opc_invokevirtual:
      case opc_new:
      case opc_checkcast:
      case opc_instanceof:
      case opc_getstatic:
      case opc_putstatic:
      case opc_getfield:
      case opc_putfield:
        operand = new CPOperand(in, true, cps);
        break;

      case opc_ldc:
        operand = new LdcOperand(in, false, this, cps);
        break;

      case opc_iinc:
        operand = new IincOperand(in, wide_mode);
        break;

      case opc_tableswitch:
        operand = new TableswitchOperand(in, this);
        break;

      case opc_lookupswitch:
        operand = new LookupswitchOperand(in, this);
        break;

      case opc_multianewarray:
        operand = new MultiarrayOperand(in, cps);
        break;

      case opc_invokeinterface:
        operand = new InvokeinterfaceOperand(in, cps);
        break;

      case opc_ldc2_w:
      case opc_ldc_w:
        operand = new LdcOperand(in, true, this, cps);
        break;
      default:
        if (opcLengths[opc] == 1)
          {
            operand = null;
          }
        else
          {
            System.err.println("Woops, unknown opcode " + opc);
            System.exit(1);
          }
        break;
      }
  }
  int size()
  {
    if (operand == null) return 1;
    return (1 + operand.size());
  }
  void makeCP(Environment env, ClassInfo cinfo)
  {
    if (operand != null)
      { operand.makeCP(env, cinfo); }
  }
  void write(DataOutput out)
       throws IOException
  {
    if (operand != null) operand.writePrefix(out);
    out.writeByte(opc);
    if (operand != null)
      { operand.write(out); }
  }
  public String toString()
  {
    String s = pc + ": " + opcNames[opc];
    if (operand != null)
      s += " " + operand.toString();
    return s;
  }
}
