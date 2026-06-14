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

package sbktech.tools.hashjava.batch;

import java.io.*;
import java.util.*;
import sbktech.tools.hashjava.bytecode.*;
import sbktech.tools.hashjava.util.*;

public class BatchObfuscator
  implements Statistics, BytecodeFactory, Obfuscator, VMConstants
{
  //////////////Statistics interface//////////////////

  public void info(String s)
  { System.out.println("[info] " + s); }

  public void verboseMessage(String s)
  { if (debug) System.out.println("[debug] " + s); }


  /////////////Obfuscator interface///////////////////////

  private NameGenerator cnameGen, fnameGen, mnameGen;

  public void obfuscate(ClassInfo cinfo)
  {
    String curClass = cinfo.originalName();

    String key = curClass.replace('/', '.');

    String packagePart=null, classNamePart=null;
    int split = key.lastIndexOf('.');
    if (split > 0)
      {
        packagePart = key.substring(0, split);
        // check if this gets renamed
        packagePart = findIfMapped(packagePart);
        if (packagePart != null)
          {
            packagePart = packagePart.replace('.', '/');
            classNamePart = key.substring(split+1, key.length());
          }
      }

    // Run it by the toplevel filter
    if (allFilter.matches(cinfo.permissions(), key))
      {
        verboseMessage("Filtered entire class " + curClass);
        if (packagePart != null)
          {
            String xx;
            if (packagePart.length() > 0)
              { xx = packagePart + "/" + classNamePart; }
            else
              { xx = classNamePart; }
            cinfo.rename(xx);
            verboseMessage
              ("Also moved " + curClass + " into package " +packagePart);
          }
        return;
      }

    // Map the class name
    String nclname=null;
    if (classFilter.matches(cinfo.permissions(), key))
      {
        nclname = key;
        verboseMessage("Filtered class (name only) " + key);
        if (packagePart != null)
          {
            String xx;
            if (packagePart.length() > 0)
              { xx = packagePart + "/" + classNamePart; }
            else
              { xx = classNamePart; }
            cinfo.rename(xx);
            verboseMessage
              ("Remapped " + curClass + " to package " +packagePart);
          }
      }
    else
      {
        if (loadMapProp != null)
          { nclname = (String) loadMapProp.get(key); }
        if (nclname == null)
          {
            if (packagePart != null)
              {
                if (packagePart.length() > 0)
                  {
                    nclname = packagePart + "/" +
                      cnameGen.checkedName(stopSymbols, key);
                  }
                else
                  { nclname = cnameGen.checkedName(stopSymbols, key); }
              }
            else
              {
                if (split > 0)
                  {
                    nclname =
                      curClass.substring(0, split)+"/"+
                      cnameGen.checkedName(stopSymbols, key);
                  }
                else
                  { nclname = cnameGen.checkedName(stopSymbols, key); }
              }
          }
        else
          { // obtained name from load map, but make sure the
            // "."s turn into "/"'s
            nclname = nclname.replace('.', '/');
          }
        cinfo.rename(nclname);

        if (dumpMapProp != null)
          {
            nclname = nclname.replace('/', '.');
            dumpMapProp.put(key, nclname);
          }
      }

    // Check each field now
    Modifiable m[] = cinfo.definedFields();
    for (int i=0; i<m.length; i++)
      {
        String fqname = key +"."+m[i].originalName();
        if (fieldFilter.matches(m[i].permissions(), fqname))
          { verboseMessage("Filtered field " + fqname); }
        else
          {
            String n=null;
            if (loadMapProp != null)
              { n = (String) loadMapProp.get(fqname); }
            if (n == null)
              { n = fnameGen.checkedName(stopSymbols, fqname); }
            m[i].rename(n);
            if (dumpMapProp != null)
              { dumpMapProp.put(fqname, n); }
          }
      }

    // Now for the methods. Do a quick stop on class and instance
    // initializers.

    Method mets[] = cinfo.definedMethods();
    for (int i=0; i<mets.length; i++)
      {
        String mname = mets[i].originalName();

        if (("<init>".equals(mname)) ||
            ("<clinit>".equals(mname)))
          { continue; }
        String fqname = key +"."+mname;
        if (methodFilter.matches(mets[i].permissions(), fqname))
          {
            verboseMessage("Filtered method " + fqname);
            continue;
          }
        if (((cinfo.permissions() & ACC_INTERFACE) != 0) &&
            affectsStoppedClass(cinfo, mets[i]))
          {
            verboseMessage("Filtered method " + fqname);
            continue;
          }
        String n=null;
        if (loadMapProp != null)
          { n = (String) loadMapProp.get(fqname); }
        if (n == null)
          { n = mnameGen.checkedName(stopSymbols, fqname); }
        mets[i].rename(n);
        if (dumpMapProp != null)
          { dumpMapProp.put(fqname, n); }
      }
  }

  ////////////BytecodeFactory implementation/////////////

  public DataInput readClass(String classname)
       throws IOException
  {
    // First search for the data in the source directories
    classname += ".class";
    for (int i=0; i<srcs.length; i++)
      {
        DataInput in;
        in = srcs[i].getFile(classname);
        if (in != null) return in;
      }
    // Then from the class path
    for (int i=0; i<dflt.length; i++)
      {
        DataInput in;
        in = dflt[i].getFile(classname);
        if (in != null) return in;
      }
    return null;
  }

  public DataOutput writeClass(String classname)
       throws IOException
  {
    // Check if this class doesn't need to be filtered out
    classname = classname.replace('/', '.');
    if (dontDump.matches(2048-1, classname))
      { return null; }
    if (verifyScriptFile != null)
      {
        verifyScriptFile.print("javap -verify ");
        verifyScriptFile.print(classname);
        verifyScriptFile.print(lineSep);
      }
    classname = classname.replace('.', File.separatorChar) + ".class";
    File f = new File(outDir, classname);

    if (f.exists())
      {
        if (!checkOverwrite)
          { return new DataOutputStream(new FileOutputStream(f)); }
        else
          {
            System.err.println("**WARNING** did not dump out " + classname + " because it already exists");
            return null;
          }
      }

    String pdir = f.getParent();
    if (pdir != null)
      {
        File fpdir = new File(pdir);
        if (!fpdir.exists())
          { fpdir.mkdirs(); }
      }
    return new DataOutputStream(new FileOutputStream(f));
  }

  BatchObfuscator(ExcludeFilter all,
                  ExcludeFilter clz,
                  ExcludeFilter met,
                  ExcludeFilter fld,
                  ExcludeFilter nodmp,
                  Vector roots,
                  String outdir,
                  Vector pMap,
                  Vector pNames,
                  NameGenerator cGen,
                  NameGenerator mGen,
                  NameGenerator fGen,
                  Props dumpMap,
                  Props useMap,
                  boolean dbg,
                  PrintStream verifyName)
       throws IOException
  {
    allFilter = all;
    fieldFilter = fld;
    methodFilter = met;
    classFilter = clz;
    dontDump = nodmp;
    outDir = outdir;
    packMap = new Matcher[pMap.size()];
    packName = new String[packMap.length];
    pMap.copyInto(packMap);
    pNames.copyInto(packName);
    cnameGen = (cGen!=null)?cGen:new FileNameGenerator();
    fnameGen = (fGen!=null)?fGen:new UnicodeGenerator();
    mnameGen = (mGen!=null)?mGen:new UnicodeGenerator();
    debug = dbg;
    if ((verifyScriptFile = verifyName) != null)
      {
        verifyName.print("cd ");
        verifyName.print(outdir);
        verifyName.print(lineSep);
      }

    verboseMessage("Searching source classes");
    srcs = new FileLocator[roots.size()];
    int idx = 0;
    for (Enumeration e=roots.elements();e.hasMoreElements();)
      {
        String r = (String)(e.nextElement());
        File f = new File(r);
        if (f.isFile())
          { srcs[idx] = new ZipFileLocator(r); }
        else
          { srcs[idx] = new DirFileLocator(r, true); }
        idx++;
      }
    // Pull off information from the classpath
    verboseMessage("Searching for base classes");
    String cpath = System.getProperties().getProperty("java.class.path");
    StringTokenizer tok = new StringTokenizer(cpath, File.pathSeparator);
    Vector tmp = new Vector();
    while (tok.hasMoreElements())
      {
        String s = tok.nextToken();
        try
          {
            File f= new File(s);
            if (!f.exists())
              { continue; }
            if (f.isFile())
              { tmp.addElement(new ZipFileLocator(s)); }
            else
              { tmp.addElement(new DirFileLocator(s, false)); }
          }
        catch(IOException ex)
          {
            System.err.println("Warning: Ignoring CLASSPATH element " + s + ": "+ ex.toString());
          }
      }
    dflt = new FileLocator[tmp.size()];
    tmp.copyInto(dflt);

    loadMapProp = useMap;
    dumpMapProp = dumpMap;
                                // Initialize namegenerators with already
                                // present symbols if we use a map.
    if (useMap != null)
      {
        stopSymbols = new Hashtable(useMap.size());
        for (Enumeration en = useMap.elements(); en.hasMoreElements();)
          {
            String sym = (String) en.nextElement();
            int lidx = sym.lastIndexOf('.');
            if (lidx >= 0)
              { sym = sym.substring(lidx+1, sym.length()); }
            stopSymbols.put(sym, sym);
          }
      }
  }

  private Props loadMapProp = null, dumpMapProp = null;
  private Hashtable stopSymbols = null;

  // True if this method in this interface affects a class
  // thats in our ExcludeAll() list

  private boolean affectsStoppedClass(ClassInfo c, Method m)
  {
    Hashtable set = c.affects(m);
    if (set == null) return false;
    for (Enumeration en=set.elements(); en.hasMoreElements();)
      {
        ClassInfo cin = (ClassInfo)(en.nextElement());
        String cname = cin.originalName();
        cname = cname.replace('/', '.');
        if (allFilter.matches(cin.permissions(), cname))
          { return true; }
      }
    return false;
  }

  // Add all classes from the source directories
  void addClasses(Environment env)
       throws IOException, BytecodeFormatException
  {
    verboseMessage("Loading classes");
    for (int i=0; i<srcs.length; i++)
      {
        for(Enumeration e=srcs[i].list(); e.hasMoreElements(); )
          {
            String cname = (String) (e.nextElement());
            if (cname.endsWith(".class"))
              {
                verboseMessage("Loaded " + cname);
                DataInput in = srcs[i].getFile(cname);
                if (in == null)
                  { System.err.println("woops, what happened to " + cname); }
                else
                  { env.addClass(in, false); }
              }
          }
      }
  }

  // Given a string, find if it gets remapped
  private final String findIfMapped(String key)
  {
    for (int i=0; i<packMap.length; i++)
      {
        if (packMap[i].match(key))
          { return packName[i]; }
      }
    return null;
  }

  private ExcludeFilter allFilter, fieldFilter, methodFilter, classFilter;
  private ExcludeFilter dontDump;
  private FileLocator srcs[];
  private FileLocator dflt[];
  private boolean checkOverwrite = false;
  private String outDir;
  private boolean debug;
  private Matcher packMap[];
  private String packName[];
  private PrintStream verifyScriptFile = null;
  private static String lineSep =
    System.getProperties().getProperty("line.separator", "\n");
}
