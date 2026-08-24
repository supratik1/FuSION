import java.awt.*;
import java.io.File;
import javax.swing.*;

public class MainPanel extends RoundedPanel {

    private JLabel currDirectory;
    private RoundedButton changeDirButton;

    private JPanel fileSelectPanel;
    protected File selectedDirectory;

    public MainPanel(String username, UserInput user, CardLayout cardLayout, JPanel cardPanel) {
        initComponents(username, user, cardLayout, cardPanel);
    }

    private void initComponents(String username, UserInput user, CardLayout cardLayout, JPanel cardPanel) {

        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(username, cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        // Body — vertical stack centred in the page
        JPanel bodyPanel = new JPanel();
        bodyPanel.setBackground(Theme.BG);
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.add(Box.createVerticalStrut(Theme.scale(20)));

        JLabel jLabel1 = new JLabel("Welcome to FuSION");
        jLabel1.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel1.setFont(Theme.title(34));
        jLabel1.setForeground(Theme.TEXT_DARK);
        bodyPanel.add(jLabel1);
        bodyPanel.add(Box.createVerticalStrut(Theme.scale(10)));

        JLabel jLabel3 = new JLabel("Empowering discovery with computational pathway insights");
        jLabel3.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel3.setFont(Theme.body(16));
        jLabel3.setForeground(Theme.TEXT_MED);
        bodyPanel.add(jLabel3);
        bodyPanel.add(Box.createVerticalStrut(Theme.scale(40)));

        JLabel jLabel5 = new JLabel("Select Working Directory");
        jLabel5.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel5.setFont(Theme.title(18));
        jLabel5.setForeground(Theme.TEXT_DARK);
        bodyPanel.add(jLabel5);
        bodyPanel.add(Box.createVerticalStrut(Theme.scale(12)));

        // Directory display card — responsive: fills available width up to a max
        selectedDirectory = new File(user.getWorkingDirectory());
        currDirectory = new JLabel(selectedDirectory.getAbsolutePath());
        currDirectory.setFont(Theme.body(14));
        currDirectory.setForeground(Theme.PRIMARY);

        JPanel showDirectory = new JPanel(new BorderLayout(Theme.GAP_SM, 0));
        showDirectory.setBackground(Theme.BG_CARD);
        showDirectory.setMaximumSize(new Dimension(Theme.scale(700), Theme.scale(52)));
        showDirectory.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(8), Theme.scale(14), Theme.scale(8), Theme.scale(14))));

        JLabel jLabel6 = new JLabel("Directory:");
        jLabel6.setFont(Theme.body(13));
        jLabel6.setForeground(Theme.TEXT_MED);

        showDirectory.add(jLabel6, BorderLayout.WEST);
        showDirectory.add(currDirectory, BorderLayout.CENTER);
        showDirectory.setAlignmentX(Component.CENTER_ALIGNMENT);
        bodyPanel.add(showDirectory);
        bodyPanel.add(Box.createVerticalStrut(Theme.scale(16)));

        changeDirButton = new RoundedButton("Change Directory", Theme.RADIUS_LG,
            new Dimension(Theme.scale(220), Theme.scale(44)));
        changeDirButton.setFont(Theme.title(15));
        changeDirButton.setBackground(Theme.PRIMARY);
        changeDirButton.setForeground(Color.WHITE);
        changeDirButton.setFocusPainted(false);
        changeDirButton.setAlignmentX(Component.CENTER_ALIGNMENT);
        changeDirButton.addActionListener(e -> changeDirectory(user));
        bodyPanel.add(changeDirButton);

        add(bodyPanel, BorderLayout.CENTER);

        RoundedButton nextButton   = Theme.navBtn("Next »", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton   = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton   = Theme.successBtn("Save", 110);

        nextButton.addActionListener(e -> {
            fileSelectPanel = new FileSelectionCard(cardLayout, cardPanel, user);
            cardPanel.add(fileSelectPanel, "fileSelect");
            cardLayout.show(cardPanel, "fileSelect");
        });
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));
        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));
        saveButton.addActionListener(e -> user.saveData());

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private void changeDirectory(UserInput user) {
        String wd = user.getWorkingDirectory();
        File start = (wd != null && !wd.isBlank()) ? new File(wd) : new File(System.getProperty("user.home"));
        JFileChooser chooser = new JFileChooser(start);
        chooser.setDialogTitle("Select Working Directory");
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        chooser.setAcceptAllFileFilterUsed(false);

        int result = chooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            selectedDirectory = chooser.getSelectedFile();
            currDirectory.setText(selectedDirectory.getAbsolutePath());
            user.setWorkingDirectory(selectedDirectory.getAbsolutePath());
        }
    }
}
