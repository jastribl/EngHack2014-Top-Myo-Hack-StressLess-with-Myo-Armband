package myoapp;

import java.awt.*;
import javax.swing.ImageIcon;

public class Weapon {

    private int x, y;
    private final int name;
    private final Image image;

    public Weapon(int xx, int yy, int nam) {
        x = xx;
        y = yy;
        name = nam;
        image = new ImageIcon(getClass().getResource("/media/" + nam + ".png")).getImage();
    }

    public void adjustLocation() {
        x -= (image.getWidth(null) / 2);
        y -= (image.getHeight(null) / 2);
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public int getName() {
        return name;
    }

    public Image getImage() {
        return image;
    }

    public void draw(Graphics g) {
        g.drawImage(image, getX(), getY(), null);
    }
}
