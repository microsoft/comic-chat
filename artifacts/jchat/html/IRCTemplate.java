////////////////////////////////////////////////////
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//	IRCTemplate represents an "*.irct" file,
//	and contains methods for building a html page
//	based upon chat history contents.
//
//	Author:
//		Kris Nye
////////////////////////////////////////////////////
import java.io.*;
import java.util.*;

public class IRCTemplate
{

    protected static String CONNECTED = "connected";
    protected static String MESSAGEOFTHEDAY = "messageOfTheDay";
    protected static String INVALIDNICK = "invalidNick";
    protected static String NICKCHANGED = "nickChanged";
    protected static String INVITATIONARRIVED = "invitationArrived";
    protected static String MESSAGEARRIVED = "messageArrived";
    protected static String WHISPERARRIVED = "whisperArrived";
    protected static String MEMBERJOINED = "memberJoined";
    protected static String MEMBERPARTED = "memberParted";
    protected static String MEMBERKICKED = "memberKicked";
    protected static String MEMBERKICKEDFORREASON = "memberKickedForReason";
    protected static String HOST = "host";
    protected static String SPEAKER = "speaker";
    protected static String OBSERVER = "observer";
    protected static String VLOCALNICK = "%localnick%";
    protected static String VLOCALROOM = "%localroom%";
    protected static String VBADNICK = "%badnick%";
    protected static String VOLDNICK = "%oldnick%";
    protected static String VNEWNICK = "%newnick%";
    protected static String VSOURCE = "%source%";
    protected static String VTARGET = "%target%";
    protected static String VMESSAGE = "%message%";
    protected static String VROOM = "%room%";
    protected static String VROOMNOPOUND = "%roomnopound%";
    protected static String VNICK = "%nick%";
    protected static String VKICKER = "%kicker%";
    protected static String VREASON = "%reason%";
    protected static String VLINE = "%line%";
    protected static String VHOST = "%host%";
    protected static String VSPEAKER = "%speaker%";
    protected static String VOBSERVER = "%observer%";
    protected static String VMEMBERCOUNT = "%membercount%";
    protected static String VMEMBERCOUNTPLUS = "%membercount+1%";
    protected static String VTAG = "%tag%";
    protected static String VDATE = "%date%";
    protected static String CHATSTARTTAG = "<chat";
    protected static String REVERSE = "reverse";
    protected static String LENGTH = "length";
    protected static String CHATSTOPTAG = "</chat>";
    protected static String MEMBERSTARTTAG = "<members>";
    protected static String MEMBERSTOPTAG = "</members>";
    protected static String OPENBRACE = "(";
    protected static String CLOSEBRACE = ")";
    protected static String PERCENT = "%";
    protected static String CR = "\r\n";
    protected static String SPACE = " ";
    protected static String EMPTY = "";
    protected static String GT = ">";
    protected static String LT = "<";

    String filename;
    File file;
    Properties lines;
    String page;
    boolean reverse;
    int chatLength;
    boolean memberTagOK;
    boolean chatTagOK;
    long lastModified;

    public void parse(String s)
    {
        page = s;
        int i = s.indexOf(CHATSTARTTAG);
        if(i > 0)
        {
            String s1 = s.substring(i, s.indexOf(GT, i) + 1);
            processChatTag(s1);
        }
        int j = s.indexOf(CHATSTOPTAG);
        chatTagOK = i >= 0 && j >= 0 && i <= j;
        if(chatTagOK)
        {
            String s2 = s.substring(i + CHATSTARTTAG.length(), j);
            s = s.substring(0, i) + CHATSTARTTAG + s.substring(j + CHATSTOPTAG.length());
            page = s;
            for(StringTokenizer stringtokenizer = new StringTokenizer(s2, CR); stringtokenizer.hasMoreTokens();)
            {
                String s3 = stringtokenizer.nextToken();
                if(s3.endsWith(CLOSEBRACE) && stringtokenizer.hasMoreTokens())
                {
                    int i1 = s3.indexOf(OPENBRACE);
                    if(i1 > 1)
                    {
                        String s5 = s3.substring(0, i1);
                        String s7 = stringtokenizer.nextToken();
                        lines.put(s5, s7);
                    }
                }
            }

        }
        else
        {
            System.out.println("Warning: " + filename + " Template does not contain a " + CHATSTARTTAG + " and a " + CHATSTOPTAG + ".");
        }
        int k = s.indexOf(MEMBERSTARTTAG);
        int l = s.indexOf(MEMBERSTOPTAG);
        memberTagOK = k >= 0 && l >= 0 && k <= l;
        if(memberTagOK)
        {
            String s4 = s.substring(k + MEMBERSTARTTAG.length(), l);
            s = s.substring(0, k) + MEMBERSTARTTAG + s.substring(l + MEMBERSTOPTAG.length());
            page = s;
            for(StringTokenizer stringtokenizer1 = new StringTokenizer(s4, CR); stringtokenizer1.hasMoreTokens();)
            {
                String s6 = stringtokenizer1.nextToken();
                int j1 = s6.indexOf(PERCENT);
                int k1 = s6.lastIndexOf(PERCENT);
                if(j1 >= 0 && k1 >= 2)
                {
                    String s8 = s6.substring(j1 + 1, k1);
                    String s9 = s6;
                    lines.put(s8, s9);
                }
            }

        }
        else
        {
            System.out.println("Warning: " + filename + " Template does not contain a " + MEMBERSTARTTAG + " and a " + MEMBERSTOPTAG + ".");
        }
    }


