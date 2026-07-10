import java.awt.*;
import javax.swing.*;

public class ReachPathBoundCard extends RoundedPanel {

    public ReachPathBoundCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);

        add(header, BorderLayout.NORTH);

        JPanel body = new JPanel(new GridLayout(3, 1, 20, 10));
        body.setBackground(Theme.BG);

        // === Reach Path Bound Panel ===
        JPanel reachPanelWrapper = new JPanel(new BorderLayout());
        reachPanelWrapper.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        reachPanelWrapper.setBackground(Theme.BG);

        JPanel reachPanel = new JPanel(new BorderLayout());
        reachPanel.setBorder(Theme.section("Maximum Signalling Path Length"));
        reachPanel.setBackground(Theme.BG_CARD);

        JSlider reachSlider = new JSlider(0, 100, user.getSignallingPathLength());
        reachSlider.setPaintTicks(true);
        reachSlider.setPaintLabels(true);
        reachSlider.setMajorTickSpacing(10);
        reachSlider.setMinorTickSpacing(1);
        reachSlider.setForeground(Color.BLACK);
        reachSlider.setBackground(Theme.BG_CARD);

        JPanel reachBoundValue = new JPanel(new FlowLayout());
        reachBoundValue.setBackground(Theme.BG_CARD);
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

        // Commit whatever is typed in the text field. Without this the value only
        // reached UserInput when the slider moved or Enter was pressed, so typing a
        // bound and clicking Next/Save silently kept the previously loaded value.
        Runnable commitReachValue = () -> {
            try {
                int value = Integer.parseInt(reachValueField.getText().trim());
                if (value >= reachSlider.getMinimum() && value <= reachSlider.getMaximum()) {
                    reachSlider.setValue(value);
                    user.setSignallingPathLength(value);
                    return;
                }
            } catch (NumberFormatException ignored) {
            }
            reachValueField.setText(String.valueOf(user.getSignallingPathLength()));
        };

        reachValueField.addActionListener(e -> commitReachValue.run());
        reachValueField.addFocusListener(new java.awt.event.FocusAdapter() {
            @Override
            public void focusLost(java.awt.event.FocusEvent e) {
                commitReachValue.run();
            }
        });

        // === Target Edges Panel ===
        JPanel targetPanelWrapper = new JPanel(new BorderLayout());
        targetPanelWrapper.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        targetPanelWrapper.setBackground(Theme.BG);

        JPanel targetPanel = new JPanel(new GridLayout(4, 1));
        targetPanel.setBorder(Theme.section("Final Edges in pathways restricted to:"));
        targetPanel.setBackground(Theme.BG_CARD);

        JRadioButton rb1 = new JRadioButton("Activation edges only");
        JRadioButton rb2 = new JRadioButton("Expression edges only");
        JRadioButton rb3 = new JRadioButton("No restriction");

        // Set foreground and transparent background for radio buttons
        for (JRadioButton rb : new JRadioButton[]{rb1, rb2, rb3}) {
            rb.setBackground(Theme.BG_CARD);
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
        JPanel configPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 16, 0));
        JLabel configLabel = new JLabel("Backend solver configuration:");
        configLabel.setFont(Theme.body(14));
        configLabel.setForeground(Theme.TEXT_MED);
        configPanel.setBackground(Theme.BG);

        RoundedButton configButton = Theme.navBtn("Configure", 130);
        configPanel.add(configLabel);
        configPanel.add(configButton);

        configButton.addActionListener(e -> {
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Solver Configuration", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(520, 380);
            dialog.setLocationRelativeTo(null);
            dialog.getContentPane().setBackground(Theme.BG);

            JPanel mainPanel = new JPanel(new GridLayout(4, 1, 0, 8));
            mainPanel.setBackground(Theme.BG);
            mainPanel.setBorder(BorderFactory.createEmptyBorder(20, 30, 10, 30));
            int[] solver = user.getSolverConfig();
            mainPanel.add(configPanel("Increment Solver Timeout", solver[0]));
            mainPanel.add(configPanel("Overall Solver Timeout", solver[1]));
            mainPanel.add(configPanel("Solutions to Count", solver[2]));
            mainPanel.add(configPanel("Solutions to Explore", solver[3]));

            JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.CENTER, 12, 10));
            buttonPanel.setBackground(Theme.BG);
            RoundedButton okay = Theme.successBtn("OK", 100);
            RoundedButton cancel = Theme.dangerBtn("Cancel", 100);
            buttonPanel.add(okay);
            buttonPanel.add(cancel);

            dialog.add(mainPanel, BorderLayout.CENTER);
            dialog.add(buttonPanel, BorderLayout.SOUTH);

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
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);


        

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
            commitReachValue.run();
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
            commitReachValue.run();
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
        JPanel panel = new JPanel(new BorderLayout(12, 0));
        panel.setBackground(Theme.BG_CARD);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(8, 14, 8, 14)));
        JLabel label = new JLabel(text);
        label.setFont(Theme.body(14));
        label.setForeground(Theme.TEXT_DARK);
        JTextField textField = new JTextField(8);
        textField.setText(String.valueOf(value));
        textField.setFont(Theme.body(14));
        textField.setHorizontalAlignment(JTextField.RIGHT);
        textField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(4, 8, 4, 8)));
        panel.add(label, BorderLayout.CENTER);
        panel.add(textField, BorderLayout.EAST);
        return panel;
    }

   
    
}
