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

package sbktech.tools.hashjava.util;

import java.util.*;
import java.io.*;

/**
 * This is a simpler version of java.util.Properties because of
 * bugs in some versions that do not save or read unicode character
 * strings properly.
 */

public class Props extends Hashtable
{
  private int lineNo = 1;
  public void load(InputStream in)
       throws IOException
  {
    int ch = in.read();
    while(true)
      {
        switch(ch)
          {
          case -1:
            return;

          case '\n':
            lineNo++;
          case '\r':
            ch = in.read();
            continue;
          }

        StringBuffer key = new StringBuffer();
        while ((ch >=0) && (ch != '=') &&
               (ch != ' ') && (ch != '\t') &&
               (ch!= '\n') && (ch!= '\r'))
          {
            key.append((char)ch);
            ch = in.read();
          }

        while ((ch == ' ') || (ch == '\t'))
          { ch = in.read(); }

        if (ch == '=') { ch = in.read(); }
        while ((ch == ' ') || (ch == '\t'))
          { ch = in.read(); }

        StringBuffer val = new StringBuffer();
        while ((ch>=0) && (ch!='\n') && (ch != '\r'))
          {
            if (ch == '\\')
              {
                if (in.read() != 'u')
                  {
                    throw
                      new IOException
                      ("Only \\uXXXX escape characters are allowed (line " +
                       lineNo + ")");
                  }
                int d = 0;
                for (int i=0; i<4; i++)
                  {
                    ch = in.read();
                    if ((ch >= '0') && (ch <= '9'))
                      { d = (d<<4) + ch - '0'; }
                    else if ((ch >= 'a') && (ch <= 'f'))
                      { d = (d<<4) + ch - 'a' + 10; }
                    else if ((ch >= 'A') && (ch <= 'F'))
                      { d = (d<<4) + ch - 'A' + 10; }
                    else
                      { throw new IOException("Only \\uXXXX escape sequences permitted (line " + lineNo + ")"); }
                  }
                ch = d;
              }
            val.append((char) ch);
            ch = in.read();
          }
        put(key.toString(), val.toString());
      }
  }
  public void save(OutputStream out)
       throws IOException
  {
    for (Enumeration e = keys(); e.hasMoreElements();)
      {
        String key = (String)(e.nextElement());
        print(out, key);
        out.write('=');
        String val = (String)get(key);
        print(out, val);
        print(out, ls);
      }
  }

  private static String ls =
  System.getProperties().getProperty("line.separator", "\n");

  private void print(OutputStream out, String data)
       throws IOException
  {
    int len = data.length();
    for (int i=0; i<len; i++)
      {
        int ch = data.charAt(i);
        if (ch > 0x7f)
          {
            out.write('\\');
            out.write('u');
            out.write(hexDigit[(ch >> 12) & 0xF]);
            out.write(hexDigit[(ch >>  8) & 0xF]);
            out.write(hexDigit[(ch >>  4) & 0xF]);
            out.write(hexDigit[(ch >>  0) & 0xF]);
          }
        else
          { out.write(ch); }
      }
  }
  private static char[] hexDigit = {
    '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
  };
}
