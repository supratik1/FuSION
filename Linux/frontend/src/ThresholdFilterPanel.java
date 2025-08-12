
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.DefaultHighlighter;

public class ThresholdFilterPanel extends RoundedPanel {

    private JTextField upperField;
    private JTextField lowerField;
    private JButton filterButton;
    private JTextArea aboveArea;
    private JTextArea betweenArea;
    private JTextArea belowArea;
    private JTextArea inactiveArea;
    private JTextArea activeArea;
    private JTextArea relaxedArea;
    private JTextArea nonrelaxedArea;
    private JTextField searchField;
    private JButton searchButton;
    private JTextArea[] resultAreas;
    private JButton moveTo;
    // private boolean[] selectedIndices;
    private String selectedText;
    // private boolean[] originalColumnIndex;

    private Map<String, boolean[]> override = new HashMap<>();

    public ThresholdFilterPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        initComponents(cardLayout, cardPanel, user);
    }

    private void initComponents(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Color.WHITE);
        setOpaque(true);

        HeaderPanel header = new HeaderPanel(user.getUsername());

        JPanel inputPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        inputPanel.setBackground(Color.WHITE);

        JPanel searchPanel = new JPanel(new FlowLayout());
        searchPanel.setBackground(Color.WHITE);
        searchPanel.setOpaque(true);

        searchField = new JTextField(8);
        searchButton = new JButton("Search");

        moveTo = new JButton("Override");
        moveTo.setEnabled(false);
        searchPanel.add(searchField);
        searchPanel.add(searchButton);
        searchPanel.add(moveTo);

        JLabel downLabel = new JLabel("Down Regulatory Threshold:");
        downLabel.setForeground(Color.BLACK);

        lowerField = new JTextField(5);
        lowerField.setText(String.valueOf(user.getDownThreshold()));
        lowerField.setForeground(Color.BLACK);
        lowerField.setCaretColor(Color.BLACK);
        lowerField.setOpaque(true);
        lowerField.setBackground(Color.WHITE);
        lowerField.setBorder(BorderFactory.createLineBorder(Color.BLACK, 1, true));
        inputPanel.add(downLabel);
        inputPanel.add(lowerField);

        JLabel upLabel = new JLabel("Up Regulatory Threshold:");
        upLabel.setForeground(Color.BLACK);

        upperField = new JTextField(5);
        upperField.setCaretColor(Color.BLACK);
        upperField.setText(String.valueOf(user.getUpThreshold()));
        upperField.setForeground(Color.BLACK);
        upperField.setOpaque(true);
        upperField.setBackground(Color.WHITE);
        upperField.setBorder(BorderFactory.createLineBorder(Color.BLACK, 1, true));
        inputPanel.add(upLabel);
        inputPanel.add(upperField);

        filterButton = new JButton("Filter Files");
        inputPanel.add(filterButton);
        inputPanel.add(searchPanel);

        add(header, BorderLayout.NORTH);

        JPanel resultPanel = new JPanel(new GridLayout(1, 7, 10, 10));
        resultPanel.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));
        resultPanel.setBackground(Color.WHITE);

        aboveArea = new JTextArea();
        betweenArea = new JTextArea();
        belowArea = new JTextArea();
        activeArea = new JTextArea();
        inactiveArea = new JTextArea();
        relaxedArea = new JTextArea();
        nonrelaxedArea = new JTextArea();

        resultAreas = new JTextArea[]{
            aboveArea, betweenArea, belowArea,
            activeArea, inactiveArea, relaxedArea, nonrelaxedArea
        };

        resultPanel.add(createStickyPanel("Up Regulatory Threshold:", aboveArea));
        resultPanel.add(createStickyPanel("Not Differentially Expressed:", betweenArea));
        resultPanel.add(createStickyPanel("Down Regulatory Thresholds:", belowArea));
        resultPanel.add(createStickyPanel("Active Node File:", activeArea));
        resultPanel.add(createStickyPanel("Inactive Node File:", inactiveArea));
        resultPanel.add(createStickyPanel("Relaxed Node File:", relaxedArea));
        resultPanel.add(createStickyPanel("Non-Relaxed Node File:", nonrelaxedArea));

        // --- Button panel style updates
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Color.WHITE);

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

        RoundedButton saveButton = new RoundedButton("Save", 20, new Dimension(100, 40));
        saveButton.setBackground(new Color(5, 161, 59));
        saveButton.setForeground(Color.WHITE);
        saveButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        saveButton.setFocusPainted(false);

        for (JButton btn : new JButton[]{filterButton, searchButton, moveTo}) {
            btn.setBackground(new Color(100, 149, 237)); // soft blue
            btn.setForeground(Color.WHITE);              // white text
            btn.setFocusPainted(false);
        }

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "relaxationPanel"));

        nextButton.addActionListener(e -> {
            user.clearOverride();
            Map<Integer, java.util.List<String>> over = new HashMap<>();
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String fileName = entry.getKey();
                boolean[] categories = entry.getValue();
                for (int i = 0; i < categories.length; i++) {
                    if (categories[i]) {
                        over.computeIfAbsent(i, k -> new ArrayList<>()).add(fileName);
                    }
                }
            }
            user.addOverride(over);
            EdgesChooserPanel edgePanel = new EdgesChooserPanel(cardLayout, cardPanel, user);
            cardPanel.add(edgePanel, "edgeEntry");
            cardLayout.show(cardPanel, "edgeEntry");
        });

        saveButton.addActionListener(e -> {
            user.clearOverride();
            Map<Integer, java.util.List<String>> over = new HashMap<>();
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String fileName = entry.getKey();
                boolean[] categories = entry.getValue();
                for (int i = 0; i < categories.length; i++) {
                    if (categories[i]) {
                        over.computeIfAbsent(i, k -> new ArrayList<>()).add(fileName);
                    }
                }
            }
            user.addOverride(over);
            user.saveData();
        });

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

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

        this.add(buttonPanel, BorderLayout.SOUTH);
        add(new JPanel(new BorderLayout()) {
            {
                add(inputPanel, BorderLayout.NORTH);
                add(resultPanel, BorderLayout.CENTER);
                setOpaque(false);
            }
        }, BorderLayout.CENTER);

        // Handlers unchanged:
        filterButton.addActionListener(e -> processFiles(user));
        searchButton.addActionListener(e -> {
            if (lowerField.getText().isBlank() || upperField.getText().isBlank()) {
                JOptionPane.showMessageDialog(this, "Enter number bounds for filtering");
                return;
            }
            if (searchField.getText().isBlank()) {
                return;
            }
            searchAndHighlight(searchField.getText());
        });

        moveTo.addActionListener(e -> {

            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Choose Transfer Destination", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(550, 550);
            dialog.setLocationRelativeTo(this);

            JPanel mainPanel = new JPanel(new GridLayout(3, 1));
            ButtonGroup mainGroup = new ButtonGroup();
            ButtonGroup sideGroup = new ButtonGroup();
            ButtonGroup laterGroup = new ButtonGroup();

            JRadioButton col1 = new JRadioButton("Above Up Regulatory Threshold");
            JRadioButton col2 = new JRadioButton("Not Differentially Expressed");
            JRadioButton col3 = new JRadioButton("Below Down Regulatory Threshold");

            JRadioButton col4 = new JRadioButton("Active Node File");
            JRadioButton col5 = new JRadioButton("Inactive Node File");

            JRadioButton col6 = new JRadioButton("Relaxed Node");
            JRadioButton col7 = new JRadioButton("Non Relaxed Node");

            JRadioButton[] columns = {col1, col2, col3, col4, col5, col6, col7};

            mainGroup.add(col1);
            mainGroup.add(col2);
            mainGroup.add(col3);

            sideGroup.add(col4);
            sideGroup.add(col5);

            laterGroup.add(col6);
            laterGroup.add(col7);

            mainPanel.add(new JPanel(new GridLayout(4, 1)) {
                {
                    add(new JLabel("Select Regulatory Threshold Field"));
                    add(col1);
                    add(col2);
                    add(col3);
                    setBorder(BorderFactory.createLineBorder(Color.BLACK));
                }
            });

            mainPanel.add(new JPanel(new GridLayout(3, 1)) {
                {
                    add(new JLabel("Select Active or Inactive node"));
                    add(col4);
                    add(col5);
                    setBorder(BorderFactory.createLineBorder(Color.BLACK));

                }
            });

            mainPanel.add(new JPanel(new GridLayout(3, 1)) {
                {
                    add(new JLabel("Select Relaxed or Non-relaxed Node"));
                    add(col6);
                    add(col7);
                    setBorder(BorderFactory.createLineBorder(Color.BLACK));

                }
            });

            if (override.containsKey(selectedText)) {
                boolean[] flags = override.get(selectedText);
                for (int i = 0; i < 7; i++) {
                    if (flags[i] == true) {
                        columns[i].setSelected(true);
                    }
                }
            }

            JPanel buttonPanel2 = new JPanel();
            JButton ok = new JButton("OK");
            JButton cancel = new JButton("Cancel");
            JButton clear = new JButton("Clear");

            boolean[] selectedIndices = new boolean[7];
            // final boolean[] optional = new boolean[2];

            ok.addActionListener(ev -> {

                boolean[] old = override.get(selectedText);

                // Optional: remove from previous
                for (int i = 0; i < 7; i++) {
                    if (old[i] == true) {
                        JTextArea source = resultAreas[i];
                        source.setText(source.getText().replace(selectedText + "\n", ""));
                        // source.setText(source.getText().replace(">> " + selectedText + "\n", ""));
                    }

                }
                if (col1.isSelected()) {
                    selectedIndices[0] = true;
                } else if (col2.isSelected()) {
                    selectedIndices[1] = true;
                } else if (col3.isSelected()) {
                    selectedIndices[2] = true;
                }
                // System.out.println(selectedMain[0]);
                // selectedIndices[3] = col4.isSelected();
                // selectedIndices[4] = col5.isSelected();
                if (col4.isSelected()) {
                    selectedIndices[3] = true;
                } else if (col5.isSelected()) {
                    selectedIndices[4] = true;
                }
                if (col6.isSelected()) {
                    selectedIndices[5] = true;
                } else if (col7.isSelected()) {
                    selectedIndices[6] = true;
                }

                override.put(selectedText, selectedIndices);
                // System.out.println(override);

                // Transfer logic
                if (selectedText != null && !selectedText.isEmpty()) {
                    // if (selectedMain[0] != -1) {
                    //     moveToColumn(selectedMain[0]);
                    // }
                    // if (optional[0]) {
                    //     moveToColumn(3);
                    // }
                    // if (optional[1]) {
                    //     moveToColumn(4);
                    // }
                    // System.out.println(selectedIndices[0]);
                    // System.out.println(selectedIndices[1]);
                    // System.out.println(selectedIndices[2]);
                    // System.out.println(selectedIndices[3]);
                    // System.out.println(selectedIndices[4]);
                    moveTo.setEnabled(false);
                    moveToColumn(selectedText);
                }

                dialog.dispose();
            });

            cancel.addActionListener(ev -> dialog.dispose());

            clear.addActionListener(ev -> {
                override.remove(selectedText);
                processFiles(user);
                dialog.dispose();
            });

            buttonPanel2.add(ok);
            buttonPanel2.add(cancel);
            buttonPanel2.add(clear);

            dialog.add(mainPanel, BorderLayout.CENTER);
            dialog.add(buttonPanel2, BorderLayout.SOUTH);
            dialog.setVisible(true);

        });
    }

    JPanel createStickyPanel(String title, JTextArea textArea) {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(Color.WHITE);

        JLabel label = new JLabel(title, SwingConstants.CENTER);
        label.setForeground(Color.BLACK);
        label.setFont(label.getFont().deriveFont(Font.BOLD, 12f));
        label.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        textArea.setForeground(Color.BLACK);
        textArea.setOpaque(true);
        textArea.setBackground(Color.WHITE);
        textArea.setEditable(false);

        JScrollPane scrollPane = new JScrollPane(textArea);
        scrollPane.setOpaque(true);
        scrollPane.setBackground(Color.WHITE);
        scrollPane.getViewport().setOpaque(true);
        scrollPane.getViewport().setBackground(Color.WHITE);
        scrollPane.setBorder(BorderFactory.createLineBorder(Color.BLACK));

        panel.setOpaque(true);
        panel.add(label, BorderLayout.NORTH);
        panel.add(scrollPane, BorderLayout.CENTER);
        return panel;
    }

    private void processFiles(UserInput user) {
        double lower, upper;
        try {
            lower = Double.parseDouble(lowerField.getText());
            upper = Double.parseDouble(upperField.getText());
            if (lower > upper) {
                JOptionPane.showMessageDialog(this, "Lower bound is greater than upper bound", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }
        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this, "Please enter valid numbers for bounds.", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        user.setDownThreshold(lower);
        user.setUpThreshold(upper);

        Map<String, Double> fileScores = readFileScores(user);

        if (fileScores == null) {
            System.out.println("Error reading file");
            return;
        }

        for (int i = 0; i < 7; i++) {
            resultAreas[i].setText("");
        }

        for (Map.Entry<String, Double> entry : fileScores.entrySet()) {
            String fileName = entry.getKey();
            Double score = entry.getValue();

            if (override.isEmpty() || !override.containsKey(fileName)) {
                if (score >= upper) {
                    aboveArea.append(fileName + " (" + score + ")\n");
                } else if (score <= lower) {
                    belowArea.append(fileName + " (" + score + ")\n");
                } else {
                    betweenArea.append(fileName + " (" + score + ")\n");
                }
            }

        }
        if (!override.isEmpty()) {
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String fileName = entry.getKey();
                boolean[] arr = entry.getValue();
                for (int i = 0; i < 7; i++) {
                    if (arr[i]) {
                        resultAreas[i].append(fileName + "\n");
                    }
                }
            }
        }

    }

    private Map<String, Double> readFileScores(UserInput user) {
        // File csv = new File(logChangesFile);
        // if (!csv.exists()) {
        //     JOptionPane.showMessageDialog(this, "CSV file 'file_scores.csv' not found in selected directory.");
        //     return null;
        // }

        Map<String, Double> scores = new HashMap<>();
        try (BufferedReader br = new BufferedReader(new FileReader(user.getLogFoldChangesFile()))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split("\t+");
                if (parts.length == 2) {
                    String filename = parts[0].trim();
                    double score = Double.parseDouble(parts[1].trim());
                    scores.put(filename, score);
                }
            }
        } catch (IOException | NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Error reading tSV: " + e.getMessage());
            return null;
        }

        return scores;
    }

    private void searchAndHighlight(String query) {
        String highlight = "";
        boolean[] originalColumnIndex = new boolean[7];
        // selectedText = query;
        if (query == null || query.trim().isEmpty()) {
            return;
        }
        query = query.trim().toLowerCase();

        int matchCount = 0;
        Set<String> uniqueMatches = new HashSet<>();

        for (int i = 0; i < resultAreas.length; i++) {
            JTextArea area = resultAreas[i];
            area.getHighlighter().removeAllHighlights();
            String[] lines = area.getText().split("\n");
            int offset = 0;

            for (String line : lines) {
                int lineLength = line.length();
                if (line.toLowerCase().contains(query)) {
                    matchCount++;
                    uniqueMatches.add(line.toLowerCase());

                    try {
                        area.getHighlighter().addHighlight(
                                offset,
                                offset + lineLength,
                                new DefaultHighlighter.DefaultHighlightPainter(Color.GREEN.darker())
                        );
                        area.setCaretPosition(offset);
                        originalColumnIndex[i] = true;
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                offset += lineLength + 1; // +1 for '\n'
            }
        }

// Store first match as selectedText for reference
        if (!uniqueMatches.isEmpty()) {
            selectedText = uniqueMatches.iterator().next();
        }

// Save matches info
        override.put(selectedText, originalColumnIndex);

// Disable moveTo if multiple different matches exist
        if (uniqueMatches.size() > 1) {
            //JOptionPane.showMessageDialog(this, "Multiple results found. Please enter a more specific ID.", "Ambiguous Entry", JOptionPane.WARNING_MESSAGE);
            moveTo.setEnabled(false);
        } else {
            moveTo.setEnabled(true);
        }

    }

    private void moveToColumn(String text) {
        boolean[] flag = override.get(text);

        for (int i = 0; i < 7; i++) {
            if (flag[i]) {
                JTextArea target = resultAreas[i];
                target.append(text + "\n");
            }
        }
    }

    // ... rest of your methods (unchanged) ...
}
