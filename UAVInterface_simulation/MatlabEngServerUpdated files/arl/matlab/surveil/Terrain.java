package arl.matlab.surveil;
import java.util.*;
import java.io.*;


/**1
 * This class reads in the height file and determines where a target can move from any particular cell.
 * 
 * 
 * @author Monica Anderson
 *
 */
/**
 * @author andersonm
 *
 */
/**
 * @author andersonm
 *
 */
public class Terrain {
	
	/**
	 * 2d array that holds the height values for each cell
	 */
	double[] terrain;
	/**
	 * width of map in cells
	 */
	int w;
	/**
	 * height of map in cells
	 */
	int h;
	/**
	 * x offset
	 */
	double startx;
	/**
	 * y offset
	 */
	double starty;
	/**
	 * Resolution of each cell
	 */
	int size;
	/**
	 * how much can a target move up or down in one step
	 */
	int possibleElevationChange=2;
	
	/**
	 * @param heightFile - name of file with height information
	 * @param w - width of height map
	 * @param h - height of height map
	 * @param size - size of each cell
	 * @param startx - starting offset
	 * @param starty - starting offset
	 */
	public Terrain(String heightFile, int w, int h,int size,double startx, double starty){
    	
		terrain = new double[w*h];
    	try {
	    	FileReader in = new FileReader(heightFile);
	    	BufferedReader inLine = new BufferedReader(in);
	    	int i=0;
	    	while ( i < w*h) terrain[i++]=Double.parseDouble(inLine.readLine());
	    	System.out.println(i+" bytes read in");
	    	inLine.close();
    	} catch (Exception e){
    		System.out.println("Problem reading file"+e.getMessage());
    	}

 		this.w=w;
		this.h=h;
		this.startx=startx;
		this.starty=starty;
		this.size=size;
	}
	
	/**
	 * @param x - x position
	 * @param y - y position
	 * @return array of movement choices
	 */
	public Choice[] getChoices(double x, double y){
		List<Choice> list = new ArrayList<Choice>(20);
		double myHeight = getHeight(x,y);
		list.add(new Choice(x,y,Choice.STOPPED));
		if (Math.abs(getHeight(x+size,y)-myHeight)<possibleElevationChange) {
			list.add(new Choice(x+size,y,Choice.RIGHT));
		}
		if (Math.abs(getHeight(x-size,y)-myHeight)<possibleElevationChange) {
			list.add(new Choice(x-size,y,Choice.LEFT));
		}
		if (Math.abs(getHeight(x+size,y+size)-myHeight)<possibleElevationChange) list.add(new Choice(x+size,y+size,Choice.DIAGONAL1));
		if (Math.abs(getHeight(x-size,y-size)-myHeight)<possibleElevationChange) list.add(new Choice(x-size,y-size,Choice.DIAGONAL2));
		if (Math.abs(getHeight(x-size,y+size)-myHeight)<possibleElevationChange) list.add(new Choice(x-size,y+size,Choice.DIAGONAL3));
		if (Math.abs(getHeight(x+size,y-size)-myHeight)<possibleElevationChange) list.add(new Choice(x+size,y-size,Choice.DIAGONAL4));
		if (Math.abs(getHeight(x,y+size)-myHeight)<possibleElevationChange) {
			list.add(new Choice(x,y+size,Choice.UP));
		}
		if (Math.abs(getHeight(x,y-size)-myHeight)<possibleElevationChange) {
			list.add(new Choice(x,y-size,Choice.DOWN));
		}
		
		Choice[] choices = new Choice[list.size()];
		return  list.toArray(choices);
	}
	/**
	 * @param x - x coordinate of cell
	 * @param y - y coordinate of cell
	 * @return height of this cell
	 */
	public double getHeight(double x,double y){
		double xOffset=-startx+x;
		double yOffset=-starty+y;
		int xIdx=(int)xOffset/size;
		int yIdx=(int)yOffset/size;
		
		int idx=((yIdx)*w)+xIdx;
		if (x<this.startx || y<this.starty) return -1*this.possibleElevationChange;
		if (idx <0 || idx > this.terrain.length) return -1*this.possibleElevationChange; 
		return terrain[idx];
	}
	
	public static void main (String[] args){
		Terrain terrain =new Terrain("heights", 73,109,5,329723,4321586.5);
		System.out.println(terrain.getHeight(4321706,329906));
	}

	
}
