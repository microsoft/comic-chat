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
import java.io.*;
import java.util.*;

/**
 * This generates names that are valid java unicode characters.
 * @author $Author: kbs $
 */

public class UnicodeGenerator extends NameGenerator
{
  // This attempts to generate valid java unicode characters.
  // This is organized as a sequence of ranges. 
  // min1, max1, min2, max2... both ends inclusive.
  // Store this as a String and to an arrayCopy to convert back to
  // an array to save time loading this guy.

  private final static String range=
  "\u0041\u005A\u0061\u007A\u00C0\u00D6\u00D8\u00F6\u00F8\u01F5\u01FA\u0217\u0250\u02A8\u02B0\u02DE\u02E0\u02E9\u0300\u0345\u0360\u0361\u0374\u0375\u037A\u037A\u037E\u037E\u0384\u038A\u038C\u038C\u038E\u038E\u038F\u03A1\u03A3\u03CE\u03D0\u03D6\u03DA\u03DA\u03DC\u03DC\u03DE\u03DE\u03E0\u03E0\u03E2\u03F3\u0401\u040C\u040E\u044F\u0451\u045C\u045E\u0486\u0490\u04C4\u04C7\u04C8\u04CB\u04CC\u04D0\u04EB\u04EE\u04F5\u04F8\u04F9\u0531\u0556\u0559\u055F\u0561\u0587\u0589\u0589\u05B0\u05B9\u05BB\u05C3\u05D0\u05EA\u05F0\u05F4\u060C\u060C\u061B\u061B\u061F\u061F\u0621\u0621\u0622\u063A\u0640\u0652\u066A\u066D\u0670\u06B7\u06BA\u06BE\u06C0\u06CE\u06D0\u06ED\u0901\u0903\u0905\u0939\u093C\u094D\u0950\u0954\u0958\u0965\u0970\u0970\u0981\u0983\u0985\u098C\u098F\u0990\u0993\u09A8\u09AA\u09B0\u09B2\u09B2\u09B6\u09B9\u09BC\u09BC\u09BE\u09BE\u09BF\u09C4\u09C7\u09C8\u09CB\u09CD\u09D7\u09D7\u09DC\u09DD\u09DF\u09E3\u09F0\u09FA\u0A02\u0A02\u0A05\u0A0A\u0A0F\u0A10\u0A13\u0A28\u0A2A\u0A30\u0A32\u0A33\u0A35\u0A36\u0A38\u0A39\u0A3C\u0A3C\u0A3E\u0A3E\u0A3F\u0A42\u0A47\u0A48\u0A4B\u0A4D\u0A59\u0A5C\u0A5E\u0A5E\u0A70\u0A74\u0A81\u0A83\u0A85\u0A8B\u0A8D\u0A8D\u0A8F\u0A8F\u0A90\u0A91\u0A93\u0AA8\u0AAA\u0AB0\u0AB2\u0AB3\u0AB5\u0AB9\u0ABC\u0AC5\u0AC7\u0AC9\u0ACB\u0ACD\u0AD0\u0AD0\u0AE0\u0AE0\u0B01\u0B03\u0B05\u0B0C\u0B0F\u0B10\u0B13\u0B28\u0B2A\u0B30\u0B32\u0B33\u0B36\u0B39\u0B3C\u0B43\u0B47\u0B48\u0B4B\u0B4D\u0B56\u0B57\u0B5C\u0B5D\u0B5F\u0B61\u0B70\u0B70\u0B82\u0B83\u0B85\u0B8A\u0B8E\u0B90\u0B92\u0B95\u0B99\u0B9A\u0B9C\u0B9C\u0B9E\u0B9F\u0BA3\u0BA4\u0BA8\u0BAA\u0BAE\u0BB5\u0BB7\u0BB9\u0BBE\u0BC2\u0BC6\u0BC8\u0BCA\u0BCD\u0BD7\u0BD7\u0BF0\u0BF2\u0C01\u0C03\u0C05\u0C0C\u0C0E\u0C10\u0C12\u0C28\u0C2A\u0C33\u0C35\u0C39\u0C3E\u0C44\u0C46\u0C48\u0C4A\u0C4D\u0C55\u0C56\u0C60\u0C61\u0C82\u0C83\u0C85\u0C8C\u0C8E\u0C90\u0C92\u0CA8\u0CAA\u0CB3\u0CB5\u0CB9\u0CBE\u0CC4\u0CC6\u0CC8\u0CCA\u0CCD\u0CD5\u0CD6\u0CDE\u0CDE\u0CE0\u0CE1\u0D02\u0D03\u0D05\u0D0C\u0D0E\u0D10\u0D12\u0D28\u0D2A\u0D39\u0D3E\u0D43\u0D46\u0D48\u0D4A\u0D4D\u0D57\u0D57\u0D60\u0D61\u0E01\u0E3A\u0E3F\u0E4F\u0E5A\u0E5B\u0E81\u0E82\u0E84\u0E84\u0E87\u0E88\u0E8A\u0E8A\u0E8D\u0E8D\u0E94\u0E97\u0E99\u0E9F\u0EA1\u0EA3\u0EA5\u0EA5\u0EA7\u0EA7\u0EAA\u0EAB\u0EAD\u0EB9\u0EBB\u0EBD\u0EC0\u0EC4\u0EC6\u0EC6\u0EC8\u0ECD\u0EDC\u0EDD\u10A0\u10C5\u10D0\u10F6\u10FB\u10FB\u1100\u1159\u115F\u11A2\u11A8\u11F9\u1E00\u1E9A\u1EA0\u1EF9\u1F00\u1F15\u1F18\u1F1D\u1F20\u1F45\u1F48\u1F4D\u1F50\u1F57\u1F59\u1F59\u1F5B\u1F5B\u1F5D\u1F5D\u1F5F\u1F7D\u1F80\u1FB4\u1FB6\u1FC4\u1FC6\u1FD3\u1FD6\u1FDB\u1FDD\u1FEF\u1FF2\u1FF4\u1FF6\u1FFE\u3041\u3094\u3099\u309E\u30A1\u30FE\u3105\u312C\u3131\u318E\u3190\u319F\u3200\u321C\u3220\u3243\u3260\u327B\u327F\u32B0\u32C0\u32CB\u32D0\u32FE\u3300\u3376\u337B\u33DD\u33E0\u33FE\u3400\u9FA5\uF900\uFA2D\uFB00\uFB06\uFB13\uFB17\uFB1E\uFB36\uFB38\uFB3C\uFB3E\uFB3E\uFB40\uFB41\uFB43\uFB44\uFB46\uFBB1\uFBD3\uFD3F\uFD50\uFD8F\uFD92\uFDC7\uFDF0\uFDFB\uFE70\uFE72\uFE74\uFE74\uFE76\uFEFC\uFF21\uFF3A\uFF41\uFF5A\uFF66\uFFBE\uFFC2\uFFC7\uFFCA\uFFCF\uFFD2\uFFD7\uFFDA\uFFDC";
  private final static char rangeArray[] = range.toCharArray();

  private int rangeIdx = 0;     // Which of the ranges are we in

                                // The actual unicode portion to be returned.
  private char idIdx = rangeArray[0];

                                // A prefix used after symbols have wrapped
                                // around.
  private String prefix = null;

  public String nextName(String s)
  {
    String ret = null;

    char rmax = rangeArray[rangeIdx+1];

    if (idIdx > rmax)
      {
                                // bump up range
        rangeIdx += 2;
        if (rangeIdx >= rangeArray.length)
          {
                                // reset everything
            if (prefix != null)
              { prefix += "_"; }
            else
              { prefix = "_"; }
            rangeIdx = 0;
          }
        idIdx = rangeArray[rangeIdx];
      }

    ret = String.valueOf(idIdx);
    if (prefix != null)
      { ret = prefix + ret; }
    idIdx ++;
    return ret;
  }
}
