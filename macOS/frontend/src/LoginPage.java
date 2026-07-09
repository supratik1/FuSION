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
    private RoundedButton loginButton;
    private RoundedButton signupButton;
    private static final String USER_DATA_FILE = "fusion_users.dat";
    private Map<String, UserData> userDatabase = new HashMap<>();
    private final ImageIcon userIcon = new ImageIcon(getClass().getResource("/icons8-male-user-50.png"));

    public LoginPage() {
        initComponents();
        loadUserData();
    }

    private void initComponents() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setUndecorated(true);
        setResizable(true);
        setSize(700, 850);
        setMinimumSize(new Dimension(500, 600));
        setLocationRelativeTo(null);

        cardLayout = new CardLayout();
        mainPanel = new JPanel();
        mainPanel.setLayout(cardLayout);
        mainPanel.setBackground(Theme.BG);
        mainPanel.setBorder(BorderFactory.createEmptyBorder(80, 120, 80, 120));

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
        appName.setFont(Theme.title(36));
        appName.setForeground(Theme.PRIMARY);

        JLabel maxIcon = new JLabel("\u25a1");
        maxIcon.setFont(new Font("SansSerif", Font.BOLD, 16));
        maxIcon.setForeground(Theme.TEXT_LIGHT);
        maxIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        maxIcon.setToolTipText("Maximize / Restore");
        maxIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if ((LoginPage.this.getExtendedState() & JFrame.MAXIMIZED_BOTH) == JFrame.MAXIMIZED_BOTH) {
                    LoginPage.this.setExtendedState(JFrame.NORMAL);
                    maxIcon.setText("\u25a1");
                } else {
                    LoginPage.this.setExtendedState(JFrame.MAXIMIZED_BOTH);
                    maxIcon.setText("\u2750");
                }
            }
        });

        JLabel closeIcon = new JLabel("\u2716");
        closeIcon.setFont(new Font("SansSerif", Font.BOLD, 18));
        closeIcon.setForeground(Theme.TEXT_LIGHT);
        closeIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        closeIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        JPanel winControls = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        winControls.setOpaque(false);
        winControls.add(maxIcon);
        winControls.add(closeIcon);

        final int[] loginDragStart = {0, 0};
        header.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) { loginDragStart[0] = e.getX(); loginDragStart[1] = e.getY(); }
            @Override
            public void mouseClicked(MouseEvent e) { if (e.getClickCount() == 2) maxIcon.dispatchEvent(e); }
        });
        header.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                if ((LoginPage.this.getExtendedState() & JFrame.MAXIMIZED_BOTH) == 0) {
                    Point loc = LoginPage.this.getLocation();
                    LoginPage.this.setLocation(loc.x + e.getX() - loginDragStart[0],
                                               loc.y + e.getY() - loginDragStart[1]);
                }
            }
        });

        header.add(appName, BorderLayout.CENTER);
        header.add(winControls, BorderLayout.EAST);

        panel.add(header, BorderLayout.NORTH);

        // Body
        JPanel bodyPanel = new JPanel();
        bodyPanel.setOpaque(false);
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.setBorder(BorderFactory.createEmptyBorder(30, 50, 20, 50));

        // Icon centered
        JLabel iconLabel = new JLabel(userIcon);
        iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        bodyPanel.add(iconLabel);
        bodyPanel.add(Box.createVerticalStrut(8));

        JLabel subtitle = new JLabel("Sign in to your account");
        subtitle.setFont(Theme.body(13));
        subtitle.setForeground(Theme.TEXT_MED);
        subtitle.setAlignmentX(Component.CENTER_ALIGNMENT);
        bodyPanel.add(subtitle);
        bodyPanel.add(Box.createVerticalStrut(24));

        // Username field \u2014 label above field
        JPanel usernameBlock = new JPanel();
        usernameBlock.setLayout(new BoxLayout(usernameBlock, BoxLayout.Y_AXIS));
        usernameBlock.setOpaque(false);
        usernameBlock.setMaximumSize(new Dimension(310, 70));
        usernameBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel userLabel = new JLabel("Username");
        userLabel.setFont(Theme.body(13));
        userLabel.setForeground(Theme.TEXT_MED);
        userLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        loginUsernameField = createStyledTextField();
        loginUsernameField.setAlignmentX(Component.LEFT_ALIGNMENT);
        usernameBlock.add(userLabel);
        usernameBlock.add(Box.createVerticalStrut(4));
        usernameBlock.add(loginUsernameField);
        bodyPanel.add(usernameBlock);
        bodyPanel.add(Box.createVerticalStrut(16));

        // Password field \u2014 label above field
        JPanel passwordBlock = new JPanel();
        passwordBlock.setLayout(new BoxLayout(passwordBlock, BoxLayout.Y_AXIS));
        passwordBlock.setOpaque(false);
        passwordBlock.setMaximumSize(new Dimension(310, 70));
        passwordBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel passLabel = new JLabel("Password");
        passLabel.setFont(Theme.body(13));
        passLabel.setForeground(Theme.TEXT_MED);
        passLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        loginPasswordField = createStyledPasswordField();
        loginPasswordField.setAlignmentX(Component.LEFT_ALIGNMENT);
        passwordBlock.add(passLabel);
        passwordBlock.add(Box.createVerticalStrut(4));
        passwordBlock.add(loginPasswordField);
        bodyPanel.add(passwordBlock);
        bodyPanel.add(Box.createVerticalStrut(12));

        // Status label
        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(Theme.body(13));
        statusLabel.setForeground(Theme.DANGER);
        statusLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        bodyPanel.add(statusLabel);
        bodyPanel.add(Box.createVerticalStrut(10));

        // Login button
        loginButton = createStyledButton("Sign In");
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
        JLabel switchBtn = new JLabel("Don't have an account? Sign up");
        switchBtn.setFont(Theme.body(13));
        switchBtn.setForeground(Theme.PRIMARY);
        switchBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        switchBtn.setAlignmentX(Component.CENTER_ALIGNMENT);
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
        appName.setFont(Theme.title(36));
        appName.setForeground(Theme.PRIMARY);

        JLabel maxIcon2 = new JLabel("\u25a1");
        maxIcon2.setFont(new Font("SansSerif", Font.BOLD, 16));
        maxIcon2.setForeground(Theme.TEXT_LIGHT);
        maxIcon2.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        maxIcon2.setToolTipText("Maximize / Restore");
        maxIcon2.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if ((LoginPage.this.getExtendedState() & JFrame.MAXIMIZED_BOTH) == JFrame.MAXIMIZED_BOTH) {
                    LoginPage.this.setExtendedState(JFrame.NORMAL);
                    maxIcon2.setText("\u25a1");
                } else {
                    LoginPage.this.setExtendedState(JFrame.MAXIMIZED_BOTH);
                    maxIcon2.setText("\u2750");
                }
            }
        });

        JLabel closeIcon = new JLabel("\u2716");
        closeIcon.setFont(new Font("SansSerif", Font.BOLD, 18));
        closeIcon.setForeground(Theme.TEXT_LIGHT);
        closeIcon.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        closeIcon.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                dispose();
            }
        });

        JPanel winControls2 = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        winControls2.setOpaque(false);
        winControls2.add(maxIcon2);
        winControls2.add(closeIcon);

        final int[] signupDragStart = {0, 0};
        header.addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent e) { signupDragStart[0] = e.getX(); signupDragStart[1] = e.getY(); }
        });
        header.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseDragged(MouseEvent e) {
                if ((LoginPage.this.getExtendedState() & JFrame.MAXIMIZED_BOTH) == 0) {
                    Point loc = LoginPage.this.getLocation();
                    LoginPage.this.setLocation(loc.x + e.getX() - signupDragStart[0],
                                               loc.y + e.getY() - signupDragStart[1]);
                }
            }
        });

        header.add(appName, BorderLayout.WEST);
        header.add(winControls2, BorderLayout.EAST);

        panel.add(header, BorderLayout.NORTH);

        // Body
        JPanel body = new JPanel();
        body.setOpaque(false);
        body.setLayout(new BoxLayout(body, BoxLayout.Y_AXIS));
        body.setBorder(BorderFactory.createEmptyBorder(20, 50, 20, 50));

        JLabel iconLabel = new JLabel(userIcon);
        iconLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(iconLabel);
        body.add(Box.createVerticalStrut(6));

        JLabel subtitle2 = new JLabel("Create a new account");
        subtitle2.setFont(Theme.body(13));
        subtitle2.setForeground(Theme.TEXT_MED);
        subtitle2.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(subtitle2);
        body.add(Box.createVerticalStrut(18));

        // Username
        JPanel userBlock = new JPanel();
        userBlock.setLayout(new BoxLayout(userBlock, BoxLayout.Y_AXIS));
        userBlock.setOpaque(false);
        userBlock.setMaximumSize(new Dimension(310, 70));
        userBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel signuserLabel = new JLabel("Username");
        signuserLabel.setFont(Theme.body(13));
        signuserLabel.setForeground(Theme.TEXT_MED);
        signuserLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        signupUsernameField = createStyledTextField();
        signupUsernameField.setAlignmentX(Component.LEFT_ALIGNMENT);
        userBlock.add(signuserLabel);
        userBlock.add(Box.createVerticalStrut(4));
        userBlock.add(signupUsernameField);
        body.add(userBlock);
        body.add(Box.createVerticalStrut(12));

        // Email
        JPanel emailBlock = new JPanel();
        emailBlock.setLayout(new BoxLayout(emailBlock, BoxLayout.Y_AXIS));
        emailBlock.setOpaque(false);
        emailBlock.setMaximumSize(new Dimension(310, 70));
        emailBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel emailLabel = new JLabel("Email");
        emailLabel.setFont(Theme.body(13));
        emailLabel.setForeground(Theme.TEXT_MED);
        emailLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        emailField = createStyledTextField();
        emailField.setAlignmentX(Component.LEFT_ALIGNMENT);
        emailBlock.add(emailLabel);
        emailBlock.add(Box.createVerticalStrut(4));
        emailBlock.add(emailField);
        body.add(emailBlock);
        body.add(Box.createVerticalStrut(12));

        // Password
        JPanel passBlock = new JPanel();
        passBlock.setLayout(new BoxLayout(passBlock, BoxLayout.Y_AXIS));
        passBlock.setOpaque(false);
        passBlock.setMaximumSize(new Dimension(310, 70));
        passBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel signpassLabel = new JLabel("Password");
        signpassLabel.setFont(Theme.body(13));
        signpassLabel.setForeground(Theme.TEXT_MED);
        signpassLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        signupPasswordField = createStyledPasswordField();
        signupPasswordField.setAlignmentX(Component.LEFT_ALIGNMENT);
        passBlock.add(signpassLabel);
        passBlock.add(Box.createVerticalStrut(4));
        passBlock.add(signupPasswordField);
        body.add(passBlock);
        body.add(Box.createVerticalStrut(12));

        // Confirm password
        JPanel cnfBlock = new JPanel();
        cnfBlock.setLayout(new BoxLayout(cnfBlock, BoxLayout.Y_AXIS));
        cnfBlock.setOpaque(false);
        cnfBlock.setMaximumSize(new Dimension(310, 70));
        cnfBlock.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel cnfpassLabel = new JLabel("Confirm Password");
        cnfpassLabel.setFont(Theme.body(13));
        cnfpassLabel.setForeground(Theme.TEXT_MED);
        cnfpassLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        JPasswordField cnfpasswordField = createStyledPasswordField();
        cnfpasswordField.setAlignmentX(Component.LEFT_ALIGNMENT);
        cnfBlock.add(cnfpassLabel);
        cnfBlock.add(Box.createVerticalStrut(4));
        cnfBlock.add(cnfpasswordField);
        body.add(cnfBlock);
        body.add(Box.createVerticalStrut(10));

        // Status
        JLabel statusLabel = new JLabel(" ");
        statusLabel.setFont(Theme.body(13));
        statusLabel.setForeground(Theme.DANGER);
        statusLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(statusLabel);
        body.add(Box.createVerticalStrut(8));

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
        JLabel switchBtn = new JLabel("Already have an account? Sign in");
        switchBtn.setFont(Theme.body(13));
        switchBtn.setForeground(Theme.PRIMARY);
        switchBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        switchBtn.setAlignmentX(Component.CENTER_ALIGNMENT);
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

    // Style helpers
    private JTextField createStyledTextField() {
        JTextField tf = new JTextField();
        tf.setPreferredSize(new Dimension(310, 40));
        tf.setBackground(Color.WHITE);
        tf.setForeground(Color.BLACK);
        tf.setFont(new Font("Segoe UI", Font.PLAIN, 16));
        tf.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 2, 0, Theme.PRIMARY),
            BorderFactory.createEmptyBorder(5, 10, 5, 10)));
        return tf;
    }

    private JPasswordField createStyledPasswordField() {
        JPasswordField pf = new JPasswordField();
        pf.setPreferredSize(new Dimension(310, 40));
        pf.setBackground(Color.WHITE);
        pf.setForeground(Color.BLACK);
        pf.setFont(new Font("Segoe UI", Font.PLAIN, 16));
        pf.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 2, 0, Theme.PRIMARY),
            BorderFactory.createEmptyBorder(5, 10, 5, 10)));
        return pf;
    }

    private RoundedButton createStyledButton(String text) {
        // RoundedButton uses custom paintComponent so background/foreground
        // work identically on all platforms (macOS Aqua L&F ignores setBackground
        // on plain JButton, making white text invisible on white button).
        RoundedButton btn = new RoundedButton(text, 20, new Dimension(310, 50));
        btn.setBackground(Theme.PRIMARY);
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 16));
        btn.setAlignmentX(Component.CENTER_ALIGNMENT);
        return btn;
    }

    // Authentication and persistence
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

