package arl.matlab.surveil;


import java.util.*;
import javax.swing.*;

import java.awt.Color;
import java.awt.event.*;


/**
 * Target class - keeps position and alul value information;  also displays a panel for each target
 * 
 * @author Monica Anderson
 *
 */
public class Target implements ActionListener {
    /**
     * 
     */
    double currentX;
    /**
     * 
     */
    double currentY;
    /**
     * previous x position
     */
    double lastX=0;
    /**
     * previous y position
     */
    double lastY=0;
    /**
     * next x position; used as predicted x position
     */
    double predictedX;
    /**
     * next y position; used as predicted y position
     */
    double predictedY;
    /**
     * determines how often the targets change direction
     */
    double randomnessFactor=2;
    /**
     * 
     */
    Random rand;
    /**
     * tracked=true means a camera can see this target.  Determined externally
     */
    boolean tracked=false;
    /**
     * 
     */
    Terrain terrain;
    /**
     * default target threshold is 10 
     */
    int alulThreshold = 10;
    /**
     * panel to hold target components
     */
    JPanel panel;
    /**
     * button to modify target value; 
     */
    JButton chgALUL;
	/**
	 * alulfor target field
	 */
	JFormattedTextField alulValue;
	/**
	 * 
	 */
	TargetGUI gui;
	/**
	 * 
	 */
	JLabel lname;
	/**
	 * 
	 */
	String name="Target";
	/**
	 * if target is in a cell that is not visible by any camera=>false
	 */
	boolean visible=false;
	/**
	 * 
	 */
	int stepsSinceLastSeenTotal=0;
	/**
	 * 
	 */
	int stepsSinceLastSeenCount=0;
	/**
	 * 
	 */
	int steps=0;
	/**
	 * 
	 */
	int lastTracked=0;
	/**
	 * 
	 */
	boolean inROI=true;
	/**
	 * 
	 */
	int lastDirection=-1;
	/**
	 * if using predetermined target trajectory
	 */
	double cellX[];
	/**
	 * if using predetermined target trajectory
	 */
	double cellY[];
	/**
	 * last step added in the trajectory
	 */
	int cellIdx=0;
	/**
	 * current step of the predetermined trajectory
	 */
	int cIdx=0;
    /**
     * if random = true; we are not using a predetermined target trajectory
     */
    boolean random=true;

    /**
     * @return
     */
    public String getName() {
		return name;
	}

	/**
	 * @param name
	 */
	public void setName(String name) {
		this.name = name;
	}

	/**
	 * @return
	 */
	public boolean isTracked() {
		return tracked;
	}

    /**
     * @return  true if this target is visible, false otherwise
     */
    public boolean isVisible(){
        return visible;
    }

	/**
	 * @param tracked
	 */
	public void setTracked(boolean tracked) {
		if (visible == true && tracked==true) this.alulValue.setForeground(Color.GREEN);
		else if (visible == true) this.alulValue.setForeground(Color.RED);
		else this.alulValue.setForeground(Color.BLACK);
		this.tracked = tracked;
		if (this.tracked) {
			this.visible=true;
			this.stepsSinceLastSeenTotal=this.steps-this.lastTracked;
			this.lastTracked=this.steps;
			this.stepsSinceLastSeenCount++;
		}
				
	}
	
	/**
	 * 
	 */
	public void enterROI(){
//		this.stepsSinceLastSeenTotal=this.steps-this.lastTracked;
		this.lastTracked=this.steps;
//		this.stepsSinceLastSeenCount++;
		inROI=true;
	}
	/**
	 * 
	 */
	public void leaveROI(){
		this.stepsSinceLastSeenTotal=this.steps-this.lastTracked;
//		this.lastTracked=this.steps;
		this.stepsSinceLastSeenCount++;
		inROI=false;
	}
	
	/**
	 * @return
	 */
	public double getAverageUndetected(){
		if (this.stepsSinceLastSeenCount==0)
            return 0;
		return this.stepsSinceLastSeenTotal/this.stepsSinceLastSeenCount;
	}

	/**
	 * @param visible
	 */
	public void setVisible(boolean visible) {
		if (visible == true && tracked==true) this.alulValue.setForeground(Color.GREEN);
		else if (visible == true) this.alulValue.setForeground(Color.RED);
		else this.alulValue.setForeground(Color.BLACK);
		this.visible = visible;
	}

	/**
	 * @return
	 */
	public double getCurrentX() {
		return currentX;
	}

	/**
	 * @param currentX
	 */
	public void setCurrentX(double currentX) {
		this.currentX = currentX;
	}

	/**
	 * @return
	 */
	public double getCurrentY() {
		return currentY;
	}

	/**
	 * @param currentY
	 */
	public void setCurrentY(double currentY) {
		this.currentY = currentY;
	}

	/**
	 * @return predicted x position
	 */
	public double getPredictedX() {
		return predictedX;
	}

	/**
	 * @param predictedX
	 */
	public void setPredictedX(double predictedX) {
		this.predictedX = predictedX;
	}

	/**
	 * @return predicted y position
	 */
	public double getPredictedY() {
		return predictedY;
	}

