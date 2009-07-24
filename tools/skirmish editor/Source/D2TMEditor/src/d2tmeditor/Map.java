package d2tmeditor;

import java.awt.Color;
import java.awt.Point;

/**
 *
 * @author Rene Zwanenburg
 */
public class Map 
{
    public static final int
            TYPE_SAND = 0,
            TYPE_ROCK = 1,
            TYPE_SPICE = 2,
            TYPE_MOUNTAIN = 3,
            TYPE_DUNE = 4,
            TYPE_SPICE_DUNE = 5,

            PLAYER_1 = 0,
            PLAYER_2 = 1,
            PLAYER_3 = 2,
            PLAYER_4 = 3;

    public static final Color[] mapColors = new Color[]{
        new Color(198, 125, 57),
        new Color(82, 81, 57),
        new Color(189, 93, 33),
        new Color(49, 48, 33),
        new Color(189, 113, 49),
        new Color(155, 80, 27),
    };

    private int[][] mapData;
    private int
            width,
            height;

    //TODO: Title field is not used at the moment, use it or remove it.
    private String title;

    private Point[] playerStart;
    private boolean[] playerEnabled;

    private int currentSelection;
    private int currentPlayerSelection;
    private boolean placePlayer;

    public Map(int width, int height){
        this.width = width;
        this.height = height;

        mapData = new int[width][height];

        playerStart = new Point[4];
        for(int i = 0; i < playerStart.length; i++)
            playerStart[i] = new Point();
        
        playerEnabled = new boolean[4];
    }

    public void setAt(int x, int y){
        if(x >= 0 && x < width && y >= 0 && y < height){
            if(placePlayer)
                playerStart[currentPlayerSelection].setLocation(x, y);
            else{
                //TODO: The 'place spice dunes instead of spice' feature should be the
                //responsibility of the MapEditor class, not the Map class. Change this.
                if(currentSelection == TYPE_SPICE && (mapData[x][y] == TYPE_DUNE ||
                        mapData[x][y] == TYPE_SPICE_DUNE))
                    mapData[x][y] = TYPE_SPICE_DUNE;
                else
                    mapData[x][y] = currentSelection;
            }
        }
    }

    public int getAt(int x, int y){
        return mapData[x][y];
    }

    public int getCurrentPlayerSelection() {
        return currentPlayerSelection;
    }

    public void setCurrentPlayerSelection(int currentPlayerSelection) {
        this.currentPlayerSelection = currentPlayerSelection;
    }

    public int getCurrentSelection() {
        return currentSelection;
    }

    public void setCurrentSelection(int currentSelection) {
        this.currentSelection = currentSelection;
    }

    public int[][] getMapData() {
        return mapData;
    }

    public boolean isPlacePlayer() {
        return placePlayer;
    }

    public void setPlacePlayer(boolean placePlayer) {
        this.placePlayer = placePlayer;
    }

    public boolean[] getPlayerEnabled() {
        return playerEnabled;
    }

    public void setPlayerEnabled(boolean[] playerEnabled) {
        this.playerEnabled = playerEnabled;
    }

    public Point[] getPlayerStart() {
        return playerStart;
    }

    public void setPlayerStart(Point[] playerStart) {
        this.playerStart = playerStart;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public int getHeight() {
        return height;
    }

    public int getWidth() {
        return width;
    }
}
