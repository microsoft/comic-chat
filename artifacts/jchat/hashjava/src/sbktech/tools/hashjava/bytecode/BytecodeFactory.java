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

package sbktech.tools.hashjava.bytecode;
import java.io.*;

/**
 * This interface is used to enable the environment to read or write
 * bytecode streams when necessary
 *
 * @author $Author: kbs $
 * @version $Revision: 1.2 $
 */

public abstract interface BytecodeFactory
{
  /**
   * This method should be implemented so that the environment
   * can get hold of a way to read classes based on current input.
   * Returning a null tells the environment to ignore reading in
   * the class data. You can also add information with the addClass()
   * method in Environment.
   * @param className name for the class to be loaded
   * @exception IOexception should throw any subclass of IOException if the
   * class cannot be loaded
   * @see Environment#addClass
   */
  DataInput readClass(String className)
       throws IOException;
  /**
   * This one lets the environment write out class files when its
   * finished obfuscating classes. This is the only way provided
   * to write out classes, so you <em>must</em> do something
   * useful here to get the altered classes.
   * @param className name for the class that needs to be written out.
   * @exception IOException should throw any subclass of IOException if
   *            the class cannot be written out
   * @see Environment#dump
   */
  DataOutput writeClass(String className)
       throws IOException;
}
