import java.awt.*;
import javax.swing.*;
import javax.swing.event.ChangeEvent;

/**
 * A card that contains a dual-thumb RangeSlider with labels for bounds.
 */
public class RelaxationBoundsPanel extends RoundedPanel {

    private final RangeSlider nodeSlider;
    private final RangeSlider edgeSlider;
    private final JLabel lowerLabel1, lowerLabel2;
    private final JLabel upperLabel, upperLabel2;

    public RelaxationBoundsPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout(10, 10));
        setBackground(Color.WHITE);

        int arr[] = user.getRelaxationBounds();

        HeaderPanel header = new HeaderPanel(user.getUsername());
        add(header, BorderLayout.NORTH);

        JLabel title = new JLabel("Node and Edge Relaxation Bounds", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 28));
        title.setForeground(Color.BLACK);
        title.setBackground(Color.WHITE);

        edgeSlider = new RangeSlider();
        edgeSlider.setMinimum(0);
        edgeSlider.setMaximum(100);
        edgeSlider.setValue(arr[2]);
        edgeSlider.setUpperValue(arr[3]);
        edgeSlider.setOpaque(false); // Let background show through
        edgeSlider.setPaintTicks(true);
        edgeSlider.setPaintLabels(true);
        edgeSlider.setMajorTickSpacing(10);
        edgeSlider.setMinorTickSpacing(1);
        edgeSlider.setForeground(Color.BLACK);
        edgeSlider.setPreferredSize(new Dimension(300, edgeSlider.getPreferredSize().height));

        nodeSlider = new RangeSlider();
        nodeSlider.setMinimum(0);
        nodeSlider.setMaximum(100);
        nodeSlider.setValue(arr[0]);
        nodeSlider.setUpperValue(arr[1]);
        nodeSlider.setOpaque(false);
        nodeSlider.setPaintTicks(true);
        nodeSlider.setPaintLabels(true);
        nodeSlider.setMajorTickSpacing(10);
        nodeSlider.setMinorTickSpacing(1);
        nodeSlider.setForeground(Color.BLACK);
        nodeSlider.setPreferredSize(new Dimension(200, nodeSlider.getPreferredSize().height));

        lowerLabel1 = new JLabel("Lower Bound: " + nodeSlider.getValue());
        lowerLabel1.setForeground(Color.BLACK);
        upperLabel = new JLabel("Upper Bound: " + nodeSlider.getUpperValue());
        upperLabel.setForeground(Color.BLACK);

        lowerLabel2 = new JLabel("Lower Bound: " + edgeSlider.getValue());
        upperLabel2 = new JLabel("Upper Bound: " + edgeSlider.getUpperValue());
        lowerLabel2.setForeground(Color.BLACK);
        upperLabel2.setForeground(Color.BLACK);

        nodeSlider.addChangeListener((ChangeEvent e) -> {
            lowerLabel1.setText("Lower Bound: " + nodeSlider.getValue());
            upperLabel.setText("Upper Bound: " + nodeSlider.getUpperValue());
            arr[0] = nodeSlider.getValue();
            arr[1] = nodeSlider.getUpperValue();
        });

        edgeSlider.addChangeListener((ChangeEvent e) -> {
            lowerLabel2.setText("Lower Bound: " + edgeSlider.getValue());
            upperLabel2.setText("Upper Bound: " + edgeSlider.getUpperValue());
            arr[2] = edgeSlider.getValue();
            arr[3] = edgeSlider.getUpperValue();
        });

        JPanel nodeSliderPanel = new JPanel();
        nodeSliderPanel.setLayout(new BoxLayout(nodeSliderPanel, BoxLayout.Y_AXIS));
        nodeSliderPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
        nodeSliderPanel.setBorder(BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(Color.BLACK), 
            "Node Relaxation Bounds", 0, 0, null, Color.BLACK));
        nodeSliderPanel.setBackground(Color.WHITE);

        nodeSliderPanel.add(lowerLabel1);
        nodeSliderPanel.add(upperLabel);
        nodeSliderPanel.add(Box.createVerticalStrut(30));
        nodeSliderPanel.add(nodeSlider);

        JPanel edgeSliderPanel = new JPanel();
        edgeSliderPanel.setLayout(new BoxLayout(edgeSliderPanel, BoxLayout.Y_AXIS));
        edgeSliderPanel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20)); 
        edgeSliderPanel.setBorder(BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(Color.BLACK), 
            "Edge Relaxation Bounds", 0, 0, null, Color.BLACK));
        edgeSliderPanel.setBackground(Color.WHITE);

        edgeSliderPanel.add(lowerLabel2);
        edgeSliderPanel.add(upperLabel2);
        edgeSliderPanel.add(Box.createVerticalStrut(30));
        edgeSliderPanel.add(edgeSlider);

        JPanel sliderPanel = new JPanel(new GridLayout(2, 1));
        
        sliderPanel.add(nodeSliderPanel);
        sliderPanel.add(edgeSliderPanel);
        sliderPanel.setBackground(Color.WHITE);

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



        nextButton.addActionListener(e -> {
            user.setRelaxationBounds(arr);
            ThresholdFilterPanel threshold = new ThresholdFilterPanel(cardLayout, cardPanel, user);
            cardPanel.add(threshold, "threshold");
            cardLayout.show(cardPanel, "threshold");
        });

        
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "reachBound"));

        
        saveButton.addActionListener(e -> {
            user.setRelaxationBounds(arr);
            user.saveData();
        });

        
        goToSessions.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(prevButton, BorderLayout.EAST);
            add(goToSessions, BorderLayout.WEST);
            setOpaque(false);
        }});

        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setOpaque(false);
        }});
        buttonPanel.setBackground(Color.WHITE);

        add(new JPanel(new BorderLayout()){{
            add(title, BorderLayout.NORTH);
            add(sliderPanel, BorderLayout.CENTER);
            setBorder(BorderFactory.createEmptyBorder(10, 40, 10, 40));
            setOpaque(false);
        }}, BorderLayout.CENTER);
        add(buttonPanel, BorderLayout.SOUTH);
    }
}
