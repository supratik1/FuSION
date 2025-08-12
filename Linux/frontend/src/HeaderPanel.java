import java.awt.*;
import javax.swing.*;

public class HeaderPanel extends RoundedPanel {
    private String username;
    private RoundedButton closeButton;
    private RoundedButton minimizeButton;

    public HeaderPanel(String username) {
        this.username = username;
        initialize();
    }

    private void initialize() {
        // Soft blue background
        setBackground(Color.WHITE); // Cornflower blue
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createEmptyBorder(10,10,10,10));

        // Username label - right aligned
        JLabel userLabel = new JLabel("User: " + username + " ");
        userLabel.setForeground(Color.BLACK);
        userLabel.setFont(new Font("Segoe UI", Font.BOLD, 16));

        minimizeButton = new RoundedButton("___", 20, new Dimension(40,30));
        // minimizeButton.setFont(new Font("Segoe UI", Font.BOLD, 25));
        minimizeButton.setFocusPainted(false);
        
        minimizeButton.setBorder(BorderFactory.createEmptyBorder(0,0,10,0));

        // Cross/close button
        closeButton = new RoundedButton("✕", 20, new Dimension(40,30));
        closeButton.setForeground(Color.WHITE);
        closeButton.setBackground(new Color(220, 20, 60)); // Crimson red
        closeButton.setFont(new Font("Segoe UI", Font.BOLD, 16));
        closeButton.setFocusPainted(false);
        closeButton.setMargin(new Insets(2, 8, 2, 8));
        // closeButton.setBorderPainted(false);


        minimizeButton.addActionListener(e -> {
            JFrame frame = (JFrame) SwingUtilities.getWindowAncestor(this);
            if (frame != null) {
                frame.setState(JFrame.ICONIFIED);
            }
        });

        closeButton.addActionListener(e -> System.exit(0));

       

        add(userLabel, BorderLayout.WEST);
        add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(minimizeButton);
            add(closeButton);
            setOpaque(false);
        }}, BorderLayout.EAST);
    }

}