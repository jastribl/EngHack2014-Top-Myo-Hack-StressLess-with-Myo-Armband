package myoapp;

import javax.swing.JOptionPane;
import redis.clients.jedis.*;

public class MyoApp {

    public static final String CHANNEL_NAME = "engHack16";

    public static void main(String[] args) throws Exception {
        final JedisPoolConfig poolConfig = new JedisPoolConfig();
        String ip = JOptionPane.showInputDialog("Enter IP Address:", "192.168.173.191");
        final JedisPool jedisPool = new JedisPool(poolConfig, ip, 6379, 0);
        final Jedis subscriberJedis = jedisPool.getResource();
        final Subscriber subscriber = new Subscriber();
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    subscriberJedis.subscribe(subscriber, CHANNEL_NAME);
                } catch (Exception e) {
                    System.out.println("Subscribing failed. " + e);
                }
            }
        }).start();
        final Jedis publisherJedis = jedisPool.getResource();
        new Publisher(publisherJedis, CHANNEL_NAME).start();
        subscriber.unsubscribe();
        jedisPool.returnResource(subscriberJedis);
        jedisPool.returnResource(publisherJedis);
    }
}
