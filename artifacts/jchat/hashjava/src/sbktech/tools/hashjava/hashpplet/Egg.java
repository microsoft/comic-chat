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

// ;-)

package sbktech.tools.hashjava.hashpplet;

import java.awt.*;
import java.util.*;

class Egg extends Panel implements Runnable
{
 // static variables are reachable a bit quicker.
  private static Image offim;
  private static Thread engine;
  private static Color gameColor= new Color(200, 200, 255);
  private static boolean started = false;

  private static Dimension sz = new Dimension(135, 235);
  public Dimension preferredSize()
  { return sz; }
  public Dimension minimumSize()
  { return sz; }

  Egg()
  {
    setFont(new Font("Courier", Font.BOLD, 36));
    setBackground(Color.lightGray);
  }

  void init()
  {
    if (!started)
      {
        started = true;
        Dimension d = size();
        offim = createImage(d.width, d.height);
        engine = new Thread(this);
        engine.setPriority(Thread.MIN_PRIORITY);
        engine.start();
      }
  }

  private final void refreshGame(int bat_c, int bat_u, int ball_x, int ball_y)
  {
    Graphics g = offim.getGraphics();

    // Clear the old board
    g.setColor(Color.black);
    g.fillRect(0, 0, 135, 235);
    g.setColor(gameColor);

    // Draw the boundaries
    g.fillRect(10, 10, 5, 205);
    g.fillRect(115, 10, 5, 205);

    // Draw the centerline
    int xpos = 23;
    while (xpos<110)
      {
        g.fillRect(xpos, 106, 10, 2);
        xpos += 15;
      }

    // Draw the bats
    g.fillRect(bat_c, 5, 20, 5);
    g.fillRect(bat_u, 215, 20, 5);

    // Draw the ball
    g.fillRect(ball_x, ball_y, 5, 5);
  }

  private static int pad_u = 70;

  public boolean handleEvent(Event evt)
  {
    switch(evt.id)
      {
      case Event.MOUSE_DOWN:
      case Event.MOUSE_UP:
      case Event.MOUSE_MOVE:
      case Event.MOUSE_ENTER:
      case Event.MOUSE_EXIT:
      case Event.MOUSE_DRAG:
        mid = evt.x;
        return true;
      default:
        return super.handleEvent(evt);
      }
  }

  private static int mid = 70;

  private final void playGame()
       throws InterruptedException
  {
    int ballx=100, bally=100;
    int vx=3, vy=3;
    int pad_c = 20;
    int bounces = -5;

    long tm = System.currentTimeMillis();

    while (true)
      {
        ballx += vx;
        bally += vy;

        if (ballx < 15)
          { ballx = 15; vx = -vx; }
        else if (ballx > 110)
          { ballx = 110; vx = -vx; }

        if (ballx < (pad_c+10))
          { pad_c-=3; }
        else if (ballx > (pad_c+10))
          { pad_c+=3; }

        if (pad_c <15)
          { pad_c = 15; }
        else if (pad_c > 95)
          { pad_c = 95; }

        if (bally < 10)
          {
            bounces++;
            if (bounces == 0)
              {
                vy = (vy>0)?(vy+1):(vy-1);
                bounces = -5;
              }

            int delta = ballx - pad_c;
            if ((delta < -5) ||
                (delta > 20))
              {
                refreshGame(pad_c, pad_u, ballx, bally);
                repaint();
                u_score++;
                break;
              }
            if (delta < 5)
              { vx--; if (vx == 0) vx = -1; }
            else if (delta > 15)
              { vx++; if (vx == 0) vx = 1; }
            bally = 10; vy = -vy;
          }
        else if (bally > 210)
          {
            bounces++;
            if (bounces == 0)
              {
                vy = (vy>0)?(vy+1):(vy-1);
                bounces = -5;
              }

                                // Check for collision
            int delta = ballx - pad_u;
            if ((delta < -5) ||
                (delta > 20))
              {
                refreshGame(pad_c, pad_u, ballx, bally);
                repaint();
                c_score++;
                break;
              }
            if (delta < 5)
              { vx--; if (vx == 0) vx = -1; }
            else if (delta > 10)
              { vx++; if (vx == 0) vx = 1; }
            bally = 210; vy = -vy;
          }

        if (mid < (pad_u + 8))
          { pad_u -=3; }
        else if (mid > (12 +pad_u))
          { pad_u += 3; }
        if (pad_u < 15)
          { pad_u = 15; }
        else if (pad_u > 95)
          { pad_u = 95; }

        refreshGame(pad_c, pad_u, ballx, bally);
        //        paint(getGraphics());
        repaint();
        tm += 50;
        Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
      }
    repaint();
  }

