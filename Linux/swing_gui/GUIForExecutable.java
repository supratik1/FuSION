import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.file.Path;
import java.nio.file.Paths;

public class GUIForExecutable extends JFrame {
    private String[] advanced = { "1", "30000", "150000", "1", "1", "", "", "", "", "", "", "", "", "",
            "common_files/inter_db_mapping_file", "common_files/hsa_to_gene_symbol_map_6334.txt",
            "common_files/hsa_path_to_path_name_map_2822.txt", "",
            "common_files/merged_kegg_master_graph.xml", "common_files/hsa_ids_not_to_be_merged.txt", ";;" };
    private String[] result;

    public GUIForExecutable() {
        setTitle("Application");
        setSize(1300, 900);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        takeInputs();
    }

    private void takeInputs() {
        JPanel panel = new JPanel(new GridLayout(1, 2, 10, 10)); // 1 row, 2 columns
        panel.setBorder(new EmptyBorder(20, 20, 20, 20)); // Add an EmptyBorder to provide margin

        JPanel lp = new JPanel(new GridLayout(11, 2, 5, 5));
        JTextField inputField1 = new JTextField();
        JTextField inputField2 = new JTextField();
        JTextField inputField3 = new JTextField();
        JTextField inputField4 = new JTextField();
        JTextField inputField5 = new JTextField();
        JTextField inputFieldlf = new JTextField();
        JTextField inputFieldef = new JTextField();

        // Set margins for input fields to add space between the border and text
        Insets margin = new Insets(5, 5, 5, 5); // Top, left, bottom, right
        inputField1.setMargin(margin);
        inputField2.setMargin(margin);
        inputField3.setMargin(margin);
        inputField4.setMargin(margin);
        inputField5.setMargin(margin);
        inputFieldlf.setMargin(margin);
        inputFieldef.setMargin(margin);

        ButtonGroup optionGroup = new ButtonGroup();
        JRadioButton radioButton1 = new JRadioButton("Activation");
        radioButton1.setToolTipText("Activation");
        JRadioButton radioButton2 = new JRadioButton("Expression");
        radioButton2.setToolTipText("Expression");
        JRadioButton radioButton3 = new JRadioButton("Both");
        radioButton3.setToolTipText("Both");

        optionGroup.add(radioButton1);
        optionGroup.add(radioButton2);
        optionGroup.add(radioButton3);

        JButton b = new JButton("Choose");
        b.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseDirectory(inputField1);
            }
        });

        JPanel bPanel = new JPanel(new GridLayout(1, 3));
        bPanel.add(b);
        bPanel.add(new JLabel());
        bPanel.add(new JLabel());

        JButton b1 = new JButton("Choose");
        b1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inputFieldlf);
            }
        });

        JPanel b1Panel = new JPanel(new GridLayout(1, 3));
        b1Panel.add(b1);
        b1Panel.add(new JLabel());
        b1Panel.add(new JLabel());

        JButton b2 = new JButton("Choose");
        b2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inputFieldef);
            }
        });

        JPanel b2Panel = new JPanel(new GridLayout(1, 3));
        b2Panel.add(b2);
        b2Panel.add(new JLabel());
        b2Panel.add(new JLabel());

        lp.add(new JLabel("Working Directory"));
        lp.add(inputField1);
        lp.add(new JLabel());
        lp.add(bPanel);
        lp.add(new JLabel("Log Fold Changes File"));
        lp.add(inputFieldlf);
        lp.add(new JLabel());
        lp.add(b1Panel);
        lp.add(new JLabel("Additional Edges File"));
        lp.add(inputFieldef);
        lp.add(new JLabel());
        lp.add(b2Panel);
        lp.add(new JLabel("Source hsa ID"));
        lp.add(inputField2);
        lp.add(new JLabel("Target hsa ID"));
        lp.add(inputField3);
        lp.add(new JLabel("Target Edges")); // 14
        lp.add(createCircularPanel(radioButton1, radioButton2, radioButton3));
        lp.add(new JLabel("Test Node hsa ID"));
        lp.add(inputField4);
        lp.add(new JLabel("Reach Path Bound"));
        lp.add(inputField5);

        JPanel rp = new JPanel(new GridLayout(10, 2, 5, 5));
        JLabel l = new JLabel("Node Relaxation Bounds :");
        l.setForeground(Color.BLUE);
        rp.add(l);
        rp.add(new JLabel());
        rp.add(new JLabel("Upper Bound"));
        JTextField inputField9 = new JTextField();
        rp.add(inputField9);
        rp.add(new JLabel("Lower Bound"));
        JTextField inputField8 = new JTextField();
        rp.add(inputField8);
        JLabel m = new JLabel("Edge Relaxation Bounds :");
        m.setForeground(Color.BLUE);
        rp.add(m);
        rp.add(new JLabel());
        JTextField inputField7 = new JTextField();
        rp.add(new JLabel("Upper Bound"));
        rp.add(inputField7);
        JTextField inputField6 = new JTextField();
        rp.add(new JLabel("Lower Bound"));
        rp.add(inputField6);
        rp.add(new JLabel("Up Regulatory Threshold"));
        JTextField inputField12 = new JTextField();
        rp.add(inputField12);
        rp.add(new JLabel("Down Regulatory Threshold"));
        JTextField inputField13 = new JTextField();
        rp.add(inputField13);

        // Set margins for input fields to add space between the border and text
        Insets margin1 = new Insets(5, 5, 5, 5); // Top, left, bottom, right
        inputField6.setMargin(margin1);
        inputField7.setMargin(margin1);
        inputField8.setMargin(margin1);
        inputField9.setMargin(margin1);
        inputField12.setMargin(margin1);
        inputField13.setMargin(margin1);

        // Path p10 = Paths.get(inputFieldlf.getText());
        // String i10 = p10.getFileName().toString();

        // Path p11 = Paths.get(inputFieldef.getText());
        // String i11 = p11.getFileName().toString();

        JButton advInputs = new JButton("Advanced Inputs");
        advInputs.setBackground(Color.BLACK);
        advInputs.setForeground(Color.white);

        rp.add(advInputs);
        advInputs.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                advanced = advancedInputs();
            }
        });

        JButton saveButton = new JButton("Save Inputs Before Run");
        saveButton.setBackground(Color.RED);
        saveButton.setForeground(Color.WHITE);
        rp.add(saveButton);

        saveButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                result = new String[] { inputField1.getText(), inputField2.getText(), inputField3.getText(),
                        inputField4.getText(),
                        inputField5.getText(), inputField6.getText(), inputField7.getText(), inputField8.getText(),
                        inputField9.getText(), getFileName(inputFieldlf), getFileName(inputFieldef),
                        inputField12.getText(),
                        inputField13.getText(),
                        getOutput(radioButton1, radioButton2, radioButton3),
                        inputFieldef.getText(),
                        inputFieldlf.getText() };
            }
        });

        JButton rButton = new JButton("RUN");

        panel.add(lp);
        panel.add(rp);

        getContentPane().add(panel, BorderLayout.CENTER);

        rButton.setBackground(Color.blue);
        rButton.setForeground(Color.white);
        rp.add(rButton);
        rButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (result == null || advanced == null) {
                    JOptionPane.showMessageDialog(GUIForExecutable.this,
                            "Please Enter Valid Inputs and Save Inputs Before Running.");
                }
                copyFile(result, advanced);
            }
        });
    }

    private String getFileName(JTextField inputField) {
        Path p10 = Paths.get(inputField.getText());
        String i10 = p10.getFileName().toString();

        return i10;
    }

    private String getOutput(JRadioButton radioButton1, JRadioButton radioButton2, JRadioButton radioButton3) {
        String s = "";

        if (radioButton1.isSelected()) {
            s += "$ONLY_ACT_EDGES_TO_TARGET";
        } else if (radioButton2.isSelected()) {
            s += "$ONLY_EXPR_EDGES_TO_TARGET";
        } else if (radioButton3.isSelected()) {
            s += "$BOTH_ACT_EXPR_EDGES_TO_TARGET";
        }
        return s;
    }

    private String[] advancedInputs() {
        JFrame advancedFrame = new JFrame();
        JPanel panel = new JPanel(new GridLayout(1, 2, 10, 10)); // 1 row, 2 columns
        panel.setBorder(new EmptyBorder(20, 20, 20, 20)); // Add an EmptyBorder to provide margin
        Insets margin2 = new Insets(5, 5, 5, 5);

        JPanel leftPanel = new JPanel(new GridLayout(13, 3, 5, 5));
        JTextField inField1 = new JTextField("1");
        inField1.setMargin(margin2);
        leftPanel.add(new JLabel("Node Split Threshold"));
        leftPanel.add(inField1);
        leftPanel.add(new JLabel());
        JTextField inField2 = new JTextField("30000");
        inField2.setMargin(margin2);
        leftPanel.add(new JLabel("Increment Solver Timeout"));
        leftPanel.add(inField2);
        leftPanel.add(new JLabel());
        JTextField inField3 = new JTextField("150000");
        inField3.setMargin(margin2);
        leftPanel.add(new JLabel("Overall Solver Timeout"));
        leftPanel.add(inField3);
        leftPanel.add(new JLabel());
        JTextField inField4 = new JTextField("1");
        inField4.setMargin(margin2);
        leftPanel.add(new JLabel("Solutions to Count"));
        leftPanel.add(inField4);
        leftPanel.add(new JLabel());
        JTextField inField5 = new JTextField("1");
        inField5.setMargin(margin2);
        leftPanel.add(new JLabel("Solutions to Explore"));
        leftPanel.add(inField5);
        leftPanel.add(new JLabel());
        // JTextField inField6 = new JTextField();
        leftPanel.add(new JLabel());
        leftPanel.add(new JLabel());
        leftPanel.add(new JLabel());
        JTextField inField15 = new JTextField("common_files/inter_db_mapping_file");
        inField15.setMargin(margin2);
        leftPanel.add(new JLabel("Cross Database Map"));
        leftPanel.add(inField15);

        JButton chooseButton1 = new JButton("Choose");
        chooseButton1.setBackground(Color.BLACK);
        chooseButton1.setForeground(Color.WHITE);
        chooseButton1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField15);
            }
        });
        leftPanel.add(chooseButton1);

        JTextField inField16 = new JTextField("common_files/hsa_to_gene_symbol_map_6334.txt");
        inField16.setMargin(margin2);
        leftPanel.add(new JLabel("HSA to Gene Symbol Map"));
        leftPanel.add(inField16);

        JButton chooseButton2 = new JButton("Choose");
        chooseButton2.setBackground(Color.BLACK);
        chooseButton2.setForeground(Color.WHITE);
        chooseButton2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField16);
            }
        });
        leftPanel.add(chooseButton2);

        JTextField inField17 = new JTextField("common_files/hsa_path_to_path_name_map_2822.txt");
        inField17.setMargin(margin2);
        leftPanel.add(new JLabel("HSA Path-to-Path Name Map"));
        leftPanel.add(inField17);

        JButton chooseButton3 = new JButton("Choose");
        chooseButton3.setBackground(Color.BLACK);
        chooseButton3.setForeground(Color.WHITE);
        chooseButton3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField17);
            }
        });
        leftPanel.add(chooseButton3);

        JTextField inField20 = new JTextField("common_files/hsa_ids_not_to_be_merged.txt");
        inField20.setMargin(margin2);
        leftPanel.add(new JLabel("HSA IDs Not to be Merged"));
        leftPanel.add(inField20);

        JButton chooseButton4 = new JButton("Choose");
        chooseButton4.setBackground(Color.BLACK);
        chooseButton4.setForeground(Color.WHITE);
        chooseButton4.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField20);
            }
        });
        leftPanel.add(chooseButton4);

        leftPanel.add(new JLabel());
        JCheckBox checkbox = new JCheckBox("Merge XML Files Afresh.");
        checkbox.setForeground(Color.BLUE);
        leftPanel.add(checkbox); // 21 if checkbox is selected then
        leftPanel.add(new JLabel());

        JTextField inField18 = new JTextField();
        inField18.setMargin(margin2);
        leftPanel.add(new JLabel("List of KEGG XML Files to Merge"));
        leftPanel.add(inField18);
        JButton chooseButton5 = new JButton("Choose");
        chooseButton5.setBackground(Color.BLACK);
        chooseButton5.setForeground(Color.WHITE);
        chooseButton5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField18);
            }
        });
        leftPanel.add(chooseButton5);
        JTextField inField19 = new JTextField("common_files/merged_kegg_master_graph.xml");
        inField19.setMargin(margin2);
        leftPanel.add(new JLabel("XML File of Merged KEGG Pathways"));
        leftPanel.add(inField19);
        JButton chooseButton6 = new JButton("Choose");
        chooseButton6.setBackground(Color.BLACK);
        chooseButton6.setForeground(Color.WHITE);
        chooseButton6.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField19);
            }
        });
        leftPanel.add(chooseButton6);

        checkbox.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent e) {
                if (checkbox.isSelected()) {
                    inField18.setText("common_files/list_of_kegg_xml_files.txt");
                } else {
                    inField18.setText("");
                }
            }
        });

        panel.add(leftPanel);

        JPanel rightPanel = new JPanel(new GridLayout(10, 3, 5, 5));
        JTextField inField7 = new JTextField();
        inField7.setMargin(margin2);
        rightPanel.add(new JLabel("Essential Edge File"));
        rightPanel.add(inField7);
        JButton chooseButton7 = new JButton("Choose");
        chooseButton7.setBackground(Color.BLACK);
        chooseButton7.setForeground(Color.WHITE);
        chooseButton7.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField7);
            }
        });
        rightPanel.add(chooseButton7);
        JTextField inField8 = new JTextField();
        inField8.setMargin(margin2);
        rightPanel.add(new JLabel("Avoid Edge File"));
        rightPanel.add(inField8);
        JButton chooseButton8 = new JButton("Choose");
        chooseButton8.setBackground(Color.BLACK);
        chooseButton8.setForeground(Color.WHITE);
        chooseButton8.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField8);
            }
        });
        rightPanel.add(chooseButton8);
        JTextField inField9 = new JTextField();
        inField9.setMargin(margin2);
        rightPanel.add(new JLabel("Inactive Node File"));
        rightPanel.add(inField9);
        JButton chooseButton9 = new JButton("Choose");
        chooseButton9.setBackground(Color.BLACK);
        chooseButton9.setForeground(Color.WHITE);
        chooseButton9.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField9);
            }
        });
        rightPanel.add(chooseButton9);
        JTextField inField10 = new JTextField();
        inField10.setMargin(margin2);
        rightPanel.add(new JLabel("Confirmed Up Reg File"));
        rightPanel.add(inField10);
        JButton chooseButton10 = new JButton("Choose");
        chooseButton10.setBackground(Color.BLACK);
        chooseButton10.setForeground(Color.WHITE);
        chooseButton10.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField10);
            }
        });
        rightPanel.add(chooseButton10);
        JTextField inField11 = new JTextField();
        inField11.setMargin(margin2);
        rightPanel.add(new JLabel("Confirmed Down Reg File"));
        rightPanel.add(inField11);
        JButton chooseButton11 = new JButton("Choose");
        chooseButton11.setBackground(Color.BLACK);
        chooseButton11.setForeground(Color.WHITE);
        chooseButton11.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField11);
            }
        });
        rightPanel.add(chooseButton11);
        JTextField inField12 = new JTextField();
        inField12.setMargin(margin2);
        rightPanel.add(new JLabel("Relaxed Nodes"));
        rightPanel.add(inField12);
        JButton chooseButton12 = new JButton("Choose");
        chooseButton12.setBackground(Color.BLACK);
        chooseButton12.setForeground(Color.WHITE);
        chooseButton12.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField12);
            }
        });
        rightPanel.add(chooseButton12);
        JTextField inField13 = new JTextField();
        inField13.setMargin(margin2);
        rightPanel.add(new JLabel("Relaxed Edges"));
        rightPanel.add(inField13);
        JButton chooseButton13 = new JButton("Choose");
        chooseButton13.setBackground(Color.BLACK);
        chooseButton13.setForeground(Color.WHITE);
        chooseButton13.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField13);
            }
        });
        rightPanel.add(chooseButton13);
        JTextField inField14 = new JTextField();
        inField14.setMargin(margin2);
        rightPanel.add(new JLabel("Non Relaxed Edges File"));
        rightPanel.add(inField14);
        JButton chooseButton14 = new JButton("Choose");
        chooseButton14.setBackground(Color.BLACK);
        chooseButton14.setForeground(Color.WHITE);
        chooseButton14.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField14);
            }
        });
        rightPanel.add(chooseButton14);
        JCheckBox checkbox2 = new JCheckBox("Default Entries");
        checkbox2.setForeground(Color.BLUE);
        rightPanel.add(checkbox2);

        checkbox2.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent e) {
                if (checkbox2.isSelected()) {
                    inField7.setText("common_files/empty_file");
                    inField8.setText("common_files/empty_file");
                    inField9.setText("common_files/empty_file");
                    inField10.setText("common_files/empty_file");
                    inField11.setText("common_files/empty_file");
                    inField12.setText("common_files/empty_file");
                    inField13.setText("common_files/empty_file");
                    inField14.setText("common_files/empty_file");
                } else {
                    inField7.setText("");
                    inField8.setText("");
                    inField9.setText("");
                    inField10.setText("");
                    inField11.setText("");
                    inField12.setText("");
                    inField13.setText("");
                    inField14.setText("");
                }
            }
        });

        String[] s = { inField1.getText(), inField2.getText(), inField3.getText(),
                inField4.getText(),
                inField5.getText(), "", inField7.getText(), inField8.getText(), inField9.getText(),
                inField10.getText(),
                inField11.getText(), inField12.getText(), inField13.getText(), inField14.getText(),
                inField15.getText(),
                inField16.getText(), inField17.getText(), inField18.getText(), inField19.getText(),
                inField20.getText(), ";;" };
        // advancedFrame.dispose();

        JButton saveInputs = new JButton("Save Inputs Before Run");
        saveInputs.setBackground(Color.RED);
        saveInputs.setForeground(Color.WHITE);
        saveInputs.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                advanced = new String[] { inField1.getText(), inField2.getText(),
                        inField3.getText(),
                        inField4.getText(),
                        inField5.getText(), "", inField7.getText(), inField8.getText(),
                        inField9.getText(),
                        inField10.getText(),
                        inField11.getText(), inField12.getText(), inField13.getText(),
                        inField14.getText(),
                        inField15.getText(),
                        inField16.getText(), inField17.getText(), inField18.getText(),
                        inField19.getText(),
                        inField20.getText(), returnString(checkbox) };
                advancedFrame.dispose();
            }
        });
        rightPanel.add(saveInputs);
        panel.add(rightPanel);
        advancedFrame.getContentPane().add(panel);

        advancedFrame.setTitle("Advanced Inputs");
        advancedFrame.setSize(1200, 600);
        advancedFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        advancedFrame.setLocationRelativeTo(null);
        advancedFrame.setVisible(true);

        if (advanced != null)
            return advanced;
        else {
            advanced = s;
            return advanced;
        }
    }

    private void chooseFile(JTextField targetField) {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
        fileChooser.setCurrentDirectory(new File(System.getProperty("user.dir")));
        int result = fileChooser.showOpenDialog(this);

        if (result == JFileChooser.APPROVE_OPTION) {
            File selectedFile = fileChooser.getSelectedFile();
            targetField.setText(selectedFile.getAbsolutePath());
        }
    }

    private void chooseDirectory(JTextField targetField) {
        JFileChooser fileChooser = new JFileChooser();
        fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        fileChooser.setCurrentDirectory(new File(System.getProperty("user.dir")));

        int result = fileChooser.showOpenDialog(this);

        if (result == JFileChooser.APPROVE_OPTION) {
            File selectedFile = fileChooser.getSelectedFile();
            targetField.setText(selectedFile.getAbsolutePath());
        }
    }

    private JPanel createCircularPanel(JRadioButton... radioButtons) {
        JPanel circularPanel = new JPanel(new GridLayout(1, 3));

        for (JRadioButton radioButton : radioButtons) {
            circularPanel.add(radioButton);
        }

        return circularPanel;
    }

    private String returnString(JCheckBox checkbox) {
        if (checkbox.isSelected()) {
            return "";
        } else {
            return ";;";
        }
    }

    private void copyFile(String[] result, String[] advanced) {
        try {
            String sourceFilePath = "advtempscript.txt";
            String targetFilePath = "script.txt";
            FileReader fileReader = new FileReader(sourceFilePath);
            BufferedReader bufferedReader = new BufferedReader(fileReader);
            BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(targetFilePath));

            // Read content from source file
            StringBuilder content = new StringBuilder();
            String line;
            while ((line = bufferedReader.readLine()) != null) {
                line = line.replace("%__1__%", result[0]);
                line = line.replace("%__2__%", result[1]);
                line = line.replace("%__3__%", result[2]);
                line = line.replace("%__4__%", result[3]);
                line = line.replace("%__5__%", result[4]);
                line = line.replace("%__6__%", result[5]);
                line = line.replace("%__7__%", result[6]);
                line = line.replace("%__8__%", result[7]);
                line = line.replace("%__9__%", result[8]);
                line = line.replace("%__10__%", result[9]);
                line = line.replace("%__11__%", result[10]);
                line = line.replace("%__12__%", result[11]);
                line = line.replace("%__13__%", result[12]);
                line = line.replace("%__14__%", result[13]);
                line = line.replace("%__lf__%", result[14]);
                line = line.replace("%__ef__%", result[15]);
                line = line.replace("%__a1__%", advanced[0]);
                line = line.replace("%__a2__%", advanced[1]);
                line = line.replace("%__a3__%", advanced[2]);
                line = line.replace("%__a4__%", advanced[3]);
                line = line.replace("%__a5__%", advanced[4]);
                // line = line.replace("%__a6__%", advanced[5]);
                line = line.replace("%__a7__%", advanced[6]);
                line = line.replace("%__a8__%", advanced[7]);
                line = line.replace("%__a9__%", advanced[8]);
                line = line.replace("%__a10__%", advanced[9]);
                line = line.replace("%__a11__%", advanced[10]);
                line = line.replace("%__a12__%", advanced[11]);
                line = line.replace("%__a13__%", advanced[12]);
                line = line.replace("%__a14__%", advanced[13]);
                line = line.replace("%__a15__%", advanced[14]);
                line = line.replace("%__a16__%", advanced[15]);
                line = line.replace("%__a17__%", advanced[16]);
                line = line.replace("%__a18__%", advanced[17]);
                line = line.replace("%__a19__%", advanced[18]);
                line = line.replace("%__a20__%", advanced[19]);
                line = line.replace("%__c__%", advanced[20]);

                // Write the modified line to the target file
                bufferedWriter.write(line);
                bufferedWriter.newLine();

                content.append(line).append("\n");
            }
            bufferedReader.close();
            fileReader.close();
            bufferedWriter.close();

            JOptionPane.showMessageDialog(this, "Script generated successfully!\nRunning Code...");

            runTool();

        } catch (IOException ex) {
            ex.printStackTrace();
            JOptionPane.showMessageDialog(this, "Error copying file: " + ex.getMessage(), "Error",
                    JOptionPane.ERROR_MESSAGE);

        }
    }

    private void runTool() {
        try {
            ProcessBuilder processBuilder = new ProcessBuilder("fusion", "-b",
                    "script.txt");
            processBuilder.redirectErrorStream(true);
            Process process = processBuilder.start();

            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            StringBuilder ans = new StringBuilder();

            JTextArea showTerminalOutput = new JTextArea();
            showTerminalOutput.setEditable(false); // To make it read-only

            // Create Swing components on the EDT
            SwingUtilities.invokeLater(() -> {
                JFrame showProcessOutput = new JFrame();
                showProcessOutput.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                showProcessOutput.setResizable(true);
                showProcessOutput.setTitle("Tool Running");
                showProcessOutput.setSize(1100, 500);
                showProcessOutput.add(new JScrollPane(showTerminalOutput));
                showProcessOutput.setVisible(true);
            });

            // Read output from the process and update the JTextArea on the EDT
            String line;
            while ((line = reader.readLine()) != null) {
                ans.append(line).append("\n");
                final String lineToAppend = line;
                SwingUtilities.invokeLater(() -> {
                    showTerminalOutput.append(lineToAppend + "\n");
                });
            }

            process.waitFor();

        } catch (IOException | InterruptedException ex) {
            ex.printStackTrace();
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            GUIForExecutable example = new GUIForExecutable();
            example.setVisible(true);
        });
    }

}