import java.awt.*;
import java.io.*;
import javax.swing.*;

public class EdgesChooserPanel extends RoundedPanel {

    private JLabel[] fileLabels = new JLabel[5];
    private String[] edges;

    public EdgesChooserPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Color.WHITE);

        edges = user.getEdgesFile();

        HeaderPanel header = new HeaderPanel(user.getUsername());
        add(header, BorderLayout.NORTH);

        
        JLabel title = new JLabel("Add Edge files", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 32));
        title.setForeground(Color.BLACK);
        title.setBackground(Color.WHITE);
        

        // Main panel with file rows
        JPanel panel = new JPanel();
        panel.setLayout(new GridLayout(3, 2, 10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(20, 20, 10, 20));
        panel.setBackground(Color.WHITE);

        panel.add(createFileRow("Additional Edges File", 0));
        panel.add(createFileRow("Essential Edge File", 1));
        panel.add(createFileRow("Avoid Edge File", 2));
        panel.add(createFileRow("Relaxed Edges File", 3));
        panel.add(createFileRow("Non Relaxed Edges File", 4));

        add(new JPanel(){{
            add(title);
            add(panel);
            setOpaque(true);
            setBackground(Color.WHITE);
        }}, BorderLayout.CENTER);

        // Buttons
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

        // Bottom button panel
        JPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Color.WHITE);

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setBackground(Color.WHITE);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setBackground(Color.WHITE);
            setOpaque(false);
        }});

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        saveButton.addActionListener(e -> {
            user.setEdgesFile(edges);
            user.saveData();
        });

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "threshold"));

        nextButton.addActionListener(e -> {
            user.setEdgesFile(edges);
            IdEntryPanel idEntry = new IdEntryPanel(cardLayout, cardPanel, user);
            cardPanel.add(idEntry, "idEntry");
            cardLayout.show(cardPanel, "idEntry");
        });

        add(buttonPanel, BorderLayout.SOUTH);
    }

    // Creates a file selection row
    private JPanel createFileRow(String title, int index) {
        JPanel rowPanel = new JPanel();
        rowPanel.setBackground(Color.WHITE);

        JLabel titleLabel = new JLabel(title);
        titleLabel.setForeground(Color.BLACK);
        titleLabel.setFont(new Font("Verdana", Font.BOLD, 18));
        rowPanel.add(titleLabel);

        fileLabels[index] = new JLabel();
        fileLabels[index].setText(edges[index].equals("") ? "No file selected" : edges[index]);
        fileLabels[index].setForeground(Color.BLACK);

        JButton chooseButton = new JButton("Choose File");
        styleButton(chooseButton);

        chooseButton.addActionListener(e -> {
            JFileChooser chooser = new JFileChooser();
            int result = chooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File selectedFile = chooser.getSelectedFile();
                fileLabels[index].setText(selectedFile.getName());
                edges[index] = selectedFile.getAbsolutePath();
            }
        });

       
        JPanel rightPanel = new JPanel(new BorderLayout(5, 5));
        rightPanel.setBackground(Color.WHITE);

        rightPanel.add(new JPanel(){{
            add(chooseButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        rightPanel.add(fileLabels[index], BorderLayout.SOUTH);

        
        rowPanel.add(rightPanel);
        return rowPanel;
        
    }

    // Styles a button in our new theme
    private void styleButton(JButton button) {
        button.setBackground(new Color(100, 149, 237)); // soft blue
        button.setForeground(Color.WHITE);
        button.setFocusPainted(false);
        button.setFont(new Font("Segoe UI", Font.BOLD, 14));
    }
}
