
import java.awt.*;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;

import javax.swing.*;

import org.json.JSONArray;
import org.json.JSONObject;

public class ReachPathBoundCard extends JPanel {

    public ReachPathBoundCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new GridLayout(3, 1, 20, 10));
        setBackground(new Color(138, 43, 226));

        // === Reach Path Bound Panel ===
        JPanel reachPanelWrapper = new JPanel(new BorderLayout());
        reachPanelWrapper.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        reachPanelWrapper.setBackground(new Color(40, 40, 40));

        JPanel reachPanel = new JPanel(new BorderLayout());
        reachPanel.setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.WHITE),
                "Maximum Signalling Path Length", 0, 0, null, Color.WHITE
        ));
        reachPanel.setBackground(new Color(138, 43, 226));

        JSlider reachSlider = new JSlider(0, 100, 50);

        JPanel reachBoundValue = new JPanel(new FlowLayout());
        reachBoundValue.setBackground(new Color(138, 43, 226));
        JLabel reachValue = new JLabel("Value: ");
        reachValue.setBackground(new Color(138, 43, 226));
        reachValue.setForeground(Color.WHITE);
        reachBoundValue.add(reachValue);

        JTextField reachValueField = new JTextField("50");
        reachValueField.setForeground(Color.WHITE);
        reachValueField.setBackground(new Color(138, 43, 226));
        reachValueField.setHorizontalAlignment(JTextField.CENTER);
        reachBoundValue.add(reachValueField);

        reachSlider.setPaintTicks(true);
        reachSlider.setPaintLabels(true);
        reachSlider.setMajorTickSpacing(10);
        reachSlider.setMinorTickSpacing(1);
        reachSlider.setForeground(Color.WHITE);
        reachSlider.setBackground(new Color(138, 43, 226));

        reachValueField.setPreferredSize(new Dimension(60, 25));
        reachPanel.add(reachSlider, BorderLayout.CENTER);
        reachPanel.add(reachBoundValue, BorderLayout.NORTH);

        reachPanelWrapper.add(reachPanel, BorderLayout.CENTER);

        // === Synchronize slider and text field ===
        reachSlider.addChangeListener(e -> {
            int value = reachSlider.getValue();
            reachValueField.setText(String.valueOf(value));
            user.setSignallingPathLength(value);
        });

        reachValueField.addActionListener(e -> {
            try {
                int value = Integer.parseInt(reachValueField.getText());
                if (value >= reachSlider.getMinimum() && value <= reachSlider.getMaximum()) {
                    reachSlider.setValue(value);
                    user.setSignallingPathLength(value);
                }
            } catch (NumberFormatException ignored) {
            }
        });

        // === Target Edges Panel ===
        JPanel targetPanelWrapper = new JPanel(new BorderLayout());
        targetPanelWrapper.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        targetPanelWrapper.setBackground(new Color(40, 40, 40));

        JPanel targetPanel = new JPanel(new GridLayout(4, 1));
        targetPanel.setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.WHITE),
                "Final Edges in pathways restricted to:", 0, 0, null, Color.WHITE
        ));
        targetPanel.setBackground(new Color(138, 43, 226));

        JRadioButton rb1 = new JRadioButton("Activation edges only");
        JRadioButton rb2 = new JRadioButton("Expression edges only");
        JRadioButton rb3 = new JRadioButton("No restriction");

        ButtonGroup group = new ButtonGroup();
        group.add(rb1);
        group.add(rb2);
        group.add(rb3);

        for (JRadioButton rb : new JRadioButton[]{rb1, rb2, rb3}) {
            rb.setBackground(new Color(138, 43, 226));
            rb.setForeground(Color.WHITE);
            targetPanel.add(rb);
        }

        targetPanelWrapper.add(targetPanel, BorderLayout.CENTER);
        // === Add to Main Panel ===
        reachPanelWrapper.setBackground(new Color(138, 43, 226));
        targetPanelWrapper.setBackground(new Color(138, 43, 226));
        add(reachPanelWrapper);
        add(targetPanelWrapper);

        JPanel configPanel = new JPanel(new FlowLayout());
        JLabel configLabel = new JLabel("Would you like to change backend solver configuration?");
        configLabel.setFont(new Font("Arial", Font.BOLD, 16));
        configLabel.setForeground(Color.WHITE);
        configLabel.setOpaque(false);
        configPanel.setOpaque(false);

        JButton configButton = new JButton("Configure");

        configPanel.add(configLabel);
        configPanel.add(configButton);

        configButton.addActionListener(e -> {
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Change Configuration", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(550, 450);
            dialog.setLocationRelativeTo(null);

            JPanel mainPanel = new JPanel(new GridLayout(6, 1));
            //mainPanel.add(configPanel("Node Split Threshold", 1));
            int[] solver = user.getSolverConfig();
            mainPanel.add(configPanel("Increment Solver Timeout", solver[0]));
            mainPanel.add(configPanel("Overall Solver Timeout", solver[1]));
            mainPanel.add(configPanel("Solutions to Count", solver[2]));
            mainPanel.add(configPanel("Solutions to Explore", solver[3]));

            JPanel buttonPanel = new JPanel(new FlowLayout());
            JButton okay = new JButton("Ok");
            JButton cancel = new JButton("Cancel");
            buttonPanel.add(okay);
            buttonPanel.add(cancel);

            mainPanel.add(buttonPanel);
            dialog.add(mainPanel);

            okay.addActionListener(ev -> {
                int arr[]= new int[4];
                int i=0;
                for (Component comp : mainPanel.getComponents()) {
                    if (comp instanceof JPanel panel) {
                        for (Component inner : panel.getComponents()) {
                            if (inner instanceof JTextField jTextField) {
                                String value = jTextField.getText();
                                // System.out.println("Found value: " + value);
                                // You can parse it or store it as needed
                                
                                try {
                                    arr[i]= Integer.parseInt(value);
                                } catch (Exception err) {
                                    JOptionPane.showMessageDialog(this, "Inavlid Inputs", "Error", JOptionPane.ERROR_MESSAGE);
                                }
                            }
                        }
                    }
                    i++;
                }
                user.setSolverConfig(arr);

                dialog.dispose();
            });
            cancel.addActionListener(ev -> {
                dialog.dispose();
            });

            dialog.setVisible(true);

        });

        JPanel footer = new JPanel();
        footer.setLayout(new BoxLayout(footer, BoxLayout.Y_AXIS));
        footer.add(configPanel);

        footer.add(Box.createVerticalStrut(60));
        footer.setOpaque(false);

        JPanel buttonPanel = new JPanel(new BorderLayout());
        buttonPanel.setBackground(new Color(138, 43, 226));
        JButton nextButton = new JButton("Next");
        nextButton.addActionListener(e -> {
            // XMLSelection xmlSelection = new XMLSelection(cardLayout, cardPanel);
            // cardPanel.add(xmlSelection , "xmlSelection");
            // cardLayout.show(cardPanel, "xmlSelection");
            int i=-1;

                if(rb1.isSelected()){
                    i=0;
                }else if(rb2.isSelected()){
                    i=1;
                }else if(rb3.isSelected()){
                    i=2;
                }

                if(i==-1){
                    JOptionPane.showMessageDialog(this, "Select restriction.");
                    return;
                }

            user.setEdgeRestriction(i);

             RelaxationBoundsPanel relaxationBoundsPanel= new RelaxationBoundsPanel(cardLayout, cardPanel, user);
             cardPanel.add(relaxationBoundsPanel, "relaxationPanel");
             cardLayout.show(cardPanel, "relaxationPanel");

        });

        JButton prevButton = new JButton("Prev");
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "xml"));

        JButton saveButton= new JButton("Save");

        saveButton.addActionListener(e->{
            String fileName = "sessions/" + user.getUsername() + ".json";
            // System.out.println(fileName);
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                // System.out.println(user.getIndex());
                // System.out.println(user.getLogFoldChangesFile());
                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());
                // System.out.println(user.getSignallingPathLength());
                int[] arr= user.getSolverConfig();
                // System.out.println(Arrays.toString(arr));

                
                sessionData.put("SignallingPath", user.getSignallingPathLength());
                sessionData.put("incSolv", arr[0]);
                sessionData.put("overSolv", arr[1]);
                sessionData.put("solCount", arr[2]);
                sessionData.put("solExplore", arr[3]);
                int i=-1;

                if(rb1.isSelected()){
                    i=0;
                }else if(rb2.isSelected()){
                    i=1;
                }else if(rb3.isSelected()){
                    i=2;
                }
                if(i==-1){
                    JOptionPane.showMessageDialog(this, "Select restriction.");
                    return;
                }

                user.setEdgeRestriction(i);
                // System.out.println(user.getEdgeRestriction());
                sessionData.put("EdgeRestriction", i);

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
        
        footer.add(buttonPanel);
        add(footer);
    }

    public JPanel configPanel(String text, int value) {
        JPanel panel = new JPanel(new FlowLayout());
        JLabel label = new JLabel(text);
        JTextField textField = new JTextField(10);
        textField.setText(String.valueOf(value));
        panel.add(label);
        panel.add(textField);

        return panel;
    }

    // Optional main method for testing
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            JFrame frame = new JFrame("Card: Reach Path Bound");
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setSize(500, 300);
            frame.add(new ReachPathBoundCard(null, null, null));
            frame.setLocationRelativeTo(null);
            frame.setVisible(true);
        });
    }
}
