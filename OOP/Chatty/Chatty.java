import java.util.ArrayList;

import javax.swing.*;

class Client {
    private String username;
    private JFrame window;

    public Client(String username) {
        this.username = username;

        window = new JFrame("User: " + username);
        window.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        window.setSize(1000, 1000);
        window.setVisible(true);
    }

    public void Update(ArrayList<JLabel> chatlog) {
        window.removeAll();

        for (JLabel message : chatlog) {
            window.add(message);
        }
    }
}

public class Chatty {
    public static void main(String[] args) {
        ArrayList<JLabel> chatLog;

        Client[] clients = {
                new Client("User 1"),
                new Client("User 2"),
                new Client("User 3"),
                new Client("User 4"),
        };
    }
}