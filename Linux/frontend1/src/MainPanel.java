
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
        setBackground(Theme.BG);

        // Top panel with username label and icon
        HeaderPanel header = new HeaderPanel(username, cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        // Body panel using BoxLayout vertical axis
        JPanel bodyPanel = new JPanel();
        bodyPanel.setBackground(Theme.BG);
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.add(Box.createVerticalStrut(20));

        JLabel jLabel1 = new JLabel("Welcome to FuSION");
        jLabel1.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel1.setFont(Theme.title(34));
        jLabel1.setForeground(Theme.TEXT_DARK);

        bodyPanel.add(jLabel1);
        bodyPanel.add(Box.createVerticalStrut(10));

        JLabel jLabel3 = new JLabel("Empowering discovery with computational pathway insights");
        jLabel3.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel3.setFont(Theme.body(16));
        jLabel3.setForeground(Theme.TEXT_MED);

        bodyPanel.add(jLabel3);
        bodyPanel.add(Box.createVerticalStrut(40));

        JLabel jLabel5 = new JLabel("Select Working Directory");
        jLabel5.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel5.setFont(Theme.title(18));
        jLabel5.setForeground(Theme.TEXT_DARK);

        bodyPanel.add(jLabel5);
        bodyPanel.add(Box.createVerticalStrut(12));

        // Directory display card
        selectedDirectory = new File(user.getWorkingDirectory());
        currDirectory = new JLabel(selectedDirectory.getAbsolutePath());
        currDirectory.setFont(Theme.body(14));
        currDirectory.setForeground(Theme.PRIMARY);

        JPanel showDirectory = new JPanel(new BorderLayout(8, 0));
        showDirectory.setBackground(Theme.BG_CARD);
        showDirectory.setPreferredSize(new Dimension(650, 48));
        showDirectory.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(8, 14, 8, 14)));

        JLabel jLabel6 = new JLabel("Directory:");
        jLabel6.setFont(Theme.body(13));
        jLabel6.setForeground(Theme.TEXT_MED);

        showDirectory.add(jLabel6, BorderLayout.WEST);
        showDirectory.add(currDirectory, BorderLayout.CENTER);

        bodyPanel.add(new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0)) {
            {
                add(showDirectory);
                setBackground(Theme.BG);
            }
        });
        bodyPanel.add(Box.createVerticalStrut(16));

        changeDirButton = new RoundedButton("Change Directory", 20, new Dimension(220, 44));
        changeDirButton.setFont(Theme.title(15));
        changeDirButton.setBackground(Theme.PRIMARY);
        changeDirButton.setForeground(Color.WHITE);
        changeDirButton.setFocusPainted(false);

        changeDirButton.addActionListener(e -> changeDirectory(user));

        bodyPanel.add(new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0)) {
            {
                add(changeDirButton);
                setBackground(Theme.BG);
            }
        });

        add(bodyPanel, BorderLayout.CENTER);

        
        RoundedButton nextButton = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);

        

        

        // Button panel at bottom
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1,2));
        buttonPanel.setBackground(Theme.BG);

        
       
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
