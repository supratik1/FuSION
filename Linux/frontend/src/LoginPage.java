
// With only this:
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;
import javax.swing.*;

public class LoginPage extends JFrame {

    private CardLayout cardLayout;
    private JPanel mainPanel;

    private JTextField loginUsernameField;
    private JPasswordField loginPasswordField;
    private JTextField signupUsernameField;
    private JPasswordField signupPasswordField;
    private JTextField emailField;
    private JButton loginButton;
    private JButton signupButton;
    private static final String USER_DATA_FILE = "fusion_users.dat";
    private Map<String, UserData> userDatabase = new HashMap<>();

    public LoginPage() {
        initComponents();
        loadUserData();
    }

    private void initComponents() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setUndecorated(true);
        setSize(590, 590);
        setLocationRelativeTo(null);

        cardLayout = new CardLayout();
        mainPanel = new JPanel(cardLayout);

        JPanel loginPanel = createLoginPanel();
        JPanel signupPanel = createSignupPanel();

        mainPanel.add(loginPanel, "login");
        mainPanel.add(signupPanel, "signup");
        add(mainPanel);
    }

    private JPanel createLoginPanel() {
        JPanel panel = new JPanel();
        panel.setLayout(null);
        panel.setBackground(new Color(138, 43, 226));

        JLabel jLabel4 = new JLabel("FuSION");
        jLabel4.setFont(new Font("Segoe UI Semibold", Font.BOLD, 36));
        jLabel4.setForeground(Color.WHITE);
        jLabel4.setBounds(230, 60, 200, 40);
        panel.add(jLabel4);

        JLabel crossIcon = new JLabel(new ImageIcon(getClass().getResource("/icons8-cross-48.png")));
        crossIcon.setBounds(540, 5, 50, 50);
        crossIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        panel.add(crossIcon);

        JLabel accountIcon = new JLabel(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));
        accountIcon.setBounds(270, 130, 50, 50);
        panel.add(accountIcon);

        JLabel jLabel2 = new JLabel("username");
        jLabel2.setFont(new Font("Verdana", 0, 18));
        jLabel2.setForeground(Color.WHITE);
        jLabel2.setBounds(140, 200, 100, 30);
        panel.add(jLabel2);

        JLabel jLabel1 = new JLabel("password");
        jLabel1.setFont(new Font("Verdana", 0, 18));
        jLabel1.setForeground(Color.WHITE);
        jLabel1.setBounds(140, 290, 100, 30);
        panel.add(jLabel1);

        loginUsernameField = new JTextField();
        loginUsernameField.setFont(new Font("Verdana", 0, 18));
        loginUsernameField.setForeground(Color.WHITE);
        loginUsernameField.setOpaque(false);
        loginUsernameField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        loginUsernameField.setCaretColor(Color.white);
        loginUsernameField.setBounds(140, 230, 310, 40);
        panel.add(loginUsernameField);

        loginPasswordField = new JPasswordField();
        loginPasswordField.setForeground(Color.WHITE);
        loginPasswordField.setOpaque(false);
        loginPasswordField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        loginPasswordField.setCaretColor(Color.white);
        loginPasswordField.setBounds(140, 320, 310, 40);
        panel.add(loginPasswordField);

        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(new Font("Arial", Font.ITALIC, 12));
        statusLabel.setForeground(Color.RED);
        statusLabel.setBounds(220, 380, 140, 20);
        panel.add(statusLabel);

        loginButton = new JButton("Login");
        loginButton.setBounds(220, 410, 140, 40);
        panel.add(loginButton);

        JLabel switchBtn = new JLabel("Don't have an account? SignUp!");
        switchBtn.setFont(new Font("Verdana", 0, 12));
        switchBtn.setForeground(Color.RED);
        switchBtn.setBounds(190, 460, 400, 20);
        panel.add(switchBtn);

        loginButton.addActionListener(e -> {
            String username = loginUsernameField.getText();
            String password = new String(loginPasswordField.getPassword());

            if (authenticateUser(username, password)) {
                dispose();
                File userFile = new File("sessions/" + username + ".json");
                if (!userFile.exists()) {
                    try {
                        if (userFile.createNewFile()) {
                            System.out.println("Empty user file created.");
                        } else {
                            System.out.println("Failed to create file.");
                        }
                    } catch (IOException err) {
                        err.printStackTrace();
                    }
                }

                SwingUtilities.invokeLater(() -> new SessionFrame(username).setVisible(true));

            } else {
                statusLabel.setText("Invalid username or password");
                loginUsernameField.setText("");
                loginPasswordField.setText("");
                JOptionPane.showMessageDialog(null, "Wrong password!", "Login Error", JOptionPane.ERROR_MESSAGE);
            }
        });

        switchBtn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                cardLayout.show(mainPanel, "signup");
            }
        });

        crossIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        return panel;
    }

    private JPanel createSignupPanel() {
        JPanel panel = new JPanel();
        panel.setLayout(null);
        panel.setBackground(new Color(138, 43, 226));

        JLabel jLabel4 = new JLabel("FuSION");
        jLabel4.setFont(new Font("Segoe UI Semibold", Font.BOLD, 36));
        jLabel4.setForeground(Color.WHITE);
        jLabel4.setBounds(230, 60, 140, 40);
        panel.add(jLabel4);

        JLabel crossIcon = new JLabel(new ImageIcon(getClass().getResource("/icons8-cross-48.png")));
        crossIcon.setBounds(540, 5, 50, 50);
        crossIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        panel.add(crossIcon);

        JLabel accountIcon = new JLabel(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));
        accountIcon.setBounds(270, 130, 50, 50);
        panel.add(accountIcon);

        JLabel userLabel = new JLabel("username");
        userLabel.setFont(new Font("Verdana", 0, 18));
        userLabel.setForeground(Color.WHITE);
        userLabel.setBounds(140, 200, 100, 30);
        panel.add(userLabel);

        signupUsernameField = new JTextField();
        signupUsernameField.setFont(new Font("Verdana", 0, 18));
        signupUsernameField.setForeground(Color.WHITE);
        signupUsernameField.setOpaque(false);
        signupUsernameField.setCaretColor(Color.white);
        signupUsernameField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        signupUsernameField.setBounds(140, 230, 310, 30);
        panel.add(signupUsernameField);

        JLabel emailLabel = new JLabel("email");
        emailLabel.setFont(new Font("Verdana", 0, 18));
        emailLabel.setForeground(Color.WHITE);
        emailLabel.setBounds(140, 270, 100, 30);
        panel.add(emailLabel);

        emailField = new JTextField();
        emailField.setFont(new Font("Verdana", 0, 18));
        emailField.setForeground(Color.WHITE);
        emailField.setOpaque(false);
        emailField.setCaretColor(Color.white);
        emailField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        emailField.setBounds(140, 300, 310, 30);
        panel.add(emailField);

        JLabel passwordLabel = new JLabel("password");
        passwordLabel.setFont(new Font("Verdana", 0, 18));
        passwordLabel.setForeground(Color.WHITE);
        passwordLabel.setBounds(140, 340, 100, 30);
        panel.add(passwordLabel);

        signupPasswordField = new JPasswordField();
        signupPasswordField.setForeground(Color.WHITE);
        signupPasswordField.setOpaque(false);
        signupPasswordField.setCaretColor(Color.white);
        signupPasswordField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        signupPasswordField.setBounds(140, 370, 310, 30);
        panel.add(signupPasswordField);

        JLabel cnfpasswordLabel = new JLabel("confirm password");
        cnfpasswordLabel.setFont(new Font("Verdana", 0, 18));
        cnfpasswordLabel.setForeground(Color.WHITE);
        cnfpasswordLabel.setBounds(140, 410, 300, 30);
        panel.add(cnfpasswordLabel);

        JPasswordField cnfpasswordField = new JPasswordField();
        cnfpasswordField.setForeground(Color.WHITE);
        cnfpasswordField.setOpaque(false);
        cnfpasswordField.setCaretColor(Color.white);
        cnfpasswordField.setBorder(BorderFactory.createMatteBorder(0, 0, 2, 0, Color.WHITE));
        cnfpasswordField.setBounds(140, 440, 310, 30);
        panel.add(cnfpasswordField);

        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(new Font("Arial", Font.ITALIC, 12));
        statusLabel.setForeground(Color.RED);
        statusLabel.setBounds(220, 480, 140, 20);
        panel.add(statusLabel);

        signupButton = new JButton("Sign Up");
        signupButton.setBounds(220, 500, 140, 40);
        panel.add(signupButton);

        JLabel switchBtn = new JLabel("Have an account? Login!");
        switchBtn.setFont(new Font("Verdana", 0, 12));
        switchBtn.setForeground(Color.RED);
        switchBtn.setBounds(220, 550, 400, 20);
        panel.add(switchBtn);

        signupButton.addActionListener(e -> {
            String email = emailField.getText();
            String username = signupUsernameField.getText();
            String password = new String(signupPasswordField.getPassword());
            String confirmPassword = new String(cnfpasswordField.getPassword());

            if (email.isEmpty() || username.isEmpty() || password.isEmpty()) {
                statusLabel.setText("All fields are required");
                return;
            }

            if (!password.equals(confirmPassword)) {
                statusLabel.setText("Passwords do not match");
                return;
            }

            if (userDatabase.containsKey(username)) {
                statusLabel.setText("User already exists");
                return;
            }

            UserData newUser = new UserData();
            newUser.setUsername(username);
            newUser.setEmail(email);
            newUser.setPasswordHash(hashPassword(password));

            File userFile = new File("sessions/" + username + ".json");
            if (userFile.exists()) {
                if (!userFile.delete()) {

                    return;
                }
            }

            try {
                if (userFile.createNewFile()) {
                    System.out.println("Empty user file created.");
                } else {
                    System.out.println("Failed to create file.");
                }
            } catch (IOException err) {
                err.printStackTrace();
            }

            userDatabase.put(username, newUser);
            saveUserData();

            JOptionPane.showMessageDialog(this, "Account created successfully!");
            signupUsernameField.setText("");
            emailField.setText("");
            signupPasswordField.setText("");
            cnfpasswordField.setText("");
            statusLabel.setText(" ");
            cardLayout.show(mainPanel, "login");
        });

        switchBtn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                cardLayout.show(mainPanel, "login");
            }
        });

        crossIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        return panel;
    }

    private boolean authenticateUser(String username, String password) {
        if (!userDatabase.containsKey(username)) {
            return false;
        }

        UserData user = userDatabase.get(username);
        String hashedPassword = hashPassword(password);

        return hashedPassword.equals(user.getPasswordHash());
    }

    //hashes a password using SHA-256
    private String hashPassword(String password) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hash = digest.digest(password.getBytes());

            // Convert to hex string
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1) {
                    hexString.append('0');
                }
                hexString.append(hex);
            }
            return hexString.toString();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            return null;
        }
    }

    //Load user data from file
    @SuppressWarnings("unchecked")
    private void loadUserData() {
        File file = new File(USER_DATA_FILE);
        if (!file.exists()) {
            return;
        }

        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(file))) {
            userDatabase = (HashMap<String, UserData>) ois.readObject();

        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this,
                    "Error loading user data: " + e.getMessage(),
                    "Error",
                    JOptionPane.ERROR_MESSAGE);
        }
    }

    //Save user data to file
    private void saveUserData() {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(USER_DATA_FILE))) {
            oos.writeObject(userDatabase);

        } catch (IOException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this,
                    "Error saving user data: " + e.getMessage(),
                    "Error",
                    JOptionPane.ERROR_MESSAGE);
        }
    }

    //User data class to store user information
    private static class UserData implements Serializable {

        private static final long serialVersionUID = 1L;

        private String username;
        private String fullName;
        private String email;
        private String passwordHash;

        public String getUsername() {
            return username;
        }

        public void setUsername(String username) {
            this.username = username;
        }

        public String getFullName() {
            return fullName;
        }

        public void setFullName(String fullName) {
            this.fullName = fullName;
        }

        public String getEmail() {
            return email;
        }

        public void setEmail(String email) {
            this.email = email;
        }

        public String getPasswordHash() {
            return passwordHash;
        }

        public void setPasswordHash(String passwordHash) {
            this.passwordHash = passwordHash;
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new LoginPage().setVisible(true);
        });
    }

}
