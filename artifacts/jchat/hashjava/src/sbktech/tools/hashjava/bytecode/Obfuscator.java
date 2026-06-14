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
 * The Obfuscater is used to perform renaming of symbols.
 * Various implementations of this interface can choose to use
 * different renaming schemes, or selectively rename classes
 * from the environment.
 *
 * <p>How does this work? After you've added all the classes
 * into an Environment and called obfuscate() in it, the environment
 * calls this interface with a ClassInfo instance for
 * every class in the environment.
 * <p>As you get each ClassInfo, peek and modify the modifiable
 * symbols with the definedFields() and definedMethods() methods
 * in ClassInfo. The general structure of any obfuscator
 * implementation would probably look like
 * <hr>
 * <pre>
 * public obfuscate(ClassInfo cinfo)
 * {
 *   if (&lt;This class should not be altered&gt;) return;
 *
 *   if (&lt;class name should be changed&gt;)
 *      cinfo.rename(&lt;new name&gt;);
 *   if (&lt;fields should be changed&gt;)
 *   {
 *     Modifiable mods[] = cinfo.definedFields();
 *     for (int i=0; i&lt;mods.length; i++)
 *     {
 *        mods[i].rename(&lt;new name&gt;);
 *     }
 *   }
 *   if (&lt;methods should be changed&gt;)
 *   {
 *     Modifiable mods[] = cinfo.definedMethods();
 *     for (int i=0; i&lt;mods.length; i++)
 *     {
 *        mods[i].rename(&lt;new name&gt;);
 *     }
 *   }
 * }
 * </pre>
 * <hr>
 * @see Modifiable
 * @see ClassInfo
 * @author $Author: kbs $
 * @version $Revision: 1.2 $
 */


public interface Obfuscator
{
  /**
   * Obfuscator implementations should implement this method to enable
   * various renaming schemes.
   * @param cinfo This contains the class to be handled. The
   *            Environment calls this method once for each
   *            class that needs to be obfuscated.
   */
  public abstract void obfuscate(ClassInfo cinfo);
}