	/**
	 * @param predictedY
	 */
	public void setPredictedY(double predictedY) {
		this.predictedY = predictedY;
	}
	
	/**
	 * @return
	 */
	public int getStepsTracked(){
		return this.steps-this.lastTracked;
	}

	/**
	 * Constructor for random trajectory
	 * 
	 * @param x
	 * @param y
	 * @param terrain
	 * @param gui
	 * @param name
	 */
	public Target (double x, double y,Terrain terrain, TargetGUI gui,String name){
        rand=new Random();
        this.currentX=x;
        this.currentY=y;
        this.terrain=terrain;
        chgALUL = new JButton("Change");
//		label = new JLabel();
        chgALUL.addActionListener(this);
        lname = new JLabel(name);
        alulValue = new JFormattedTextField();
		alulValue.setValue(this.alulThreshold);
		this.alulValue.setForeground(Color.BLACK);
		alulValue.setColumns(10);
		panel = new JPanel();
		panel.add(lname);
		panel.add(alulValue);
		panel.add(chgALUL);
        Choice[] choices = terrain.getChoices(currentX,currentY);
        int chooseOne = rand.nextInt(choices.length);
        predictedX=choices[chooseOne].X;
        predictedY=choices[chooseOne].Y;
		if (gui != null)
            gui.add(panel);	
	}

	/**
	 * Constructor for seeded random trajectory
	 * 
	 * @param	x		Initial X coord
	 * @param	y		Initial Y coord
	 * @param	terrain	Terrain for generating possible movements
	 * @param	gui		GUI for target information
	 * @param	name	Text for JLabel
	 * @param	seed	Seed for random number generator
	 */
	public Target (double x, double y,Terrain terrain, TargetGUI gui,String name, int seed){
        rand=new Random(seed);
        this.currentX=x;
        this.currentY=y;
        this.terrain=terrain;
        chgALUL = new JButton("Change");
//		label = new JLabel();
        chgALUL.addActionListener(this);
        lname = new JLabel(name);
        alulValue = new JFormattedTextField();
		alulValue.setValue(this.alulThreshold);
		this.alulValue.setForeground(Color.BLACK);
		alulValue.setColumns(10);
		panel = new JPanel();
		panel.add(lname);
		panel.add(alulValue);
		panel.add(chgALUL);
        Choice[] choices = terrain.getChoices(currentX,currentY);
        int chooseOne = rand.nextInt(choices.length);
        predictedX=choices[chooseOne].X;
        predictedY=choices[chooseOne].Y;
		if (gui != null)
            gui.add(panel);	
	}

	/**
	 * @param x
	 * @param y
	 * @param terrain
	 * @param gui
	 */
	public Target (double x, double y,Terrain terrain, TargetGUI gui){
		this(x,y,terrain,gui," ");
    }
	
	/**
	 * Constructor for precanned trajectory
	 * 
	 * @param x
	 * @param y
	 * @param gui
	 * @param name
	 * @param cellCount
	 */
	public Target(double x, double y, TargetGUI gui, String name,int cellCount){
		this.cellX=new double[cellCount];
		this.cellY=new double[cellCount];
		this.random=false;
        chgALUL = new JButton("Change");
        chgALUL.addActionListener(this);
        lname = new JLabel(name);
		alulValue = new JFormattedTextField();
		alulValue.setValue(this.alulThreshold);
		this.alulValue.setForeground(Color.BLACK);
		alulValue.setColumns(10);
		panel = new JPanel();
		panel.add(lname);
		panel.add(alulValue);
		panel.add(chgALUL);
		this.cellX[cellIdx]=x;
		this.cellY[cellIdx]=y;
		cellIdx++;
		if (gui != null)
            gui.add(panel);			
	}

	/**
	 * add position to predetermined trajectory
	 * @param x 
	 * @param y
	 */
	public void addStep(double x, double y){
		this.cellX[cellIdx]=x;
		this.cellY[cellIdx]=y;
		cellIdx++;		
	}
	
    /* (non-Javadoc)
     * @see java.lang.Object#toString()
     */
    public String toString(){
    	return currentX+":"+currentY+" "+predictedX+":"+predictedY+" "+tracked+" "+this.lastDirection;
    }

    /**
     * move the target forward one time step; moves the target randomly or via predetermined trajectory
     */
    public void step(){
        if (this.random==true) {
            this.steps++;
            lastX=currentX;
            lastY=currentY;
            Choice[] choices = terrain.getChoices(predictedX,predictedY);
            int chooseOne;
            if (rand.nextInt(4) >3)
                chooseOne = rand.nextInt(choices.length);
            else {
                chooseOne = getLastChoice(choices,lastDirection);
                if (chooseOne == -1 || lastDirection==Choice.STOPPED)
           	        chooseOne = rand.nextInt(choices.length);
            }
        		
            currentX=predictedX;
            currentY=predictedY;
            predictedX=choices[chooseOne].X;
            predictedY=choices[chooseOne].Y;
            lastDirection=choices[chooseOne].direction;
    	}
        else {
            currentX=predictedX;
            currentY=predictedY;
            predictedX=cellX[cIdx];
            predictedY=cellY[cIdx];
            cIdx++;
    	}
    }

