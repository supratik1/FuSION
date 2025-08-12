import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

public class IdEntryPanel extends RoundedPanel {

    private JTextField inputField1, inputField2, inputField3;
    private JComboBox<String> suggestions1, suggestions2, suggestions3;
    private Map<String, String> idMap = new HashMap<>();
    private File idMappingFile;

    public IdEntryPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Color.WHITE);


        HeaderPanel header = new HeaderPanel(user.getUsername());
        add(header, BorderLayout.NORTH);

        

        String[] mappingFile = user.getMappingFile();

        // Main center panel with vertical stacking
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        center.setBackground(Color.WHITE);
        center.setOpaque(true);


        JLabel title = new JLabel("Add HSA IDs", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 30));
        title.setForeground(Color.BLACK);
        title.setBackground(Color.WHITE);
        title.setBorder(BorderFactory.createEmptyBorder(5,5,5,5));
        center.add(title);

// ---- First row: Source hsa ID ----
        JPanel row1 = new JPanel(new FlowLayout(FlowLayout.LEFT, 20, 5));
        row1.setBackground(Color.WHITE);
        JLabel label1 = new JLabel("Source hsa ID");
        label1.setForeground(Color.BLACK);

        inputField1 = new JTextField(15); // width in columns
        inputField1.setBackground(Color.WHITE);
        inputField1.setForeground(Color.BLACK);
        inputField1.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLACK));

        suggestions1 = new JComboBox<>();
        suggestions1.setPreferredSize(new Dimension(300, 30));

        row1.add(label1);
        row1.add(inputField1);
        row1.add(suggestions1);
        
        center.add(row1);

// ---- Second row: Target hsa ID ----
        JPanel row2 = new JPanel(new FlowLayout(FlowLayout.LEFT, 20, 5));
        row2.setBackground(Color.WHITE);
        JLabel label2 = new JLabel("Target hsa ID");
        label2.setForeground(Color.BLACK);

        inputField2 = new JTextField(15);
        inputField2.setBackground(Color.WHITE);
        inputField2.setForeground(Color.BLACK);
        inputField2.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLACK));

        suggestions2 = new JComboBox<>();
        suggestions2.setPreferredSize(new Dimension(300, 30));

        row2.add(label2);
        row2.add(inputField2);
        row2.add(suggestions2);
        center.add(row2);

// ---- Third row: Candidate hsa ID ----
        JPanel row3 = new JPanel(new FlowLayout(FlowLayout.LEFT, 20, 5));
        row3.setBackground(Color.WHITE);
        JLabel label3 = new JLabel("Candidate Implicated Node hsa ID");
        label3.setForeground(Color.BLACK);

        inputField3 = new JTextField(15);
        inputField3.setBackground(Color.WHITE);
        inputField3.setForeground(Color.BLACK);
        inputField3.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.BLACK));

        suggestions3 = new JComboBox<>();
        suggestions3.setPreferredSize(new Dimension(300, 30));

        row3.add(label3);
        row3.add(inputField3);
        row3.add(suggestions3);
        center.add(row3);

// ---- Fourth row: HSA not merged panel ----
        JPanel hsaNot = new JPanel(new FlowLayout(FlowLayout.LEFT, 20, 5));
        hsaNot.setBackground(Color.WHITE);
        JLabel hsaJLabel = new JLabel("HSA IDs not to be merged:");
        hsaJLabel.setForeground(Color.BLACK);

        JLabel ans = new JLabel();
        ans.setForeground(Color.BLACK);
        ans.setText("No File Selected");

        JButton hsaButton = new JButton("Choose");
        hsaButton.setBackground(new Color(100, 149, 237));
        hsaButton.setForeground(Color.WHITE);

        hsaNot.add(hsaJLabel);
        hsaNot.add(hsaButton);
        hsaNot.add(ans);
        center.add(hsaNot);

