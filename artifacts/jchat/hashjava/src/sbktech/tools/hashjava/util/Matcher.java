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

/**
 * Simple backtracking matcher, understands wild chars "*" and "?" only
 */

public abstract class Matcher
{
  private Matcher next = null;

  // This one is implemented by the subclasses
  abstract boolean match(String s, int fromWhere);

  // This is used by subclasses to hand off the
  // check to subsequent matchers in the list.

  protected final boolean nextMatch(String s, int fromWhere)
  {
    int slen = s.length();

    if (s.length() <= fromWhere)
      {
        if (next == null) return true;
        return next.match(s, fromWhere);
      }

    return ((next != null) && next.match(s, fromWhere));
  }

  /**
   * See if this string matches the current matcher
   */
  public boolean match(String s)
  {  return match(s, 0); }

  /**
   * Create a new Matcher from a pattern. The only special
   * characters are * and ?, and they have their usual meanings.
   * This is the only way to create new instances of a Matcher.
   */
  public synchronized static Matcher compile(String pat)
  {
    ret = null; tail = null;
    int len = pat.length();
    int cstart = 0, cend = 0;

    for (int i=0; i<len; i++)
      {
        char t = pat.charAt(i);
        Matcher tmp;

        switch(t)
          {
          case '*':
            if (cend > cstart)
              { append(new StringMatcher(pat.substring(cstart, cend))); }
            append(new StarMatcher());
            cend++;
            cstart = cend;
            break;
          case '?':
            if (cend > cstart)
              { append(new StringMatcher(pat.substring(cstart, cend))); }
            cend++;
            cstart = cend;
            append(new QMatcher());
            break;
          default:
            cend++;
            break;
          }
      }

    if (cend > cstart)
      { append(new StringMatcher(pat.substring(cstart, cend))); }
    return ret;
  }

  private static Matcher ret, tail;

  private static void append(Matcher end)
  {
    if (tail != null)
      { tail.next = end; tail = end; }
    else
      { tail = ret = end; }
  }
}

// Match a substring
class StringMatcher extends Matcher
{
  public String toString()
  { return s; }

  private String s;
  private int len;

  StringMatcher(String s)
  {
    this.s = s;
    this.len = s.length();
  }

  boolean match(String tar, int start)
  {
    if ((start + len) > tar.length()) return false;

    return
      (tar.regionMatches(false, start, s, 0, len) &&
       nextMatch(tar, start+len));
  }
}

class StarMatcher extends Matcher
{
  boolean match(String s, int start)
  {
    int len = s.length();

    //    System.out.println(">>Starmatch on " + s.substring(start, s.length()));
    for (int i=len; i>=start; i--)
      {
        //        System.out.println(">>Starmatch [test " + s.substring(i, s.length()));
        if (nextMatch(s, i))
          {
            //            System.out.println("<<Starmatch on " + s.substring(start, s.length()) +" true");
            return true;
          }
      }
    //    System.out.println("<<Starmatch on " + s.substring(start, s.length()) +" false");
    return false;
  }
}

class QMatcher extends Matcher
{
  boolean match(String s, int start)
  {
    if (s.length() <= start) return false;
    //    System.out.println(">>Qmatch on " + s.substring(start, s.length()));

    boolean ret = nextMatch(s, start+1);
    //    System.out.println("<<Qmatch on " + s.substring(start, s.length()) + " " + ret);
    return ret;
  }
}
