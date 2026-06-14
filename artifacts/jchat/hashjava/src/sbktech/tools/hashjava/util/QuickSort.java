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

public class QuickSort
{
  /**
   * @param v is a Vector of (end - start + 1) Sortable elements
   * @param start is the first index of the entries to be sorted
   * @param end is the last index of the entries to be sorted.
   */
  public static void sort(Vector v, int start, int end)
  {
    int lo = start;
    int hi = end;
    if (hi > lo)
      {
        Sortable mid = (Sortable) v.elementAt((lo+hi)/2);

        while (lo <= hi)
          {
            while ((lo < end) &&
                   ((Sortable)(v.elementAt(lo))).lessThan(mid))
              ++lo;
            while ((hi > start) &&
                   (mid.lessThan(v.elementAt(hi))))
              hi--;
            if (lo <= hi)
              {
                Object tmp = v.elementAt(lo);
                v.setElementAt(v.elementAt(hi), lo);
                v.setElementAt(tmp, hi);
                lo++;
                hi--;
              }
          }

                                // sort the halves
        if (start < hi) sort(v, start, hi);
        if (lo < end) sort(v, lo, end);
      }
  }
}
