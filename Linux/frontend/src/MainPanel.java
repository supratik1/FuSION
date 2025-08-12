
import java.awt.*;
import java.io.File;
import javax.swing.*;

public class MainPanel extends RoundedPanel {

    private JLabel currDirectory;
    private RoundedButton changeDirButton;

    private JPanel fileSelectPanel;
    protected File selectedDirectory;
    //protected UserInput user;

    public MainPanel(String username, UserInput user, CardLayout cardLayout, JPanel cardPanel) {

        // user = new UserInput(fileName, id);
        // user.setUsername(username);
        // setTitle("FuSiON");
        // setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        // setSize(1400, 800);
        // setSize(1400, 800);
        // setMinimumSize(new Dimension(1400, 800));
        // setMaximumSize(new Dimension(1400, 800));
        // setResizable(false);
        // setLocationRelativeTo(null);
        // setLayout(cardLayout);
        initComponents(username, user, cardLayout, cardPanel);

    }

    private void initComponents(String username, UserInput user, CardLayout cardLayout, JPanel cardPanel) {

        setLayout(new BorderLayout());
        setBackground(Color.WHITE);   // White main background

        // Top panel with username label and icon
        HeaderPanel header = new HeaderPanel(username);
        add(header, BorderLayout.NORTH);

        // Body panel using BoxLayout vertical axis
        JPanel bodyPanel = new JPanel();
        bodyPanel.setBackground(Color.WHITE);  // White background
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.add(Box.createVerticalStrut(20));

        JLabel jLabel1 = new JLabel("Welcome to FuSiON");
        jLabel1.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel1.setFont(new Font("Verdana", Font.BOLD, 38));
        jLabel1.setForeground(Color.BLACK);  // Black text

        bodyPanel.add(jLabel1);
        bodyPanel.add(Box.createVerticalStrut(30));

        JLabel jLabel3 = new JLabel("Empowering your discovery process with computational insights to ");
        jLabel3.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel3.setFont(new Font("Verdana", Font.BOLD, 18));
        jLabel3.setForeground(Color.BLACK);  // Black text

        bodyPanel.add(jLabel3);

        JLabel jLabel2 = new JLabel("spotlight functionally significant actors");
        jLabel2.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel2.setFont(new Font("Verdana", Font.BOLD, 18));
        jLabel2.setForeground(Color.BLACK);  // Black text

        bodyPanel.add(jLabel2);
        bodyPanel.add(Box.createVerticalStrut(50));

        JLabel jLabel5 = new JLabel("Kindly Select your working Directory");
        jLabel5.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel5.setFont(new Font("Segoe UI", Font.BOLD, 22));
        jLabel5.setForeground(Color.BLACK);  // Black text
        jLabel5.setHorizontalAlignment(SwingConstants.CENTER);

        bodyPanel.add(jLabel5);
        bodyPanel.add(Box.createVerticalStrut(20));

        // Show current directory panel
        JPanel showDirectory = new JPanel(new FlowLayout(FlowLayout.LEFT));
        showDirectory.setBackground(Color.WHITE);
        showDirectory.setPreferredSize(new Dimension(650, 60));
        showDirectory.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLACK));  // Black border

        JLabel jLabel6 = new JLabel("Current Directory :");
        jLabel6.setFont(new Font("Segoe UI", Font.PLAIN, 16));
        jLabel6.setForeground(Color.BLACK);

        showDirectory.add(jLabel6);

        selectedDirectory = new File(user.getWorkingDirectory());
        currDirectory = new JLabel(selectedDirectory.getAbsolutePath());
        currDirectory.setFont(new Font("Verdana", Font.BOLD, 14));
        currDirectory.setForeground(Color.BLACK);  // Black text

        showDirectory.add(currDirectory);

        bodyPanel.add(new JPanel(new FlowLayout()) {
            {
                add(showDirectory);
                setBackground(Color.WHITE);
            }
        });

        // Change Directory button with soft blue background and black text for better contrast
        changeDirButton = new RoundedButton("Change Directory", 20, new Dimension(300, 50));
        changeDirButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        changeDirButton.setBackground(new Color(86, 12, 112));  // soft blue
        changeDirButton.setForeground(Color.WHITE);  // black text for readability
        changeDirButton.setFocusPainted(false);

        changeDirButton.addActionListener(e -> changeDirectory(user));

        bodyPanel.add(new JPanel(new FlowLayout()) {
            {
                add(changeDirButton);
                setBackground(Color.WHITE);
            }
        });

        add(bodyPanel, BorderLayout.CENTER);

        
        RoundedButton nextButton = new RoundedButton("Next", 20, new Dimension(100, 40));
        nextButton.setBackground(new Color(100, 149, 237));
        nextButton.setForeground(Color.WHITE);
        nextButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        nextButton.setFocusPainted(false);

        RoundedButton goToSessions = new RoundedButton("Go to Sessions", 20, new Dimension(170, 40));
        goToSessions.setBackground(new Color(222, 129, 7));
        goToSessions.setForeground(Color.WHITE);
        goToSessions.setFont(new Font("Segoe UI", Font.BOLD, 18));
        goToSessions.setFocusPainted(false);

        RoundedButton prevButton = new RoundedButton("Prev", 20, new Dimension(100, 40));
        prevButton.setBackground(new Color(100, 149, 237));
        prevButton.setForeground(Color.WHITE);
        prevButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        prevButton.setFocusPainted(false);
        
        RoundedButton saveButton = new RoundedButton("Save", 20, new Dimension(100,40));
        saveButton.setBackground(new Color(5, 161, 59));
        saveButton.setForeground(Color.WHITE);
        saveButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        saveButton.setFocusPainted(false);

        

        

        // Button panel at bottom
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1,2));
        buttonPanel.setBackground(Color.WHITE);

        
       
        nextButton.addActionListener(e -> {
            fileSelectPanel = new FileSelectionCard(cardLayout, cardPanel, user);
            cardPanel.add(fileSelectPanel, "fileSelect");
            cardLayout.show(cardPanel, "fileSelect");
        });

        
        
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        
        
        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        saveButton.addActionListener(e -> user.saveData());

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(prevButton, BorderLayout.EAST);
                add(goToSessions, BorderLayout.WEST);
                setBackground(Color.WHITE);
                setOpaque(false);
            }
        });

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(nextButton, BorderLayout.WEST);
                add(saveButton, BorderLayout.EAST);
                setBackground(Color.WHITE);
                setOpaque(false);
            }
        });

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private void changeDirectory(UserInput user) {
        JFileChooser chooser = new JFileChooser(new File(System.getProperty("user.home")));
        chooser.setDialogTitle("Select Working Directory");
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        chooser.setAcceptAllFileFilterUsed(false);

        int result = chooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            selectedDirectory = chooser.getSelectedFile();

            currDirectory.setText(selectedDirectory.getAbsolutePath());
        }
        user.setWorkingDirectory(selectedDirectory.getAbsolutePath());
    }

    // public void setCurrentDirectory(String path) {
    //     currDirectory.setText(path);
    // }
    // public static void main(String[] args) {
    //     SwingUtilities.invokeLater(() -> new MainFrame("", null).setVisible(true));
    // }
}
