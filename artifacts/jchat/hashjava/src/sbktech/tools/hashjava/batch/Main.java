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

import sbktech.tools.hashjava.bytecode.*;
import sbktech.tools.hashjava.util.*;
import java.io.*;
import java.util.*;

public class Main implements VMConstants
{
  private static void usage()
  {
    System.err.println
      ("Usage: sbktech.tools.hashjava.batch.Main [-verbose] <config file>");
  }
  private static StreamTokenizer in;
  private static boolean verbose = false;

  public static void main(String argv[])
  {
    String confFile = null;

    for (int i=0; i<argv.length; i++)
      {
        if (argv[i].equals("-verbose"))
          { verbose = true; }
        else if (argv[i].startsWith("-"))
          { usage(); return; }
        else if (confFile != null)
          { usage(); return; }
        else
          { confFile = argv[i]; }
      }

    if (confFile == null)
      { usage(); return; }

    try
      {
        in =
          new StreamTokenizer
          (new FileInputStream
           (confFile));
      }
    catch(FileNotFoundException ex)
      {
        System.err.println("Error: did not find file " + argv[0]);
        return;
      }

    in.eolIsSignificant(false);

    try
      {
        in.nextToken();
        compile();
      }
    catch(IOException ex)
      {
        System.err.println("Error reading " + argv[0]);
        ex.printStackTrace(System.err);
        return;
      }

    if (errs > 0) return;

    // Make sure we have enough data to proceed

    if (outputDir == null)
      {
        System.err.println("Missing OutputDir directive");
        return;
      }

    if (roots.size() == 0)
      {
        System.err.println("Missing ObfuscateRoot directive");
        return;
      }

    // Create the exclusion filters.
    ExcludeFilter all, clz, met, fld, nodmp;
    all = makeFilter("ExcludeAll");
    clz = makeFilter("ExcludeClass");
    met = makeFilter("ExcludeMethod");
    fld = makeFilter("ExcludeField");
    nodmp = makeFilter("DontDump");


    try
      {
        // Maybe the map file properties
        Props dumpMapProp = null, loadMapProp = null;
        if (dumpMap != null)
          { dumpMapProp = new Props(); }
        if (loadMap != null)
          {
            loadMapProp = new Props();
            BufferedInputStream binp =
              new BufferedInputStream
              (new FileInputStream(loadMap));
            loadMapProp.load(binp);
            binp.close();
          }

        BatchObfuscator ob = new BatchObfuscator
          (all, clz, met, fld, nodmp, roots, outputDir,
           remapvec, renamevec, cnameGen, mnameGen, fnameGen,
           dumpMapProp, loadMapProp, verbose, verifyScriptName);
        Environment env = new Environment(ob, ob, ob);
        env.setSourceFileDebugging(sourceDbg);
        env.setLineNumberDebugging(lineDbg);
        env.setLocalVariableDebugging(lvarsDbg);
        ob.addClasses(env);
        ob.verboseMessage("Obfuscating");
        env.obfuscate();
        env.dump();
        if (dumpMap != null)
          {
            BufferedOutputStream out =
              new BufferedOutputStream
               (new FileOutputStream(dumpMap));
            dumpMapProp.save(out);
            out.close();
          }
        if (verifyScriptName != null)
          { verifyScriptName.close(); }
      }
    catch(IOException ex)
      {
        System.err.println("Failed to obfuscate " + ex);
        return;
      }
    catch(BytecodeFormatException e1)
      {
        System.err.println("Invalid bytecode " + e1);
        return;
      }
  }

  private static ExcludeFilter makeFilter(String key)
  {
    Vector types = (Vector) fieldToType.get(key);
    Vector pats  = (Vector) fieldToPat.get(key);
    Matcher m[] = new Matcher[types.size()];
    int t[] = new int[types.size()];
    pats.copyInto(m);
    int i=0;
    for (Enumeration e=types.elements(); e.hasMoreElements();)
      { t[i] = ((Integer)(e.nextElement())).intValue(); i++; }

    return new ExcludeFilter(m, t);
  }

