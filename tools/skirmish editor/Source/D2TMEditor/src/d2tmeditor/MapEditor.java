package d2tmeditor;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import javax.swing.JPanel;

/**
 *
 * @author Rene Zwanenburg
 */
public class MapEditor extends JPanel
{
    private Map map;

    private Font font = new Font(Font.MONOSPACED, Font.PLAIN, 10);
    
    private int brushSize = 1;

    public MapEditor(){
        addMouseListener(new MouseAdapter(){
            @Override
            public void mousePressed(MouseEvent e){
                if(map == null)
                    return;

                if(map.isPlacePlayer())
                    map.setAt(e.getX()/8, e.getY()/8);
                else
                    for(int x = 0; x < brushSize; x++)
                        for(int y = 0; y < brushSize; y++)
                            map.setAt(e.getX()/8 + x, e.getY()/8 + y);

                repaint();
            }
        });
        addMouseMotionListener(new MouseMotionAdapter(){
            @Override
            public void mouseDragged(MouseEvent e){
                if(map == null)
                    return;
                
                if(map.isPlacePlayer())
                    map.setAt(e.getX()/8, e.getY()/8);
                else
                    for(int x = 0; x < brushSize; x++)
                        for(int y = 0; y < brushSize; y++)
                            map.setAt(e.getX()/8 + x, e.getY()/8 + y);
                
                repaint();
            }
        });
    }

    @Override
    protected void paintComponent(Graphics g){
        if(map == null){
            super.paintComponent(g);
            return;
        }

        g.setColor(Color.BLACK);
        g.fillRect(0, 0, getWidth(), getHeight());

        for(int y = 0; y < map.getHeight(); y++)
            for(int x = 0; x < map.getWidth(); x++){
                g.setColor(Map.mapColors[map.getAt(x, y)]);
                g.fillRect(x*8, y*8, 7, 7);
            }

        for(int i = 0; i < map.getPlayerStart().length; i++)
            if(map.getPlayerEnabled()[i]){
                g.setColor(Color.WHITE);
                g.fillRect(
                        map.getPlayerStart()[i].x*8,
                        map.getPlayerStart()[i].y*8,
                        7, 7);

                g.setColor(Color.BLACK);
                g.setFont(font);
                g.drawString((i+1) + "",
                        map.getPlayerStart()[i].x*8,
                        map.getPlayerStart()[i].y*8 + 7);
            }
    }

    public Map getMap() {
        return map;
    }

    public void setMap(Map map) {
        this.map = map;
    }

    public int getBrushSize() {
        return brushSize;
    }

    public void setBrushSize(int brushSize) {
        this.brushSize = brushSize;
    }
}
