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

class DebuggingPanel extends Panel implements WizCheck
{
  CheckboxGroup source, lines, vars;
  Checkbox sourceRetain, sourceRemove, sourceAlter;
  Checkbox linesRetain, linesRemove, linesAlter;
  Checkbox varsRetain, varsRemove, varsAlter;
  Main fparent;

  public boolean check()
  {
    Checkbox cb = source.getCurrent();
    if (cb == sourceRetain)
      { fparent.sourceDebugFlag = Environment.DEBUG_RETAIN; }
    else if (cb == sourceRemove)
      { fparent.sourceDebugFlag = Environment.DEBUG_REMOVE; }
    else
      { fparent.sourceDebugFlag = Environment.DEBUG_CORRUPT; }

    cb = lines.getCurrent();
    if (cb == linesRetain)
      { fparent.linesDebugFlag = Environment.DEBUG_RETAIN; }
    else if (cb == linesRemove)
      { fparent.linesDebugFlag = Environment.DEBUG_REMOVE; }
    else
      { fparent.linesDebugFlag = Environment.DEBUG_CORRUPT; }

    cb = vars.getCurrent();
    if (cb == varsRetain)
      { fparent.varsDebugFlag = Environment.DEBUG_RETAIN; }
    else if (cb == varsRemove)
      { fparent.varsDebugFlag = Environment.DEBUG_REMOVE; }
    else
      { fparent.varsDebugFlag = Environment.DEBUG_CORRUPT; }

    return true;
  }

  DebuggingPanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());

    TextDisplay td = new TextDisplay("If you want to retain enough debugging information to obtain stack traces, retain source file names and line numbers. Otherwise, these default settings will work reasonably well.");
    add("Center", td);

    Panel tmp = new Panel();
    add("South", tmp);
    tmp.setLayout(new GridLayout(4, 3));
    tmp.add(new Label("Source file name"));
    tmp.add(new Label("Line numbers"));
    tmp.add(new Label("Local variable names"));

    source = new CheckboxGroup();
    lines = new CheckboxGroup();
    vars = new CheckboxGroup();

    tmp.add(sourceRetain = makeCheckbox("Retain", false, source));
    tmp.add(linesRetain = makeCheckbox("Retain", false, lines));
    tmp.add(varsRetain = makeCheckbox("Retain", false, vars));

    tmp.add(sourceAlter = makeCheckbox("Alter", true, source));
    tmp.add(linesAlter = makeCheckbox("Alter", false, lines));
    tmp.add(varsAlter = makeCheckbox("Alter", true, vars));

    tmp.add(sourceRemove = makeCheckbox("Remove", false, source));
    tmp.add(linesRemove = makeCheckbox("Remove", true, lines));
    tmp.add(varsRemove = makeCheckbox("Remove", false, vars));
  }
                                // Stupid mistake in 1.1 API
  private Checkbox makeCheckbox(String label, boolean state, CheckboxGroup cbg)
  {
    Checkbox cb = new Checkbox(label);
    cb.setCheckboxGroup(cbg);
    cb.setState(state);
    return cb;
  }
}
