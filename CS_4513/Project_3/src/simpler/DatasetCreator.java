package simpler;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Random;

/**
 * Simple program that creates two datasets and writes them into two .csv files.
 * @author Adam Camilli
 *
 */
public final class DatasetCreator {

	/**
	 * <code>Customer</code> primary keys that have not been used for a <code>Transaction</code>.
	 */
	ArrayList<String> prevCustPKs = new ArrayList<String>();

	public static void main(String[] args) {
		
		DatasetCreator dc = new DatasetCreator();
		
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
		
		File custcsv = new File("Customer" + "s" + ".txt");
		File transcsv = new File("Transaction" + "s" + ".txt");

		try {
			for (int i = 0; i < entries; i++) 
				dc.writeToCSV(custcsv, new Customer(), true);
			for (int i = 0; i < entries; i++) {
				Customer c = new Customer();
				String ID = dc.getPrevPK();
				c.setValue("ID", ID);
				dc.writeToCSV(transcsv, new Transaction(c), false);
			}
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.exit(1);
		}

	}
	
	public String getPrevPK() {
		Random rand = new Random();
		int idx = rand.nextInt(this.prevCustPKs.size());
		String PK = this.prevCustPKs.get(idx);
		this.prevCustPKs.remove(idx);
		return PK;
	}
	
	public void writeToCSV(File csv, Datatype entry, Boolean collectPKs) throws IOException {
		FileWriter fw = new FileWriter(csv.getName(), true); // Append to end
	    BufferedReader br = new BufferedReader(new FileReader(csv.getName()));
		String value;
		
		for (String attr : entry.getAttributes()) {
			// If this value must be unique, use BufferedReader to ensure file does not have it anywhere else
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
