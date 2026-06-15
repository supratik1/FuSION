import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;

public class EdgesChooserPanel extends RoundedPanel {

    private JLabel[] fileLabels = new JLabel[6];
    private String[] edges;
    private int[] coexpThreshHolder;
    private int[] coexpNHolder;

    public EdgesChooserPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(Theme.BG);

        edges = new String[6];
        String[] orig = user.getEdgesFile();
        for (int i = 0; i < 5; i++) edges[i] = orig[i];
        edges[5] = user.getCoexpressionCsv() == null ? "" : user.getCoexpressionCsv();

        coexpThreshHolder = new int[]{ user.getCoexpThresh() };
        coexpNHolder      = new int[]{ user.getFrozenThresh() };

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        JPanel titleArea = new JPanel();
        titleArea.setLayout(new BoxLayout(titleArea, BoxLayout.Y_AXIS));
        titleArea.setBackground(Theme.BG);
        titleArea.setBorder(BorderFactory.createEmptyBorder(24, 40, 0, 40));

        JLabel title = new JLabel("Edge Files");
        title.setFont(Theme.title(26));
        title.setForeground(Theme.TEXT_DARK);
        title.setAlignmentX(Component.CENTER_ALIGNMENT);
        JLabel titleSub = new JLabel("Optional files to constrain the pathway search");
        titleSub.setFont(Theme.body(14));
        titleSub.setForeground(Theme.TEXT_MED);
        titleSub.setAlignmentX(Component.CENTER_ALIGNMENT);
        titleArea.add(title);
        titleArea.add(Box.createVerticalStrut(4));
        titleArea.add(titleSub);

        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        panel.setBorder(BorderFactory.createEmptyBorder(16, 40, 16, 40));
        panel.setBackground(Theme.BG);

