package simpler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

public class Transaction implements Datatype {
	
	private HashMap<String,String> attributes = new HashMap<String,String>();
	
	private Transaction() {}
	
	public Transaction(Customer custID) {
		attributes.put("TransID", null);
		attributes.put("CustID", custID.getValue(custID.getPrimaryKey()));
		attributes.put("TransValue", null);
		attributes.put("TransNumItems", null);
		attributes.put("TransDesc", null);
	}
	
	public Transaction(int transID, Customer custID, int transValue, int transItems, String transDesc) {
		attributes.put("TransID", Integer.toString(transID));
		attributes.put("CustID", custID.getValue(custID.getPrimaryKey()));
		attributes.put("TransValue", Integer.toString(transValue));
		attributes.put("TransNumItems", Integer.toString(transItems));
		attributes.put("TransDesc", transDesc);
	}
	
	public String getPrimaryKey() {
		return "TransID";
	}
	
	public ArrayList<String> getAttributes() {
		ArrayList<String> attrNames = new ArrayList<String>();
		attrNames.add("TransID");
		attrNames.add("CustID");
		attrNames.add("TransValue");
		attrNames.add("TransNumItems");
		attrNames.add("TransDesc");
		return attrNames;
	}

	@Override
	public String getValue(String key) {
		if (this.attributes.containsKey(key)) {
			return this.attributes.get(key);
		} return null;
	}
	
	@Override
	public void setValue(String key, String value) {
		if (this.attributes.containsKey(key)) {
			this.attributes.put(key, value);
		}
	}
	
	protected String randomAlphabetStr(int min, int max) {
		String alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz -";
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
		if (key.equals("TransID")) {
			rand = new Random();
			adjusted = rand.nextInt(5000000) + 1;
			return adjusted.toString();
		} else if (key.equals("CustID")) {		// Don't change value for foreign key
			return this.attributes.get(key);
		} else if (key.equals("TransValue")) { 
			rand = new Random();
			adjusted = rand.nextInt(10000) + 10;
			return adjusted.toString();
		} else if (key.equals("TransNumItems")) {
			rand = new Random();
			adjusted = rand.nextInt(10) + 1;
			return adjusted.toString();
		} else if (key.equals("TransDesc")) {
			return randomAlphabetStr(20,50);
		} else {
			return null;
		}
	}

}
