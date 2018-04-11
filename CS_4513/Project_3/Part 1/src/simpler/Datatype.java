package simpler;

import java.util.ArrayList;

/**
 * Interface for data types used with <code>DatasetCreator</code>.
 * @author Adam Camilli (aocamilli@wpi.edu)
 *
 */
public interface Datatype {
	/**
	 * Returns the NAME of the primary key attribute, NOT the actual value.
	 * @return Name of primary key attribute
	 */
	public String getPrimaryKey();
	/**
	 * Returns the NAMES of attributes, NOT the actual values.
	 * @return Names of all attributes
	 */
	public ArrayList<String> getAttributes();
	public String getValue(String key);
	public void setValue(String key, String value);
	/**
	 * Gets an appropriate randomized value for a particular attribute of a <code>Datatype</code>. This value
	 * is not guaranteed to be unique!
	 * @param key Attribute
	 * @return Appropriate random value.
	 */
	public String getRandomValueFor(String key);
}
