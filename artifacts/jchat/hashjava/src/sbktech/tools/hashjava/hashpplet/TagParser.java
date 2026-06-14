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

import java.io.*;
import java.util.*;
import java.net.*;

// Get applet tags from an input stream

class TagParser
{
  private Vector things = new Vector();
  private InputStream in;
  private OutputStream out;
  private int peekC;

  TagParser(InputStream in, OutputStream out)
       throws IOException
  {
    this.in = in;
    this.out = out;
    int c = advance();
    while (c >= 0)
      {
        if (c == '<')
          {
            if (advance() != '/')
              {
                if ("applet".equalsIgnoreCase(parseId()))
                  {
                    things.addElement(parseTags());
                  }
              }
          }
        c = advance();
      }
  }

  Enumeration elements()
  { return things.elements(); }

  private String parseId()
       throws IOException
  {
    StringBuffer ret = new StringBuffer();

    for (int c = peek();
         ((c >= 0) &&
          ((c >= 'A') &&
           (c <= 'Z')) ||
          ((c >= 'a') &&
           (c <= 'z')) ||
          ((c >= '0') &&
           (c <= '9')) ||
          (c == '_') ||
          (c == '.'));
         c = advance())
      { ret.append((char)c); }
    return new String(ret);
  }
          
  private Hashtable parseTags()
       throws IOException
  {
    Hashtable ret = new Hashtable();

    skipws();

    int c;
    String id=null, val=null;

    while(((c = peek()) >=0) &&
          (c != '>'))
      {
        id = parseId();
        skipws();
        if (peek() == '=')
          {
            StringBuffer valb = new StringBuffer();
            advance();
            int quote = -1;
            if (((c = peek()) == '\'') ||
                (c == '\"'))
              { quote = c; c = advance();  }
            while((c >= 0) &&
                  (((quote == -1) &&
                    (c != ' ') &&
                    (c != '\t') &&
                    (c != '\n') &&
                    (c != '\r') &&
                    (c != '>'))
                   ||
                   ((quote >= 0) && (c != quote))))
              { valb.append((char) c); c = advance(); }
            if ((c == quote) && (quote != -1))
              { c = advance(); }
            skipws();
            val = new String(valb);
            ret.put(id.toLowerCase(), val);
          }
      }
    return ret;
  }

  private final void skipws()
       throws IOException
  {
    int c;
    c = peek();
    while ((c >= 0) &&
           ((c == ' ') ||
            (c == '\t') ||
            (c == '\r') ||
            (c == '\n')))
      { c = advance(); }
  }
  private final int peek()
  { return peekC; }
  private final int advance()
       throws IOException
  {
    peekC = in.read();
    if (peekC != -1)
      { out.write(peekC); }
    return peekC;
  }
}
