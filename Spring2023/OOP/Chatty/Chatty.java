import java.awt.GridLayout;

import javax.swing.*;

class Message {

}

class Client {
    private String user;

    private JFrame window;
    // private JLabel 

    public Client(String user) {
        this.user = user;

        window = new JFrame(user);
        window.setSize(250, 500);
        window.setLayout(new GridLayout(2, 1));

        window.setVisible(true);
    }
}

public class Chatty {
    public static void main(String[] args) {
        Client[] clients = {
            new Client("User 1"),
            new Client("User 2"),
            new Client("User 3"),
            new Client("User 4"),
        };
    }
}