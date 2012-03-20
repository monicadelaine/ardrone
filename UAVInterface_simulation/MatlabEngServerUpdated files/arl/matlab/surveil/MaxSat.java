package arl.matlab.surveil;

import java.io.*;

public class MaxSat
{

    public MaxSat()
    {
    }

	// Return true in bool[n] if cam n should turn right
	// 		false if cam n should turn left
   public double[][] solveCSP(String file, int numCameras)
   {
      Runtime r = Runtime.getRuntime();

      double[][] ret = {{0.0}};

      try{

         // Create process and get streams and readers
         String ex = "java -jar arl/matlab/surveil/sat4j-maxsat.jar ";
         for(int i = 0; i < file.length(); ++i){
            if(file.charAt(i) == ' '){ // Check if this char is a space
               file = "\"" + file + "\"";
               break;
            }
         }
	      ex += file;
         Process p = r.exec(ex);
         InputStream in = p.getInputStream();
         BufferedInputStream buf = new BufferedInputStream(in);
         InputStreamReader inread = new InputStreamReader(buf);
         BufferedReader buffRead = new BufferedReader(inread);

         // Read output
         String line;
         while((line = buffRead.readLine()) != null){
            if(line.charAt(0) == 'v'){
               String sub = line.substring(2);
		         // Be aware this tranforms from 1-index to 0-index
		         // That is, axiom 2n-1 corresponds to split[2 * (n-1)]
               String[] split = sub.split(" ");
               ret = new double[numCameras][2];
               for(int i = 0; i < numCameras; ++i){ // foreach camera
			   /*
                  for(int j = 1; j < 10; ++j){ // foreach possible action
                     if(split[(i * 9) + (j-1)].charAt(0) != '-'){
                        // Get pan value
                        if(j / 3 == 0 || j / 3 == 3)
                           ret[i][0] = 0;
                        else if(j / 3 == 1)
                           ret[i][0] = 1;
                        else // j / 4 == 2
                           ret[i][0] = -1;

                        // Get tilt value
                        if(j % 3 == 0)
                           ret[i][1] = 0;
                        else if(j % 3 == 1)
                           ret[i][1] = 1;
                        else // j % 3 == 2
                           ret[i][1] = -1;
				*/
				for(int j = 1; j < 5; ++j){ // foreach possible action
                     if(split[(i * 9) + (j-1)].charAt(0) != '-'){
                        // Get pan value
                        if(j / 3 == 0 || j / 3 == 3)
                           ret[i][0] = 0;
                        else if(j / 3 == 1)
                           ret[i][0] = 1;
                        else // j / 4 == 2
                           ret[i][0] = -1;

                        // Get tilt value
                        if(j % 3 == 0)
                           ret[i][1] = 0;
                        else if(j % 3 == 1)
                           ret[i][1] = 1;
                        else // j % 3 == 2
                           ret[i][1] = -1;
                        // Normalize
                        if(Math.abs(ret[i][0])==1 && Math.abs(ret[i][1])==1){
                           ret[i][0] *= Math.sqrt(2) / 2;
                           ret[i][1] *= Math.sqrt(2) / 2;
                        }

                        break; // Found action for this camera, go to next
                     }
                  }
               }

               break; // Found truth value line
            }
         }

         // Check for failure
         try{
            if(p.waitFor() != 0){
               System.err.println("exit value = " + p.exitValue());
               ret[0][0] = 2;
			   }
         }
         catch (InterruptedException e){
            System.err.println(e);
	         ret[0][0] = 3;
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

      // Return actions
      return ret;
   }

   public static void main(String[] args)
   {
	   MaxSat test = new MaxSat();

		// THIS IS NOT A PROPER TEST!!!!
      double[][] ret = test.solveCSP(args[0], Integer.parseInt(args[1]));
      for(int i = 0; i < ret.length; ++i){
         if(ret[i][0] == 1)
            System.out.println("Camera " + i + " should turn right");
         else
            System.out.println("Camera " + i + " should turn left");
      }

      return;
   }
}
