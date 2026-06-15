import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import javax.swing.*;



public class FileSelectionCard extends RoundedPanel {

    private JLabel selectedFileLabel;

    public FileSelectionCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {

        setLayout(new BorderLayout());
        setBackground(Theme.BG);
        // setBorder(BorderFactory.createEmptyBorder(40, 40, 40, 40));

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);

        add(header, BorderLayout.NORTH);


        JPanel body = new JPanel();
        body.setBorder(BorderFactory.createEmptyBorder(40, 60, 40, 60));
        body.setLayout(new BoxLayout(body, BoxLayout.Y_AXIS));
        body.setBackground(Theme.BG);

        JLabel jLabel1 = new JLabel("Log Fold Changes File");
        jLabel1.setFont(Theme.title(26));
        jLabel1.setForeground(Theme.TEXT_DARK);
        jLabel1.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(jLabel1);

        JLabel jLabel1sub = new JLabel("Select the TSV file containing expression fold-change data");
        jLabel1sub.setFont(Theme.body(14));
        jLabel1sub.setForeground(Theme.TEXT_MED);
        jLabel1sub.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(Box.createVerticalStrut(6));
        body.add(jLabel1sub);
        body.add(Box.createVerticalStrut(40));

        RoundedButton nextButton = Theme.navBtn("Next →", 120);
        nextButton.setEnabled(false);

        // File display card
        selectedFileLabel = new JLabel();
        if (user.getLogFoldChangesFile() != null && !"".equals(user.getLogFoldChangesFile())) {
            selectedFileLabel.setText(user.getLogFoldChangesFile());
            nextButton.setEnabled(true);
        } else {
            selectedFileLabel.setText("No file selected");
        }
        selectedFileLabel.setFont(Theme.body(14));
        selectedFileLabel.setForeground(Theme.TEXT_MED);

        JPanel fileCard = new JPanel(new BorderLayout(12, 0));
        fileCard.setBackground(Theme.BG_CARD);
        fileCard.setMaximumSize(new Dimension(600, 52));
        fileCard.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));

        JLabel fileIcon = new JLabel("📄");
        fileIcon.setFont(new Font("Segoe UI Emoji", Font.PLAIN, 18));
        fileCard.add(fileIcon, BorderLayout.WEST);
        fileCard.add(selectedFileLabel, BorderLayout.CENTER);
        fileCard.setAlignmentX(Component.CENTER_ALIGNMENT);
        body.add(fileCard);
        body.add(Box.createVerticalStrut(16));

        RoundedButton chooseButton = Theme.navBtn("Choose File", 180);

        JPanel choosePanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
        choosePanel.add(chooseButton);
        choosePanel.setOpaque(false);
        body.add(choosePanel);

        add(body, BorderLayout.CENTER);

        chooseButton.addActionListener((ActionEvent e) -> {
            JFileChooser fileChooser = new JFileChooser(user.getWorkingDirectory());
            int result = fileChooser.showOpenDialog(FileSelectionCard.this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File selectedFile = fileChooser.getSelectedFile();
                selectedFileLabel.setText("Selected: " + selectedFile.getName());
                user.setLogFoldChangesFile(selectedFile.getAbsolutePath());
                nextButton.setEnabled(true);
            }
        });

        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);

        nextButton.addActionListener((ActionEvent evt) -> {
            XMLSelection xmlFile = new XMLSelection(cardLayout, cardPanel, user);
            cardPanel.add(xmlFile, "xml");
            cardLayout.show(cardPanel, "xml");
        });

        prevButton.addActionListener(e -> {
            cardLayout.show(cardPanel, "Main");
        });

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(prevButton, BorderLayout.EAST);
                add(goToSessions, BorderLayout.WEST);
                setOpaque(false);
            }
        });

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(nextButton, BorderLayout.WEST);
                add(saveButton, BorderLayout.EAST);
                setOpaque(false);
            }
        });

        add(buttonPanel, BorderLayout.SOUTH);

        saveButton.addActionListener(e -> {
            user.saveData();
        });

        goToSessions.addActionListener(e -> {
            cardLayout.show(cardPanel, "sessions");
        });

    }
}
