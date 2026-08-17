import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

public class IdEntryPanel extends RoundedPanel {

    private JTextField inputField1, inputField2, inputField3;
    private JComboBox<String> suggestions1, suggestions2, suggestions3;
    Map<String, List<String>> idMap = new HashMap<>();

    public IdEntryPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        String[] mappingFile = user.getMappingFile();

        // Main center panel with vertical stacking
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        center.setBackground(Theme.BG);
        center.setOpaque(true);

        center.setBorder(BorderFactory.createEmptyBorder(20, 50, 20, 50));

        JLabel title = new JLabel("Gene IDs");
        title.setFont(Theme.title(26));
        title.setForeground(Theme.TEXT_DARK);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        center.add(title);
        center.add(Box.createVerticalStrut(4));
        JLabel titleSub = new JLabel("Enter starting gene, ending gene, and gene under study");
        titleSub.setFont(Theme.body(14));
        titleSub.setForeground(Theme.TEXT_MED);
        titleSub.setAlignmentX(Component.CENTER_ALIGNMENT);
        center.add(titleSub);
        center.add(Box.createVerticalStrut(24));

        // ---- First row: Source hsa ID ----
        JLabel label1 = new JLabel("Starting Gene");
        label1.setFont(Theme.title(13));
        label1.setForeground(Theme.TEXT_DARK);

        inputField1 = new JTextField(18);
        inputField1.setBackground(new Color(243, 247, 255));
        inputField1.setForeground(new Color(15, 23, 42));
        inputField1.setCaretColor(Theme.PRIMARY);
        inputField1.setFont(Theme.body(14));

        suggestions1 = new JComboBox<>();
        suggestions1.setPreferredSize(new Dimension(280, 30));

        JPanel row1 = createIdRow(label1, inputField1, suggestions1);
        center.add(row1);
        center.add(Box.createVerticalStrut(12));

        // ---- Second row: Target hsa ID ----
        JLabel label2 = new JLabel("Ending Gene");
        label2.setFont(Theme.title(13));
        label2.setForeground(Theme.TEXT_DARK);

        inputField2 = new JTextField(18);
        inputField2.setBackground(new Color(243, 247, 255));
        inputField2.setForeground(new Color(15, 23, 42));
        inputField2.setCaretColor(Theme.PRIMARY);
        inputField2.setFont(Theme.body(14));

        suggestions2 = new JComboBox<>();
        suggestions2.setPreferredSize(new Dimension(280, 30));

        JPanel row2 = createIdRow(label2, inputField2, suggestions2);
        center.add(row2);
        center.add(Box.createVerticalStrut(12));

        // ---- Third row: Candidate hsa ID ----
        JLabel label3 = new JLabel("Gene Under Study (FSC Test)");
        label3.setFont(Theme.title(13));
        label3.setForeground(Theme.TEXT_DARK);

        inputField3 = new JTextField(18);
        inputField3.setBackground(new Color(243, 247, 255));
        inputField3.setForeground(new Color(15, 23, 42));
        inputField3.setCaretColor(Theme.PRIMARY);
        inputField3.setFont(Theme.body(14));

        suggestions3 = new JComboBox<>();
        suggestions3.setPreferredSize(new Dimension(280, 30));

        JPanel row3 = createIdRow(label3, inputField3, suggestions3);
        center.add(row3);
        center.add(Box.createVerticalStrut(16));

