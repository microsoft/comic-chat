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

// Panel to contain a next/previous button to maintain
// a consistent look about installation sequences

package sbktech.tools.hashjava.hashpplet;

import java.awt.*;

class WizPanel extends Panel
{
  Container parent;
  Button prev, next;
  int cindex;
  Component display;
  WizCheck checker;

  Insets ins = new Insets(50, 10, 10, 20);

  public Insets insets()
  { return ins; }

  WizPanel(Container parent,
           String prevName, boolean prevEnabled,
           String nextName, boolean nextEnabled,
           Component display, WizCheck checker)
  {
    this.parent = parent;
    this.display = display;
    this.checker = checker;

    setLayout(new BorderLayout());
    add("Center", display);
    Panel bar = new Panel();
    bar.setLayout(new BorderLayout());
    bar.add("North", new HRule());
    Panel tmp = new Panel();
    bar.add("Center", tmp);
    tmp.setLayout(new FlowLayout(FlowLayout.CENTER, 0, 5));
    if (prevName != null)
      {
        tmp.add(prev = new Button(prevName));
        prev.enable(prevEnabled);
      }
    if (nextName != null)
      {
        tmp.add(next = new Button(nextName));
        next.enable(nextEnabled);
      }
    add("South", bar);
  }

  public boolean action(Event e, Object arg)
  {
    if (e.target == next)
      {
        if (checker.check())
          {
            CardLayout cl = (CardLayout)(parent.getLayout());
            cl.next(parent);
            return true;
          }
        else
          { return false; }
      }
    if (e.target == prev)
      {
        CardLayout cl = (CardLayout)(parent.getLayout());
        cl.previous(parent);
        return true;
      }
    return false;
  }
}
