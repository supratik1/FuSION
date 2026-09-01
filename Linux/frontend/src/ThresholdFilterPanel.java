
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.DefaultHighlighter;

public class ThresholdFilterPanel extends RoundedPanel {

    private static final Color C_OVER_ACTIVE  = new Color(210,  55,  55);
    private static final Color C_UNCHANGED    = new Color(115, 118, 128);
    private static final Color C_UNDER_ACTIVE = new Color( 50, 100, 205);
    private static final Color C_ACTIVE       = new Color( 45, 160,  75);
    private static final Color C_INACTIVE     = new Color( 65,  70,  82);
    private static final Color C_FLEXIBLE     = new Color(200, 135,  35);
    private static final Color C_MUST_MATCH   = new Color(120,  45, 175);

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
    private String selectedText;
    private String selectedHsaId;

    private Map<String, boolean[]> override = new HashMap<>();
    private Map<String, String> overrideDisplay = new HashMap<>();
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

        JPanel inputPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, Theme.GAP_SM, Theme.scale(6)));
        inputPanel.setBackground(Theme.BG_CARD);
        inputPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(Theme.scale(4), Theme.scale(10), Theme.scale(4), Theme.scale(10))));

        JLabel downLabel = new JLabel("Down Regulatory Threshold:");
        downLabel.setFont(Theme.body(13));
        downLabel.setForeground(Theme.TEXT_MED);

        lowerField = new JTextField(6);
        lowerField.setText(String.valueOf(user.getDownThreshold()));
        lowerField.setFont(Theme.body(13));
        Theme.styleDarkField(lowerField);
        lowerField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(3), Theme.scale(6), Theme.scale(3), Theme.scale(6))));

        JLabel upLabel = new JLabel("Up Regulatory Threshold:");
        upLabel.setFont(Theme.body(13));
        upLabel.setForeground(Theme.TEXT_MED);

        upperField = new JTextField(6);
        upperField.setText(String.valueOf(user.getUpThreshold()));
        upperField.setFont(Theme.body(13));
        Theme.styleDarkField(upperField);
        upperField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(3), Theme.scale(6), Theme.scale(3), Theme.scale(6))));

        filterButton = new RoundedButton("Filter Files", Theme.scale(14),
            new Dimension(Theme.scale(110), Theme.scale(32)));

        JSeparator sep = new JSeparator(JSeparator.VERTICAL);
        sep.setPreferredSize(new Dimension(1, Theme.scale(24)));
        sep.setForeground(Theme.BORDER);

        searchField = new JTextField(10);
        searchField.setFont(Theme.body(13));
        Theme.styleDarkField(searchField);
        searchField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(3), Theme.scale(6), Theme.scale(3), Theme.scale(6))));

        searchButton = new RoundedButton("Search", Theme.scale(14),
            new Dimension(Theme.scale(80), Theme.scale(32)));
        moveTo = new RoundedButton("Override", Theme.scale(14),
            new Dimension(Theme.scale(100), Theme.scale(32)));
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

        JPanel resultPanel = new JPanel(new GridLayout(1, 7, Theme.scale(10), Theme.scale(10)));
        resultPanel.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(10), Theme.scale(20), Theme.scale(10), Theme.scale(20)));
        resultPanel.setBackground(Theme.BG);

        aboveArea      = new JTextArea();
        betweenArea    = new JTextArea();
        belowArea      = new JTextArea();
        activeArea     = new JTextArea();
        inactiveArea   = new JTextArea();
        relaxedArea    = new JTextArea();
        nonrelaxedArea = new JTextArea();

        resultAreas = new JTextArea[]{
            aboveArea, betweenArea, belowArea,
            activeArea, inactiveArea, relaxedArea, nonrelaxedArea
        };

        for (int i = 0; i < resultAreas.length; i++) {
            addClickToSelect(resultAreas[i], i);
        }

        resultPanel.add(createStickyPanel("Up Regulatory Threshold:",    aboveArea,      C_OVER_ACTIVE,  T_OVER_ACTIVE));
        resultPanel.add(createStickyPanel("Not Differentially Expressed:", betweenArea,  C_UNCHANGED,    T_UNCHANGED));
        resultPanel.add(createStickyPanel("Down Regulatory Threshold:", belowArea,       C_UNDER_ACTIVE, T_UNDER_ACTIVE));
        resultPanel.add(createStickyPanel("Active Node File:",          activeArea,      C_ACTIVE,       T_ACTIVE));
        resultPanel.add(createStickyPanel("Inactive Node File:",        inactiveArea,    C_INACTIVE,     T_INACTIVE));
        resultPanel.add(createStickyPanel("Relaxed Node File:",         relaxedArea,     C_FLEXIBLE,     T_FLEXIBLE));
        resultPanel.add(createStickyPanel("Non-Relaxed Node File:",     nonrelaxedArea,  C_MUST_MATCH,   T_MUST_MATCH));

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton   = Theme.navBtn("Next »", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton   = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton   = Theme.successBtn("Save", 110);

        for (RoundedButton btn : new RoundedButton[]{filterButton, searchButton}) {
            btn.setBackground(Theme.PRIMARY);
            btn.setForeground(Color.WHITE);
            btn.setFont(Theme.title(13));
        }
        moveTo.setBackground(Theme.WARNING);
        moveTo.setForeground(new Color(26, 17, 0));
        moveTo.setFont(Theme.title(13));

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "idEntry"));

        nextButton.addActionListener(e -> {
            user.clearOverride();
            Map<Integer, java.util.List<String>> over = new HashMap<>();
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                boolean[] categories = entry.getValue();
                for (int i = 0; i < categories.length; i++) {
                    if (categories[i]) {
                        over.computeIfAbsent(i, k -> new ArrayList<>()).add(entry.getKey());
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
                boolean[] categories = entry.getValue();
                for (int i = 0; i < categories.length; i++) {
                    if (categories[i]) {
                        over.computeIfAbsent(i, k -> new ArrayList<>()).add(entry.getKey());
                    }
                }
            }
            user.addOverride(over);
            user.saveData();
        });

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        this.add(buttonPanel, BorderLayout.SOUTH);
        add(new JPanel(new BorderLayout()) {{
            JPanel topBar = new JPanel(new BorderLayout());
            topBar.setOpaque(false);
            topBar.add(inputPanel, BorderLayout.NORTH);
            topBar.add(buildLegend(), BorderLayout.SOUTH);
            add(topBar, BorderLayout.NORTH);
            add(resultPanel, BorderLayout.CENTER);
            setOpaque(false);
        }}, BorderLayout.CENTER);

        filterButton.addActionListener(e -> processFiles(user));

        // On every panel visit: restore saved overrides, then auto-run filter if file present
        if (user.hasAnyOverride()) restoreOverrideFromUser(user);
        String _logFold = user.getLogFoldChangesFile();
        if (_logFold != null && !_logFold.isBlank() && new File(_logFold).exists()) {
            SwingUtilities.invokeLater(() -> processFiles(user));
        }

        searchButton.addActionListener(e -> {
            if (lowerField.getText().isBlank() || upperField.getText().isBlank()) {
                JOptionPane.showMessageDialog(this, "Enter number bounds for filtering");
                return;
            }
            if (searchField.getText().isBlank()) return;
            searchAndHighlight(searchField.getText());
        });

        moveTo.addActionListener(e -> {
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Choose Transfer Destination", true);
            dialog.setLayout(new BorderLayout(Theme.GAP_SM, Theme.GAP_SM));
            dialog.setSize(Theme.scale(550), Theme.scale(550));
            dialog.setLocationRelativeTo(this);
            dialog.getContentPane().setBackground(Theme.BG);

            JPanel mainPanel = new JPanel(new GridLayout(3, 1));
            mainPanel.setBackground(Theme.BG);
            ButtonGroup allGroup = new ButtonGroup();

            JRadioButton col1 = makeDialogRadio("Up Regulatory Threshold");
            JRadioButton col2 = makeDialogRadio("Not Differentially Expressed");
            JRadioButton col3 = makeDialogRadio("Down Regulatory Threshold");
            JRadioButton col4 = makeDialogRadio("Active Node File");
            JRadioButton col5 = makeDialogRadio("Inactive Node File");
            JRadioButton col6 = makeDialogRadio("Relaxed Node File");
            JRadioButton col7 = makeDialogRadio("Non-Relaxed Node File");
            JRadioButton[] columns = {col1, col2, col3, col4, col5, col6, col7};

            for (JRadioButton rb : columns) allGroup.add(rb);

            mainPanel.add(makeDialogGroup("Regulatory Category", col1, col2, col3));
            mainPanel.add(makeDialogGroup("Node Activity", col4, col5));
            mainPanel.add(makeDialogGroup("Node Relaxation", col6, col7));

            if (override.containsKey(selectedHsaId)) {
                boolean[] flags = override.get(selectedHsaId);
                for (int i = 0; i < 7; i++) {
                    if (flags[i]) columns[i].setSelected(true);
                }
            }

            boolean[] selectedIndices = new boolean[7];

            RoundedButton ok     = Theme.successBtn("OK", 100);
            RoundedButton cancel = Theme.dangerBtn("Cancel", 100);
            RoundedButton clear  = Theme.warningBtn("Clear", 100);

            ok.addActionListener(ev -> {
                // Remove from every area that previously held this node
                for (int i = 0; i < 7; i++) {
                    String raw = resultAreas[i].getText();
                    StringBuilder sb = new StringBuilder();
                    for (String ln : raw.split("\n", -1)) {
                        String t = ln.trim();
                        if (!t.isEmpty()
                                && !t.equals(selectedHsaId)
                                && !t.startsWith(selectedHsaId + " ")
                                && !t.startsWith(selectedHsaId + "-")) {
                            sb.append(ln).append("\n");
                        }
                    }
                    resultAreas[i].setText(sb.toString());
                }
                // Record new single destination
                for (int i = 0; i < 7; i++) selectedIndices[i] = columns[i].isSelected();

                override.put(selectedHsaId, selectedIndices);
                if (selectedHsaId != null && !selectedHsaId.isEmpty()) {
                    moveTo.setEnabled(false);
                    moveToColumn(selectedHsaId);
                }
                dialog.dispose();
            });

            cancel.addActionListener(ev -> dialog.dispose());
            clear.addActionListener(ev -> {
                override.remove(selectedHsaId);
                overrideDisplay.remove(selectedHsaId);
                processFiles(user);
                dialog.dispose();
            });

            JPanel dialogBtns = new JPanel(new FlowLayout(FlowLayout.CENTER, Theme.GAP_SM, Theme.GAP_SM));
            dialogBtns.setBackground(Theme.BG);
            dialogBtns.add(ok);
            dialogBtns.add(cancel);
            dialogBtns.add(clear);

            dialog.add(mainPanel, BorderLayout.CENTER);
            dialog.add(dialogBtns, BorderLayout.SOUTH);
            dialog.setVisible(true);
        });
    }

    private JRadioButton makeDialogRadio(String text) {
        JRadioButton rb = new JRadioButton(text);
        rb.setFont(Theme.body(13));
        rb.setForeground(Theme.TEXT_DARK);
        rb.setBackground(Theme.BG_CARD);
        rb.setOpaque(true);
        return rb;
    }

    private JPanel makeDialogGroup(String title, JRadioButton... radios) {
        JPanel p = new JPanel(new GridLayout(radios.length + 1, 1));
        p.setBackground(Theme.BG_CARD);
        p.setBorder(BorderFactory.createLineBorder(Theme.BORDER));
        JLabel lbl = new JLabel(title);
        lbl.setFont(Theme.title(13));
        lbl.setForeground(Theme.TEXT_DARK);
        lbl.setBorder(BorderFactory.createEmptyBorder(0, Theme.scale(8), 0, 0));
        p.add(lbl);
        for (JRadioButton rb : radios) p.add(rb);
        return p;
    }

    JPanel createStickyPanel(String title, JTextArea textArea, Color accent, Color tint) {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(tint);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 4, 0, 0, accent),
            BorderFactory.createLineBorder(accent.darker(), 1)));

        JLabel label = new JLabel(title, SwingConstants.CENTER);
        label.setForeground(Color.WHITE);
        label.setFont(Theme.title(11));
        label.setOpaque(true);
        label.setBackground(accent);
        label.setBorder(BorderFactory.createEmptyBorder(Theme.scale(6), Theme.scale(4), Theme.scale(6), Theme.scale(4)));

        textArea.setForeground(new Color(30, 30, 40));
        textArea.setFont(Theme.mono(12));
        textArea.setOpaque(true);
        textArea.setBackground(tint);
        textArea.setEditable(false);
        textArea.setLineWrap(true);
        textArea.setWrapStyleWord(true);

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
        JPanel legend = new JPanel(new FlowLayout(FlowLayout.LEFT, Theme.scale(14), Theme.scale(6)));
        legend.setBackground(Theme.SURFACE);
        legend.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(1, 0, 1, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(Theme.scale(2), Theme.scale(10), Theme.scale(2), Theme.scale(10))));

        JLabel title = new JLabel("Expression Key:");
        title.setFont(Theme.body(11));
        title.setForeground(Theme.TEXT_MED);
        legend.add(title);

        legend.add(legendChip("Up Regulated",     C_OVER_ACTIVE));
        legend.add(legendChip("Not Diff Expr",   C_UNCHANGED));
        legend.add(legendChip("Down Regulated",  C_UNDER_ACTIVE));
        legend.add(legendChip("Active Node",     C_ACTIVE));
        legend.add(legendChip("Inactive Node",   C_INACTIVE));
        legend.add(legendChip("Relaxed Node",    C_FLEXIBLE));
        legend.add(legendChip("Non-Relaxed",     C_MUST_MATCH));
        return legend;
    }

    private JLabel legendChip(String text, Color color) {
        JLabel chip = new JLabel("  " + text + "  ") {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(color);
                g2.fillRoundRect(0, 0, getWidth(), getHeight(), Theme.scale(10), Theme.scale(10));
                g2.dispose();
                super.paintComponent(g);
            }
        };
        chip.setFont(Theme.body(10));
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
        if (fileScores == null) return;

        for (int i = 0; i < 7; i++) resultAreas[i].setText("");

        for (Map.Entry<String, Double> entry : fileScores.entrySet()) {
            String fileName = entry.getKey();
            Double score = entry.getValue();
            String geneName = hsaToGene.get(fileName);
            String display = (geneName != null && !geneName.isEmpty())
                    ? fileName + " - " + geneName + " (" + score + ")"
                    : fileName + " (" + score + ")";

            if (override.isEmpty() || !override.containsKey(fileName)) {
                if (score >= upper)       aboveArea.append(display + "\n");
                else if (score <= lower)  belowArea.append(display + "\n");
                else                      betweenArea.append(display + "\n");
            }
        }
        if (!override.isEmpty()) {
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String hsaId = entry.getKey();
                String display = overrideDisplay.getOrDefault(hsaId, hsaId);
                boolean[] arr = entry.getValue();
                for (int i = 0; i < 7; i++) {
                    if (arr[i]) resultAreas[i].append(display + "\n");
                }
            }
        }
    }

    private Map<String, Double> readFileScores(UserInput user) {
        Map<String, Double> scores = new HashMap<>();
        try (BufferedReader br = new BufferedReader(new FileReader(user.getLogFoldChangesFile()))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split("\\s+");
                if (parts.length == 2) {
                    scores.put(parts[0].trim(), Double.parseDouble(parts[1].trim()));
                }
            }
        } catch (IOException | NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Error reading file: " + e.getMessage());
            return null;
        }
        return scores;
    }

    private void searchAndHighlight(String query) {
        boolean[] originalColumnIndex = new boolean[7];
        if (query == null || query.trim().isEmpty()) return;
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
                    uniqueMatches.add(line);
                    try {
                        area.getHighlighter().addHighlight(
                                offset, offset + lineLength,
                                new DefaultHighlighter.DefaultHighlightPainter(Color.GREEN.darker()));
                        area.setCaretPosition(offset);
                        originalColumnIndex[i] = true;
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                offset += lineLength + 1;
            }
        }

        if (!uniqueMatches.isEmpty()) {
            selectedText = uniqueMatches.iterator().next();
            selectedHsaId = selectedText.split("\\s+")[0];
            overrideDisplay.put(selectedHsaId, selectedText);
            override.put(selectedHsaId, originalColumnIndex);
        }

        if (uniqueMatches.size() > 1) {
            moveTo.setEnabled(false);
        } else {
            moveTo.setEnabled(true);
        }
    }

    private void addClickToSelect(JTextArea area, int colIndex) {
        area.addMouseListener(new java.awt.event.MouseAdapter() {
            @Override
            public void mouseClicked(java.awt.event.MouseEvent e) {
                int pos = (int) area.viewToModel2D(e.getPoint());
                if (pos < 0) return;
                try {
                    // Use logical line (getLineOf*) so word-wrap doesn't truncate the entry
                    int lineNum   = area.getLineOfOffset(pos);
                    int lineStart = area.getLineStartOffset(lineNum);
                    int lineEnd   = area.getLineEndOffset(lineNum);
                    String line   = area.getText(lineStart, lineEnd - lineStart).trim();
                    if (line.isEmpty()) return;

                    for (JTextArea a : resultAreas) a.getHighlighter().removeAllHighlights();
                    area.getHighlighter().addHighlight(lineStart, lineEnd,
                        new DefaultHighlighter.DefaultHighlightPainter(Color.GREEN.darker()));

                    selectedText  = line;
                    selectedHsaId = line.split("\\s+")[0];
                    overrideDisplay.put(selectedHsaId, line);

                    if (!override.containsKey(selectedHsaId)) {
                        boolean[] cols = new boolean[7];
                        cols[colIndex] = true;
                        override.put(selectedHsaId, cols);
                    }

                    moveTo.setEnabled(true);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        });
    }

    private void moveToColumn(String hsaId) {
        boolean[] flag = override.get(hsaId);
        String display = overrideDisplay.getOrDefault(hsaId, hsaId);
        for (int i = 0; i < 7; i++) {
            if (flag[i]) resultAreas[i].append(display + "\n");
        }
    }

    private void restoreOverrideFromUser(UserInput user) {
        override.clear();
        overrideDisplay.clear();
        for (int col = 0; col < 7; col++) {
            java.util.List<String> ids = user.getOverride(col);
            if (ids != null) {
                for (String hsaId : ids) {
                    boolean[] flags = override.computeIfAbsent(hsaId, k -> new boolean[7]);
                    flags[col] = true;
                }
            }
        }
        rebuildOverrideDisplay(user);
    }

    private void rebuildOverrideDisplay(UserInput user) {
        String logFold = user.getLogFoldChangesFile();
        if (logFold == null || logFold.isBlank()) return;
        Map<String, Double> scores = new HashMap<>();
        try (BufferedReader br = new BufferedReader(new FileReader(logFold))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.trim().split("\\s+");
                if (parts.length >= 2) {
                    try { scores.put(parts[0].trim(), Double.parseDouble(parts[1].trim())); }
                    catch (NumberFormatException ignored) {}
                }
            }
        } catch (IOException ignored) {}
        for (String hsaId : override.keySet()) {
            Double score = scores.get(hsaId);
            String gene = hsaToGene.get(hsaId);
            String display = (gene != null && !gene.isEmpty())
                ? hsaId + " - " + gene + (score != null ? " (" + score + ")" : "")
                : hsaId + (score != null ? " (" + score + ")" : "");
            overrideDisplay.put(hsaId, display);
        }
    }
}
