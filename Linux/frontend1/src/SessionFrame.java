
import java.awt.*;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.LinkedHashMap;
import javax.swing.*;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class SessionFrame extends JFrame {

    private CardLayout cardLayout;
    JPanel mainPanel;
    UserInput user;
    int index = -1;

    public SessionFrame(String username) {
        setTitle("FuSiON");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(true);
        setSize(1200, 750);
        setMinimumSize(new Dimension(900, 550));
        setUndecorated(true);
        setLocationRelativeTo(null);

        mainPanel = new JPanel();
        cardLayout = new CardLayout();
        mainPanel.setLayout(cardLayout);
        mainPanel.setBackground(Theme.BG);
        mainPanel.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));

        RoundedPanel sessionPanel = SessionPanel(username);
        mainPanel.add(sessionPanel, "sessions");

        add(mainPanel);
        setContentPane(mainPanel);
    }

    private RoundedPanel SessionPanel(String username) {
        RoundedPanel panel = new RoundedPanel();
        panel.setLayout(new BorderLayout());
        panel.setBackground(Theme.BG_CARD);

        HeaderPanel header = new HeaderPanel(username);
        panel.add(header, BorderLayout.NORTH);

        JPanel sessions = new SessionSelectorPanel(username);
        panel.add(sessions, BorderLayout.CENTER);

        return panel;
    }

    public class SessionSelectorPanel extends JPanel {

        private DefaultListModel<String> listModel;
        private JList<String> sessionList;
        private LinkedHashMap<String, String> sessionMap = new LinkedHashMap<>();

        public SessionSelectorPanel(String username) {
            setLayout(new BorderLayout(10, 10));
            setBorder(BorderFactory.createEmptyBorder(24, 60, 16, 60));
            setBackground(Theme.BG_CARD);

            // --- Title section ---
            JLabel title = new JLabel("Your Sessions");
            title.setForeground(Theme.TEXT_DARK);
            title.setFont(Theme.title(22));

            JLabel titleSub = new JLabel("Select a session to continue or create a new one");
            titleSub.setFont(Theme.body(14));
            titleSub.setForeground(Theme.TEXT_MED);

            JPanel titlePanel = new JPanel();
            titlePanel.setLayout(new BoxLayout(titlePanel, BoxLayout.Y_AXIS));
            titlePanel.setBackground(Theme.BG_CARD);
            titlePanel.setBorder(BorderFactory.createEmptyBorder(0, 0, 16, 0));
            title.setAlignmentX(Component.LEFT_ALIGNMENT);
            titleSub.setAlignmentX(Component.LEFT_ALIGNMENT);
            titlePanel.add(title);
            titlePanel.add(Box.createVerticalStrut(4));
            titlePanel.add(titleSub);
            add(titlePanel, BorderLayout.NORTH);

            listModel = new DefaultListModel<>();
            sessionList = new JList<>(listModel);
            sessionList.setOpaque(true);
            sessionList.setBackground(Theme.BG_CARD);
            sessionList.setForeground(Theme.TEXT_DARK);
            sessionList.setFont(Theme.body(15));
            sessionList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            sessionList.setFixedCellHeight(40);
            sessionList.setBorder(BorderFactory.createEmptyBorder(4, 12, 4, 12));
            sessionList.setSelectionBackground(Theme.PRIMARY);
            sessionList.setSelectionForeground(Color.WHITE);
            sessionList.setFocusable(false);

            JScrollPane scrollPane = new JScrollPane(sessionList);
            scrollPane.setBackground(Theme.BG_CARD);
            scrollPane.getViewport().setBackground(Theme.BG_CARD);
            scrollPane.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));

            add(scrollPane, BorderLayout.CENTER);

            // Buttons
            JPanel buttons = new JPanel(new BorderLayout());
            buttons.setBackground(Theme.BG_CARD);
            buttons.setBorder(BorderFactory.createEmptyBorder(12, 0, 0, 0));

            RoundedButton continueBtn = Theme.navBtn("Configure →", 150);
            RoundedButton submitBtn   = Theme.successBtn("▶  Submit", 140);
            RoundedButton newBtn      = Theme.navBtn("New Session", 150);
            RoundedButton logOut      = Theme.dangerBtn("Log Out", 120);
            RoundedButton deleteBtn   = Theme.warningBtn("Delete", 120);

            JPanel leftBtns = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 0));
            leftBtns.setOpaque(false);
            leftBtns.add(logOut);

            JPanel rightBtns = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
            rightBtns.setOpaque(false);
            rightBtns.add(deleteBtn);
            rightBtns.add(continueBtn);
            rightBtns.add(submitBtn);
            rightBtns.add(newBtn);

            buttons.add(leftBtns, BorderLayout.WEST);
            buttons.add(rightBtns, BorderLayout.EAST);
            add(buttons, BorderLayout.SOUTH);

            loadSessions(username);

            continueBtn.addActionListener(e -> {
                index = sessionList.getSelectedIndex();
                if (index == -1) {
                    JOptionPane.showMessageDialog(this, "Kindly Select a Session");
                    return;
                }
                String fileName = "frontend/sessions/" + username + ".json";
                user = new UserInput(fileName, index);
                user.setUsername(username);
                MainPanel main = new MainPanel(username, user, cardLayout, mainPanel);
                mainPanel.add(main, "Main");
                cardLayout.show(mainPanel, "Main");
            });

            submitBtn.addActionListener(e -> {
                index = sessionList.getSelectedIndex();
                if (index == -1) {
                    JOptionPane.showMessageDialog(this, "Kindly Select a Session");
                    return;
                }
                String fileName = "frontend/sessions/" + username + ".json";
                user = new UserInput(fileName, index);
                user.setUsername(username);
                try {
                    new ScriptFile(user);
                } catch (Exception ex) {
                    JOptionPane.showMessageDialog(this,
                        "Error preparing script:\n" + ex.getMessage(),
                        "Error", JOptionPane.ERROR_MESSAGE);
                    return;
                }
                runFusion(username);
            });

            deleteBtn.addActionListener(e -> {
                index = sessionList.getSelectedIndex();
                if (index == -1) {
                    JOptionPane.showMessageDialog(this, "Kindly Select a Session");
                    return;
                }
                int result = JOptionPane.showConfirmDialog(
                        this,
                        "Are you sure you want to delete this session?",
                        "Delete Session",
                        JOptionPane.OK_CANCEL_OPTION,
                        JOptionPane.WARNING_MESSAGE
                );
                if (result == JOptionPane.OK_OPTION) {
                    try {
                        String file = "frontend/sessions/" + username + ".json";
                        String json = Files.readString(Path.of(file));
                        JSONArray array = new JSONArray(json);
                        JSONArray newArray = new JSONArray();
                        for (int i = 0; i < array.length(); i++) {
                            if (i != index) {
                                newArray.put(array.get(i));
                            }
                        }
                        Files.writeString(Path.of(file), newArray.toString(2));
                        loadSessions(username);
                    } catch (Exception er) {
                        er.printStackTrace();
                    }
                }
            });

            newBtn.addActionListener(e -> {
                String newSessionName = JOptionPane.showInputDialog(this, "Enter new session name:");
                if (sessionMap.containsKey(newSessionName)) {
                    JOptionPane.showMessageDialog(this, "Session name already exists!");
                    return;
                }
                if (newSessionName != null && !newSessionName.trim().isEmpty()) {
                    index = addNewSession(username, newSessionName.trim());
                }
                String fileName = "frontend/sessions/" + username + ".json";
                user = new UserInput(fileName, index);
                user.setUsername(username);
                MainPanel main = new MainPanel(username, user, cardLayout, mainPanel);
                mainPanel.add(main, "Main");
                cardLayout.show(mainPanel, "Main");
            });

            logOut.addActionListener(e -> {
                SwingUtilities.invokeLater(() -> new LoginPage().setVisible(true));
                dispose();
            });
        }

        private void loadSessions(String username) {
            listModel.clear();
            sessionMap.clear();
            try {
                String file = "frontend/sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);
                for (int i = 0; i < array.length(); i++) {
                    JSONObject obj = array.getJSONObject(i);
                    String name = obj.getString("sessionName");
                    String timestamp = obj.getString("timestamp");
                    listModel.addElement(name + " (" + timestamp + ")");
                    sessionMap.put(name, timestamp);
                }
            } catch (IOException | JSONException e) {
                System.out.println("No previous sessions or error reading file.");
            }
        }

        private int addNewSession(String username, String name) {
            String timestamp = LocalDateTime.now().toString();
            sessionMap.put(name, timestamp);
            listModel.addElement(name + " (" + timestamp + ")");
            // while (sessionMap.size() > 10) {
            //     String firstKey = sessionMap.keySet().iterator().next();
            //     sessionMap.remove(firstKey);
            //     listModel.removeElement(firstKey);
            // }
            String folderName = "frontend/sessions";
            File directory = new File(folderName);
            if (!directory.exists()) {
                directory.mkdirs();
            }
            try {
                String file = "frontend/sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);
                JSONObject obj = new JSONObject();
                obj.put("sessionName", name);
                obj.put("timestamp", timestamp);
                
                array.put(obj);
                Files.writeString(Path.of(file), array.toString(2));
                
                loadSessions(username);
            } catch (Exception er) {
                er.printStackTrace();
            }
            return sessionMap.size() - 1;
        }
    }


    private static final DateTimeFormatter TIME_FMT = DateTimeFormatter.ofPattern("HH:mm:ss");

    private void runFusion(String username) {
        JTextArea terminal = new JTextArea();
        terminal.setEditable(false);
        terminal.setBackground(new Color(18, 18, 18));
        terminal.setForeground(new Color(220, 220, 220));
        terminal.setCaretColor(Color.WHITE);
        terminal.setFont(new Font("JetBrains Mono", Font.PLAIN, 13));
        terminal.setBorder(BorderFactory.createEmptyBorder(8, 8, 8, 8));

        JLabel stageLabel  = new JLabel("Initializing…");
        JLabel statusLabel = new JLabel("Request under process...");
        Icon runIcon   = dotIcon(new Color(0, 180, 0));
        Icon idleIcon  = dotIcon(new Color(100, 100, 100));
        Icon errorIcon = dotIcon(new Color(200, 50, 50));
        stageLabel.setIcon(runIcon);
        statusLabel.setIcon(runIcon);

        JFrame frame = new JFrame("FuSION — Running");
        frame.setSize(1100, 550);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

        JPanel hdr = new JPanel(new BorderLayout());
        hdr.setBackground(new Color(30, 30, 30));
        hdr.setBorder(BorderFactory.createEmptyBorder(8, 12, 8, 12));
        stageLabel.setForeground(Color.WHITE);
        stageLabel.setFont(new Font("Segoe UI", Font.BOLD, 15));
        hdr.add(stageLabel, BorderLayout.WEST);

        JPanel statusBar = new JPanel(new BorderLayout());
        statusBar.setBackground(new Color(30, 30, 30));
        statusBar.setBorder(BorderFactory.createEmptyBorder(6, 10, 6, 10));
        statusLabel.setForeground(new Color(180, 180, 180));
        statusLabel.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        statusBar.add(statusLabel, BorderLayout.WEST);

        frame.setLayout(new BorderLayout());
        frame.add(hdr, BorderLayout.NORTH);
        frame.add(new JScrollPane(terminal), BorderLayout.CENTER);
        frame.add(statusBar, BorderLayout.SOUTH);
        frame.setVisible(true);

        new Thread(() -> {
            try {
                ProcessBuilder pb = new ProcessBuilder("./fusion", "-b", "output_script.txt");
                pb.redirectErrorStream(true);
                Process process = pb.start();
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) {
                    final String ln = line;
                    final String ts = "[" + LocalTime.now().format(TIME_FMT) + "] ";
                    final String stage = detectStage(ln);
                    SwingUtilities.invokeLater(() -> {
                        terminal.append(ts + ln + "\n");
                        terminal.setCaretPosition(terminal.getDocument().getLength());
                        if (stage != null) {
                            stageLabel.setText(stage);
                            frame.setTitle("FuSION — " + stage);
                            statusLabel.setText(stage + "...");
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
            } catch (Exception ex) {
                stageLabel.setIcon(errorIcon);
                stageLabel.setText("Execution Failed");
                SwingUtilities.invokeLater(() ->
                    JOptionPane.showMessageDialog(frame, ex.getMessage(),
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

    private static Icon dotIcon(Color color) {
        return new Icon() {
            public int getIconWidth()  { return 10; }
            public int getIconHeight() { return 10; }
            public void paintIcon(Component c, Graphics g, int x, int y) {
                g.setColor(color); g.fillOval(x, y, 10, 10);
            }
        };
    }
}


