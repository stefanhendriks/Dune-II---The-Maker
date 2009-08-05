package logviewer;

/**
 *
 * @author Rene Zwanenburg
 */
public class Message 
{
    private int house;
    private int cell;
    private int target;

    private String unitDesc;
    private String message;

    public Message(String m){
        String temp = m.substring(m.indexOf(":")+2, m.indexOf(" - "));
        unitDesc = temp;

        m = m.substring(m.indexOf(" - ")+9);
        house = Integer.parseInt(m.split(",")[0]);

        m = m.substring(m.indexOf(" = ") + 3);
        cell = Integer.parseInt(m.split(",")[0]);

        m = m.substring(m.indexOf(" = ") + 3);
        target = Integer.parseInt(m.split(" ] ")[0]);

        message = m.substring(m.indexOf("'")+1, m.lastIndexOf("'"));
    }

    public String getUnitDescription(){
        return unitDesc;
    }

    public int getHouse(){
        return house;
    }

    public String getMessage(){
        return message;
    }

    public int getCell(){
        return cell;
    }

    public int getTarget(){
        return target;
    }

    @Override
    public String toString(){
        return message + ", Cell = " + cell + ", Target = " + target;
    }
}
