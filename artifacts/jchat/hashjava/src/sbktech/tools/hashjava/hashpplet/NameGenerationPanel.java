/*
 * Copyright (C) 1997 KB Sriram
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

class NameGenerationPanel extends Panel implements WizCheck
{
  private CheckboxGroup cname, fname, mname;
  private Checkbox cnameNum, cnameUnicode, cnameFile;
  private Checkbox fnameNum, fnameUnicode, fnameFile;
  private Checkbox mnameNum, mnameUnicode, mnameFile;

  Main fparent;

  public boolean check()
  {
    Checkbox cb = cname.getCurrent();
    if (cb == cnameNum)
      { fparent.cnameGenerator = new NumberGenerator(); }
    else if (cb == cnameUnicode)
      { fparent.cnameGenerator = new UnicodeGenerator(); }
    else
      { fparent.cnameGenerator = new FileNameGenerator(); }

    cb = fname.getCurrent();
    if (cb == fnameNum)
      { fparent.fnameGenerator = new NumberGenerator(); }
    else if (cb == fnameUnicode)
      { fparent.fnameGenerator = new UnicodeGenerator(); }
    else
      { fparent.fnameGenerator = new FileNameGenerator(); }

    cb = mname.getCurrent();
    if (cb == mnameNum)
      { fparent.mnameGenerator = new NumberGenerator(); }
    else if (cb == mnameUnicode)
      { fparent.mnameGenerator = new UnicodeGenerator(); }
    else
      { fparent.mnameGenerator = new FileNameGenerator(); }

    return true;
  }

  NameGenerationPanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());

    TextDisplay td = new TextDisplay("The default settings will generate the most compact classes, but stack traces will be harder to read if methods are Unicode strings.");
    add("Center", td);

    Panel tmp = new Panel();
    add("South", tmp);

    tmp.setLayout(new GridLayout(4, 3));
    tmp.add(new Label("Class names"));
    tmp.add(new Label("Method names"));
    tmp.add(new Label("Field names"));

    cname = new CheckboxGroup();
    mname = new CheckboxGroup();
    fname = new CheckboxGroup();

    tmp.add(cnameNum = makeCheckbox("_number", false, cname));
    tmp.add(mnameNum = makeCheckbox("_number", false, mname));
    tmp.add(fnameNum = makeCheckbox("_number", false, fname));

    tmp.add(cnameUnicode = makeCheckbox("unicode", false, cname));
    tmp.add(mnameUnicode = makeCheckbox("unicode", true, mname));
    tmp.add(fnameUnicode = makeCheckbox("unicode", true, fname));

    tmp.add(cnameFile = makeCheckbox("lowercase letters", true, cname));
    tmp.add(mnameFile = makeCheckbox("lowercase letters", false, mname));
    tmp.add(fnameFile = makeCheckbox("lowercase letters", false, fname));
  }
                                // *sigh* stupid 1.1 API bug
  private Checkbox makeCheckbox(String label, boolean state, CheckboxGroup cbg)
  {
    Checkbox cb = new Checkbox(label);
    cb.setCheckboxGroup(cbg);
    cb.setState(state);
    return cb;
  }
}
