/*
 * Copyright (C) 1996,1997 KB Sriram
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

class ParamPanel extends Panel implements WizCheck
{
  Main fparent;

  public boolean check()
  {
    fparent.renameFields = renameFields.getState();
    fparent.renameMethods = renameMethods.getState();
    fparent.renameClasses = renameClasses.getState();
    return true;
  }

  ParamPanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());

    TextDisplay td = new TextDisplay("If your code contains Class.forName() calls, you should turn off renaming class names.");
    add("Center", td);

    Panel tmp = new Panel();
    add("South", tmp);
    tmp.setLayout(new GridLayout(0, 1));
    tmp.add(renameClasses = makeCheckbox("Rename classes", true));
    tmp.add(renameFields = makeCheckbox("Rename fields", true));
    tmp.add(renameMethods = makeCheckbox("Rename methods", true));
  }
                                // *sigh* stupid 1.1 API bug
  private Checkbox makeCheckbox(String label, boolean state)
  {
    Checkbox cb = new Checkbox(label);
    cb.setState(state);
    return cb;
  }
  private Checkbox renameClasses, renameFields, renameMethods;
}
