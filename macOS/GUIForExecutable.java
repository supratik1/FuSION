import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.text.*;
import java.io.BufferedWriter;
import java.io.FileWriter;

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
import java.awt.image.BufferedImage;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

public class GUIForExecutable extends JFrame {
    // private String[] advanced = { "1", "30000", "150000", "1", "1", "", "", "", "", "", "", "", "", "",
    //         "./common_files/inter_db_mapping_file", "./common_files/hsa_to_gene_symbol_map_6334.txt",
    //         "./common_files/hsa_path_to_path_name_map_2822.txt", "",
    //         "./common_files/merged_kegg_master_graph.xml", "./common_files/hsa_ids_not_to_be_merged.txt", ";;", "",
    //         "" };
    private String[] advanced;
    private String[] result;
    int heartbeatinterval;

    public GUIForExecutable() {
        setTitle("FuSION");
        setSize(1900, 1050);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        takeInputs();
    }

    private ImageIcon infoIcon() {
        Icon originalIcon = UIManager.getIcon("OptionPane.informationIcon");
        BufferedImage bufferedImage = new BufferedImage(
                originalIcon.getIconWidth(),
                originalIcon.getIconHeight(),
                BufferedImage.TYPE_INT_ARGB);
        Graphics g = bufferedImage.getGraphics();
        originalIcon.paintIcon(null, g, 0, 0);
        g.dispose();

        int scaledWidth = originalIcon.getIconWidth() / 2; // Adjust the scale factor as needed
        int scaledHeight = originalIcon.getIconHeight() / 2; // Adjust the scale factor as needed
        Image scaledImage = bufferedImage.getScaledInstance(scaledWidth, scaledHeight, Image.SCALE_SMOOTH);
        ImageIcon scaledIcon = new ImageIcon(scaledImage);

        return scaledIcon;
    }

