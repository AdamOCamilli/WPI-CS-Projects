package simpler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

public class Customer implements Datatype {

	private HashMap<String,String> attributes = new HashMap<String,String>();
	
	public Customer() {
		attributes.put("ID", null);
		attributes.put("Name", null);
		attributes.put("Age", null);
		attributes.put("CountryCode", null);
		attributes.put("Salary", null);
	}
	
	public Customer(int ID, String name, int age, int countryCode, int salary) {
		attributes.put("ID", Integer.toString(ID));
		attributes.put("Name", name);
		attributes.put("Age", Integer.toString(age));
		attributes.put("CountryCode", Integer.toString(countryCode));
		attributes.put("Salary", Integer.toString(salary));
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

	
	/**
	 * 
	 * @param key
	 * @param value
	 * @return null if key not found
	 */
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
	
	/**
	 * Returns a possible random value for the given attribute
	 * @param key
	 * @return Random value within assignment description bounds
	 */
	public String getRandomValueFor(String key) {
		Random rand;
		Integer adjusted;
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
			adjusted = rand.nextInt(10000) + 100;
			return adjusted.toString();
		} else {
			return null;
		}
	}
	
}
