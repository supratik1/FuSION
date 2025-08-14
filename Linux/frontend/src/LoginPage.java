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
        setSize(700, 850);
        setLocationRelativeTo(null);

        cardLayout = new CardLayout();
        mainPanel = new JPanel();
        mainPanel.setLayout(cardLayout);
        mainPanel.setBorder(BorderFactory.createEmptyBorder(100,100,100,100));

        RoundedPanel loginPanel = createLoginPanel();
        RoundedPanel signupPanel = createSignupPanel();

        mainPanel.add(loginPanel, "login");
        mainPanel.add(signupPanel, "signup");
        add(mainPanel);
    }

    private RoundedPanel createLoginPanel() {
        RoundedPanel panel = new RoundedPanel();
        panel.setLayout(new BorderLayout());
        panel.setBackground(Color.WHITE);
        panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 40, 40));

        // Header with app name and close button
        JPanel header = new JPanel(new BorderLayout());
        header.setOpaque(false);
        JLabel appName = new JLabel("FuSION");
        appName.setFont(new Font("Segoe UI Semibold", Font.BOLD, 36));
        appName.setForeground(new Color(50, 50, 50));

        JLabel closeIcon = new JLabel("\u2716"); // Unicode X
        closeIcon.setFont(new Font("SansSerif", Font.BOLD, 24));
        closeIcon.setForeground(Color.GRAY);
        closeIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        closeIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        header.add(appName, BorderLayout.CENTER);
        header.add(closeIcon, BorderLayout.EAST);

        panel.add(header, BorderLayout.NORTH);

        // Body
        JPanel bodyPanel = new JPanel();
        bodyPanel.setOpaque(false);
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.setBorder(BorderFactory.createEmptyBorder(60, 60, 20, 60));

        JLabel iconLabel = new JLabel(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));
        iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        bodyPanel.add(new JPanel(){{
            add(iconLabel);
            setBackground(Color.WHITE);
        }});
        bodyPanel.add(Box.createVerticalStrut(20));

        // Username field
        JLabel userLabel = new JLabel("username");
        userLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        userLabel.setForeground(Color.GRAY);
        
        bodyPanel.add(Box.createVerticalStrut(5));

        loginUsernameField = createStyledTextField();
        bodyPanel.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(userLabel);
            add(loginUsernameField);
            
            setOpaque(false);
        }});
        bodyPanel.add(Box.createVerticalStrut(15));

        // Password field
        JLabel passLabel = new JLabel("password");
        passLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        passLabel.setForeground(Color.GRAY);
        
        bodyPanel.add(Box.createVerticalStrut(5));

        loginPasswordField = createStyledPasswordField();
        bodyPanel.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(passLabel);
            add(loginPasswordField);
            setOpaque(false);
        }});
        bodyPanel.add(Box.createVerticalStrut(15));

        // Status label
        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(new Font("Segoe UI", Font.BOLD, 14));
        statusLabel.setForeground(Color.RED);
        bodyPanel.add(statusLabel);
        bodyPanel.add(Box.createVerticalStrut(15));

        // Login button
        loginButton = createStyledButton("Login");
        loginButton.addActionListener(e -> {
            String username = loginUsernameField.getText();
            String password = new String(loginPasswordField.getPassword());

            if (authenticateUser(username, password)) {
                dispose();
                File sessionsDir = new File("frontend/sessions");

                if (!sessionsDir.exists()) {
                    sessionsDir.mkdirs();
                }

                // Create the user file
                File userFile = new File(sessionsDir, username + ".json");
                if (!userFile.exists()) {
                    try (FileWriter fw = new FileWriter(userFile)) {
                        fw.write("[]"); // valid empty JSON array
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
        bodyPanel.add(loginButton);
        bodyPanel.add(Box.createVerticalStrut(20));

        // Switch to signup
        JLabel switchBtn = new JLabel("Don't have an account? SignUp!");
        switchBtn.setFont(new Font("Segoe UI", Font.PLAIN, 14));
        switchBtn.setForeground(new Color(100, 149, 237));
        switchBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        switchBtn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                cardLayout.show(mainPanel, "signup");
            }
        });
        bodyPanel.add(switchBtn);

        panel.add(bodyPanel, BorderLayout.CENTER);
        return panel;
    }

    private RoundedPanel createSignupPanel() {
        RoundedPanel panel = new RoundedPanel();
        panel.setBackground(Color.WHITE);
        panel.setLayout(new BorderLayout());
        panel.setBorder(BorderFactory.createEmptyBorder(20, 40, 40, 40));

        // Header
        JPanel header = new JPanel(new BorderLayout());
        header.setOpaque(false);
        JLabel appName = new JLabel("FuSION");
        appName.setFont(new Font("Segoe UI Semibold", Font.BOLD, 36));
        appName.setForeground(new Color(50, 50, 50));

        JLabel closeIcon = new JLabel("\u2716");
        closeIcon.setFont(new Font("SansSerif", Font.BOLD, 24));
        closeIcon.setForeground(Color.GRAY);
        closeIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        closeIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        header.add(appName, BorderLayout.WEST);
        header.add(closeIcon, BorderLayout.EAST);

        panel.add(header, BorderLayout.NORTH);

        // Body
        JPanel body = new JPanel();
        body.setOpaque(false);
        body.setLayout(new BoxLayout(body, BoxLayout.Y_AXIS));
        body.setBorder(BorderFactory.createEmptyBorder(30, 60, 20, 60));

        JLabel iconLabel = new JLabel(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));
        iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(iconLabel);
        body.add(Box.createVerticalStrut(20));

        // Username
        JLabel signuserLabel = new JLabel("username");
        signuserLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        signuserLabel.setForeground(Color.GRAY);

        signupUsernameField = createStyledTextField();

        body.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(signuserLabel);
            add(signupUsernameField);
            setOpaque(false);
        }});

        body.add(Box.createVerticalStrut(15));

        // Email
        JLabel emailLabel = new JLabel("email");
        emailLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        emailLabel.setForeground(Color.GRAY);
        emailField = createStyledTextField();

        body.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(emailLabel);
            add(emailField);
            setOpaque(false);
            
        }});
        body.add(Box.createVerticalStrut(15));

        // Password
        JLabel signpassLabel = new JLabel("password");
        signpassLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        signpassLabel.setForeground(Color.GRAY);
        signupPasswordField = createStyledPasswordField();
        body.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(signpassLabel);
            add(signupPasswordField);
            setOpaque(false);
            
        }});
        body.add(Box.createVerticalStrut(15));

        // Confirm password
        JLabel cnfpassLabel = new JLabel("confirm password");
        cnfpassLabel.setFont(new Font("Segoe UI", Font.PLAIN, 18));
        cnfpassLabel.setForeground(Color.GRAY);
        JPasswordField cnfpasswordField = createStyledPasswordField();
        body.add(new JPanel(new FlowLayout(FlowLayout.LEFT)){{
            add(cnfpassLabel);
            add(cnfpasswordField);
            setOpaque(false);
            
        }});
        body.add(Box.createVerticalStrut(15));

        // Status
        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(new Font("Segoe UI", Font.BOLD, 14));
        statusLabel.setForeground(Color.RED);
        body.add(statusLabel);
        body.add(Box.createVerticalStrut(15));

        // Signup button
        signupButton = createStyledButton("Sign Up");
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

            File sessionsDir = new File("frontend/sessions");

            if (!sessionsDir.exists()) {
                sessionsDir.mkdirs();
            }

            // Create the user file
            File userFile = new File(sessionsDir, username + ".json");
            if (!userFile.exists()) {
                try (FileWriter fw = new FileWriter(userFile)) {
                    fw.write("[]"); //  valid empty JSON array
                } catch (IOException err) {
                    err.printStackTrace();
                }
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
        body.add(signupButton);
        body.add(Box.createVerticalStrut(20));

        // Switch to login
        JLabel switchBtn = new JLabel("Have an account? Login!");
        switchBtn.setFont(new Font("Segoe UI", Font.PLAIN, 14));
        switchBtn.setForeground(new Color(100, 149, 237));
        switchBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        switchBtn.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                cardLayout.show(mainPanel, "login");
            }
        });
        body.add(switchBtn);

        panel.add(body, BorderLayout.CENTER);
        return panel;
    }

    // 🔹 Style helpers
    private JTextField createStyledTextField() {
        JTextField tf = new JTextField();
        tf.setPreferredSize(new Dimension(310, 40));
        tf.setBackground(Color.WHITE);
        tf.setForeground(Color.BLACK);
        tf.setFont(new Font("Segoe UI", Font.PLAIN, 16));
        tf.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 2, 0, Color.GRAY),
            BorderFactory.createEmptyBorder(5,10,5,10)
            
                
        ));
        return tf;
    }

    private JPasswordField createStyledPasswordField() {
        JPasswordField pf = new JPasswordField();
        pf.setPreferredSize(new Dimension(310, 40));
        pf.setBackground(Color.WHITE);
        pf.setForeground(Color.BLACK);
        pf.setFont(new Font("Segoe UI", Font.PLAIN, 16));
        pf.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createMatteBorder(0, 0, 2, 0, Color.GRAY),
                BorderFactory.createEmptyBorder(5,10,5,10)
        ));
        return pf;
    }

    private JButton createStyledButton(String text) {
        JButton btn = new JButton(text);
        btn.setBackground(new Color(100, 149, 237)); // soft blue
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 16));
        btn.setFocusPainted(false);
        btn.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));
        return btn;
    }

    // 🔹 Authentication and persistence (unchanged)
    private boolean authenticateUser(String username, String password) {
        if (!userDatabase.containsKey(username)) return false;
        UserData user = userDatabase.get(username);
        return hashPassword(password).equals(user.getPasswordHash());
    }

    private String hashPassword(String password) {
        try {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] hash = digest.digest(password.getBytes());
            StringBuilder hexString = new StringBuilder();
            for (byte b : hash) {
                String hex = Integer.toHexString(0xff & b);
                if (hex.length() == 1) hexString.append('0');
                hexString.append(hex);
            }
            return hexString.toString();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
            return null;
        }
    }

    @SuppressWarnings("unchecked")
    private void loadUserData() {
        File file = new File(USER_DATA_FILE);
        if (!file.exists()) return;
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(file))) {
            userDatabase = (HashMap<String, UserData>) ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error loading user data: " + e.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void saveUserData() {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(USER_DATA_FILE))) {
            oos.writeObject(userDatabase);
        } catch (IOException e) {
            e.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error saving user data: " + e.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private static class UserData implements Serializable {
        private static final long serialVersionUID = 1L;
        private String username;
        private String fullName;
        private String email;
        private String passwordHash;
        public String getUsername() { return username; }
        public void setUsername(String username) { this.username = username; }
        public String getFullName() { return fullName; }
        public void setFullName(String fullName) { this.fullName = fullName; }
        public String getEmail() { return email; }
        public void setEmail(String email) { this.email = email; }
        public String getPasswordHash() { return passwordHash; }
        public void setPasswordHash(String passwordHash) { this.passwordHash = passwordHash; }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new LoginPage().setVisible(true));
    }
}

