
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.DefaultHighlighter;

public class ThresholdFilterPanel extends RoundedPanel {

    // ── Expression category accent colors ──────────────────────────────────
    private static final Color C_OVER_ACTIVE  = new Color(210,  55,  55);
    private static final Color C_UNCHANGED    = new Color(115, 118, 128);
    private static final Color C_UNDER_ACTIVE = new Color( 50, 100, 205);
    private static final Color C_ACTIVE       = new Color( 45, 160,  75);
    private static final Color C_INACTIVE     = new Color( 65,  70,  82);
    private static final Color C_FLEXIBLE     = new Color(200, 135,  35);
    private static final Color C_MUST_MATCH   = new Color(120,  45, 175);

    // ── Light tints for text-area backgrounds ──────────────────────────────
    private static final Color T_OVER_ACTIVE  = new Color(255, 235, 235);
    private static final Color T_UNCHANGED    = new Color(246, 246, 249);
    private static final Color T_UNDER_ACTIVE = new Color(232, 240, 255);
    private static final Color T_ACTIVE       = new Color(232, 252, 238);
    private static final Color T_INACTIVE     = new Color(238, 238, 243);
    private static final Color T_FLEXIBLE     = new Color(255, 248, 225);
    private static final Color T_MUST_MATCH   = new Color(248, 235, 255);

    private JTextField upperField;
    private JTextField lowerField;
    private RoundedButton filterButton;
    private JTextArea aboveArea;
    private JTextArea betweenArea;
    private JTextArea belowArea;
    private JTextArea inactiveArea;
    private JTextArea activeArea;
    private JTextArea relaxedArea;
    private JTextArea nonrelaxedArea;
    private JTextField searchField;
    private RoundedButton searchButton;
    private JTextArea[] resultAreas;
    private RoundedButton moveTo;
    // private boolean[] selectedIndices;
    private String selectedText;
    // private boolean[] originalColumnIndex;

    private Map<String, boolean[]> override = new HashMap<>();
    private Map<String, String> hsaToGene = new HashMap<>();

