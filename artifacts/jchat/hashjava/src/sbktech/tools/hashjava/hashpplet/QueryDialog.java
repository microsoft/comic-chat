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

import java.awt.*;

class QueryDialog extends Dialog
{
  Button buttons[];
  int lastClicked = -1;
  Thread targetThread;

  int getClickIndex()
  { return lastClicked; }

  QueryDialog(Frame parent, String title, String info,
              String bnames[], Thread t)
  {
    super(parent, title, false);
    targetThread = t;

    setBackground(Color.lightGray);
    add("Center", new TextDisplay(info));
    Panel tmp = new Panel();
    add("South", tmp);
    buttons = new Button[bnames.length];
    for (int i=0; i<bnames.length; i++)
      { tmp.add(buttons[i] = new Button(bnames[i])); }
    resize(300, 200);
    //    show();
  }

  public boolean handleEvent(Event e)
  {
    if (e.id == e.WINDOW_DESTROY)
      {
        lastClicked = -1;
        dispose();
        targetThread.resume();
        return true;
      }
    return super.handleEvent(e);
  }

  public boolean action(Event e, Object arg)
  {
    for (int i=0; i<buttons.length; i++)
      {
        if (buttons[i] == e.target)
          {
            lastClicked = i;
            dispose();
            targetThread.resume();
            return true;
          }
      }
    return false;
  }
}