        panel.add(createFileRow("Additional Edges File", 0, false));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createFileRow("Essential Edge File", 1, false));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createFileRow("Avoid Edge File", 2, false));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createFileRow("Relaxed Edges File", 3, false));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createFileRow("Non Relaxed Edges File", 4, false));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createFileRow("Coexpression CSV File", 5, true));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createIntRow("Coexpression Threshold (COEXP_THRESH)", coexpThreshHolder, 0));
        panel.add(Box.createVerticalStrut(8));
        panel.add(createIntRow("Frozen Threshold (FROZEN_THRESH)", coexpNHolder, 0));

        JScrollPane scrollPane = new JScrollPane(panel);
        scrollPane.setBorder(null);
        scrollPane.getVerticalScrollBar().setUnitIncrement(14);
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);

        JPanel center = new JPanel(new BorderLayout());
        center.setBackground(Theme.BG);
        center.add(titleArea, BorderLayout.NORTH);
        center.add(scrollPane, BorderLayout.CENTER);
        add(center, BorderLayout.CENTER);

        RoundedButton nextButton    = Theme.navBtn("Next →", 110);
        RoundedButton goToSessions  = Theme.warningBtn("Sessions", 140);
        RoundedButton prevButton    = Theme.navBtn("← Prev", 110);
        RoundedButton saveButton    = Theme.successBtn("Save", 110);

        JPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setBackground(Theme.BG);
            setOpaque(false);
        }});

        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        saveButton.addActionListener(e -> saveToUser(user));

        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "threshold"));

        nextButton.addActionListener(e -> {
            saveToUser(user);
            user.saveData();
            Gui4Panel gui4Panel = new Gui4Panel(cardLayout, cardPanel, user);
            cardPanel.add(gui4Panel, "gui4Panel");
            cardLayout.show(cardPanel, "gui4Panel");
        });

        add(buttonPanel, BorderLayout.SOUTH);
    }

    private void saveToUser(UserInput user) {
        String[] edgesOnly = new String[5];
        System.arraycopy(edges, 0, edgesOnly, 0, 5);
        user.setEdgesFile(edgesOnly);
        user.setCoexpressionCsv(edges[5]);
        user.setCoexpThresh(coexpThreshHolder[0]);
        user.setFrozenThresh(coexpNHolder[0]);
    }

    private JPanel createFileRow(String title, int index, boolean csvOnly) {
        JPanel rowPanel = new JPanel(new BorderLayout(12, 0));
        rowPanel.setBackground(Theme.BG_CARD);
        rowPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 60));
        rowPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));

        JPanel infoPanel = new JPanel();
        infoPanel.setLayout(new BoxLayout(infoPanel, BoxLayout.Y_AXIS));
        infoPanel.setOpaque(false);

        JLabel titleLabel = new JLabel(title);
        titleLabel.setFont(Theme.title(14));
        titleLabel.setForeground(Theme.TEXT_DARK);

        fileLabels[index] = new JLabel();
        fileLabels[index].setText(edges[index].equals("") ? "No file selected" : new File(edges[index]).getName());
        fileLabels[index].setFont(Theme.body(12));
        fileLabels[index].setForeground(Theme.TEXT_MED);

        infoPanel.add(titleLabel);
        infoPanel.add(fileLabels[index]);

        RoundedButton chooseButton = Theme.navBtn("Choose", 110);
        chooseButton.addActionListener(e -> {
            JFileChooser chooser = new JFileChooser();
            if (csvOnly) {
                chooser.setFileFilter(new FileNameExtensionFilter("CSV files (*.csv)", "csv"));
                chooser.setAcceptAllFileFilterUsed(false);
            }
            if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
                File selectedFile = chooser.getSelectedFile();
                fileLabels[index].setText(selectedFile.getName());
                edges[index] = selectedFile.getAbsolutePath();
            }
        });

        rowPanel.add(infoPanel, BorderLayout.CENTER);
        rowPanel.add(chooseButton, BorderLayout.EAST);
        return rowPanel;
    }

    private JPanel createIntRow(String title, int[] holder, int holderIdx) {
        JPanel rowPanel = new JPanel(new BorderLayout(12, 0));
        rowPanel.setBackground(Theme.BG_CARD);
        rowPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 60));
        rowPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(10, 16, 10, 16)));

        JLabel titleLabel = new JLabel(title);
        titleLabel.setFont(Theme.title(14));
        titleLabel.setForeground(Theme.TEXT_DARK);

        JTextField valueField = new JTextField(String.valueOf(holder[holderIdx]), 5);
        valueField.setFont(Theme.title(14));
        valueField.setForeground(Theme.TEXT_DARK);
        valueField.setHorizontalAlignment(JTextField.CENTER);
        valueField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(2, 6, 2, 6)));

        // Sync holder when user types directly
        valueField.addActionListener(e -> {
            try { holder[holderIdx] = Integer.parseInt(valueField.getText().trim()); }
            catch (NumberFormatException ex) { valueField.setText(String.valueOf(holder[holderIdx])); }
        });
        valueField.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusLost(java.awt.event.FocusEvent e) {
                try { holder[holderIdx] = Integer.parseInt(valueField.getText().trim()); }
                catch (NumberFormatException ex) { valueField.setText(String.valueOf(holder[holderIdx])); }
            }
        });

        RoundedButton minusBtn = new RoundedButton("−", 8, new Dimension(34, 34));
        minusBtn.setBackground(Theme.BORDER);
        minusBtn.setForeground(Theme.TEXT_DARK);
        minusBtn.setFont(Theme.title(16));

        RoundedButton plusBtn = new RoundedButton("+", 8, new Dimension(34, 34));
        plusBtn.setBackground(Theme.PRIMARY);
        plusBtn.setForeground(Color.WHITE);
        plusBtn.setFont(Theme.title(16));

        minusBtn.addActionListener(e -> {
            holder[holderIdx]--;
            valueField.setText(String.valueOf(holder[holderIdx]));
        });
        plusBtn.addActionListener(e -> {
            holder[holderIdx]++;
            valueField.setText(String.valueOf(holder[holderIdx]));
        });

        JPanel controls = new JPanel(new FlowLayout(FlowLayout.RIGHT, 6, 0));
        controls.setOpaque(false);
        controls.add(minusBtn);
        controls.add(valueField);
        controls.add(plusBtn);

        rowPanel.add(titleLabel, BorderLayout.CENTER);
        rowPanel.add(controls, BorderLayout.EAST);
        return rowPanel;
    }
}