    public IRCTemplate()
    {
        lines = new Properties();
        chatLength = 100;
    }


    public IRCTemplate(String s)
        throws IOException
    {
        lines = new Properties();
        chatLength = 100;
        filename = s;
        file = new File(s);
        load();
    }


    public String connected(String s, String as[])
    {
        String s1 = getTemplateString(CONNECTED);
        if(s1 == null)
            return EMPTY;
        s1 = replaceAll(s1, VHOST, s);
        if(as != null)
        {
            StringBuffer stringbuffer = new StringBuffer();
            String s2 = getTemplateString(MESSAGEOFTHEDAY);
            for(int i = 0; i < as.length; i++)
            {
                String s3 = replaceAll(s2, VLINE, as[i]);
                if(s3 != null)
                {
                    stringbuffer.append(s3);
                    stringbuffer.append(CR);
                }
            }

            s1 = s1 + stringbuffer.toString();
        }
        return s1;
    }


    public String memberJoined(String s, String s1)
    {
        String s2 = getTemplateString(MEMBERJOINED);
        if(s2 == null)
        {
            return null;
        }
        else
        {
            s2 = replaceAll(s2, VROOM, s);
            s2 = replaceAll(s2, VNICK, s1);
            return s2;
        }
    }


    public void processChatTag(String s)
    {
        if(s == null)
            return;
        if(s.indexOf(REVERSE) >= 0)
            reverse = true;
        int i = s.indexOf(LENGTH);
        if(i > 0)
        {
            int j = s.indexOf(GT, i);
            int k = s.indexOf(SPACE, i);
            if(k > 0 && k < j)
                j = k;
            String s1 = s.substring(i, j);
            try
            {
                chatLength = Integer.parseInt(s1.substring(s1.indexOf("=") + 1));
            }
            catch(Exception exception)
            {
                exception.printStackTrace(System.out);
            }
        }
    }


    public String buildChat(Vector vector)
    {
        StringBuffer stringbuffer = new StringBuffer();
        synchronized (vector) 
        {
            if(reverse)
            {
                int i = vector.size();
                for(int k = i - 1; k >= 0; k--)
                {
                    stringbuffer.append((String)vector.elementAt(k));
                    stringbuffer.append(CR);
                }

            }
            else
            {
                int j = vector.size();
                for(int l = 0; l < j; l++)
                {
                    stringbuffer.append((String)vector.elementAt(l));
                    stringbuffer.append(CR);
                }

            }
        }

        return stringbuffer.toString();
    }


    public String messageArrived(String s, String s1, String s2, boolean flag)
    {
        String s3 = getTemplateString(flag ? WHISPERARRIVED : MESSAGEARRIVED);
        if(s3 == null)
        {
            return null;
        }
        else
        {
            s3 = replaceAll(s3, VSOURCE, s);
            s3 = replaceAll(s3, VTARGET, s1);
            s3 = replaceAll(s3, VMESSAGE, s2);
            return s3;
        }
    }


    public String memberParted(String s, String s1)
    {
        String s2 = getTemplateString(MEMBERPARTED);
        if(s2 == null)
        {
            return null;
        }
        else
        {
            s2 = replaceAll(s2, VROOM, s);
            s2 = replaceAll(s2, VNICK, s1);
            return s2;
        }
    }


    public void trim(Vector vector, int i)
    {
        if(i < 0)
            vector.removeAllElements();
        else
            for(; vector.size() > i; vector.removeElementAt(0));
    }


    public String memberKicked(String s, String s1, String s2, String s3)
    {
        String s4 = getTemplateString(s3 != null ? MEMBERKICKEDFORREASON : MEMBERKICKED);
        if(s4 == null)
        {
            return null;
        }
        else
        {
            s4 = replaceAll(s4, VROOM, s);
            s4 = replaceAll(s4, VNICK, s1);
            s4 = replaceAll(s4, VKICKER, s2);
            s4 = replaceAll(s4, VREASON, s3);
            return s4;
        }
    }


