package myoapp;

import java.io.IOException;
import javax.sound.sampled.*;
import javax.swing.JOptionPane;
import org.json.simple.*;
import redis.clients.jedis.JedisPubSub;

public class Subscriber extends JedisPubSub {

    private ScreenGame screenGame;
    private int computerNumber;
    public static Clip clip;

    public Subscriber() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    screenGame = new ScreenGame();
                    String name = JOptionPane.showInputDialog("Enter Computer Number:");
                    if (name == null || name.matches("")) {
                        computerNumber = 0;
                        screenGame.quit();
                    } else {
                        computerNumber = Integer.valueOf(name);
                    }
                } catch (Exception e) {
                    System.out.println("Subscribing failed. " + e);
                }
            }
        }).start();
    }

    @Override
    public void onMessage(String channel, String message) {
        JSONArray array = (JSONArray) JSONValue.parse(message);
        if (Integer.valueOf(array.get(0).toString()) == computerNumber) {
            screenGame.unPasue();
            int type = Integer.valueOf(array.get(1).toString());
            if (type > 0 && type < 9) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            try {
                                clip = (Clip) AudioSystem.getMixer(AudioSystem.getMixerInfo()[0]).getLine(new DataLine.Info(Clip.class, null));
                            } catch (LineUnavailableException lue) {
                                lue.printStackTrace();
                            }
                            try {
                                clip.open(AudioSystem.getAudioInputStream(getClass().getResource("/media/" + type + ".wav")));
                            } catch (IOException | UnsupportedAudioFileException | LineUnavailableException ex) {
                            }
                            clip.start();
                            do {
                                try {
                                    Thread.sleep(50);
                                } catch (InterruptedException ie) {
                                }
                            } while (clip.isActive());
                        } catch (Exception e) {
                            System.out.println("Subscribing failed. " + e);
                        }
                    }
                }).start();
            }
            if (type > 0 && type < 8) {
                if (type == 5) {
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ex) {
                    }

                }
                screenGame.addWeapon(type);
            } else if (type == 8) {
                screenGame.clearAll();
            }
        } else {
            screenGame.pause();
        }
        screenGame.drawGame();
    }

    @Override
    public void onPMessage(String pattern, String channel, String message) {
    }

    @Override
    public void onSubscribe(String channel, int subscribedChannels) {
    }

    @Override
    public void onUnsubscribe(String channel, int subscribedChannels) {
    }

    @Override
    public void onPUnsubscribe(String pattern, int subscribedChannels) {
    }

    @Override
    public void onPSubscribe(String pattern, int subscribedChannels) {
    }
}
