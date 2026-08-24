import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class SessionFrame extends JFrame {

    private CardLayout cardLayout;
    JPanel mainPanel;
    UserInput user;
    int index = -1;

    public SessionFrame(String username) {
        setTitle("FuSION");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(true);
        setSize(Theme.scale(1200), Theme.scale(750));
        setMinimumSize(new Dimension(Theme.scale(900), Theme.scale(550)));
        setUndecorated(true);
        setLocationRelativeTo(null);

        mainPanel = new JPanel();
        cardLayout = new CardLayout();
        mainPanel.setLayout(cardLayout);
        mainPanel.setBackground(Theme.BG);
        mainPanel.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));

        JPanel sessionPage = new JPanel(new BorderLayout());
        sessionPage.setBackground(Theme.BG);
        sessionPage.add(new HeaderPanel(username), BorderLayout.NORTH);
        sessionPage.add(new SessionSelectorPanel(username), BorderLayout.CENTER);

        mainPanel.add(sessionPage, "sessions");
        add(mainPanel);
        setContentPane(mainPanel);
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Session selector — rows with inline actions
    // ─────────────────────────────────────────────────────────────────────────
    public class SessionSelectorPanel extends JPanel {

        private final String username;
        private final List<RowEntry> rows = new ArrayList<>();
        private JPanel rowsContainer;
        private JLabel countLabel;
        private int selectedJsonIndex = -1;

        // Shared selection colours
        private static final Color SEL_BG  = new Color(59, 130, 246, 40);
        private static final Color NORM_BG = Theme.BG;
        private static final Color HOVER_BG = new Color(59, 130, 246, 18);

        private static class RowEntry {
            String name, rawTimestamp;
            int jsonIndex;
            JPanel panel;
            RoundedButton configBtn, submitBtn, deleteBtn;
        }

        public SessionSelectorPanel(String username) {
            this.username = username;
            setLayout(new BorderLayout(0, 0));
            setBackground(Theme.BG);
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(24), Theme.scale(60), Theme.scale(16), Theme.scale(60)));

            add(buildHeader(), BorderLayout.NORTH);

            rowsContainer = new JPanel();
            rowsContainer.setLayout(new BoxLayout(rowsContainer, BoxLayout.Y_AXIS));
            rowsContainer.setBackground(Theme.BG);

            JScrollPane scroll = new JScrollPane(rowsContainer);
            scroll.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));
            scroll.getViewport().setBackground(Theme.BG);
            scroll.getVerticalScrollBar().setUnitIncrement(Theme.scale(16));
            scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
            add(scroll, BorderLayout.CENTER);

            add(buildFooter(), BorderLayout.SOUTH);

            loadSessions();
        }

        // ── Header: title + search + new button ──────────────────────────────

        private JPanel buildHeader() {
            JPanel outer = new JPanel(new BorderLayout(0, 0));
            outer.setBackground(Theme.BG);
            outer.setBorder(BorderFactory.createEmptyBorder(0, 0, Theme.scale(14), 0));

            JPanel titleArea = new JPanel();
            titleArea.setLayout(new BoxLayout(titleArea, BoxLayout.Y_AXIS));
            titleArea.setBackground(Theme.BG);

            JLabel title = new JLabel("Your Sessions");
            title.setFont(Theme.title(22));
            title.setForeground(Theme.TEXT_DARK);
            title.setAlignmentX(LEFT_ALIGNMENT);

            JLabel sub = new JLabel("Select a session to configure or submit directly");
            sub.setFont(Theme.body(13));
            sub.setForeground(Theme.TEXT_MED);
            sub.setAlignmentX(LEFT_ALIGNMENT);

            titleArea.add(title);
            titleArea.add(Box.createVerticalStrut(Theme.scale(3)));
            titleArea.add(sub);

            JPanel controls = new JPanel(new FlowLayout(FlowLayout.RIGHT, Theme.GAP_SM, 0));
            controls.setBackground(Theme.BG);

            JTextField searchField = buildSearchField();
            RoundedButton newBtn = Theme.navBtn("+ New Session", 160);
            newBtn.addActionListener(e -> promptNewSession());

            controls.add(searchField);
            controls.add(newBtn);

            outer.add(titleArea, BorderLayout.CENTER);
            outer.add(controls, BorderLayout.EAST);
            return outer;
        }

        private JTextField buildSearchField() {
            JTextField f = new JTextField(20);
            f.setFont(Theme.body(13));
            f.setBackground(Theme.BG_CARD);
            f.setForeground(Theme.TEXT_MED);
            f.setCaretColor(Theme.PRIMARY);
            f.setText("Search sessions…");
            f.setMargin(new Insets(Theme.scale(7), Theme.scale(11), Theme.scale(7), Theme.scale(11)));
            f.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));

            f.addFocusListener(new FocusAdapter() {
                @Override public void focusGained(FocusEvent e) {
                    if (f.getText().equals("Search sessions…")) {
                        f.setText("");
                        f.setForeground(Theme.TEXT_DARK);
                    }
                }
                @Override public void focusLost(FocusEvent e) {
                    if (f.getText().trim().isEmpty()) {
                        f.setText("Search sessions…");
                        f.setForeground(Theme.TEXT_MED);
                    }
                }
            });

            f.getDocument().addDocumentListener(new DocumentListener() {
                @Override public void insertUpdate(DocumentEvent e)  { filterRows(f.getText()); }
                @Override public void removeUpdate(DocumentEvent e)  { filterRows(f.getText()); }
                @Override public void changedUpdate(DocumentEvent e) { filterRows(f.getText()); }
            });

            return f;
        }

        // ── Footer: count + log out ───────────────────────────────────────────

        private JPanel buildFooter() {
            JPanel fp = new JPanel(new BorderLayout());
            fp.setBackground(Theme.BG);
            fp.setBorder(BorderFactory.createEmptyBorder(Theme.scale(11), 0, 0, 0));

            countLabel = new JLabel("Showing 0 sessions");
            countLabel.setFont(Theme.body(12));
            countLabel.setForeground(Theme.TEXT_LIGHT);

            RoundedButton logOut = Theme.dangerBtn("Log Out", 120);
            logOut.addActionListener(e -> {
                SwingUtilities.invokeLater(() -> new LoginPage().setVisible(true));
                dispose();
            });

            fp.add(countLabel, BorderLayout.WEST);
            fp.add(logOut, BorderLayout.EAST);
            return fp;
        }

        // ── Row builder ───────────────────────────────────────────────────────

        private JPanel makeRow(RowEntry entry) {
            JPanel row = new JPanel(new BorderLayout(Theme.GAP_SM, 0)) {
                @Override protected void paintComponent(Graphics g) {
                    super.paintComponent(g);
                    if (selectedJsonIndex == entry.jsonIndex) {
                        Graphics2D g2 = (Graphics2D) g.create();
                        g2.setColor(Theme.PRIMARY);
                        g2.fillRect(0, 0, Theme.scale(3), getHeight());
                        g2.dispose();
                    }
                }
            };
            row.setBackground(NORM_BG);
            row.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(58)));
            row.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
                BorderFactory.createEmptyBorder(Theme.scale(6), Theme.scale(16), Theme.scale(6), Theme.scale(12))));

            // ── Letter badge ──
            JComponent badge = new JComponent() {
                @Override protected void paintComponent(Graphics g) {
                    Graphics2D g2 = (Graphics2D) g.create();
                    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    boolean sel = (selectedJsonIndex == entry.jsonIndex);
                    g2.setColor(sel ? new Color(59, 130, 246, 50) : new Color(59, 130, 246, 22));
                    g2.fillRoundRect(0, 0, getWidth(), getHeight(), 9, 9);
                    g2.setColor(sel ? Theme.PRIMARY : new Color(59, 130, 246, 160));
                    g2.setFont(Theme.title(13));
                    FontMetrics fm = g2.getFontMetrics();
                    String letter = entry.name.isEmpty() ? "?" : String.valueOf(entry.name.charAt(0)).toUpperCase();
                    int tx = (getWidth()  - fm.stringWidth(letter)) / 2;
                    int ty = (getHeight() + fm.getAscent() - fm.getDescent()) / 2;
                    g2.drawString(letter, tx, ty);
                    g2.dispose();
                }
                @Override public Dimension getPreferredSize() { return new Dimension(Theme.scale(36), Theme.scale(36)); }
                @Override public Dimension getMinimumSize()   { return getPreferredSize(); }
                @Override public Dimension getMaximumSize()   { return getPreferredSize(); }
            };
            badge.setOpaque(false);

            JPanel badgeWrap = new JPanel(new GridBagLayout());
            badgeWrap.setOpaque(false);
            badgeWrap.add(badge);

            // ── Info ──
            JPanel info = new JPanel();
            info.setLayout(new BoxLayout(info, BoxLayout.Y_AXIS));
            info.setOpaque(false);

            JLabel nameLabel = new JLabel(entry.name);
            nameLabel.setFont(Theme.title(13));
            nameLabel.setForeground(Theme.TEXT_DARK);
            nameLabel.setAlignmentX(LEFT_ALIGNMENT);

            JLabel dateLabel = new JLabel(formatTimestamp(entry.rawTimestamp));
            dateLabel.setFont(Theme.body(11));
            dateLabel.setForeground(Theme.TEXT_MED);
            dateLabel.setAlignmentX(LEFT_ALIGNMENT);

            info.add(nameLabel);
            info.add(Box.createVerticalStrut(Theme.scale(2)));
            info.add(dateLabel);

            // ── Action buttons (hidden unless selected) ──
            JPanel actions = new JPanel(new FlowLayout(FlowLayout.RIGHT, Theme.GAP_SM, 0));
            actions.setOpaque(false);

            entry.deleteBtn = Theme.warningBtn("Delete", 90);
            entry.configBtn = Theme.navBtn("Configure »", 140);
            entry.submitBtn = Theme.successBtn("▶  Submit", 120);

            entry.deleteBtn.setVisible(false);
            entry.configBtn.setVisible(false);
            entry.submitBtn.setVisible(false);

            entry.deleteBtn.addActionListener(e -> deleteSession(entry.jsonIndex));
            entry.configBtn.addActionListener(e -> openConfigure(username, entry.jsonIndex));
            entry.submitBtn.addActionListener(e -> openSubmit(username, entry.jsonIndex));

            actions.add(entry.deleteBtn);
            actions.add(entry.configBtn);
            actions.add(entry.submitBtn);

            row.add(badgeWrap, BorderLayout.WEST);
            row.add(info, BorderLayout.CENTER);
            row.add(actions, BorderLayout.EAST);

            // ── Click / hover ──
            MouseAdapter ma = new MouseAdapter() {
                @Override public void mouseClicked(MouseEvent e) { selectRow(entry.jsonIndex); }
                @Override public void mouseEntered(MouseEvent e) {
                    if (selectedJsonIndex != entry.jsonIndex) row.setBackground(HOVER_BG);
                }
                @Override public void mouseExited(MouseEvent e) {
                    if (selectedJsonIndex != entry.jsonIndex) row.setBackground(NORM_BG);
                }
            };
            row.addMouseListener(ma);
            info.addMouseListener(ma);
            badge.addMouseListener(ma);

            entry.panel = row;
            return row;
        }

        // ── Selection ─────────────────────────────────────────────────────────

        private void selectRow(int jsonIndex) {
            int prev = selectedJsonIndex;
            selectedJsonIndex = jsonIndex;
            index = jsonIndex;

            for (RowEntry e : rows) {
                boolean sel = (e.jsonIndex == jsonIndex);
                boolean prevSel = (e.jsonIndex == prev);
                if (sel || prevSel) {
                    e.panel.setBackground(sel ? SEL_BG : NORM_BG);
                    e.configBtn.setVisible(sel);
                    e.submitBtn.setVisible(sel);
                    e.deleteBtn.setVisible(sel);
                    e.panel.revalidate();
                    e.panel.repaint();
                }
            }
        }

        // ── Filter ────────────────────────────────────────────────────────────

        private void filterRows(String text) {
            String q = text.equalsIgnoreCase("Search sessions…") ? "" : text.toLowerCase().trim();
            int shown = 0;
            for (RowEntry e : rows) {
                boolean match = q.isEmpty() || e.name.toLowerCase().contains(q);
                e.panel.setVisible(match);
                if (match) shown++;
            }
            countLabel.setText("Showing " + shown + " session" + (shown == 1 ? "" : "s"));
            rowsContainer.revalidate();
            rowsContainer.repaint();
        }

        // ── Session data ─────────────────────────────────────────────────────

        private void loadSessions() {
            rows.clear();
            rowsContainer.removeAll();
            selectedJsonIndex = -1;
            index = -1;

            try {
                String file = "frontend/sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);
                for (int i = 0; i < array.length(); i++) {
                    JSONObject obj = array.getJSONObject(i);
                    RowEntry entry = new RowEntry();
                    entry.name         = obj.getString("sessionName");
                    entry.rawTimestamp = obj.getString("timestamp");
                    entry.jsonIndex    = i;
                    rows.add(entry);
                    rowsContainer.add(makeRow(entry));
                }
            } catch (IOException | JSONException e) {
                System.out.println("No sessions or error reading file.");
            }

            countLabel.setText("Showing " + rows.size() + " session" + (rows.size() == 1 ? "" : "s"));
            rowsContainer.revalidate();
            rowsContainer.repaint();
        }

        private void deleteSession(int jsonIndex) {
            int result = JOptionPane.showConfirmDialog(this,
                "Delete this session permanently?", "Delete Session",
                JOptionPane.OK_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE);
            if (result != JOptionPane.OK_OPTION) return;

            try {
                String file = "frontend/sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);
                JSONArray updated = new JSONArray();
                for (int i = 0; i < array.length(); i++) {
                    if (i != jsonIndex) updated.put(array.get(i));
                }
                Files.writeString(Path.of(file), updated.toString(2));
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            loadSessions();
        }

        private void promptNewSession() {
            String name = JOptionPane.showInputDialog(this, "Session name:");
            if (name == null || name.trim().isEmpty()) return;
            for (RowEntry e : rows) {
                if (e.name.equals(name.trim())) {
                    JOptionPane.showMessageDialog(this, "A session with that name already exists.");
                    return;
                }
            }
            int newIdx = addNewSession(username, name.trim());
            openConfigure(username, newIdx);
        }

        private int addNewSession(String username, String name) {
            String timestamp = LocalDateTime.now().toString();
            File dir = new File("frontend/sessions");
            if (!dir.exists()) dir.mkdirs();
            try {
                String file = "frontend/sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);
                JSONObject obj = new JSONObject();
                obj.put("sessionName", name);
                obj.put("timestamp", timestamp);
                array.put(obj);
                Files.writeString(Path.of(file), array.toString(2));
                return array.length() - 1;
            } catch (Exception ex) {
                ex.printStackTrace();
                return 0;
            }
        }

        private void openConfigure(String username, int jsonIndex) {
            user = new UserInput("frontend/sessions/" + username + ".json", jsonIndex);
            user.setUsername(username);
            MainPanel main = new MainPanel(username, user, cardLayout, mainPanel);
            mainPanel.add(main, "Main");
            cardLayout.show(mainPanel, "Main");
        }

        private void openSubmit(String username, int jsonIndex) {
            user = new UserInput("frontend/sessions/" + username + ".json", jsonIndex);
            user.setUsername(username);
            Gui4Panel gui4 = new Gui4Panel(cardLayout, mainPanel, user);
            mainPanel.add(gui4, "gui4Direct");
            cardLayout.show(mainPanel, "gui4Direct");
        }

        // ── Helpers ───────────────────────────────────────────────────────────

        private String formatTimestamp(String raw) {
            try {
                String s = raw.length() >= 19 ? raw.substring(0, 19) : raw;
                LocalDateTime dt = LocalDateTime.parse(s);
                return dt.format(DateTimeFormatter.ofPattern("MMM d, yyyy  ·  HH:mm"));
            } catch (Exception e) {
                return raw;
            }
        }
    }
}
