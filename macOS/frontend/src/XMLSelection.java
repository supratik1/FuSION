
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
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        JPanel centerPanel = new JPanel();
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        centerPanel.setBackground(Theme.BG);
        centerPanel.setBorder(BorderFactory.createEmptyBorder(30, 60, 40, 60));

        JLabel title = new JLabel("Pathway XML Files");
        title.setFont(Theme.title(26));
        title.setForeground(Theme.TEXT_DARK);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        centerPanel.add(title);
        centerPanel.add(Box.createVerticalStrut(4));

        JLabel titleSub = new JLabel("Choose the source of KEGG pathway data");
        titleSub.setFont(Theme.body(14));
        titleSub.setForeground(Theme.TEXT_MED);
        titleSub.setAlignmentX(Component.CENTER_ALIGNMENT);
        centerPanel.add(titleSub);
        centerPanel.add(Box.createVerticalStrut(24));

        // Radio buttons selection section
        JLabel chooseLabel = new JLabel("Select input type:");
        chooseLabel.setFont(Theme.title(14));
        chooseLabel.setForeground(Theme.TEXT_DARK);

        JRadioButton chooseFile1Radio = new JRadioButton("List of KEGG XML Files to Merge");
        chooseFile1Radio.setOpaque(false);
        chooseFile1Radio.setFont(Theme.body(14));
        chooseFile1Radio.setForeground(Theme.TEXT_DARK);

        JRadioButton chooseFile2Radio = new JRadioButton("XML File of Merged KEGG Pathways");
        chooseFile2Radio.setOpaque(false);
        chooseFile2Radio.setFont(Theme.body(14));
        chooseFile2Radio.setForeground(Theme.TEXT_DARK);

        ButtonGroup group = new ButtonGroup();
        group.add(chooseFile1Radio);
        group.add(chooseFile2Radio);

        // Components for file 1
        JLabel label1 = new JLabel("List of KEGG XML Files to Merge:");
        label1.setFont(Theme.title(14));
        label1.setForeground(Theme.TEXT_DARK);

        RoundedButton chooseButton1 = Theme.navBtn("Choose", 110);
        chooseButton1.setEnabled(false);

        JLabel fileNameLabel1 = new JLabel();
        if (user.getTxtFile() != null && !user.getTxtFile().equals("")) {
            File txt = new File(user.getTxtFile());
            fileNameLabel1.setText(txt.getName());
            chooseFile1Radio.setSelected(true);
            chooseButton1.setEnabled(true);
        } else {
            fileNameLabel1.setText("No file selected");
        }
        fileNameLabel1.setFont(Theme.body(13));
        fileNameLabel1.setForeground(Theme.TEXT_MED);

        // Components for file 2
        JLabel label2 = new JLabel("XML File of Merged KEGG Pathways:");
        label2.setFont(Theme.title(14));
        label2.setForeground(Theme.TEXT_DARK);

        RoundedButton chooseButton2 = Theme.navBtn("Choose", 110);
        chooseButton2.setEnabled(false);

        JLabel fileNameLabel2 = new JLabel();
        if (user.getXMLFile() != null && !user.getXMLFile().equals("")) {
            File txt = new File(user.getXMLFile());
            fileNameLabel2.setText(txt.getName());
            chooseFile2Radio.setSelected(true);
            chooseButton2.setEnabled(true);
        } else {
            fileNameLabel2.setText("No file selected");
        }
        fileNameLabel2.setFont(Theme.body(13));
        fileNameLabel2.setForeground(Theme.TEXT_MED);

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

            user.setKegg("");
        });

        chooseFile2Radio.addActionListener(e -> {
            chooseButton1.setEnabled(false);
            chooseButton2.setEnabled(true);
            txtFile = null;
            user.setTxtFile(null);
            fileNameLabel1.setText("No file selected");

            user.setKegg(";;");
        });

        // Radio selection card
        JPanel radioCard = new JPanel();
        radioCard.setLayout(new BoxLayout(radioCard, BoxLayout.Y_AXIS));
        radioCard.setBackground(Theme.BG_CARD);
        radioCard.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(12, 16, 12, 16)));
        radioCard.setMaximumSize(new Dimension(620, 100));
        radioCard.setAlignmentX(Component.CENTER_ALIGNMENT);
        chooseLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        chooseFile1Radio.setAlignmentX(Component.LEFT_ALIGNMENT);
        chooseFile2Radio.setAlignmentX(Component.LEFT_ALIGNMENT);
        radioCard.add(chooseLabel);
        radioCard.add(Box.createVerticalStrut(8));
        radioCard.add(chooseFile1Radio);
        radioCard.add(Box.createVerticalStrut(4));
        radioCard.add(chooseFile2Radio);
        centerPanel.add(radioCard);
        centerPanel.add(Box.createVerticalStrut(20));

        // File 1 card
        JPanel file1Card = new JPanel(new BorderLayout(12, 0));
        file1Card.setBackground(Theme.BG_CARD);
        file1Card.setMaximumSize(new Dimension(620, 64));
        file1Card.setAlignmentX(Component.CENTER_ALIGNMENT);
        file1Card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));
        JPanel file1Info = new JPanel();
        file1Info.setLayout(new BoxLayout(file1Info, BoxLayout.Y_AXIS));
        file1Info.setOpaque(false);
        label1.setAlignmentX(Component.LEFT_ALIGNMENT);
        fileNameLabel1.setAlignmentX(Component.LEFT_ALIGNMENT);
        file1Info.add(label1);
        file1Info.add(fileNameLabel1);
        file1Card.add(file1Info, BorderLayout.CENTER);
        file1Card.add(chooseButton1, BorderLayout.EAST);
        centerPanel.add(file1Card);
        centerPanel.add(Box.createVerticalStrut(12));

        // File 2 card
        JPanel file2Card = new JPanel(new BorderLayout(12, 0));
        file2Card.setBackground(Theme.BG_CARD);
        file2Card.setMaximumSize(new Dimension(620, 64));
        file2Card.setAlignmentX(Component.CENTER_ALIGNMENT);
        file2Card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));
        JPanel file2Info = new JPanel();
        file2Info.setLayout(new BoxLayout(file2Info, BoxLayout.Y_AXIS));
        file2Info.setOpaque(false);
        label2.setAlignmentX(Component.LEFT_ALIGNMENT);
        fileNameLabel2.setAlignmentX(Component.LEFT_ALIGNMENT);
        file2Info.add(label2);
        file2Info.add(fileNameLabel2);
        file2Card.add(file2Info, BorderLayout.CENTER);
        file2Card.add(chooseButton2, BorderLayout.EAST);
        centerPanel.add(file2Card);
        centerPanel.add(Box.createVerticalStrut(20));

        // Node split threshold section
        JPanel nodeSplitCard = new JPanel(new BorderLayout(16, 0));
        nodeSplitCard.setBackground(Theme.BG_CARD);
        nodeSplitCard.setMaximumSize(new Dimension(620, 56));
        nodeSplitCard.setAlignmentX(Component.CENTER_ALIGNMENT);
        nodeSplitCard.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));

        JLabel nodeSplitLabel = new JLabel("Node Split Threshold");
        nodeSplitLabel.setFont(Theme.title(14));
        nodeSplitLabel.setForeground(Theme.TEXT_DARK);

        JTextField nodeSplitField = new JTextField(6);
        nodeSplitField.setText("" + user.getNodeSplitThreshold());
        nodeSplitField.setFont(Theme.body(14));
        nodeSplitField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(4, 8, 4, 8)));

        RoundedButton iButton = new RoundedButton("i", 14, new Dimension(28, 28));
        iButton.setFont(Theme.body(13));
        iButton.setBackground(Theme.PRIMARY);
        iButton.setForeground(Color.WHITE);
        iButton.setFocusPainted(false);
        iButton.setToolTipText("Threshold to split nodes while merging pathways");

        JPanel nodeSplitRight = new JPanel(new FlowLayout(FlowLayout.RIGHT, 8, 0));
        nodeSplitRight.setOpaque(false);
        nodeSplitRight.add(nodeSplitField);
        nodeSplitRight.add(iButton);

        nodeSplitCard.add(nodeSplitLabel, BorderLayout.CENTER);
        nodeSplitCard.add(nodeSplitRight, BorderLayout.EAST);
        centerPanel.add(nodeSplitCard);

        add(centerPanel, BorderLayout.CENTER);

        // Button panel - keep white background and standard buttons
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);

        
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
