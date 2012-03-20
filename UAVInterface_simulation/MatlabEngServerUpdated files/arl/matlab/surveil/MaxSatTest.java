package arl.matlab.surveil;

import java.io.*;

public class MaxSatTest
{

    public MaxSatTest()
    {
    }

	// Return true in bool[n] if cam n should turn right
	// 		false if cam n should turn left
    public boolean[] solveCSP(String file, int numCameras)
    {
        Runtime r = Runtime.getRuntime();

        boolean[] ret = {false};

        try{

            // Create process and get streams and readers
            String ex = "java -jar sat4j-maxsat.jar " + file;
			for(int i = 0; i < file.length(); ++i){
				if(file.charAt(i) == ' '){ // Check if this char is a space
					file = "\"" + file + "\"";
					break;
				}
			}
			System.out.println(file);
            Process p = r.exec(ex);
            InputStream in = p.getInputStream();
            BufferedInputStream buf = new BufferedInputStream(in);
            InputStreamReader inread = new InputStreamReader(buf);
            BufferedReader buffRead = new BufferedReader(inread);

            // Read output
            String line;
            boolean cont = true;
            while((line = buffRead.readLine()) != null && cont){
                if(line.charAt(0) == 'v'){
                    String sub = line.substring(2);
                    String[] split = sub.split(" ");
                    ret = new boolean[numCameras];
                    for(int i = 0; i < numCameras; ++i){
                        if(split[i * 2].charAt(0) == '-')
                            ret[i] = false;
                        else
                            ret[i] = true;
                    }
                    cont = false;
                }
            }

            // Check for failure
            try{
                if(p.waitFor() != 0)
                    System.err.println("exit value = " + p.exitValue());
            }
            catch (InterruptedException e){
                System.err.println(e);
            }
            finally {
                // Close the streams
                buffRead.close();
                inread.close();
                buf.close();
                in.close();
            }
        }
        catch (IOException e){
            System.err.println(e.getMessage());
            System.exit(1);
        }
        return ret;
    }

    public static void main(String[] args)
    {
		MaxSatTest test = new MaxSatTest();

        boolean[] ret = test.solveCSP(args[0], Integer.parseInt(args[1]));
        for(int i = 0; i < ret.length; ++i){
            if(ret[i])
                System.out.println("Camera " + i + " should turn right");
            else
                System.out.println("Camera " + i + " should turn left");
        }

        return;
    }
}
