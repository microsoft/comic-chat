import java.io.*;

public class Hex
{

	public static void main(String[] args)
	{
                if (args.length == 0)
                {
                 try
                 {
                 while(true)
                 {
                  System.out.println(System.in.read());
                 }
                 }
                 catch(Exception e){}
                }
		for (int i = 0; i < args.length; i++)
		{
			int num = Integer.parseInt(args[i]);
			System.out.println(Integer.toString(num, 16));
		}
	}

}
