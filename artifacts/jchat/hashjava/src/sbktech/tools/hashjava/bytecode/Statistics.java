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
 * This is just a way to let informational messages while
 * performing things propagate back to the user.
 *
 * <p>At some time, maybe we can add enough additional information
 * about the process so it justifies its name :)
 * @author $Author: kbs $
 * @version $Revision: 1.2 $
 */

public interface Statistics
{
  /**
   * This is called to provide extremely detailed/debugging information
   */
  void verboseMessage(String s);

  /**
   * This is less detailed, more to announce high-level passes
   */
  void info(String s);
}
