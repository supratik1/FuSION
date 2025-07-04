
import java.awt.*;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.util.LinkedHashMap;
import java.util.Map;

import javax.swing.BorderFactory;
import javax.swing.DefaultListModel;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListSelectionModel;
import javax.swing.SwingUtilities;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class SessionFrame extends JFrame {

    
    UserInput user;
    int index=-1;

    public SessionFrame(String username) {
        // String fileName= "sessions/"+username+".json";
        // user = new UserInput(fileName, id);
        // user.setUsername(username);
        setTitle("FuSiON");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1400, 800);
        setMinimumSize(new Dimension(1400, 800));
        setMaximumSize(new Dimension(1400, 800));
        setResizable(false);
        setLocationRelativeTo(null);

        JPanel sessionPanel = SessionPanel(username);

        add(sessionPanel);

    }

    private JPanel SessionPanel(String username) {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBackground(new Color(138, 43, 226));

        JPanel topPanel = new JPanel(new FlowLayout());
        topPanel.setOpaque(false);
        JLabel jLabel1 = new JLabel("Welcome");
        jLabel1.setFont(new Font("Verdana", Font.PLAIN, 18));
        jLabel1.setForeground(Color.WHITE);
        JLabel jLabel4 = new JLabel(username);
        jLabel4.setFont(new Font("Verdana", Font.PLAIN, 16));
        jLabel4.setForeground(Color.WHITE);
        jLabel4.setIcon(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));
        //jLabel4.setBounds(340, 20, 300, 50);
        topPanel.add(jLabel1);
        topPanel.add(jLabel4);
        panel.add(topPanel, BorderLayout.NORTH);

        JPanel sessions = new SessionSelectorPanel(username);

        panel.add(sessions, BorderLayout.CENTER);

        JButton nxtButton = new JButton("next");
        nxtButton.addActionListener(e -> {

        });

        panel.add(nxtButton, BorderLayout.SOUTH);

        return panel;
    }

    public class SessionSelectorPanel extends JPanel {

        private DefaultListModel<String> listModel;
        private JList<String> sessionList;
        private LinkedHashMap<String, String> sessionMap = new LinkedHashMap<>();

        public SessionSelectorPanel(String username) {
            setLayout(new BorderLayout(10, 10));
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            setBackground(new Color(138, 43, 226));

            JLabel title = new JLabel("Previously Saved Sessions:");
            title.setForeground(Color.WHITE);
            title.setFont(new Font("Arial", Font.BOLD, 16));
            add(title, BorderLayout.NORTH);

            listModel = new DefaultListModel<>();

            sessionList = new JList<>(listModel);
            sessionList.setOpaque(false);
            sessionList.setBackground(new Color(0, 0, 0, 0));
            sessionList.setForeground(Color.WHITE);
            sessionList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            JScrollPane scrollPane = new JScrollPane(sessionList);
            scrollPane.setOpaque(false);
            scrollPane.getViewport().setOpaque(false);
            scrollPane.setForeground(Color.WHITE);
            add(scrollPane, BorderLayout.CENTER);

            JPanel buttons = new JPanel(new FlowLayout(FlowLayout.RIGHT));
            buttons.setOpaque(false);
            JButton continueBtn = new JButton("Continue Session");
            JButton newBtn = new JButton("New Session");
            buttons.add(continueBtn);
            buttons.add(newBtn);
            add(buttons, BorderLayout.SOUTH);

            loadSessions(username);

            continueBtn.addActionListener(e -> {
                index = sessionList.getSelectedIndex();
                System.out.println(index);
                if(index == -1){
                    JOptionPane.showMessageDialog(this, "Kindly Select a Session");
                    return;
                }

                // String timestamp = sessionMap.get(selected);
                String fileName = "sessions/" + username + ".json";
                user = new UserInput(fileName, index);
                user.setUsername(username);
                dispose();
                SwingUtilities.invokeLater(() -> new MainFrame(username, user).setVisible(true));

            });

            newBtn.addActionListener(e -> {
                
                String newSessionName = JOptionPane.showInputDialog(this, "Enter new session name:");
                if(sessionMap.containsKey(newSessionName)){
                    JOptionPane.showMessageDialog(this, "Session name already exists!");
                    return;
                }
                if (newSessionName != null && !newSessionName.trim().isEmpty() ) {
                    index= addNewSession(username, newSessionName.trim());
                }
                
                String fileName = "sessions/" + username + ".json";
                user = new UserInput(fileName, index);
                user.setUsername(username);

                dispose();
                SwingUtilities.invokeLater(() -> new MainFrame(username, user).setVisible(true));

            });
        }

        private void loadSessions(String username) {
            try {
                String file = "sessions/" + username + ".json";
                String json = Files.readString(Path.of(file));
                JSONArray array = new JSONArray(json);

                for (int i = 0; i < array.length(); i++) {
                    JSONObject obj = array.getJSONObject(i);
                    String name = obj.getString("sessionName");
                    String timestamp = obj.getString("timestamp");
                    listModel.addElement(name + "(" + timestamp + ")");
                    sessionMap.put(name, timestamp);
                }

            } catch (IOException | JSONException e) {
                System.out.println("No previous sessions or error reading file.");
            }
        }

        private int addNewSession(String username, String name) {
            String timestamp = LocalDateTime.now().toString();
            sessionMap.put(name, timestamp);
            listModel.addElement(name + "(" + timestamp + ")");

            // Keep only the last 5
            while (sessionMap.size() > 5) {
                String firstKey = sessionMap.keySet().iterator().next();
                sessionMap.remove(firstKey);
                listModel.removeElement(firstKey);
            }

            

            // Save to file
            JSONArray array = new JSONArray();
            for (Map.Entry<String, String> entry : sessionMap.entrySet()) {
                JSONObject obj = new JSONObject();

                obj.put("sessionName", entry.getKey());
                obj.put("timestamp", entry.getValue());
                array.put(obj);
            }

            String fileName = "sessions/" + username + ".json";

            try (FileWriter fw = new FileWriter(fileName)) {
                fw.write(array.toString(2)); // Pretty print with 2 spaces
            } catch (IOException e) {
                e.printStackTrace();
            }

            return sessionMap.size()-1;
        }

    }
}
