import java.awt.*;
import java.io.*;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.nio.file.Files;
import java.nio.file.Path;
import javax.swing.*;

public class Gui4Panel extends RoundedPanel {

    private final gui4 graphEditor;

    public Gui4Panel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Color.WHITE);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        writeDataFile(user);

        graphEditor = new gui4(true);

        // Auto-restore saved pipeline for this session
        if (!user.getPipelineData().isEmpty()) {
            graphEditor.loadPipelineFromString(user.getPipelineData());
        }

        Container content = graphEditor.getContentPane();
        add(content, BorderLayout.CENTER);

        // ── Bottom buttons ────────────────────────────────────────────────
        RoundedButton submitBtn     = Theme.successBtn("▶  Submit", 130);
        submitBtn.setFont(new Font("Segoe UI", Font.BOLD, 14));

        RoundedButton saveSessionBtn = Theme.navBtn("💾  Save Session", 150);
        saveSessionBtn.setFont(new Font("Segoe UI", Font.PLAIN, 13));

        RoundedButton prevButton    = Theme.navBtn("← Prev", 110);
        RoundedButton sessionBtn    = Theme.warningBtn("Sessions", 140);

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "edgeEntry"));
        sessionBtn.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        saveSessionBtn.addActionListener(e -> {
            user.setPipelineData(graphEditor.getPipelineAsString());
            user.saveData();
            JOptionPane.showMessageDialog(this,
                "Session saved — pipeline included.",
                "Saved", JOptionPane.INFORMATION_MESSAGE);
        });

        submitBtn.addActionListener(e -> {
            // Save pipeline to session before running
            user.setPipelineData(graphEditor.getPipelineAsString());
            user.saveData();
            try {
                new ScriptFile(user);
                runTool(user.getUsername());
            } catch (Exception ex) {
                ex.printStackTrace();
                JOptionPane.showMessageDialog(this,
                    "Error: " + ex.getMessage(), "Run Error", JOptionPane.ERROR_MESSAGE);
            }
        });

        JPanel leftBtns = new JPanel(new BorderLayout());
        leftBtns.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        leftBtns.add(sessionBtn, BorderLayout.WEST);
        leftBtns.add(prevButton, BorderLayout.EAST);
        leftBtns.setOpaque(false);

        JPanel rightBtns = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 10));
        rightBtns.setOpaque(false);
        rightBtns.add(saveSessionBtn);
        rightBtns.add(submitBtn);

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Color.WHITE);
        buttonPanel.add(leftBtns);
        buttonPanel.add(rightBtns);

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private static void writeDataFile(UserInput user) {
        try {
            StringBuilder sb = new StringBuilder();
            String[] edges   = user.getEdgesFile();
            String[] mapping = user.getMappingFile();
            int[]    relax   = user.getRelaxationBounds();
            int[]    solver  = user.getSolverConfig();

            appendEntry(sb, "log_fold_changes",  "file",    user.getLogFoldChangesFile());
            appendEntry(sb, "microarray_data",   "file",    user.getTxtFile());
            appendEntry(sb, "kegg_xml",          "file",    user.getXMLFile());
            appendEntry(sb, "additional_edges",  "file",    edges[0]);
            appendEntry(sb, "essential_edges",   "file",    edges[1]);
            appendEntry(sb, "avoid_edges",       "file",    edges[2]);
            appendEntry(sb, "relaxed_edges",     "file",    edges[3]);
            appendEntry(sb, "non_relaxed_edges", "file",    edges[4]);
            appendEntry(sb, "hsa_to_gene_map",   "file",    mapping[0]);
            appendEntry(sb, "hsa_path_map",      "file",    mapping[1]);
            appendEntry(sb, "cross_db_map",      "file",    mapping[2]);
            appendEntry(sb, "hsa_not_merged",    "file",    user.gethsaNotFile());

            sb.append("signalling_path_length,integer,").append(user.getSignallingPathLength()).append('\n');
            sb.append("node_relax_lower,integer,")      .append(relax[0]).append('\n');
            sb.append("node_relax_upper,integer,")      .append(relax[1]).append('\n');
            sb.append("edge_relax_lower,integer,")      .append(relax[2]).append('\n');
            sb.append("edge_relax_upper,integer,")      .append(relax[3]).append('\n');
            sb.append("node_split_threshold,integer,")  .append(user.getNodeSplitThreshold()).append('\n');
            sb.append("inc_solver_timeout,integer,")    .append(solver[0]).append('\n');
            sb.append("over_solver_timeout,integer,")   .append(solver[1]).append('\n');
            sb.append("solution_count,integer,")        .append(solver[2]).append('\n');
            sb.append("solution_explore,integer,")      .append(solver[3]).append('\n');
            sb.append("up_threshold,float,")  .append(user.getUpThreshold()).append('\n');
            sb.append("down_threshold,float,").append(user.getDownThreshold()).append('\n');

            Files.writeString(Path.of("Data_Coming_From_Frontend.txt"), sb.toString());
        } catch (IOException e) {
            System.err.println("Warning: could not write Data_Coming_From_Frontend.txt: " + e.getMessage());
        }
    }

    private static void appendEntry(StringBuilder sb, String name, String type, String value) {
        if (value != null && !value.trim().isEmpty()) {
            String displayVal = "file".equalsIgnoreCase(type) ? new java.io.File(value).getName() : value;
            sb.append(name).append(',').append(type).append(',').append(displayVal).append('\n');
        }
    }

    // ── Execution ─────────────────────────────────────────────────────────

    private static final DateTimeFormatter TIME_FMT = DateTimeFormatter.ofPattern("HH:mm:ss");
    private final Icon runningIcon = createDotIcon(new Color(0, 180, 0));
    private final Icon idleIcon    = createDotIcon(new Color(100, 100, 100));
    private final Icon errorIcon   = createDotIcon(new Color(200, 50, 50));

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
                    final String lineCopy = line;
                    final String timestamp = "[" + LocalTime.now().format(TIME_FMT) + "] ";
                    final String stage = detectStage(lineCopy);
                    SwingUtilities.invokeLater(() -> {
                        terminal.append(timestamp + lineCopy + "\n");
                        terminal.setCaretPosition(terminal.getDocument().getLength());
                        if (stage != null) {
                            stageLabel.setText(stage);
                            frame.setTitle("FuSION — " + stage);
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
                    frame.setTitle("FuSION — Completed");
                    int choice = JOptionPane.showConfirmDialog(frame,
                        "Do you want to run another experiment?",
                        "Run Another?", JOptionPane.YES_NO_OPTION);
                    if (choice == JOptionPane.YES_OPTION)
                        SwingUtilities.invokeLater(() -> new SessionFrame(username).setVisible(true));
                });
            } catch (Exception e) {
                stageLabel.setIcon(errorIcon);
                stageLabel.setText("Execution Failed");
                SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(frame, e.getMessage(),
                        "Execution Error", JOptionPane.ERROR_MESSAGE));
            }
        }).start();
    }

    private String detectStage(String line) {
        line = line.trim();
        if (line.startsWith("exec mkdir"))  return "Initializing workspace";
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
        JFrame frame = new JFrame("FuSION — Initializing");
        frame.setSize(1100, 550);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel hdr = new JPanel(new BorderLayout());
        hdr.setBackground(new Color(30, 30, 30));
        hdr.setBorder(BorderFactory.createEmptyBorder(8, 12, 8, 12));
        stageLabel.setForeground(Color.WHITE);
        stageLabel.setFont(new Font("Segoe UI", Font.BOLD, 15));
        hdr.add(stageLabel, BorderLayout.WEST);

        terminal.setBackground(new Color(18, 18, 18));
        terminal.setForeground(new Color(220, 220, 220));
        terminal.setCaretColor(Color.WHITE);
        terminal.setFont(new Font("JetBrains Mono", Font.PLAIN, 13));
        terminal.setBorder(BorderFactory.createEmptyBorder(8, 8, 8, 8));

        JScrollPane scrollPane = new JScrollPane(terminal);
        scrollPane.setBorder(null);

        JPanel statusBar = new JPanel(new BorderLayout());
        statusBar.setBackground(new Color(30, 30, 30));
        statusBar.setBorder(BorderFactory.createEmptyBorder(6, 10, 6, 10));
        statusLabel.setForeground(new Color(180, 180, 180));
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        statusBar.add(statusLabel, BorderLayout.WEST);

        frame.setLayout(new BorderLayout());
        frame.add(hdr,        BorderLayout.NORTH);
        frame.add(scrollPane, BorderLayout.CENTER);
        frame.add(statusBar,  BorderLayout.SOUTH);
        return frame;
    }

    private static Icon createDotIcon(Color color) {
        return new Icon() {
            public int getIconWidth()  { return 10; }
            public int getIconHeight() { return 10; }
            public void paintIcon(Component c, Graphics g, int x, int y) {
                g.setColor(color); g.fillOval(x, y, 10, 10);
            }
        };
    }
}
