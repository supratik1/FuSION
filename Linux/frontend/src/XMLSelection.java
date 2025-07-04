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

public class XMLSelection extends JPanel {

    private File txtFile;
    private File xmlFile;

    public XMLSelection(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout(10,10));
        setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
        setBackground(new Color(138, 43, 226));

        JLabel title = new JLabel("Add XML files", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 30));
        title.setForeground(Color.WHITE);
        title.setBackground(new Color(138, 43, 226));
        add(title, BorderLayout.NORTH);

        JPanel centerPanel= new JPanel();
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        centerPanel.setBackground(new Color(138, 43, 226));
        centerPanel.setBorder(BorderFactory.createEmptyBorder(60, 60, 60, 60));

        // Radio buttons to choose which file to enable
        JLabel chooseLabel = new JLabel("Which file do you want to choose?");
        chooseLabel.setFont(new Font("Arial", Font.BOLD, 20));
        chooseLabel.setForeground(Color.WHITE);
        JRadioButton chooseFile1Radio = new JRadioButton("List of KEGG XML Files to Merge");
        chooseFile1Radio.setForeground(Color.WHITE);
        chooseFile1Radio.setOpaque(false);
        JRadioButton chooseFile2Radio = new JRadioButton("XML File of Merged KEGG Pathways");
        chooseFile2Radio.setForeground(Color.WHITE);
        chooseFile2Radio.setOpaque(false);
        ButtonGroup group = new ButtonGroup();
        group.add(chooseFile1Radio);
        group.add(chooseFile2Radio);

        

        // Components for file 1
        JLabel label1 = new JLabel("Select list of KEGG XML Files to Merge:");
        label1.setFont(new Font("Arial", Font.BOLD, 18));

        label1.setForeground(Color.WHITE);
        JButton chooseButton1 = new JButton("Choose");
        chooseButton1.setForeground(Color.WHITE);
        chooseButton1.setBackground(Color.BLACK);
        JLabel fileNameLabel1 = new JLabel();
        if(user.getTxtFile()!=null && !user.getTxtFile().equals("")){
            fileNameLabel1.setText(user.getTxtFile());
        }else{
            fileNameLabel1.setText("No file Selected");
        }
        fileNameLabel1.setForeground(Color.WHITE);
        chooseButton1.setEnabled(false);
        

        // Components for file 2
        JLabel label2 = new JLabel("Select XML File of Merged KEGG Pathways:");
        label2.setFont(new Font("Arial", Font.BOLD, 18));

        label2.setForeground(Color.WHITE);
        JButton chooseButton2 = new JButton("Choose");
        chooseButton2.setForeground(Color.WHITE);
        chooseButton2.setBackground(Color.BLACK);
        JLabel fileNameLabel2 = new JLabel();
        System.out.println(user.getXMLFile());
        if(user.getXMLFile()!=null && !user.getXMLFile().equals("")){
            fileNameLabel2.setText(user.getXMLFile());
        }else{
            fileNameLabel2.setText("No file Selected");
        }
        fileNameLabel2.setForeground(Color.WHITE);
        chooseButton2.setEnabled(false);

        

        // File selection logic
        chooseButton1.addActionListener((ActionEvent e) -> {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                txtFile = fileChooser.getSelectedFile();
                user.setTxtFile(txtFile.getAbsolutePath());
                fileNameLabel1.setText("Selected: " + txtFile.getName());
            }
        });

        chooseButton2.addActionListener((ActionEvent e) -> {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                xmlFile = fileChooser.getSelectedFile();
                user.setXMLFile(xmlFile.getAbsolutePath());
                fileNameLabel2.setText("Selected: " + xmlFile.getName());
            }
        });

        // Enable corresponding file chooser on radio selection
        chooseFile1Radio.addActionListener(e -> {
            chooseButton1.setEnabled(true);
            chooseButton2.setEnabled(false);

            xmlFile = null;
            user.setXMLFile(null);
            fileNameLabel2.setText("No file selected");
        });

        chooseFile2Radio.addActionListener(e -> {
            chooseButton1.setEnabled(false);
            chooseButton2.setEnabled(true);

            txtFile = null;
            user.setTxtFile(null);
            fileNameLabel1.setText("No file selected");
        });

        // Layout
        centerPanel.add(chooseLabel);
        centerPanel.add(Box.createVerticalStrut(5));
        centerPanel.add(chooseFile1Radio);
        centerPanel.add(chooseFile2Radio);
        centerPanel.add(Box.createVerticalStrut(20));

        centerPanel.add(label1);
        centerPanel.add(chooseButton1);
        centerPanel.add(fileNameLabel1);
        centerPanel.add(Box.createVerticalStrut(20));

        centerPanel.add(label2);
        centerPanel.add(chooseButton2);
        centerPanel.add(fileNameLabel2);

        centerPanel.add(Box.createVerticalStrut(20));
        JPanel nodeSplitPanel= new JPanel();
        nodeSplitPanel.setOpaque(false);
        JLabel nodeSplitLabel= new JLabel("Threshold to split Nodes while merging");
        nodeSplitLabel.setFont(new Font("Arial", Font.BOLD, 16));
        nodeSplitLabel.setForeground(Color.WHITE);
        JTextField nodeSplitField= new JTextField(8);
        nodeSplitField.setText(""+user.getNodeSplitThreshold());

        JButton iButton = new JButton("i");
        iButton.setFont(new Font("Arial", Font.BOLD, 14));
        iButton.setBackground(Color.BLUE);
        iButton.setForeground(Color.WHITE);
        iButton.setPreferredSize(new Dimension(40, 20));
        nodeSplitPanel.add(iButton);
        nodeSplitPanel.add(nodeSplitLabel);
        nodeSplitPanel.add(nodeSplitField);

        centerPanel.add(nodeSplitPanel);


        add(centerPanel, BorderLayout.CENTER);

        JPanel buttonPanel = new JPanel(new BorderLayout());
        buttonPanel.setBackground(new Color(138, 43, 226));
        JButton nextButton = new JButton("Next");
        nextButton.addActionListener(e -> {
            // XMLSelection xmlSelection = new XMLSelection(cardLayout, cardPanel);
            // cardPanel.add(xmlSelection , "xmlSelection");
            // cardLayout.show(cardPanel, "xmlSelection");
            user.setNodeSplitThreshold(Integer.parseInt(nodeSplitField.getText()));
            ReachPathBoundCard reachBound = new ReachPathBoundCard(cardLayout, cardPanel, user);
            cardPanel.add(reachBound, "reachBound");
            cardLayout.show(cardPanel, "reachBound");

        });

        JButton prevButton = new JButton("Prev");
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "fileSelect"));


        JButton saveButton= new JButton("Save");
        saveButton.addActionListener(e -> {
            user.setNodeSplitThreshold(Integer.parseInt(nodeSplitField.getText()));
            String fileName = "sessions/" + user.getUsername() + ".json";
            // System.out.println(fileName);
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                // System.out.println(user.getIndex());
                // System.out.println(user.getLogFoldChangesFile());
                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());
                // System.out.println(user.getTxtFile()+":"+user.getXMLFile());
                sessionData.put("nodeSplitThreshold", user.getNodeSplitThreshold());
                sessionData.put("xml", user.getXMLFile());
                sessionData.put("txt", user.getTxtFile());
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

        buttonPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        buttonPanel.add(new JPanel(){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }}, BorderLayout.SOUTH);
        
        add(buttonPanel, BorderLayout.SOUTH);
    }

    // public File getSelectedFile1() {
    //     return selectedFile1;
    // }

    // public File getSelectedFile2() {
    //     return selectedFile2;
    // }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Card: Reach Path Bound");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(500, 300);
            frame.add(new XMLSelection(null, null, null));
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });
    }
}
