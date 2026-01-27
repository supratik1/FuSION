import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.awt.geom.QuadCurve2D;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;
import java.util.List;
import java.util.Queue;
import java.io.*;
import java.text.SimpleDateFormat;

import javax.swing.*;
import javax.swing.event.*;


public class gui4 extends JFrame {
    private static final long serialVersionUID = 1L;
    
    private final DrawingPanel drawingPanel = new DrawingPanel();
    private final List<FunctionBlock> functionBlocks = new ArrayList<>();
    private int functionCounter = 1;    
    FunctionBlock dragSource = null;
    int dragSourceOutputIndex = -1;
    List<Connection> connections = new ArrayList<>();
    private JTextArea resultArea;
    private JTabbedPane tabbedPane;
    private Connection selectedConnection = null;
    private final Map<String, BlockTemplate> blockLibrary = new HashMap<>();
    private final Map<String, Integer> instanceCounter = new HashMap<>();
    private JComboBox<String> blockSelector;
    private JPanel blockListPanel;
    public int inputCount = 0;
    List<FunctionBlock> blockInstances = new ArrayList<>();
    private double prevZoomFactor = 1.0;
    private double zoomFactor = 1.0;
    private FunctionBlock clipboardBlock = null; // For copy/paste functionality

    private JButton hamburgerButton;

    // History tab components
    private JTextArea historyArea;
    private List<String> executionHistory = new ArrayList<>();
    private static final String HISTORY_FILE = "execution_history.txt";

    // Block descriptions map - Full forms and descriptions
    private static final Map<String, String[]> BLOCK_DESCRIPTIONS = new HashMap<>();
    
    static {
        // Format: {Full Name, Description, Category}
         BLOCK_DESCRIPTIONS.put("let", new String[]{
            "Nope",
            "Nope"
        });
        
        BLOCK_DESCRIPTIONS.put("cudf", new String[]{
            "Create Up Down File",
            "Processes package dependency files and generates upgrade plans. Used for managing software package dependencies and conflicts.",
            "Package Management"
        });

         BLOCK_DESCRIPTIONS.put("exec", new String[]{
            "Execute",
            "Nope"
        });

        BLOCK_DESCRIPTIONS.put("start", new String[]{
            "Start/Initialize Process",
            "Entry point block that initializes the workflow. Takes multiple file inputs and outputs a Status indicating success or failure.",
            "Control Flow"
        });

        BLOCK_DESCRIPTIONS.put("mff", new String[]{
            "Merge From File",
            "Computes maximum flow in a network graph. Takes a float parameter and outputs a graph structure.",
            "Graph Algorithms"
        });

        BLOCK_DESCRIPTIONS.put("wgx", new String[]{
            "write Graph Xml",
            "Processes weighted graph data and performs graph transformations. Combines two string inputs into a processed output.",
            "Graph Processing"
        });

        BLOCK_DESCRIPTIONS.put("rgx", new String[]{
            "Read Graph Xml",
            "Nope"
        });
        
        BLOCK_DESCRIPTIONS.put("fb_rch", new String[]{
            "Final Bound Reach",
            "Nope"
        });

        BLOCK_DESCRIPTIONS.put("result", new String[]{
            "Result",
            "Aggregates multiple inputs (float, string, file, graph) and produces consolidated outputs. Final processing block for workflows.",
            "Output/Aggregation"
        });
    }

    private static String getDefaultValue(String type) {
        switch (type) {
            case "float": return "0.0";
            case "integer": return "0";
            case "int": return "0";
            case "string": return "default_string";
            case "file": return "";
            case "graph": return "default_graph";
            case "Status": return "default_status";
            case "character": return "a";
            case "char": return "a";
            default: return "default";
        }
    }

    // Get block description
    public static String[] getBlockDescription(String blockName) {
        // Extract base name (remove _1, _2 suffix)
        String baseName = blockName.replaceAll("_\\d+$", "");
        return BLOCK_DESCRIPTIONS.getOrDefault(baseName, new String[]{
            blockName.toUpperCase(),
            "Custom block - No description available",
            "Custom"
        });
    }

    public gui4() {
        loadExecutionHistory();
        initializeGUI();
    }
    
