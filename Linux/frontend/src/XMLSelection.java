import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import javax.swing.*;

public class XMLSelection extends RoundedPanel {

    private File txtFile;
    private File xmlFile;

    public XMLSelection(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        JPanel centerPanel = new JPanel();
        centerPanel.setLayout(new BoxLayout(centerPanel, BoxLayout.Y_AXIS));
        centerPanel.setBackground(Theme.BG);
        centerPanel.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(30), Theme.scale(60), Theme.scale(40), Theme.scale(60)));

        JLabel title = new JLabel("Add XML files");
        title.setFont(Theme.title(26));
        title.setForeground(Theme.TEXT_DARK);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        centerPanel.add(title);
        centerPanel.add(Box.createVerticalStrut(Theme.scale(24)));

        // Radio selection
        JLabel chooseLabel = new JLabel("Which file do you want to choose?");
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

        // File 1
        JLabel label1 = new JLabel("List of KEGG XML Files to Merge:");
        label1.setFont(Theme.title(14));
        label1.setForeground(Theme.TEXT_DARK);

        RoundedButton chooseButton1 = Theme.navBtn("Choose TXT File", 150);
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

        // File 2
        JLabel label2 = new JLabel("XML File of Merged KEGG Pathways:");
        label2.setFont(Theme.title(14));
        label2.setForeground(Theme.TEXT_DARK);

        RoundedButton chooseButton2 = Theme.navBtn("Choose XML File", 150);
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

        // File chooser listeners
        chooseButton1.addActionListener((ActionEvent e) -> {
            JFileChooser fc = openInWorkDir(user);
            int result = fc.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                txtFile = fc.getSelectedFile();
                user.setTxtFile(txtFile.getAbsolutePath());
                fileNameLabel1.setText("Selected: " + txtFile.getName());
            }
        });

        chooseButton2.addActionListener((ActionEvent e) -> {
            JFileChooser fc = openInWorkDir(user);
            int result = fc.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                xmlFile = fc.getSelectedFile();
                user.setXMLFile(xmlFile.getAbsolutePath());
                fileNameLabel2.setText("Selected: " + xmlFile.getName());
            }
        });

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

        // Radio card
        JPanel radioCard = new JPanel();
        radioCard.setLayout(new BoxLayout(radioCard, BoxLayout.Y_AXIS));
        radioCard.setBackground(Theme.BG_CARD);
        radioCard.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(12), Theme.scale(16), Theme.scale(12), Theme.scale(16))));
        radioCard.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(120)));
        radioCard.setAlignmentX(Component.LEFT_ALIGNMENT);
        chooseLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        chooseFile1Radio.setAlignmentX(Component.LEFT_ALIGNMENT);
        chooseFile2Radio.setAlignmentX(Component.LEFT_ALIGNMENT);
        radioCard.add(chooseLabel);
        radioCard.add(Box.createVerticalStrut(Theme.scale(8)));
        radioCard.add(chooseFile1Radio);
        radioCard.add(Box.createVerticalStrut(Theme.scale(4)));
        radioCard.add(chooseFile2Radio);
        centerPanel.add(radioCard);
        centerPanel.add(Box.createVerticalStrut(Theme.scale(20)));

        // File 1 card
        JPanel file1Card = new JPanel(new BorderLayout(Theme.GAP_SM, 0));
        file1Card.setBackground(Theme.BG_CARD);
        file1Card.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(76)));
        file1Card.setAlignmentX(Component.LEFT_ALIGNMENT);
        file1Card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(16), Theme.scale(10), Theme.scale(16))));
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
        centerPanel.add(Box.createVerticalStrut(Theme.scale(12)));

        // File 2 card
        JPanel file2Card = new JPanel(new BorderLayout(Theme.GAP_SM, 0));
        file2Card.setBackground(Theme.BG_CARD);
        file2Card.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(76)));
        file2Card.setAlignmentX(Component.LEFT_ALIGNMENT);
        file2Card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(16), Theme.scale(10), Theme.scale(16))));
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
        centerPanel.add(Box.createVerticalStrut(Theme.scale(20)));

        // Node split threshold card
        JPanel nodeSplitCard = new JPanel(new BorderLayout(Theme.GAP_MD, 0));
        nodeSplitCard.setBackground(Theme.BG_CARD);
        nodeSplitCard.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(60)));
        nodeSplitCard.setAlignmentX(Component.LEFT_ALIGNMENT);
        nodeSplitCard.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(16), Theme.scale(10), Theme.scale(16))));

        JLabel nodeSplitLabel = new JLabel("Threshold to split Nodes while merging");
        nodeSplitLabel.setFont(Theme.title(14));
        nodeSplitLabel.setForeground(Theme.TEXT_DARK);

        JTextField nodeSplitField = new JTextField(6);
        nodeSplitField.setText("" + user.getNodeSplitThreshold());
        nodeSplitField.setFont(Theme.body(14));
        Theme.styleDarkField(nodeSplitField);
        nodeSplitField.setMargin(new Insets(Theme.scale(4), Theme.scale(8), Theme.scale(4), Theme.scale(8)));
        nodeSplitField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(0, 0, 0, 0)));

        JPanel nodeSplitRight = new JPanel(new FlowLayout(FlowLayout.RIGHT, Theme.GAP_SM, 0));
        nodeSplitRight.setOpaque(false);
        nodeSplitRight.add(nodeSplitField);

        nodeSplitCard.add(nodeSplitLabel, BorderLayout.CENTER);
        nodeSplitCard.add(nodeSplitRight, BorderLayout.EAST);
        centerPanel.add(nodeSplitCard);

        add(centerPanel, BorderLayout.CENTER);

        // Button panel
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton   = Theme.navBtn("Next »", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton   = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton   = Theme.successBtn("Save", 110);

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

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setOpaque(false);
        }});

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private static JFileChooser openInWorkDir(UserInput user) {
        if (user == null) return new JFileChooser();
        String wd = user.getWorkingDirectory();
        return (wd != null && !wd.trim().isEmpty()) ? new JFileChooser(new File(wd)) : new JFileChooser();
    }
}
