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

package sbktech.tools.hashjava.util;


import java.io.*;
import java.util.*;

/**
 * Overcome <em>STILL</em> missing parts of java packages. *Sigh*
 */

public class ZipReader
{
  /**
   * Extract files from a zip uncompressed archive
   */
  public ZipReader(String path)
       throws IOException
  {
    //    System.out.println("Reading " + path);
    raf = new RandomAccessFile(path, "r");
    long len = raf.length();

    while (raf.getFilePointer() < len)
      {
        int sig = raf.readInt();
        switch(sig)
          {
          case SIG_DIR:
                                // Don't really need this stuff
            return;

          case SIG_FILE:
            readFile();
            break;

          case SIG_EDIR:
                                // don't really need this
            return;

          default:
            System.out.println("Unknown signature " +
                               Integer.toHexString(sig) + " (" + sig + ")");
            return;
          }
      }
  }
  /**
   * List files in the archive
   */
  public Enumeration list()
  { return files.keys(); }

  /**
   * Return a DataInput given a file path name in the archive
   * will return a null if this file is not present in the archive
   */
  public DataInput getFile(String pathname)
       throws IOException
  {
    Long pos;
    if ((pos = (Long)(files.get(pathname))) == null)
      { return null; }

                                // We perform an unsightly hack
                                // to avoid allocating more memory
    raf.seek(pos.longValue());
    return raf;
  }

                                // Read an end directory entry
  private final void readEDir()
       throws IOException
  {
                                // skip to comment length
    raf.skipBytes(16);
    byte cname[] = new byte[swap(raf.readShort())];
    raf.readFully(cname);
  }
                                // Read a directory entry
  private final void readDir()
       throws IOException
  {
                                // Skip to file lengths
    raf.skipBytes(16);
    int compSize = swap(raf.readInt());
    int uncompSize = swap(raf.readInt());
    short fnamelen = swap(raf.readShort());
    short extralen = swap(raf.readShort());
    short comlen = swap(raf.readShort());
                                // Skip other uninteresting stuff
    raf.skipBytes(12);

    byte fname[] = new byte[fnamelen];
    raf.readFully(fname);
    String cdS = new String(fname, 0, 0, fname.length);
    raf.skipBytes(extralen + comlen);
  }

                                // Read a file header
  private final void readFile()
       throws IOException
  {
                                // Skip to file size position
    raf.skipBytes(14);
    int compressSize = swap(raf.readInt());
    int uncompressSize = swap(raf.readInt());

    //    System.out.print(uncompressSize);

    if (compressSize != uncompressSize)
      { throw new IOException("Sorry, zipfile contains compressed files"); }

    byte fname[] = new byte[swap(raf.readShort())];
    short extralen = swap(raf.readShort());

    raf.readFully(fname);
    String fnameS = new String(fname, 0, 0, fname.length);
    //    System.out.println(" " + fnameS);

                                // Store position of file
    Long pos= new Long(raf.getFilePointer());
    files.put(fnameS, pos);

                                // Skip over file
    raf.skipBytes(uncompressSize + extralen);
  }

  private static final short swap(short x)
  {
    return (short)(((short)((x & 0x00ff) << 8)) +
                   ((short)((x >> 8) & 0x00ff)));
  }

  private static final int swap(int x)
  {
    int low = x & 0xffff;
    int hi = (x >> 16) & 0xffff;

    low = (swap((short) low) & 0xffff);
    hi = (swap((short) hi) & 0xffff);
    return ((low << 16) + hi);
  }

  public static final int SIG_FILE = 0x504b0304;
  public static final int SIG_DIR = 0x504b0102;
  public static final int SIG_EDIR = 0x504b0506;

  private RandomAccessFile raf;
  private Hashtable files = new Hashtable();
}
