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

// *sigh*
// I really really did NOT want to write this
package sbktech.tools.hashjava.hashpplet;

import java.awt.*;
import java.util.*;

class TextDisplay extends Canvas
{
  String words[];
  int wordLengths[];
  int ht;
  boolean inited = false;
  String text;

  TextDisplay(String text)
  {
    this.text = text;
  }
  void init()
  {
    FontMetrics metrics = getFontMetrics(getFont());
    ht = metrics.getHeight();

    Vector tmp = new Vector();
    StringTokenizer tok = new StringTokenizer(text, " \r\t");
    while(tok.hasMoreElements())
      { tmp.addElement(tok.nextElement()); }
    words = new String[tmp.size()];
    wordLengths = new int[tmp.size()];

    int i = 0;
    for (Enumeration e = tmp.elements(); e.hasMoreElements();)
      {
        String s = (String)(e.nextElement());
        if (s.endsWith("\n"))
          {
            words[i] = s.replace('\n', ' ');
            wordLengths[i] = -metrics.stringWidth(s);
          }
        else
          {
            s += " ";
            words[i] = s;
            wordLengths[i] = metrics.stringWidth(s);
          }
        i++;
      }
    inited = true;
  }

  public void paint(Graphics g)
  {
    Dimension d = size();

    if (!inited)
      { init(); }

    int curx = 0;
    int cury = ht;

    for (int i=0; i<words.length; i++)
      {
        int curlen = (wordLengths[i] < 0)?-wordLengths[i]:wordLengths[i];
        if (d.width < (curx + curlen))
          {
            curx = 0; cury += ht;
            if (cury > d.height) return;
          }
            
        g.drawString(words[i], curx, cury);
        if (wordLengths[i] < 0)
          { curx = 0; cury += ht; }
        else
          { curx += wordLengths[i]; }
      }
  }
}