    // Create Legend Panel for type colors - positioned at top right
    private JPanel createLegendPanel() {
        JPanel legendPanel = new JPanel();
        legendPanel.setLayout(new FlowLayout(FlowLayout.RIGHT, 10, 5));
        legendPanel.setBackground(new Color(245, 245, 250, 230));
        legendPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(100, 100, 150), 1),
            BorderFactory.createEmptyBorder(5, 10, 5, 10)
        ));

        // Add "Legend:" label
        JLabel legendLabel = new JLabel("Legend: ");
        legendLabel.setFont(new Font("SansSerif", Font.BOLD, 12));
        legendLabel.setForeground(new Color(50, 50, 100));
        legendPanel.add(legendLabel);

        // Type colors mapping
        Object[][] typeColors = {
            {"float", new Color(139, 0, 0)},
            {"integer", new Color(0, 0, 139)},
            {"string", new Color(255, 140, 0)},
            {"file", new Color(199, 21, 133)},
            {"graph", new Color(0, 139, 139)},
            {"Status", new Color(0, 100, 0)},
            {"char", new Color(184, 134, 11)}
        };

        for (Object[] typeColor : typeColors) {
            String typeName = (String) typeColor[0];
            Color color = (Color) typeColor[1];
            
            // Create colored box
            JPanel colorBox = new JPanel() {
                @Override
                protected void paintComponent(Graphics g) {
                    super.paintComponent(g);
                    Graphics2D g2 = (Graphics2D) g;
                    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    g2.setColor(color);
                    g2.fillRoundRect(0, 0, getWidth(), getHeight(), 4, 4);
                    g2.setColor(Color.DARK_GRAY);
                    g2.drawRoundRect(0, 0, getWidth() - 1, getHeight() - 1, 4, 4);
                }
            };
            colorBox.setPreferredSize(new Dimension(14, 14));
            colorBox.setOpaque(false);
            
            // Label
            JLabel label = new JLabel(typeName);
            label.setFont(new Font("SansSerif", Font.BOLD, 11));
            label.setForeground(color.darker());
            
            // Add to legend
            legendPanel.add(colorBox);
            legendPanel.add(label);
            legendPanel.add(Box.createHorizontalStrut(5));
        }

        return legendPanel;
    }
    
    private void initializeGUI() {
        setTitle("Visual Function Graph Editor");
        setSize(1400, 900);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());

        // === Top Panel (controls) ===
        JPanel topPanel = new JPanel();
        topPanel.setLayout((LayoutManager) new BoxLayout(topPanel, BoxLayout.Y_AXIS));

        JPanel controlsPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));

        // === Hamburger Icon Button ===
        hamburgerButton = new JButton("\u2630");
        hamburgerButton.setFont(new Font("SansSerif", Font.BOLD, 18));
        hamburgerButton.setToolTipText("Menu Options");
        hamburgerButton.setMargin(new Insets(2, 8, 2, 8));
        hamburgerButton.addActionListener(e -> {
            JPopupMenu menu = new JPopupMenu();
            
            JMenuItem editInputs = new JMenuItem("Edit Block Inputs");
            editInputs.addActionListener(ev -> showBlockNamesDialog());
            menu.add(editInputs);
            
            JMenuItem viewNamingHistory = new JMenuItem("View Naming History");
            viewNamingHistory.addActionListener(ev -> showNamingHistoryDialog());
            menu.add(viewNamingHistory);
            
            menu.addSeparator();
            
            JMenuItem viewBlockInfo = new JMenuItem("View All Block Descriptions");
            viewBlockInfo.addActionListener(ev -> showAllBlockDescriptions());
            menu.add(viewBlockInfo);
            
            menu.show(hamburgerButton, 0, hamburgerButton.getHeight());
        });
        controlsPanel.add(hamburgerButton);

        // === Block Selector Dropdown ===
        blockSelector = new JComboBox<String>();
        blockSelector.setRenderer(new BlockSelectorRenderer());
        controlsPanel.add(new JLabel("Select Block:"));
        controlsPanel.add(blockSelector);

        // === Add Block Button ===
        JButton addBlockBtn = new JButton("Add Block");
        addBlockBtn.addActionListener(e -> addSelectedBlockInstance());
        controlsPanel.add(addBlockBtn);

        // === Add New Block Template Button ===
        JButton addTemplateBtn = new JButton("+ New Template");
        addTemplateBtn.setBackground(new Color(138, 43, 226));
        addTemplateBtn.addActionListener(e -> showNewBlockTemplateDialog());
        controlsPanel.add(addTemplateBtn);

        // === Search Field and Button ===
        JTextField searchField = new JTextField(15);
        JButton searchButton = new JButton("Search");
        searchButton.addActionListener(e -> searchFunctionBlock(searchField.getText().toLowerCase()));
        searchField.addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    searchFunctionBlock(searchField.getText().toLowerCase());
                }
            }
        });
        controlsPanel.add(new JLabel("Search:"));
        controlsPanel.add(searchField);
        controlsPanel.add(searchButton);

        // === Zoom In/Out Buttons ===
        JButton zoomInButton = new JButton("🔍+");
        zoomInButton.addActionListener(e -> drawingPanel.zoom(1.2));
        JButton zoomOutButton = new JButton("🔍−");
        zoomOutButton.addActionListener(e -> drawingPanel.zoom(0.8));
        controlsPanel.add(zoomInButton);
        controlsPanel.add(zoomOutButton);

        // === Execute Button ===
        JButton executeBtn = new JButton("▶ Execute");
        executeBtn.setBackground(new Color(34, 139, 34));
        executeBtn.addActionListener(e -> executeGraph());
        controlsPanel.add(executeBtn);

        topPanel.add(controlsPanel);
        
        blockListPanel = new JPanel();
        blockListPanel.setLayout(new BoxLayout(blockListPanel, BoxLayout.X_AXIS));
        JScrollPane blockScrollPane = new JScrollPane(blockListPanel);
        blockScrollPane.setPreferredSize(new Dimension(250, 80));
        blockScrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        blockScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_NEVER);
        topPanel.add(blockScrollPane);

        // Initialize block library
        populateBlockLibrary();

        add(topPanel, BorderLayout.NORTH);

        // === Main Content with Tabs ===
        tabbedPane = new JTabbedPane();
        
        // Drawing Canvas Tab with Legend at top right
        JPanel editorPanel = new JPanel(new BorderLayout());
        
        // Create top panel for legend (aligned right)
        JPanel legendContainer = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, 0));
        legendContainer.setBackground(new Color(248, 248, 252));
        legendContainer.add(createLegendPanel());
        editorPanel.add(legendContainer, BorderLayout.NORTH);
        
        // Canvas scroll pane
        JScrollPane canvasScroll = new JScrollPane(drawingPanel);
        canvasScroll.getVerticalScrollBar().setUnitIncrement(16);
        editorPanel.add(canvasScroll, BorderLayout.CENTER);
        
        tabbedPane.addTab("Graph Editor", editorPanel);
        
        // Results Tab
        resultArea = new JTextArea();
        resultArea.setEditable(false);
        resultArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        JScrollPane resultScroll = new JScrollPane(resultArea);
        tabbedPane.addTab("Results", resultScroll);

        // History Tab
        JPanel historyPanel = createHistoryPanel();
        tabbedPane.addTab("History", historyPanel);
        
        add(tabbedPane, BorderLayout.CENTER);

        // === Keyboard shortcuts ===
        setupKeyboardShortcuts();

        setVisible(true);
    }

    // Create History Panel
    private JPanel createHistoryPanel() {
        JPanel panel = new JPanel(new BorderLayout(10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        panel.setBackground(new Color(248, 249, 250));

        // Header panel
        JPanel headerPanel = new JPanel(new BorderLayout());
        headerPanel.setOpaque(false);
        
        JLabel titleLabel = new JLabel("Execution History");
        titleLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        titleLabel.setForeground(new Color(60, 60, 100));
        headerPanel.add(titleLabel, BorderLayout.WEST);

        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        buttonPanel.setOpaque(false);
        
        JButton refreshBtn = new JButton("🔄 Refresh");
        refreshBtn.addActionListener(e -> refreshHistoryDisplay());
        buttonPanel.add(refreshBtn);
        
        JButton clearBtn = new JButton("🗑️ Clear All");
        clearBtn.setBackground(new Color(220, 53, 69));
        clearBtn.addActionListener(e -> clearExecutionHistory());
        buttonPanel.add(clearBtn);
        
        headerPanel.add(buttonPanel, BorderLayout.EAST);
        panel.add(headerPanel, BorderLayout.NORTH);

        // History text area
        historyArea = new JTextArea();
        historyArea.setEditable(false);
        historyArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        historyArea.setBackground(Color.WHITE);
        JScrollPane historyScroll = new JScrollPane(historyArea);
        historyScroll.setBorder(BorderFactory.createLineBorder(new Color(200, 200, 220), 1));
        panel.add(historyScroll, BorderLayout.CENTER);

        // Refresh display
        refreshHistoryDisplay();

        return panel;
    }

    // Load execution history from file
    private void loadExecutionHistory() {
        File file = new File(HISTORY_FILE);
        if (file.exists()) {
            try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                executionHistory.clear();
                StringBuilder currentEntry = new StringBuilder();
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.equals("===END_ENTRY===")) {
                        if (currentEntry.length() > 0) {
                            executionHistory.add(currentEntry.toString());
                            currentEntry = new StringBuilder();
                        }
                    } else {
                        if (currentEntry.length() > 0) {
                            currentEntry.append("\n");
                        }
                        currentEntry.append(line);
                    }
                }
                if (currentEntry.length() > 0) {
                    executionHistory.add(currentEntry.toString());
                }
            } catch (Exception e) {
                System.err.println("Error loading history: " + e.getMessage());
                executionHistory = new ArrayList<>();
            }
        }
    }

    // Save execution history to file
    private void saveExecutionHistory() {
        try (PrintWriter writer = new PrintWriter(new FileWriter(HISTORY_FILE))) {
            for (String entry : executionHistory) {
                writer.println(entry);
                writer.println("===END_ENTRY===");
            }
        } catch (Exception e) {
            System.err.println("Error saving history: " + e.getMessage());
        }
    }

    // Add to execution history
    private void addToExecutionHistory(String content) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        String timestamp = sdf.format(new Date());
        
        StringBuilder entry = new StringBuilder();
        entry.append("========================================\n");
        entry.append("EXECUTION: ").append(timestamp).append("\n");
        entry.append("Blocks: ").append(functionBlocks.size());
        entry.append(" | Connections: ").append(connections.size()).append("\n");
        entry.append("========================================\n\n");
        entry.append(content);
        
        executionHistory.add(entry.toString());
        
        // Keep only last 50 entries
        while (executionHistory.size() > 50) {
            executionHistory.remove(0);
        }
        
        saveExecutionHistory();
        refreshHistoryDisplay();
    }

    // Refresh history display
    private void refreshHistoryDisplay() {
        if (historyArea == null) return;
        
        if (executionHistory.isEmpty()) {
            historyArea.setText("No execution history yet.\n\nRun an execution to see history here.");
        } else {
            StringBuilder display = new StringBuilder();
            display.append("=== EXECUTION HISTORY ===\n");
            display.append("Total entries: ").append(executionHistory.size()).append("\n\n");
            
            // Show in reverse order (newest first)
            for (int i = executionHistory.size() - 1; i >= 0; i--) {
                display.append("[Entry #").append(i + 1).append("]\n");
                display.append(executionHistory.get(i));
                display.append("\n\n");
            }
            
            historyArea.setText(display.toString());
            historyArea.setCaretPosition(0);
        }
    }

    // Clear execution history
    private void clearExecutionHistory() {
        if (executionHistory.isEmpty()) {
            JOptionPane.showMessageDialog(this, "History is already empty.", "Info", JOptionPane.INFORMATION_MESSAGE);
            return;
        }
        
        int confirm = JOptionPane.showConfirmDialog(this,
            "Are you sure you want to clear all execution history?\nThis action cannot be undone.",
            "Confirm Clear All",
            JOptionPane.YES_NO_OPTION,
            JOptionPane.WARNING_MESSAGE);
        
        if (confirm == JOptionPane.YES_OPTION) {
            executionHistory.clear();
            saveExecutionHistory();
            refreshHistoryDisplay();
            JOptionPane.showMessageDialog(this, "Execution history cleared.", "Success", JOptionPane.INFORMATION_MESSAGE);
        }
    }

    // Custom renderer for block selector dropdown
    class BlockSelectorRenderer extends DefaultListCellRenderer {
        @Override
        public Component getListCellRendererComponent(JList<?> list, Object value, int index, boolean isSelected, boolean cellHasFocus) {
            super.getListCellRendererComponent(list, value, index, isSelected, cellHasFocus);
            
            if (value != null) {
                String blockName = value.toString();
                if (!blockName.equals("Add New Block Template...")) {
                    String[] desc = getBlockDescription(blockName);
                    setText(blockName + " - " + desc[0]);
                    setToolTipText("<html><b>" + desc[0] + "</b><br>" + desc[1] + "</html>");
                }
            }
            return this;
        }
    }

    // Setup keyboard shortcuts
    private void setupKeyboardShortcuts() {
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(e -> {
            if (e.getID() == KeyEvent.KEY_PRESSED) {
                if (e.isControlDown()) {
                    switch (e.getKeyCode()) {
                        case KeyEvent.VK_C:
                            copySelectedBlock();
                            return true;
                        case KeyEvent.VK_V:
                            pasteBlock();
                            return true;
                        case KeyEvent.VK_D:
                            DelSelectedBlock();
                            return true;
                    }
                }
            }
            return false;
        });
    }

    // Copy selected block
    private void copySelectedBlock() {
        for (FunctionBlock fb : functionBlocks) {
            if (fb.isSelected) {
                clipboardBlock = fb;
                return;
            }
        }
        JOptionPane.showMessageDialog(this, 
            "No block selected. Click on a block first, then press Ctrl+C.", 
            "No Selection", 
            JOptionPane.WARNING_MESSAGE);
    }

    // Paste block from clipboard
    private void pasteBlock() {
        if (clipboardBlock == null) {
            JOptionPane.showMessageDialog(this, 
                "Nothing to paste. Copy a block first (Ctrl+C).", 
                "Clipboard Empty", 
                JOptionPane.WARNING_MESSAGE);
            return;
        }

        BlockTemplate template = clipboardBlock.template;
        int count = instanceCounter.getOrDefault(template.name, 0) + 1;
        instanceCounter.put(template.name, count);

        String instanceName = template.name + "_" + count;
        FunctionBlock newBlock = new FunctionBlock(instanceName, template);
        
        // Offset position from original
        int x = clipboardBlock.getX() + 50;
        int y = clipboardBlock.getY() + 50;
        
        int scaledWidth = (int)(newBlock.getPreferredSize().width * zoomFactor);
        int scaledHeight = (int)(newBlock.getPreferredSize().height * zoomFactor);
        newBlock.setBounds(x, y, scaledWidth, scaledHeight);

        // Copy input values
        for (int i = 0; i < template.inputCount && i < clipboardBlock.inputValues.length; i++) {
            newBlock.inputValues[i] = clipboardBlock.inputValues[i];
        }

        functionCounter++;
        functionBlocks.add(newBlock);
        drawingPanel.add(newBlock);
        updateCanvasSize();
        drawingPanel.repaint();
    }

    // Delete selected block
    private void DelSelectedBlock() {
        for (FunctionBlock fb : functionBlocks) {
            if (fb.isSelected) {
                functionBlocks.remove(fb);
                drawingPanel.remove(fb);
                drawingPanel.repaint();
                return;
            }
        }
    }

    // Clear all blocks
    private void clearAllBlocks() {
        int confirm = JOptionPane.showConfirmDialog(this, 
            "Are you sure you want to clear all blocks and connections?", 
            "Confirm Clear", 
            JOptionPane.YES_NO_OPTION);
        
        if (confirm == JOptionPane.YES_OPTION) {
            for (FunctionBlock fb : functionBlocks) {
                drawingPanel.remove(fb);
            }
            functionBlocks.clear();
            connections.clear();
            functionCounter = 1;
            for (String key : instanceCounter.keySet()) {
                instanceCounter.put(key, 0);
            }
            updateCanvasSize();
            drawingPanel.repaint();
        }
    }

    // Show all block descriptions dialog
    private void showAllBlockDescriptions() {
        JPanel panel = new JPanel();
        panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
        
        for (Map.Entry<String, String[]> entry : BLOCK_DESCRIPTIONS.entrySet()) {
            String[] desc = entry.getValue();
            
            JPanel blockPanel = new JPanel(new BorderLayout());
            blockPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(new Color(100, 100, 200), 2),
                BorderFactory.createEmptyBorder(10, 10, 10, 10)
            ));
            blockPanel.setBackground(new Color(240, 240, 255));
            
            JLabel titleLabel = new JLabel("<html><b style='font-size:14px;color:#4040A0;'>" + 
                entry.getKey().toUpperCase() + "</b> - " + desc[0] + "</html>");
            
            JTextArea descArea = new JTextArea(desc[1]);
            descArea.setLineWrap(true);
            descArea.setWrapStyleWord(true);
            descArea.setEditable(false);
            descArea.setBackground(new Color(240, 240, 255));
            descArea.setBorder(BorderFactory.createEmptyBorder(5, 0, 0, 0));
            
            JPanel textPanel = new JPanel(new BorderLayout());
            textPanel.setBackground(new Color(240, 240, 255));
            textPanel.add(titleLabel, BorderLayout.NORTH);
            textPanel.add(descArea, BorderLayout.SOUTH);
            
            blockPanel.add(textPanel, BorderLayout.CENTER);
            panel.add(blockPanel);
            panel.add(Box.createVerticalStrut(10));
        }
        
        JScrollPane scrollPane = new JScrollPane(panel);
        scrollPane.setPreferredSize(new Dimension(500, 400));
        
        JOptionPane.showMessageDialog(this, scrollPane, "Block Descriptions & Full Forms", JOptionPane.INFORMATION_MESSAGE);
    }

    // Placeholder JTextField class
    class PlaceholderTextField extends JTextField {
        private String placeholder = "";

        public PlaceholderTextField(String placeholder) {
            this.placeholder = placeholder;
        }

        public void setPlaceholder(String placeholder) {
            this.placeholder = placeholder;
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            if (getText().isEmpty() && !placeholder.isEmpty()) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setColor(Color.GRAY);
                FontMetrics fm = g2.getFontMetrics();
                int x = getInsets().left;
                int y = (getHeight() - fm.getHeight()) / 2 + fm.getAscent();
                g2.drawString(placeholder, x, y);
                g2.dispose();
            }
        }
    }

    // Custom cell editor for Value column with type placeholder
    class TypeAwareCellEditor extends DefaultCellEditor {
        private String currentType;

        public TypeAwareCellEditor() {
            super(new PlaceholderTextField(""));
        }

        @Override
        public Component getTableCellEditorComponent(JTable table, Object value, boolean isSelected, int row, int column) {
            PlaceholderTextField field = (PlaceholderTextField) super.getTableCellEditorComponent(table, value, isSelected, row, column);
            currentType = (String) table.getValueAt(row, 2);
            field.setPlaceholder(currentType);
            return field;
        }

        @Override
        public boolean stopCellEditing() {
            return super.stopCellEditing();
        }
    }

    // Show dialog with editable input values for all blocks
    private void showBlockNamesDialog() {
        int totalInputs = 0;
        for (FunctionBlock fb : functionBlocks) {
            totalInputs += fb.template.inputCount;
        }

        String[] columnNames = {"Block Name", "Input", "Type", "Value"};
        Object[][] data = new Object[totalInputs][4];

        int row = 0;
        for (FunctionBlock fb : functionBlocks) {
            for (int i = 0; i < fb.template.inputCount; i++) {
                data[row][0] = fb.name;
                data[row][1] = "Input " + (i + 1);
                data[row][2] = fb.template.inputTypes[i];
                boolean connected = false;
                String value = fb.inputValues[i];
                for (Connection c : connections) {
                    if (c.to == fb && c.toIdx == i) {
                        connected = true;
                        value = "$" + c.from.name + ".output" + (c.fromIdx + 1);
                        break;
                    }
                }
                data[row][3] = value;
                row++;
            }
        }

        Object[][] oldData = new Object[totalInputs][4];
        for(int i=0; i<data.length; i++) oldData[i] = data[i].clone();

        class InputTableModel extends javax.swing.table.DefaultTableModel {
            public InputTableModel(Object[][] data, Object[] columnNames) {
                super(data, columnNames);
            }

            @Override
            public boolean isCellEditable(int row, int column) {
                if (column == 3) {
                    int currentRow = 0;
                    for (FunctionBlock fb : functionBlocks) {
                        for (int i = 0; i < fb.template.inputCount; i++) {
                            if (currentRow == row) {
                                boolean connected = false;
                                for (Connection c : connections) {
                                    if (c.to == fb && c.toIdx == i) {
                                        connected = true;
                                        break;
                                    }
                                }
                                return !connected;
                            }
                            currentRow++;
                        }
                    }
                }
                return false;
            }
        }

        JTable table = new JTable(new InputTableModel(data, columnNames));
        table.setPreferredScrollableViewportSize(new Dimension(600, 400));
        table.setFillsViewportHeight(true);
        table.getColumnModel().getColumn(3).setCellEditor(new TypeAwareCellEditor());

        JScrollPane scrollPane = new JScrollPane(table);

        JDialog dialog = new JDialog(this, "Edit Block Input Values", true);

        table.getModel().addTableModelListener(e -> {
            if (e.getType() == TableModelEvent.UPDATE && e.getColumn() == 3) {
                int r = e.getFirstRow();
                String value = (String) table.getValueAt(r, 3);
                String type = (String) table.getValueAt(r, 2);
                if (!isValidValue(value, type)) {
                    JOptionPane.showMessageDialog(dialog, "Invalid value for type " + type + ": '" + value + "'", "Validation Error", JOptionPane.ERROR_MESSAGE);
                    table.setValueAt(oldData[r][3], r, 3);
                } else {
                    oldData[r][3] = value;
                }
            }
        });
        dialog.setLayout(new BorderLayout());
        dialog.add(scrollPane, BorderLayout.CENTER);

        JPanel buttonPanel = new JPanel(new FlowLayout());
        JButton okButton = new JButton("OK");
        JButton cancelButton = new JButton("Cancel");
        buttonPanel.add(okButton);
        buttonPanel.add(cancelButton);
        dialog.add(buttonPanel, BorderLayout.SOUTH);

        final int[] result = {JOptionPane.CANCEL_OPTION};

        okButton.addActionListener(e -> {
            boolean allValid = true;
            StringBuilder errorMsg = new StringBuilder("Invalid values:\n");
            int r = 0;
            for (FunctionBlock fb : functionBlocks) {
                for (int i = 0; i < fb.template.inputCount; i++) {
                    boolean connected = false;
                    for (Connection c : connections) {
                        if (c.to == fb && c.toIdx == i) {
                            connected = true;
                            break;
                        }
                    }
                    if (!connected) {
                        String value = (String) table.getValueAt(r, 3);
                        String type = fb.template.inputTypes[i];
                        if (!isValidValue(value, type)) {
                            allValid = false;
                            errorMsg.append("- Block '").append(fb.name).append("', Input ").append((i + 1))
                                    .append(" (").append(type).append("): '").append(value).append("'\n");
                        }
                    }
                    r++;
                }
            }
            if (!allValid) {
                JOptionPane.showMessageDialog(dialog, errorMsg.toString(), "Validation Error", JOptionPane.ERROR_MESSAGE);
            } else {
                result[0] = JOptionPane.OK_OPTION;
                dialog.dispose();
            }
        });

        cancelButton.addActionListener(e -> {
            result[0] = JOptionPane.CANCEL_OPTION;
            dialog.dispose();
        });

        dialog.pack();
        dialog.setLocationRelativeTo(this);
        dialog.setVisible(true);

        if (result[0] == JOptionPane.OK_OPTION) {
            row = 0;
            for (FunctionBlock fb : functionBlocks) {
                for (int i = 0; i < fb.template.inputCount; i++) {
                    boolean connected = false;
                    for (Connection c : connections) {
                        if (c.to == fb && c.toIdx == i) {
                            connected = true;
                            break;
                        }
                    }
                    if (!connected) {
                        fb.inputValues[i] = (String) table.getValueAt(row, 3);
                    }
                    row++;
                }
            }
        }
    }

    private boolean isValidValue(String value, String type) {
        switch (type) {
            case "float":
                try {
                    Double.parseDouble(value);
                    return true;
                } catch (Exception e) {
                    return false;
                }
            case "integer":
            case "int":
                try {
                    Integer.parseInt(value);
                    return true;
                } catch (Exception e) {
                    return false;
                }
            case "char":
                return value != null && value.length() <= 1;
            case "string":
                return value != null;
            case "file":
                return value == null || value.trim().isEmpty() || (value.contains(".") || value.contains("/") || value.contains("\\"));
            case "graph":
            case "Status":
            case "character":
            default:
                return true;
        }
    }

    // Show naming history dialog
    private void showNamingHistoryDialog() {
        StringBuilder historyText = new StringBuilder();
        for (FunctionBlock fb : functionBlocks) {
            historyText.append("Block: ").append(fb.name).append("\n");
            historyText.append("Original: ").append(fb.originalName).append("\n");
            historyText.append("History: ").append(String.join(" -> ", fb.nameHistory)).append("\n\n");
        }

        JTextArea textArea = new JTextArea(historyText.toString());
        textArea.setEditable(false);
        textArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        JScrollPane scrollPane = new JScrollPane(textArea);
        scrollPane.setPreferredSize(new Dimension(400, 300));

        JOptionPane.showMessageDialog(this, scrollPane, "Naming History", JOptionPane.INFORMATION_MESSAGE);
    }

    public void searchFunctionBlock(String query) {
        drawingPanel.searchResults.clear();
        boolean found = false;
        for (FunctionBlock block : functionBlocks) {
            if (block.name != null && block.name.toLowerCase().contains(query)) {
                drawingPanel.searchResults.add(block);
                if (!found) {
                    highlightAndScrollToBlock(block);
                    found = true;
                }
            }
        }
        if (!found && !query.trim().isEmpty()) {
            JOptionPane.showMessageDialog(this, "No blocks found matching: " + query);
        }
        drawingPanel.repaint();
    }

    private void highlightAndScrollToBlock(FunctionBlock block) {
        Rectangle bounds = block.getBounds();

        int x = (int) (bounds.x * drawingPanel.zoomFactor + drawingPanel.translateX);
        int y = (int) (bounds.y * drawingPanel.zoomFactor + drawingPanel.translateY);
        int w = (int) (bounds.width * drawingPanel.zoomFactor);
        int h = (int) (bounds.height * drawingPanel.zoomFactor);
        Rectangle zoomed = new Rectangle(x, y, w, h);

        drawingPanel.scrollRectToVisible(zoomed);
        block.setBorder(BorderFactory.createLineBorder(Color.RED, 3));

        new javax.swing.Timer(3000, e -> {
            block.updateBorder();
            drawingPanel.repaint();
        }).start();
    }

    private void populateBlockLibrary() {
        addTemplate(new BlockTemplate("cudf", 3, 2, new String[]{"file", "integer","integer"}, new String[]{"file","file"}));
        addTemplate(new BlockTemplate("start", 4, 1, new String[]{"file", "file","file","file"}, new String[]{"Status"}));
        addTemplate(new BlockTemplate("wgx", 2, 1, new String[]{"string", "string"}, new String[]{"string"}));
        addTemplate(new BlockTemplate("mff", 1, 1, new String[]{"float"}, new String[]{"graph"}));
        addTemplate(new BlockTemplate("result", 4, 3, new String[]{"float","string","file","graph"}, new String[]{"graph","string","float"}));

        for (String blockName : blockLibrary.keySet()) {
            blockSelector.addItem(blockName);
        }
        blockSelector.addItem("Add New Block Template...");
    }

    private void updateCanvasSize() {
        int maxX = 0;
        int maxY = 0;

        for (FunctionBlock block : functionBlocks) {
            Rectangle bounds = block.getBounds();
            maxX = Math.max(maxX, bounds.x + bounds.width);
            maxY = Math.max(maxY, bounds.y + bounds.height);
        }

        maxX += 100;
        maxY += 100;

        drawingPanel.setPreferredSize(new Dimension(maxX, maxY));
        drawingPanel.revalidate();
    }

    private void addTemplate(BlockTemplate template) {
        blockLibrary.put(template.name, template);
        instanceCounter.put(template.name, 0);
        
        // Add description if not exists
        if (!BLOCK_DESCRIPTIONS.containsKey(template.name)) {
            BLOCK_DESCRIPTIONS.put(template.name, new String[]{
                template.name.toUpperCase() + " Block",
                "Custom block with " + template.inputCount + " inputs and " + template.outputCount + " outputs.",
                "Custom"
            });
        }

        JButton blockBtn = new JButton("<html><center><b>" + template.name + "</b><br><font size='2'>" + 
            getBlockDescription(template.name)[0] + "</font></center></html>");
        blockBtn.setAlignmentX(Component.LEFT_ALIGNMENT);
        blockBtn.setPreferredSize(new Dimension(150, 50));
        blockBtn.setMaximumSize(new Dimension(150, 50));
        blockBtn.setToolTipText("<html><b>" + getBlockDescription(template.name)[0] + "</b><br>" + 
            getBlockDescription(template.name)[1] + "</html>");
        blockBtn.addActionListener(e -> addBlockInstance(template.name));

        blockListPanel.add(blockBtn);
        blockListPanel.add(Box.createHorizontalStrut(5));
        blockListPanel.revalidate();
        blockListPanel.repaint();
    }

    private void addBlockInstance(String blockName) {
        if (!blockLibrary.containsKey(blockName)) return;

        BlockTemplate template = blockLibrary.get(blockName);
        int count = instanceCounter.getOrDefault(template.name, 0) + 1;
        instanceCounter.put(template.name, count);

        String instanceName = template.name + "_" + count;
        FunctionBlock block = new FunctionBlock(instanceName, template);
        int x = 100 + functionCounter * 60;
        int y = 100 + functionCounter * 40;
        
        int scaledWidth = (int)(block.getPreferredSize().width * zoomFactor);
        int scaledHeight = (int)(block.getPreferredSize().height * zoomFactor);
        block.setBounds(x, y, scaledWidth, scaledHeight);

        functionCounter++;
        functionBlocks.add(block);
        drawingPanel.add(block);
        updateCanvasSize();
        drawingPanel.repaint();
    }

    private void addSelectedBlockInstance() {
        String selected = (String) blockSelector.getSelectedItem();
        if (selected != null && selected.equals("Add New Block Template...")) {
            showNewBlockTemplateDialog();
            return;
        }
        addBlockInstance(selected);
    }

    private void showNewBlockTemplateDialog() {
        JTextField nameField = new JTextField(10);
        JTextField fullNameField = new JTextField(20);
        JTextArea descriptionArea = new JTextArea(3, 20);
        descriptionArea.setLineWrap(true);
        descriptionArea.setWrapStyleWord(true);
        JSpinner inputSpinner = new JSpinner(new SpinnerNumberModel(1, 0, 10, 1));
        JSpinner outputSpinner = new JSpinner(new SpinnerNumberModel(1, 1, 5, 1));

        JPanel panel = new JPanel(new GridLayout(0, 2, 5, 5));
        panel.add(new JLabel("Function Name (short):"));
        panel.add(nameField);
        panel.add(new JLabel("Full Name:"));
        panel.add(fullNameField);
        panel.add(new JLabel("Description:"));
        panel.add(new JScrollPane(descriptionArea));
        panel.add(new JLabel("# Inputs:"));
        panel.add(inputSpinner);
        panel.add(new JLabel("# Outputs:"));
        panel.add(outputSpinner);

        int res = JOptionPane.showConfirmDialog(this, panel, "New Block Template", JOptionPane.OK_CANCEL_OPTION);
        if (res != JOptionPane.OK_OPTION) return;

        int inCount = (int) inputSpinner.getValue();
        int outCount = (int) outputSpinner.getValue();

        String name = nameField.getText().trim();
        if (name.isEmpty() || blockLibrary.containsKey(name)) {
            JOptionPane.showMessageDialog(this, "Invalid or duplicate name");
            return;
        }

        // Add description
        String fullName = fullNameField.getText().trim().isEmpty() ? name.toUpperCase() + " Block" : fullNameField.getText().trim();
        String description = descriptionArea.getText().trim().isEmpty() ? "Custom block" : descriptionArea.getText().trim();

        BLOCK_DESCRIPTIONS.put(name, new String[]{fullName, description, "Custom"});

        String[] types = {"float", "string", "file", "graph","int", "Status", "char"};
        String[] inTypes = new String[inCount];
        String[] outTypes = new String[outCount];

        JPanel typePanel = new JPanel(new GridLayout(inCount + outCount, 2));
        @SuppressWarnings("unchecked")
        JComboBox<String>[] inComboBoxes = new JComboBox[inCount];
        @SuppressWarnings("unchecked")
        JComboBox<String>[] outComboBoxes = new JComboBox[outCount];
        
        for (int i = 0; i < inCount; i++) {
            typePanel.add(new JLabel("Input " + (i + 1) + ":"));
            JComboBox<String> cb = new JComboBox<>(types);
            inComboBoxes[i] = cb;
            typePanel.add(cb);
            inTypes[i] = types[0];
        }
        for (int i = 0; i < outCount; i++) {
            typePanel.add(new JLabel("Output " + (i + 1) + ":"));
            JComboBox<String> cb = new JComboBox<>(types);
            outComboBoxes[i] = cb;
            typePanel.add(cb);
            outTypes[i] = types[0];
        }

        int typeRes = JOptionPane.showConfirmDialog(this, typePanel, "Select Types", JOptionPane.OK_CANCEL_OPTION);
        if (typeRes == JOptionPane.OK_OPTION) {
            for (int i = 0; i < inCount; i++) {
                inTypes[i] = (String) inComboBoxes[i].getSelectedItem();
            }
            for (int i = 0; i < outCount; i++) {
                outTypes[i] = (String) outComboBoxes[i].getSelectedItem();
            }
            
            addTemplate(new BlockTemplate(name, inCount, outCount, inTypes, outTypes));
            blockSelector.removeAllItems();
            for (String blockName : blockLibrary.keySet()) {
                blockSelector.addItem(blockName);
            }
            blockSelector.addItem("Add New Block Template...");
        }
    }

    private void executeGraph() {
        List<FunctionBlock> order = getTopologicalOrder();
        if (order == null) return;

        String executionContent = saveExecutionPlan(order);
        
        // Add to history
        addToExecutionHistory(executionContent);
        
        StringBuilder resultText = new StringBuilder();
        resultText.append("=== EXECUTION PLAN ===\n\n");
        resultText.append("Execution plan saved to execution.txt\n");
        resultText.append("Execution added to History tab\n\n");
        resultText.append("=== TOPOLOGICAL ORDER ===\n");
        for (int i = 0; i < order.size(); i++) {
            FunctionBlock fb = order.get(i);
            String[] desc = getBlockDescription(fb.name);
            resultText.append((i + 1) + ". " + fb.originalName + " (" + desc[0] + ")\n");
        }
        resultText.append("\n=== CONNECTIONS ===\n");
        for (Connection c : connections) {
            if (c.toIdx == -1) {
                resultText.append("$" + c.from.originalName + ".output" + (c.fromIdx + 1) + " -> $" + c.to.originalName + " (status)\n");
            } else {
                resultText.append("$" + c.from.originalName + ".output" + (c.fromIdx + 1) + " -> $" + c.to.originalName + ".input" + (c.toIdx + 1) + "\n");
            }
        }
        
        resultArea.setText(resultText.toString());
        tabbedPane.setSelectedIndex(1); // Switch to Results tab
    }

    class BlockTemplate {
        String name;
        int inputCount;
        int outputCount;
        String[] inputTypes;
        String[] outputTypes;
        String[] defaultValues;

        BlockTemplate(String name, int inputCount, int outputCount, String[] inputTypes, String[] outputTypes) {
            this.name = name;
            this.inputCount = inputCount;
            this.outputCount = outputCount;
            this.inputTypes = inputTypes;
            this.outputTypes = outputTypes;
            this.defaultValues = new String[inputCount];
            for (int i = 0; i < inputCount; i++) {
                this.defaultValues[i] = getDefaultValue(inputTypes[i]);
            }
        }

        @Override
        public String toString() {
            return name + " (" + inputCount + " in, " + outputCount + " out)";
        }
    }

    class DrawingPanel extends JPanel {
        private static final long serialVersionUID = 1L;
        
        public double zoomFactor = 1.0;
        private transient Connection selectedConnection = null;
        private Point mousePoint = null;
        private boolean showMagnifier = false;
        private transient List<FunctionBlock> searchResults = new ArrayList<>();
        private transient FunctionBlock focusedBlock = null;
        public double translateX = 0;
        public double translateY = 0;

        DrawingPanel() {
            setPreferredSize(new Dimension(2000, 1200));
            setLayout(null);

            addMouseListener(new MouseAdapter() {
                public void mousePressed(MouseEvent e) {
                    selectedConnection = null;

                    Point rawClick = e.getPoint();
                    Point click = rawClick;
                    Container parent = getParent();
                    if (parent instanceof JViewport) {
                        JViewport viewport = (JViewport) parent;
                        Point viewPos = viewport.getViewPosition();
                        click = new Point((int)((rawClick.x + viewPos.x - translateX) / zoomFactor),
                                          (int)((rawClick.y + viewPos.y - translateY) / zoomFactor));
                    } else {
                        click = new Point((int)((rawClick.x - translateX) / zoomFactor),
                                          (int)((rawClick.y - translateY) / zoomFactor));
                    }

                    for (Connection conn : connections) {
                        Point from = getOutputPoint(conn.from, conn.fromIdx);
                        Point to = getInputPoint(conn.to, conn.toIdx);
                        if (isPointNearLine(click, from, to, 20)) {
                            selectedConnection = conn;
                            gui4.this.selectedConnection = conn;
                            break;
                        }
                    }

                    if (SwingUtilities.isRightMouseButton(e) && selectedConnection != null) {
                        JPopupMenu contextMenu = new JPopupMenu();
                        JMenuItem deleteItem = new JMenuItem("Delete Connection");
                        deleteItem.addActionListener(ev -> {
                            connections.remove(selectedConnection);
                            selectedConnection = null;
                            gui4.this.selectedConnection = null;
                            repaint();
                        });
                        contextMenu.add(deleteItem);
                        contextMenu.show(drawingPanel, e.getX(), e.getY());
                        e.consume();
                        return;
                    }

                    repaint();
                    requestFocusInWindow();
                }

                public void mouseExited(MouseEvent e) {
                    showMagnifier = false;
                    repaint();
                }
            });

            addMouseMotionListener(new MouseMotionAdapter() {
                public void mouseMoved(MouseEvent e) {
                    mousePoint = e.getPoint();
                    showMagnifier = (zoomFactor < 1.0);
                    repaint();
                }
            });
            
            addMouseWheelListener(e -> {
                if (e.isControlDown()) {
                    double factor = e.getWheelRotation() < 0 ? 1.1 : 0.9;
                    Point focus = e.getPoint();
                    zoom(factor, focus);
                    e.consume();
                }
            });

            setFocusable(true);
            addKeyListener(new KeyAdapter() {
                public void keyPressed(KeyEvent e) {
                    if (e.getKeyCode() == KeyEvent.VK_DELETE && selectedConnection != null) {
                        connections.remove(selectedConnection);
                        selectedConnection = null;
                        gui4.this.selectedConnection = null;
                        repaint(); 
                    }
                }
            });
        }

        private Color getConnectionColor(String type) {
            switch (type) {
                case "float": return new Color(139, 0, 0);
                case "integer": return new Color(0, 0, 139);
                case "int": return new Color(0, 0, 139);
                case "string": return new Color(255, 140, 0);
                case "file": return new Color(199, 21, 133);
                case "graph": return new Color(0, 139, 139);
                case "Status": return new Color(0, 100, 0);
                case "character": return new Color(184, 134, 11);
                case "char": return new Color(184, 134, 11);
                default: return Color.DARK_GRAY;
            }
        }

        public void zoom(double factor, Point focusPoint) {
            double oldZoom = zoomFactor;
            zoomFactor *= factor;
            zoomFactor = Math.max(0.3, Math.min(3.0, zoomFactor));
            gui4.this.zoomFactor = zoomFactor;

            for (FunctionBlock block : functionBlocks) {
                Point loc = block.getLocation();
                Dimension size = block.getPreferredSize();

                int newX = (int)(loc.x * zoomFactor / oldZoom);
                int newY = (int)(loc.y * zoomFactor / oldZoom);
                int newWidth = (int)(size.width * zoomFactor);
                int newHeight = (int)(size.height * zoomFactor);

                block.setBounds(newX, newY, newWidth, newHeight);
            }

            updateCanvasSize();

            Container parent = getParent();
            if (parent instanceof JViewport) {
                JViewport viewport = (JViewport) parent;
                Point viewPos = viewport.getViewPosition();
                double dx = (focusPoint.x - viewPos.x) / oldZoom;
                double dy = (focusPoint.y - viewPos.y) / oldZoom;
                int newX = (int)(dx * zoomFactor - focusPoint.x);
                int newY = (int)(dy * zoomFactor - focusPoint.y);
                viewport.setViewPosition(new Point(viewPos.x + newX, viewPos.y + newY));
            }

            repaint();
        }

        public void zoom(double factor) {
            double oldZoomFactor = zoomFactor;
            zoomFactor *= factor;
            zoomFactor = Math.max(0.3, Math.min(3.0, zoomFactor));
            gui4.this.zoomFactor = zoomFactor;
            
            for (FunctionBlock block : functionBlocks) {
                Point loc = block.getLocation();
                Dimension pref = block.getPreferredSize();
                
                int newX = (int)(loc.x * zoomFactor / oldZoomFactor);
                int newY = (int)(loc.y * zoomFactor / oldZoomFactor);
                int newWidth = (int)(pref.width * zoomFactor);
                int newHeight = (int)(pref.height * zoomFactor);
                
                block.setBounds(newX, newY, newWidth, newHeight);
            }
            
            updateCanvasSize();
            repaint();
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            Graphics2D g2 = (Graphics2D) g.create();
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

            // Draw grid
            g2.setColor(new Color(230, 230, 230));
            int gridSize = 20;
            for (int x = 0; x < getWidth(); x += gridSize) {
                g2.drawLine(x, 0, x, getHeight());
            }
            for (int y = 0; y < getHeight(); y += gridSize) {
                g2.drawLine(0, y, getWidth(), y);
            }

            g2.translate(translateX, translateY);
            g2.scale(zoomFactor, zoomFactor);

            for (Connection conn : connections) {
                Point from = getOutputPoint(conn.from, conn.fromIdx);
                Point to = getInputPoint(conn.to, conn.toIdx);
                drawCurvedArrow(g2, from, to, conn.type, conn == selectedConnection);
            }

            g2.dispose();
            
            if (!searchResults.isEmpty()) {
                Graphics2D g2d = (Graphics2D) g.create();
                g2d.setColor(new Color(255, 0, 0, 100)); 
                g2d.setStroke(new BasicStroke(3));
                for (FunctionBlock block : searchResults) {
                    Rectangle bounds = block.getBounds();
                    g2d.drawRect(bounds.x - 2, bounds.y - 2, bounds.width + 4, bounds.height + 4);
                }
                g2d.dispose();
            }
        }

        private void drawCurvedArrow(Graphics2D g2, Point from, Point to, String type, boolean isSelected) {
            int dx = to.x - from.x;
            int dy = to.y - from.y;

            int ctrlX = (from.x + to.x) / 2 + Math.min(50, Math.abs(dx) / 4);
            int ctrlY = (from.y + to.y) / 2 - Math.min(50, Math.abs(dy) / 4);

            QuadCurve2D q = new QuadCurve2D.Float();
            q.setCurve(from.x, from.y, ctrlX, ctrlY, to.x, to.y);

            Color connColor = getConnectionColor(type);
            g2.setColor(isSelected ? Color.RED : connColor);
            g2.setStroke(new BasicStroke(2));
            g2.draw(q);

            double t = 0.5;
            double oneMinusT = 1 - t;
            double midX = oneMinusT * oneMinusT * from.x + 2 * oneMinusT * t * ctrlX + t * t * to.x;
            double midY = oneMinusT * oneMinusT * from.y + 2 * oneMinusT * t * ctrlY + t * t * to.y;

            double dxTangent = 2 * (1 - t) * (ctrlX - from.x) + 2 * t * (to.x - ctrlX);
            double dyTangent = 2 * (1 - t) * (ctrlY - from.y) + 2 * t * (to.y - ctrlY);
            double angle = Math.atan2(dyTangent, dxTangent);

            Font baseFont = new Font("SansSerif", Font.PLAIN, 12);
            g2.setFont(baseFont);

            AffineTransform old = g2.getTransform();
            g2.translate(midX, midY);
            g2.rotate(angle);
            String label = type;
            g2.drawString(label, -label.length() * 3, -5);
            g2.setTransform(old);

            drawArrowHead(g2, ctrlX, ctrlY, to.x, to.y, isSelected ? Color.RED : connColor);
        }

        private void drawArrowHead(Graphics2D g2, int x1, int y1, int x2, int y2, Color color) {
            double angle = Math.atan2(y2 - y1, x2 - x1);
            int arrowLength = 10;

            int x = x2;
            int y = y2;

            int xA = (int)(x - arrowLength * Math.cos(angle - Math.PI / 6));
            int yA = (int)(y - arrowLength * Math.sin(angle - Math.PI / 6));
            int xB = (int)(x - arrowLength * Math.cos(angle + Math.PI / 6));
            int yB = (int)(y - arrowLength * Math.sin(angle + Math.PI / 6));

            int[] xPoints = {x, xA, xB};
            int[] yPoints = {y, yA, yB};

            g2.setColor(color);
            g2.fillPolygon(xPoints, yPoints, 3);
        }
    }

    Point getOutputPoint(FunctionBlock fb, int outputIndex) {
        if (fb.outputArrows == null || outputIndex >= fb.outputArrows.length) {
            return new Point(fb.getX() + fb.getWidth(), fb.getY() + 30);
        }
        JLabel output = fb.outputArrows[outputIndex];
        Point p = SwingUtilities.convertPoint(output, output.getWidth(), output.getHeight() / 2, drawingPanel);
        return new Point((int)(p.x / drawingPanel.zoomFactor), (int)(p.y / drawingPanel.zoomFactor));
    }

    Point getInputPoint(FunctionBlock fb, int inputIndex) {
        if (inputIndex == -1) {
            Point p = new Point(fb.getX() + fb.getWidth() / 2, fb.getY() + fb.getHeight() / 2);
            return new Point((int)(p.x / drawingPanel.zoomFactor), (int)(p.y / drawingPanel.zoomFactor));
        }
        if (fb.inputArrows == null || inputIndex >= fb.inputArrows.length) {
            return new Point(fb.getX(), fb.getY() + 30);
        }
        JLabel input = fb.inputArrows[inputIndex];
        Point p = SwingUtilities.convertPoint(input, 0, input.getHeight() / 2, drawingPanel);
        return new Point((int)(p.x / drawingPanel.zoomFactor), (int)(p.y / drawingPanel.zoomFactor));
    }

    private List<FunctionBlock> getTopologicalOrder() {
        Map<FunctionBlock, Set<FunctionBlock>> adj = new HashMap<>();
        Map<FunctionBlock, Integer> inDegree = new HashMap<>();

        for (FunctionBlock fb : functionBlocks) {
            adj.put(fb, new HashSet<>());
            inDegree.put(fb, 0);
        }

        for (Connection c : connections) {
            if (adj.containsKey(c.from) && adj.containsKey(c.to)) {
                adj.get(c.from).add(c.to);
                inDegree.put(c.to, inDegree.get(c.to) + 1);
            }
        }

        Queue<FunctionBlock> q = new LinkedList<>();
        Set<FunctionBlock> visited = new HashSet<>();

        for (FunctionBlock fb : functionBlocks) {
            if (inDegree.get(fb) == 0) {
                q.add(fb);
                visited.add(fb);
            }
        }

        List<FunctionBlock> order = new ArrayList<>();

        while (!q.isEmpty()) {
            FunctionBlock curr = q.poll();
            order.add(curr);

            for (FunctionBlock neighbor : adj.get(curr)) {
                inDegree.put(neighbor, inDegree.get(neighbor) - 1);
                if (inDegree.get(neighbor) == 0 && !visited.contains(neighbor)) {
                    q.add(neighbor);
                    visited.add(neighbor);
                }
            }

            if (q.isEmpty()) {
                for (FunctionBlock fb : functionBlocks) {
                    if (!visited.contains(fb)) {
                        q.add(fb);
                        visited.add(fb);
                        break;
                    }
                }
            }
        }

        if (order.size() != functionBlocks.size()) {
            Set<FunctionBlock> visitedC = new HashSet<>();
            Set<FunctionBlock> recStack = new HashSet<>();
            List<String> cyclePath = new ArrayList<>();

            for (FunctionBlock fb : functionBlocks) {
                if (!visitedC.contains(fb)) {
                    cyclePath.clear();
                    if (detectCycle(fb, adj, visitedC, recStack, cyclePath)) {
                        JOptionPane.showMessageDialog(this,
                                "Circular dependency detected:\n" + String.join(" → ", cyclePath) +
                                        "\nPlease edit/delete some arrows to resolve.",
                                "Cycle Detected",
                                JOptionPane.ERROR_MESSAGE);
                        return null;
                    }
                }
            }
        }

        for (FunctionBlock fb : functionBlocks) {
            if (!order.contains(fb)) {
                order.add(fb);
            }
        }

        return order;
    }

    private boolean detectCycle(FunctionBlock node, Map<FunctionBlock, Set<FunctionBlock>> adj,
                            Set<FunctionBlock> visited, Set<FunctionBlock> recStack, List<String> cyclePath) {
        Map<FunctionBlock, FunctionBlock> parentMap = new HashMap<>();
        return dfsCycle(node, adj, visited, recStack, parentMap, cyclePath);
    }

    private boolean dfsCycle(FunctionBlock current, Map<FunctionBlock, Set<FunctionBlock>> adj,
                            Set<FunctionBlock> visited, Set<FunctionBlock> recStack,
                            Map<FunctionBlock, FunctionBlock> parentMap, List<String> cyclePath) {
        visited.add(current);
        recStack.add(current);

        for (FunctionBlock neighbor : adj.get(current)) {
            if (!visited.contains(neighbor)) {
                parentMap.put(neighbor, current);
                if (dfsCycle(neighbor, adj, visited, recStack, parentMap, cyclePath)) {
                    return true;
                }
            } else if (recStack.contains(neighbor)) {
                List<String> tempCycle = new ArrayList<>();
                FunctionBlock temp = current;
                tempCycle.add(neighbor.name);

                while (temp != null && temp != neighbor) {
                    tempCycle.add(temp.name);
                    temp = parentMap.get(temp);
                }
                tempCycle.add(neighbor.name);

                Collections.reverse(tempCycle);
                cyclePath.addAll(tempCycle);
                return true;
            }
        }

        recStack.remove(current);
        return false;
    }

    class Connection {
        FunctionBlock from, to;
        int fromIdx, toIdx;
        String type;

        Connection(FunctionBlock from, int fromIdx, FunctionBlock to, int toIdx, String type) {
            this.from = from;
            this.fromIdx = fromIdx;
            this.to = to;
            this.toIdx = toIdx;
            this.type = type;
        }
    }

    class FunctionBlock extends JPanel {
        private static final long serialVersionUID = 1L;

        String name;
        String originalName;
        List<String> nameHistory;
        String[] inputValues;
        JButton[] outputDots;
        transient BlockTemplate template;
        public JLabel[] inputArrows;
        public JLabel[] outputArrows;
        private Component outputPanel;
        boolean isSelected = false;

        private String getDefaultValue(String type) {
            switch (type) {
                case "float": return "0.0";
                case "integer": return "0";
                case "int": return "0";
                case "string": return "default_string";
                case "file": return "";
                case "graph": return "default_graph";
                case "Status": return "default_status";
                case "character": return "a";
                case "char": return "a";
                default: return "default";
            }
        }

        // Update border with description
        void updateBorder() {
            String[] desc = getBlockDescription(name);
            setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(isSelected ? Color.BLUE : Color.GRAY, isSelected ? 2 : 1),
                "<html><b>" + name + "</b><br><font size='2' color='gray'>" + desc[0] + "</font></html>"
            ));
        }

        FunctionBlock(String name, BlockTemplate template) {
            super();
            this.name = name;
            this.originalName = name;
            this.nameHistory = new ArrayList<>();
            this.nameHistory.add(name);
            this.template = template;
            setLayout(new BorderLayout(10, 10));
            
            // Enhanced border with full name
            String[] desc = getBlockDescription(name);
            setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.GRAY, 1),
                "<html><b>" + name + "</b><br><font size='2' color='gray'>" + desc[0] + "</font></html>"
            ));
            
            setBackground(new Color(230, 230, 250));
            setOpaque(true);
            
            // Tooltip with full description
            setToolTipText("<html><b>" + desc[0] + "</b><br><br>" + 
                desc[1] + "</i><br><br>" +
                "<b>Inputs:</b> " + template.inputCount + " | <b>Outputs:</b> " + template.outputCount + 
                "<br><br><font color='blue'>Right-click for options | Ctrl+C to copy</font></html>");

            inputValues = new String[template.inputCount];
            JPanel inputPanel = new JPanel();
            inputPanel.setLayout(new GridLayout(template.inputCount, 1, 0, 8));
            inputPanel.setOpaque(false);
            inputArrows = new JLabel[template.inputCount];

            for (int i = 0; i < template.inputCount; i++) {
                inputValues[i] = getDefaultValue(template.inputTypes[i]);

                final int idx = i;
                JLabel inArrow = new JLabel("●→");
                inArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                inArrow.setForeground(getTypeColor(template.inputTypes[i]));
                inArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                inArrow.setToolTipText("<html><b>Input " + (idx + 1) + "</b><br>Type: " + template.inputTypes[idx] + "</html>");
                inArrow.setBorder(BorderFactory.createEmptyBorder(6, 5, 6, 20));
                inArrow.addMouseListener(new MouseAdapter() {
                    public void mouseReleased(MouseEvent e) {
                        if (dragSource != null) {
                            String outType = dragSource.template.outputTypes[dragSourceOutputIndex];
                            String inType = template.inputTypes[idx];
                            if (!outType.equals(inType)) {
                                JOptionPane.showMessageDialog(gui4.this,
                                    "Type mismatch: Cannot connect output (" + outType + ") to input (" + inType + ")",
                                    "Type Mismatch",
                                    JOptionPane.ERROR_MESSAGE);
                                dragSource = null;
                                dragSourceOutputIndex = -1;
                                return;
                            }

                            boolean inputAlreadyConnected = false;
                            for (Connection conn : connections) {
                                if (conn.to == FunctionBlock.this && conn.toIdx == idx) {
                                    inputAlreadyConnected = true;
                                    break;
                                }
                            }
                            if (inputAlreadyConnected) {
                                JOptionPane.showMessageDialog(gui4.this,
                                    "Input " + (idx + 1) + " of block " + name + " is already connected.",
                                    "Connection Error",
                                    JOptionPane.ERROR_MESSAGE);
                                dragSource = null;
                                dragSourceOutputIndex = -1;
                                return;
                            }

                            connections.add(new Connection(dragSource, dragSourceOutputIndex, FunctionBlock.this, idx, inType));
                            dragSource = null;
                            dragSourceOutputIndex = -1;
                            drawingPanel.setCursor(Cursor.getDefaultCursor());
                            drawingPanel.repaint();
                        }
                    }
                    
                    public void mouseEntered(MouseEvent e) {
                        inArrow.setFont(new Font("SansSerif", Font.BOLD, 16));
                    }
                    
                    public void mouseExited(MouseEvent e) {
                        inArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                    }
                });
                inputArrows[i] = inArrow;
                inputPanel.add(inArrow);
            }

            JPanel outputPanel = new JPanel(new GridLayout(template.outputCount, 1, 0, 8));
            outputPanel.setOpaque(false);
            outputArrows = new JLabel[template.outputCount];

            for (int j = 0; j < template.outputCount; j++) {
                final int oidx = j;
                JLabel outArrow = new JLabel("→●");
                outArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                outArrow.setForeground(getTypeColor(template.outputTypes[j]));
                outArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                outArrow.setToolTipText("<html><b>Output " + (oidx + 1) + "</b><br>Type: " + template.outputTypes[oidx] + "<br><i>Click to start connection</i></html>");
                outArrow.addMouseListener(new MouseAdapter() {
                    public void mouseClicked(MouseEvent e) {
                        dragSource = FunctionBlock.this;
                        dragSourceOutputIndex = oidx;
                        drawingPanel.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
                    }
                    
                    public void mouseEntered(MouseEvent e) {
                        outArrow.setFont(new Font("SansSerif", Font.BOLD, 16));
                    }
                    
                    public void mouseExited(MouseEvent e) {
                        outArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                    }
                });
                outputArrows[j] = outArrow;
                outputPanel.add(outArrow);
            }

            JPanel ioWrapper = new JPanel(new BorderLayout());
            ioWrapper.setOpaque(false);
            ioWrapper.add(inputPanel, BorderLayout.WEST);
            ioWrapper.add(new JLabel(), BorderLayout.CENTER);

            this.add(ioWrapper, BorderLayout.CENTER);
            this.add(outputPanel, BorderLayout.EAST);

            // Status connection listener
            addMouseListener(new MouseAdapter() {
                public void mousePressed(MouseEvent e) {
                    // Select this block
                    for (FunctionBlock fb : functionBlocks) {
                        fb.isSelected = false;
                        fb.updateBorder();
                    }
                    isSelected = true;
                    updateBorder();
                }
                
                public void mouseReleased(MouseEvent e) {
                    if (dragSource != null) {
                        boolean isStatusOutput = false;
                        if (dragSourceOutputIndex != -1 && dragSource.template != null &&
                            dragSourceOutputIndex < dragSource.template.outputTypes.length) {
                            isStatusOutput = "Status".equals(dragSource.template.outputTypes[dragSourceOutputIndex]);
                        }

                        if (isStatusOutput) {
                            boolean alreadyConnected = false;
                            for (Connection conn : connections) {
                                if (conn.from == dragSource && conn.to == FunctionBlock.this && conn.toIdx == -1) {
                                    alreadyConnected = true;
                                    break;
                                }
                            }
                            if (!alreadyConnected) {
                                connections.add(new Connection(dragSource, dragSourceOutputIndex, FunctionBlock.this, -1, "Status"));
                                dragSource = null;
                                dragSourceOutputIndex = -1;
                                drawingPanel.setCursor(Cursor.getDefaultCursor());
                                drawingPanel.repaint();
                            } else {
                                JOptionPane.showMessageDialog(gui4.this, "Status connection already exists.");
                            }
                        }
                    }
                }
            });

            // Context menu
            JPopupMenu menu = new JPopupMenu();
            
            JMenuItem viewInfo = new JMenuItem("📋 View Block Info");
            viewInfo.addActionListener(e -> showBlockInfo());
            menu.add(viewInfo);
            
            menu.addSeparator();
            
            JMenuItem copy = new JMenuItem("📄 Copy (Ctrl+C)");
            copy.addActionListener(e -> {
                clipboardBlock = this;
                JOptionPane.showMessageDialog(gui4.this, "Block copied to clipboard!");
            });
            menu.add(copy);
            
            JMenuItem duplicate = new JMenuItem("📋 Duplicate (Ctrl+D)");
            duplicate.addActionListener(e -> {
                clipboardBlock = this;
                pasteBlock();
            });
            menu.add(duplicate);
            
            menu.addSeparator();
            
            JMenuItem rename = new JMenuItem("✏️ Rename");
            rename.addActionListener(e -> {
                String oldName = FunctionBlock.this.name;
                String newName = JOptionPane.showInputDialog("Enter new name for " + oldName);

                if (newName != null && !newName.trim().isEmpty() && !newName.equals(oldName)) {
                    newName = newName.trim();
                    FunctionBlock.this.name = newName;
                    FunctionBlock.this.nameHistory.add(newName);
                    updateBorder();
                    repaint();
                }
            });
            menu.add(rename);

            JMenuItem changeInputs = new JMenuItem("🔧 Change Input Count");
            changeInputs.addActionListener(e -> {
                try {
                    int newCount = Integer.parseInt(JOptionPane.showInputDialog("Enter new input count:"));
                    if (newCount >= 0 && newCount != template.inputCount) {
                        connections.removeIf(conn -> conn.to == FunctionBlock.this);
                        
                        String[] newInputTypes = new String[newCount];
                        for (int i = 0; i < newCount && i < template.inputTypes.length; i++) {
                            newInputTypes[i] = template.inputTypes[i];
                        }
                        for (int i = template.inputTypes.length; i < newCount; i++) {
                            newInputTypes[i] = "float";
                        }
                        
                        template.inputCount = newCount;
                        template.inputTypes = newInputTypes;
                        
                        rebuildUI();
                    }
                } catch (Exception ignored) {}
            });
            menu.add(changeInputs);

            menu.addSeparator();
            
            JMenuItem delete = new JMenuItem("🗑️ Delete");
            delete.addActionListener(e -> {
                int confirm = JOptionPane.showConfirmDialog(gui4.this, 
                    "Delete block '" + name + "'?", 
                    "Confirm Delete", 
                    JOptionPane.YES_NO_OPTION);
                if (confirm == JOptionPane.YES_OPTION) {
                    functionBlocks.remove(FunctionBlock.this);
                    drawingPanel.remove(FunctionBlock.this);
                    connections.removeIf(conn -> conn.from == FunctionBlock.this || conn.to == FunctionBlock.this);
                    updateCanvasSize();
                    drawingPanel.repaint();
                }
            });
            menu.add(delete);

            this.setComponentPopupMenu(menu);

            int minHeight = 100;
            int numSlots = Math.max(template.inputCount, template.outputCount);
            int dynamicHeight = 30 * numSlots + 70;
            this.setSize(180, Math.max(minHeight, dynamicHeight));
            this.setPreferredSize(new Dimension(getWidth(), getHeight()));
            this.revalidate();
            this.setBounds(getLocation().x, getLocation().y, getWidth(), getHeight());

            enableDrag(this);
        }
        
        // Get color for type
        private Color getTypeColor(String type) {
            switch (type) {
                case "float": return new Color(139, 0, 0);
                case "integer": case "int": return new Color(0, 0, 139);
                case "string": return new Color(255, 140, 0);
                case "file": return new Color(199, 21, 133);
                case "graph": return new Color(0, 139, 139);
                case "Status": return new Color(0, 100, 0);
                case "character": case "char": return new Color(184, 134, 11);
                default: return Color.DARK_GRAY;
            }
        }
        
        // Show block info dialog
        private void showBlockInfo() {
            String[] desc = getBlockDescription(name);
            
            StringBuilder info = new StringBuilder();
            info.append("=== BLOCK INFORMATION ===\n\n");
            info.append("Name: ").append(name).append("\n");
            info.append("Full Name: ").append(desc[0]).append("\n");
            info.append("Description:\n").append(desc[1]).append("\n\n");
            info.append("=== INPUTS (").append(template.inputCount).append(") ===\n");
            for (int i = 0; i < template.inputCount; i++) {
                info.append("  Input ").append(i + 1).append(": ").append(template.inputTypes[i]);
                info.append(" = ").append(inputValues[i]).append("\n");
            }
            info.append("\n=== OUTPUTS (").append(template.outputCount).append(") ===\n");
            for (int i = 0; i < template.outputCount; i++) {
                info.append("  Output ").append(i + 1).append(": ").append(template.outputTypes[i]).append("\n");
            }
            
            JTextArea textArea = new JTextArea(info.toString());
            textArea.setEditable(false);
            textArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
            JScrollPane scrollPane = new JScrollPane(textArea);
            scrollPane.setPreferredSize(new Dimension(400, 350));
            
            JOptionPane.showMessageDialog(gui4.this, scrollPane, "Block Info: " + name, JOptionPane.INFORMATION_MESSAGE);
        }

        private void rebuildUI() {
            this.removeAll();

            this.setLayout(new BorderLayout(10, 10));
            updateBorder();
            this.setBackground(new Color(230, 230, 250));

            String[] oldValues = inputValues != null ? inputValues.clone() : null;
            inputValues = new String[template.inputCount];
            
            JPanel inputPanel = new JPanel();
            inputPanel.setLayout(new GridLayout(template.inputCount, 1, 0, 8));
            inputPanel.setOpaque(false);
            inputArrows = new JLabel[template.inputCount];

            for (int i = 0; i < template.inputCount; i++) {
                inputValues[i] = (oldValues != null && i < oldValues.length) ? oldValues[i] : getDefaultValue(template.inputTypes[i]);

                final int idx = i;
                JLabel inArrow = new JLabel("●→");
                inArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                inArrow.setForeground(getTypeColor(template.inputTypes[i]));
                inArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                inArrow.setToolTipText("<html><b>Input " + (idx + 1) + "</b><br>Type: " + template.inputTypes[idx] + "</html>");
                inArrow.setBorder(BorderFactory.createEmptyBorder(6, 5, 6, 20));
                inArrow.addMouseListener(new MouseAdapter() {
                    public void mouseReleased(MouseEvent e) {
                        if (dragSource != null) {
                            String outType = dragSource.template.outputTypes[dragSourceOutputIndex];
                            String inType = template.inputTypes[idx];
                            if (!outType.equals(inType)) {
                                JOptionPane.showMessageDialog(gui4.this,
                                    "Type mismatch: Cannot connect output (" + outType + ") to input (" + inType + ")",
                                    "Type Mismatch",
                                    JOptionPane.ERROR_MESSAGE);
                                dragSource = null;
                                dragSourceOutputIndex = -1;
                                return;
                            }

                            connections.add(new Connection(dragSource, dragSourceOutputIndex, FunctionBlock.this, idx, inType));
                            dragSource = null;
                            dragSourceOutputIndex = -1;
                            drawingPanel.setCursor(Cursor.getDefaultCursor());
                            drawingPanel.repaint();
                        }
                    }
                });
                inputArrows[i] = inArrow;
                inputPanel.add(inArrow);
            }

            JPanel outputPanel = new JPanel(new GridLayout(template.outputCount, 1, 0, 8));
            outputPanel.setOpaque(false);
            outputArrows = new JLabel[template.outputCount];

            for (int j = 0; j < template.outputCount; j++) {
                final int oidx = j;
                JLabel outArrow = new JLabel("→●");
                outArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                outArrow.setForeground(getTypeColor(template.outputTypes[j]));
                outArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                outArrow.setToolTipText("<html><b>Output " + (oidx + 1) + "</b><br>Type: " + template.outputTypes[oidx] + "</html>");
                outArrow.addMouseListener(new MouseAdapter() {
                    public void mouseClicked(MouseEvent e) {
                        dragSource = FunctionBlock.this;
                        dragSourceOutputIndex = oidx;
                    }
                });
                outputArrows[j] = outArrow;
                outputPanel.add(outArrow);
            }

            JPanel ioWrapper = new JPanel(new BorderLayout());
            ioWrapper.setOpaque(false);
            ioWrapper.add(inputPanel, BorderLayout.WEST);
            ioWrapper.add(new JLabel(), BorderLayout.CENTER);

            this.add(ioWrapper, BorderLayout.CENTER);
            this.add(outputPanel, BorderLayout.EAST);

            int minHeight = 100;
            int numSlots = Math.max(template.inputCount, template.outputCount);
            int dynamicHeight = 30 * numSlots + 70;
            this.setSize(180, Math.max(minHeight, dynamicHeight));
            this.setPreferredSize(new Dimension(getWidth(), getHeight()));
            this.revalidate();
            this.repaint();
        }

        void startDrag(FunctionBlock src, int outIndex) {
            dragSource = src;
            dragSourceOutputIndex = outIndex;
        }

        public String[] getInputs() {
            return inputValues;
        }

        public String getName() {
            return name;
        }

        private void enableDrag(JComponent comp) {
            final Point[] offset = new Point[1];

            comp.addMouseListener(new MouseAdapter() {
                public void mousePressed(MouseEvent e) {
                    offset[0] = e.getPoint();
                    comp.requestFocusInWindow(); 
                }
            });

            comp.addMouseMotionListener(new MouseMotionAdapter() {
                public void mouseDragged(MouseEvent e) {
                    Point parent = SwingUtilities.convertPoint(comp, e.getPoint(), drawingPanel);
                    if (offset[0] != null) {
                        int newX = parent.x - offset[0].x;
                        int newY = parent.y - offset[0].y;
                        
                        comp.setLocation(newX, newY);
                        updateCanvasSize();
                        drawingPanel.repaint();
                    }
                }
            });
        }
    }

    private boolean isPointNearLine(Point pt, Point a, Point b, double tolerance) {
        double dist = ptLineDist(a.x, a.y, b.x, b.y, pt.x, pt.y);
        return dist <= tolerance;
    }

    private double ptLineDist(int x1, int y1, int x2, int y2, int px, int py) {
        double dx = x2 - x1;
        double dy = y2 - y1;
        if (dx == 0 && dy == 0) {
            dx = px - x1;
            dy = py - y1;
            return Math.sqrt(dx * dx + dy * dy);
        }

        double t = ((px - x1) * dx + (py - y1) * dy) / (dx * dx + dy * dy);
        if (t < 0) {
            dx = px - x1;
            dy = py - y1;
        } else if (t > 1) {
            dx = px - x2;
            dy = py - y2;
        } else {
            double nearX = x1 + t * dx;
            double nearY = y1 + t * dy;
            dx = px - nearX;
            dy = py - nearY;
        }
        return Math.sqrt(dx * dx + dy * dy);
    }

    private String saveExecutionPlan(List<FunctionBlock> order) {
        if (order == null || order.isEmpty()) {
            System.err.println("No execution plan to save.");
            return "";
        }

        StringBuilder content = new StringBuilder();

        try (PrintWriter writer = new PrintWriter("execution.txt")) {

            content.append("# Topological Sort Order\n");
            writer.println("# Topological Sort Order");
            for (int i = 0; i < order.size(); i++) {
                FunctionBlock fb = order.get(i);
                String[] desc = getBlockDescription(fb.originalName);
                String line = (i + 1) + ". " + fb.originalName + " (" + desc[0] + ")";
                content.append(line).append("\n");
                writer.println(line);
            }
            content.append("\n");
            writer.println();

            for (FunctionBlock fb : order) {
                String varName = fb.originalName;
                String[] desc = getBlockDescription(fb.originalName);
                
                String headerLine = "# " + desc[0] + " - " + desc[2];
                content.append(headerLine).append("\n");
                writer.println(headerLine);
                
                String letLine = "let $" + varName;
                content.append(letLine).append("\n");
                writer.println(letLine);

                if (fb.template.outputTypes.length > 0 &&
                    "Status".equals(fb.template.outputTypes[0])) {
                    String statusLine = "#status " + fb.originalName;
                    content.append(statusLine).append("\n");
                    writer.println(statusLine);
                }

                for (int i = 0; i < fb.template.inputCount; i++) {
                    String inputValue = "";

                    for (Connection c : connections) {
                        if (c.to == fb && c.toIdx == i) {
                            inputValue = "$" + c.from.originalName + ".output" + (c.fromIdx + 1);
                            break;
                        }
                    }

                    if (inputValue.isEmpty()) {
                        inputValue = fb.inputValues[i];
                    }

                    String inputLine = "    input" + (i + 1) + " " + inputValue;
                    content.append(inputLine).append("\n");
                    writer.println(inputLine);
                }

                for (int o = 0; o < fb.template.outputCount; o++) {
                    String outputLine = "    output" + (o + 1) + " $" + varName + ".output" + (o + 1);
                    content.append(outputLine).append("\n");
                    writer.println(outputLine);
                }
                content.append("\n");
                writer.println();
            }

            content.append("# Connections\n");
            writer.println("# Connections");
            for (Connection c : connections) {
                String connLine;
                if (c.toIdx == -1) {
                    connLine = "$" + c.from.originalName + ".output" + (c.fromIdx + 1) + " -> $" + c.to.originalName + " (status)";
                } else {
                    connLine = "$" + c.from.originalName + ".output" + (c.fromIdx + 1)
                            + " -> $" + c.to.originalName + ".input" + (c.toIdx + 1);
                }
                content.append(connLine).append("\n");
                writer.println(connLine);
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

        return content.toString();
    }

   
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception e) {
                e.printStackTrace();
            }
            new gui4().setVisible(true);
        });
    }
}