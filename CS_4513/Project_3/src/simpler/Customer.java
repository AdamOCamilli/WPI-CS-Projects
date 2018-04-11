package simpler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

/**
 * A <code>Datatype</code> with the following attributes: <p>
 * 	ID:[1,50000]:Int -------------->Primary Key<br>
 * 	Name: [10,20]:String <br>
 * 	Age: [10,70]:Int <br>
 *  CountryCode: [1,10]:Int <br>
 *  Salary: [100,10000]:Float <br>
 * @author Adam Camilli (aocamilli@wpi.edu)
 *
 */
public class Customer implements Datatype {

	private HashMap<String,String> attributes = new HashMap<String,String>();
	
	public Customer() {
		attributes.put("ID", null);
		attributes.put("Name", null);
		attributes.put("Age", null);
		attributes.put("CountryCode", null);
		attributes.put("Salary", null);
	}
	
	public Customer(int ID, String name, int age, int countryCode, double salary) {
		attributes.put("ID", Integer.toString(ID));
		attributes.put("Name", name);
		attributes.put("Age", Integer.toString(age));
		attributes.put("CountryCode", Integer.toString(countryCode));
		attributes.put("Salary", Double.toString(salary));
	}
	
	public String getPrimaryKey() {
		return "ID";
	}
	
	public ArrayList<String> getAttributes() {
		ArrayList<String> attrNames = new ArrayList<String>();
		attrNames.add("ID");
		attrNames.add("Name");
		attrNames.add("Age");
		attrNames.add("CountryCode");
		attrNames.add("Salary");
		return attrNames;
	}

	public String getValue(String key) {
		if (this.attributes.containsKey(key)) {
			return this.attributes.get(key);
		} return null;
	}
	
	public void setValue(String key, String value) {
		if (this.attributes.containsKey(key)) {
			this.attributes.put(key, value);
		}
	}
	
	protected String randomAlphabetStr(int min, int max) {
		String alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		Random rand = new Random();
		int count = rand.nextInt(max) + min;
		
		StringBuffer randStr = new StringBuffer();
		int index;
		rand = new Random();
		for (; count >= 0; count--) {
			index = (int) (rand.nextFloat() * alphabet.length());
            randStr.append(alphabet.charAt(index));
		}
		
		return randStr.toString();
	}
	
	public String getRandomValueFor(String key) {
		Random rand;
		Integer adjusted;
		Double adjusted_double;
		if (key.equals("ID")) {
			rand = new Random();
			adjusted = rand.nextInt(50000) + 1;
			return adjusted.toString();
		} else if (key.equals("Name")) {
			return randomAlphabetStr(10,20);
		} else if (key.equals("Age")) {
			rand = new Random();
			adjusted = rand.nextInt(70) + 10;
			return adjusted.toString();
		} else if (key.equals("CountryCode")) {
			rand = new Random();
			adjusted = rand.nextInt(10) + 1;
			return adjusted.toString();
		} else if (key.equals("Salary")) {
			rand = new Random();
			adjusted_double = (10000.0 - 100.0)*rand.nextDouble() + 100.0;
			return String.valueOf(Math.round(adjusted_double * 100.0) / 100.0); // Round to hundredths place (cents)
		} else {
			return null;
		}
	}
	
}
