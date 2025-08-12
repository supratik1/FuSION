
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import javax.swing.*;

public class XMLSelection extends RoundedPanel {

    private File txtFile;
    private File xmlFile;

    // Replace your current XMLSelection constructor code with this:
    public XMLSelection(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
        setBackground(Color.WHITE);

        HeaderPanel header = new HeaderPanel(user.getUsername());
        add(header, BorderLayout.NORTH);

        JLabel title = new JLabel("Add XML files", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 34));
        title.setForeground(Color.BLACK);
        // Remove purple background on title to keep it clean
        title.setBackground(Color.WHITE);

        JPanel centerPanel = new JPanel();
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        centerPanel.setBackground(Color.WHITE);  // changed from purple
        centerPanel.setBorder(BorderFactory.createEmptyBorder(30, 60, 60, 60));

        centerPanel.add(title);

        // Radio buttons to choose which file to enable
        JLabel chooseLabel = new JLabel("Which file do you want to choose?");
        chooseLabel.setFont(new Font("Arial", Font.BOLD, 24));
        chooseLabel.setForeground(Color.BLACK);

        JRadioButton chooseFile1Radio = new JRadioButton("List of KEGG XML Files to Merge");
        chooseFile1Radio.setOpaque(false);
        chooseFile1Radio.setForeground(Color.BLACK); // set text black

        JRadioButton chooseFile2Radio = new JRadioButton("XML File of Merged KEGG Pathways");
        chooseFile2Radio.setOpaque(false);
        chooseFile2Radio.setForeground(Color.BLACK); // set text black

        ButtonGroup group = new ButtonGroup();
        group.add(chooseFile1Radio);
        group.add(chooseFile2Radio);

        // Components for file 1
        JLabel label1 = new JLabel("Select list of KEGG XML Files to Merge:");
        label1.setFont(new Font("Arial", Font.BOLD, 20));
        label1.setForeground(Color.BLACK);

        JButton chooseButton1 = new JButton("Choose");
        chooseButton1.setEnabled(false);
        chooseButton1.setForeground(Color.WHITE);
        chooseButton1.setBackground(Color.BLACK); 
        chooseButton1.setFocusPainted(false);
        chooseButton1.setOpaque(true);

        JLabel fileNameLabel1 = new JLabel();
        if (user.getTxtFile() != null && !user.getTxtFile().equals("")) {
            File txt = new File(user.getTxtFile());
            fileNameLabel1.setText("Selected: " + txt.getName());
            chooseFile1Radio.setSelected(true);
            chooseButton1.setEnabled(true);
        } else {
            fileNameLabel1.setText("No file Selected");
        }
        fileNameLabel1.setForeground(Color.BLACK);

        // Components for file 2
        JLabel label2 = new JLabel("Select XML File of Merged KEGG Pathways:");
        label2.setFont(new Font("Arial", Font.BOLD, 20));
        label2.setForeground(Color.BLACK);

        JButton chooseButton2 = new JButton("Choose");
        chooseButton2.setEnabled(false);
        chooseButton2.setForeground(Color.WHITE);
        chooseButton2.setBackground(Color.BLACK); 
        chooseButton2.setFocusPainted(false);
        chooseButton2.setOpaque(true);

        JLabel fileNameLabel2 = new JLabel();
        if (user.getXMLFile() != null && !user.getXMLFile().equals("")) {
            File txt = new File(user.getXMLFile());
            fileNameLabel2.setText("Selected: " + txt.getName());
            chooseFile2Radio.setSelected(true);
            chooseButton2.setEnabled(true);
        } else {
            fileNameLabel2.setText("No file Selected");
        }
        fileNameLabel2.setForeground(Color.BLACK);

        // File chooser listeners remain same, no UI change needed:
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

        // Enable/disable buttons according to radio button selection
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

