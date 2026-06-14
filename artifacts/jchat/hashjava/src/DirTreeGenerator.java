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

/**
 * Given a directory, generate a list of filenames
 * all under this tree
 */

import java.util.Vector;
import java.io.File;
import java.util.Enumeration;

class DirTreeGenerator
{
  Vector vec = new Vector();

  /**
   * return an Enumeration of all the files from this tree listing.
   * Each element of the enumeration contains a String with the
   * name of the file. Note that directories are also included
   * in this listing.
   */

  Enumeration list()
  { return vec.elements(); }

  private void traverse(File f)
    throws InvalidDirectoryError
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
          { throw new InvalidDirectoryError("Cannot read " + tmp); }
        if (tmp.isDirectory())
          { traverse(tmp); }
      }
  }

  /**
   * Create a new tree generator starting from a given
   * directory.
   * Throws InvalidDirectoryError if this isn't a directory,
   * or if there was trouble opening the directory.
   */
  DirTreeGenerator(String dir) throws InvalidDirectoryError
  {
                                // This is silly, but apparantly
                                // the only thing that works is to
                                // append a separator char for the
                                // directory *sigh*
    if (dir.charAt(dir.length()-1) != File.separatorChar)
      { dir += File.separator; }
    File root = new File(dir);
    if (!root.isDirectory())
      { throw new InvalidDirectoryError(dir + " is not a directory"); }
    traverse(root);
  }
}

