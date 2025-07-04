
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.LocalDateTime;
import java.util.LinkedHashMap;
import java.util.Map;

import javax.swing.*;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class MainFrame extends JFrame {

    private JLabel currDirectory;
    private JButton changeDirButton;
    private JPanel mainPanel;
    private CardLayout cardLayout;
    private JPanel homePanel;
    private JPanel fileSelectPanel;
    protected File selectedDirectory;
    //protected UserInput user;

    public MainFrame(String username, UserInput user) {
        // String fileName= "sessions/"+username+".json";
        // user = new UserInput(fileName, id);
        // user.setUsername(username);
        setTitle("FuSiON");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1400, 800);
        setSize(1400, 800);
        setMinimumSize(new Dimension(1400, 800));
        setMaximumSize(new Dimension(1400, 800));
        setResizable(false);
        setLocationRelativeTo(null);
        setLayout(cardLayout);
        initComponents(username, user);

    }

    private void initComponents(String username, UserInput user) {
        cardLayout = new CardLayout();
        mainPanel = new JPanel(cardLayout);

        homePanel = new JPanel(new BorderLayout());
        homePanel.setBackground(new Color(138, 43, 226));

        JPanel topPanel = new JPanel(new FlowLayout());
        topPanel.setOpaque(false);
        JLabel jLabel4 = new JLabel(username);
        jLabel4.setFont(new Font("Verdana", Font.PLAIN, 16));
        jLabel4.setForeground(Color.WHITE);
        jLabel4.setIcon(new ImageIcon(getClass().getResource("/icons8-male-user-50.png")));

        topPanel.add(jLabel4);
        homePanel.add(topPanel, BorderLayout.NORTH);

        JPanel bodyPanel = new JPanel();
        bodyPanel.setOpaque(false);
        bodyPanel.setLayout(new BoxLayout(bodyPanel, BoxLayout.Y_AXIS));
        bodyPanel.add(Box.createVerticalStrut(20));

        JLabel jLabel1 = new JLabel("Welcome to FuSiON");
        jLabel1.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel1.setFont(new Font("Verdana", Font.PLAIN, 36));
        jLabel1.setForeground(Color.WHITE);

        bodyPanel.add(jLabel1);
        bodyPanel.add(Box.createVerticalStrut(30));

        JLabel jLabel3 = new JLabel("Empowering your discovery process with computational insights to ");
        jLabel3.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel3.setFont(new Font("Verdana", Font.PLAIN, 14));
        jLabel3.setForeground(Color.WHITE);

        bodyPanel.add(jLabel3);

        JLabel jLabel2 = new JLabel("spotlight functionally significant actors");
        jLabel2.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel2.setFont(new Font("Verdana", Font.PLAIN, 14));
        jLabel2.setForeground(Color.WHITE);

        bodyPanel.add(jLabel2);
        bodyPanel.add(Box.createVerticalStrut(50));

        JLabel jLabel5 = new JLabel("Kindly Select your working Directory");
        jLabel5.setAlignmentX(Component.CENTER_ALIGNMENT);
        jLabel5.setFont(new Font("Segoe UI", Font.PLAIN, 24));
        jLabel5.setForeground(Color.WHITE);
        jLabel5.setHorizontalAlignment(SwingConstants.CENTER);

        bodyPanel.add(jLabel5);
        bodyPanel.add(Box.createVerticalStrut(20));

        JPanel showDirectory = new JPanel(new FlowLayout());
        showDirectory.setOpaque(false);
        showDirectory.setPreferredSize(new Dimension(650, 90));

        showDirectory.setBorder(BorderFactory.createMatteBorder(2, 2, 2, 2, Color.WHITE));

        JLabel jLabel6 = new JLabel("Current Directory :");
        jLabel6.setFont(new Font("Segoe UI", Font.PLAIN, 14));
        jLabel6.setForeground(Color.WHITE);

        showDirectory.add(jLabel6);
        

        selectedDirectory = new File(user.getWorkingDirectory());
        currDirectory = new JLabel(selectedDirectory.getAbsolutePath());
        currDirectory.setFont(new Font("Verdana", Font.BOLD, 14));
        currDirectory.setForeground(Color.WHITE);
        //currDirectory.setBounds(370, 320, 200, 30);
        showDirectory.add(currDirectory);
        

        bodyPanel.add(new JPanel(new FlowLayout()) {
            {
                add(showDirectory);
                setOpaque(false);
            }
        });

        changeDirButton = new JButton("Change Directory");
        changeDirButton.setFont(new Font("Segoe UI", Font.BOLD, 16));
        changeDirButton.setBackground(new Color(144, 224, 239));
        changeDirButton.setForeground(Color.WHITE);

        changeDirButton.addActionListener(e->changeDirectory(user)) ;
        bodyPanel.add(new JPanel(new FlowLayout()) {
            {
                add(changeDirButton);
                setOpaque(false);
            }
        });
        JButton saveButton = new JButton("Save");
        bodyPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)) {
            {
                add(saveButton);
                setOpaque(false);
            }
        });

        homePanel.add(bodyPanel, BorderLayout.CENTER);

        JPanel ButtonPanel = new JPanel(new FlowLayout());
        ButtonPanel.setOpaque(false);
        JButton nextButton = new JButton("Next");
        //nextButton.setBounds(500, 390, 60,30);

        nextButton.addActionListener(e -> {
            // if(!cardExists("fileSelect")){
            
            fileSelectPanel = new FileSelectionCard(cardLayout, mainPanel, user);
            mainPanel.add(fileSelectPanel, "fileSelect");

            cardLayout.show(mainPanel, "fileSelect");
        });

        JButton prevButton = new JButton("Prev");

        
        saveButton.addActionListener(e -> {
            String fileName = "sessions/" + username + ".json";
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                System.out.println(user.getIndex());
                System.out.println(user.getWorkingDirectory());
                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

                sessionData.put("workingDirectory", user.getWorkingDirectory());
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

        //prevButton.setBounds(250, 390, 70,30);
        ButtonPanel.add(prevButton);
        ButtonPanel.add(nextButton);
       
        prevButton.addActionListener(e -> {
            // dispose();
            // SwingUtilities.invokeLater(() -> new LoginPage().setVisible(true));
            cardLayout.show(mainPanel, "session");
        });

        homePanel.add(ButtonPanel, BorderLayout.SOUTH);

        // Add both panels to the card layout
        mainPanel.add(homePanel, "home");

        setContentPane(mainPanel);
    }

    private void changeDirectory(UserInput user) {
        JFileChooser chooser = new JFileChooser(new File(System.getProperty("user.home")));
        chooser.setDialogTitle("Select Working Directory");
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        chooser.setAcceptAllFileFilterUsed(false);

        int result = chooser.showOpenDialog(this);
        if (result == JFileChooser.APPROVE_OPTION) {
            selectedDirectory = chooser.getSelectedFile();

            currDirectory.setText(selectedDirectory.getAbsolutePath());
        }
        user.setWorkingDirectory(selectedDirectory.getAbsolutePath());
    }

    // public void setCurrentDirectory(String path) {
    //     currDirectory.setText(path);
    // }
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> new MainFrame("", null).setVisible(true));
    }

}
