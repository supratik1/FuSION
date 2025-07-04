
import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import org.json.JSONArray;
import org.json.JSONObject;
import java.nio.file.Files;
import java.nio.file.Path;


public class IdEntryPanel extends JPanel {

    private JTextField inputField1, inputField2, inputField3;
    private JComboBox<String> suggestions1, suggestions2, suggestions3;
    private Map<String, String> idMap = new HashMap<>(); // key: common name, value: ID
    private File idMappingFile;

    public IdEntryPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(new Color(138, 43, 226));

         String[] mappingFile = user.getMappingFile();
        JPanel center = new JPanel(null);
        center.setOpaque(false);

        JLabel label1 = new JLabel("Source hsa ID");
        label1.setForeground(Color.WHITE);
        label1.setBounds(50, 30, 200, 30);
        center.add(label1);

        inputField1 = new JTextField();
        inputField1.setBounds(260, 30, 200, 30);
        inputField1.setOpaque(false);
        inputField1.setForeground(Color.WHITE);
        inputField1.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.WHITE));
        center.add(inputField1);

        suggestions1 = new JComboBox<>();
        suggestions1.setBounds(470, 30, 300, 30);
        suggestions1.setEditable(false);
        center.add(suggestions1);

        JLabel label2 = new JLabel("Target hsa ID");
        label2.setBounds(50, 80, 200, 30);
        label2.setForeground(Color.WHITE);
        center.add(label2);

        inputField2 = new JTextField();
        inputField2.setBounds(260, 80, 200, 30);
        inputField2.setOpaque(false);
        inputField2.setForeground(Color.WHITE);
        inputField2.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.WHITE));
        center.add(inputField2);

        suggestions2 = new JComboBox<>();
        suggestions2.setBounds(470, 80, 300, 30);
        suggestions2.setEditable(false);
        center.add(suggestions2);

        JLabel label3 = new JLabel("Candidate Implicated Node hsa ID");
        label3.setBounds(50, 130, 200, 30);
        label3.setForeground(Color.WHITE);
        center.add(label3);

        inputField3 = new JTextField();
        inputField3.setBounds(260, 130, 200, 30);
        inputField3.setOpaque(false);
        inputField3.setForeground(Color.WHITE);
        inputField3.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.WHITE));
        center.add(inputField3);

        suggestions3 = new JComboBox<>();
        suggestions3.setBounds(470, 130, 300, 30);
        suggestions3.setEditable(false);
        center.add(suggestions3);

        JPanel hsaNot= new JPanel();
        JLabel hsaJLabel= new JLabel("HSA IDs not to be merged:");
        hsaJLabel.setForeground(Color.WHITE);
        JButton hsaButton= new JButton("Choose");
        hsaButton.addActionListener(e->{
            JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    File hsaNotmerged = chooser.getSelectedFile();
                    user.sethsaNotFile(hsaNotmerged.getAbsolutePath());
                    
                }
        });
        hsaNot.add(hsaJLabel);
        hsaNot.add(hsaButton);
        hsaNot.setBounds(30, 200, 500, 30);
        hsaNot.setOpaque(false);
        center.add(hsaNot);

        JLabel ques = new JLabel("Would you like to change the Mapping Files?");
        ques.setFont(new Font("Arial", Font.BOLD, 16));
        ques.setForeground(Color.WHITE);
        ques.setOpaque(false);
        ques.setBounds(50, 300, 500, 40);
        center.add(ques);

        JButton changeMapping = new JButton("Change Mapping File");
        changeMapping.setBounds(50, 350, 200, 30);
        center.add(changeMapping);

        add(center, BorderLayout.CENTER);

        // Default load
        idMappingFile = new File(mappingFile[0]); // Replace with your default
        if (idMappingFile.exists()) {
            loadMappingFile(idMappingFile);
        }

        inputField1.getDocument().addDocumentListener(new SuggestionUpdater(inputField1, suggestions1));
        inputField2.getDocument().addDocumentListener(new SuggestionUpdater(inputField2, suggestions2));
        inputField3.getDocument().addDocumentListener(new SuggestionUpdater(inputField3, suggestions3));

        changeMapping.addActionListener(e -> {

            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Change Mapping File", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(550, 450);
            dialog.setLocationRelativeTo(null);
            dialog.setVisible(true);

            JPanel panel= new JPanel(new GridLayout(1, 3));

            JButton hsaToGene= new JButton("Choose");
            JButton hsaToPath = new JButton("Choose");
            JButton interDB = new JButton("Choose");

            panel.add(new JPanel(new BorderLayout()){{
                add(new JLabel("HSA to Gene Symbol Map"), BorderLayout.CENTER);
                add(hsaToGene, BorderLayout.SOUTH);

            }});

            panel.add(new JPanel(new BorderLayout()){{
                add(new JLabel("HSA path to Path Name Map"), BorderLayout.CENTER);
                add(hsaToGene, BorderLayout.SOUTH);

            }});

            panel.add(new JPanel(new BorderLayout()){{
                add(new JLabel("Cross Database Map"), BorderLayout.CENTER);
                add(hsaToGene, BorderLayout.SOUTH);

            }});
            dialog.add(panel, BorderLayout.CENTER);

            JButton okButton = new JButton("OK");

            dialog.add(new JPanel(){{
                add(okButton, BorderLayout.SOUTH);
        
            }});
            okButton.addActionListener(err->{
                user.setMappingFile(mappingFile);
                dialog.dispose();
            });

           

            hsaToGene.addActionListener(err->{
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    idMappingFile = chooser.getSelectedFile();
                    mappingFile[0]= idMappingFile.getAbsolutePath();
                    loadMappingFile(idMappingFile);
                    
                }
            });

            hsaToPath.addActionListener(err->{
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    File file= chooser.getSelectedFile();
                    mappingFile[1]= file.getAbsolutePath();

                }
            });

            interDB.addActionListener(err->{
                JFileChooser chooser = new JFileChooser();
                int result = chooser.showOpenDialog(this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    File file= chooser.getSelectedFile();
                    mappingFile[2]= file.getAbsolutePath();
                }
            });



            
        });

        JPanel buttonPanel = new JPanel(new BorderLayout());
        buttonPanel.setOpaque(false);

        JButton saveButton = new JButton("Save");

        buttonPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }});

        JButton prevButton = new JButton("Prev");
        prevButton.setBounds(30, 380, 80, 60);
        prevButton.addActionListener(e -> {

            cardLayout.show(cardPanel, "edgeEntry");
        });
        

        JButton nextButton = new JButton("Next");
        nextButton.setBounds(750, 380, 60, 60);
        nextButton.addActionListener(e -> {
            user.setMappingFile(mappingFile);
            try {
                new ScriptFile(user);
            } catch (Exception et) {
                et.printStackTrace();
            }
            
            // user.setSourceHsaID(suggestions1.getSelectedItem());
        });
        buttonPanel.add(new JPanel(){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }});
        add(buttonPanel, BorderLayout.SOUTH);

        saveButton.addActionListener(e->{
            user.setMappingFile(mappingFile);

            String fileName = "sessions/" + user.getUsername() + ".json";
            
            // try {
            //     String json = Files.readString(Path.of(fileName));
            //     JSONArray sessionsArray = new JSONArray(json);
            //     // Session to find

            //     JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

            //     // sessionData.put("SourceHsaID", sour);
            //     // sessionData.put("TargetHsaID", edges[1]);
            //     // sessionData.put("CandidateID", edges[2]);
            //     // sessionData.put("HsaNotMerged", edges[3]);
            //     // sessionData.put("HsaToGene", edges[3]);
            //     // sessionData.put("HsaToPath", edges[3]);
            //     // sessionData.put("InterDB", edges[3]);
            //     // String sessionName = sessionData.getString("sessionName");
            //     // String timestamp = sessionData.getString("timestamp");
            //     try (FileWriter fw = new FileWriter(fileName)) {
            //         fw.write(sessionsArray.toString(2)); // Pretty print with 2 spaces
            //     } catch (IOException er) {
            //         er.printStackTrace();
            //     }

            // } catch (Exception err) {
            //     err.printStackTrace();
            // }
        });
    }

    private void loadMappingFile(File file) {
        idMap.clear();
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty()) {
                    continue;
                }

                // Split on 2 or more whitespace characters
                String[] parts = line.split("\\s+");

                if (parts.length >= 2) {
                    // First part is ID, rest is common name (to support names with spaces)
                    String id = parts[0].trim();
                    String commonName = String.join(" ", Arrays.copyOfRange(parts, 1, parts.length)).toLowerCase();
                    idMap.put(commonName, id);
                }
            }
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(this, "Error reading mapping file: " + ex.getMessage());
        }
    }

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
            String input = field.getText().toLowerCase();
            box.removeAllItems();

            if (input.isEmpty()) {
                return;
            }

            List<String> exactMatches = new ArrayList<>();
            List<String> partialMatches = new ArrayList<>();

            for (Map.Entry<String, String> entry : idMap.entrySet()) {
                String commonName = entry.getKey();             // already lowercase
                String id = entry.getValue().toLowerCase();
                String display = entry.getValue() + " - " + commonName;

                // Exact match for ID or common name
                if (commonName.equals(input) || id.equals(input)) {
                    exactMatches.add(display);
                } // Partial match (substring match)
                else if (commonName.contains(input) || id.contains(input)) {
                    partialMatches.add(display);
                }
            }

            // Show exact matches first
            for (String match : exactMatches) {
                box.addItem(match);
            }
            // Then show partial matches
            for (String match : partialMatches) {
                box.addItem(match);
            }
        }

    }
}
