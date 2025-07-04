
import java.awt.*;
import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;
import javax.swing.*;
import javax.swing.text.DefaultHighlighter;

import org.json.JSONArray;
import org.json.JSONObject;

public class ThresholdFilterPanel extends JPanel {

    private JTextField upperField;
    private JTextField lowerField;
    private JButton filterButton;
    private JTextArea aboveArea;
    private JTextArea betweenArea;
    private JTextArea belowArea;
    private JTextArea inactiveArea;
    private JTextArea activeArea;
    private JTextArea relaxedArea;
    private JTextArea nonrelaxedArea;
    private JTextField searchField;
    private JButton searchButton;
    private JTextArea[] resultAreas;
    private JButton moveTo;
    // private boolean[] selectedIndices;
    private String selectedText;
    // private boolean[] originalColumnIndex;

    private Map<String, boolean[]> override = new HashMap<>();

    // private File logChangesFile;

    public ThresholdFilterPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        
        initComponents(cardLayout, cardPanel, user);

    }

    private void initComponents(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(new Color(138, 43, 226));
        setOpaque(true);

        JPanel inputPanel = new JPanel(new FlowLayout());

        JPanel searchPanel = new JPanel(new FlowLayout());
        searchPanel.setOpaque(false);
        searchField = new JTextField(8);
        searchButton = new JButton("Search");

        moveTo = new JButton("Override");
        moveTo.setEnabled(false);
        searchPanel.add(searchField);
        searchPanel.add(searchButton);
        searchPanel.add(moveTo);

        inputPanel.add(new JLabel("Down Regulatory Threshold:") {
            {
                setForeground(Color.WHITE);
            }
        });

        lowerField = new JTextField(5);
        lowerField.setText(String.valueOf(user.getDownThreshold()));
        lowerField.setForeground(Color.WHITE);
        lowerField.setCaretColor(Color.WHITE);
        lowerField.setOpaque(false);
        lowerField.setBackground(new Color(255, 255, 255, 100));
        lowerField.setBorder(BorderFactory.createLineBorder(Color.WHITE, 2, true));
        inputPanel.add(lowerField);

        inputPanel.add(new JLabel("Up Regulatory Threshold:") {
            {
                setForeground(Color.WHITE);
            }
        });
        upperField = new JTextField(5);
        upperField.setCaretColor(Color.WHITE);
        upperField.setText(String.valueOf(user.getUpThreshold()));
        upperField.setForeground(Color.WHITE);
        upperField.setOpaque(false);
        upperField.setBackground(new Color(255, 255, 255, 100));
        upperField.setBorder(BorderFactory.createLineBorder(Color.WHITE, 2, true));
        inputPanel.add(upperField);

        filterButton = new JButton("Filter Files");
        inputPanel.add(filterButton);
        inputPanel.add(searchPanel);

        inputPanel.setBackground(new Color(138, 43, 226));

        add(inputPanel, BorderLayout.NORTH);

        JPanel resultPanel = new JPanel(new GridLayout(1, 7, 10, 10));
        resultPanel.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20));

        aboveArea = new JTextArea();
        betweenArea = new JTextArea();
        belowArea = new JTextArea();
        activeArea = new JTextArea();
        inactiveArea = new JTextArea();
        relaxedArea= new JTextArea();
        nonrelaxedArea= new JTextArea();

        resultAreas = new JTextArea[]{aboveArea, betweenArea, belowArea, activeArea, inactiveArea, relaxedArea, nonrelaxedArea};

        resultPanel.add(createStickyPanel("Up Regulatory Threshold:", aboveArea));
        resultPanel.add(createStickyPanel("Not Differentially Expressed:", betweenArea));
        resultPanel.add(createStickyPanel("Down Regulatory Thresholds:", belowArea));
        resultPanel.add(createStickyPanel("Active Node File:", activeArea));
        resultPanel.add(createStickyPanel("Inactive Node File:", inactiveArea));
        resultPanel.add(createStickyPanel("Relaxed Node File:", relaxedArea));
        resultPanel.add(createStickyPanel("Non-Relaxed Node File:", nonrelaxedArea));

        resultPanel.setBackground(new Color(138, 43, 226));

        JPanel buttons = new JPanel(new BorderLayout());
        JButton saveButton= new JButton("Save");

        buttons.setBackground(new Color(138, 43, 226));

        JButton prevButton = new JButton("Prev");
        
        prevButton.addActionListener(e -> {

            cardLayout.show(cardPanel, "relaxationPanel");
        });

        JButton nextButton = new JButton("Next");
        
        nextButton.addActionListener(e -> {
            EdgesChooserPanel edgePanel = new EdgesChooserPanel(cardLayout, cardPanel, user);
            cardPanel.add(edgePanel, "edgeEntry");
            cardLayout.show(cardPanel, "edgeEntry");
           
        });

        saveButton.addActionListener(e->{

            String fileName = "sessions/" + user.getUsername() + ".json";
            
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

                sessionData.put("dThreshold", user.getDownThreshold());
                sessionData.put("uThreshold", user.getUpThreshold());
                
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

        buttons.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        buttons.add(new JPanel(){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }}, BorderLayout.SOUTH);

        this.add(buttons, BorderLayout.SOUTH);

        add(resultPanel, BorderLayout.CENTER);

        filterButton.addActionListener(e -> {
            
            processFiles(user);

        });
        searchButton.addActionListener(e -> {
            if (lowerField.getText().isBlank() || upperField.getText().isBlank()) {
                JOptionPane.showMessageDialog(this, "Enter number bounds for filtering");
                return;
            }
            if (searchField.getText().isBlank()) {
                return;
            }

            searchAndHighlight(searchField.getText());
            moveTo.setEnabled(true);
        });

        moveTo.addActionListener(e -> {
            String text = searchField.getText();
            JDialog dialog = new JDialog((JFrame) SwingUtilities.getWindowAncestor(this), "Choose Transfer Destination", true);
            dialog.setLayout(new BorderLayout(10, 10));
            dialog.setSize(350, 250);
            dialog.setLocationRelativeTo(this);

            JPanel mainPanel = new JPanel(new GridLayout(0, 1));
            ButtonGroup mainGroup = new ButtonGroup();
            ButtonGroup sideGroup = new ButtonGroup();
            ButtonGroup laterGroup= new ButtonGroup();

            JRadioButton col1 = new JRadioButton("Above Up Regulatory Threshold");
            JRadioButton col2 = new JRadioButton("Not Differentially Expressed");
            JRadioButton col3 = new JRadioButton("Below Down Regulatory Threshold");

            JRadioButton col4 = new JRadioButton("Active Node File");
            JRadioButton col5 = new JRadioButton("Inactive Node File");

            JRadioButton col6 = new JRadioButton("Relaxed Node");
            JRadioButton col7 = new JRadioButton("Non Relaxed Node");

            JRadioButton[] columns = {col1, col2, col3, col4, col5, col6, col7};

            mainGroup.add(col1);
            mainGroup.add(col2);
            mainGroup.add(col3);

            sideGroup.add(col4);
            sideGroup.add(col5);

            laterGroup.add(col6);
            laterGroup.add(col7);

            mainPanel.add(col1);
            mainPanel.add(col2);
            mainPanel.add(col3);
            mainPanel.add(col4);
            mainPanel.add(col5);
            mainPanel.add(col6);
            mainPanel.add(col7);

            if (override.containsKey(text)) {
                boolean[] flags = override.get(text);
                for (int i = 0; i < 7; i++) {
                    if (flags[i] == true) {
                        columns[i].setSelected(true);
                    }
                }
            }

            JPanel buttonPanel = new JPanel();
            JButton ok = new JButton("OK");
            JButton cancel = new JButton("Cancel");
            JButton clear= new JButton("Clear");
            

            boolean[] selectedIndices = new boolean[7];
            // final boolean[] optional = new boolean[2];

            ok.addActionListener(ev -> {

                boolean[] old = override.get(text);

                // Optional: remove from previous
                for (int i = 0; i < 7; i++) {
                    if (old[i] == true) {
                        JTextArea source = resultAreas[i];
                        source.setText(source.getText().replace(selectedText + "\n", ""));
                        // source.setText(source.getText().replace(">> " + selectedText + "\n", ""));
                    }

                }
                if (col1.isSelected()) {
                    selectedIndices[0] = true;
                } else if (col2.isSelected()) {
                    selectedIndices[1] = true;
                } else if (col3.isSelected()) {
                    selectedIndices[2] = true;
                }
                // System.out.println(selectedMain[0]);
                // selectedIndices[3] = col4.isSelected();
                // selectedIndices[4] = col5.isSelected();
                if (col4.isSelected()) {
                    selectedIndices[3] = true;
                } else if(col5.isSelected()){
                    selectedIndices[4] = true;
                }
                if(col6.isSelected()){
                    selectedIndices[5] = true;
                }else if(col7.isSelected()){
                    selectedIndices[6]= true;
                }

                override.put(text, selectedIndices);
                // System.out.println(override);

                // Transfer logic
                if (selectedText != null && !selectedText.isEmpty()) {
                    // if (selectedMain[0] != -1) {
                    //     moveToColumn(selectedMain[0]);
                    // }
                    // if (optional[0]) {
                    //     moveToColumn(3);
                    // }
                    // if (optional[1]) {
                    //     moveToColumn(4);
                    // }
                    // System.out.println(selectedIndices[0]);
                    // System.out.println(selectedIndices[1]);
                    // System.out.println(selectedIndices[2]);
                    // System.out.println(selectedIndices[3]);
                    // System.out.println(selectedIndices[4]);
                    moveTo.setEnabled(false);
                    moveToColumn(text);
                }

                dialog.dispose();
            });

            cancel.addActionListener(ev -> dialog.dispose());

            clear.addActionListener(ev->{
                override.remove(text);
                processFiles(user);
                dialog.dispose();
            });

            buttonPanel.add(ok);
            buttonPanel.add(cancel);
            buttonPanel.add(clear);

            dialog.add(mainPanel, BorderLayout.CENTER);
            dialog.add(buttonPanel, BorderLayout.SOUTH);
            dialog.setVisible(true);
        });

    }

    JPanel createStickyPanel(String title, JTextArea textArea) {
        JPanel panel = new JPanel(new BorderLayout());
        JLabel label = new JLabel(title, SwingConstants.CENTER);
        label.setForeground(Color.WHITE);
        label.setFont(label.getFont().deriveFont(Font.BOLD, 12f));
        label.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        textArea.setForeground(Color.WHITE);
        textArea.setOpaque(false);
        textArea.setEditable(false);

        JScrollPane scrollPane = new JScrollPane(textArea);
        scrollPane.setOpaque(false);
        scrollPane.getViewport().setOpaque(false);
        scrollPane.setBorder(BorderFactory.createLineBorder(Color.WHITE));

        panel.setOpaque(false);
        panel.add(label, BorderLayout.NORTH);
        panel.add(scrollPane, BorderLayout.CENTER);
        return panel;
    }

    private void processFiles(UserInput user) {
        double lower, upper;
        try {
            lower = Double.parseDouble(lowerField.getText());
            upper = Double.parseDouble(upperField.getText());
            if (lower > upper) {
                JOptionPane.showMessageDialog(this, "Lower bound is greater than upper bound", "Error", JOptionPane.ERROR_MESSAGE);
                return;
            }
        } catch (NumberFormatException ex) {
            JOptionPane.showMessageDialog(this, "Please enter valid numbers for bounds.", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        user.setDownThreshold(lower);
        user.setUpThreshold(upper);

        Map<String, Double> fileScores = readFileScores(user);

        if (fileScores == null) {
            System.out.println("Error reading file");
            return;
        }

        for (int i = 0; i < 7; i++) {
            resultAreas[i].setText("");
        }

        for (Map.Entry<String, Double> entry : fileScores.entrySet()) {
            String fileName = entry.getKey();
            Double score = entry.getValue();

            if (override.isEmpty() || !override.containsKey(fileName)) {
                if (score >= upper) {
                    aboveArea.append(fileName + " (" + score + ")\n");
                } else if (score <= lower) {
                    belowArea.append(fileName + " (" + score + ")\n");
                } else {
                    betweenArea.append(fileName + " (" + score + ")\n");
                }
            }

        }
        if (!override.isEmpty()) {
            for (Map.Entry<String, boolean[]> entry : override.entrySet()) {
                String fileName = entry.getKey();
                boolean[] arr = entry.getValue();
                for (int i = 0; i < 7; i++) {
                    if (arr[i]) {
                        resultAreas[i].append(fileName + "\n");
                    }
                }

            }
        }

    }

    private Map<String, Double> readFileScores(UserInput user) {
        // File csv = new File(logChangesFile);
        // if (!csv.exists()) {
        //     JOptionPane.showMessageDialog(this, "CSV file 'file_scores.csv' not found in selected directory.");
        //     return null;
        // }

        Map<String, Double> scores = new HashMap<>();
        try (BufferedReader br = new BufferedReader(new FileReader(user.getLogFoldChangesFile()))) {
            String line;
            while ((line = br.readLine()) != null) {
                String[] parts = line.split("\t+");
                if (parts.length == 2) {
                    String filename = parts[0].trim();
                    double score = Double.parseDouble(parts[1].trim());
                    scores.put(filename, score);
                }
            }
        } catch (IOException | NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Error reading tSV: " + e.getMessage());
            return null;
        }

        return scores;
    }

    private void searchAndHighlight(String query) {
        boolean[] originalColumnIndex = new boolean[7];
        // selectedText = query;
        if (query == null || query.trim().isEmpty()) {
            return;
        }
        query = query.trim().toLowerCase();

        for (int i = 0; i < resultAreas.length; i++) {
            JTextArea area = resultAreas[i];
            area.getHighlighter().removeAllHighlights();
            String[] lines = area.getText().split("\n");
            int offset = 0;

            for (String line : lines) {
                int lineLength = line.length();
                if (line.toLowerCase().contains(query)) {
                    selectedText = line;
                    try {
                        area.getHighlighter().addHighlight(
                                offset,
                                offset + lineLength,
                                new DefaultHighlighter.DefaultHighlightPainter(Color.GREEN.darker())
                        );
                        area.setCaretPosition(offset);
                        originalColumnIndex[i] = true;

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    // break; // only highlight the first match per area (optional)
                }
                offset += lineLength + 1; // +1 for the '\n'
            }
        }
        override.put(query, originalColumnIndex);
    }

    private void moveToColumn(String text) {
        boolean[] flag = override.get(text);

        for (int i = 0; i < 7; i++) {
            if (flag[i]) {
                JTextArea target = resultAreas[i];
                target.append(text + "\n");
            }
        }

        //     try {
        //         //String content = target.getText();
        //         // int index = content.indexOf(">> " + selectedText);
        //         if (index >= 0) {
        //             target.getHighlighter().removeAllHighlights(); // Optional: clear old highlights
        //             target.getHighlighter().addHighlight(
        //                     index,
        //                     index + (">> " + selectedText).length(),
        //                     new DefaultHighlighter.DefaultHighlightPainter(Color.GREEN.darker())
        //             );
        //             target.setCaretPosition(index); // Scroll to it
        //         }
        //     } catch (Exception ex) {
        //         ex.printStackTrace();
        //     }
    }

}
