import java.awt.*;
import javax.swing.*;

public class ReachPathBoundCard extends RoundedPanel {

    public ReachPathBoundCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Color.WHITE);  // white background

        HeaderPanel header = new HeaderPanel(user.getUsername());

        add(header, BorderLayout.NORTH);

        JPanel body = new JPanel(new GridLayout(3, 1, 20, 10));
        body.setBackground(Color.WHITE);

        // === Reach Path Bound Panel ===
        JPanel reachPanelWrapper = new JPanel(new BorderLayout());
        reachPanelWrapper.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        reachPanelWrapper.setBackground(Color.WHITE);

        JPanel reachPanel = new JPanel(new BorderLayout());
        reachPanel.setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.BLACK),
                "Maximum Signalling Path Length", 0, 0, null, Color.BLACK
        ));
        reachPanel.setBackground(Color.WHITE);

        JSlider reachSlider = new JSlider(0, 100, user.getSignallingPathLength());
        reachSlider.setPaintTicks(true);
        reachSlider.setPaintLabels(true);
        reachSlider.setMajorTickSpacing(10);
        reachSlider.setMinorTickSpacing(1);
        reachSlider.setForeground(Color.BLACK);
        reachSlider.setBackground(Color.WHITE);

        JPanel reachBoundValue = new JPanel(new FlowLayout());
        reachBoundValue.setBackground(Color.WHITE);
        JLabel reachValue = new JLabel("Value: ");
        reachValue.setForeground(Color.BLACK);
        reachBoundValue.add(reachValue);

        JTextField reachValueField = new JTextField("" + user.getSignallingPathLength());
        reachValueField.setForeground(Color.BLACK);
        reachValueField.setBackground(Color.WHITE);
        reachValueField.setHorizontalAlignment(JTextField.CENTER);
        reachValueField.setPreferredSize(new Dimension(60, 25));
        reachBoundValue.add(reachValueField);

        reachPanel.add(reachBoundValue, BorderLayout.NORTH);
        reachPanel.add(reachSlider, BorderLayout.CENTER);
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
        targetPanelWrapper.setBackground(Color.WHITE);

        JPanel targetPanel = new JPanel(new GridLayout(4, 1));
        targetPanel.setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.BLACK),
                "Final Edges in pathways restricted to:", 0, 0, null, Color.BLACK
        ));
        targetPanel.setBackground(Color.WHITE);

        JRadioButton rb1 = new JRadioButton("Activation edges only");
        JRadioButton rb2 = new JRadioButton("Expression edges only");
        JRadioButton rb3 = new JRadioButton("No restriction");

        // Set foreground and transparent background for radio buttons
        for (JRadioButton rb : new JRadioButton[]{rb1, rb2, rb3}) {
            rb.setBackground(Color.WHITE);
            rb.setForeground(Color.BLACK);
            rb.setOpaque(true);
            targetPanel.add(rb);
        }

        ButtonGroup group = new ButtonGroup();
        group.add(rb1);
        group.add(rb2);
        group.add(rb3);

        int it = user.getEdgeRestriction();
        JRadioButton[] radio = {rb3, rb1, rb2};
        if (it != -1) {
            radio[it].setSelected(true);
        }

        targetPanelWrapper.add(targetPanel, BorderLayout.CENTER);

        body.add(reachPanelWrapper);
        body.add(targetPanelWrapper);

        // === Backend solver configuration panel ===
        JPanel configPanel = new JPanel(new FlowLayout());
        JLabel configLabel = new JLabel("Would you like to change backend solver configuration?");
        configLabel.setFont(new Font("Arial", Font.BOLD, 16));
        configLabel.setForeground(Color.BLACK);
        configPanel.setBackground(Color.WHITE);

        JButton configButton = new JButton("Configure");
        configPanel.add(configLabel);
        configPanel.add(configButton);

        configButton.addActionListener(e -> {
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Change Configuration", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(550, 450);
            dialog.setLocationRelativeTo(null);

            JPanel mainPanel = new JPanel(new GridLayout(6, 1));
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
                int arr[] = new int[4];
                int i = 0;
                for (Component comp : mainPanel.getComponents()) {

                    if (comp instanceof JPanel panel) {

                        for (Component inner : panel.getComponents()) {
                            if (inner instanceof JTextField jTextField) {
                                String value = jTextField.getText();
                                try {
                                    arr[i] = Integer.parseInt(value);
                                } catch (Exception err) {
                                    JOptionPane.showMessageDialog(this, "Invalid Inputs", "Error", JOptionPane.ERROR_MESSAGE);
                                }
                            }
                        }
                    }
                    i++;
                }
                user.setSolverConfig(arr);

                dialog.dispose();
            });
            cancel.addActionListener(ev -> dialog.dispose());

            dialog.setVisible(true);
        });

        body.add(configPanel);

        // === Bottom buttons panel ===
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


        

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(prevButton, BorderLayout.EAST);
                add(goToSessions, BorderLayout.WEST);
                setBackground(Color.WHITE);
                setOpaque(false);
            }
        });

        buttonPanel.add(new JPanel(new BorderLayout()) {
            {
                setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                add(nextButton, BorderLayout.WEST);
                add(saveButton, BorderLayout.EAST);
                setBackground(Color.WHITE);
                setOpaque(false);
            }
        });

        // Assign foreground/background colors for buttons to suit white bg
        

        nextButton.addActionListener(e -> {
            int i = -1;

            if (rb1.isSelected()) {
                i = 1;
            } else if (rb2.isSelected()) {
                i = 2;
            } else if (rb3.isSelected()) {
                i = 0;
            }

            if (i == -1) {
                JOptionPane.showMessageDialog(this, "Select restriction.");
                return;
            }

            user.setEdgeRestriction(i);

            RelaxationBoundsPanel relaxationBoundsPanel = new RelaxationBoundsPanel(cardLayout, cardPanel, user);
            cardPanel.add(relaxationBoundsPanel, "relaxationPanel");
            cardLayout.show(cardPanel, "relaxationPanel");
        });

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "xml"));

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        saveButton.addActionListener(e -> {
            int i = -1;

            if (rb1.isSelected()) {
                i = 1;
            } else if (rb2.isSelected()) {
                i = 2;
            } else if (rb3.isSelected()) {
                i = 0;
            }
            if (i == -1) {
                JOptionPane.showMessageDialog(this, "Select restriction.");
                return;
            }

            user.setEdgeRestriction(i);
            user.saveData();
        });

        add(body, BorderLayout.CENTER);
        add(buttonPanel, BorderLayout.SOUTH);
    }

    public JPanel configPanel(String text, int value) {
        JPanel panel = new JPanel(new FlowLayout());
        panel.setBackground(Color.WHITE);
        JLabel label = new JLabel(text);
        label.setForeground(Color.BLACK);
        JTextField textField = new JTextField(10);
        textField.setText(String.valueOf(value));
        panel.add(label);
        panel.add(textField);

        return panel;
    }

   
    
}
