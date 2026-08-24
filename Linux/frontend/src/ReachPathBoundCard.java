import java.awt.*;
import javax.swing.*;

public class ReachPathBoundCard extends RoundedPanel {

    public ReachPathBoundCard(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        // ── Page heading ─────────────────────────────────────────────────────
        JPanel headingArea = new JPanel();
        headingArea.setLayout(new BoxLayout(headingArea, BoxLayout.Y_AXIS));
        headingArea.setBackground(Theme.BG);
        headingArea.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(22), Theme.scale(48), Theme.scale(8), Theme.scale(48)));

        JLabel pageTitle = new JLabel("Maximum Signalling Path Length");
        pageTitle.setFont(Theme.title(28));
        pageTitle.setForeground(Theme.TEXT_DARK);
        pageTitle.setAlignmentX(Component.LEFT_ALIGNMENT);

        headingArea.add(pageTitle);

        // ── Card 1: Reach Path Bound ─────────────────────────────────────────
        JPanel reachCard = makeCard();
        JLabel reachTitle = new JLabel("Value:");
        reachTitle.setFont(Theme.title(16));
        reachTitle.setForeground(Theme.TEXT_DARK);

        JSlider reachSlider = new JSlider(0, 100, user.getSignallingPathLength());
        reachSlider.setPaintTicks(true);
        reachSlider.setPaintLabels(true);
        reachSlider.setMajorTickSpacing(10);
        reachSlider.setMinorTickSpacing(1);
        reachSlider.setForeground(Theme.TEXT_MED);
        reachSlider.setBackground(Theme.BG_CARD);
        reachSlider.setOpaque(false);

        JPanel reachValueRow = new JPanel(new FlowLayout(FlowLayout.LEFT, Theme.scale(8), 0));
        reachValueRow.setOpaque(false);
        JLabel reachValueLabel = new JLabel("Current value:");
        reachValueLabel.setFont(Theme.body(13));
        reachValueLabel.setForeground(Theme.TEXT_MED);

        JTextField reachValueField = new JTextField(5);
        reachValueField.setText("" + user.getSignallingPathLength());
        reachValueField.setHorizontalAlignment(JTextField.CENTER);
        reachValueField.setFont(Theme.title(14));
        Theme.styleDarkField(reachValueField);
        reachValueField.setMargin(new Insets(Theme.scale(4), Theme.scale(8), Theme.scale(4), Theme.scale(8)));
        reachValueField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(2), Theme.scale(6), Theme.scale(2), Theme.scale(6))));
        reachValueRow.add(reachValueLabel);
        reachValueRow.add(reachValueField);

        reachCard.add(reachTitle,    BorderLayout.NORTH);
        reachCard.add(reachSlider,   BorderLayout.CENTER);
        reachCard.add(reachValueRow, BorderLayout.SOUTH);
        reachCard.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(200)));

        reachSlider.addChangeListener(e -> {
            reachValueField.setText(String.valueOf(reachSlider.getValue()));
            user.setSignallingPathLength(reachSlider.getValue());
        });
        reachValueField.addActionListener(e -> {
            try {
                int v = Integer.parseInt(reachValueField.getText().trim());
                if (v >= reachSlider.getMinimum() && v <= reachSlider.getMaximum()) {
                    reachSlider.setValue(v);
                    user.setSignallingPathLength(v);
                }
            } catch (NumberFormatException ignored) {}
        });

        // ── Card 2: Target Edges ─────────────────────────────────────────────
        JPanel targetCard = makeCard();
        JLabel targetTitle = new JLabel("Final Edges in pathways restricted to:");
        targetTitle.setFont(Theme.title(16));
        targetTitle.setForeground(Theme.TEXT_DARK);

        JRadioButton rb1 = styledRadio("Activation edges only");
        JRadioButton rb2 = styledRadio("Expression edges only");
        JRadioButton rb3 = styledRadio("No restriction");

        ButtonGroup group = new ButtonGroup();
        group.add(rb1); group.add(rb2); group.add(rb3);

        int it = user.getEdgeRestriction();
        JRadioButton[] radio = {rb3, rb1, rb2};
        if (it != -1) radio[it].setSelected(true);

        JPanel radioPanel = new JPanel();
        radioPanel.setLayout(new BoxLayout(radioPanel, BoxLayout.Y_AXIS));
        radioPanel.setOpaque(false);
        radioPanel.add(Box.createVerticalStrut(Theme.scale(8)));
        radioPanel.add(rb1);
        radioPanel.add(Box.createVerticalStrut(Theme.scale(6)));
        radioPanel.add(rb2);
        radioPanel.add(Box.createVerticalStrut(Theme.scale(6)));
        radioPanel.add(rb3);

        targetCard.add(targetTitle, BorderLayout.NORTH);
        targetCard.add(radioPanel,  BorderLayout.CENTER);
        targetCard.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(200)));

        // ── Card 3: Solver Config ────────────────────────────────────────────
        JPanel configCard = makeCard();
        JLabel configTitle = new JLabel("Would you like to change backend solver configuration?");
        configTitle.setFont(Theme.body(14));
        configTitle.setForeground(Theme.TEXT_DARK);
        configTitle.setAlignmentX(Component.LEFT_ALIGNMENT);

        RoundedButton configButton = Theme.navBtn("Configure", 120);

        JPanel configInfo = new JPanel(new FlowLayout(FlowLayout.LEFT, Theme.scale(12), 0));
        configInfo.setOpaque(false);
        configInfo.add(configTitle);
        configInfo.add(configButton);

        configCard.add(configInfo, BorderLayout.CENTER);
        configCard.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(150)));
        configButton.addActionListener(e -> showSolverDialog(user));

        // ── Stack cards in a scrollable box ──────────────────────────────────
        JPanel cardsBox = new JPanel();
        cardsBox.setLayout(new BoxLayout(cardsBox, BoxLayout.Y_AXIS));
        cardsBox.setBackground(Theme.BG);
        cardsBox.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(12), Theme.scale(48), Theme.scale(16), Theme.scale(48)));
        cardsBox.add(reachCard);
        cardsBox.add(Box.createVerticalStrut(Theme.scale(14)));
        cardsBox.add(targetCard);
        cardsBox.add(Box.createVerticalStrut(Theme.scale(14)));
        cardsBox.add(configCard);

        JScrollPane scrollPane = new JScrollPane(cardsBox);
        scrollPane.setBorder(null);
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        scrollPane.getVerticalScrollBar().setUnitIncrement(Theme.scale(16));
        scrollPane.setBackground(Theme.BG);
        scrollPane.getViewport().setBackground(Theme.BG);

        JPanel centerWrapper = new JPanel(new BorderLayout());
        centerWrapper.setBackground(Theme.BG);
        centerWrapper.add(headingArea, BorderLayout.NORTH);
        centerWrapper.add(scrollPane,  BorderLayout.CENTER);
        add(centerWrapper, BorderLayout.CENTER);

        // ── Bottom navigation ─────────────────────────────────────────────────
        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        RoundedButton nextButton   = Theme.navBtn("Next »", 110);
        RoundedButton goToSessions = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton   = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton   = Theme.successBtn("Save", 110);

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(goToSessions, BorderLayout.WEST);
            add(prevButton,   BorderLayout.EAST);
            setOpaque(false);
        }});
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton,  BorderLayout.WEST);
            add(saveButton,  BorderLayout.EAST);
            setOpaque(false);
        }});

        nextButton.addActionListener(e -> {
            int i = rb1.isSelected() ? 1 : rb2.isSelected() ? 2 : rb3.isSelected() ? 0 : -1;
            if (i == -1) { JOptionPane.showMessageDialog(this, "Please select an interaction type."); return; }
            user.setEdgeRestriction(i);
            RelaxationBoundsPanel p = new RelaxationBoundsPanel(cardLayout, cardPanel, user);
            cardPanel.add(p, "relaxationPanel");
            cardLayout.show(cardPanel, "relaxationPanel");
        });
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "xml"));
        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));
        saveButton.addActionListener(e -> {
            int i = rb1.isSelected() ? 1 : rb2.isSelected() ? 2 : rb3.isSelected() ? 0 : -1;
            if (i == -1) { JOptionPane.showMessageDialog(this, "Please select an interaction type."); return; }
            user.setEdgeRestriction(i); user.saveData();
        });

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private JPanel makeCard() {
        JPanel p = new JPanel(new BorderLayout(0, Theme.scale(8)));
        p.setBackground(Theme.BG_CARD);
        p.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(
                Theme.scale(16), Theme.scale(20), Theme.scale(16), Theme.scale(20))));
        return p;
    }

    private JRadioButton styledRadio(String text) {
        JRadioButton rb = new JRadioButton(text);
        rb.setBackground(Theme.BG_CARD);
        rb.setForeground(Theme.TEXT_DARK);
        rb.setFont(Theme.body(14));
        rb.setOpaque(false);
        return rb;
    }

    private void showSolverDialog(UserInput user) {
        JDialog dialog = new JDialog((java.awt.Frame) null, "Change Configuration", true);
        dialog.setLayout(new BorderLayout(Theme.GAP_SM, Theme.GAP_SM));
        dialog.setSize(Theme.scale(620), Theme.scale(380));
        dialog.setLocationRelativeTo(null);
        dialog.getContentPane().setBackground(Theme.BG);

        JPanel mainPanel = new JPanel(new GridLayout(4, 1, 0, Theme.scale(8)));
        mainPanel.setBackground(Theme.BG);
        mainPanel.setBorder(BorderFactory.createEmptyBorder(
            Theme.scale(20), Theme.scale(30), Theme.scale(10), Theme.scale(30)));
        int[] solver = user.getSolverConfig();
        mainPanel.add(configRow("Increment Solver Timeout", solver[0]));
        mainPanel.add(configRow("Overall Solver Timeout",  solver[1]));
        mainPanel.add(configRow("Solutions to Count",      solver[2]));
        mainPanel.add(configRow("Solutions to Explore",    solver[3]));

        JPanel buttonRow = new JPanel(new FlowLayout(FlowLayout.CENTER, Theme.GAP_SM, Theme.GAP_SM));
        buttonRow.setBackground(Theme.BG);
        RoundedButton okay   = Theme.successBtn("Ok",     100);
        RoundedButton cancel = Theme.dangerBtn("Cancel",  100);
        buttonRow.add(okay);
        buttonRow.add(cancel);

        dialog.add(mainPanel,  BorderLayout.CENTER);
        dialog.add(buttonRow,  BorderLayout.SOUTH);

        okay.addActionListener(ev -> {
            int[] arr = new int[4]; int i = 0;
            for (Component comp : mainPanel.getComponents()) {
                if (comp instanceof JPanel p) {
                    for (Component inner : p.getComponents()) {
                        if (inner instanceof JTextField tf) {
                            try { arr[i] = Integer.parseInt(tf.getText()); }
                            catch (Exception ex) {
                                JOptionPane.showMessageDialog(dialog,
                                    "Invalid value in row " + (i + 1), "Error", JOptionPane.ERROR_MESSAGE);
                                return;
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
    }

    public JPanel configRow(String text, int value) {
        JPanel panel = new JPanel(new BorderLayout(Theme.scale(12), 0));
        panel.setBackground(Theme.BG_CARD);
        panel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(
                Theme.scale(8), Theme.scale(14), Theme.scale(8), Theme.scale(14))));
        JLabel label = new JLabel(text);
        label.setFont(Theme.body(14));
        label.setForeground(Theme.TEXT_DARK);
        JTextField tf = new JTextField(10);
        tf.setText(String.valueOf(value));
        tf.setFont(Theme.body(14));
        Theme.styleDarkField(tf);
        tf.setHorizontalAlignment(JTextField.RIGHT);
        tf.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(Theme.scale(4), Theme.scale(8), Theme.scale(4), Theme.scale(8))));
        panel.add(label, BorderLayout.CENTER);
        panel.add(tf,    BorderLayout.EAST);
        return panel;
    }
}
