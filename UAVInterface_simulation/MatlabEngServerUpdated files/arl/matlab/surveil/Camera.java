package arl.matlab.surveil;

import java.util.Random;

import javax.swing.*;
import java.awt.event.*;
import java.awt.geom.Arc2D.Double;
import java.awt.*;


/**
 * @author andersonm
 *
 */
public class Camera{
    JPanel panel;
    boolean hold=false;
	TargetGUI gui;
	JLabel lname;
	JTextField angleField;
	private double angle;
	String name="Target";
   

	public Camera (TargetGUI gui,String name){
        lname = new JLabel(name);
        angleField = new JTextField();
        angleField.setColumns(10);
        angleField.setEditable(false);
		panel = new JPanel(new GridLayout(1,2));
		
		panel.add(lname);
		panel.add(this.angleField);
		gui.add(panel);	
	}

	public double getAngle() {
		return angle;
	}

	public void setAngle(double angle) {
		this.angleField.setText(""+angle);
		this.angle = angle;
	}

	public boolean isHold() {
		return hold;
	}

	public void setHold(boolean hold) {
		this.hold = hold;
	}
	
	


}
