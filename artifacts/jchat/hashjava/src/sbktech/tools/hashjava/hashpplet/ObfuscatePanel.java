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

package sbktech.tools.hashjava.hashpplet;

import sbktech.tools.hashjava.util.*;
import sbktech.tools.hashjava.bytecode.*;

import java.awt.*;
import java.io.*;
import java.net.*;
import java.util.*;

class ObfuscatePanel extends Panel implements WizCheck, Runnable, BytecodeFactory, Statistics, Obfuscator
{
  Main fparent;
  Label infoLabel;
  List infoText;
  Checkbox classinit, load, obfuscate, write;
  public boolean check()
  { System.exit(0); return true; }

  private void reset()
  {
    classinit.enable(false);
    classinit.setState(false);
    load.enable(false);
    load.setState(false);
    obfuscate.enable(false);
    obfuscate.setState(false);
    write.enable(false);
    write.setState(false);
    searchpath = null;
    baseClasses.clear();
    env = null;
    launch.enable(true);
    repaint();
  }

  ObfuscatePanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());
    Panel center = new Panel();
    center.setLayout(new GridLayout(0, 1));
    center.add(classinit = new Checkbox("Initialize base classes"));
    classinit.enable(false);

    center.add(load = new Checkbox("Load applet"));
    load.enable(false);

    center.add(obfuscate = new Checkbox("Obfuscate"));
    obfuscate.enable(false);

    center.add(write = new Checkbox("Dump obfuscated classes"));
    write.enable(false);

    add("North", center);
    Panel fud = new Panel();
    fud.setLayout(new BorderLayout());
    fud.add("North", infoLabel = new Label("", Label.LEFT));
    Panel bud = new Panel();
    bud.setLayout(new BorderLayout());
    bud.add("North", new Label("Messages", Label.LEFT));
    bud.add("Center", infoText = new List());
    fud.add("Center", bud);
    add("Center", fud);
    infoLabel.setForeground(Color.blue);
    infoLabel.setFont(new Font("Courier", Font.BOLD, 8));
    Panel tmp = new Panel();
    tmp.add(launch = new Button("Obfuscate!"));
    add("South", tmp);
  }

  Button launch;

  public boolean action(Event e, Object arg)
  {
    if (e.target == launch)
      {
        Thread t = new Thread(this);
        t.setPriority(t.MIN_PRIORITY);
        t.start();
        launch.enable(false);
        return true;
      }
    return false;
  }
  Vector searchpath;

  private void loadClasses()
  {
    classinit.enable(true);
    repaint();
    searchpath  = new Vector();
    String cpath = System.getProperties().getProperty("java.class.path");
    //    System.out.println("CLASSPATH is " + cpath);
    StringTokenizer tok = new StringTokenizer(cpath, File.pathSeparator);
    while(tok.hasMoreElements())
      {
        String s = tok.nextToken();
        File f = new File(s);
        if (!f.exists())
          { continue; }
        if (f.isFile())
          {
            localInfo("Preloading " + s);
            searchpath.addElement(new ZipFileLocator(s));
            localInfo("");
            //  System.out.println("CLASSPATH added " +s + " to zip list");
          }
        else
          {
            searchpath.addElement(new DirFileLocator(s));
            // System.out.println("CLASSPATH added " +s+" to directory list");
          }
      }
    classinit.setState(true);
    classinit.enable(false);
    repaint();
  }

  // Store all base applets being obfuscated
  private Hashtable baseClasses = new Hashtable();
  // Environment where things are happening
  private Environment env;

  private void loadApplet()
       throws Exception
  {
    load.enable(true);
    repaint();
    InputStream in = null;
    BufferedOutputStream out = null;
    File fileS = null;
    URL uSource = null;

    if (fparent.fileSource != null)
      {
        fileS = new File(fparent.fileSource);
        localInfo("Opening " + fileS);
        in = new BufferedInputStream(new FileInputStream(fileS));
      }
    else if (fparent.urlSource != null)
      {
        uSource = new URL(fparent.urlSource);
        localInfo("Opening " + uSource);
        in = uSource.openStream();
      }

    out = new BufferedOutputStream(new FileOutputStream(fparent.outPath));

    TagParser t= new TagParser(in, out);

    env = new Environment(this, this, this);
    env.setSourceFileDebugging(fparent.sourceDebugFlag);
    env.setLineNumberDebugging(fparent.linesDebugFlag);
    env.setLocalVariableDebugging(fparent.varsDebugFlag);

    int napplets = 0;
    for (Enumeration e = t.elements(); e.hasMoreElements();)
      {
        Hashtable tags  = (Hashtable)(e.nextElement());
        String code = (String) tags.get("code");
        if (code.endsWith(".class"))
          {
            code = code.substring(0, code.length()-6);
          }

        baseClasses.put(code, code);

                                // Add any codebase things
        String codebase;
        codebase = (String) tags.get("codebase");
        if  (codebase == null)
          codebase = "";
        else if (codebase.endsWith("/"))
          codebase = codebase.substring(0, codebase.length()-1);

        if (fparent.outPath.lastIndexOf(File.separatorChar) >= 0)
          {
            codebaseOut =
              fparent.outPath.substring
              (0, fparent.outPath.lastIndexOf(File.separatorChar));
          }
        else
          { codebaseOut = "."; }

        if ((codebase.length() > 0) &&
            (codebase.charAt(0) == '/'))
          {
            codebaseOut = 
              (codebaseOut + codebase).replace('/', File.separatorChar);
          }
        else if (codebase.startsWith("http://"))
          {
            codebaseOut =
              (codebaseOut + codebase.substring(6, codebase.length()))
              .replace('/', File.separatorChar);
          }
        else
          {
            codebaseOut =
              (codebaseOut + "/" + codebase).replace('/', File.separatorChar);
          }
            
        if (fileS != null)
          {
            String rootD = fileS.getParent();
            if (codebase.length() > 0)
              { rootD += File.separator + codebase; }

            searchpath.addElement
              (new DirFileLocator(rootD));
            // System.out.println("Applet added " + rootD + " to directory list");
          }
        else
          {
            String us = uSource.toString();
            URL rootU = null;
            if (codebase.startsWith("/"))
              {
                rootU = new URL(uSource.getProtocol(),
                                uSource.getHost(),
                                uSource.getPort(),
                                codebase);
              }
            else if (codebase.length() > 0)
              {
                rootU = new URL(uSource, codebase);
              }
            else
              {
                us = us.substring(0, us.lastIndexOf('/') + 1);
                rootU = new URL(us);
              }
            searchpath.addElement
              (new HttpFileLocator(rootU));
          }
        localInfo("Adding " + code);
        napplets++;
                                // Now add this class to the environment
        DataInput base = readClass(code);
        if (base != null)
          {
                                // resolve it fully to trap
                                // all references to classes
            env.addClass(base, true);
          }
        else
          {
            throw new FileNotFoundException(code);
          }
      }
    out.flush();
    if (napplets == 0)
      {
                                // No applets found
        throw new FileNotFoundException("No applet tags were found!");
      }
    if (napplets == 1)
      { info("Loaded one applet"); }
    else
      { info("Loaded " + napplets + " applets"); }
    load.setState(true);
    load.enable(false);
    repaint();
  }

  private void obfus()
       throws IOException
  {
    obfuscate.enable(true);
    repaint();
    env.obfuscate();
    obfuscate.setState(true);
    obfuscate.enable(false);
    repaint();
  }

  private void dumpit()
       throws IOException
  {
    write.enable(true);
    repaint();
    env.dump();
    write.setState(true);
    write.enable(false);
    repaint();
  }

  public void run()
  {
    try
      {
                                // Load up classes
        loadClasses();
                                // Load applet
        loadApplet();

                                // Start obfuscating
        obfus();
                                // dump out classes
        dumpit();

        info("Congratulations! Get HTML, bytecode at " + fparent.outPath);
        reset();
      }
    catch(Exception ex)
      {
        Thread cur = Thread.currentThread();
        String choices[] = {" Ok "};
        ex.printStackTrace();
        new QueryDialog
          (fparent,
           "Error!",
           "Cannot continue because \n " + ex.toString() +
           "\n \n Some errors can be fixed by returning to the previous screens and verifying or altering the entries.",
           choices, cur).show();
        cur.suspend();
        reset();
        return;
      }
  }

                                // Statistics stuff
  public void localInfo(String s)
  {
    infoLabel.setText(s);
    repaint();
  }

  public void info(String s)
  {
    infoText.addItem(s);
    infoText.makeVisible(infoText.countItems() - 1);
  }

  public void verboseMessage(String s)
  { }

                                // Obfuscator
  private int class_idx = 0;
  private int field_idx = 0;
  private int method_idx = 0;

  public void obfuscate(ClassInfo cinfo)
  {
    String curClass = cinfo.originalName();

    if (curClass.startsWith("java/")) return;
    if (curClass.startsWith("netscape/")) return;
    if (curClass.startsWith("sun/")) return;

                                // Rename every field in the class
    Modifiable m[];
    if (fparent.renameFields)
      {
        m = cinfo.definedFields();
        for (int i=0; i<m.length; i++)
          { m[i].rename(fparent.fnameGenerator.nextName()); }
      }

                                // And every method not a constructor
                                // or static class initializer
    if (fparent.renameMethods)
      {
        Method mets[] = cinfo.definedMethods();
        for (int i=0; i<mets.length; i++)
          {
            String mname = mets[i].originalName();
            if (!("<init>".equals(mname)) &&
                !("<clinit>".equals(mname)) &&
                !(implementsStandardMethod(cinfo, mets[i])))
              { mets[i].rename(fparent.mnameGenerator.nextName()); }
          }
      }

                                // Rename the class too if it isn't
                                // the base class(es)
    String key = curClass.replace('/', '.');
    if (fparent.renameClasses)
      {
        if (baseClasses.get(key) == null)
          {
            if (curClass.lastIndexOf('/') > 0)
              {
                String n =
                  curClass.substring(0, curClass.lastIndexOf('/'))
                  + "/" + fparent.cnameGenerator.nextName();
                cinfo.rename(n);
              }
            else
              { cinfo.rename(fparent.cnameGenerator.nextName()); }
          }
      }
    localInfo("Obfuscated " + curClass);
  }

                                // BytecodeFactory
  public DataInput readClass(String cname)
       throws IOException
  {
    DataInput ret = null;
    cname = cname.replace('.', '/')+ ".class";
    localInfo("Loading " + cname);
    //    System.out.println("loading file name " + cname);
    for (Enumeration e = searchpath.elements(); e.hasMoreElements();)
      {
        FileLocator fl = (FileLocator)(e.nextElement());
        ret = fl.getFile(cname);
        if (ret != null)
          { localInfo("Loaded " + cname); return ret; }
      }
    localInfo("did not find " + cname);
    return null;
  }

  private boolean checkOverwrite = true;
  private String codebaseOut = null;

  public DataOutput writeClass(String cname)
       throws IOException
  {
    if (cname.startsWith("java/"))
      return null;
    if (cname.startsWith("netscape/"))
      return null;
    if (cname.startsWith("sun/"))
      return null;

    cname = cname.replace('/', File.separatorChar) +".class";

    File f = new File(codebaseOut, cname);

    localInfo("Writing out " + cname);
    if (checkOverwrite && f.exists())
      {
        Thread cur = Thread.currentThread();
        String choices[] = {"Overwrite", "Overwrite All", "Skip"};
        QueryDialog qd = new QueryDialog
          (fparent,
           "Warning",
           "File " + f + " already exists",
           choices, cur);
        qd.show();
        cur.suspend();
        switch(qd.getClickIndex())
          {
          default:
            return null;
          case 1:
            checkOverwrite = false;
                                // Fall through to
          case 0:
            return new DataOutputStream(new FileOutputStream(f));
          }
      }
    String pdir = f.getParent();
    if (pdir != null)
      {
        File fpdir = new File(pdir);
        if (!fpdir.exists())
          { fpdir.mkdirs(); }
        // return new DataOutputStream(new FileOutputStream(fpdir, f.getName()));
      }
    return new DataOutputStream(new FileOutputStream(f));
  }

  // Determine if this class is an interface, and which implements
  // any class in a "standard" package
  private static boolean implementsStandardMethod(ClassInfo cin, Method m)
  {
    if ((cin.permissions() & VMConstants.ACC_INTERFACE) == 0) return false;
    Hashtable set = cin.affects(m);
    if (set == null) return false;
    for (Enumeration en=set.elements(); en.hasMoreElements();)
      {
        ClassInfo c= (ClassInfo)(en.nextElement());
        String cname = c.originalName();
        if (cname.startsWith("java/") ||
            cname.startsWith("netscape/") ||
            cname.startsWith("sun/"))
          { return true; }
      }
    return false;
  }
}
