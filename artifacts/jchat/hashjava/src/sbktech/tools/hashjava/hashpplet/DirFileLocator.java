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

class DirFileLocator implements FileLocator
{
  String rootDir;

  DirFileLocator(String rootName)
  {
    // Avoid bugs with VM implementations that alter the
    // working directory.

    if (".".equals(rootName))
      { rootName = System.getProperty("user.dir", "."); }

    if (rootName.endsWith(File.separator))
      { rootDir = rootName.substring(0, rootName.length()-1); }
    else
      { rootDir = rootName; }
  }

  public DataInput getFile(String fileName)
  {
    fileName = fileName.replace('/', File.separatorChar);

    File f = new File(rootDir, fileName);
    // System.out.println("Dir file " + f);
    if (f.canRead())
      {
        try
          {
            // System.out.println("Dir locator " + rootDir + " found " + f);
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
}
