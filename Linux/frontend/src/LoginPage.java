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

    private JTextField    loginUsernameField;
    private JPasswordField loginPasswordField;
    private JTextField    signupUsernameField;
    private JPasswordField signupPasswordField;
    private JTextField    emailField;
    private RoundedButton loginButton;
    private RoundedButton signupButton;

    private static final String USER_DATA_FILE = "fusion_users.dat";
    private Map<String, UserData> userDatabase = new HashMap<>();

    private final int[] drag = {0, 0};
    private MouseAdapter dragger;

    public LoginPage() {
        initComponents();
        loadUserData();
    }

    private void initComponents() {
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setUndecorated(true);
        setResizable(true);
        setSize(Theme.scale(700), Theme.scale(850));
        setMinimumSize(new Dimension(Theme.scale(500), Theme.scale(620)));
        setLocationRelativeTo(null);

        dragger = new MouseAdapter() {
            @Override public void mousePressed(MouseEvent e) {
                drag[0] = e.getXOnScreen() - getX();
                drag[1] = e.getYOnScreen() - getY();
            }
            @Override public void mouseDragged(MouseEvent e) {
                if ((getExtendedState() & JFrame.MAXIMIZED_BOTH) == 0)
                    setLocation(e.getXOnScreen() - drag[0], e.getYOnScreen() - drag[1]);
            }
        };

        // ── Gradient background ───────────────────────────────
        JPanel bg = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(new Color(6, 13, 22));
                g2.fillRect(0, 0, getWidth(), getHeight());
                paintGlow(g2, (int)(getWidth() * 0.30f), (int)(getHeight() * 0.20f),
                              (int)(getWidth() * 0.58f), new Color(37, 99, 235, 55));
                paintGlow(g2, (int)(getWidth() * 0.75f), (int)(getHeight() * 0.80f),
                              (int)(getWidth() * 0.50f), new Color(15, 50, 110, 76));
                g2.dispose();
            }
            private void paintGlow(Graphics2D g2, int cx, int cy, int r, Color c) {
                RadialGradientPaint p = new RadialGradientPaint(cx, cy, r,
                    new float[]{0f, 1f}, new Color[]{c, new Color(0, 0, 0, 0)});
                g2.setPaint(p);
                g2.fillRect(0, 0, getWidth(), getHeight());
            }
        };
        bg.setLayout(new BorderLayout());
        bg.addMouseListener(dragger);
        bg.addMouseMotionListener(dragger);

        // ── Window controls (top-right) ───────────────────────
        JPanel topBar = new JPanel(new FlowLayout(FlowLayout.RIGHT, Theme.GAP_MD, Theme.GAP_SM));
        topBar.setOpaque(false);

        JLabel maxIcon = winBtn("□");
        maxIcon.addMouseListener(new MouseAdapter() {
            @Override public void mouseClicked(MouseEvent e) {
                boolean mx = (getExtendedState() & JFrame.MAXIMIZED_BOTH) == JFrame.MAXIMIZED_BOTH;
                setExtendedState(mx ? JFrame.NORMAL : JFrame.MAXIMIZED_BOTH);
                maxIcon.setText(mx ? "□" : "❐");
            }
            @Override public void mouseEntered(MouseEvent e) { maxIcon.setForeground(new Color(90, 140, 190)); }
            @Override public void mouseExited(MouseEvent e)  { maxIcon.setForeground(new Color(72, 110, 148)); }
        });

        JLabel closeIcon = winBtn("x");
        closeIcon.addMouseListener(new MouseAdapter() {
            @Override public void mouseClicked(MouseEvent e) { dispose(); }
            @Override public void mouseEntered(MouseEvent e) { closeIcon.setForeground(new Color(239, 68, 68)); }
            @Override public void mouseExited(MouseEvent e)  { closeIcon.setForeground(new Color(72, 110, 148)); }
        });

        topBar.add(maxIcon);
        topBar.add(closeIcon);
        bg.add(topBar, BorderLayout.NORTH);

        // ── Centred card layout ───────────────────────────────
        cardLayout = new CardLayout();
        mainPanel = new JPanel(cardLayout);
        mainPanel.setOpaque(false);
        mainPanel.add(createLoginPanel(), "login");
        mainPanel.add(createSignupPanel(), "signup");

        JPanel center = new JPanel(new GridBagLayout());
        center.setOpaque(false);
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.anchor = GridBagConstraints.CENTER;
        center.add(mainPanel, gbc);
        bg.add(center, BorderLayout.CENTER);

        add(bg);
    }

    // ── Glass card factory ────────────────────────────────────

    private JPanel glassCard(int w) {
        JPanel p = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(new Color(15, 28, 46, 218));
                g2.fillRoundRect(0, 0, getWidth(), getHeight(), 28, 28);
                g2.setColor(new Color(59, 130, 246, 46));
                g2.setStroke(new BasicStroke(1f));
                g2.drawRoundRect(0, 0, getWidth()-1, getHeight()-1, 28, 28);
                g2.setColor(new Color(255, 255, 255, 13));
                g2.drawLine(14, 1, getWidth()-14, 1);
                g2.dispose();
            }
            @Override public boolean isOpaque() { return false; }
        };
        p.setLayout(new BoxLayout(p, BoxLayout.Y_AXIS));
        p.setBorder(BorderFactory.createEmptyBorder(Theme.scale(34), Theme.scale(36), Theme.scale(30), Theme.scale(36)));
        p.setMinimumSize(new Dimension(Theme.scale(w), 0));
        p.addMouseListener(dragger);
        p.addMouseMotionListener(dragger);
        return p;
    }

    // ── Login panel ───────────────────────────────────────────

    private JPanel createLoginPanel() {
        JPanel p = glassCard(380);

        JLabel mark = new JLabel("FuSION");
        mark.setFont(Theme.title(26));
        mark.setForeground(new Color(96, 165, 250));
        mark.setAlignmentX(LEFT_ALIGNMENT);
        p.add(mark);
        p.add(Box.createVerticalStrut(Theme.scale(22)));

        JLabel heading = new JLabel("Welcome back");
        heading.setFont(Theme.title(15));
        heading.setForeground(new Color(192, 216, 244));
        heading.setAlignmentX(LEFT_ALIGNMENT);
        p.add(heading);
        p.add(Box.createVerticalStrut(Theme.scale(3)));

        JLabel sub = new JLabel("Sign in to your account");
        sub.setFont(Theme.body(11));
        sub.setForeground(Theme.TEXT_MED);
        sub.setAlignmentX(LEFT_ALIGNMENT);
        p.add(sub);
        p.add(Box.createVerticalStrut(Theme.scale(24)));

        p.add(glassLabel("Username"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        loginUsernameField = glassField();
        loginUsernameField.setAlignmentX(LEFT_ALIGNMENT);
        p.add(loginUsernameField);
        p.add(Box.createVerticalStrut(Theme.scale(13)));

        p.add(glassLabel("Password"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        loginPasswordField = glassPassField();
        loginPasswordField.setAlignmentX(LEFT_ALIGNMENT);
        p.add(loginPasswordField);
        p.add(Box.createVerticalStrut(Theme.scale(6)));

        // Forgot password — accessible JButton styled as link
        JPanel forgotRow = new JPanel(new BorderLayout());
        forgotRow.setOpaque(false);
        forgotRow.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(22)));
        forgotRow.setAlignmentX(LEFT_ALIGNMENT);
        JButton forgot = new JButton("Forgot password?");
        forgot.setFont(Theme.body(11));
        forgot.setForeground(Theme.PRIMARY);
        forgot.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        forgot.setContentAreaFilled(false);
        forgot.setBorderPainted(false);
        forgot.setFocusPainted(true);
        forgot.setOpaque(false);
        forgot.addActionListener(e -> JOptionPane.showMessageDialog(LoginPage.this,
            "Please contact your administrator to reset your password.",
            "Forgot Password", JOptionPane.INFORMATION_MESSAGE));
        forgotRow.add(forgot, BorderLayout.EAST);
        p.add(forgotRow);
        p.add(Box.createVerticalStrut(Theme.scale(16)));

        JLabel status = new JLabel(" ");
        status.setFont(Theme.body(12));
        status.setForeground(Theme.DANGER);
        status.setAlignmentX(LEFT_ALIGNMENT);
        status.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(18)));
        p.add(status);
        p.add(Box.createVerticalStrut(Theme.scale(6)));

        loginButton = glassBtn("Sign In");
        loginButton.addActionListener(e -> {
            String uname = loginUsernameField.getText().trim();
            String pwd   = new String(loginPasswordField.getPassword());
            if (authenticateUser(uname, pwd)) {
                dispose();
                java.io.File dir = new java.io.File("frontend/sessions");
                if (!dir.exists()) dir.mkdirs();
                java.io.File uf = new java.io.File(dir, uname + ".json");
                if (!uf.exists()) {
                    try (FileWriter fw = new FileWriter(uf)) { fw.write("[]"); }
                    catch (IOException err) { err.printStackTrace(); }
                }
                SwingUtilities.invokeLater(() -> new SessionFrame(uname).setVisible(true));
            } else {
                status.setText("Invalid username or password");
                loginUsernameField.setText("");
                loginPasswordField.setText("");
            }
        });
        p.add(loginButton);
        p.add(Box.createVerticalStrut(Theme.scale(14)));

        JLabel sw = new JLabel("Don't have an account? Sign up");
        sw.setFont(Theme.body(12));
        sw.setForeground(Theme.PRIMARY);
        sw.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        sw.setAlignmentX(LEFT_ALIGNMENT);
        sw.addMouseListener(new MouseAdapter() {
            @Override public void mouseClicked(MouseEvent e) { cardLayout.show(mainPanel, "signup"); }
        });
        p.add(sw);

        return p;
    }

    // ── Signup panel ──────────────────────────────────────────

    private JPanel createSignupPanel() {
        JPanel p = glassCard(380);

        JLabel mark = new JLabel("FuSION");
        mark.setFont(Theme.title(26));
        mark.setForeground(new Color(96, 165, 250));
        mark.setAlignmentX(LEFT_ALIGNMENT);
        p.add(mark);
        p.add(Box.createVerticalStrut(Theme.scale(18)));

        JLabel heading = new JLabel("Create an account");
        heading.setFont(Theme.title(15));
        heading.setForeground(new Color(192, 216, 244));
        heading.setAlignmentX(LEFT_ALIGNMENT);
        p.add(heading);
        p.add(Box.createVerticalStrut(Theme.scale(3)));

        JLabel sub = new JLabel("Fill in the details below to get started");
        sub.setFont(Theme.body(11));
        sub.setForeground(Theme.TEXT_MED);
        sub.setAlignmentX(LEFT_ALIGNMENT);
        p.add(sub);
        p.add(Box.createVerticalStrut(Theme.scale(20)));

        p.add(glassLabel("Username"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        signupUsernameField = glassField();
        signupUsernameField.setAlignmentX(LEFT_ALIGNMENT);
        p.add(signupUsernameField);
        p.add(Box.createVerticalStrut(Theme.scale(11)));

        p.add(glassLabel("Email"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        emailField = glassField();
        emailField.setAlignmentX(LEFT_ALIGNMENT);
        p.add(emailField);
        p.add(Box.createVerticalStrut(Theme.scale(11)));

        p.add(glassLabel("Password"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        signupPasswordField = glassPassField();
        signupPasswordField.setAlignmentX(LEFT_ALIGNMENT);
        p.add(signupPasswordField);
        p.add(Box.createVerticalStrut(Theme.scale(11)));

        JPasswordField cnfPass = glassPassField();
        p.add(glassLabel("Confirm Password"));
        p.add(Box.createVerticalStrut(Theme.scale(5)));
        cnfPass.setAlignmentX(LEFT_ALIGNMENT);
        p.add(cnfPass);
        p.add(Box.createVerticalStrut(Theme.scale(8)));

        JLabel status = new JLabel(" ");
        status.setFont(Theme.body(12));
        status.setForeground(Theme.DANGER);
        status.setAlignmentX(LEFT_ALIGNMENT);
        status.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(18)));
        p.add(status);
        p.add(Box.createVerticalStrut(Theme.scale(6)));

        signupButton = glassBtn("Sign Up");
        signupButton.addActionListener(e -> {
            String email = emailField.getText().trim();
            String uname = signupUsernameField.getText().trim();
            String pwd   = new String(signupPasswordField.getPassword());
            String conf  = new String(cnfPass.getPassword());

            if (email.isEmpty() || uname.isEmpty() || pwd.isEmpty()) {
                status.setText("All fields are required"); return;
            }
            if (!pwd.equals(conf)) { status.setText("Passwords do not match"); return; }
            if (userDatabase.containsKey(uname)) { status.setText("Username already exists"); return; }

            UserData nd = new UserData();
            nd.setUsername(uname); nd.setEmail(email); nd.setPasswordHash(hashPassword(pwd));

            java.io.File dir = new java.io.File("frontend/sessions");
            if (!dir.exists()) dir.mkdirs();
            java.io.File uf = new java.io.File(dir, uname + ".json");
            if (!uf.exists()) {
                try (FileWriter fw = new FileWriter(uf)) { fw.write("[]"); }
                catch (IOException err) { err.printStackTrace(); }
            }

            userDatabase.put(uname, nd);
            saveUserData();
            JOptionPane.showMessageDialog(this, "Account created! You can now sign in.");
            signupUsernameField.setText(""); emailField.setText("");
            signupPasswordField.setText(""); cnfPass.setText(""); status.setText(" ");
            cardLayout.show(mainPanel, "login");
        });
        p.add(signupButton);
        p.add(Box.createVerticalStrut(Theme.scale(12)));

        JLabel sw = new JLabel("Already have an account? Sign in");
        sw.setFont(Theme.body(12));
        sw.setForeground(Theme.PRIMARY);
        sw.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        sw.setAlignmentX(LEFT_ALIGNMENT);
        sw.addMouseListener(new MouseAdapter() {
            @Override public void mouseClicked(MouseEvent e) { cardLayout.show(mainPanel, "login"); }
        });
        p.add(sw);

        return p;
    }

    // ── Shared form element helpers ───────────────────────────

    private JLabel glassLabel(String text) {
        JLabel l = new JLabel(text.toUpperCase());
        l.setFont(Theme.title(11));
        l.setForeground(Theme.TEXT_MED);
        l.setAlignmentX(LEFT_ALIGNMENT);
        return l;
    }

    private JTextField glassField() {
        JTextField f = new JTextField();
        styleGlassField(f);
        return f;
    }

    private JPasswordField glassPassField() {
        JPasswordField f = new JPasswordField();
        styleGlassField(f);
        return f;
    }

    private void styleGlassField(JTextField f) {
        f.setColumns(22);
        f.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(40)));
        f.setMargin(new Insets(Theme.scale(8), Theme.scale(12), Theme.scale(8), Theme.scale(12)));
        f.setBackground(new Color(8, 16, 26));
        f.setForeground(new Color(192, 216, 244));
        f.setCaretColor(Theme.PRIMARY);
        f.setFont(Theme.body(13));
        f.setOpaque(true);

        javax.swing.border.Border nb = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(59, 130, 246, 51), 1),
            BorderFactory.createEmptyBorder(Theme.scale(8), Theme.scale(14), Theme.scale(8), Theme.scale(14)));
        javax.swing.border.Border fb = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.PRIMARY, 1),
            BorderFactory.createEmptyBorder(Theme.scale(7), Theme.scale(13), Theme.scale(7), Theme.scale(13)));
        f.setBorder(nb);
        f.addFocusListener(new FocusAdapter() {
            @Override public void focusGained(FocusEvent e) { f.setBorder(fb); }
            @Override public void focusLost(FocusEvent e)   { f.setBorder(nb); }
        });
    }

    private RoundedButton glassBtn(String text) {
        RoundedButton b = new RoundedButton(text, 8, new Dimension(Integer.MAX_VALUE, Theme.scale(42)));
        b.setBackground(Theme.PRIMARY);
        b.setForeground(Color.WHITE);
        b.setFont(Theme.title(14));
        b.setAlignmentX(LEFT_ALIGNMENT);
        b.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(42)));
        return b;
    }

    private JLabel winBtn(String text) {
        JLabel l = new JLabel(text);
        l.setFont(new Font("SansSerif", Font.BOLD, Theme.scale(14)));
        l.setForeground(new Color(72, 110, 148));
        l.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        return l;
    }

    // ── Auth + persistence ────────────────────────────────────

    private boolean authenticateUser(String username, String password) {
        if (!userDatabase.containsKey(username)) return false;
        return hashPassword(password).equals(userDatabase.get(username).getPasswordHash());
    }

    private String hashPassword(String password) {
        try {
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            byte[] hash = md.digest(password.getBytes());
            StringBuilder sb = new StringBuilder();
            for (byte b : hash) { String h = Integer.toHexString(0xff & b); if (h.length() == 1) sb.append('0'); sb.append(h); }
            return sb.toString();
        } catch (NoSuchAlgorithmException e) { return null; }
    }

    @SuppressWarnings("unchecked")
    private void loadUserData() {
        java.io.File f = new java.io.File(USER_DATA_FILE);
        if (!f.exists()) return;
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(f))) {
            userDatabase = (HashMap<String, UserData>) ois.readObject();
        } catch (IOException | ClassNotFoundException e) { e.printStackTrace(); }
    }

    private void saveUserData() {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(USER_DATA_FILE))) {
            oos.writeObject(userDatabase);
        } catch (IOException e) { e.printStackTrace(); }
    }

    private static class UserData implements Serializable {
        private static final long serialVersionUID = 1L;
        private String username, fullName, email, passwordHash;
        public String getUsername()  { return username; }
        public void setUsername(String u)  { username = u; }
        public String getFullName()  { return fullName; }
        public void setFullName(String n)  { fullName = n; }
        public String getEmail()     { return email; }
        public void setEmail(String e)     { email = e; }
        public String getPasswordHash()    { return passwordHash; }
        public void setPasswordHash(String h) { passwordHash = h; }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new LoginPage().setVisible(true));
    }
}
