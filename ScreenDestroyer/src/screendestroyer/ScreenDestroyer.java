package screendestroyer;

import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import java.util.ArrayList;
import javax.sound.sampled.*;
import javax.swing.*;

public final class ScreenDestroyer extends JFrame {

    private final Image memoryImage;
    private final Graphics memoryGraphics;
    private final int screenWidth, screenHeight;
    private final ArrayList<Weapon> weapons;
    //Weapons
    //  1. Punch
    //  2. Scratch
    //  3. Pistol
    //  4. Rifle
    //  5. Rocket Launcher
    //  6. Bow & Arrow
    //  7. clear
    private int currentWeapon = 1;
    Point startLocation, endLocation;
    private boolean isFadded;
    private Clip weaponSound;

    ScreenDestroyer() {
        setTitle("Screen Destroyer - Justin Stribling");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        screenWidth = (int) screenSize.getWidth();
        screenHeight = (int) screenSize.getHeight();
        setLocation(0, 0);
        setUndecorated(true);
        setSize(screenWidth, screenHeight);
        setResizable(false);
        setFocusable(true);
        addMouseListener(new MouseListener() {

            @Override
            public void mouseClicked(MouseEvent me) {
            }

            @Override
            public void mousePressed(MouseEvent me) {
                if (currentWeapon == 2) {
                    startLocation = new Point(me.getLocationOnScreen());
                } else {
                    addWeapon(currentWeapon, me.getLocationOnScreen().x, me.getLocationOnScreen().y, 0);
                }
            }

            @Override
            public void mouseReleased(MouseEvent me) {
                if (currentWeapon == 2) {
                    endLocation = new Point(me.getLocationOnScreen());
                    addWeapon(currentWeapon, (endLocation.x + startLocation.x) / 2, (endLocation.y + startLocation.y) / 2, 0);
                    //currentWeapon stays the same
                    //x = average x between start x and stop x
                    //y = average y between start y and stop y
                    //mod = andgle
                }
            }

            @Override
            public void mouseEntered(MouseEvent me) {
            }

            @Override
            public void mouseExited(MouseEvent me) {
            }
        });
        addKeyListener(new KeyListener() {

            @Override
            public void keyTyped(KeyEvent ke) {
            }

            @Override
            public void keyPressed(KeyEvent ke) {
                int key = Character.getNumericValue(ke.getKeyChar());
                if (ke.getKeyChar() == ' ') {
                    clearGame();
                } else if (key > 0 && key < 7) {
                    currentWeapon = key;
                }

            }

            @Override
            public void keyReleased(KeyEvent ke) {
            }

        });
        setVisible(true);
        memoryImage = createImage(screenWidth, screenHeight);
        memoryGraphics = memoryImage.getGraphics();
        weapons = new ArrayList();
        isFadded = false;
        // drawGame();
    }

    public static void main(String[] args) {
        ScreenDestroyer screenDestroyer = new ScreenDestroyer();
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
        for (Weapon weapon : weapons) {
            weapon.draw(memoryGraphics);
        }
        if (isFadded) {
            memoryGraphics.drawImage(new ImageIcon(getClass().getResource("/media/fadded.png")).getImage(), 0, 0, screenWidth, screenHeight, this);
        }
        getGraphics().drawImage(memoryImage, 0, 0, this);
    }

    public void pause() {
        isFadded = true;
    }

    public void unPasue() {
        isFadded = false;
    }

    public void addWeapon(int type, int x, int y, int mod) {
        playSound(type);
        try {
            Thread.sleep((type == 5) ? 1000 : 0);
        } catch (InterruptedException e) {
        }
        weapons.add(new Weapon(x, y, type));
        drawGame();
    }

    public void playSound(int type) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    weaponSound = (Clip) AudioSystem.getMixer(AudioSystem.getMixerInfo()[0]).getLine(new DataLine.Info(Clip.class, null));
                    weaponSound.open(AudioSystem.getAudioInputStream(getClass().getResource("/media/" + type + ".wav")));
                    weaponSound.start();

                    do {
                        Thread.sleep(50);
                    } while (weaponSound.isActive());
                } catch (LineUnavailableException | UnsupportedAudioFileException | IOException | InterruptedException e) {
                }
            }
        }).start();
    }

    public void clearGame() {
        playSound(7);
        weapons.clear();
        drawGame();
    }
}
