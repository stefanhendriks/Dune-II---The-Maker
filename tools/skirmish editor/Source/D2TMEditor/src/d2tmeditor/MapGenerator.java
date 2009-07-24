package d2tmeditor;

import java.util.Random;

/**
 *
 * @author Rene Zwanenburg
 */
public class MapGenerator 
{
    private long seed;
    private Random r;

    private Map map;

    public MapGenerator(){
        this((long)(Math.random()*Long.MAX_VALUE));
    }

    public MapGenerator(long seed){
        r = new Random(seed);

        map = new Map(63, 63);
    }

    public void setMapSize(int width, int height){
        if(map.getWidth() != width && map.getHeight() != height)
            map = new Map(width, height);
    }

    public Map getMap(){
        return map;
    }

    public void setSeed(long seed){
        this.seed = seed;
    }

    

    public Map generateMap(){
        r.setSeed(seed);

        



        return map;
    }
}