        // Add components to center panel
        centerPanel.add(Box.createVerticalStrut(20));
        centerPanel.add(chooseLabel);
        centerPanel.add(Box.createVerticalStrut(20));
        centerPanel.add(chooseFile1Radio);
        centerPanel.add(chooseFile2Radio);
        centerPanel.add(Box.createVerticalStrut(30));
        centerPanel.add(label1);
        centerPanel.add(chooseButton1);
        centerPanel.add(fileNameLabel1);
        centerPanel.add(Box.createVerticalStrut(30));
        centerPanel.add(label2);
        centerPanel.add(chooseButton2);
        centerPanel.add(fileNameLabel2);
        centerPanel.add(Box.createVerticalStrut(30));

        // Node split threshold panel
        JPanel nodeSplitPanel = new JPanel();
        nodeSplitPanel.setOpaque(false);
        JLabel nodeSplitLabel = new JLabel("Threshold to split Nodes while merging");
        nodeSplitLabel.setFont(new Font("Arial", Font.BOLD, 20));
        nodeSplitLabel.setForeground(Color.BLACK);  // black text

        JTextField nodeSplitField = new JTextField(8);
        nodeSplitField.setText("" + user.getNodeSplitThreshold());

        JButton iButton = new JButton("i");
        iButton.setFont(new Font("Arial", Font.BOLD, 16));
        iButton.setBackground(new Color(70, 130, 180)); // soft blue
        iButton.setForeground(Color.WHITE);
        iButton.setPreferredSize(new Dimension(40, 20));
        iButton.setFocusPainted(false);
        iButton.setOpaque(true);

        nodeSplitPanel.add(iButton);
        nodeSplitPanel.add(nodeSplitLabel);
        nodeSplitPanel.add(nodeSplitField);

        centerPanel.add(nodeSplitPanel);

        add(centerPanel, BorderLayout.CENTER);

        // Button panel - keep white background and standard buttons
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Color.WHITE);

        RoundedButton nextButton = new RoundedButton("Next", 20, new Dimension(100, 40));
        
        nextButton.setBackground(new Color(100, 149, 237));
        nextButton.setForeground(Color.WHITE);
        nextButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        nextButton.setFocusPainted(false);

        RoundedButton goToSessions = new RoundedButton("Go to Sessions", 20, new Dimension(170, 40));
        goToSessions.setBackground(new Color(222, 129, 7));
        goToSessions.setForeground(Color.WHITE);
        goToSessions.setFont(new Font("Segoe UI", Font.BOLD, 18));
        goToSessions.setFocusPainted(false);

        RoundedButton prevButton = new RoundedButton("Prev", 20, new Dimension(100, 40));
        prevButton.setBackground(new Color(100, 149, 237));
        prevButton.setForeground(Color.WHITE);
        prevButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        prevButton.setFocusPainted(false);
        
        RoundedButton saveButton = new RoundedButton("Save", 20, new Dimension(100,40));
        saveButton.setBackground(new Color(5, 161, 59));
        saveButton.setForeground(Color.WHITE);
        saveButton.setFont(new Font("Segoe UI", Font.BOLD, 18));
        saveButton.setFocusPainted(false);

        
        nextButton.addActionListener(e -> {
            user.setNodeSplitThreshold(Integer.parseInt(nodeSplitField.getText()));
            ReachPathBoundCard reachBound = new ReachPathBoundCard(cardLayout, cardPanel, user);
            cardPanel.add(reachBound, "reachBound");
            cardLayout.show(cardPanel, "reachBound");
        });

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "fileSelect"));

        saveButton.addActionListener(e -> {
            user.setNodeSplitThreshold(Integer.parseInt(nodeSplitField.getText()));
            user.saveData();
        });

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(prevButton, BorderLayout.EAST);
                add(goToSessions, BorderLayout.WEST);
                setOpaque(false);
            }
        });

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(nextButton, BorderLayout.WEST);
                add(saveButton, BorderLayout.EAST);
                setOpaque(false);
            }
        });

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