        // ---- Fourth row: HSA not merged panel ----
        JPanel hsaNot = new JPanel(new BorderLayout(12, 0));
        hsaNot.setBackground(Theme.BG_CARD);
        hsaNot.setMaximumSize(new Dimension(Integer.MAX_VALUE, 54));
        hsaNot.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));

        JPanel hsaInfo = new JPanel();
        hsaInfo.setLayout(new BoxLayout(hsaInfo, BoxLayout.Y_AXIS));
        hsaInfo.setOpaque(false);
        JLabel hsaJLabel = new JLabel("Genes to Keep Separate");
        hsaJLabel.setFont(Theme.title(13));
        hsaJLabel.setForeground(Theme.TEXT_DARK);
        JLabel ans = new JLabel("No File Selected");
        ans.setFont(Theme.body(12));
        ans.setForeground(Theme.TEXT_MED);
        hsaInfo.add(hsaJLabel);
        hsaInfo.add(ans);

        RoundedButton hsaButton = Theme.navBtn("Choose", 110);

        hsaNot.add(hsaInfo, BorderLayout.CENTER);
        hsaNot.add(hsaButton, BorderLayout.EAST);
        center.add(hsaNot);
        center.add(Box.createVerticalStrut(10));

        // ---- Fifth row: Mapping file change ----
        JPanel mappingPanel = new JPanel(new BorderLayout(12, 0));
        mappingPanel.setBackground(Theme.BG_CARD);
        mappingPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 48));
        mappingPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(8, 16, 8, 16)));
        JLabel ques = new JLabel("Mapping files");
        ques.setFont(Theme.title(13));
        ques.setForeground(Theme.TEXT_DARK);
        RoundedButton changeMapping = Theme.navBtn("Change Mapping Files", 200);
        mappingPanel.add(ques, BorderLayout.CENTER);
        mappingPanel.add(changeMapping, BorderLayout.EAST);
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

        // Load gene IDs from the user's uploaded fold change file
        loadGeneIdsFromFoldChangeFile(user.getLogFoldChangesFile());
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
            panel.setBackground(Theme.BG_CARD);

            RoundedButton hsaToGeneBtn = new RoundedButton("Choose", 14, new Dimension(110, 30));
            hsaToGeneBtn.setBackground(Theme.PRIMARY);
            hsaToGeneBtn.setForeground(Color.WHITE);
            hsaToGeneBtn.setFont(new Font("Segoe UI", Font.BOLD, 13));

            RoundedButton hsaToPathBtn = new RoundedButton("Choose", 14, new Dimension(110, 30));
            hsaToPathBtn.setBackground(Theme.PRIMARY);
            hsaToPathBtn.setForeground(Color.WHITE);
            hsaToPathBtn.setFont(new Font("Segoe UI", Font.BOLD, 13));

            RoundedButton interDBBtn = new RoundedButton("Choose", 14, new Dimension(110, 30));
            interDBBtn.setBackground(Theme.PRIMARY);
            interDBBtn.setForeground(Color.WHITE);
            interDBBtn.setFont(new Font("Segoe UI", Font.BOLD, 13));

            JLabel geneField = new JLabel();
            geneField.setText(mappingFile[0]);
            JLabel pathField = new JLabel();
            pathField.setText(mappingFile[1]);
            JLabel dbField = new JLabel();
            dbField.setText(!mappingFile[2].equals("") ? mappingFile[2] : "No file selected");

            panel.add(new JPanel(new BorderLayout()) {
                {
                    setBackground(Theme.BG_CARD);
                    add(new JLabel("HSA to Gene Symbol Map", SwingConstants.CENTER), BorderLayout.NORTH);
                    add(geneField, BorderLayout.CENTER);
                    add(hsaToGeneBtn, BorderLayout.SOUTH);
                }
            });
            panel.add(new JPanel(new BorderLayout()) {
                {
                    setBackground(Theme.BG_CARD);
                    add(new JLabel("HSA path to Path Name Map", SwingConstants.CENTER), BorderLayout.NORTH);
                    add(pathField, BorderLayout.CENTER);
                    add(hsaToPathBtn, BorderLayout.SOUTH);
                }
            });
            panel.add(new JPanel(new BorderLayout()) {
                {
                    setBackground(Theme.BG_CARD);
                    add(new JLabel("Cross Database Map", SwingConstants.CENTER), BorderLayout.NORTH);
                    add(dbField, BorderLayout.CENTER);
                    add(interDBBtn, BorderLayout.SOUTH);
                }
            });
            dialog.add(panel, BorderLayout.CENTER);

            // OK button at bottom
            RoundedButton okButton = new RoundedButton("OK", 14, new Dimension(90, 32));
            okButton.setBackground(Theme.PRIMARY);
            okButton.setForeground(Color.WHITE);
            okButton.setFont(new Font("Segoe UI", Font.BOLD, 14));
            JPanel bottomPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
            bottomPanel.setBackground(Theme.BG);
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
                    mappingFile[0] = chooser.getSelectedFile().getAbsolutePath();
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

        RoundedButton nextButton = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "relaxationPanel"));
        saveButton.addActionListener(e -> {
            user.setMappingFile(mappingFile);
            user.saveData();
        });

        nextButton.addActionListener(e -> {
            user.setMappingFile(mappingFile);
            ThresholdFilterPanel threshold = new ThresholdFilterPanel(cardLayout, cardPanel, user);
            cardPanel.add(threshold, "threshold");
            cardLayout.show(cardPanel, "threshold");
        });

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        JPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(prevButton, BorderLayout.EAST);
                add(goToSessions, BorderLayout.WEST);
                setOpaque(false);
                setBackground(Theme.BG);
            }
        });
        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(nextButton, BorderLayout.WEST);
                add(saveButton, BorderLayout.EAST);
                setOpaque(false);
                setBackground(Theme.BG);
            }
        });
        add(buttonPanel, BorderLayout.SOUTH);
    }

    private JPanel createIdRow(JLabel label, JTextField field, JComboBox<String> combo) {
        // Border styles for normal and focused states
        javax.swing.border.Border normalBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(100, 140, 200), 1),
            BorderFactory.createEmptyBorder(7, 10, 7, 10));
        javax.swing.border.Border focusBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.PRIMARY, 2),
            BorderFactory.createEmptyBorder(6, 9, 6, 9));

        field.setBorder(normalBorder);
        field.setOpaque(true);
        field.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent e) { field.setBorder(focusBorder); }
            public void focusLost(java.awt.event.FocusEvent e)   { field.setBorder(normalBorder); }
        });

        JPanel card = new JPanel(new BorderLayout(10, 4));
        card.setBackground(Theme.BG_CARD);
        card.setMaximumSize(new Dimension(Integer.MAX_VALUE, 78));
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));
        JPanel left = new JPanel();
        left.setLayout(new BoxLayout(left, BoxLayout.Y_AXIS));
        left.setOpaque(false);
        label.setAlignmentX(Component.LEFT_ALIGNMENT);
        field.setAlignmentX(Component.LEFT_ALIGNMENT);
        field.setMaximumSize(new Dimension(240, 38));
        left.add(label);
        left.add(Box.createVerticalStrut(5));
        left.add(field);
        card.add(left, BorderLayout.CENTER);
        card.add(combo, BorderLayout.EAST);
        return card;
    }

    private boolean performExactMatch(JTextField field) {
        String input = field.getText().trim().toLowerCase();
        for (Map.Entry<String, List<String>> entry : idMap.entrySet()) {
            for (String id : entry.getValue()) {
                if (input.equals(entry.getKey()) || input.equalsIgnoreCase(id)) {
                    field.setText(id);
                    return true;
                }
            }
        }
        JOptionPane.showMessageDialog(field, "No exact match found.");
        return true;
    }

    /** Populates idMap from the user's gene activity data (fold change) file. */
    private void loadGeneIdsFromFoldChangeFile(String filePath) {
        idMap.clear();
        if (filePath == null || filePath.isBlank()) return;
        File file = new File(filePath.trim());
        if (!file.exists()) return;
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) continue;
                String[] parts = line.split("\\s+");
                if (parts.length >= 1) {
                    String id = parts[0].trim();
                    if (!id.isEmpty())
                        idMap.computeIfAbsent(id.toLowerCase(), k -> new ArrayList<>()).add(id);
                }
            }
        } catch (IOException ignored) {}
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
        public void changedUpdate(DocumentEvent e) {
        }


        private void updateSuggestions() {
            String input = field.getText().toLowerCase().trim();
            box.removeAllItems();
            if (input.isEmpty()) return;
            List<String> exactMatches   = new ArrayList<>();
            List<String> partialMatches = new ArrayList<>();
            for (Map.Entry<String, List<String>> entry : idMap.entrySet()) {
                String key = entry.getKey();          // lowercase gene id
                for (String id : entry.getValue()) {  // original-case gene id
                    if (key.equals(input))
                        exactMatches.add(id);
                    else if (key.contains(input))
                        partialMatches.add(id);
                }
            }
            for (String m : exactMatches)   box.addItem(m);
            for (String m : partialMatches) box.addItem(m);
        }
    }

    
    private static final DateTimeFormatter TIME_FMT =
            DateTimeFormatter.ofPattern("HH:mm:ss");

    Icon runningIcon = createDotIcon(new Color(0, 180, 0));   // green
    Icon idleIcon    = createDotIcon(new Color(100, 100, 100));
    Icon errorIcon   = createDotIcon(new Color(200, 50, 50));


    private void runTool(String username) {

        JTextArea terminal = new JTextArea();
        terminal.setEditable(false);

        JLabel stageLabel = new JLabel("Initializing…");
        JLabel statusLabel = new JLabel("Request under process...");

        stageLabel.setIcon(runningIcon);
        statusLabel.setIcon(runningIcon);

        JFrame frame = createModernOutputWindow(
                terminal, stageLabel, statusLabel
        );

        frame.setVisible(true);


        new Thread(() -> {
            try {
                ProcessBuilder pb =
                        new ProcessBuilder("./fusion", "-b", "output_script.txt");
                pb.redirectErrorStream(true);
                Process process = pb.start();

                BufferedReader reader =
                        new BufferedReader(new InputStreamReader(process.getInputStream()));

                String line;
                
                while ((line = reader.readLine()) != null) {

                    final String lineCopy = line;   
                    final String timestamp =
                            "[" + LocalTime.now().format(TIME_FMT) + "] ";

                    final String stage = detectStage(lineCopy);

                    SwingUtilities.invokeLater(() -> {
                        terminal.append(timestamp + lineCopy + "\n");
                        terminal.setCaretPosition(
                                terminal.getDocument().getLength()
                        );

                        if (stage != null) {
                            stageLabel.setText(stage);
                            frame.setTitle("FuSION - " + stage);
                            statusLabel.setText( stage + "...");

                            stageLabel.setIcon(runningIcon);
                            

                            statusLabel.setIcon(runningIcon);
                            

                        }

                    });
                }


                process.waitFor();

                SwingUtilities.invokeLater(() -> {
                    stageLabel.setText(" Execution Completed");
                    statusLabel.setText("✔ Done");
                    stageLabel.setIcon(idleIcon);
                    statusLabel.setIcon(idleIcon);

                    frame.setTitle("FuSION - Completed");


                    int choice = JOptionPane.showConfirmDialog(
                            frame,
                            "Do you want to run another experiment?",
                            "Run Another?",
                            JOptionPane.YES_NO_OPTION
                    );

                    if (choice == JOptionPane.YES_OPTION) {
                        new SessionFrame(username);
                    }
                });

            } catch (Exception e) {
                stageLabel.setIcon(errorIcon);
                stageLabel.setText("Execution Failed");

                SwingUtilities.invokeLater(() ->
                        JOptionPane.showMessageDialog(frame,
                                e.getMessage(),
                                "Execution Error",
                                JOptionPane.ERROR_MESSAGE));
            }
        }).start();
    }

    /**
     * Maps backend output lines to execution stages
     */
    private String detectStage(String line) {

        line = line.trim();

        if (line.startsWith("exec mkdir")) return "Initializing workspace";
        if (line.startsWith("cudf")) return "Processing fold-change data";
        if (line.startsWith("start")) return "Loading mappings & metadata";
        if (line.startsWith("mff")) return "Merging pathways";
        if (line.startsWith("wgx")) return "Writing merged graph";
        if (line.startsWith("rgx")) return "Reading graph XML";
        if (line.startsWith("fb_rch")) return "Reachability pruning";
        if (line.startsWith("pathz3")) return "Constraint solving (Z3)";
        if (line.contains("gnuplot")) return "Generating plots";
        if (line.equals("exit")) return "Finalizing";

        return null; // no title change
    }

    private JFrame createModernOutputWindow(
        JTextArea terminal,
        JLabel stageLabel,
        JLabel statusLabel
    ) {
        JFrame frame = new JFrame("FuSION - Initializing");
        frame.setSize(1100, 550);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        // ---- Header ----
        JPanel header = new JPanel(new BorderLayout());
        header.setBackground(new Color(30, 30, 30));
        header.setBorder(BorderFactory.createEmptyBorder(8, 12, 8, 12));

        stageLabel.setForeground(Color.WHITE);
        stageLabel.setFont(new Font("Segoe UI", Font.BOLD, 15));

        header.add(stageLabel, BorderLayout.WEST);

        // ---- Terminal ----
        terminal.setBackground(new Color(18, 18, 18));
        terminal.setForeground(new Color(220, 220, 220));
        terminal.setCaretColor(Color.WHITE);
        terminal.setFont(new Font("JetBrains Mono", Font.PLAIN, 13));
        terminal.setBorder(BorderFactory.createEmptyBorder(8, 8, 8, 8));

        JScrollPane scrollPane = new JScrollPane(terminal);
        scrollPane.setBorder(null);

        // ---- Status Bar ----
        JPanel statusBar = new JPanel(new BorderLayout());
        statusBar.setBackground(new Color(30, 30, 30));
        statusBar.setBorder(BorderFactory.createEmptyBorder(6, 10, 6, 10));

        statusLabel.setForeground(new Color(180, 180, 180));
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));

        statusBar.add(statusLabel, BorderLayout.WEST);

        // ---- Layout ----
        frame.setLayout(new BorderLayout());
        frame.add(header, BorderLayout.NORTH);
        frame.add(scrollPane, BorderLayout.CENTER);
        frame.add(statusBar, BorderLayout.SOUTH);

        return frame;
    }

    private Icon createDotIcon(Color color) {
        return new Icon() {
            @Override public int getIconWidth() { return 10; }
            @Override public int getIconHeight() { return 10; }

            @Override
            public void paintIcon(Component c, Graphics g, int x, int y) {
                g.setColor(color);
                g.fillOval(x, y, 10, 10);
            }
        };
    }




}