    public ThresholdFilterPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        loadGeneMap(user.getMappingFile()[0]);
        initComponents(cardLayout, cardPanel, user);
    }

    private void loadGeneMap(String mapFilePath) {
        if (mapFilePath == null || mapFilePath.isEmpty()) return;
        File f = new File(mapFilePath);
        if (!f.exists()) return;
        try (BufferedReader br = new BufferedReader(new FileReader(f))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.trim().split("\\s+", 2);
                if (parts.length >= 2) {
                    hsaToGene.put(parts[0].trim(), parts[1].trim());
                }
            }
        } catch (IOException ex) { /* ignore */ }
    }

    private void initComponents(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Theme.BG);
        setOpaque(true);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);

        JPanel inputPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 10, 6));
        inputPanel.setBackground(Theme.BG_CARD);
        inputPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(4, 10, 4, 10)));

        JLabel downLabel = new JLabel("Under-Active Gene Cutoff:");
        downLabel.setFont(Theme.body(13));
        downLabel.setForeground(Theme.TEXT_MED);

        lowerField = new JTextField(6);
        lowerField.setText(String.valueOf(user.getDownThreshold()));
        lowerField.setFont(Theme.body(13));
        lowerField.setForeground(Theme.TEXT_DARK);
        lowerField.setCaretColor(Theme.TEXT_DARK);
        lowerField.setOpaque(true);
        lowerField.setBackground(Color.WHITE);
        lowerField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(3, 6, 3, 6)));

        JLabel upLabel = new JLabel("Over-Active Gene Cutoff:");
        upLabel.setFont(Theme.body(13));
        upLabel.setForeground(Theme.TEXT_MED);

        upperField = new JTextField(6);
        upperField.setCaretColor(Theme.TEXT_DARK);
        upperField.setText(String.valueOf(user.getUpThreshold()));
        upperField.setFont(Theme.body(13));
        upperField.setForeground(Theme.TEXT_DARK);
        upperField.setOpaque(true);
        upperField.setBackground(Color.WHITE);
        upperField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(3, 6, 3, 6)));

        filterButton = new RoundedButton("Filter", 14, new Dimension(90, 30));

        JSeparator sep = new JSeparator(JSeparator.VERTICAL);
        sep.setPreferredSize(new Dimension(1, 24));
        sep.setForeground(Theme.BORDER);

        searchField = new JTextField(10);
        searchField.setFont(Theme.body(13));
        searchField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(3, 6, 3, 6)));
        searchButton = new RoundedButton("Search", 14, new Dimension(80, 30));
        moveTo = new RoundedButton("Override", 14, new Dimension(90, 30));
        moveTo.setEnabled(false);

        inputPanel.add(downLabel);
        inputPanel.add(lowerField);
        inputPanel.add(upLabel);
        inputPanel.add(upperField);
        inputPanel.add(filterButton);
        inputPanel.add(sep);
        inputPanel.add(searchField);
        inputPanel.add(searchButton);
        inputPanel.add(moveTo);

        add(header, BorderLayout.NORTH);

        JPanel resultPanel = new JPanel(new GridLayout(1, 7, 10, 10));
        resultPanel.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));
        resultPanel.setBackground(Theme.BG);

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

        resultPanel.add(createStickyPanel("Over-Active Genes:",           aboveArea,      C_OVER_ACTIVE,  T_OVER_ACTIVE));
        resultPanel.add(createStickyPanel("Unchanged Genes:",             betweenArea,    C_UNCHANGED,    T_UNCHANGED));
        resultPanel.add(createStickyPanel("Under-Active Genes:",          belowArea,      C_UNDER_ACTIVE, T_UNDER_ACTIVE));
        resultPanel.add(createStickyPanel("Active Genes:",                activeArea,     C_ACTIVE,       T_ACTIVE));
        resultPanel.add(createStickyPanel("Known Inactive Genes:",        inactiveArea,   C_INACTIVE,     T_INACTIVE));
        resultPanel.add(createStickyPanel("Genes Allowed to be Flexible:",relaxedArea,    C_FLEXIBLE,     T_FLEXIBLE));
        resultPanel.add(createStickyPanel("Genes That Must Match:",       nonrelaxedArea, C_MUST_MATCH,   T_MUST_MATCH));

        // --- Button panel style updates
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);

        for (RoundedButton btn : new RoundedButton[]{filterButton, searchButton}) {
            btn.setBackground(Theme.PRIMARY);
            btn.setForeground(Color.WHITE);
            btn.setFont(Theme.title(13));
            btn.setFocusPainted(false);
        }
        moveTo.setBackground(Theme.WARNING);
        moveTo.setForeground(Color.WHITE);
        moveTo.setFont(Theme.title(13));
        moveTo.setFocusPainted(false);

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "idEntry"));

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
                JPanel topBar = new JPanel(new BorderLayout());
                topBar.setOpaque(false);
                topBar.add(inputPanel, BorderLayout.NORTH);
                topBar.add(buildLegend(), BorderLayout.SOUTH);
                add(topBar, BorderLayout.NORTH);
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

            JRadioButton col1 = new JRadioButton("Over-Active Genes");
            JRadioButton col2 = new JRadioButton("Unchanged Genes");
            JRadioButton col3 = new JRadioButton("Under-Active Genes");

            JRadioButton col4 = new JRadioButton("Active Genes");
            JRadioButton col5 = new JRadioButton("Known Inactive Genes");

            JRadioButton col6 = new JRadioButton("Genes Allowed to be Flexible");
            JRadioButton col7 = new JRadioButton("Genes That Must Match");

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
                    add(new JLabel("Gene Activity Category"));
                    add(col1);
                    add(col2);
                    add(col3);
                    setBorder(BorderFactory.createLineBorder(Theme.BORDER));
                }
            });

            mainPanel.add(new JPanel(new GridLayout(3, 1)) {
                {
                    add(new JLabel("Gene Activity Status"));
                    add(col4);
                    add(col5);
                    setBorder(BorderFactory.createLineBorder(Theme.BORDER));

                }
            });

            mainPanel.add(new JPanel(new GridLayout(3, 1)) {
                {
                    add(new JLabel("Gene Flexibility"));
                    add(col6);
                    add(col7);
                    setBorder(BorderFactory.createLineBorder(Theme.BORDER));

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

    JPanel createStickyPanel(String title, JTextArea textArea, Color accent, Color tint) {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(tint);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 4, 0, 0, accent),
            BorderFactory.createLineBorder(accent.darker(), 1)
        ));

        JLabel label = new JLabel(title, SwingConstants.CENTER);
        label.setForeground(Color.WHITE);
        label.setFont(Theme.title(11));
        label.setOpaque(true);
        label.setBackground(accent);
        label.setBorder(BorderFactory.createEmptyBorder(6, 4, 6, 4));

        textArea.setForeground(new Color(30, 30, 40));
        textArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        textArea.setOpaque(true);
        textArea.setBackground(tint);
        textArea.setEditable(false);

        JScrollPane scrollPane = new JScrollPane(textArea);
        scrollPane.setOpaque(true);
        scrollPane.setBackground(tint);
        scrollPane.getViewport().setOpaque(true);
        scrollPane.getViewport().setBackground(tint);
        scrollPane.setBorder(BorderFactory.createEmptyBorder());

        panel.setOpaque(true);
        panel.add(label, BorderLayout.NORTH);
        panel.add(scrollPane, BorderLayout.CENTER);
        return panel;
    }

    private JPanel buildLegend() {
        JPanel legend = new JPanel(new FlowLayout(FlowLayout.LEFT, 14, 6));
        legend.setBackground(new Color(245, 245, 250));
        legend.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(1, 0, 1, 0, new Color(210, 212, 220)),
            BorderFactory.createEmptyBorder(2, 10, 2, 10)
        ));

        JLabel title = new JLabel("Expression Key:");
        title.setFont(new Font("SansSerif", Font.BOLD, 11));
        title.setForeground(new Color(80, 85, 100));
        legend.add(title);

        legend.add(legendChip("Over-Active",  C_OVER_ACTIVE));
        legend.add(legendChip("Unchanged",    C_UNCHANGED));
        legend.add(legendChip("Under-Active", C_UNDER_ACTIVE));
        legend.add(legendChip("Active",       C_ACTIVE));
        legend.add(legendChip("Inactive",     C_INACTIVE));
        legend.add(legendChip("Flexible",     C_FLEXIBLE));
        legend.add(legendChip("Must Match",   C_MUST_MATCH));
        return legend;
    }

    private JLabel legendChip(String text, Color color) {
        JLabel chip = new JLabel("  " + text + "  ") {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(color);
                g2.fillRoundRect(0, 0, getWidth(), getHeight(), 10, 10);
                g2.dispose();
                super.paintComponent(g);
            }
        };
        chip.setFont(new Font("SansSerif", Font.BOLD, 10));
        chip.setForeground(Color.WHITE);
        chip.setOpaque(false);
        return chip;
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
            String geneName = hsaToGene.get(fileName);
            String display = (geneName != null && !geneName.isEmpty())
                    ? fileName + " - " + geneName + " (" + score + ")"
                    : fileName + " (" + score + ")";

            if (override.isEmpty() || !override.containsKey(fileName)) {
                if (score >= upper) {
                    aboveArea.append(display + "\n");
                } else if (score <= lower) {
                    belowArea.append(display + "\n");
                } else {
                    betweenArea.append(display + "\n");
                }
            }
        }
        if (!override.isEmpty()) {
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String fileName = entry.getKey();
                String geneName = hsaToGene.get(fileName);
                String display = (geneName != null && !geneName.isEmpty())
                        ? fileName + " - " + geneName : fileName;
                boolean[] arr = entry.getValue();
                for (int i = 0; i < 7; i++) {
                    if (arr[i]) {
                        resultAreas[i].append(display + "\n");
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
                String[] parts = line.split("\\s+");
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
