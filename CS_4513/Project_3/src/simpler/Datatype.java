package simpler;

import java.util.ArrayList;

public interface Datatype {
	public String getPrimaryKey();
	public ArrayList<String> getAttributes();
	public String getValue(String key);
	public void setValue(String key, String value);
	public String getRandomValueFor(String key);
}
