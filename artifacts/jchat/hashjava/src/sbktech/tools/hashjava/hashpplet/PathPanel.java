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

class PathPanel extends Panel implements WizCheck
{
  String path;
  Checkbox urlCheck, fileCheck;
  Button browseButton;
  TextField urlText, fileText;
  Main fparent;

  public boolean check()
  {
    String path;
    if (urlCheck.getState())
      { path = urlText.getText(); }
    else
      { path = fileText.getText(); }

    if ((path == null) ||
        (path.length() == 0))
      {
        new ErrorDialog("Error",
                        "Please enter either a URL or a path to an html file");
        return false;
      }

    if (path.startsWith("http://www.sbktech.org"))
      { fparent.startEgg(); }

                                // Check if the URL is well formed
    if (urlCheck.getState())
      {
        try
          { new URL(path); }
        catch(Exception ex)
          {
            new ErrorDialog("Error", "Please enter a valid URL (like http://some.host.com/path/to/file.html)");
            return false;
          }
        fparent.setSourceURL(path);
      }
    else
      {
        try
          {
            File file = new File(fileText.getText());
            if (!file.exists())
              {
                new ErrorDialog("Error", "File ``" + fileText.getText() + "'' does not exist. Please enter a valid file name first");
                return false;
              }
            if (!file.canRead())
              {
                new ErrorDialog
                  ("Error", "File ``"+fileText.getText() + "'' is not readable. Please enter a readable file name first.");
                return false;
              }
          }
        catch(Exception ex)
          {
            new ErrorDialog("Error", "Unable to read file because " + ex);
            return false;
          }
        fparent.setSourceFile(path);
      }
    return true;
  }

  PathPanel(Main fparent)
  {
    this.fparent = fparent;
    setLayout(new BorderLayout());

    TextDisplay td = new TextDisplay("Enter path to an HTML file containing the applet(s) to be obfuscated.");
    add("Center", td);

    Panel tmp = new Panel();
    add("South", tmp);
    tmp.setLayout(new GridLayout(0, 1));

    CheckboxGroup cbg = new CheckboxGroup();
    tmp.add(urlCheck = new Checkbox("Load from URL", cbg, true));
    tmp.add(urlText = new TextField("http://"));
    tmp.add(fileCheck = new Checkbox("Load from file", cbg, false));
    Panel tmp1 = new Panel();
    tmp1.setLayout(new BorderLayout());
    tmp1.add("Center", fileText = new TextField());
    tmp1.add("East", browseButton = new Button("Browse"));
    tmp.add(tmp1);
    urlMode(true);
  }
  public boolean action(Event e, Object arg)
  {
    if (e.target == urlCheck)
      {
        urlMode(urlCheck.getState());
        return true;
      }
    if (e.target == fileCheck)
      {
        urlMode(!fileCheck.getState());
        return true;
      }
    if (e.target == browseButton)
      {
        FileDialog fd = new FileDialog(fparent, "Select HTML file with applet",
                                       FileDialog.LOAD);
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
  void urlMode(boolean mode)
  {
    urlText.enable(mode);
    urlText.setEditable(mode);

    fileText.enable(!mode);
    fileText.setEditable(!mode);
    browseButton.enable(!mode);
  }
}
