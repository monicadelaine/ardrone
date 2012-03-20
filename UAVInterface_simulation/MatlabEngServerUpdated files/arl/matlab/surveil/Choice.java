package arl.matlab.surveil;

/**
 * Class to hold a movement choice
 * 
 * @author Monica Anderson
 *
 */
public class Choice {
	/**
	 * new x coordinate 
	 */
	double X;
	/**
	 * new y coordinate
	 */
	double Y;
	/**
	 * direction from current position
	 */
	int direction;
	/**
	 * 
	 */
	final static int LEFT=0;
	/**
	 * 
	 */
	final static int RIGHT=1;
	/**
	 * 
	 */
	final static int UP=2;
	/**
	 * 
	 */
	final static int DOWN=3;
	/**
	 * 
	 */
	final static int DIAGONAL1=4;
	/**
	 * 
	 */
	final static int DIAGONAL2=5;
	/**
	 * 
	 */
	final static int DIAGONAL3=6;
	/**
	 * 
	 */
	final static int DIAGONAL4=7;
	/**
	 * 
	 */
	final static int STOPPED=8;
	/**
	 * @param X
	 * @param Y
	 * @param direction
	 */
	public Choice(double X,double Y,int direction){
		this.X=X;
		this.Y=Y;
		this.direction=direction;
	}
	/**
	 * @return x coordinate
	 */
	public double getX() {
		return X;
	}
	/**
	 * @param x - set x coordinate
	 */
	public void setX(double x) {
		X = x;
	}
	/**
	 * @return y coordinate
	 */
	public double getY() {
		return Y;
	}
	/**
	 * @param y get y coordinate
	 */
	public void setY(double y) {
		Y = y;
	}
	
	/**
	 * @return last direction traveled
	 */
	public int getDirection(){
		return this.direction;
	}

}
