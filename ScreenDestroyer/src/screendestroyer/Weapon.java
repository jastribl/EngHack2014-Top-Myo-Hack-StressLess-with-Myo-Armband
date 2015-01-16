package screendestroyer;

import java.awt.*;
import javax.swing.ImageIcon;

public final class Weapon {

    private int x, y;
    private final Image image;

    public Weapon(int xx, int yy, int type) {
        x = xx;
        y = yy;
        image = new ImageIcon(getClass().getResource("/media/" + type + ".png")).getImage();
        adjustLocation(type);
    }

    private void adjustLocation(int type) {
        if (type == 6) {
            x -= image.getWidth(null);
        } else {
            x -= (image.getWidth(null) / 2);
            y -= (image.getHeight(null) / 2);
        }
    }

    public void draw(Graphics g) {
        g.drawImage(image, x, y, null);
    }
}
