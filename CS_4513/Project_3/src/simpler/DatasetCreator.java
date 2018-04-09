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

	ArrayList<String> prevCustPKs = new ArrayList<String>();

	public static void main(String[] args) {
		
		DatasetCreator dc = new DatasetCreator();
		
		int entries = 20000;
		File custcsv = new File("Customer" + "s" + ".csv");
		File transcsv = new File("Transaction" + "s" + ".csv");

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
