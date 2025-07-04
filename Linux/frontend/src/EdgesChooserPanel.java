import java.awt.*;
import java.io.*;
import javax.swing.*;
import javax.swing.BorderFactory.*;
import org.json.JSONArray;
import org.json.JSONObject;
import java.nio.file.Files;
import java.nio.file.Path;

public class EdgesChooserPanel extends JPanel {

    private JLabel[] fileLabels = new JLabel[5];
    private String[] edges;


    public EdgesChooserPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout());
        setBackground(new Color(138, 43, 226));
        setBorder(BorderFactory.createEmptyBorder(20,20,10,20));

        edges= user.getEdgesFile();

        JLabel title = new JLabel("Add Edge files", JLabel.CENTER);
        title.setFont(new Font("Arial", Font.BOLD, 30));
        title.setForeground(Color.WHITE);
        title.setBackground(new Color(138, 43, 226));
        add(title, BorderLayout.NORTH);

        JPanel panel = new JPanel();
        panel.setLayout(new GridLayout(3, 2, 10, 10)); 
        panel.setBorder(BorderFactory.createEmptyBorder(20,20,10,20));
        panel.add(createFileRow("Additional Edges File", 0));
        panel.add(createFileRow("Essential Edge File", 1));
        panel.add(createFileRow("Avoid Edge File",2));
        panel.add(createFileRow("Relaxed Edges File", 3));
        panel.add(createFileRow("Non Relaxed Edges File", 4));

        panel.setOpaque(false);

        add(panel, BorderLayout.CENTER);

        JButton saveButton= new JButton("Save");
        JButton prevButton= new JButton("Prev");
        JButton nextButton= new JButton("Next");

        JPanel buttonPanel= new JPanel(new BorderLayout());
        buttonPanel.setOpaque(false);

        buttonPanel.add(new JPanel(new FlowLayout(FlowLayout.RIGHT)){{
            add(saveButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);

        buttonPanel.add(new JPanel(){{
            add(prevButton);
            add(nextButton);
            setOpaque(false);
        }}, BorderLayout.SOUTH);

        add(buttonPanel, BorderLayout.SOUTH);

        saveButton.addActionListener(e->{
            user.setEdgesFile(edges);
            String fileName = "sessions/" + user.getUsername() + ".json";
            
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                JSONObject sessionData = sessionsArray.getJSONObject(user.getIndex());

                sessionData.put("AdditionalEdge", edges[0]);
                sessionData.put("EssentialEdge", edges[1]);
                sessionData.put("AvoidEdge", edges[2]);
                sessionData.put("RelaxedEdge", edges[3]);
                sessionData.put("NonelaxedEdge", edges[3]);
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

        prevButton.addActionListener(e->{
            cardLayout.show(cardPanel, "threshold");
        });

        nextButton.addActionListener(e->{
            user.setEdgesFile(edges);
            IdEntryPanel idEntry = new IdEntryPanel(cardLayout, cardPanel, user);
            cardPanel.add(idEntry, "idEntry");
            cardLayout.show(cardPanel, "idEntry");
        });
    }

    private JPanel createFileRow(String title, int index) {
        JPanel rowPanel = new JPanel();
        rowPanel.setBackground(new Color(138, 43, 226)); // optional styling

        JLabel titleLabel = new JLabel(title);
        titleLabel.setForeground(Color.WHITE); // styling
        titleLabel.setFont(new Font("Verdana", Font.BOLD, 20));
        rowPanel.add(titleLabel);

        fileLabels[index] = new JLabel("No file selected");
        fileLabels[index].setForeground(Color.WHITE);

        JButton chooseButton = new JButton("Choose File");
        chooseButton.addActionListener(e -> {
            JFileChooser chooser = new JFileChooser();
            int result = chooser.showOpenDialog(this);
            if (result == JFileChooser.APPROVE_OPTION) {
                File selectedFile = chooser.getSelectedFile();
                fileLabels[index].setText(selectedFile.getName());
                edges[index]= selectedFile.getAbsolutePath();
                // Optionally store the file reference
            }
        });

        JPanel rightPanel = new JPanel(new BorderLayout(5, 5));
        rightPanel.setOpaque(false);
        rightPanel.add(new JPanel(){{
            add(chooseButton);
            setOpaque(false);
        }}, BorderLayout.CENTER);
        rightPanel.add(fileLabels[index], BorderLayout.SOUTH);

        rowPanel.add(rightPanel);
        return rowPanel;
    }
}

