package simpler;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

/**
 * Simple program that creates two datasets and lists them in two .txt files in CSV (Comma Separated Value) format. <p>
 * Can be used from command line: <p>
 * {@code $ javac src/simpler/*.java} <br>
 * {@code $ java -cp . simpler.DatasetCreator <number of entries> } <p>
 * If previous files were created, newly created files will be titled {@code Datatypes1.txt, Datatypes2.txt} and so on.
 * @author Adam Camilli
 *
 */
public final class DatasetCreator {

	/**
	 * <code>Customer</code> primary keys that have not been used as foreign keys for a <code>Transaction</code>.
	 */
	ArrayList<String> prevCustPKs = new ArrayList<String>();

	public static void main(String[] args) {
		
		DatasetCreator dc = new DatasetCreator();
		
		if (args.length > 1 || args.length <= 0) {
			System.out.println("Usage: java -cp . simpler.DatasetCreator <number of entries>");
			return;
		}
		
		int entries = 0;
		try {
			entries = Integer.parseInt(args[0]);
		} catch (NumberFormatException nfe) {
			System.out.println(args[0] + " is not valid number of entries.");
			nfe.printStackTrace();
			return;
		}
		
		if (entries > 50000) {
			System.out.println("Cannot create more than 50000 unique entries");
			return;
		} else if (entries <= 0) {
			System.out.println(args[0] + " is not a valid number of entries.");
			return;
		}
		
		File custcsv = new File("Customers" + ".txt");
		File transcsv = new File("Transactions" + ".txt");
		
		// If previous file exist, append 1,2,3... as appropriate
		for (int i = 1; custcsv.exists(); i++) {
			custcsv = new File("Customers" + i + ".txt");
		} for (int i = 1; transcsv.exists(); i++) {
			transcsv = new File("Transactions" + i + ".txt");
		}

		try {
			for (int i = 0; i < entries; i++) 
				dc.writeToCSV(custcsv, new Customer(), true);
			for (int i = 0; i < entries; i++) {
				Customer c = new Customer();
				String ID = dc.getPrevPK(dc.prevCustPKs);
				c.setValue("ID", ID);
				dc.writeToCSV(transcsv, new Transaction(c), false);
			}
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}

	}
	
	/**
	 * Returns unused primary key and then removes it from the list of unused primary keys.
	 * @return Unused <code>Customer</code> primary key.
	 */
	public String getPrevPK(ArrayList<String> prevPKList) {
		Random rand = new Random();
		int idx = rand.nextInt(prevPKList.size());
		String PK = prevPKList.get(idx);
		prevPKList.remove(idx);
		return PK;
	}
	
	public void writeToCSV(File csv, Datatype entry, Boolean collectPKs) throws IOException {
		FileWriter fw = new FileWriter(csv.getName(), true); // Append to end, don't overwrite
	    BufferedReader br = new BufferedReader(new FileReader(csv.getName()));
		String value;
		
		for (String attr : entry.getAttributes()) {
			// This value must be unique, use BufferedReader to ensure other entries do not have it
			if (entry.getPrimaryKey().equals(attr)) { 	// Ensure primary key is unique within file
				value = entry.getRandomValueFor(attr);
				while (this.prevCustPKs.contains(value)) 	// Shuffle for new unique primary key
					value = entry.getRandomValueFor(attr);
				if (collectPKs)
					this.prevCustPKs.add(value);
				fw.write(value);
			} else {
				value = entry.getRandomValueFor(attr);
				if (value != null)
					fw.write("," + value); // Ensure final value doesn't have comma after it
			}
		}
		
		// Add newline for next entry
		fw.write("\n");
		
		br.close();
		fw.close();
	}
	
	

	

}
