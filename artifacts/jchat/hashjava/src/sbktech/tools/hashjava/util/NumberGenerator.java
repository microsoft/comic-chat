/*
 * Copyright (C) 1997 KB Sriram
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

import java.util.*;

/**
 * This generates names that are sequences of numbers like _<number>
 * @author $Author: kbs $
 */


public class NumberGenerator extends NameGenerator
{
  private int n=0;              // integer rep of actual string returned.

  public String nextName(String s)
  { return "_" + n++; }
}
