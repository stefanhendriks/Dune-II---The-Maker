package logviewer;


import java.util.ArrayList;

/**
 *
 * @author Rene Zwanenburg
 */
public class Unit 
{
    private int id;

    private ArrayList<Message> messages = new ArrayList<Message>();
    private String desc;
    private int house = -1;

    public Unit(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public void addMessage(String m){
        Message message = new Message(m);
        messages.add(message);

        if(desc == null)
            desc = message.getUnitDescription();
        if(house == -1)
            house = message.getHouse();
    }

    @Override
    public String toString(){
        return desc + ", House = " + house + ", ID = " + id;
    }

    public ArrayList<Message> getMessages(){
        return messages;
    }
}
