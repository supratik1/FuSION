import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.*;
import java.util.List;
import java.util.LinkedHashSet;
import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

public class IdEntryPanel extends RoundedPanel {

    private JTextField inputField1, inputField2, inputField3;
    private JComboBox<String> suggestions1, suggestions2, suggestions3;
    Map<String, List<String>> idMap     = new HashMap<>();
    Map<String, String>       hsaToGene = new HashMap<>();  // lowercase hsa id -> gene symbol
    Map<String, String>       geneToHsa = new HashMap<>();  // lowercase gene symbol -> lowercase hsa id
    private UserInput user;
    private JLabel geneStatusLabel;

    public IdEntryPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        this.user = user;
        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        String[] mappingFile = user.getMappingFile();

        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        center.setBackground(Theme.BG);
        center.setOpaque(true);
        center.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(20), Theme.scale(50), Theme.scale(20), Theme.scale(50)));

        JLabel title = new JLabel("Add HSA IDs");
        title.setFont(Theme.title(26));
        title.setForeground(Theme.TEXT_DARK);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        center.add(title);
        center.add(Box.createVerticalStrut(Theme.scale(8)));

        geneStatusLabel = new JLabel("Loading gene list…");
        geneStatusLabel.setFont(Theme.body(12));
        geneStatusLabel.setForeground(Theme.TEXT_MED);
        geneStatusLabel.setAlignmentX(Component.CENTER_ALIGNMENT);
        center.add(geneStatusLabel);
        center.add(Box.createVerticalStrut(Theme.scale(16)));

        // ---- Row 1: Source ----
        JLabel label1 = new JLabel("Source hsa ID");
        label1.setFont(Theme.title(13));
        label1.setForeground(Theme.TEXT_DARK);

        inputField1 = new JTextField(18);
        Theme.styleDarkField(inputField1);
        inputField1.setFont(Theme.body(14));

        suggestions1 = makeDarkCombo(Theme.scale(260), Theme.scale(36));

        JPanel row1 = createIdRow(label1, inputField1, suggestions1);
        center.add(row1);
        center.add(Box.createVerticalStrut(Theme.scale(12)));

        // ---- Row 2: Target ----
        JLabel label2 = new JLabel("Target hsa ID");
        label2.setFont(Theme.title(13));
        label2.setForeground(Theme.TEXT_DARK);

        inputField2 = new JTextField(18);
        Theme.styleDarkField(inputField2);
        inputField2.setFont(Theme.body(14));

        suggestions2 = makeDarkCombo(Theme.scale(260), Theme.scale(36));

        JPanel row2 = createIdRow(label2, inputField2, suggestions2);
        center.add(row2);
        center.add(Box.createVerticalStrut(Theme.scale(12)));

        // ---- Row 3: Candidate ----
        JLabel label3 = new JLabel("Candidate Implicated Node hsa ID");
        label3.setFont(Theme.title(13));
        label3.setForeground(Theme.TEXT_DARK);

        inputField3 = new JTextField(18);
        Theme.styleDarkField(inputField3);
        inputField3.setFont(Theme.body(14));

        suggestions3 = makeDarkCombo(Theme.scale(260), Theme.scale(36));

        JPanel row3 = createIdRow(label3, inputField3, suggestions3);
        center.add(row3);
        center.add(Box.createVerticalStrut(Theme.scale(16)));

        // ---- Row 4: HSA not merged ----
        JPanel hsaNot = new JPanel(new BorderLayout(Theme.scale(12), 0));
        hsaNot.setBackground(Theme.BG_CARD);
        hsaNot.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(72)));
        hsaNot.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(16), Theme.scale(10), Theme.scale(16))));

        JPanel hsaInfo = new JPanel();
        hsaInfo.setLayout(new BoxLayout(hsaInfo, BoxLayout.Y_AXIS));
        hsaInfo.setOpaque(false);
        JLabel hsaJLabel = new JLabel("HSA IDs not to be merged:");
        hsaJLabel.setFont(Theme.title(13));
        hsaJLabel.setForeground(Theme.TEXT_DARK);
        JLabel ans = new JLabel("No File Selected");
        ans.setFont(Theme.body(12));
        ans.setForeground(Theme.TEXT_MED);
        hsaInfo.add(hsaJLabel);
        hsaInfo.add(ans);

        RoundedButton hsaButton = Theme.navBtn("Choose TXT File", 140);
        hsaNot.add(hsaInfo, BorderLayout.CENTER);
        hsaNot.add(hsaButton, BorderLayout.EAST);
        center.add(hsaNot);
        center.add(Box.createVerticalStrut(Theme.scale(10)));

        // ---- Row 5: Mapping files ----
        JPanel mappingPanel = new JPanel(new BorderLayout(Theme.scale(12), 0));
        mappingPanel.setBackground(Theme.BG_CARD);
        mappingPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(48)));
        mappingPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(8), Theme.scale(16), Theme.scale(8), Theme.scale(16))));
        JLabel ques = new JLabel("Would you like to change the Mapping Files?");
        ques.setFont(Theme.title(13));
        ques.setForeground(Theme.TEXT_DARK);
        RoundedButton changeMapping = Theme.navBtn("Change Mapping File", 200);
        mappingPanel.add(ques, BorderLayout.CENTER);
        mappingPanel.add(changeMapping, BorderLayout.EAST);
        center.add(mappingPanel);

        add(center, BorderLayout.CENTER);

        // ---- Keyboard listeners ----
        inputField1.addKeyListener(new KeyAdapter() {
            @Override public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER && performExactMatch(inputField1))
                    user.setSourceHsaID(inputField1.getText());
            }
        });
        inputField2.addKeyListener(new KeyAdapter() {
            @Override public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER && performExactMatch(inputField2))
                    user.setTargetHsaID(inputField2.getText());
            }
        });
        inputField3.addKeyListener(new KeyAdapter() {
            @Override public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER && performExactMatch(inputField3))
                    user.setCandidateID(inputField3.getText());
            }
        });

        suggestions1.addActionListener(e -> {
            String sel = (String) suggestions1.getSelectedItem();
            if (suggestions1.isPopupVisible() && sel != null) {
                String id = extractHsaId(sel);
                SwingUtilities.invokeLater(() -> inputField1.setText(id));
                user.setSourceHsaID(id);
            }
        });
        suggestions2.addActionListener(e -> {
            String sel = (String) suggestions2.getSelectedItem();
            if (suggestions2.isPopupVisible() && sel != null) {
                String id = extractHsaId(sel);
                SwingUtilities.invokeLater(() -> inputField2.setText(id));
                user.setTargetHsaID(id);
            }
        });
        suggestions3.addActionListener(e -> {
            String sel = (String) suggestions3.getSelectedItem();
            if (suggestions3.isPopupVisible() && sel != null) {
                String id = extractHsaId(sel);
                SwingUtilities.invokeLater(() -> inputField3.setText(id));
                user.setCandidateID(id);
            }
        });

        hsaButton.addActionListener(e -> {
            JFileChooser chooser = openInWorkDir();
            if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
                File f = chooser.getSelectedFile();
                user.sethsaNotFile(f.getAbsolutePath());
                ans.setText(f.getName());
            }
        });

        inputField1.getDocument().addDocumentListener(new SuggestionUpdater(inputField1, suggestions1));
        inputField2.getDocument().addDocumentListener(new SuggestionUpdater(inputField2, suggestions2));
        inputField3.getDocument().addDocumentListener(new SuggestionUpdater(inputField3, suggestions3));

        changeMapping.addActionListener(e -> {
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Change Mapping File", true);
            dialog.setLayout(new BorderLayout(Theme.GAP_SM, Theme.GAP_SM));
            dialog.setSize(Theme.scale(1150), Theme.scale(300));
            dialog.setLocationRelativeTo(null);
            dialog.getContentPane().setBackground(Theme.BG);

            JPanel panel = new JPanel(new GridLayout(1, 3, Theme.scale(15), Theme.scale(15)));
            panel.setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(30), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            panel.setBackground(Theme.BG_CARD);

            RoundedButton hsaToGeneBtn = Theme.navBtn("Choose Map File", 150);
            RoundedButton hsaToPathBtn = Theme.navBtn("Choose Map File", 150);
            RoundedButton interDBBtn   = Theme.navBtn("Choose Map File", 150);

            JLabel geneField = new JLabel(mappingFile[0]);
            geneField.setFont(Theme.body(12));
            geneField.setForeground(Theme.TEXT_MED);
            JLabel pathField = new JLabel(mappingFile[1]);
            pathField.setFont(Theme.body(12));
            pathField.setForeground(Theme.TEXT_MED);
            JLabel dbField = new JLabel(!mappingFile[2].equals("") ? mappingFile[2] : "No file selected");
            dbField.setFont(Theme.body(12));
            dbField.setForeground(Theme.TEXT_MED);

            for (JLabel lbl : new JLabel[]{geneField, pathField, dbField}) {
                lbl.setBackground(Theme.BG_CARD);
            }

            JLabel geneTitle = new JLabel("HSA to Gene Symbol Map", SwingConstants.CENTER);
            geneTitle.setFont(Theme.title(13));
            geneTitle.setForeground(Theme.TEXT_DARK);
            JLabel pathTitle = new JLabel("HSA path to Path Name Map", SwingConstants.CENTER);
            pathTitle.setFont(Theme.title(13));
            pathTitle.setForeground(Theme.TEXT_DARK);
            JLabel dbTitle = new JLabel("Cross Database Map", SwingConstants.CENTER);
            dbTitle.setFont(Theme.title(13));
            dbTitle.setForeground(Theme.TEXT_DARK);

            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Theme.BG_CARD);
                add(geneTitle, BorderLayout.NORTH);
                add(geneField, BorderLayout.CENTER);
                add(hsaToGeneBtn, BorderLayout.SOUTH);
            }});
            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Theme.BG_CARD);
                add(pathTitle, BorderLayout.NORTH);
                add(pathField, BorderLayout.CENTER);
                add(hsaToPathBtn, BorderLayout.SOUTH);
            }});
            panel.add(new JPanel(new BorderLayout()) {{
                setBackground(Theme.BG_CARD);
                add(dbTitle, BorderLayout.NORTH);
                add(dbField, BorderLayout.CENTER);
                add(interDBBtn, BorderLayout.SOUTH);
            }});
            dialog.add(panel, BorderLayout.CENTER);

            RoundedButton okButton = Theme.successBtn("OK", 90);
            JPanel bottomPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
            bottomPanel.setBackground(Theme.BG);
            bottomPanel.add(okButton);
            dialog.add(bottomPanel, BorderLayout.SOUTH);

            okButton.addActionListener(err -> { user.setMappingFile(mappingFile); dialog.dispose(); });
            hsaToGeneBtn.addActionListener(err -> {
                JFileChooser chooser = openInWorkDir();
                if (chooser.showOpenDialog(dialog) == JFileChooser.APPROVE_OPTION) {
                    mappingFile[0] = chooser.getSelectedFile().getAbsolutePath();
                    geneField.setText(mappingFile[0]);
                }
            });
            hsaToPathBtn.addActionListener(err -> {
                JFileChooser chooser = openInWorkDir();
                if (chooser.showOpenDialog(dialog) == JFileChooser.APPROVE_OPTION) {
                    mappingFile[1] = chooser.getSelectedFile().getAbsolutePath();
                    pathField.setText(mappingFile[1]);
                }
            });
            interDBBtn.addActionListener(err -> {
                JFileChooser chooser = openInWorkDir();
                if (chooser.showOpenDialog(dialog) == JFileChooser.APPROVE_OPTION) {
                    mappingFile[2] = chooser.getSelectedFile().getAbsolutePath();
                    dbField.setText(mappingFile[2]);
                }
            });

            dialog.setVisible(true);
        });

        RoundedButton nextButton   = Theme.navBtn("Next »", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton   = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton   = Theme.successBtn("Save", 110);

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "relaxationPanel"));
        saveButton.addActionListener(e -> { user.setMappingFile(mappingFile); user.saveData(); });
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

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setOpaque(false);
        }});
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setOpaque(false);
        }});
        add(buttonPanel, BorderLayout.SOUTH);
    }

    @Override
    public void addNotify() {
        super.addNotify();
        reloadGenes();
    }

    private void reloadGenes() {
        loadGeneIdsFromFoldChangeFile(user.getLogFoldChangesFile());
        loadGeneSymbols(user.getMappingFile()[0]);
        SwingUtilities.invokeLater(() -> {
            if (idMap.isEmpty()) {
                String f = user.getLogFoldChangesFile();
                if (f == null || f.isBlank()) {
                    geneStatusLabel.setText("No gene list — select your Gene Activity Data file first (Step 2)");
                } else {
                    geneStatusLabel.setText("Gene file not found on this device — reselect it in Step 2");
                }
                geneStatusLabel.setForeground(Theme.WARNING);
            } else {
                String name = new File(user.getLogFoldChangesFile()).getName();
                geneStatusLabel.setText(idMap.size() + " genes loaded from " + name + " — type to search");
                geneStatusLabel.setForeground(Theme.SUCCESS);
            }
        });
    }

    private JPanel createIdRow(JLabel label, JTextField field, JComboBox<String> combo) {
        javax.swing.border.Border normalBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(7), Theme.scale(10), Theme.scale(7), Theme.scale(10)));
        javax.swing.border.Border focusBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.PRIMARY, 2),
            BorderFactory.createEmptyBorder(Theme.scale(6), Theme.scale(9), Theme.scale(6), Theme.scale(9)));

        field.setBorder(normalBorder);
        field.setOpaque(true);
        field.addFocusListener(new FocusAdapter() {
            public void focusGained(FocusEvent e) { field.setBorder(focusBorder); }
            public void focusLost(FocusEvent e)   { field.setBorder(normalBorder); }
        });

        JPanel card = new JPanel(new BorderLayout(Theme.scale(10), Theme.scale(4)));
        card.setBackground(Theme.BG_CARD);
        card.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(96)));
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(16), Theme.scale(10), Theme.scale(16))));
        JPanel left = new JPanel();
        left.setLayout(new BoxLayout(left, BoxLayout.Y_AXIS));
        left.setOpaque(false);
        label.setAlignmentX(Component.LEFT_ALIGNMENT);
        field.setAlignmentX(Component.LEFT_ALIGNMENT);
        field.setMaximumSize(new Dimension(Theme.scale(240), Theme.scale(38)));
        left.add(label);
        left.add(Box.createVerticalStrut(Theme.scale(5)));
        left.add(field);

        // Wrap combo in GridBagLayout panel so it stays at its preferred height
        // (BorderLayout.EAST would stretch it to fill the full card height)
        JPanel comboWrap = new JPanel(new GridBagLayout());
        comboWrap.setOpaque(false);
        combo.setMaximumSize(new Dimension(Theme.scale(260), Theme.scale(38)));
        combo.setPreferredSize(new Dimension(Theme.scale(260), Theme.scale(38)));
        comboWrap.add(combo);

        card.add(left,      BorderLayout.CENTER);
        card.add(comboWrap, BorderLayout.EAST);
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

    /** "EGFR (hsa:1956)" → "hsa:1956";  plain id → as-is. */
    private static String extractHsaId(String item) {
        if (item == null) return null;
        int p = item.lastIndexOf(" (");
        if (p >= 0 && item.endsWith(")"))
            return item.substring(p + 2, item.length() - 1);
        return item;
    }

    /** "hsa:1956" → "EGFR (hsa:1956)" when known, else just "hsa:1956". */
    private String formatItem(String hsaId) {
        String gene = hsaToGene.get(hsaId.toLowerCase());
        return gene != null ? gene + " (" + hsaId + ")" : hsaId;
    }

    /** Look up in idMap tolerating "hsa:ID" vs "ID" format mismatch. */
    private List<String> lookupInIdMap(String hsaLower) {
        List<String> r = idMap.get(hsaLower);
        if (r != null) return r;
        String alt = hsaLower.startsWith("hsa:") ? hsaLower.substring(4) : "hsa:" + hsaLower;
        return idMap.getOrDefault(alt, Collections.emptyList());
    }

    /** Load HSA→GeneSymbol mapping (tab-separated: col0=hsa_id, col1=gene_symbol). */
    private void loadGeneSymbols(String filePath) {
        hsaToGene.clear();
        geneToHsa.clear();
        if (filePath == null || filePath.isBlank()) return;
        File file = new File(filePath.trim());
        if (!file.exists()) return;
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#")) continue;
                String[] parts = line.split("\t");
                if (parts.length >= 2) {
                    String hsa  = parts[0].trim();
                    String gene = parts[1].trim();
                    if (!hsa.isEmpty() && !gene.isEmpty()) {
                        hsaToGene.put(hsa.toLowerCase(), gene);
                        geneToHsa.put(gene.toLowerCase(), hsa.toLowerCase());
                    }
                }
            }
        } catch (IOException ignored) {}
    }

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

    private class SuggestionUpdater implements DocumentListener {
        private final JTextField field;
        private final JComboBox<String> box;

        SuggestionUpdater(JTextField field, JComboBox<String> box) {
            this.field = field;
            this.box = box;
        }

        @Override public void insertUpdate(DocumentEvent e) { updateSuggestions(); }
        @Override public void removeUpdate(DocumentEvent e) { updateSuggestions(); }
        @Override public void changedUpdate(DocumentEvent e) {}

        private void updateSuggestions() {
            String input = field.getText().toLowerCase().trim();
            box.removeAllItems();
            if (input.isEmpty()) return;

            // Use insertion-order set to deduplicate while preserving rank
            LinkedHashSet<String> exact   = new LinkedHashSet<>();
            LinkedHashSet<String> prefix  = new LinkedHashSet<>();
            LinkedHashSet<String> partial = new LinkedHashSet<>();

            // Search by HSA ID
            for (Map.Entry<String, List<String>> entry : idMap.entrySet()) {
                String key = entry.getKey();
                for (String id : entry.getValue()) {
                    if (key.equals(input))        exact.add(id);
                    else if (key.startsWith(input)) prefix.add(id);
                    else if (key.contains(input))   partial.add(id);
                }
            }

            // Search by gene symbol
            for (Map.Entry<String, String> ge : geneToHsa.entrySet()) {
                String gene  = ge.getKey();  // lowercase symbol
                String hsaLc = ge.getValue(); // lowercase hsa id
                List<String> ids = lookupInIdMap(hsaLc);
                if (!ids.isEmpty()) {
                    if (gene.equals(input))        ids.forEach(exact::add);
                    else if (gene.startsWith(input)) ids.forEach(prefix::add);
                    else if (gene.contains(input))   ids.forEach(partial::add);
                }
            }

            for (String id : exact)   box.addItem(formatItem(id));
            for (String id : prefix)  box.addItem(formatItem(id));
            for (String id : partial) box.addItem(formatItem(id));
        }
    }

    private static final DateTimeFormatter TIME_FMT = DateTimeFormatter.ofPattern("HH:mm:ss");

    Icon runningIcon = createDotIcon(new Color(0, 180, 0));
    Icon idleIcon    = createDotIcon(new Color(100, 100, 100));
    Icon errorIcon   = createDotIcon(new Color(200, 50, 50));

    private void runTool(String username) {
        JTextArea terminal = new JTextArea();
        terminal.setEditable(false);
        JLabel stageLabel  = new JLabel("Initializing…");
        JLabel statusLabel = new JLabel("Request under process...");
        stageLabel.setIcon(runningIcon);
        statusLabel.setIcon(runningIcon);

        JFrame frame = createModernOutputWindow(terminal, stageLabel, statusLabel);
        frame.setVisible(true);

        new Thread(() -> {
            try {
                ProcessBuilder pb = new ProcessBuilder("./fusion", "-b", "output_script.txt");
                pb.redirectErrorStream(true);
                Process process = pb.start();
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    final String lineCopy  = line;
                    final String timestamp = "[" + LocalTime.now().format(TIME_FMT) + "] ";
                    final String stage     = detectStage(lineCopy);
                    SwingUtilities.invokeLater(() -> {
                        terminal.append(timestamp + lineCopy + "\n");
                        terminal.setCaretPosition(terminal.getDocument().getLength());
                        if (stage != null) {
                            stageLabel.setText(stage);
                            frame.setTitle("FuSION - " + stage);
                            statusLabel.setText(stage + "...");
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
                    int choice = JOptionPane.showConfirmDialog(frame,
                        "Do you want to run another experiment?", "Run Another?",
                        JOptionPane.YES_NO_OPTION);
                    if (choice == JOptionPane.YES_OPTION) new SessionFrame(username);
                });
            } catch (Exception e) {
                stageLabel.setIcon(errorIcon);
                stageLabel.setText("Execution Failed");
                SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(null, e.getMessage(), "Execution Error", JOptionPane.ERROR_MESSAGE));
            }
        }).start();
    }

    private String detectStage(String line) {
        line = line.trim();
        if (line.startsWith("exec mkdir")) return "Initializing workspace";
        if (line.startsWith("cudf"))        return "Processing fold-change data";
        if (line.startsWith("start"))       return "Loading mappings & metadata";
        if (line.startsWith("mff"))         return "Merging pathways";
        if (line.startsWith("wgx"))         return "Writing merged graph";
        if (line.startsWith("rgx"))         return "Reading graph XML";
        if (line.startsWith("fb_rch"))      return "Reachability pruning";
        if (line.startsWith("pathz3"))      return "Constraint solving (Z3)";
        if (line.contains("gnuplot"))       return "Generating plots";
        if (line.equals("exit"))            return "Finalizing";
        return null;
    }

    private JFrame createModernOutputWindow(JTextArea terminal, JLabel stageLabel, JLabel statusLabel) {
        JFrame frame = new JFrame("FuSION - Initializing");
        frame.setSize(Theme.scale(1100), Theme.scale(550));
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel header = new JPanel(new BorderLayout());
        header.setBackground(new Color(30, 30, 30));
        header.setBorder(BorderFactory.createEmptyBorder(Theme.scale(8), Theme.scale(12), Theme.scale(8), Theme.scale(12)));
        stageLabel.setForeground(Color.WHITE);
        stageLabel.setFont(Theme.title(15));
        header.add(stageLabel, BorderLayout.WEST);

        terminal.setBackground(new Color(18, 18, 18));
        terminal.setForeground(new Color(220, 220, 220));
        terminal.setCaretColor(Color.WHITE);
        terminal.setFont(Theme.mono(13));
        terminal.setBorder(BorderFactory.createEmptyBorder(Theme.scale(8), Theme.scale(8), Theme.scale(8), Theme.scale(8)));
        terminal.setLineWrap(true);
        terminal.setWrapStyleWord(true);

        JScrollPane scrollPane = new JScrollPane(terminal);
        scrollPane.setBorder(null);

        JPanel statusBar = new JPanel(new BorderLayout());
        statusBar.setBackground(new Color(30, 30, 30));
        statusBar.setBorder(BorderFactory.createEmptyBorder(Theme.scale(6), Theme.scale(10), Theme.scale(6), Theme.scale(10)));
        statusLabel.setForeground(new Color(180, 180, 180));
        statusLabel.setFont(Theme.body(12));
        statusBar.add(statusLabel, BorderLayout.WEST);

        frame.setLayout(new BorderLayout());
        frame.add(header,     BorderLayout.NORTH);
        frame.add(scrollPane, BorderLayout.CENTER);
        frame.add(statusBar,  BorderLayout.SOUTH);
        return frame;
    }

    private static Icon createDotIcon(Color color) {
        return new Icon() {
            @Override public int getIconWidth()  { return 10; }
            @Override public int getIconHeight() { return 10; }
            @Override public void paintIcon(Component c, Graphics g, int x, int y) {
                g.setColor(color);
                g.fillOval(x, y, 10, 10);
            }
        };
    }

    private static JComboBox<String> makeDarkCombo(int prefW, int prefH) {
        JComboBox<String> c = new JComboBox<>();
        c.setUI(new javax.swing.plaf.basic.BasicComboBoxUI());
        c.setBackground(Theme.BG_CARD);
        c.setForeground(Theme.TEXT_DARK);
        c.setFont(Theme.body(13));
        c.setPreferredSize(new Dimension(prefW, prefH));
        c.setForeground(Color.BLACK);
        c.setBackground(Color.WHITE);
        c.setRenderer(new DefaultListCellRenderer() {
            @Override
            public Component getListCellRendererComponent(JList<?> list, Object val,
                    int idx, boolean sel, boolean focus) {
                super.getListCellRendererComponent(list, val, idx, sel, focus);
                if (idx == -1) {
                    // closed combo display area — always black on white
                    setBackground(Color.WHITE);
                    setForeground(Color.BLACK);
                } else {
                    setBackground(sel ? Theme.PRIMARY : Color.WHITE);
                    setForeground(sel ? Color.WHITE : Color.BLACK);
                }
                setBorder(BorderFactory.createEmptyBorder(3, 8, 3, 8));
                return this;
            }
        });
        return c;
    }

    private JFileChooser openInWorkDir() {
        String wd = user.getWorkingDirectory();
        return (wd != null && !wd.trim().isEmpty()) ? new JFileChooser(new File(wd)) : new JFileChooser();
    }
}