  public void run()
  {
    try
      {
        fadeToBlack();
        smiley();
        title();
        do  { playGame(); }
        while (scoreAndMore());
      }
    catch (InterruptedException ex)
      { }
    engine = null;
  }

  private static int c_score=0;
  private static int u_score = 0;

  private final boolean scoreAndMore()
       throws InterruptedException
  {
    Graphics g = offim.getGraphics();
    g.setFont(getFont());
    g.setColor(gameColor);
    g.drawString(Integer.toString(c_score), 90, 50);
    g.drawString(Integer.toString(u_score), 90, 150);
    paint(getGraphics());
    //    repaint();
    Thread.sleep(5000);
    return ((c_score < 5)&&(u_score < 5));
  }

  public void update(Graphics g)
  { paint(g); }

  public void paint(Graphics g)
  {
    if (offim != null)
      {
        synchronized(this)
          { g.drawImage(offim, 0, 0, this); }
      }
  }

  private final void fadeToBlack()
       //       throws InterruptedException
  {
    Dimension d = size();
    int wd = d.width, ht = d.height;

    /*
    long tm = System.currentTimeMillis();
    for (int i=255; i>=0; i--)
      {
        Graphics offgr = offim.getGraphics();
        Color c = new Color(i, i, i);
        offgr.setColor(c);
        setBackground(c);
        offgr.fillRect(0, 0, wd, ht);
        repaint();
        // paint(getGraphics());
        tm += 5;
        Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
      }
      */
    setBackground(Color.black);
    Graphics g = offim.getGraphics();
    g.setColor(Color.black);
    g.fillRect(0, 0, wd, ht);
    repaint();
  }

  private final void smiley()
       throws InterruptedException
  {
    slapAndFade(100, Color.green, new Font("Courier", Font.BOLD, 36),
                250, ";-)");
  }

  private final void slapAndFade(int startht, Color startColor,
                                 Font fn, int delay, String string)
       throws InterruptedException
  {
    int ht = startht;

    FontMetrics met = getFontMetrics(fn);

    int tlen = string.length();
    int wd = 20;
    long tm = System.currentTimeMillis();
    for (int i=0; i<tlen; i++)
      {
        Graphics offgr = offim.getGraphics();
        offgr.setColor(startColor);
        offgr.setFont(fn);
        String cur = string.substring(i, i+1);
        if (".".equals(cur))
          {
            ht += met.getHeight();
            wd = 20;
          }
        else
          {
            offgr.drawString(cur, wd, ht);
            wd += met.charWidth(cur.charAt(0));
          }
        // paint(getGraphics());
        repaint();
        tm += delay;
        Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
      }

    StringTokenizer tok = new StringTokenizer(string, ".", false);
    Vector v = new Vector(10);
    while(tok.hasMoreElements())
      { v.addElement(tok.nextToken()); }
    String words[] = new String[v.size()];
    v.copyInto(words);

    Color c = startColor;

    tm = System.currentTimeMillis();
    for (int i=255; i>=0; i--)
      {
        int r = c.getRed();int g = c.getGreen();int b = c.getBlue();
        if (--r < 0) r = 0;  if (--g < 0) g = 0; if (--b < 0) b = 0;
        int wlen = words.length;
        ht = startht;
        synchronized(this)
          {
            Graphics offgr = offim.getGraphics();
            offgr.setFont(fn);
            offgr.setColor(c = new Color(r, g, b));

            for (int j=0; j<wlen; j++)
              {
                offgr.drawString(words[j], 20, ht);
                ht += met.getHeight();
              }
          }
            // paint(getGraphics());
        repaint();

        tm += 5;
        Thread.sleep(Math.max(0, tm - System.currentTimeMillis()));
      }
  }
  private final void title()
       throws InterruptedException
  {
    slapAndFade(30, gameColor,
                new Font("Dialog", Font.BOLD, 24), 100,
                "`` Avoid. missing. ball. for. high. score ''");
  }
  /*
  public static void main(String a[])
  {
    Frame f = new Frame("test");
    Egg egg = new Egg();
    f.setLayout(new BorderLayout());
    f.resize(135, 235);
    f.add("Center", egg);
    f.show();
    egg.init();
  } */
}
