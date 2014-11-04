package myoapp;

import java.awt.*;
import java.util.ArrayList;
import javax.swing.*;

public final class ScreenGame extends JFrame {

    private final JPanel panel;
    private final Image memoryImage;
    private final Graphics memoryGraphics;
    private final int screenWidth, screenHeight;
    public ArrayList<Weapon> weapons;
    private boolean isFadded;

    ScreenGame() {
        setTitle("EngHack - Group16 - ScreenDestoyer");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        screenWidth = (int) screenSize.getWidth();
        screenHeight = (int) screenSize.getHeight();
        setLocation(0, 0);
        setUndecorated(true);
        setSize((int) screenWidth, (int) screenHeight);
        setResizable(false);
        panel = new JPanel();
        panel.setFocusable(true);
        add(panel);
        setVisible(true);
        memoryImage = createImage(getWidth(), getHeight());
        memoryGraphics = memoryImage.getGraphics();
        weapons = new ArrayList();
        isFadded = false;
        drawGame();
    }

    public void quit() {
        System.exit(0);
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);
        drawGame();
    }

    public void drawGame() {
        memoryGraphics.drawImage(new ImageIcon(getClass().getResource("/media/background.jpg")).getImage(), 0, 0, screenWidth, screenHeight, this);
        try {
            for (Weapon weapon : weapons) {
                weapon.draw(memoryGraphics);
            }
        } catch (java.util.ConcurrentModificationException e) {
            System.out.println("error");
        }
        if (isFadded) {
            memoryGraphics.drawImage(new ImageIcon(getClass().getResource("/media/fadded.png")).getImage(), 0, 0, screenWidth, screenHeight, this);
        }
        panel.getGraphics().drawImage(memoryImage, 0, 0, panel);
    }

    public void pause() {
        isFadded = true;
    }

    public void unPasue() {
        isFadded = false;
    }

    public void addWeapon(int type) {
        weapons.add(new Weapon((int) (Math.random() * screenWidth), (int) (Math.random() * screenHeight), type));
        weapons.get(weapons.size() - 1).adjustLocation();
    }

    public void clearAll() {
        weapons.clear();
    }
}
