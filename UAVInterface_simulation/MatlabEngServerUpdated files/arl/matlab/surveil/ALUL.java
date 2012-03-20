package arl.matlab.surveil;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.text.*;

/**
 * ALUL field - holds the current user-settable maximum alul value;
 * also display the current calculated system alul value
 * 
 * @author Monica Anderson
 *
 */
public class ALUL implements ActionListener{
	
	/**
	 * Button to change the system alul value
	 */
	JButton update;
	/**
	 * panel that holds the alul components
	 */
	JPanel panel;
	/**
	 * label that holds the current alul system value
	 */
	JLabel label;
	/**
	 * field to hold alul maximum
	 */
	JFormattedTextField alulValue;
	/**
	 * alul system maximum value
	 */
	int alul=10;
	/**
	 * format number to 2 decimal places
	 */
	NumberFormat format;
	
	/**
	 * @param component to hold this panel
	 */
	public ALUL(TargetGUI gui){
		update = new JButton("Change");
		update.addActionListener(this);
		alulValue = new JFormattedTextField();
		alulValue.setValue(this.alul);
		alulValue.setColumns(10);
		
		format = NumberFormat.getNumberInstance();
		format.setMaximumFractionDigits(2);
		panel = new JPanel();
		panel.setLayout(new FlowLayout());
		label=new JLabel("ALUL Value ()");
		panel.add(label);
		panel.add(alulValue);
		panel.add(update);
		gui.add(panel);
	}
	
	/**
	 * @return alul value
	 */
	public int getALUL() {
		return this.alul;
	}
	
	/* (non-Javadoc)
	 * @see java.awt.event.ActionListener#actionPerformed(java.awt.event.ActionEvent)
	 */
	public void actionPerformed(ActionEvent arg0) {
		// assumed to be the buttonpush
		alul=((Number)alulValue.getValue()).intValue();
		System.out.println("new value "+alul);		
	}

	/**
	 * @param val - new alul vale
	 */
	public void setCurrentALUL(double val){
		label.setText("ALUL Value ("+format.format(val)+")");
	}

	/**
	 * @return label
	 */
	public JLabel getLabel() {
		return label;
	}

	/**
	 * @param label
	 */
	public void setLabel(JLabel label) {
		this.label = label;
	}

	/**
	 * @param alul
	 */
	public void setAlul(int alul) {
		this.alul = alul;
		alulValue = new JFormattedTextField();
		alulValue.setText(format.format(alul));
	}

}
