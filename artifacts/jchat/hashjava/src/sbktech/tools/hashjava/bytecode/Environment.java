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

/**
 * This is the class  used to start obfuscating bytecode.
 * It contains all the information needed to process a set of class files,
 * and is instantiated to start the obfuscation process.
 *
 * <p>To use this class, first implement the BytecodeFactory
 * and Statistics interfaces. These are interfaces
 * used to read or write bytecode (for BytecodeFactory)
 * and generate debugging information (for Statistics)
 *
 * <p>Next, implement an Obfuscator interface, used to
 * rename symbols defined in the bytecode streams.
 *
 * <p>Now create an instance of this class to start the process.
 * The high level sequence looks as follows
 * <hr>
 * <pre>
 *   Environment env = new Environment(myBytecodeFactory,
 *                                     myObfuscator,
 *                                     myStatistics);
 *   while (&lt;there are more classes to add&gt;)
 *     {
 *       env.addClass(&lt;DataInput for the bytecode for class&gt;);
 *     }
 *   env.obfuscate();
 *   env.dump();
 * </pre>
 * <hr>
 * @see BytecodeFactory
 * @see Obfuscator
 * @see Statistics
 *
 * @author $Author: kbs $
 * @version $Revision: 1.7 $
 */

public class Environment
{

////////////////////////Public access///////////////////////////////////

  /**
   * Use this to start adding classes to the environment for obfuscation.
   * The method will also add all the superclasses and interfaces
   * implemented by this class, reading them through the
   * BytecodeFactory interface. Adding a class twice causes no harm,
   * except to slow down the program a little.
   * @param in DataInput that contains the definition of the bytecode
   *           to be added.
   * @param resolveFully if true, this will also attempt to add any
   *           classes <em>referenced</em> by this class, instead of
   *           only the superclasses and interfaces implemented by
   *           this class. Exceptions are classes in the <tt>java.*</tt>
   *           package. These will never be resolved fully to save loading
   *           time.
   * @exception IOException if the DataInput part of things fail
   * @exception BytecodeFormatException if the bytecode itself is hosed.
   * @see BytecodeFactory
   */

  public void addClass(DataInput in, boolean resolveFully)
       throws IOException, BytecodeFormatException
  {
    ClassInfo cinfo;
    try { cinfo = new ClassInfo(in, this); }
    catch (IOException ex)
      {
        stats.info("Failed to read bytecode " + ex.toString());
        return;
      }
    catch(BytecodeFormatException bex)
      {
        stats.info("Invalid bytecode " + bex.toString());
        return;
      }
    String curName = cinfo.originalName();

                                // Store the class by its original name
    if (classes.get(curName) == null)
      { classes.put(curName, cinfo); }
    else
      {
        stats.verboseMessage
          ("Ignoring " + curName + " because it has already been added");
        return;
      }

                                // Add the dependent classes.
    String deps[] = cinfo.getDependencies(resolveFully);
    for (int i=0; i<deps.length; i++)
      {
        String s = deps[i];
        if (classes.get(s) == null)
          {
            stats.verboseMessage
              (curName + " ==>loads==> " + s);
            DataInput ninput = bfactory.readClass(s);
            if (ninput != null)
              { addClass(ninput, resolveFully); }
            else
              {
                stats.verboseMessage
                  ("Warning: didn't get data stream for " + s);
              }
          }
      }
  }

  /**
   * create a new environment to start off the whole process.
   * @param bfactory This is a class to let the environment
   *               read and write Class streams when it is necessary
   * @param obfuscator This is the only way to actually alter the names
   *               of symbols encountered in the process.
   * @param stats    This is called at various points in the manipulation
   *               to entertain the user.
   */
  public Environment (BytecodeFactory bfactory,
                      Obfuscator obfuscator,
                      Statistics stats)
  {
    this.bfactory = bfactory;
    this.obfuscator = obfuscator;
    this.stats = stats;
    symtab = new Hashtable();
    classes = new Hashtable();
  }