    public String getTemplateString(String s)
    {
        return lines.getProperty(s, null);
    }


    public String buildPage(String s, String s1, String s2, Vector vector, Vector vector1, String as[], String as1[], 
            String as2[])
    {
        trim(vector1, chatLength - vector.size());
        transfer(vector, vector1);
        String s3 = new String(page);
        if(chatTagOK)
        {
            String s4 = buildChat(vector1);
            s3 = replace(s3, CHATSTARTTAG, s4);
        }
        if(memberTagOK)
        {
            String s5 = buildMembers(as, as1, as2);
            s3 = replace(s3, MEMBERSTARTTAG, s5);
        }
        int i = (as == null ? 0 : as.length) + (as1 == null ? 0 : as1.length) + (as2 == null ? 0 : as2.length);
        s3 = replace(s3, VMEMBERCOUNT, Integer.toString(i));
        s3 = replace(s3, VMEMBERCOUNTPLUS, Integer.toString(i + 1));
        s3 = replaceAll(s3, VTAG, s);
        s3 = replaceAll(s3, VDATE, new Date().toString());
        s3 = replaceAll(s3, VLOCALNICK, s1);
        s3 = replaceAll(s3, VLOCALROOM, s2);
        if(s2 != null && s2.charAt(0) == '#')
            s3 = replaceAll(s3, VROOMNOPOUND, s2.substring(1));
        return s3;
    }


    public String replace(String s, String s1, String s2)
    {
        int i = s.indexOf(s1);
        if(i >= 0)
            return s.substring(0, i) + s2 + s.substring(i + s1.length());
        else
            return s;
    }


    public boolean isReverse()
    {
        return reverse;
    }


    public void update()
    {
        System.out.println("Updating " + filename);
        if(lastModified != file.lastModified())
        {
            System.out.println("Reloading " + filename);
            try
            {
                load();
            }
            catch(Exception ex) {}
            lastModified = file.lastModified();
        }
    }


    public void load()
        throws IOException
    {
        byte abyte0[] = Server.load(file);
        if(abyte0 == null)
        {
            throw new IOException("Cannot load " + filename);
        }
        else
        {
            lastModified = file.lastModified();
            String s = new String(abyte0);
            parse(s);
            return;
        }
    }


    public String disconnected(String s)
    {
        return EMPTY;
    }


    public String invitationArrived(String s, String s1)
    {
        String s2 = getTemplateString(INVITATIONARRIVED);
        if(s2 == null)
        {
            return null;
        }
        else
        {
            s2 = replaceAll(s2, VSOURCE, s);
            s2 = replaceAll(s2, VROOM, s1);
            return s2;
        }
    }


    public void dump(PrintStream printstream)
    {
        printstream.println(this);
        printstream.println("**PAGE**");
        printstream.println(page);
        printstream.println("**LINES**");
        lines.save(printstream, "");
    }


    public String replaceAll(String s, String s1, String s2)
    {
        if(s == null || s1 == null || s2 == null)
            return s;
        for(int i = s.indexOf(s1); i >= 0; i = s.indexOf(s1, i + s2.length()))
            s = s.substring(0, i) + s2 + s.substring(i + s1.length());

        return s;
    }


    public void transfer(Vector vector, Vector vector1)
    {
        Object obj;
        for(; !vector.isEmpty(); vector1.addElement(obj))
        {
            obj = vector.elementAt(0);
            vector.removeElementAt(0);
        }

    }


    public String invalidNick(String s)
    {
        String s1 = getTemplateString(INVALIDNICK);
        if(s1 == null)
        {
            return null;
        }
        else
        {
            s1 = replaceAll(s1, VBADNICK, s);
            return s1;
        }
    }


    protected String buildMembers(String as[], String as1[], String as2[])
    {
        StringBuffer stringbuffer = new StringBuffer();
        appendMembers(stringbuffer, as, getTemplateString(HOST), VHOST);
        appendMembers(stringbuffer, as1, getTemplateString(SPEAKER), VSPEAKER);
        appendMembers(stringbuffer, as2, getTemplateString(OBSERVER), VOBSERVER);
        return stringbuffer.toString();
    }


    public String nickChanged(String s, String s1)
    {
        String s2 = getTemplateString(NICKCHANGED);
        if(s2 == null)
        {
            return null;
        }
        else
        {
            s2 = replaceAll(s2, VOLDNICK, s);
            s2 = replaceAll(s2, VNEWNICK, s1);
            return s2;
        }
    }


    public void appendMembers(StringBuffer stringbuffer, String as[], String s, String s1)
    {
        if(as == null)
            return;
        for(int i = 0; i < as.length; i++)
        {
            stringbuffer.append(replace(s, s1, as[i]));
            stringbuffer.append(CR);
        }

    }

}
