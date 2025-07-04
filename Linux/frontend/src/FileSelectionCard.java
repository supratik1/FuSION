import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import javax.swing.*;

import org.json.JSONArray;
import org.json.JSONObject;

public class FileSelectionCard extends JPanel {
    
    private JLabel selectedFileLabel;

    public FileSelectionCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        
        setLayout(new BorderLayout());
        setBackground(new Color(138, 43, 226));
        setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));

        JPanel top= new JPanel();

        JLabel jLabel1 = new JLabel("Select Log Fold Changes File");
        jLabel1.setFont(new Font("Verdana", Font.BOLD, 30));
        jLabel1.setForeground(Color.WHITE);

        top.add(jLabel1);
        top.setOpaque(false);
        
        add(top, BorderLayout.NORTH);

        JPanel body= new JPanel();
        body.setLayout(new BoxLayout(body, BoxLayout.Y_AXIS));
        body.setOpaque(false);
        body.add(Box.createVerticalStrut(100));

        JPanel jPanel1 = new JPanel();
        jPanel1.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.WHITE));
        jPanel1.setOpaque(false);
        jPanel1.setPreferredSize(new Dimension(500, 40));
        JButton nextButton = new JButton("Next");
        nextButton.setEnabled(false);
        
        selectedFileLabel = new JLabel();
        if(user.getLogFoldChangesFile()!=null && !"".equals(user.getLogFoldChangesFile())){
            selectedFileLabel.setText(user.getLogFoldChangesFile());
            nextButton.setEnabled(true);
        }else{
            selectedFileLabel.setText("No File Selected");
        }
        selectedFileLabel.setForeground(Color.WHITE);
        jPanel1.add(selectedFileLabel);
        JPanel jp= new JPanel();
        jp.add(jPanel1);
        jp.setOpaque(false);

        body.add(jp);

        // body.add(Box.createVerticalStrut(50));

        JButton chooseButton = new JButton("Choose File");
        chooseButton.setFont(new Font("Segoe UI", Font.BOLD, 16));
        chooseButton.setBackground(new Color(144, 224, 239));
        chooseButton.setForeground(Color.WHITE);
        JPanel choosePanel= new JPanel();
        choosePanel.add(chooseButton);
        choosePanel.setOpaque(false);
        body.add(choosePanel);

        add(body, BorderLayout.CENTER);

        

        chooseButton.addActionListener((ActionEvent e) -> {
            JFileChooser fileChooser = new JFileChooser(user.getWorkingDirectory());
            int result = fileChooser.showOpenDialog(FileSelectionCard.this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File selectedFile = fileChooser.getSelectedFile();
                selectedFileLabel.setText("Selected: " + selectedFile.getName());
                user.setLogFoldChangesFile(selectedFile.getAbsolutePath());
                nextButton.setEnabled(true);

                // Action to go to next card with file
                
            }
        });

        nextButton.addActionListener((ActionEvent evt)->{
                    

                    XMLSelection xmlFile= new XMLSelection(cardLayout, cardPanel, user);
                    cardPanel.add(xmlFile, "xml");
                    cardLayout.show(cardPanel, "xml");
        });
        
        JButton prevButton = new JButton("Prev");
        prevButton.addActionListener(e -> {
            cardLayout.show(cardPanel, "home");
        });
        JButton saveButton = new JButton("Save");

        JPanel ButtonPanel= new JPanel(new BorderLayout());
        ButtonPanel.setOpaque(false);

        ButtonPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        ButtonPanel.add(new JPanel(new FlowLayout()){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }},BorderLayout.SOUTH);

        add(ButtonPanel, BorderLayout.SOUTH);

        saveButton.addActionListener(e -> {
            String fileName = "sessions/" + user.getUsername() + ".json";
            System.out.println(fileName);
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                System.out.println(user.getIndex());
                System.out.println(user.getLogFoldChangesFile());
                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

                sessionData.put("logFold", user.getLogFoldChangesFile());
                // String sessionName = sessionData.getString("sessionName");
                // String timestamp = sessionData.getString("timestamp");
                try (FileWriter fw = new FileWriter(fileName)) {
                    fw.write(sessionsArray.toString(2)); // Pretty print with 2 spaces
                } catch (IOException er) {
                    er.printStackTrace();
                }

            } catch (Exception err) {
                err.printStackTrace();
            }

        });
        

        

       
    }
}