  private static void compile()
       throws IOException
  {
    while (in.ttype != in.TT_EOF)
      {
        try
          {
            statement();
            expect(';');
          }
        catch(ParseException ex)
          { errs++; skipToSemi(); }
      }
  }

  private static void statement()
       throws IOException, ParseException
  {
    if (in.ttype != in.TT_WORD)
      { error("Expected a keyword"); throw new ParseException(); }

    String k = in.sval; in.nextToken();
    expect('(');

    if ((k.equals("ObfuscateRoot")) ||
        (k.equals("OutputDir")) ||
        (k.equals("LoadMap")) ||
        (k.equals("DumpMap")))
      { path_args(k); }

    else if (fieldToPat.get(k) != null)
      {
        Vector tvec = (Vector)(fieldToType.get(k));
        Vector pvec = (Vector)(fieldToPat.get(k));
        field_args(tvec, pvec);
      }

    else if (k.equals("Verbose"))
      { verbose = true; }

    else if (k.equals("RenamePackage"))
      { rename_package(); }

    else if ((k.equals("ClassNameGenerator")) ||
             (k.equals("MethodNameGenerator")) ||
             (k.equals("FieldNameGenerator")))
      { generator_class(k); }

    else if ((k.equals("SourceName")) ||
             (k.equals("LineDebug")) ||
             (k.equals("LocalsDebug")))
      { boolean_args(k); }

    else if (k.equals("VerifierScript"))
      { verify_script(); }
    else
      {
        error("Unknown keyword " + k);
        throw new ParseException();
      }
    expect(')');
  }

  private static void generator_class(String k)
       throws IOException, ParseException
  {
    String cname = in.sval;
    expect('"');
    NameGenerator ng = null;
    try
      {
        Class c = Class.forName(cname);
        ng = (NameGenerator)(c.newInstance());
      }
    catch(Throwable th)
      {
        throw new ParseException
          ("Unable to create name generator " + cname + ": " + th.toString());
      }
      
    if (k.equals("ClassNameGenerator"))
      { cnameGen = ng; }
    else if (k.equals("FieldNameGenerator"))
      { fnameGen = ng; }
    else
      { mnameGen = ng; }
  }

  private static void verify_script()
       throws IOException, ParseException
  {
    verifyScriptName = new PrintStream(new FileOutputStream(in.sval));
    expect('"');
  }

  private static void rename_package()
       throws IOException, ParseException
  {
    String pname = in.sval;
    expect('"');
    expect(',');
    String nname = in.sval;
    expect('"');
    remapvec.addElement(Matcher.compile(pname));
    renamevec.addElement(nname);
  }

  private static void path_args(String k)
       throws IOException, ParseException
  {
    String path = in.sval;
    expect('"');
    if (k.equals("ObfuscateRoot"))
      { roots.addElement(path); }
    else if (k.equals("OutputDir"))
      { outputDir = path; }
    else if (k.equals("LoadMap"))
      { loadMap = path; }
    else
      { dumpMap = path; }
  }

  private static void boolean_args(String k)
       throws IOException, ParseException
  {
    String v = in.sval;
    if (in.ttype != in.TT_WORD)
      {
        error("Expected true/false");
        throw new ParseException();
      }
    in.nextToken();
    int val;
    if (v.equals("true"))
      { val = Environment.DEBUG_RETAIN; }
    else if (v.equals("false"))
      { val = Environment.DEBUG_REMOVE; }
    else
      {
        error("Expected a true/false");
        throw new ParseException();
      }
    if (k.equals("SourceName"))
      { sourceDbg = val; }
    else if (k.equals("LineDebug"))
      { lineDbg = val; }
    else
      { lvarsDbg = val; }
  }

  private static void field_args(Vector tvec, Vector pvec)
       throws IOException, ParseException
  {
    int tval = type_spec();
    expect(',');
    String path = in.sval;
    expect('"');
    tvec.addElement(new Integer(tval));
    pvec.addElement(Matcher.compile(path));
  }

