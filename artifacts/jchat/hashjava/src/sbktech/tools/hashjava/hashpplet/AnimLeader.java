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

// I guess I had a little too much time on my hands ;-)
package sbktech.tools.hashjava.hashpplet;

import java.awt.*;
import java.io.*;


public class AnimLeader extends Panel implements Runnable
{
  Thread engine;
  int curx, cury;
  int linex;
  int wd, ht, len, desc, font_ht;
  Graphics bufGraphics;
  Image bufImage;
  String displayString = "java";
  String replaceString = "hashjava";
  Dimension d;

  AnimLeader()
  {
    Font f = new Font("Courier", Font.BOLD, 36);
    setFont(f);
    FontMetrics metrics = getFontMetrics(f);
    len = metrics.stringWidth(displayString);
    desc = metrics.getMaxDescent();
    font_ht = metrics.getHeight();
    d = new Dimension(100, font_ht + 5);
  }

  public Dimension preferredSize()
  { return d; }
  public Dimension minimumSize()
  { return d; }

  public void init()
  {
    d = new Dimension(getParent().size().width, font_ht+5);
    bufImage = createImage(d.width, d.height);
    bufGraphics = bufImage.getGraphics();
    bufGraphics.setFont(getFont());

    bufGraphics.setColor(Color.black);
    curx = d.width/3;
    linex = d.width;
    cury = font_ht + 5 - desc;
    wd = d.width;
    ht = d.height;
    Thread t = new Thread(this);
    t.setPriority(Thread.MIN_PRIORITY);
    t.start();
  }

  private final void moveText()
  {
    curx --;
    bufGraphics.setColor(Color.black);
    bufGraphics.drawString(displayString, curx, cury);
  }

  private final void lines()
  {
    linex -= 3;
    bufGraphics.clearRect(linex, 0, wd, ht);
    bufGraphics.setColor(Color.red);
    bufGraphics.drawString(replaceString, linex, cury);
        
    bufGraphics.setColor(Color.blue);
    for (int i=4; i>0; i--)
      {
        bufGraphics.drawLine(linex, cury - 3*i, wd, cury - 3*i);
      }
  }

  public void run()
  {
    try
      {
        long tm = System.currentTimeMillis();
        tm = System.currentTimeMillis();
        moveText();
        int thresh = curx + len;

        while (linex > 0)
          {
            if (linex <= thresh)
              { moveText(); }
            lines();
            paint(getGraphics());
            tm += 20;
            Thread.sleep
              (Math.max(0, tm - System.currentTimeMillis()));
          }
      }
    catch(InterruptedException ex)
      {}

                                // make sure we are at right spot
    linex += 3;
    lines();
  }    

  public void update(Graphics g)
  { paint(g); }

  public void paint(Graphics g)
  {
    if (bufImage != null)
      { g.drawImage(bufImage, 0, 0, this); }
  }
}
