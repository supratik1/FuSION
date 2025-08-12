import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import javax.swing.*;



public class FileSelectionCard extends RoundedPanel {

    private JLabel selectedFileLabel;

    public FileSelectionCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {

        setLayout(new BorderLayout());
        setBackground(Color.WHITE);
        // setBorder(BorderFactory.createEmptyBorder(40, 40, 40, 40));

        HeaderPanel header = new HeaderPanel(user.getUsername());

        add(header, BorderLayout.NORTH);


        JPanel top = new JPanel();
        top.setBackground(Color.WHITE);
        
        JLabel jLabel1 = new JLabel("Select Log Fold Changes File");
        jLabel1.setFont(new Font("Verdana", Font.BOLD, 34));
        jLabel1.setForeground(Color.BLACK);

        top.add(jLabel1);
        

        JPanel body = new JPanel();
        body.setBorder(BorderFactory.createEmptyBorder(40,40,40,40));
        body.setLayout(new BoxLayout(body, BoxLayout.Y_AXIS));
        body.setBackground(Color.WHITE);
        body.add(top);
        body.add(Box.createVerticalStrut(100));

        JPanel jPanel1 = new JPanel();
        jPanel1.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLACK));
        jPanel1.setOpaque(false);
        jPanel1.setPreferredSize(new Dimension(500, 40));

        RoundedButton nextButton = new RoundedButton("Next", 20, new Dimension(100, 40));
        nextButton.setEnabled(false);
        nextButton.setBackground(new Color(100, 149, 237));
        nextButton.setForeground(Color.WHITE);
        nextButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        nextButton.setFocusPainted(false);

        selectedFileLabel = new JLabel();
        if (user.getLogFoldChangesFile() != null && !"".equals(user.getLogFoldChangesFile())) {
            selectedFileLabel.setText(user.getLogFoldChangesFile());
            nextButton.setEnabled(true);
        } else {
            selectedFileLabel.setText("No File Selected");
        }
        selectedFileLabel.setForeground(Color.BLACK);
        jPanel1.add(selectedFileLabel);

        JPanel jp = new JPanel();
        jp.add(jPanel1);
        jp.setOpaque(false);

        body.add(jp);

        RoundedButton chooseButton = new RoundedButton("Choose File", 20, new Dimension(200, 50));
        chooseButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        chooseButton.setBackground(new Color(100, 149, 237)); // cornflower blue
        chooseButton.setForeground(Color.WHITE);
        chooseButton.setFocusPainted(false);

        JPanel choosePanel = new JPanel();
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
        buttonPanel.setBackground(Color.WHITE);

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