  private static int type_spec()
       throws IOException, ParseException
  {
    boolean done = false;
    int ret = 0;
    while ((in.ttype != in.TT_EOF) && !done)
      {
        String k = in.sval;
        if (in.ttype != in.TT_WORD)
          {
            error("Expected a TYPE specification");
            throw new ParseException();
          }
        Integer ival = (Integer) typeConstants.get(k);
        if (ival == null)
          {
            error(k + " is not a valid TYPE");
            throw new ParseException();
          }
        ret |= (ival.intValue());
        in.nextToken();
        done = (in.ttype != '&');
        if (!done)
          { expect('&'); }
      }
    return ret;
  }

  private static Vector roots = new Vector();
  private static Hashtable packageMap = new Hashtable();
  private static NameGenerator cnameGen=null,fnameGen=null,mnameGen=null;
  private static String outputDir;
  private static int errs = 0;

  private static void skipToSemi()
       throws IOException
  {
    while ((in.ttype != in.TT_EOF) &&
           (in.ttype != ';'))
      { in.nextToken(); }
    in.nextToken();
  }

  private static void expect(char c)
       throws IOException, ParseException
  {
    if (in.ttype != c)
      {
        error("Expected ``" + c + "''");
        throw new ParseException();
      }
    in.nextToken();
  }

  private static void error(String s)
  {
    System.err.println(s + " near " + in);
  }
  private static Hashtable typeConstants;
  private static Hashtable fieldToType;
  private static Hashtable fieldToPat;
  private static Vector remapvec, renamevec;
  private static int lvarsDbg, sourceDbg, lineDbg;
  private static PrintStream verifyScriptName=null;
  private static String loadMap=null, dumpMap=null;

  static
  {
    sourceDbg = lineDbg = lvarsDbg = Environment.DEBUG_CORRUPT;
    typeConstants = new Hashtable(20);
    typeConstants.put("NONE", new Integer(0));
    typeConstants.put("PUBLIC", new Integer(ACC_PUBLIC));
    typeConstants.put("PRIVATE", new Integer(ACC_PRIVATE));
    typeConstants.put("PROTECTED", new Integer(ACC_PROTECTED));
    typeConstants.put("STATIC", new Integer(ACC_STATIC));
    typeConstants.put("FINAL", new Integer(ACC_FINAL));
    typeConstants.put("SYNCHRONIZED", new Integer(ACC_SYNCHRONIZED));
    typeConstants.put("VOLATILE", new Integer(ACC_VOLATILE));
    typeConstants.put("TRANSIENT", new Integer(ACC_TRANSIENT));
    typeConstants.put("NATIVE", new Integer(ACC_NATIVE));
    typeConstants.put("INTERFACE", new Integer(ACC_INTERFACE));
    typeConstants.put("ABSTRACT", new Integer(ACC_ABSTRACT));
    typeConstants.put("ALL", new Integer(-1));

    fieldToType = new Hashtable(10);
    fieldToPat = new Hashtable(10);

    Vector tmp;
    fieldToType.put("ExcludeAll", tmp = new Vector());
    tmp.addElement(new Integer(-1));
    fieldToPat.put("ExcludeAll", tmp = new Vector());
    tmp.addElement(Matcher.compile("java.*"));
    fieldToType.put("ExcludeClass", new Vector());
    fieldToPat.put("ExcludeClass", new Vector());
    fieldToType.put("ExcludeMethod", new Vector());
    fieldToPat.put("ExcludeMethod", new Vector());
    fieldToType.put("ExcludeField", new Vector());
    fieldToPat.put("ExcludeField", new Vector());
    fieldToType.put("DontDump", tmp = new Vector());
    tmp.addElement(new Integer(-1));
    fieldToPat.put("DontDump", tmp = new Vector());
    tmp.addElement(Matcher.compile("java.*"));

    remapvec = new Vector(5);
    renamevec = new Vector(5);
  }
}

class ParseException extends Exception
{
  ParseException()
  { super(); }
  ParseException(String s)
  { super(s); }
}
