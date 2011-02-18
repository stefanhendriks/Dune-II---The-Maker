package logviewer;


import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;

/**
 *
 * @author Rene Zwanenburg
 */
public final class LogReader
{
    private LogReader(){}

    public static ArrayList<Unit> readFile(File f) {
        ArrayList<Unit> l = new ArrayList<Unit>(); // Temporary unordered unit storage
        ArrayList<String> lines = new ArrayList<String>(); // Stores unit info lines

        try{
            BufferedReader in = new BufferedReader(new FileReader(f));
            String line = null;

            while((line = in.readLine()) != null){ // while more lines remain...
                line = line.trim(); // Remove all leading and trailing spaces.
                if(line.length() == 0 || !line.startsWith("0| [UNIT")) // If line is empty or isn't about units...
                    continue; // Go to next line.

                lines.add(line);
            }
        } catch(Exception e){
            System.err.println("Error Reading File:");
            e.printStackTrace();
            return null;
        }

        for(int i = 0; i < lines.size(); i++) // cut off unused first characters
            lines.set(i, lines.get(i).substring(9));

        for(String s : lines){ // adds units to unordered list
            if(getUnit(getUnitID(s), l) == null)
                l.add(new Unit(getUnitID(s)));
        }

        ArrayList<Unit> units = new ArrayList<Unit>(); // ordered unit storage
        int maxID = l.size(); // Lowest possible amount of id's is unordered storage size
        for(int i = 0; i < maxID; i++){
            if(getUnit(i, l) == null){ // If id 'i' is not in the list...
                maxID++; // The highest possible ID increases by one.
                continue;
            }

            units.add(getUnit(i, l));
        }

        for(String s : lines)
            getUnit(getUnitID(s), units).addMessage(s); // Parsing the line is up to the unit
        
        return units;
    }

    private static int getUnitID(String line){
        return Integer.parseInt(line.split("]")[0]);
    }

    private static Unit getUnit(int id, ArrayList<Unit> list){
        for(Unit u : list)
            if(u.getId() == id)
                return u;

        return null;
    }
}
