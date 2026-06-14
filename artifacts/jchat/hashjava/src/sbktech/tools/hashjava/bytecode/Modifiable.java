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

/**
 * A Modifiable class contains a symbol that can be renamed. You can
 * examine and alter the symbols name with this interface. There
 * are only three types of modifiable objects -- classnames, fields
 * in a class and methods in a class.
 * @see ClassInfo#definedFields
 * @see ClassInfo#definedMethods
 * @author $Author: kbs $
 * @version $Revision: 1.2 $
 */

public interface Modifiable
{
 /**
  * get the access permissions for this symbol
  * @return an integer (see values in VMConstants)
  *         with the access permissions for this symbol
  * @see VMConstants
  */
  public abstract int permissions();

 /**
  * Get the original name for this Symbol
  */
  public abstract String originalName();

  /**
   * Get the (possibly renamed) name for this Symbol
   */
  public abstract String newName();

  /**
   * Rename the symbol
   * @param newName set the new name for this symbol
   */
  public abstract void rename(String newName);
}
