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
import java.util.*;

/**
 * A HRule simple generates a rule like area
 * @author $Author: kbs $
 */
class HRule extends Canvas
{
  private Dimension sz = new Dimension(25, 12);
  //  private Color light = new Color(224, 224, 224);
  public Dimension minumumSize()
  { return sz; }
  public Dimension preferredSize()
  { return sz; }

  public HRule()
  { setBackground(Color.lightGray); }

  public void update(Graphics g)
  { paint(g); }

  public void paint(Graphics g)
  {
    Dimension sz = size();
    g.setColor(Color.gray);
    g.drawLine(0, 6, sz.width, 6);
    g.setColor(Color.white);
    g.drawLine(0, 7, sz.width, 7);
    g.setColor(Color.lightGray);
  }
}