    /**
     * Generate random cells for targets
     * 
     * @param	terrain	Terrain for target to travel over
     * @param	initX	Initial X coord for target
     * @param	initY	Initial Y coord for target
     * @param   seed    Seed for random number generator
     * @param   steps   Number of steps to simulate
     */
    public static double[][] createPath(Terrain terrain, double initX,
			double initY, int seed, int steps){

		double[][] ret = new double[steps][2];
		ret[0][0] = initX;
		ret[0][1] = initY;
		double currentX = initX;
		double currentY = initY;
		int stepIdx = 0;
        Random rand = new Random(seed);
		int lastDirection = -1;

		// Use current X and Y for step 1
		Choice[] choices = terrain.getChoices(currentX, currentY);
		int chooseOne;
		if(rand.nextInt(4) > 3){
			chooseOne = rand.nextInt(choices.length);
		}
		else{
			chooseOne = getLastChoice(choices,lastDirection);
                if (chooseOne == -1 || lastDirection==Choice.STOPPED)
                    chooseOne = rand.nextInt(choices.length);
        }

        double predictedX=choices[chooseOne].X;
        double predictedY=choices[chooseOne].Y;
        lastDirection=choices[chooseOne].direction;

		ret[stepIdx][0] = currentX;
		ret[stepIdx][1] = currentY;
		++stepIdx;

		// Now use predicted X and Y
        for(int x = 1; x < steps; ++x){
            double lastX=currentX;
            double lastY=currentY;
            choices = terrain.getChoices(predictedX,predictedY);
            if (rand.nextInt(4) >3){
                chooseOne = rand.nextInt(choices.length);
            }
            else {
                chooseOne = getLastChoice(choices,lastDirection);
                if (chooseOne == -1 || lastDirection==Choice.STOPPED)
                    chooseOne = rand.nextInt(choices.length);
            }

            currentX=predictedX;
            currentY=predictedY;
            predictedX=choices[chooseOne].X;
            predictedY=choices[chooseOne].Y;
            lastDirection=choices[chooseOne].direction;

			ret[stepIdx][0] = currentX;
			ret[stepIdx][1] = currentY;
			++stepIdx;
        }

		return ret;
    }

	/* (non-Javadoc)
	 * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent arg0) {
		// assumed to be the buttonpush
		alulThreshold=((Number)alulValue.getValue()).intValue();
		System.out.println("new value "+alulThreshold);
	}
	
	/**
	 * @param choices
	 * @param lastDirection
	 * @return
	 */
	public static int getLastChoice(Choice[] choices, int lastDirection){
		for (int i=0;i<choices.length;i++)
            if (choices[i].getDirection()==lastDirection)
                return i;
        return -1;
	}

	/**
	 * @return
	 */
	public int getAlulThreshold() {
        return alulThreshold;
	}

	/**
	 * @param alulThreshold
	 */
	public void setAlulThreshold(int alulThreshold) {
		this.alulThreshold = alulThreshold;
		this.alulValue.setText(Integer.toString(alulThreshold));
	}

	/**
	 * @return
	 */
	public boolean isInROI() {
		return inROI;
	}

	/**
	 * @param inROI
	 */
	public void setInROI(boolean inROI) {
		this.inROI = inROI;
	}
	
	/**
	 * @param r
	 * @param g
	 * @param b
	 */
	public void setLabelColor(int r, int g, int b){
		this.lname.setForeground(new Color(r,g,b));
	}
	
    public static void main(String[] args){
    	TargetGUI gui=new TargetGUI();
		Terrain terrain = new Terrain("heights", 73,109,5,329723,4321586.5);
    	Target t = new Target(329778.1,4321589,terrain,gui,"target 1");
    	Target t2 = new Target(329778.1,4321589,gui,"target 2",100);
    	Target t3 = new Target(329778.1,4321589,terrain,gui,"target 3");
    	Target t4 = new Target(329778.1,4321589,terrain,gui,"target 4");
    	Camera cam1 = new Camera(gui,"Cam 1");
    	Camera cam2 = new Camera(gui,"Cam 2");
    	Camera cam3 = new Camera(gui,"Cam 3");
    	Camera cam4 = new Camera(gui,"Cam 4");
    	ALUL alul = new ALUL(gui);
    	cam1.setAngle(10);
    	cam2.setAngle(20);
    	gui.setVisible(true);
    	System.out.println(t.toString());
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t.step();
    	System.out.println(t);
    	t2.addStep(1,1);
    	t2.addStep(1,21);
    	t2.addStep(11,11);
    	t2.addStep(6,7);
    	t2.addStep(14,1);
    	t2.addStep(1,21);
    	t2.addStep(13,13);
    	t2.addStep(1,10);
    	t2.step();
    	System.out.println(t2);
    	t2.step();
    	System.out.println(t2);
    	t2.step();
    	System.out.println(t2);
    	t2.step();
    	System.out.println(t2);
    	t2.step();
    	System.out.println(t2);
    	t2.step();
    	System.out.println(t2);
    }
}
