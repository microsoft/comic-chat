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

class OutPanel extends Panel implements WizCheck
{
  String path;
  Button browseButton;
  TextField fileText;
  Main fparent;

  public boolean check()
  {
    String path = fileText.getText();
    
    if ((path == null) ||
        (path.length() == 0))
      {
        new ErrorDialog("Error",
                        "Please enter a path to a new html file");
        return false;
      }

    File file;
    try
      {
        file = new File(fileText.getText());
      }
    catch(Exception ex)
      {
        new ErrorDialog("Error", "Unable to open file because " + ex);
        return false;
      }

    if (file.exists())
      {
        new ErrorDialog("Warning", file + " already exists and will be overwritten.\n \n You can also return the previous screen to pick a different file.");
      }
    fparent.setOutPath(file.toString());
    return true;
  }

  OutPanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());

    TextDisplay td = new TextDisplay("Enter path to a new HTML file which will refer to the obfuscated bytecode.\n \n The obfuscated code will also be written into the same directory as this HTML file. So it is best to specify a new HTML file in an empty directory.");
    add("Center", td);

    Panel tmp = new Panel();
    add("South", tmp);
    tmp.setLayout(new BorderLayout());
    tmp.add("Center", fileText = new TextField());
    tmp.add("East", browseButton = new Button("Browse"));
  }
  public boolean action(Event e, Object arg)
  {
    if (e.target == browseButton)
      {
        FileDialog fd = new FileDialog
          (fparent, "Select new output HTML file name",
           FileDialog.SAVE);
        fd.show();
        String s = fd.getDirectory();
        if (s != null)
          {
            String t = fd.getFile();
            if (t != null)
              {
                fileText.setText(s + t);
              }
          }
        return true;
      }
    return false;
  }
}