// ---- Fifth row: Mapping file change ----
        JPanel mappingPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 20, 5));
        mappingPanel.setBackground(Color.WHITE);
        JLabel ques = new JLabel("Would you like to change the Mapping Files?");
        ques.setForeground(Color.BLACK);
        JButton changeMapping = new JButton("Change Mapping File");
        changeMapping.setBackground(new Color(100, 149, 237));
        changeMapping.setForeground(Color.WHITE);
        mappingPanel.add(ques);
        mappingPanel.add(changeMapping);
        center.add(mappingPanel);

        add(center, BorderLayout.CENTER);


        

        // Listeners unchanged
        inputField1.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    if (performExactMatch(inputField1)) {
                        user.setSourceHsaID(inputField1.getText());
                    }
                }
            }
        });

        inputField2.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    if (performExactMatch(inputField2)) {
                        user.setTargetHsaID(inputField2.getText());
                    }
                }
            }
        });

        inputField3.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    if (performExactMatch(inputField3)) {
                        user.setCandidateID(inputField3.getText());
                    }
                }
            }
        });

        // --- Suggestions logic ---
        suggestions1.addActionListener(e -> {
            String selected = (String) suggestions1.getSelectedItem();
            if (suggestions1.isPopupVisible()) {
                if (selected != null) {
                    SwingUtilities.invokeLater(() -> inputField1.setText(selected));
                }
                user.setSourceHsaID(selected);
            }
        });
        suggestions2.addActionListener(e -> {
            String selected = (String) suggestions2.getSelectedItem();
            if (suggestions2.isPopupVisible()) {
                if (selected != null) {
                    SwingUtilities.invokeLater(() -> inputField2.setText(selected));
                }
                user.setTargetHsaID(selected);
            }
        });
        suggestions3.addActionListener(e -> {
            String selected = (String) suggestions3.getSelectedItem();
            if (suggestions3.isPopupVisible()) {
                if (selected != null) {
                    SwingUtilities.invokeLater(() -> inputField3.setText(selected));
                }
                user.setCandidateID(selected);
            }
        });

        


        

        hsaButton.addActionListener(e -> {
            JFileChooser chooser = new JFileChooser();
            int result = chooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File hsaNotmerged = chooser.getSelectedFile();
                user.sethsaNotFile(hsaNotmerged.getAbsolutePath());
                ans.setText(hsaNotmerged.getName());
            }
        });
        

        

        // Default load
        idMappingFile = new File(mappingFile[0]);
        if (idMappingFile.exists()) {
            loadMappingFile(idMappingFile);
        }
        inputField1.getDocument().addDocumentListener(new SuggestionUpdater(inputField1, suggestions1));
        inputField2.getDocument().addDocumentListener(new SuggestionUpdater(inputField2, suggestions2));
        inputField3.getDocument().addDocumentListener(new SuggestionUpdater(inputField3, suggestions3));

        changeMapping.addActionListener(e -> {

            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Change Mapping File", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(1150, 300);
            dialog.setLocationRelativeTo(null);

            JPanel panel = new JPanel(new GridLayout(1, 3, 15, 15));
            panel.setBorder(BorderFactory.createEmptyBorder(30, 10, 10, 10));
            panel.setBackground(Color.WHITE);

            JButton hsaToGeneBtn = new JButton("Choose");
            hsaToGeneBtn.setBackground(new Color(100,149,237));
            hsaToGeneBtn.setForeground(Color.WHITE);

            JButton hsaToPathBtn = new JButton("Choose");
            hsaToPathBtn.setBackground(new Color(100,149,237));
            hsaToPathBtn.setForeground(Color.WHITE);

            JButton interDBBtn = new JButton("Choose");
            interDBBtn.setBackground(new Color(100,149,237));
            interDBBtn.setForeground(Color.WHITE);

            JLabel geneField = new JLabel();
            geneField.setText(mappingFile[0]);
            JLabel pathField = new JLabel();
            pathField.setText(mappingFile[1]);
            JLabel dbField = new JLabel();
            dbField.setText(!mappingFile[2].equals("") ? mappingFile[2] : "No file selected");

            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Color.WHITE);
                add(new JLabel("HSA to Gene Symbol Map", SwingConstants.CENTER), BorderLayout.NORTH);
                add(geneField, BorderLayout.CENTER);
                add(hsaToGeneBtn, BorderLayout.SOUTH);
            }});
            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Color.WHITE);
                add(new JLabel("HSA path to Path Name Map", SwingConstants.CENTER), BorderLayout.NORTH);
                add(pathField, BorderLayout.CENTER);
                add(hsaToPathBtn, BorderLayout.SOUTH);
            }});
            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Color.WHITE);
                add(new JLabel("Cross Database Map", SwingConstants.CENTER), BorderLayout.NORTH);
                add(dbField, BorderLayout.CENTER);
                add(interDBBtn, BorderLayout.SOUTH);
            }});
            dialog.add(panel, BorderLayout.CENTER);

            // OK button at bottom
            JButton okButton = new JButton("OK");
            okButton.setBackground(new Color(100,149,237));
            okButton.setForeground(Color.WHITE);
            JPanel bottomPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
            bottomPanel.setBackground(Color.WHITE);
            bottomPanel.add(okButton);
            dialog.add(bottomPanel, BorderLayout.SOUTH);

            okButton.addActionListener(err -> {
                user.setMappingFile(mappingFile);
                dialog.dispose();
            });
            hsaToGeneBtn.addActionListener(err -> {
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(dialog);
                if (result == JFileChooser.APPROVE_OPTION) {
                    idMappingFile = chooser.getSelectedFile();
                    mappingFile[0] = idMappingFile.getAbsolutePath();
                    loadMappingFile(idMappingFile);
                    geneField.setText(mappingFile[0]);
                }
            });
            hsaToPathBtn.addActionListener(err -> {
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(dialog);
                if (result == JFileChooser.APPROVE_OPTION) {
                    File file = chooser.getSelectedFile();
                    mappingFile[1] = file.getAbsolutePath();
                    pathField.setText(mappingFile[1]);
                }
            });
            interDBBtn.addActionListener(err -> {
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(dialog);
                if (result == JFileChooser.APPROVE_OPTION) {
                    File file = chooser.getSelectedFile();
                    mappingFile[2] = file.getAbsolutePath();
                    dbField.setText(mappingFile[2]);
                }
            });

            dialog.setVisible(true);
        });

        RoundedButton nextButton = new RoundedButton("Submit", 20, new Dimension(100, 40));
        
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

        
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "edgeEntry"));
        saveButton.addActionListener(e -> {
            user.setMappingFile(mappingFile);
            user.saveData();
        });

        
        nextButton.addActionListener(e -> {
            user.setMappingFile(mappingFile);
            try {
                new ScriptFile(user);
            } catch (Exception et) {
                et.printStackTrace();
            }
        });

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        JPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Color.WHITE);

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setOpaque(false);
            setBackground(Color.WHITE);
        }});
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setOpaque(false);
            setBackground(Color.WHITE);
        }});
        add(buttonPanel, BorderLayout.SOUTH);
    }

    private boolean performExactMatch(JTextField field) {
        String input = field.getText().trim().toLowerCase();
        for (Map.Entry<String, String> entry : idMap.entrySet()) {
            String commonName = entry.getKey().toLowerCase();
            String id = entry.getValue().toLowerCase();
            if (input.equals(commonName) || input.equals(id)) {
                String display = entry.getValue() + " - " + entry.getKey();
                field.setText(display);
                return true;
            }
        }
        JOptionPane.showMessageDialog(field, "No exact match found.");
        return true;
    }

    private void loadMappingFile(File file) {
        idMap.clear();
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                String[] parts = line.split("\\s+");
                if (parts.length >= 2) {
                    String id = parts[0].trim();
                    String commonName = String.join(" ", Arrays.copyOfRange(parts, 1, parts.length)).toLowerCase();
                    idMap.put(commonName, id);
                }
            }
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(this, "Error reading mapping file: " + ex.getMessage());
        }
    }

    // Suggestion updater unchanged except handles styling within input/suggestion:
    private class SuggestionUpdater implements DocumentListener {

        private final JTextField field;
        private final JComboBox<String> box;

        SuggestionUpdater(JTextField field, JComboBox<String> box) {
            this.field = field;
            this.box = box;
        }

        @Override
        public void insertUpdate(DocumentEvent e) {
            updateSuggestions();
        }

        @Override
        public void removeUpdate(DocumentEvent e) {
            updateSuggestions();
        }

        @Override
        public void changedUpdate(DocumentEvent e) {}

        private void updateSuggestions() {
            String input = field.getText().toLowerCase();
            box.removeAllItems();
            if (input.isEmpty()) return;
            List<String> exactMatches = new ArrayList<>();
            List<String> partialMatches = new ArrayList<>();
            for (Map.Entry<String, String> entry : idMap.entrySet()) {
                String commonName = entry.getKey();
                String id = entry.getValue().toLowerCase();
                String display = entry.getValue() + " - " + commonName;
                if (commonName.equals(input) || id.equals(input)) {
                    exactMatches.add(display);
                }
                else if (commonName.contains(input) || id.contains(input)) {
                    partialMatches.add(display);
                }
            }
            for (String match : exactMatches) box.addItem(match);
            for (String match : partialMatches) box.addItem(match);
        }
    }
}
