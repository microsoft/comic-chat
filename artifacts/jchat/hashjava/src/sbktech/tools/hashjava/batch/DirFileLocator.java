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

package sbktech.tools.hashjava.batch;

import sbktech.tools.hashjava.util.*;
import java.io.*;
import java.net.*;
import java.util.*;

class DirFileLocator implements FileLocator
{
  String rootDir;

  DirFileLocator(String rootName, boolean generateList)
       throws IOException
  {
    if (rootName.endsWith(File.separator))
      { rootDir = rootName.substring(0, rootName.length()-1); }
    else
      { rootDir = rootName; }

    File root = new File(rootDir);
    if (!root.isDirectory())
      { throw new IOException(rootDir + " is not a directory"); }
    if (generateList)
      { vec = new Vector(); traverse(root); }
  }

  private Vector vec = null;

  public Enumeration list()
  { return vec.elements(); }

  public DataInput getFile(String fileName)
  {
    fileName = fileName.replace('/', File.separatorChar);
    // First try it as-is
    File f = new File(fileName);
    // Otherwise, from the root directory
    if (!f.canRead())
      { f = new File(rootDir, fileName); }
    //    System.out.println("Dir file " + f);
    if (f.canRead())
      {
        try
          {
            return
              new DataInputStream
              (new BufferedInputStream
               (new FileInputStream(f)));
          }
        catch(Exception ex)
          { }
      }
    return null;
  }

  private void traverse(File f)
    throws IOException
  {
    String base = f.getPath();
    String l[] = f.list();
    int len = l.length;

    for (int i=0; i<len; i++)
      {
        String fname;
        if (!base.endsWith(File.separator))
          { fname = base + File.separator + l[i]; }
        else
          { fname = base + l[i]; }
        vec.addElement(fname);
        File tmp = new File(fname);
        if (!tmp.canRead())
          { throw new IOException("Cannot read " + tmp); }
        if (tmp.isDirectory())
          { traverse(tmp); }
      }
  }
}
