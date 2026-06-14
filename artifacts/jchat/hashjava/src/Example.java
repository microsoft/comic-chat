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

/**
 * This is just supposed to be a proof of concept implementation
 * of using the library. It obfuscates all the .class files
 * it finds under a directory, writing them out into another directory.
 *
 * Usage: java Example <indir> <outdir>
 */

import java.io.*;
import java.util.*;
import sbktech.tools.hashjava.bytecode.*;
import sbktech.tools.hashjava.util.ZipReader;

public class Example
implements Statistics, BytecodeFactory, Obfuscator, VMConstants
{
  //////////////Statistics interface//////////////////

  public void info(String s)
  { System.out.println("[info] " + s); }

  public void verboseMessage(String s)
  { System.out.println("[debug] " + s); }


  /////////////Obfuscator interface///////////////////////
  //This is a silly algorithm, to test things out.
  //It appends a number to everything not in the java package,
  //and avoids renaming any class which contains a static main()

  private int class_idx = 0;
  private int field_idx = 0;
  private int method_idx = 0;

  public void obfuscate(ClassInfo cinfo)
  {
    boolean hasMain = false;

    String curClass = cinfo.originalName();

                                // Dont bother with java/ packages.
    if (curClass.startsWith("java/")) return;

                                // Rename every field in the class
    Modifiable m[] = cinfo.definedFields();
    for (int i=0; i<m.length; i++)
      { m[i].rename(m[i].originalName() + field_idx++); }


                                // And every method that isn't a
                                // constructor or the static class
                                // initializer or "main" or implements
                                // a method from "java/"
    m = cinfo.definedMethods();
    for (int i=0; i<m.length; i++)
      {
        String mname = m[i].originalName();

        if (!("<init>".equals(mname)) &&
            !("<clinit>".equals(mname)))
          {
            if ("main".equals(mname) &&
                ((m[i].permissions() & ACC_STATIC) != 0))
              { hasMain = true; } // Check if this class has a static main()
            else
              {
                if (((cinfo.permissions() & ACC_INTERFACE) == 0) ||
                    !implementsJavaMethod(cinfo, (Method) m[i]))
                  { m[i].rename(mname + method_idx++); }
              }
          }
      }
                                // Add a copyright string as an attribute

    String embed = "This is some copyright string";
    byte data[] = new byte[embed.length()];
    embed.getBytes(0, data.length, data, 0);
    cinfo.addAttribute("\rRandom Copyright Notice", data);


                                // Rename the class too if there isn't a
                                // static main() in it.
    if (!hasMain)
      {
        cinfo.rename(curClass + class_idx++);
      }
  }

  ////////////BytecodeFactory implementation/////////////

  public DataInput readClass(String classname)
       throws IOException
  {
                                // Search for the class under the
                                // source directory if it isn't a
                                // java/ class. Otherwise, search in
                                // the zip file

    DataInput ret=null;
    if (!classname.startsWith("java/"))
      {
        classname = srcDir + classname;
      }
    else
      {
        return coreClasses.getFile(classname + ".class");
      }

    classname = (classname.replace('/', File.separatorChar)) + ".class";
    try
      {
        ret =
          new DataInputStream
          (new FileInputStream
           (classname));
      }
    catch(FileNotFoundException ex)
      { return null; }

    return ret;
  }

  public DataOutput writeClass(String classname)
       throws IOException
  {

                                // Ignore writing out all the base
                                // classes, since we never modify them,
                                // and no references from them can be
                                // modified.

    if (classname.startsWith("java/"))
      return null;              // returning null tells the environment
                                // to ignore writing out this file.


                                // Otherwise, write out into the target
                                // directory
    classname =
      targetDir + (classname.replace('/',File.separatorChar)) + ".class";
    String dirname = classname.substring
      (0, classname.lastIndexOf(File.separatorChar));
    File d = new File(dirname);
    d.mkdirs();
    DataOutput ret =
      new DataOutputStream
      (new FileOutputStream
       (classname));
    return ret;
  }

  private static String targetDir;
  private static String srcDir;
  private static ZipReader coreClasses;
  // Convenience function to verify that a particular method
  // from an interface is not implemented in the "java.*" hierarchy
  private static boolean implementsJavaMethod(ClassInfo cinfo, Method m)
  {
    Hashtable set = cinfo.affects(m);
    if (set == null)
      return false;
    for (Enumeration en=set.elements(); en.hasMoreElements();)
      {
        ClassInfo c = (ClassInfo)(en.nextElement());
        if (c.originalName().startsWith("java/"))
          { return true; }
      }
    return false;
  }

  public static void main(String argv[])
    throws IOException, BytecodeFormatException, InvalidDirectoryError
  {
    if (argv.length != 2)
      {
        System.err.println
          ("Usage: java Example <in directory> <out directory>");
        System.exit(1);
      }

                                // Load up the zip file
                                // The zip reader is a convenient
                                // way to read data from zip files.
    coreClasses = new ZipReader("\\java\\lib\\classes.zip");

    srcDir = argv[0];
    targetDir = argv[1];
    if (!(targetDir.endsWith(File.separator)))
      { targetDir += File.separator; }
    if (!(srcDir.endsWith(File.separator)))
      { srcDir += File.separator; }

    DirTreeGenerator inDir = new DirTreeGenerator(argv[0]);

    Example t = new Example();
    Environment env = new Environment(t, t, t);

    // Make life a bit more complicated when putting out debugging symbols.

    env.setSourceFileDebugging(env.DEBUG_CORRUPT);
    env.setLineNumberDebugging(env.DEBUG_REMOVE);
    env.setLocalVariableDebugging(env.DEBUG_CORRUPT);

    for (Enumeration e = inDir.list(); e.hasMoreElements();)
      {
        String cname = (String)(e.nextElement());
        if (cname.endsWith(".class"))
          {
            env.addClass
              (new DataInputStream(new FileInputStream(cname)), false);
          }
      }
    env.obfuscate();
    env.dump();
  }
}