    private void takeInputs() {
        File advinitFile = new File("RuntimeData/advanced.txt");
        if(advinitFile.exists() && !advinitFile.isDirectory()) { 
                    String[] arr = new String[23];
                    String flag="";
                try {
                    BufferedReader readadvanced = new BufferedReader(new FileReader("RuntimeData/advanced.txt"));
                    String line;
                    int i =0;
                    
                    while((line = readadvanced.readLine())!=null) {
                        flag+=(line);
                        arr[i] = line;
                        i++;
                    }
                    // Close the BufferedReader object
                    readadvanced.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }

                if(!flag.equals("")){
                    advanced = arr;
                }
                else{
                     advanced = new String[]{"1", "30000", "150000", "1", "1", "", "", "", "", "", "", "", "", "",
                            "./common_files/inter_db_mapping_file", "./common_files/hsa_to_gene_symbol_map_6334.txt",
                            "./common_files/hsa_path_to_path_name_map_2822.txt", "",
                            "./common_files/merged_kegg_master_graph.xml", "./common_files/hsa_ids_not_to_be_merged.txt", ";;", "",
                            "" };
                }
        }

        JPanel panel = new JPanel(new GridLayout(1, 2, 10, 10)); // 1 row, 2 columns
        panel.setBorder(new EmptyBorder(20, 20, 20, 20)); // Add an EmptyBorder to provide margin

        JPanel lp = new JPanel(new GridLayout(11, 2, 10, 10));
        JTextField inputField1 = new JTextField();
        Font f1 = inputField1.getFont();
        Font lg1 = new Font(Font.SANS_SERIF, f1.getStyle(), f1.getSize() + 6);
        inputField1.setFont(lg1);
        JTextField inputField2 = new JTextField();
        Font f2 = inputField2.getFont();
        Font lg2 = new Font(Font.SANS_SERIF, f2.getStyle(), f2.getSize() + 6);
        inputField2.setFont(lg2);
        JTextField inputField3 = new JTextField();
        Font f3 = inputField3.getFont();
        Font lg3 = new Font(Font.SANS_SERIF, f3.getStyle(), f3.getSize() + 6);
        inputField3.setFont(lg3);
        JTextField inputField4 = new JTextField();
        Font f4 = inputField4.getFont();
        Font lg4 = new Font(Font.SANS_SERIF, f4.getStyle(), f4.getSize() + 6);
        inputField4.setFont(lg4);
        JTextField inputField5 = new JTextField();
        Font f5 = inputField5.getFont();
        Font lg5 = new Font(Font.SANS_SERIF, f5.getStyle(), f5.getSize() + 6);
        inputField5.setFont(lg5);
        JTextField inputFieldlf = new JTextField();
        Font flf = inputFieldlf.getFont();
        Font lglf = new Font(Font.SANS_SERIF, flf.getStyle(), flf.getSize() + 6);
        inputFieldlf.setFont(lglf);
        // JTextField inputFieldef = new JTextField();
        // Font fef = inputFieldef.getFont();
        // Font lgef = new Font(Font.SANS_SERIF, fef.getStyle(), fef.getSize() + 6);
        // inputFieldef.setFont(lgef);

        // Set margins for input fields to add space between the border and text
        Insets margin = new Insets(3, 7, 3, 7); // Top, left, bottom, right
        inputField1.setMargin(margin);
        inputField2.setMargin(margin);
        inputField3.setMargin(margin);
        inputField4.setMargin(margin);
        inputField5.setMargin(margin);
        inputFieldlf.setMargin(margin);
        // inputFieldef.setMargin(margin);

        ButtonGroup optionGroup = new ButtonGroup();
        JRadioButton radioButton1 = new JRadioButton("Activation");
        Font larger = new Font(radioButton1.getFont().getName(), radioButton1.getFont().getStyle(),
                radioButton1.getFont().getSize() + 4);
        radioButton1.setFont(larger);
        radioButton1.setToolTipText("Activation");
        JRadioButton radioButton2 = new JRadioButton("Expression");
        Font larger2 = new Font(radioButton2.getFont().getName(), radioButton2.getFont().getStyle(),
                radioButton2.getFont().getSize() + 4);
        radioButton2.setFont(larger2);
        radioButton2.setToolTipText("Expression");
        JRadioButton radioButton3 = new JRadioButton("Both");
        Font larger3 = new Font(radioButton3.getFont().getName(), radioButton3.getFont().getStyle(),
                radioButton3.getFont().getSize() + 4);
        radioButton3.setFont(larger3);
        radioButton3.setToolTipText("Both");

        optionGroup.add(radioButton1);
        optionGroup.add(radioButton2);
        optionGroup.add(radioButton3);

        JButton b = new JButton("Choose");
        Font defaultFont = b.getFont();
        Font largerFont = new Font(Font.MONOSPACED, defaultFont.getStyle(), defaultFont.getSize() + 4);
        b.setFont(largerFont);

        b.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseDirectory(inputField1);
            }
        });

        JPanel bPanel = new JPanel(new GridLayout(2, 3));
        bPanel.add(b);
        bPanel.add(new JLabel());
        bPanel.add(new JLabel());

        JButton b1 = new JButton("Choose");
        Font defaultFont1 = b1.getFont();
        Font lar = new Font(Font.MONOSPACED, defaultFont1.getStyle(), defaultFont1.getSize() + 4);
        b1.setFont(lar);
        b1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inputFieldlf);
            }
        });

        JPanel b1Panel = new JPanel(new GridLayout(2, 3));
        b1Panel.add(b1);
        b1Panel.add(new JLabel());
        b1Panel.add(new JLabel());

        // JButton b2 = new JButton("Choose");
        // b2.addActionListener(new ActionListener() {
        // @Override
        // public void actionPerformed(ActionEvent e) {
        // chooseFile(inputFieldef);
        // }
        // });

        // JPanel b2Panel = new JPanel(new GridLayout(1, 3));
        // b2Panel.add(b2);
        // b2Panel.add(new JLabel());
        // b2Panel.add(new JLabel());

        JLabel j1 = new JLabel("Working Directory");
        j1.setToolTipText(
                "Working Directory is where all your documents and files necessary to run the tool is stored.");
        Font def1 = j1.getFont();
        Font largerFont1 = new Font(Font.SANS_SERIF, Font.BOLD, def1.getSize() + 6);
        j1.setFont(largerFont1);
        lp.add(j1);
        lp.add(inputField1);
        lp.add(new JLabel());
        lp.add(bPanel);
        JLabel j2 = new JLabel("Log Fold Changes File");
        Font def2 = j2.getFont();
        Font largerFont2 = new Font(Font.SANS_SERIF, Font.BOLD, def2.getSize() + 6);
        j2.setFont(largerFont2);
        lp.add(j2);
        lp.add(inputFieldlf);
        lp.add(new JLabel());
        lp.add(b1Panel);
        // lp.add(new JLabel("Additional Edges File"));
        // lp.add(inputFieldef);
        // lp.add(new JLabel());
        // lp.add(b2Panel);
        JLabel j3 = new JLabel("Source hsa ID");
        Font def3 = j3.getFont();
        Font largerFont3 = new Font(Font.SANS_SERIF, Font.BOLD, def3.getSize() + 6);
        j3.setFont(largerFont3);
        lp.add(j3);
        lp.add(inputField2);
        JLabel j4 = new JLabel("Target hsa ID");
        Font def4 = j4.getFont();
        Font largerFont4 = new Font(Font.SANS_SERIF, Font.BOLD, def4.getSize() + 6);
        j4.setFont(largerFont4);
        lp.add(j4);
        lp.add(inputField3);
        JLabel j5 = new JLabel("Target Edges");
        Font def5 = j5.getFont();
        Font largerFont5 = new Font(Font.SANS_SERIF, Font.BOLD, def5.getSize() + 6);
        j5.setFont(largerFont5);
        lp.add(j5);
        lp.add(createCircularPanel(radioButton1, radioButton2, radioButton3));
        JLabel j6 = new JLabel("Candidate Implicated Node hsa ID");
        Font def6 = j6.getFont();
        Font largerFont6 = new Font(Font.SANS_SERIF, Font.BOLD, def6.getSize() + 6);
        j6.setFont(largerFont6);
        lp.add(j6);
        lp.add(inputField4);
        JLabel j7 = new JLabel("Reach Path Bound");
        Font def7 = j7.getFont();
        Font largerFont7 = new Font(Font.SANS_SERIF, Font.BOLD, def7.getSize() + 6);
        j7.setFont(largerFont7);
        lp.add(j7);
        lp.add(inputField5);
        lp.add(new JLabel());
        JSlider slider = new JSlider(JSlider.HORIZONTAL, 0, 100, 0);
        slider.setMajorTickSpacing(10);
        slider.setMinorTickSpacing(1);
        slider.setPaintTicks(true);
        slider.setPaintLabels(true);
        slider.setForeground(Color.black);
        slider.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                // Update the label with the current slider value
                int sliderValue = ((JSlider) e.getSource()).getValue();
                inputField5.setText("" + sliderValue);
            }
        });
        lp.add(slider);

        JPanel rp = new JPanel(new GridLayout(15, 2, 5, 5));
        JLabel l = new JLabel("Node Relaxation Bounds :");
        l.setForeground(Color.BLUE);
        l.setFont(largerFont7);
        rp.add(l);
        rp.add(new JLabel());
        JLabel rp1 = new JLabel("Upper Bound");
        rp1.setFont(largerFont7);
        rp.add(rp1);
        JTextField inputField9 = new JTextField();
        inputField9.setFont(lg1);
        rp.add(inputField9);
        rp.add(new JLabel());
        JSlider slider2 = new JSlider(JSlider.HORIZONTAL, 0, 150, 0);
        slider2.setMajorTickSpacing(20);
        slider2.setMinorTickSpacing(5);
        slider2.setPaintTicks(true);
        slider2.setPaintLabels(true);
        slider2.setForeground(Color.black);
        slider2.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                // Update the label with the current slider value
                int sliderValue = ((JSlider) e.getSource()).getValue();
                inputField9.setText("" + sliderValue);
            }
        });
        rp.add(slider2);
        JLabel rp2 = new JLabel("Lower Bound");
        rp2.setFont(largerFont7);
        rp.add(rp2);
        JTextField inputField8 = new JTextField();
        inputField8.setFont(lg1);
        rp.add(inputField8);
        rp.add(new JLabel());
        JSlider slider3 = new JSlider(JSlider.HORIZONTAL, 0, 150, 0);
        slider3.setMajorTickSpacing(20);
        slider3.setMinorTickSpacing(5);
        slider3.setPaintTicks(true);
        slider3.setPaintLabels(true);
        slider3.setForeground(Color.black);
        slider3.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                // Update the label with the current slider value
                int sliderValue = ((JSlider) e.getSource()).getValue();
                inputField8.setText("" + sliderValue);
            }
        });
        rp.add(slider3);
        JLabel m = new JLabel("Edge Relaxation Bounds :");
        m.setForeground(Color.BLUE);
        m.setFont(largerFont7);
        rp.add(m);
        rp.add(new JLabel());
        JTextField inputField7 = new JTextField();
        inputField7.setFont(lg1);
        JLabel rp3 = new JLabel("Upper Bound");
        rp3.setFont(largerFont7);
        rp.add(rp3);
        rp.add(inputField7);
        JTextField inputField6 = new JTextField();
        inputField6.setFont(lg1);
        rp.add(new JLabel());
        JSlider slider4 = new JSlider(JSlider.HORIZONTAL, 0, 150, 0);
        slider4.setMajorTickSpacing(20);
        slider4.setMinorTickSpacing(5);
        slider4.setPaintTicks(true);
        slider4.setPaintLabels(true);
        slider4.setForeground(Color.black);
        slider4.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                // Update the label with the current slider value
                int sliderValue = ((JSlider) e.getSource()).getValue();
                inputField7.setText("" + sliderValue);
            }
        });
        rp.add(slider4);
        JLabel rp4 = new JLabel("Lower Bound");
        rp4.setFont(largerFont7);
        rp.add(rp4);
        rp.add(inputField6);
        rp.add(new JLabel());
        JSlider slider5 = new JSlider(JSlider.HORIZONTAL, 0, 150, 0);
        slider5.setMajorTickSpacing(20);
        slider5.setMinorTickSpacing(5);
        slider5.setPaintTicks(true);
        slider5.setPaintLabels(true);
        slider5.setForeground(Color.black);
        slider5.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent e) {
                // Update the label with the current slider value
                int sliderValue = ((JSlider) e.getSource()).getValue();
                inputField6.setText("" + sliderValue);
            }
        });
        rp.add(slider5);
        JLabel rp5 = new JLabel("Up Regulatory Threshold");
        rp5.setFont(largerFont7);
        rp.add(rp5);
        JTextField inputField12 = new JTextField();
        inputField12.setFont(lg1);
        rp.add(inputField12);
        JLabel rp6 = new JLabel("Down Regulatory Threshold");
        rp6.setFont(largerFont7);
        rp.add(rp6);
        JTextField inputField13 = new JTextField();
        inputField13.setFont(lg1);
        rp.add(inputField13);

        JLabel rp7 = new JLabel("Heartbeat Interval");
        rp7.setFont(largerFont7);
        rp.add(rp7);
        JTextField inputField14 = new JTextField();
        inputField14.setFont(lg1);
        rp.add(inputField14);

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

        // ############################################ check for existing inputs #########################################
        File f = new File("RuntimeData/result.txt");
        if(f.exists() && !f.isDirectory()) { 
    // do something
                String[] arr = new String[16];
                try {
                    BufferedReader readresult = new BufferedReader(new FileReader("RuntimeData/result.txt"));
                    String line;
                    int i =0;
                    while((line = readresult.readLine())!=null) {
                        //System.out.println(line);
                        //line = readresult.readLine();
                        arr[i] = line;
                        i++;
                    }
                    // Close the BufferedReader object
                    readresult.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }

                result = Arrays.copyOf(arr,arr.length -1);
                if(arr[15]!=null)
                    heartbeatinterval = Integer.valueOf(arr[15]);
                else
                    heartbeatinterval = 10000;
                // result = new String[] { inputField1.getText(), inputField2.getText(), inputField3.getText(),
                //     inputField4.getText(),
                //     inputField5.getText(), inputField6.getText(), inputField7.getText(), inputField8.getText(),
                //     inputField9.getText(), getFileName(inputFieldlf), "",
                //     inputField12.getText(),
                //     inputField13.getText(),
                //     getOutput(radioButton1, radioButton2, radioButton3),
                //     inputFieldlf.getText(), "" };

                inputField1.setText(result[0]);
                inputField2.setText(result[1]);
                inputField3.setText(result[2]);
                inputField4.setText(result[3]);
                inputField5.setText(result[4]);
                inputField6.setText(result[5]);
                inputField7.setText(result[6]);
                inputField8.setText(result[7]);
                inputField9.setText(result[8]);
                inputFieldlf.setText(result[14]);
                //""
                inputField12.setText(result[11]);
                inputField13.setText(result[12]);
                inputField14.setText(Integer.toString(heartbeatinterval));
                //heartbeat
                //inputField14.setText(arr[15]);
                //$ONLY_ACT_EDGES_TO_TARGET if active radio button is pressed
                //$BOTH something is 'both' radio button is pressed
                //$ONLY_EXPR_EDGES_TO_TARGET is expr radio button is pressed

                //String decide = result[13].substring(ABORT)
                if(result[13].length()>=5){
                    if(result[13].substring(0,5).equals("$BOTH")){
                        radioButton3.setSelected(true);
                    }
                    else if(result[13].substring(0,8).equals("$ONLY_ACT")){
                        radioButton1.setSelected(true);
                    }
                    else if(result[13].substring(0,8).equals("$ONLY_EXP")){
                        radioButton2.setSelected(true);
                    }
                }

        }
        else{
            heartbeatinterval = 10000;
            inputField14.setText("10000");
        }

        // JRadioButton usepreadv = new JRadioButton("Use previous advanced inputs");
        // lp.add(usepreadv);
        // lp.add(createCircularPanel(usepreadv));

        JButton informationButton = new JButton("About the Tool");
        informationButton.setFont(largerFont7);
        informationButton.setBackground(Color.black);
        informationButton.setForeground(Color.white);
        informationButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                aboutTheTool();
            }
        });

        JButton advInputs = new JButton("Advanced Inputs");
        advInputs.setFont(largerFont7);
        advInputs.setBackground(Color.BLACK);
        advInputs.setForeground(Color.white);
        lp.add(informationButton);
        lp.add(advInputs);
        advInputs.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                advanced = advancedInputs();
            }
        });

        rp.add(new JLabel());
        rp.add(new JLabel());



        JButton saveButton = new JButton("Save Inputs Before Run");
        saveButton.setBackground(Color.RED);
        saveButton.setForeground(Color.WHITE);
        saveButton.setFont(largerFont7);
        rp.add(saveButton);

        saveButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                result = new String[] { inputField1.getText(), inputField2.getText(), inputField3.getText(),
                        inputField4.getText(),
                        inputField5.getText(), inputField6.getText(), inputField7.getText(), inputField8.getText(),
                        inputField9.getText(), getFileName(inputFieldlf), "",
                        inputField12.getText(),
                        inputField13.getText(),
                        getOutput(radioButton1, radioButton2, radioButton3),
                        inputFieldlf.getText(), "" };
                heartbeatinterval = Integer.valueOf(inputField14.getText());
            }
        });

        JButton rButton = new JButton("RUN");
        rButton.setFont(largerFont7);
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

                try {
                    BufferedWriter writeresult = new BufferedWriter(new FileWriter("RuntimeData/result.txt"));
                    writeresult.write(result[0]);
                    for(int i=1;i<result.length;i++){
                        writeresult.write("\n" + result[i]);
                    }
                    writeresult.write(inputField14.getText());
                    writeresult.close();

                } catch (IOException ex) {
                    ex.printStackTrace();
                }

                try {
                    BufferedWriter writeadvanced = new BufferedWriter(new FileWriter("RuntimeData/advanced.txt"));
                    writeadvanced.write(advanced[0]);
                    for(int i=1;i<advanced.length;i++){
                        writeadvanced.write("\n" + advanced[i]);
                    }
                    writeadvanced.close();

                } catch (IOException ex) {
                    ex.printStackTrace();
                }

                copyFile(result, advanced);
            }
        });


    }

    private void aboutTheTool() {
        JFrame info = new JFrame();
        String title = "FuSION : Functional Significance In nOisy Networks\n\n";
        String devby = "FuSION was developed at IIT Bombay jointly by:\n"
                + "1. S. Akshay, Sukanya Basu and Supratik Chakraborty (IIT Bombay)\n"
                + "2. Rangapriya Sundararajan and Prasanna Venkatraman (ACTREC, Tata Memorial Centre)\n"
                + "3. With help from Jahnabi Roy (IIT Delhi)\n\n";
        String what = "What does FuSION do?\n\n";
        String inf = "Given a collection of gene regulatory networks (currently KEGG networks), microarray reads from a wet-lab experiment, and stimulus, observation-of-interest and a suspected actor in the experiment, FuSION uses a combination of biological domain knowledge encoded in the networks, state-of-the-art SMT solving and Pareto theory to determine if the suspected actor plays a functionally signficant role in the stimuls-to-observation signaling circuits present in the given regulatory networks, in a manner that is consistent with the microarray reads. The stimulus and observation-of-interest must be mapped to activated/deactivated/expressed/repressed states of suitable nodes in the regulatory networks for FuSION to do its reasoning. The tool is best used as a computational filter to zoom down on likely actors (or equivalently, filter out unlikely actors) playing significant functional roles in wet-lab experiments.\n\n";
        String inf2 = "Since microarray data is susceptible to some noise-induced errors, and since publicly available gene regulatory networks may have some errors or imprecisely captured relations, it is important that such noise/errors are factored in before we implicate a suspect actor as playing a functionally significant role. At the same time, it is also important to realize that microarray and sequencing technology is fairly advanced today, and publicly available gene regulatory networks are periodically curated and revised to remove errors. Therefore, while we must allow the possibility of some noise/errors, it is unreasonable to expect overwhelmingly large noise in microarray data or massive errors in publicly available gene regulatory networks. In cognizance of this, FuSION allows the user to specify two kinds of bounded relaxations:\n";
        String pt1 = ">> A bounded number of edges in the networks may be considered to be in error. The user need not specify which specific edges are in error, and FuSION can search over all bounded subsets of edges that can potentially be in error. If the user knows that certain network edges are definitely not in error, she/he/they can specify these explicitly to FuSION. In this case, such edges will never be considered as erroneous in the search conducted by FuSION.\n";
        String pt2 = ">> A bounded number of microarray reads may be considered to be in error. The current version of FuSION works by thresholding microarray reads using user-provided threshold values to classify actors as differentially expressed, differentially repressed or unknown. A relaxation of microarray reads allows a bounded number of differentially expressed or differentially repressed actors to have their classification altered. The user need not specify which specific reads are in error, and FuSION searches over all bounded subsets of differentially expressed reads that can potentially be in error. If the user knows that some differentially expressed reads are definitely not in error, she/he.they can specify these explicitly to FuSION. The specified reads will then be excluded from the subsets of potentially erroneous reads considered by FuSION.\n\n";
        String last = "FuSION tries to determine whether stimulus-to-observation signaling circuits can be extracted from the provided gene regulatory networks, while being consistent with the microarray reads (modulo bounded relaxations, as discussed above). For example, FuSION may find that removing an suspected actor from the regulatory networks makes it impossible to extract stimulus-to-observation signaling circuits unless larger bounds of relaxations are used for network edges and/or microarray reads. In such a case, the implicated actor is likely to play a functionally significant positive role in the stimulus-to-observation signaling. Similarly, if removing the actor makes it possible to extract stimulus-to-observation signaling circuits with smaller bounds of relaxations than those necessary when the actor was present, the actor potentially plays a functionally significant negative role in the stimulus-to-observation signaling.\n\n";
        String interp = "Interpreting FuSION's outputs\n\n";
        String inter = "FuSION presents the results of its reasoning as a pair of Pareto curves to the user. One of these curves is constructed with the suspect actor present in the regulatory networks, while the other is constructed without the suspect actor. Each curve describes the minimum relaxations necessary to allow the discovery of signaling circuits from stimulus to observation. If the generated pair of curves don't overlap or intersect, it is reasonable to infers that the suspect actor plays a functionally significant role in the signaling from stimulus to observation, given the noise bounds provided. In case the curves coincide or intersect, one cannot infer the functional significance of the implicated actor within the given noise bounds. In such cases, it is possible that the suspect actor plays no significant role in the signaling circuits, or the actor may play a significant role in some signaling circuits, although compensatory signaling circuits may be operational in the absence of the actor. To probe further in such cases, we may need to specify that certain other actors/signaling edges must/must not be present in the stimulus-to-observation signaling circuits (FuSION allows such specification) and the computational experiments repeated with and without the suspected actor being present.\n\n";
        JTextArea information = new JTextArea(
                title + devby + "--> " + what + inf + inf2 + "\n" + pt1 + pt2 + last + "--> " + interp + inter);
        information.setLineWrap(true);
        information.setWrapStyleWord(true);
        information.setBackground(Color.BLACK);
        information.setForeground(Color.WHITE);
        information.setBorder(new EmptyBorder(20, 20, 20, 20));
        JScrollPane scrollPane = new JScrollPane(information);
        info.add(scrollPane);
        Font infoFont = information.getFont();
        Font large = new Font(Font.MONOSPACED, infoFont.getStyle(), infoFont.getSize() + 5);
        information.setFont(large);
        info.setTitle("About the Tool");
        info.setSize(1000, 800);
        info.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        info.setLocationRelativeTo(null); // Center the frame on the screen
        info.setVisible(true);
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
        Font def1Font = inField1.getFont();
        Font larg = new Font(Font.SANS_SERIF, def1Font.getStyle(), def1Font.getSize() + 4);
        inField1.setFont(larg);
        inField1.setMargin(margin2);
        JLabel ad1 = new JLabel("Node Split Threshold");
        ad1.setFont(larg);
        leftPanel.add(ad1);
        leftPanel.add(inField1);
        leftPanel.add(new JLabel());
        JTextField inField2 = new JTextField("30000");
        inField2.setFont(larg);
        inField2.setMargin(margin2);
        JLabel ad2 = new JLabel("Increment Solver Timeout");
        ad2.setFont(larg);
        leftPanel.add(ad2);
        leftPanel.add(inField2);
        leftPanel.add(new JLabel());
        JTextField inField3 = new JTextField("150000");
        inField3.setFont(larg);
        inField3.setMargin(margin2);
        JLabel ad3 = new JLabel("Overall Solver Timeout");
        ad3.setFont(larg);
        leftPanel.add(ad3);
        leftPanel.add(inField3);
        leftPanel.add(new JLabel());
        JTextField inField4 = new JTextField("1");
        inField4.setFont(larg);
        inField4.setMargin(margin2);
        JLabel ad4 = new JLabel("Solutions to Count");
        ad4.setFont(larg);
        leftPanel.add(ad4);
        leftPanel.add(inField4);
        leftPanel.add(new JLabel());
        JTextField inField5 = new JTextField("1");
        inField5.setFont(larg);
        inField5.setMargin(margin2);
        JLabel ad5 = new JLabel("Solutions to Explore");
        ad5.setFont(larg);
        leftPanel.add(ad5);
        leftPanel.add(inField5);
        leftPanel.add(new JLabel());

        JTextField inputFieldef = new JTextField();
        JButton b2 = new JButton("Choose");
        b2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inputFieldef);
            }
        });
        b2.setFont(larg);
        b2.setBackground(Color.BLACK);
        b2.setForeground(Color.WHITE);
        JLabel ad6 = new JLabel("Additional Edges File");
        ad6.setFont(larg);
        inputFieldef.setFont(larg);
        leftPanel.add(ad6);
        leftPanel.add(inputFieldef);
        leftPanel.add(b2);
        JTextField inField15 = new JTextField("../common_files/inter_db_mapping_file");
        inField15.setFont(larg);
        inField15.setMargin(margin2);
        JLabel ad7 = new JLabel("Cross Database Map");
        ad7.setFont(larg);
        leftPanel.add(ad7);
        leftPanel.add(inField15);

        JButton chooseButton1 = new JButton("Choose");
        chooseButton1.setBackground(Color.BLACK);
        chooseButton1.setForeground(Color.WHITE);
        chooseButton1.setFont(larg);
        chooseButton1.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField15);
            }
        });
        leftPanel.add(chooseButton1);

        JTextField inField16 = new JTextField("../common_files/hsa_to_gene_symbol_map_6334.txt");
        inField16.setFont(larg);
        inField16.setMargin(margin2);
        JLabel ad8 = new JLabel("HSA to Gene Symbol Map");
        ad8.setFont(larg);
        leftPanel.add(ad8);
        leftPanel.add(inField16);

        JButton chooseButton2 = new JButton("Choose");
        chooseButton2.setBackground(Color.BLACK);
        chooseButton2.setForeground(Color.WHITE);
        chooseButton2.setFont(larg);
        chooseButton2.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField16);
            }
        });
        leftPanel.add(chooseButton2);

        JTextField inField17 = new JTextField("../common_files/hsa_path_to_path_name_map_2822.txt");
        inField17.setFont(larg);
        inField17.setMargin(margin2);
        JLabel ad9 = new JLabel("HSA to Gene Symbol Map");
        ad9.setFont(larg);
        leftPanel.add(ad9);
        leftPanel.add(inField17);

        JButton chooseButton3 = new JButton("Choose");
        chooseButton3.setBackground(Color.BLACK);
        chooseButton3.setFont(larg);
        chooseButton3.setForeground(Color.WHITE);
        chooseButton3.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField17);
            }
        });
        leftPanel.add(chooseButton3);

        JTextField inField20 = new JTextField("../common_files/hsa_ids_not_to_be_merged.txt");
        inField20.setFont(larg);
        inField20.setMargin(margin2);
        JLabel ad10 = new JLabel("HSA IDs Not to be Merged");
        ad10.setFont(larg);
        leftPanel.add(ad10);
        leftPanel.add(inField20);

        JButton chooseButton4 = new JButton("Choose");
        chooseButton4.setFont(larg);
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
        checkbox.setFont(larg);
        checkbox.setForeground(Color.BLUE);
        leftPanel.add(checkbox); // 21 if checkbox is selected then
        leftPanel.add(new JLabel());

        JTextField inField18 = new JTextField();
        inField18.setFont(larg);
        inField18.setMargin(margin2);
        JLabel ad11 = new JLabel("List of KEGG XML Files to Merge");
        ad11.setFont(larg);
        leftPanel.add(ad11);
        leftPanel.add(inField18);
        JButton chooseButton5 = new JButton("Choose");
        chooseButton5.setFont(larg);
        chooseButton5.setBackground(Color.BLACK);
        chooseButton5.setForeground(Color.WHITE);
        chooseButton5.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField18);
            }
        });
        leftPanel.add(chooseButton5);
        JTextField inField19 = new JTextField("../common_files/merged_kegg_master_graph.xml");
        inField19.setFont(larg);
        inField19.setMargin(margin2);
        JLabel ad12 = new JLabel("XML File of Merged KEGG Pathways");
        ad12.setFont(larg);
        leftPanel.add(ad12);
        leftPanel.add(inField19);
        JButton chooseButton6 = new JButton("Choose");
        chooseButton6.setBackground(Color.BLACK);
        chooseButton6.setFont(larg);
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
                    inField18.setText("../common_files/list_of_kegg_xml_files.txt");
                } else {
                    inField18.setText("");
                }
            }
        });

        panel.add(leftPanel);

        JPanel rightPanel = new JPanel(new GridLayout(10, 3, 5, 5));
        JTextField inField7 = new JTextField();
        inField7.setFont(larg);
        inField7.setMargin(margin2);
        JLabel ad13 = new JLabel("Essential Edge File");
        ad13.setFont(larg);
        rightPanel.add(ad13);
        rightPanel.add(inField7);
        JButton chooseButton7 = new JButton("Choose");
        chooseButton7.setFont(larg);
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
        inField8.setFont(larg);
        inField8.setMargin(margin2);
        JLabel ad14 = new JLabel("Avoid Edge File");
        ad14.setFont(larg);
        rightPanel.add(ad14);
        rightPanel.add(inField8);
        JButton chooseButton8 = new JButton("Choose");
        chooseButton8.setFont(larg);
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
        inField9.setFont(larg);
        inField9.setMargin(margin2);
        JLabel ad15 = new JLabel("Inactive Node File");
        ad15.setFont(larg);
        rightPanel.add(ad15);
        rightPanel.add(inField9);
        JButton chooseButton9 = new JButton("Choose");
        chooseButton9.setFont(larg);
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
        inField10.setFont(larg);
        inField10.setMargin(margin2);
        JLabel ad16 = new JLabel("Confirmed Up Reg File");
        ad16.setFont(larg);
        rightPanel.add(ad16);
        rightPanel.add(inField10);
        JButton chooseButton10 = new JButton("Choose");
        chooseButton10.setFont(larg);
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
        inField11.setFont(larg);
        inField11.setMargin(margin2);
        JLabel ad17 = new JLabel("Confirmed Down Reg File");
        ad17.setFont(larg);
        rightPanel.add(ad17);
        rightPanel.add(inField11);
        JButton chooseButton11 = new JButton("Choose");
        chooseButton11.setFont(larg);
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
        inField12.setFont(larg);
        inField12.setMargin(margin2);
        JLabel ad18 = new JLabel("Relaxed Nodes");
        ad18.setFont(larg);
        rightPanel.add(ad18);
        rightPanel.add(inField12);
        JButton chooseButton12 = new JButton("Choose");
        chooseButton12.setFont(larg);
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
        inField13.setFont(larg);
        inField13.setMargin(margin2);
        JLabel ad19 = new JLabel("Relaxed Edges");
        ad19.setFont(larg);
        rightPanel.add(ad19);
        rightPanel.add(inField13);
        JButton chooseButton13 = new JButton("Choose");
        chooseButton13.setFont(larg);
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
        inField14.setFont(larg);
        inField14.setMargin(margin2);
        JLabel ad20 = new JLabel("Non Relaxed Edges File");
        ad20.setFont(larg);
        rightPanel.add(ad20);
        rightPanel.add(inField14);
        JButton chooseButton14 = new JButton("Choose");
        chooseButton14.setBackground(Color.BLACK);
        chooseButton14.setFont(larg);
        chooseButton14.setForeground(Color.WHITE);
        chooseButton14.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                chooseFile(inField14);
            }
        });
        rightPanel.add(chooseButton14);
        JCheckBox checkbox2 = new JCheckBox("Default Entries");
        checkbox2.setFont(larg);
        checkbox2.setForeground(Color.BLUE);
        rightPanel.add(checkbox2);

        checkbox2.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent e) {
                if (checkbox2.isSelected()) {
                    inField7.setText("../common_files/empty_file");
                    inField8.setText("../common_files/empty_file");
                    inField9.setText("../common_files/empty_file");
                    inField10.setText("../common_files/empty_file");
                    inField11.setText("../common_files/empty_file");
                    inField12.setText("../common_files/empty_file");
                    inField13.setText("../common_files/empty_file");
                    inField14.setText("../common_files/empty_file");
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
                inField20.getText(), ";;", inputFieldef.getText(), getFileName(inputFieldef) };
        // advancedFrame.dispose();
        
         // ############################################ check for existing inputs #########################################
         //File f = new File("RuntimeData/advanced.txt");
         //if(true) { 
                //      String[] arr = new String[23];
                //  try {
                //      BufferedReader readadvanced = new BufferedReader(new FileReader("RuntimeData/advanced.txt"));
                //      String line;
                //      int i =0;
                //      while((line = readadvanced.readLine())!=null) {
                //          //System.out.println(line);
                //          //line = readresult.readLine();
                //          arr[i] = line;
                //          i++;
                //      }
                //      // Close the BufferedReader object
                //      readadvanced.close();
                //  } catch (IOException ex) {
                //      ex.printStackTrace();
                //  }

                //  advanced = arr;

                //  advanced = new String[] { inField1.getText(), inField2.getText(),
                //     inField3.getText(),
                //     inField4.getText(),
                //     inField5.getText(), "", inField7.getText(), inField8.getText(),
                //     inField9.getText(),
                //     inField10.getText(),
                //     inField11.getText(), inField12.getText(), inField13.getText(),
                //     inField14.getText(),
                //     inField15.getText(),
                //     inField16.getText(), inField17.getText(), inField18.getText(),
                //     inField19.getText(),
                //     inField20.getText(), returnString(checkbox), inputFieldef.getText(),
                //     getFileName(inputFieldef) };

                inField1.setText(advanced[0]);
                inField2.setText(advanced[1]);
                inField3.setText(advanced[2]);
                inField4.setText(advanced[3]);
                inField5.setText(advanced[4]);
                //""
                inField7.setText(advanced[6]);
                inField8.setText(advanced[7]);
                inField9.setText(advanced[8]);
                inField10.setText(advanced[9]);
                inField11.setText(advanced[10]);
                inField12.setText(advanced[11]);
                inField13.setText(advanced[12]);
                inField14.setText(advanced[13]);
                inField15.setText(advanced[14]);
                inField16.setText(advanced[15]);
                inField17.setText(advanced[16]);
                inField18.setText(advanced[17]);
                inField19.setText(advanced[18]);
                inField20.setText(advanced[19]);
                //checkbox.setText(advanced[20]);
                inputFieldef.setText(advanced[21]);
                
         

        JButton saveInputs = new JButton("Save Inputs Before Run");
        saveInputs.setFont(larg);
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
                        inField20.getText(), returnString(checkbox), inputFieldef.getText(),
                        getFileName(inputFieldef) };
                advancedFrame.dispose();
            }
        });
        rightPanel.add(saveInputs);
        panel.add(rightPanel);
        advancedFrame.getContentPane().add(panel);

        advancedFrame.setTitle("Advanced Inputs");
        advancedFrame.setFont(larg);
        advancedFrame.setSize(1500, 800);
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
                line = line.replace("%__11__%", advanced[22]);
                line = line.replace("%__12__%", result[11]);
                line = line.replace("%__13__%", result[12]);
                line = line.replace("%__14__%", result[13]);
                line = line.replace("%__lf__%", result[14]);
                line = line.replace("%__ef__%", advanced[21]);
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
        SwingUtilities.invokeLater(() -> {
            try {
                
                ProcessBuilder processBuilder = new ProcessBuilder("./fusion", "-b",
                             "script.txt");
                processBuilder.redirectErrorStream(true);
                Process process = processBuilder.start();

                SwingWorker<Void,Void> showthread = new SwingWorker<Void,Void>() {
            

                    @Override
                    protected Void doInBackground() throws Exception{
                        
                            int hbinterval = heartbeatinterval;
            
                           try{    
                               BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                               StringBuilder ans = new StringBuilder();
                   
                               JTextArea showTerminalOutput = new JTextArea();
                               showTerminalOutput.setEditable(false); 
                           
                               SwingUtilities.invokeLater(() -> {
                                   JFrame showProcessOutput = new JFrame();
                                   showProcessOutput.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                                   showProcessOutput.setResizable(true);
                                   showProcessOutput.setTitle("Tool Running");
                                   showProcessOutput.setSize(1100, 500);
                                   showProcessOutput.add(new JScrollPane(showTerminalOutput));
                                   showProcessOutput.setVisible(true);
                                   
                                   JButton kButton = new JButton("KILL");
                                   kButton.setBackground(Color.BLACK);
                                   kButton.setForeground(Color.red);
                                   kButton.setFocusable(false);
                                   showProcessOutput.add(kButton,BorderLayout.WEST);
                                   kButton.addActionListener(new ActionListener() {
                                       @Override
                                       public void actionPerformed(ActionEvent e) {
                                          
                                           try {
                                               process.destroy();
                                               showProcessOutput.dispose();
                                           } catch (Exception ex) {
                                               ex.printStackTrace();
                                           }
                           
                                       }
                                   });
                               });
                               
                               BufferedWriter writer = new BufferedWriter(new FileWriter("RuntimeData/terminaloutput.txt"));
                              
                               SwingWorker<Void,Void> check = new SwingWorker<Void,Void>() {
                                   String toappend="Processing";
                                   long prevtimestamp = 0;
                                   long timestamp = 1;
                   
                                   @Override
                                   protected Void doInBackground() throws Exception{
                                       boolean b = true;
                                       while(b==true){
                                           String temp = ans.toString();
                                           Thread.sleep(hbinterval);
                                           
                                           if(temp.equals(ans.toString())){
                                               prevtimestamp = timestamp;
                                               File f = new File("limits_timefile.txt");
                                               if(f.exists() && !f.isDirectory()) { 
                                                   timestamp = f.lastModified();
                                               }
                                               
                                               SwingUtilities.invokeLater(() -> {
                                                   if(prevtimestamp==timestamp)
                                                       showTerminalOutput.append("Still processing :" + toappend + "\n");
                                                   else{
                                                       try {
                                                           if(f.exists() && !f.isDirectory()) { 
                                                               timestamp = f.lastModified();
                                                               String timline;
                                                               BufferedReader timelimsreader = new BufferedReader(new FileReader("limits_timefile.txt"));
                       
                                                               while((timline=timelimsreader.readLine()) != null){
                                                                   toappend = timline;
                                                               }
                       
                                                               timelimsreader.close();
                                                               showTerminalOutput.append("Working on a new process! :" + toappend + "\n");
                                                           }
                                                           
                                                       } catch (Exception e) {
                                                           e.printStackTrace();
                                                       }
                                                   }
                                                   
                                               });
                                           }
                                       }
                                       return null;
                                   }
                               };
                              
                               check.execute();
                               String line;
                   
                               while ((line = reader.readLine()) != null) {
                                   writer.write(line + "\n");
                                   final String lineToAppend;
                                   
                                   if(line.length()>=9 && line.substring(0,9).equals("*DISPLAY*")){
                                       lineToAppend = line.substring(9);
                                       
                                   }
                                   else{
                                           lineToAppend="";
                                   }
                                   
                                   ans.append(line).append("\n");
                   
                                   SwingUtilities.invokeLater(() -> {
                                       if(!lineToAppend.equals("")){
                                           showTerminalOutput.append(lineToAppend + "\n");
                                       }
                                   });
                               }
                   
                               writer.close();
                               check.cancel(true);
                               
                           }
                           catch(IOException e){
                               e.printStackTrace();
                           }
                                      
                        return null;
                    }
                };
                
                showthread.execute();               
        
            } catch (IOException ex) {
                 ex.printStackTrace();
            }
        
            });
    }

    /*
     * private void runTool() {
     * try {
     * ProcessBuilder processBuilder = new ProcessBuilder("./fusion", "-b",
     * "script.txt");
     * processBuilder.redirectErrorStream(true);
     * Process process = processBuilder.start();
     * 
     * BufferedReader reader = new BufferedReader(new
     * InputStreamReader(process.getInputStream()));
     * StringBuilder ans = new StringBuilder();
     * 
     * JTextArea showTerminalOutput = new JTextArea();
     * showTerminalOutput.setEditable(false); // To make it read-only
     * 
     * // Create Swing components on the EDT
     * SwingUtilities.invokeLater(() -> {
     * JFrame showProcessOutput = new JFrame();
     * showProcessOutput.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
     * showProcessOutput.setResizable(true);
     * showProcessOutput.setTitle("Tool Running");
     * showProcessOutput.setSize(1100, 500);
     * showProcessOutput.add(new JScrollPane(showTerminalOutput));
     * showProcessOutput.setVisible(true);
     * });
     * 
     * // Create a SwingWorker for background processing
     * SwingWorker<Void, String> worker = new SwingWorker<Void, String>() {
     * 
     * @Override
     * protected Void doInBackground() {
     * try {
     * String line;
     * while ((line = reader.readLine()) != null) {
     * ans.append(line).append("\n");
     * publish(line); // Publish the line for EDT update
     * }
     * } catch (IOException e) {
     * e.printStackTrace();
     * }
     * return null;
     * }
     * 
     * @Override
     * protected void process(java.util.List<String> chunks) {
     * // Update the JTextArea on the EDT
     * for (String line : chunks) {
     * showTerminalOutput.append(line + "\n");
     * }
     * }
     * };
     * 
     * // Start the SwingWorker
     * worker.execute();
     * 
     * // Wait for the process to complete
     * process.waitFor();
     * 
     * } catch (IOException | InterruptedException ex) {
     * ex.printStackTrace();
     * }
     * }
     */

    public static void main(String[] args) {

            try {
                UIManager.setLookAndFeel(
                UIManager.getCrossPlatformLookAndFeelClassName());
            } 
            catch (UnsupportedLookAndFeelException e) {
                e.printStackTrace();
            }
            catch (ClassNotFoundException e) {
                e.printStackTrace();
            }
            catch (InstantiationException e) {
                e.printStackTrace();
            }
            catch (IllegalAccessException e) {
                e.printStackTrace();
            }

        GUIForExecutable example = new GUIForExecutable();
        example.setVisible(true);
    }

}