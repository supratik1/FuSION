
import java.awt.*;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import javax.swing.*;
import javax.swing.event.ChangeEvent;

import org.json.JSONArray;
import org.json.JSONObject;

/**
 * A card that contains a dual-thumb RangeSlider with labels for bounds.
 */
public class RelaxationBoundsPanel extends JPanel {

    private final RangeSlider nodeSlider;
    private final RangeSlider edgeSlider;
    private final JLabel lowerLabel1, lowerLabel2;
    private final JLabel upperLabel, upperLabel2;

    public RelaxationBoundsPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout(10, 10));
        setBorder(BorderFactory.createEmptyBorder(20, 20, 10, 20));
        setBackground(new Color(138, 43, 226));

        int arr[]= user.getRelaxationBounds();

        JLabel title = new JLabel("Node and Edge Relaxation Bounds", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 28));
        title.setForeground(Color.WHITE);
        title.setBackground(new Color(138, 43, 226));
        add(title, BorderLayout.NORTH);

        edgeSlider = new RangeSlider();
        edgeSlider.setMinimum(0);
        edgeSlider.setMaximum(100);
        edgeSlider.setValue(arr[2]);
        edgeSlider.setUpperValue(arr[3]);
        edgeSlider.setOpaque(false);
        edgeSlider.setPaintTicks(true);
        edgeSlider.setPaintLabels(true);
        edgeSlider.setMajorTickSpacing(10); // Adjust as needed
        edgeSlider.setMinorTickSpacing(1);
        edgeSlider.setForeground(Color.WHITE);
        edgeSlider.setPreferredSize(new Dimension(300, edgeSlider.getPreferredSize().height));

        nodeSlider = new RangeSlider();
        nodeSlider.setMinimum(0);
        nodeSlider.setMaximum(100);
        nodeSlider.setValue(arr[0]);
        nodeSlider.setUpperValue(arr[1]);
        nodeSlider.setOpaque(false);
        nodeSlider.setPaintTicks(true);
        nodeSlider.setPaintLabels(true);
        nodeSlider.setMajorTickSpacing(10); // Adjust as needed
        nodeSlider.setMinorTickSpacing(1);
        nodeSlider.setForeground(Color.WHITE);
        nodeSlider.setPreferredSize(new Dimension(200, nodeSlider.getPreferredSize().height));

        lowerLabel1 = new JLabel("Lower Bound: " + nodeSlider.getValue());
        lowerLabel1.setForeground(Color.WHITE);
        upperLabel = new JLabel("Upper Bound: " + nodeSlider.getUpperValue());
        upperLabel.setForeground(Color.WHITE);

        lowerLabel2 = new JLabel("Lower Bound: " + edgeSlider.getValue());
        upperLabel2 = new JLabel("Upper Bound: " + edgeSlider.getUpperValue());
        lowerLabel2.setForeground(Color.WHITE);
        upperLabel2.setForeground(Color.WHITE);

        nodeSlider.addChangeListener((ChangeEvent e) -> {
            lowerLabel1.setText("Lower Bound: " + nodeSlider.getValue());
            upperLabel.setText("Upper Bound: " + nodeSlider.getUpperValue());
            arr[0]= nodeSlider.getValue();
            arr[1]= nodeSlider.getUpperValue();
            
            // System.out.println(arr);
        });

        edgeSlider.addChangeListener((ChangeEvent e) -> {
            lowerLabel2.setText("Lower Bound: " + edgeSlider.getValue());
            upperLabel2.setText("Upper Bound: " + edgeSlider.getUpperValue());
            arr[2]= edgeSlider.getValue();
            arr[3]= edgeSlider.getUpperValue();
            
            // System.out.println(arr);
        });

        JPanel nodeSliderPanel = new JPanel();
        nodeSliderPanel.setLayout(new BoxLayout(nodeSliderPanel, BoxLayout.Y_AXIS));
        nodeSliderPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.WHITE), "Node Relaxation Bounds", 0, 0, null, Color.WHITE));
        nodeSliderPanel.add(lowerLabel1);
        nodeSliderPanel.add(upperLabel);
        nodeSliderPanel.add(Box.createVerticalStrut(30));
        nodeSliderPanel.add(nodeSlider);
        nodeSliderPanel.setBackground(new Color(138, 43, 226));

        JPanel edgeSliderPanel = new JPanel();
        edgeSliderPanel.setLayout(new BoxLayout(edgeSliderPanel, BoxLayout.Y_AXIS));
        edgeSliderPanel.setBorder(BorderFactory.createTitledBorder(BorderFactory.createLineBorder(Color.WHITE), "Edge Relaxation Bounds", 0, 0, null, Color.WHITE));
        edgeSliderPanel.add(lowerLabel2);
        edgeSliderPanel.add(upperLabel2);
        edgeSliderPanel.add(Box.createVerticalStrut(30));
        edgeSliderPanel.add(edgeSlider);
        edgeSliderPanel.setBackground(new Color(138, 43, 226));

        JPanel sliderPanel = new JPanel(new GridLayout(2, 1));
        sliderPanel.add(nodeSliderPanel);
        sliderPanel.add(edgeSliderPanel);
        sliderPanel.setBackground(new Color(138, 43, 226));

        JButton nextButton = new JButton("Next");
        nextButton.addActionListener(e -> {
            // ReachPathBoundCard reachPathBound = new ReachPathBoundCard(cardLayout, parentPanel);
            // parentPanel.add(reachPathBound, "reachPath");
            // cardLayout.show(parentPanel, "reachPath");
            user.setRelaxationBounds(arr);

            ThresholdFilterPanel threshold= new ThresholdFilterPanel(cardLayout, cardPanel, user);
            cardPanel.add(threshold, "threshold");
            cardLayout.show(cardPanel, "threshold");

           
        });

        JButton prevButton = new JButton("Prev");
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "reachBound"));

        JButton saveButton= new JButton("Save");
        saveButton.addActionListener(e->{
            user.setRelaxationBounds(arr);
            String fileName = "sessions/" + user.getUsername() + ".json";
            
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

                sessionData.put("nodeLBound", arr[0]);
                sessionData.put("nodeUBound", arr[1]);
                sessionData.put("edgeLBound", arr[2]);
                sessionData.put("edgeUBound", arr[3]);
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

        JPanel bottomPanel = new JPanel(new BorderLayout());
        bottomPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        bottomPanel.add(new JPanel(){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }}, BorderLayout.SOUTH);
        
        bottomPanel.setBackground(new Color(138, 43, 226));

        add(sliderPanel, BorderLayout.CENTER);
        add(bottomPanel, BorderLayout.SOUTH);
    }

    
}
