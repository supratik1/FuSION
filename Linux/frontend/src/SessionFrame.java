
import java.awt.*;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
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
        setResizable(false);
        setSize(1400, 800);
        setMinimumSize(new Dimension(1400, 800));
        setMaximumSize(new Dimension(1400, 800));
        setUndecorated(true);
        setLocationRelativeTo(null);

        mainPanel = new JPanel();
        cardLayout = new CardLayout();
        mainPanel.setLayout(cardLayout);
        mainPanel.setBackground(new Color(230, 242, 242));
        mainPanel.setBorder(BorderFactory.createMatteBorder(10, 10, 10, 10, Color.WHITE));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(100, 100, 100, 100));

        RoundedPanel sessionPanel = SessionPanel(username);
        mainPanel.add(sessionPanel, "sessions");

        add(mainPanel);
        setContentPane(mainPanel);
    }

    private RoundedPanel SessionPanel(String username) {
        RoundedPanel panel = new RoundedPanel();
        panel.setLayout(new BorderLayout());
        panel.setBackground(Color.WHITE);

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
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            setBackground(Color.WHITE);

            // --- Title section ---
            JLabel title = new JLabel("Previously Saved Sessions:");
            title.setForeground(new Color(50, 50, 50)); // dark gray instead of pure black
            title.setFont(new Font("Segoe UI", Font.BOLD, 20)); // modern font and size

            JPanel titlePanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 15));
            titlePanel.setBackground(Color.WHITE);
            titlePanel.add(title);
            add(titlePanel, BorderLayout.NORTH);

// --- Modern styled list ---
            listModel = new DefaultListModel<>();
            sessionList = new JList<>(listModel);
            sessionList.setOpaque(true);
            sessionList.setBackground(Color.WHITE);
            sessionList.setForeground(Color.DARK_GRAY);
            sessionList.setFont(new Font("Segoe UI", Font.PLAIN, 16));
            sessionList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            sessionList.setFixedCellHeight(36); // taller rows
            sessionList.setBorder(BorderFactory.createEmptyBorder(5, 10, 5, 10));
            sessionList.setSelectionBackground(new Color(100, 149, 237)); // soft blue
            sessionList.setSelectionForeground(Color.WHITE);
            sessionList.setFocusable(false); // removes focus border

// --- Scroll pane as card ---
            JScrollPane scrollPane = new JScrollPane(sessionList);
            scrollPane.setBackground(Color.WHITE);
            scrollPane.getViewport().setBackground(Color.WHITE);
            scrollPane.setPreferredSize(new Dimension(1000, 220));
            scrollPane.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createLineBorder(new Color(200, 200, 200), 1, true), // rounded outer
                    BorderFactory.createEmptyBorder(5, 5, 5, 5) // padding inside
            ));

// --- Holder panel with subtle background ---
            JPanel scrollHolder = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 15));
            scrollHolder.setBackground(new Color(245, 247, 250)); // very light gray
            scrollHolder.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
            scrollHolder.add(scrollPane);

            add(new JPanel(){{
                add(scrollHolder);
                setOpaque(false);
                setBorder(BorderFactory.createEmptyBorder(0, 50 , 50, 50));
            }}, BorderLayout.CENTER);

            // Buttons
            RoundedPanel buttons = new RoundedPanel();
            buttons.setLayout(new GridLayout(1, 2));
            buttons.setBackground(Color.WHITE);

            RoundedButton continueBtn = styledButton("Continue Session");
            RoundedButton newBtn = styledButton("New Session");
            RoundedButton logOut = styledButton("Log Out");
            RoundedButton deleteBtn = styledButton("Delete Session");

            JPanel leftBtns = new JPanel(new FlowLayout(FlowLayout.LEFT));
            leftBtns.setOpaque(false);
            leftBtns.add(logOut);

            JPanel rightBtns = new JPanel(new FlowLayout(FlowLayout.RIGHT));
            rightBtns.setOpaque(false);
            rightBtns.add(continueBtn);
            rightBtns.add(deleteBtn);
            rightBtns.add(newBtn);

            buttons.add(leftBtns);
            buttons.add(rightBtns);
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

    // Common styling for buttons
    private RoundedButton styledButton(String text) {
        RoundedButton btn = new RoundedButton(text, 20, new Dimension(170, 40));
        btn.setBackground(new Color(100, 149, 237));
        btn.setForeground(Color.WHITE);
        btn.setFont(new Font("Segoe UI", Font.BOLD, 16));
        btn.setFocusPainted(false);
        return btn;
    }
}


