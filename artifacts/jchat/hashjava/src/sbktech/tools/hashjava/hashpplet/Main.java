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

/**
 * Hash applets over the net
 */

public class Main extends Frame
{
  String fileSource = null, urlSource = null, outPath = null;
  boolean renameFields = true,
          renameClasses = true,
          renameMethods = true,
          illegalIdentifiers = false;
  int sourceDebugFlag = Environment.DEBUG_CORRUPT;
  int linesDebugFlag = Environment.DEBUG_REMOVE;
  int varsDebugFlag = Environment.DEBUG_CORRUPT;
  NameGenerator cnameGenerator, mnameGenerator, fnameGenerator;

  void setSourceFile(String s)
  { fileSource = s; urlSource = null; }
  void setSourceURL(String s)
  { urlSource = s; fileSource = null; }
  void setOutPath(String s)
  { outPath = s; }
  void startEgg()
  { egg.init(); }

  Main(String s)
  { super(s); setBackground(Color.lightGray); }

  public boolean handleEvent(Event evt)
  {
    if (evt.id == Event.WINDOW_DESTROY)
      { System.exit(0); return true; }
    return super.handleEvent(evt);
  }

  public static void main(String argv[])
  {
    Main top = new Main("Free tools from http://www.sbktech.org");
    Panel p = new Panel();
    CardLayout clayout = new CardLayout();
    p.setLayout(clayout);
    p.setFont(new Font("Dialog", Font.PLAIN, 12));

    IntroPanel ip = new IntroPanel();

    CopyingPanel cp = new CopyingPanel();
    WizPanel wp =
      new WizPanel
      (p, null, false, "   Continue   ", true, cp, cp);
    p.add("License", wp);

    wp =
      new WizPanel
      (p, "   License   ", true, "    Next>    ", true, ip, ip);
    p.add("Intro", wp);

    PathPanel pp = new PathPanel(top);
    wp =
      new WizPanel
      (p, "    <Prev    ", true, "    Next>    ", true, pp, pp);
    p.add("Path", wp);

    OutPanel op = new OutPanel(top);
    wp = new WizPanel(p, "    <Prev    ", true, "    Next>    ", true, op, op);
    p.add("Out", wp);

    ParamPanel parp = new ParamPanel(top);
    wp = new WizPanel(p, "    <Prev    ", true, "    Next>    ", true, parp, parp);
    p.add("Parm", wp);

    DebuggingPanel dbgp = new DebuggingPanel(top);
    wp = new WizPanel(p, "    <Prev    ", true, "    Next>    ", true, dbgp, dbgp);
    p.add("Debug", wp);

    NameGenerationPanel ngp = new NameGenerationPanel(top);
    wp = new WizPanel(p, "    <Prev    ", true, "    Next>    ", true, ngp, ngp);
    p.add("Name", wp);

    ObfuscatePanel obp = new ObfuscatePanel(top);
    wp = new WizPanel(p, "    <Prev    ", true, "    Quit    ", true, obp, obp);
    p.add("Obfuscate", wp);

    clayout.show(p, "Intro");

    top.resize(500, 350);
    AnimLeader al;
    top.add("South", al = new AnimLeader());
    top.add("Center", p);
    top.add("West", egg = new Egg());
    top.show();
    al.init();
  }
  private static Egg egg;
}
