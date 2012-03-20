package arl.matlab.surveil;

import javax.swing.*;
import java.awt.*;

/** 
 * GUI container that holds the simulator components
 * 
 * @author Monica Anderson
 *
 */
public class TargetGUI extends JFrame {
	
	/**
	 * 
	 * - used Gridlayout(0,1) to make sure that components stack under the last one
	 */
	public TargetGUI(){
		setLayout(new GridLayout(0,1));
		setBounds(100,100,400,700);
	}

}