  /**
   * Start the obfuscating process. This should be called only after
   * <em>all</em> the classes that are involved have been
   * added to the Environment.
   * Is also hands off each class that was added to the environment to the
   * obfuscator.
   * @see #addClass
   * @see Obfuscator#obfuscate
   */
  public void obfuscate()
  {
                                // First, resolve the defined methods
                                // in all classes.
    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        cinfo.resolveDefinitions();
      }
                                // Next, pass responsibility to pick new
                                // names to the interfaces, if any

    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        cinfo.puntInterfaces();
      }

                                // Pass 3: merge punted interfaces
    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        cinfo.mergeInterfaces();
      }

                                // Pass 4: compile dependency lists
                                // for interfaces
    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        cinfo.notifyInterfaces();
      }
    
                                // Pass 5: generate the method arrays
    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        cinfo.makeArrays();
      }


                                // Now, hand off the classes to the obfuscator
    for (Enumeration e = classes.elements();
         e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo)(e.nextElement());
        obfuscator.obfuscate(cinfo);
      }
  }

  /**
   * Write out the contents of the current environment into a set of
   * classes.
   */
  public void dump ()
        throws IOException
  {
    for (Enumeration e = classes.elements(); e.hasMoreElements();)
      {
        ClassInfo cinfo = (ClassInfo) e.nextElement();
        DataOutput out = bfactory.writeClass(cinfo.newName());
        if (out != null)
          {
            stats.verboseMessage("Dumping out " + cinfo.newName());
            cinfo.write(out);
            // Workaround to deal with FileOutputStreams that might
            // be left open
            if (out instanceof OutputStream)
              { ((OutputStream)out).close(); }
          }
      }
  }

  /**
   * Control how line number debugging information is written out. The
   * default setting is DEBUG_REMOVE.
   * @param val is one of DEBUG_REMOVE, DEBUG_CORRUPT or DEBUG_RETAIN
   */
  public void setLineNumberDebugging(int val)
  { lineNumberDebugging = val; }

  /**
   * Control how local variable table information is written out. The default
   * setting is DEBUG_REMOVE
   * @param val is one of DEBUG_REMOVE, DEBUG_CORRUPT or DEBUG_RETAIN
   */
  public void setLocalVariableDebugging(int val)
  { localVariableDebugging = val; }

  /**
   * Control how source file debugging information is written out. The
   * default setting is DEBUG_REMOVE
   * @param val is one of DEBUG_REMOVE, DEBUG_CORRUPT or DEBUG_RETAIN
   */
  public void setSourceFileDebugging(int val)
  { sourceDebugging = val; }

  /**
   * Constant indicating if debug information should be removed.
   */
  public final static int DEBUG_REMOVE=0;
  /**
   * Constant indicating if debug information should be altered in ways
   * that tries to break disassemblers/debuggers/decompilers
   */
  public final static int DEBUG_CORRUPT=1;
  /**
   * Constant indicating if original debug information should be retained.
   */
  public final static int DEBUG_RETAIN=2;


//////////////////////////////Package Access////////////////////////////

  Statistics stats;
  int sourceDebugging = 0, lineNumberDebugging = 0, localVariableDebugging = 0;

  // Return the (possibly altered) name for a class

  String newClassName(String cname)
  {
                                // First check if we have an
                                // array of classes
    if (cname.charAt(0) == '[')
      {
        return CP.setNewSig(cname, this);
      }

                                // This is a regular class name
    ClassInfo cinfo = (ClassInfo)(classes.get(cname));
    if (cinfo == null)
      { return cname; }
    return cinfo.newName();
  }

  // Return the (possibly altered) name for a field in a class
  String newFieldName(String cname, NameTypeCP ntcp)
  {
    ClassInfo cinfo = (ClassInfo)(classes.get(cname));
    if (cinfo == null)
      { return ntcp.Sname; }

    return cinfo.newFieldName(ntcp);
  }

  // Return the (possible altered) name for a method in a class
  String newMethodName(String cname, NameTypeCP ntcp)
  {
    ClassInfo cinfo = (ClassInfo)(classes.get(cname));
    if (cinfo == null)
      { return ntcp.Sname; }

    return cinfo.newMethodName(ntcp);
  }

  // Whats the corresponding ClassInfo?
  ClassInfo forName(String clname)
  {
    ClassInfo ret = (ClassInfo)(classes.get(clname));
    if (ret == null)
      { stats.info("Hoops, " + clname + " wasn't found"); }
    return ret;
  }

//////////////////////////////Private Section///////////////////////////
  private BytecodeFactory bfactory;
  private Obfuscator obfuscator;
  private Hashtable classes;    // all classes added so far
  private Hashtable symtab;     // all symbols added so far
}
