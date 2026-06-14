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
import java.io.*;
import java.net.*;

class HttpFileLocator implements FileLocator
{
  private URL baseURL = null;

  HttpFileLocator(URL u)
  {
    try
      {
        String s = u.toString();
        if (s.charAt(s.length()-1) == '/')
          { baseURL = u; }
        else
          { baseURL = new URL(s + "/"); }
        //        System.out.println("Base URL is " + baseURL);
      }
    catch(Exception ex)
      { }
  }

  public DataInput getFile(String fileName)
  {
    try
      {
        URL u = new URL(baseURL, fileName);
        //        System.out.println("http req: " + u);
        return (new DataInputStream(u.openStream()));
      }
    catch(Exception ex)
      { }
    return null;
  }
}
