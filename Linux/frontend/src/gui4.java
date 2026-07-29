import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
import java.awt.geom.CubicCurve2D;
import java.awt.geom.Path2D;
import java.awt.geom.QuadCurve2D;
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
    private FunctionBlock clipboardBlock = null;
    private JButton hamburgerButton;
    private JTextArea historyArea;
    private List<String> executionHistory = new ArrayList<>();
    private static final String HISTORY_FILE = "execution_history.txt";
    private static final String DESCRIPTIONS_FILE = "block_descriptions.txt";
    private static final String TEMPLATES_FILE = "block_templates.txt";
    // === FILE PANEL FIELDS ===
    private JPanel fileEntriesPanel;
    private JLabel filePanelSourceLabel;
    private static final String DEFAULT_INPUT_FILE = "Data_Coming_From_Frontend.txt";
    private String currentInputFilePath = DEFAULT_INPUT_FILE;
    private File lastSavedPipelineFile = null;
    private int  selectedGroupIndex = -1;   // -1 = All
    private JButton[] groupSidebarButtons;  // sidebar nav buttons
    // FileEntryConnection: a connection from a file-panel entry to a block input (no line drawn)
    static class FileEntryConnection {
        String entryName;     // param name from file
        String entryType;     // integer/float/string/boolean/file/...
        String entryValue;    // value from file
        int    entryPosition; // 1-based row number in the data file
        FunctionBlock toBlock;
        int toInputIndex;     // 0-based
        String fileName;      // auto-generated, editable
        FileEntryConnection(String entryName, String entryType, String entryValue,
                            int entryPosition, FunctionBlock toBlock, int toInputIndex) {
            this.entryName     = entryName;
            this.entryType     = entryType;
            this.entryValue    = entryValue;
            this.entryPosition = entryPosition;
            this.toBlock       = toBlock;
            this.toInputIndex  = toInputIndex;
            // Auto name: entryName_blockName_inputN
            this.fileName = entryName + "_" + toBlock.name + "_input" + (toInputIndex + 1);
        }
    }
    List<FileEntryConnection> fileEntryConnections = new ArrayList<>();
    // Pending file-entry connection: set when user clicks 'Connect' on a file panel card
    String pendingEntryName     = null;
    String pendingEntryType     = null;
    String pendingEntryValue    = null;
    int    pendingEntryPosition = 0;   // 1-based position in the data file
    private static Map<String, String[]> BLOCK_DESCRIPTIONS = new HashMap<>();
    private boolean embeddedMode = false;
    private Runnable onExecuteCallback;
    static {
        BLOCK_DESCRIPTIONS.put("let", new String[]{
            "Let Variable Assignment",
            "Assigns values to variables. Takes two integer inputs and produces a file output.",
            "Variable Operations"
        });
        
        BLOCK_DESCRIPTIONS.put("cudf", new String[]{
            "Create Up Down File",
            "Processes package dependency files and generates upgrade plans. Used for managing software package dependencies and conflicts.",
            "Package Management"
        });
        BLOCK_DESCRIPTIONS.put("exec", new String[]{
            "Execute",
            "Executes a command or process. Takes two integer inputs and produces a file output.",
            "Execution"
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
            "Write Graph Xml",
            "Processes weighted graph data and performs graph transformations. Combines two string inputs into a processed output.",
            "Graph Processing"
        });
        BLOCK_DESCRIPTIONS.put("rgx", new String[]{
            "Read Graph Xml",
            "Reads graph data from XML format. Takes two integer inputs and produces a file output.",
            "Graph Processing"
        });
        
        BLOCK_DESCRIPTIONS.put("fb_rch", new String[]{
            "Final Bound Reach",
            "Calculates reachability bounds in graphs. Takes two integer inputs and produces a file output.",
            "Graph Algorithms"
        });
        
        BLOCK_DESCRIPTIONS.put("pathz3", new String[]{
            "Path Z3",
            "Uses Z3 solver for path analysis. Takes two integer inputs and produces a file output.",
            "Analysis"
        });
        
        BLOCK_DESCRIPTIONS.put("result", new String[]{
            "Result",
            "Aggregates multiple inputs (float, string, file, graph) and produces consolidated outputs. Final processing block for workflows.",
            "Output/Aggregation"
        });
    }
    private static String getDefaultValue(String type) {
        if (type == null) return "default";
        switch (type.trim().toLowerCase()) {
            case "float": return "0.0";
            case "integer": return "0";
            case "int": return "0";
            case "string": return "default_string";
            case "file": return "";
            case "graph": return "default_graph";
            case "status": return "default_status";
            case "character": return "a";
            case "char": return "a";
            default: return "default";
        }
    }
    public static String[] getBlockDescription(String blockName) {
        if (blockName == null) {
            return new String[]{"Unknown", "No description available", "Custom"};
        }
        String baseName = blockName.replaceAll("_\\d+$", "");
        String[] desc = BLOCK_DESCRIPTIONS.get(baseName);
        if (desc != null && desc.length >= 3) {
            return desc;
        } else if (desc != null && desc.length == 2) {
            return new String[]{desc[0], desc[1], "Custom"};
        }
        return new String[]{
            blockName.toUpperCase(),
            "Custom block - No description available",
            "Custom"
        };
    }
    public gui4() {
        loadBlockDescriptions();
        loadBlockTemplates();
        loadExecutionHistory();
        initializeGUI();
        
        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                saveAllData();
            }
        });
    }
    public gui4(boolean embedded) {
        this.embeddedMode = embedded;
        loadBlockDescriptions();
        loadBlockTemplates();
        loadExecutionHistory();
        initializeGUI();
        if (!embedded) {
            addWindowListener(new WindowAdapter() {
                @Override
                public void windowClosing(WindowEvent e) {
                    saveAllData();
                }
            });
        }
    }
    public void setOnExecuteCallback(Runnable callback) {
        this.onExecuteCallback = callback;
    }
    private void saveAllData() {
        saveBlockDescriptions();
        saveBlockTemplates();
        saveExecutionHistory();
    }
    // =====================================================
    // SAVE / LOAD PIPELINE
    // =====================================================
    private static String encodeVal(String s) {
        if (s == null) return "";
        try { return java.net.URLEncoder.encode(s, "UTF-8"); }
        catch (Exception e) { return s; }
    }
    private static String decodeVal(String s) {
        if (s == null || s.isEmpty()) return "";
        try { return java.net.URLDecoder.decode(s, "UTF-8"); }
        catch (Exception e) { return s; }
    }
    private void savePipeline() {
        JFileChooser chooser = new JFileChooser(".");
        chooser.setDialogTitle("Save Pipeline");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter(
            "Pipeline files (*.pipeline)", "pipeline"));
        chooser.setSelectedFile(new File("my_pipeline.pipeline"));
        if (chooser.showSaveDialog(this) != JFileChooser.APPROVE_OPTION) return;
        File file = chooser.getSelectedFile();
        if (!file.getName().contains(".")) {
            file = new File(file.getAbsolutePath() + ".pipeline");
        }
        try (PrintWriter w = new PrintWriter(new FileWriter(file))) {
            writePipelineTo(w);
            lastSavedPipelineFile = file;
            JOptionPane.showMessageDialog(this,
                "Pipeline saved successfully!\n" + file.getAbsolutePath()
                + "\n\nBlocks: " + functionBlocks.size()
                + "  |  Connections: " + connections.size(),
                "Pipeline Saved", JOptionPane.INFORMATION_MESSAGE);
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this,
                "Error saving pipeline:\n" + ex.getMessage(),
                "Save Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    /** Creates the standard 7-block FuSION template pipeline and saves it as new_pipeline_2.pipeline. */
    private void loadTemplatePipeline() {
        if (!functionBlocks.isEmpty()) {
            int res = JOptionPane.showConfirmDialog(this,
                "This will replace the current pipeline. Continue?",
                "Load Template Pipeline", JOptionPane.YES_NO_OPTION);
            if (res != JOptionPane.YES_OPTION) return;
        }
        applyTemplatePipeline(true);
    }

    private void applyTemplatePipeline(boolean showDialogs) {
        // Clear existing pipeline
        for (FunctionBlock fb : functionBlocks) drawingPanel.remove(fb);
        functionBlocks.clear();
        connections.clear();
        fileEntryConnections.clear();
        dragSource = null; dragSourceOutputIndex = -1; selectedConnection = null;
        for (String key : instanceCounter.keySet()) instanceCounter.put(key, 0);
        functionCounter = 1;

        int y0 = 50, gap = 250;
        FunctionBlock cudfB     = makeTemplateBlock("cudf",   50,  y0);
        FunctionBlock startB    = makeTemplateBlock("start",  50,  y0 + gap);
        FunctionBlock mffB      = makeTemplateBlock("mff",    50,  y0 + gap * 2);
        FunctionBlock wgxB      = makeTemplateBlock("wgx",    50,  y0 + gap * 3);
        FunctionBlock rgxB      = makeTemplateBlock("rgx",    50,  y0 + gap * 4);
        FunctionBlock fb_rchB   = makeTemplateBlock("fb_rch", 50,  y0 + gap * 5);
        FunctionBlock pathz3WB  = makeTemplateBlock("pathz3", 380, y0 + gap);
        FunctionBlock pathz3WoB = makeTemplateBlock("pathz3", 680, y0 - 50);

        if (cudfB == null || startB == null || mffB == null || wgxB == null
                || rgxB == null || fb_rchB == null || pathz3WB == null || pathz3WoB == null) {
            if (showDialogs) JOptionPane.showMessageDialog(this,
                "Could not create template — one or more block types missing from library.",
                "Template Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        // Wire input-panel entries to block inputs (FENTRY connections only, no block-to-block)
        {
            // Pre-populate with known param types so connections exist even before a data file is loaded.
            // Values are overridden below if the data file already exists.
            String[][] knownParams = {
                {"LOG_FOLD_CHANGE_FILE","file"}, {"LIST_OF_MERGED_KEGG_XML_FILE","file"},
                {"MERGED_KEGG_PATHWAYS_XML_FILE","file"}, {"ADDITIONAL_EDGES_FILE","file"},
                {"ESSENTIAL_EDGES_FILE","file"}, {"AVOID_EDGES_FILE","file"},
                {"INACTIVE_NODES_FILE","file"}, {"CONFIRMED_UP_REG_FILE","file"},
                {"CONFIRMED_DOWN_REG_FILE","file"}, {"RELAXED_NODES_FILE","file"},
                {"RELAXED_EDGES_FILE","file"}, {"NONRELAXED_EDGES_FILE","file"},
                {"COEXPRESSION_CSV","file"}, {"HSA_TO_GENE_SYMBOL_MAP_FILE","file"},
                {"HSA_PATH_TO_PATH_NAME_MAP_FILE","file"}, {"INTER_DB_MAP_FILE","file"},
                {"HSA_IDS_NOT_TO_BE_MERGED_FILE","file"}, {"WORK_DIR","string"},
                {"SRC_NODE","string"}, {"TGT_NODE","string"},
                {"NODE_TO_TEST_FOR_SIGNIFICANCE","string"},
                {"REACH_PATH_BOUND","integer"}, {"EDGE_RELAX_LB","integer"},
                {"EDGE_RELAX_UB","integer"}, {"NODE_RELAX_LB","integer"},
                {"NODE_RELAX_UB","integer"}, {"NODE_SPLIT_THRESHOLD","integer"},
                {"CONSTR_SOLVER_TIMEOUT1","integer"}, {"CONSTR_SOLVER_TIMEOUT2","integer"},
                {"NUM_SOLNS_TO_COUNT","integer"}, {"NUM_SOLNS_TO_EXPLORE","integer"},
                {"UP_REG_THRESH","float"}, {"DOWN_REG_THRESH","float"},
                {"COEXP_THRESH","float"}, {"FROZEN_THRESH","float"},
                {"EDGES_TO_TARGET","integer"},
            };
            java.util.Map<String, String[]> ent = new java.util.LinkedHashMap<>();
            for (int i = 0; i < knownParams.length; i++) {
                ent.put(knownParams[i][0], new String[]{knownParams[i][1], "", String.valueOf(i + 1)});
            }
            // Override with real values from data file if it already exists
            File df = new File(currentInputFilePath);
            if (!df.isAbsolute() || !df.exists()) {
                String dir = getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
                File alt = new File(new File(dir).getParent(), currentInputFilePath);
                if (alt.exists()) df = alt;
            }
            if (df.exists()) {
                try (BufferedReader br = new BufferedReader(new java.io.FileReader(df))) {
                    String ln; int pos = 0;
                    while ((ln = br.readLine()) != null) {
                        ln = ln.trim();
                        if (ln.isEmpty() || ln.startsWith("#")) continue;
                        pos++;
                        String[] p = ln.split(",", 3);
                        if (p.length >= 2) {
                            String nm = p[0].trim(), tp = p[1].trim();
                            String val = p.length >= 3 ? p[2].trim() : "";
                            ent.put(nm.toUpperCase(), new String[]{tp, val, String.valueOf(pos)});
                        }
                    }
                } catch (Exception ignored) {}
            }
            // cudf: log fold change file + thresholds
            linkEntry(ent, "LOG_FOLD_CHANGE_FILE", cudfB, 0);
            linkEntry(ent, "UP_REG_THRESH",        cudfB, 1);
            linkEntry(ent, "DOWN_REG_THRESH",      cudfB, 2);
            // start: reference map files ([1] is derived exception file, skip)
            linkEntry(ent, "INTER_DB_MAP_FILE",               startB, 0);
            linkEntry(ent, "HSA_TO_GENE_SYMBOL_MAP_FILE",     startB, 2);
            linkEntry(ent, "HSA_PATH_TO_PATH_NAME_MAP_FILE",  startB, 3);
            // mff: user provides either a list-of-XMLs file or a pre-merged XML — connect both to [0]
            linkEntry(ent, "LIST_OF_MERGED_KEGG_XML_FILE",    mffB, 0);
            linkEntry(ent, "MERGED_KEGG_PATHWAYS_XML_FILE",   mffB, 0);
            linkEntry(ent, "NODE_SPLIT_THRESHOLD",            mffB, 1);
            linkEntry(ent, "INTER_DB_MAP_FILE",               mffB, 2);
            // rgx: [0] is derived; node split threshold → [1]
            linkEntry(ent, "NODE_SPLIT_THRESHOLD", rgxB, 1);
            // fb_rch: src/tgt node IDs, path bound, edge direction
            linkEntry(ent, "SRC_NODE",         fb_rchB, 0);
            linkEntry(ent, "TGT_NODE",         fb_rchB, 1);
            linkEntry(ent, "REACH_PATH_BOUND", fb_rchB, 2);
            linkEntry(ent, "EDGES_TO_TARGET",  fb_rchB, 3);
            // pathz3 (with FSC) — all frontend-provided slots
            linkEntry(ent, "EDGE_RELAX_LB",           pathz3WB,  3);
            linkEntry(ent, "NODE_RELAX_LB",           pathz3WB,  4);
            linkEntry(ent, "EDGE_RELAX_UB",           pathz3WB,  5);
            linkEntry(ent, "NODE_RELAX_UB",           pathz3WB,  6);
            linkEntry(ent, "REACH_PATH_BOUND",        pathz3WB,  7);
            linkEntry(ent, "ESSENTIAL_EDGES_FILE",    pathz3WB, 13);
            linkEntry(ent, "AVOID_EDGES_FILE",        pathz3WB, 14);
            linkEntry(ent, "INACTIVE_NODES_FILE",     pathz3WB, 16);
            linkEntry(ent, "CONFIRMED_UP_REG_FILE",   pathz3WB, 17);
            linkEntry(ent, "CONFIRMED_DOWN_REG_FILE", pathz3WB, 18);
            linkEntry(ent, "RELAXED_NODES_FILE",      pathz3WB, 19);
            linkEntry(ent, "RELAXED_EDGES_FILE",      pathz3WB, 21);
            linkEntry(ent, "NONRELAXED_EDGES_FILE",   pathz3WB, 22);
            linkEntry(ent, "LOG_FOLD_CHANGE_FILE",    pathz3WB, 23);
            linkEntry(ent, "NUM_SOLNS_TO_COUNT",      pathz3WB, 24);
            linkEntry(ent, "NUM_SOLNS_TO_EXPLORE",    pathz3WB, 25);
            linkEntry(ent, "CONSTR_SOLVER_TIMEOUT1",  pathz3WB, 26);
            linkEntry(ent, "CONSTR_SOLVER_TIMEOUT2",  pathz3WB, 27);
            // pathz3 (without FSC) — same frontend slots
            linkEntry(ent, "EDGE_RELAX_LB",           pathz3WoB,  3);
            linkEntry(ent, "NODE_RELAX_LB",           pathz3WoB,  4);
            linkEntry(ent, "EDGE_RELAX_UB",           pathz3WoB,  5);
            linkEntry(ent, "NODE_RELAX_UB",           pathz3WoB,  6);
            linkEntry(ent, "REACH_PATH_BOUND",        pathz3WoB,  7);
            linkEntry(ent, "ESSENTIAL_EDGES_FILE",    pathz3WoB, 13);
            linkEntry(ent, "AVOID_EDGES_FILE",        pathz3WoB, 14);
            linkEntry(ent, "INACTIVE_NODES_FILE",     pathz3WoB, 16);
            linkEntry(ent, "CONFIRMED_UP_REG_FILE",   pathz3WoB, 17);
            linkEntry(ent, "CONFIRMED_DOWN_REG_FILE", pathz3WoB, 18);
            linkEntry(ent, "RELAXED_NODES_FILE",      pathz3WoB, 19);
            linkEntry(ent, "RELAXED_EDGES_FILE",      pathz3WoB, 21);
            linkEntry(ent, "NONRELAXED_EDGES_FILE",   pathz3WoB, 22);
            linkEntry(ent, "LOG_FOLD_CHANGE_FILE",    pathz3WoB, 23);
            linkEntry(ent, "NUM_SOLNS_TO_COUNT",      pathz3WoB, 24);
            linkEntry(ent, "NUM_SOLNS_TO_EXPLORE",    pathz3WoB, 25);
            linkEntry(ent, "CONSTR_SOLVER_TIMEOUT1",  pathz3WoB, 26);
            linkEntry(ent, "CONSTR_SOLVER_TIMEOUT2",  pathz3WoB, 27);
        }

        for (FunctionBlock fb : functionBlocks) fb.refreshInputBadges();
        updateCanvasSize();
        drawingPanel.repaint();

        // Save as my_pipeline2.pipeline in the pipeline/ sibling folder
        File saveDir;
        String classDir = getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
        File pipelineFolder = new File(new File(classDir).getParentFile().getParentFile(), "pipeline");
        if (pipelineFolder.exists() && pipelineFolder.isDirectory()) {
            saveDir = pipelineFolder;
        } else if (lastSavedPipelineFile != null && lastSavedPipelineFile.getParentFile() != null) {
            saveDir = lastSavedPipelineFile.getParentFile();
        } else {
            saveDir = new File(System.getProperty("user.dir"));
        }
        File pipeFile = new File(saveDir, "my_pipeline2.pipeline");
        try (PrintWriter pw = new PrintWriter(new java.io.FileWriter(pipeFile))) {
            writePipelineTo(pw);
            lastSavedPipelineFile = pipeFile;
            if (showDialogs) JOptionPane.showMessageDialog(this,
                "Template pipeline created and saved:\n" + pipeFile.getAbsolutePath()
                + "\n\nBlocks: " + functionBlocks.size()
                + "  |  Connections: " + (connections.size() + fileEntryConnections.size()),
                "Template Pipeline Ready", JOptionPane.INFORMATION_MESSAGE);
        } catch (Exception ex) {
            if (showDialogs) JOptionPane.showMessageDialog(this,
                "Template pipeline created.\nCould not auto-save: " + ex.getMessage(),
                "Template Pipeline", JOptionPane.WARNING_MESSAGE);
        }
    }

    /** Creates a single FunctionBlock of the given type, adds it to the canvas, returns it. */
    private FunctionBlock makeTemplateBlock(String type, int x, int y) {
        BlockTemplate template = blockLibrary.get(type);
        if (template == null) return null;
        int count = instanceCounter.getOrDefault(type, 0) + 1;
        instanceCounter.put(type, count);
        FunctionBlock fb = new FunctionBlock(type + "_" + count, template);
        fb.originalName = type;
        fb.setBounds(x, y, fb.getPreferredSize().width, fb.getPreferredSize().height);
        functionBlocks.add(fb);
        drawingPanel.add(fb);
        functionCounter++;
        return fb;
    }

    /** Returns the current data-file entries as UPPERCASE_NAME → [type, value, positionStr]. */
    private java.util.Map<String, String[]> getDataFileEntriesMap() {
        java.util.Map<String, String[]> map = new java.util.LinkedHashMap<>();
        File f = new File(currentInputFilePath);
        if (!f.isAbsolute() || !f.exists()) {
            String dir = getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
            File alt = new File(new File(dir).getParent(), currentInputFilePath);
            if (alt.exists()) f = alt;
        }
        if (!f.exists()) return map;
        try (BufferedReader br = new BufferedReader(new FileReader(f))) {
            String line; int pos = 0;
            while ((line = br.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#")) continue;
                pos++;
                String[] parts = line.split(",", 3);
                String name  = parts[0].trim().toUpperCase();
                String type  = parts.length >= 2 ? parts[1].trim() : "string";
                String value = parts.length >= 3 ? parts[2].trim() : "";
                map.put(name, new String[]{type, value, String.valueOf(pos)});
            }
        } catch (Exception ignored) {}
        return map;
    }

    /** Creates a FileEntryConnection from the data-file entry named paramName to the given block input.
        Also sets the block's inputValues[inputIdx] to the entry value. */
    private void linkEntry(java.util.Map<String, String[]> entries, String paramName,
                           FunctionBlock toBlock, int inputIdx) {
        String[] e = entries.get(paramName.toUpperCase());
        if (e == null) return;
        toBlock.inputValues[inputIdx] = e[1];
        FileEntryConnection fec = new FileEntryConnection(
                paramName, e[0], e[1], Integer.parseInt(e[2]), toBlock, inputIdx);
        fileEntryConnections.add(fec);
    }

    /** Returns the value for a key from the data-file entries map, or "" if absent. */
    private String entVal(java.util.Map<String, String[]> ent, String key) {
        String[] v = ent.get(key.toUpperCase());
        if (v != null && v.length >= 2 && !v[1].isEmpty()) return v[1];
        java.util.Map<String, String> aliases = getParamAliases();
        for (java.util.Map.Entry<String, String> ae : aliases.entrySet()) {
            if (ae.getValue().equalsIgnoreCase(key)) {
                v = ent.get(ae.getKey().toUpperCase());
                if (v != null && v.length >= 2 && !v[1].isEmpty()) return v[1];
            }
        }
        return "";
    }

    /** Serializes the current pipeline state to any PrintWriter (file or string). */
    private void writePipelineTo(PrintWriter w) {
        w.println("#PIPELINE_V1");
        w.println("ZOOM=" + zoomFactor);
        w.println("FUNC_COUNTER=" + functionCounter);
        for (Map.Entry<String, Integer> entry : instanceCounter.entrySet()) {
            w.println("INST=" + encodeVal(entry.getKey()) + "," + entry.getValue());
        }
        w.println("BLOCK_COUNT=" + functionBlocks.size());
        for (int b = 0; b < functionBlocks.size(); b++) {
            FunctionBlock fb = functionBlocks.get(b);
            w.println("BLOCK_BEGIN=" + b);
            w.println("B_NAME=" + encodeVal(fb.name));
            w.println("B_ORIG=" + encodeVal(fb.originalName));
            w.println("B_TMPL=" + encodeVal(fb.template.name));
            w.println("B_INCOUNT=" + fb.template.inputCount);
            w.println("B_OUTCOUNT=" + fb.template.outputCount);
            for (int i = 0; i < fb.template.inputCount; i++) w.println("B_INTYPE=" + encodeVal(fb.template.inputTypes[i]));
            for (int i = 0; i < fb.template.outputCount; i++) w.println("B_OUTTYPE=" + encodeVal(fb.template.outputTypes[i]));
            for (int i = 0; i < fb.template.inputCount; i++) w.println("B_INVAL=" + encodeVal(fb.inputValues[i]));
            for (int i = 0; i < fb.template.outputCount; i++) {
                String ov = (fb.outputVarNames != null && i < fb.outputVarNames.length) ? fb.outputVarNames[i] : "";
                w.println("B_OUTVAR=" + encodeVal(ov != null ? ov : ""));
            }
            w.println("B_X=" + fb.getX());
            w.println("B_Y=" + fb.getY());
            w.println("B_W=" + fb.getWidth());
            w.println("B_H=" + fb.getHeight());
            for (String h : fb.nameHistory) w.println("B_HIST=" + encodeVal(h));
            w.println("BLOCK_END");
        }
        for (Connection c : connections) {
            int fi = functionBlocks.indexOf(c.from);
            int ti = functionBlocks.indexOf(c.to);
            if (fi >= 0 && ti >= 0) {
                w.println("CONN=" + fi + "," + c.fromIdx + "," + ti + "," + c.toIdx
                    + "," + encodeVal(c.type != null ? c.type : "")
                    + "," + encodeVal(c.fileName != null ? c.fileName : ""));
            }
        }
        for (FileEntryConnection fec : fileEntryConnections) {
            int ti = functionBlocks.indexOf(fec.toBlock);
            if (ti >= 0) {
                w.println("FENTRY=" + encodeVal(fec.entryName)
                    + "," + encodeVal(fec.entryType)
                    + "," + encodeVal(fec.entryValue != null ? fec.entryValue : "")
                    + "," + fec.entryPosition
                    + "," + ti + "," + fec.toInputIndex
                    + "," + encodeVal(fec.fileName != null ? fec.fileName : ""));
            }
        }
        w.println("#END");
    }

    /** Returns the pipeline serialized as a String (for embedding in session JSON). */
    public String getPipelineAsString() {
        java.io.StringWriter sw = new java.io.StringWriter();
        try (PrintWriter w = new PrintWriter(sw)) { writePipelineTo(w); }
        return sw.toString();
    }
    @SuppressWarnings("unchecked")
    private void loadPipeline() {
        JFileChooser chooser = new JFileChooser(".");
        chooser.setDialogTitle("Load Pipeline");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter(
            "Pipeline files (*.pipeline)", "pipeline"));
        if (chooser.showOpenDialog(this) != JFileChooser.APPROVE_OPTION) return;
        File file = chooser.getSelectedFile();
        if (!file.exists()) {
            JOptionPane.showMessageDialog(this, "File not found.",
                "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        if (!functionBlocks.isEmpty()) {
            int confirm = JOptionPane.showConfirmDialog(this,
                "Loading a pipeline will replace the current one.\nDo you want to continue?",
                "Confirm Load", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);
            if (confirm != JOptionPane.YES_OPTION) return;
        }
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            doLoadPipeline(reader);
            JOptionPane.showMessageDialog(this,
                "Pipeline loaded successfully!\n"
                + functionBlocks.size() + " blocks, "
                + connections.size() + " connections loaded.\n\n"
                + "File: " + file.getName(),
                "Pipeline Loaded", JOptionPane.INFORMATION_MESSAGE);
        } catch (Exception ex) {
            JOptionPane.showMessageDialog(this,
                "Error loading pipeline:\n" + ex.getMessage(),
                "Load Error", JOptionPane.ERROR_MESSAGE);
            ex.printStackTrace();
        }
    }

    @SuppressWarnings("unchecked")
    private void doLoadPipeline(BufferedReader reader) throws Exception {
        String firstLine = reader.readLine();
        if (firstLine == null || !firstLine.trim().equals("#PIPELINE_V1")) {
            throw new Exception("Invalid pipeline file format.");
        }
            // Clear existing state
            for (FunctionBlock fb : functionBlocks) {
                drawingPanel.remove(fb);
            }
            functionBlocks.clear();
            connections.clear();
            fileEntryConnections.clear();
            dragSource = null;
            dragSourceOutputIndex = -1;
            selectedConnection = null;
            clipboardBlock = null;
            // Parse the file
            List<List<String>> blockDataList = new ArrayList<>();
            List<String> currentBlockLines = null;
            List<String> connLines = new ArrayList<>();
            List<String> fentryLines = new ArrayList<>();
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.startsWith("#END")) break;
                if (line.isEmpty() || line.startsWith("#")) continue;
                if (line.startsWith("ZOOM=")) {
                    zoomFactor = Double.parseDouble(line.substring(5));
                    drawingPanel.zoomFactor = zoomFactor;
                } else if (line.startsWith("FUNC_COUNTER=")) {
                    functionCounter = Integer.parseInt(line.substring(13));
                } else if (line.startsWith("INST=")) {
                    String[] parts = line.substring(5).split(",", 2);
                    if (parts.length == 2) {
                        instanceCounter.put(decodeVal(parts[0]),
                            Integer.parseInt(parts[1].trim()));
                    }
                } else if (line.startsWith("BLOCK_BEGIN=")) {
                    currentBlockLines = new ArrayList<>();
                } else if (line.equals("BLOCK_END")) {
                    if (currentBlockLines != null) {
                        blockDataList.add(currentBlockLines);
                        currentBlockLines = null;
                    }
                } else if (currentBlockLines != null) {
                    currentBlockLines.add(line);
                } else if (line.startsWith("CONN=")) {
                    connLines.add(line.substring(5));
                } else if (line.startsWith("FENTRY=")) {
                    fentryLines.add(line.substring(7));
                }
            }
            // Recreate blocks
            for (List<String> bdata : blockDataList) {
                String bName = "", bOrig = "", bTmpl = "";
                int inCount = 0, outCount = 0;
                int bx = 100, by = 100;
                List<String> inTypes = new ArrayList<>();
                List<String> outTypes = new ArrayList<>();
                List<String> inVals = new ArrayList<>();
                List<String> outVars = new ArrayList<>();
                List<String> history = new ArrayList<>();
                for (String bl : bdata) {
                    String val = bl.contains("=") ? bl.substring(bl.indexOf('=') + 1) : "";
                    if (bl.startsWith("B_NAME=")) bName = decodeVal(val);
                    else if (bl.startsWith("B_ORIG=")) bOrig = decodeVal(val);
                    else if (bl.startsWith("B_TMPL=")) bTmpl = decodeVal(val);
                    else if (bl.startsWith("B_INCOUNT=")) inCount = Integer.parseInt(val);
                    else if (bl.startsWith("B_OUTCOUNT=")) outCount = Integer.parseInt(val);
                    else if (bl.startsWith("B_INTYPE=")) inTypes.add(decodeVal(val));
                    else if (bl.startsWith("B_OUTTYPE=")) outTypes.add(decodeVal(val));
                    else if (bl.startsWith("B_INVAL=")) inVals.add(decodeVal(val));
                    else if (bl.startsWith("B_OUTVAR=")) outVars.add(decodeVal(val));
                    else if (bl.startsWith("B_X=")) bx = Integer.parseInt(val);
                    else if (bl.startsWith("B_Y=")) by = Integer.parseInt(val);
                    // B_W and B_H are ignored — block size is computed from input/output count
                    else if (bl.startsWith("B_HIST=")) history.add(decodeVal(val));
                }
                String[] inTypesArr = inTypes.toArray(new String[0]);
                String[] outTypesArr = outTypes.toArray(new String[0]);
                // Ensure template in library
                if (!blockLibrary.containsKey(bTmpl)) {
                    BlockTemplate libT = new BlockTemplate(
                        bTmpl, inCount, outCount, inTypesArr.clone(), outTypesArr.clone());
                    blockLibrary.put(bTmpl, libT);
                    instanceCounter.putIfAbsent(bTmpl, 0);
                    if (!BLOCK_DESCRIPTIONS.containsKey(bTmpl)) {
                        BLOCK_DESCRIPTIONS.put(bTmpl, new String[]{
                            bTmpl.toUpperCase() + " Block",
                            "Loaded from saved pipeline.",
                            "Custom"
                        });
                    }
                }
                // Per-instance template (may differ from library)
                BlockTemplate instT = new BlockTemplate(
                    bTmpl, inCount, outCount, inTypesArr, outTypesArr);
                FunctionBlock fb = new FunctionBlock(bName, instT);
                fb.originalName = bOrig;
                fb.nameHistory = new ArrayList<>(history);
                for (int i = 0; i < inCount && i < inVals.size(); i++) {
                    fb.inputValues[i] = inVals.get(i);
                }
                for (int i = 0; i < outCount && i < outVars.size(); i++) {
                    String ov = outVars.get(i);
                    if (ov != null && !ov.isEmpty()) fb.outputVarNames[i] = ov;
                }
                // Position from file; size is already computed by recomputeSize() in constructor
                fb.setBounds(bx, by, fb.getWidth(), fb.getHeight());
                functionBlocks.add(fb);
                drawingPanel.add(fb);
            }
            // Recreate connections
            for (String cl : connLines) {
                String[] p = cl.split(",", 6);
                if (p.length >= 5) {
                    int fi = Integer.parseInt(p[0].trim());
                    int foi = Integer.parseInt(p[1].trim());
                    int ti = Integer.parseInt(p[2].trim());
                    int tii = Integer.parseInt(p[3].trim());
                    String type = decodeVal(p[4]);
                    String fn = p.length > 5 ? decodeVal(p[5]) : null;
                    if (fn != null && fn.isEmpty()) fn = null;
                    if (fi >= 0 && fi < functionBlocks.size()
                        && ti >= 0 && ti < functionBlocks.size()) {
                        Connection conn = new Connection(
                            functionBlocks.get(fi), foi,
                            functionBlocks.get(ti), tii, type);
                        conn.fileName = fn;
                        connections.add(conn);
                    }
                }
            }
            // Recreate file entry connections
            for (String fl : fentryLines) {
                String[] p = fl.split(",", 7);
                if (p.length >= 7) {
                    String eName = decodeVal(p[0]);
                    String eType = decodeVal(p[1]);
                    String eValue = decodeVal(p[2]);
                    int ePos = Integer.parseInt(p[3].trim());
                    int tbi = Integer.parseInt(p[4].trim());
                    int tii = Integer.parseInt(p[5].trim());
                    String fn = decodeVal(p[6]);
                    if (fn.isEmpty()) fn = null;
                    if (tbi >= 0 && tbi < functionBlocks.size()) {
                        FunctionBlock target = functionBlocks.get(tbi);
                        FileEntryConnection fec = new FileEntryConnection(
                            eName, eType, eValue, ePos, target, tii);
                        if (fn != null) fec.fileName = fn;
                        fileEntryConnections.add(fec);
                        target.refreshInputBadges();
                    }
                }
            }
            // Refresh UI
            refreshBlockListPanel();
            refreshBlockSelector();
            updateCanvasSize();
            loadAndDisplayInputFile();
            drawingPanel.repaint();
            tabbedPane.setSelectedIndex(0);
    }

    /** Loads a pipeline from a String — used for session restore, no dialogs shown. */
    public void loadPipelineFromString(String data) {
        if (data == null || data.trim().isEmpty()) return;
        try (BufferedReader reader = new BufferedReader(new java.io.StringReader(data))) {
            doLoadPipeline(reader);
            drawingPanel.repaint();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void loadBlockDescriptions() {
        File file = new File(DESCRIPTIONS_FILE);
        if (file.exists()) {
            try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.startsWith("BLOCK:")) {
                        String blockName = line.substring(6).trim();
                        String fullName = reader.readLine();
                        String description = reader.readLine();
                        String category = reader.readLine();
                        if (fullName != null && description != null && category != null) {
                            fullName = fullName.startsWith("FULLNAME:") ? fullName.substring(9).trim() : fullName;
                            description = description.startsWith("DESC:") ? description.substring(5).trim() : description;
                            category = category.startsWith("CATEGORY:") ? category.substring(9).trim() : category;
                            description = description.replace("\\n", "\n");
                            BLOCK_DESCRIPTIONS.put(blockName, new String[]{fullName, description, category});
                        }
                    }
                }
            } catch (Exception e) {
                System.err.println("Error loading descriptions: " + e.getMessage());
            }
        }
    }
    private void saveBlockDescriptions() {
        try (PrintWriter writer = new PrintWriter(new FileWriter(DESCRIPTIONS_FILE))) {
            for (Map.Entry<String, String[]> entry : BLOCK_DESCRIPTIONS.entrySet()) {
                String[] desc = entry.getValue();
                if (desc.length >= 3) {
                    writer.println("BLOCK:" + entry.getKey());
                    writer.println("FULLNAME:" + desc[0]);
                    writer.println("DESC:" + desc[1].replace("\n", "\\n"));
                    writer.println("CATEGORY:" + desc[2]);
                }
            }
        } catch (Exception e) {
            System.err.println("Error saving descriptions: " + e.getMessage());
        }
    }
    private void loadBlockTemplates() {
        File file = new File(TEMPLATES_FILE);
        if (file.exists()) {
            try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    if (line.startsWith("TEMPLATE:")) {
                        String name = line.substring(9).trim();
                        String inputLine = reader.readLine();
                        String outputLine = reader.readLine();
                        String inTypesLine = reader.readLine();
                        String outTypesLine = reader.readLine();
                        
                        if (inputLine != null && outputLine != null && inTypesLine != null && outTypesLine != null) {
                            try {
                                int inputCount = 0;
                                if (inputLine.startsWith("INPUTS:")) {
                                    inputCount = Integer.parseInt(inputLine.substring(7).trim());
                                }
                                
                                int outputCount = 0;
                                if (outputLine.startsWith("OUTPUTS:")) {
                                    outputCount = Integer.parseInt(outputLine.substring(8).trim());
                                }
                                
                                String[] inTypes = new String[inputCount];
                                if (inTypesLine.startsWith("INTYPES:")) {
                                    String typesStr = inTypesLine.substring(8).trim();
                                    if (!typesStr.isEmpty()) {
                                        String[] parsed = typesStr.split(",");
                                        for (int i = 0; i < inputCount && i < parsed.length; i++) {
                                            inTypes[i] = parsed[i].trim();
                                        }
                                    }
                                }
                                for (int i = 0; i < inputCount; i++) {
                                    if (inTypes[i] == null || inTypes[i].isEmpty()) {
                                        inTypes[i] = "float";
                                    }
                                }
                                
                                String[] outTypes = new String[outputCount];
                                if (outTypesLine.startsWith("OUTTYPES:")) {
                                    String typesStr = outTypesLine.substring(9).trim();
                                    if (!typesStr.isEmpty()) {
                                        String[] parsed = typesStr.split(",");
                                        for (int i = 0; i < outputCount && i < parsed.length; i++) {
                                            outTypes[i] = parsed[i].trim();
                                        }
                                    }
                                }
                                for (int i = 0; i < outputCount; i++) {
                                    if (outTypes[i] == null || outTypes[i].isEmpty()) {
                                        outTypes[i] = "float";
                                    }
                                }
                                
                                if (!blockLibrary.containsKey(name)) {
                                    BlockTemplate template = new BlockTemplate(name, inputCount, outputCount, inTypes, outTypes);
                                    blockLibrary.put(name, template);
                                    instanceCounter.put(name, 0);
                                }
                            } catch (Exception parseEx) {
                                System.err.println("Error parsing template " + name + ": " + parseEx.getMessage());
                            }
                        }
                    }
                }
            } catch (Exception e) {
                System.err.println("Error loading templates: " + e.getMessage());
            }
        }
    }
    private void saveBlockTemplates() {
        try (PrintWriter writer = new PrintWriter(new FileWriter(TEMPLATES_FILE))) {
            for (Map.Entry<String, BlockTemplate> entry : blockLibrary.entrySet()) {
                BlockTemplate t = entry.getValue();
                writer.println("TEMPLATE:" + t.name);
                writer.println("INPUTS:" + t.inputCount);
                writer.println("OUTPUTS:" + t.outputCount);
                writer.println("INTYPES:" + String.join(",", t.inputTypes));
                writer.println("OUTTYPES:" + String.join(",", t.outputTypes));
            }
        } catch (Exception e) {
            System.err.println("Error saving templates: " + e.getMessage());
        }
    }
    private JPanel createLegendPanel() {
        JPanel legendPanel = new JPanel();
        legendPanel.setLayout(new FlowLayout(FlowLayout.RIGHT, 10, 5));
        legendPanel.setBackground(Theme.SURFACE);
        legendPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(5, 10, 5, 10)
        ));
        JLabel legendLabel = new JLabel("Types: ");
        legendLabel.setFont(new Font("SansSerif", Font.BOLD, 11));
        legendLabel.setForeground(Theme.TEXT_MED);
        legendPanel.add(legendLabel);
        Object[][] typeColors = {
            {"float",   Theme.T_FLOAT},
            {"integer", Theme.T_INT},
            {"string",  Theme.T_STRING},
            {"file",    Theme.T_FILE},
            {"graph",   Theme.T_GRAPH},
            {"status",  Theme.SUCCESS},
            {"char",    Theme.WARNING}
        };
        for (Object[] typeColor : typeColors) {
            String typeName = (String) typeColor[0];
            Color color = (Color) typeColor[1];
            
            JPanel colorBox = new JPanel() {
                @Override
                protected void paintComponent(Graphics g) {
                    super.paintComponent(g);
                    Graphics2D g2 = (Graphics2D) g;
                    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                    g2.setColor(color);
                    g2.fillRoundRect(0, 0, getWidth(), getHeight(), 4, 4);
                    g2.setColor(color.darker());
                    g2.drawRoundRect(0, 0, getWidth() - 1, getHeight() - 1, 4, 4);
                }
            };
            colorBox.setPreferredSize(new Dimension(12, 12));
            colorBox.setOpaque(false);

            JLabel label = new JLabel(typeName);
            label.setFont(new Font("SansSerif", Font.PLAIN, 11));
            label.setForeground(color);
            
            legendPanel.add(colorBox);
            legendPanel.add(label);
            legendPanel.add(Box.createHorizontalStrut(5));
        }
        return legendPanel;
    }
    private void initializeGUI() {
        setTitle("FuSION — Pipeline Editor");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        // Start maximized; user can toggle fullscreen with F11
        setExtendedState(JFrame.MAXIMIZED_BOTH);
        // Fit to screen size so it never overflows
        Dimension screen = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
        setSize(screen.width, screen.height);
        JPanel topPanel = new JPanel();
        topPanel.setLayout((LayoutManager) new BoxLayout(topPanel, BoxLayout.Y_AXIS));
        topPanel.setBackground(Theme.TOOLBAR_BG);
        JPanel controlsPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 4));
        controlsPanel.setBackground(Theme.TOOLBAR_BG);
        controlsPanel.setOpaque(true);
        hamburgerButton = new JButton("\u2630");
        hamburgerButton.setFont(new Font("SansSerif", Font.BOLD, 16));
        hamburgerButton.setToolTipText("Menu Options");
        hamburgerButton.setMargin(new Insets(2, 8, 2, 8));
        hamburgerButton.setBackground(Theme.SURFACE);
        hamburgerButton.setForeground(Theme.TEXT_DARK);
        hamburgerButton.setOpaque(true);
        hamburgerButton.setBorderPainted(false);
        hamburgerButton.setFocusPainted(false);
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
            
            menu.addSeparator();
            
            JMenuItem savePipelineItem = new JMenuItem("\uD83D\uDCBE Save Pipeline (Ctrl+S)");
            savePipelineItem.addActionListener(ev -> savePipeline());
            menu.add(savePipelineItem);
            
            JMenuItem loadPipelineItem = new JMenuItem("\uD83D\uDCC2 Load Pipeline (Ctrl+O)");
            loadPipelineItem.addActionListener(ev -> loadPipeline());
            menu.add(loadPipelineItem);

            JMenuItem templatePipelineItem = new JMenuItem("\u2728 Load Template Pipeline");
            templatePipelineItem.addActionListener(ev -> loadTemplatePipeline());
            menu.add(templatePipelineItem);

            menu.show(hamburgerButton, 0, hamburgerButton.getHeight());
        });
        controlsPanel.add(hamburgerButton);
        blockSelector = new JComboBox<String>();
        blockSelector.setRenderer(new BlockSelectorRenderer());
        blockSelector.setPreferredSize(new Dimension(120, 24));
        blockSelector.setMaximumSize(new Dimension(120, 24));
        JLabel blockLbl = new JLabel("Block:");
        blockLbl.setForeground(Theme.TEXT_MED);
        controlsPanel.add(blockLbl);
        controlsPanel.add(blockSelector);
        JButton addBlockBtn = new JButton("Add");
        addBlockBtn.setBackground(Theme.SURFACE);
        addBlockBtn.setForeground(Theme.TEXT_DARK);
        addBlockBtn.setOpaque(true);
        addBlockBtn.setBorderPainted(false);
        addBlockBtn.setFocusPainted(false);
        addBlockBtn.setToolTipText("Add selected block to canvas");
        addBlockBtn.addActionListener(e -> addSelectedBlockInstance());
        controlsPanel.add(addBlockBtn);
        JButton addTemplateBtn = new JButton("+Tmpl");
        addTemplateBtn.setBackground(new Color(80, 50, 180));
        addTemplateBtn.setForeground(Color.WHITE);
        addTemplateBtn.setOpaque(true);
        addTemplateBtn.setBorderPainted(false);
        addTemplateBtn.setFocusPainted(false);
        addTemplateBtn.setToolTipText("New Block Template");
        addTemplateBtn.addActionListener(e -> showNewBlockTemplateDialog());
        controlsPanel.add(addTemplateBtn);
        JTextField searchField = new JTextField(8);
        searchField.setBackground(Theme.SURFACE);
        searchField.setForeground(Theme.TEXT_DARK);
        searchField.setCaretColor(Theme.TEXT_DARK);
        searchField.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(2, 4, 2, 4)));
        JButton searchButton = new JButton("Search");
        searchButton.setBackground(Theme.SURFACE);
        searchButton.setForeground(Theme.TEXT_DARK);
        searchButton.setOpaque(true);
        searchButton.setBorderPainted(false);
        searchButton.setFocusPainted(false);
        searchButton.addActionListener(e -> searchFunctionBlock(searchField.getText().toLowerCase()));
        searchField.addKeyListener(new KeyAdapter() {
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    searchFunctionBlock(searchField.getText().toLowerCase());
                }
            }
        });
        JLabel searchLbl = new JLabel("Search:");
        searchLbl.setForeground(Theme.TEXT_MED);
        controlsPanel.add(searchLbl);
        controlsPanel.add(searchField);
        controlsPanel.add(searchButton);
        JButton zoomInButton = new JButton("\ud83d\uDD0D+");
        zoomInButton.setBackground(Theme.SURFACE);
        zoomInButton.setForeground(Theme.TEXT_DARK);
        zoomInButton.setOpaque(true);
        zoomInButton.setBorderPainted(false);
        zoomInButton.setFocusPainted(false);
        zoomInButton.addActionListener(e -> drawingPanel.zoom(1.2));
        JButton zoomOutButton = new JButton("\ud83d\uDD0D\u2212");
        zoomOutButton.setBackground(Theme.SURFACE);
        zoomOutButton.setForeground(Theme.TEXT_DARK);
        zoomOutButton.setOpaque(true);
        zoomOutButton.setBorderPainted(false);
        zoomOutButton.setFocusPainted(false);
        zoomOutButton.addActionListener(e -> drawingPanel.zoom(0.8));
        controlsPanel.add(zoomInButton);
        controlsPanel.add(zoomOutButton);
        JButton executeBtn = new JButton("\u25B6 Execute");
        executeBtn.setBackground(Theme.SUCCESS_DK);
        executeBtn.setForeground(Color.WHITE);
        executeBtn.setOpaque(true);
        executeBtn.setBorderPainted(false);
        executeBtn.setFocusPainted(false);
        executeBtn.addActionListener(e -> executeGraph());
        controlsPanel.add(executeBtn);

        JButton genScriptBtn = new JButton("\ud83d\udcc4 Gen Script");
        genScriptBtn.setBackground(new Color(80, 50, 180));
        genScriptBtn.setForeground(Color.WHITE);
        genScriptBtn.setOpaque(true);
        genScriptBtn.setBorderPainted(false);
        genScriptBtn.setFocusPainted(false);
        genScriptBtn.setToolTipText("Generate advtempscript from current pipeline");
        genScriptBtn.addActionListener(e -> generateScriptFromPipeline());
        controlsPanel.add(genScriptBtn);

        JButton saveBtn = new JButton("\ud83d\uDCBE Save");
        saveBtn.setBackground(Theme.PRIMARY_DK);
        saveBtn.setForeground(Color.WHITE);
        saveBtn.setOpaque(true);
        saveBtn.setBorderPainted(false);
        saveBtn.setFocusPainted(false);
        saveBtn.setToolTipText("Save Pipeline (Ctrl+S)");
        saveBtn.addActionListener(e -> savePipeline());
        controlsPanel.add(saveBtn);

        JButton loadBtn = new JButton("\ud83d\uDCC2 Load");
        loadBtn.setBackground(Theme.WARNING_DK);
        loadBtn.setForeground(Color.WHITE);
        loadBtn.setOpaque(true);
        loadBtn.setBorderPainted(false);
        loadBtn.setFocusPainted(false);
        loadBtn.setToolTipText("Load Pipeline (Ctrl+O)");
        loadBtn.addActionListener(e -> loadPipeline());
        controlsPanel.add(loadBtn);

        JButton newPageBtn = new JButton("\ud83d\udcc4 New");
        newPageBtn.setBackground(Theme.DANGER);
        newPageBtn.setForeground(Color.WHITE);
        newPageBtn.setOpaque(true);
        newPageBtn.setBorderPainted(false);
        newPageBtn.setFocusPainted(false);
        newPageBtn.setToolTipText("Clear everything and start a fresh pipeline");
        newPageBtn.addActionListener(e -> newPipeline());
        controlsPanel.add(newPageBtn);

        JButton templateBtn = new JButton("\u2728 Template");
        templateBtn.setBackground(new Color(120, 40, 160));
        templateBtn.setForeground(Color.WHITE);
        templateBtn.setOpaque(true);
        templateBtn.setBorderPainted(false);
        templateBtn.setFocusPainted(false);
        templateBtn.setToolTipText("Create template pipeline with all 7 blocks pre-connected");
        templateBtn.addActionListener(e -> loadTemplatePipeline());
        controlsPanel.add(templateBtn);

        controlsPanel.setBorder(BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER));
        topPanel.add(controlsPanel);
        
        blockListPanel = new JPanel();
        blockListPanel.setLayout(new FlowLayout(FlowLayout.LEFT, 5, 5));
        blockListPanel.setBackground(Theme.TOOLBAR_BG);
        JScrollPane blockScrollPane = new JScrollPane(blockListPanel);
        blockScrollPane.setPreferredSize(new Dimension(1280, 76));
        blockScrollPane.setMaximumSize(new Dimension(Integer.MAX_VALUE, 76));
        blockScrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        blockScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_NEVER);
        blockScrollPane.setBackground(Theme.TOOLBAR_BG);
        blockScrollPane.getViewport().setBackground(Theme.TOOLBAR_BG);
        blockScrollPane.setBorder(BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER));
        topPanel.add(blockScrollPane);
        populateBlockLibrary();
        add(topPanel, BorderLayout.NORTH);
        tabbedPane = new JTabbedPane();
        
        JPanel editorPanel = new JPanel(new BorderLayout());
        
        JPanel legendContainer = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, 0));
        legendContainer.setBackground(Theme.SURFACE);
        legendContainer.add(createLegendPanel());
        editorPanel.add(legendContainer, BorderLayout.NORTH);
        
        // === FILE PANEL (left side, resizable via JSplitPane) ===
        JPanel filePanel = createFilePanel();

        JScrollPane canvasScroll = new JScrollPane(drawingPanel);
        canvasScroll.getVerticalScrollBar().setUnitIncrement(16);

        JSplitPane centerSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, filePanel, canvasScroll);
        centerSplit.setDividerLocation(310);
        centerSplit.setDividerSize(5);
        centerSplit.setResizeWeight(0.0);   // canvas gets all extra space on window resize
        centerSplit.setContinuousLayout(true);
        centerSplit.setBorder(null);
        editorPanel.add(centerSplit, BorderLayout.CENTER);
        // === END FILE PANEL ===
        
        tabbedPane.addTab("Graph Editor", editorPanel);
        
        resultArea = new JTextArea();
        resultArea.setEditable(false);
        resultArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        JScrollPane resultScroll = new JScrollPane(resultArea);
        tabbedPane.addTab("Results", resultScroll);
        JPanel historyPanel = createHistoryPanel();
        tabbedPane.addTab("History", historyPanel);
        
        add(tabbedPane, BorderLayout.CENTER);
        setupKeyboardShortcuts();
        if (!embeddedMode) setVisible(true);
        // Auto-load template pipeline on every fresh session (no saved pipeline yet)
        SwingUtilities.invokeLater(() -> applyTemplatePipeline(false));
    }
    // =====================================================
    // FILE PANEL METHODS  (reads Data_Coming_From_Frontend.txt)
    // Format: name,type,value  (comma-separated, 3 columns)
    // =====================================================
    /** Returns the accent color for a given type string (mirrors graph editor colours). */
    private Color getFilePanelTypeColor(String type) {
        if (type == null) return Theme.T_OTHER;
        switch (type.trim().toLowerCase()) {
            case "float":                   return Theme.T_FLOAT;
            case "integer": case "int":     return Theme.T_INT;
            case "string":                  return Theme.T_STRING;
            case "file":                    return Theme.T_FILE;
            case "graph":                   return Theme.T_GRAPH;
            case "status":                  return Theme.SUCCESS;
            case "boolean":                 return Theme.SUCCESS_DK;
            case "char": case "character":  return Theme.WARNING;
            default:                        return Theme.T_OTHER;
        }
    }
    // ---- Group definitions for the input panel sidebar ----
    private static final String[][] GROUP_DEFS = {
        { "\uD83E\uDDEC", "Genes",      "Target genes and the node under study" },
        { "\uD83D\uDCCA", "Expression", "Fold-change data and regulation thresholds" },
        { "\uD83C\uDF10", "Pathway",    "KEGG XML files and additional edges" },
        { "\uD83D\uDD0D", "Path Search","Reachability bound and edge direction rules" },
        { "\u2696",       "Relaxation", "Node / edge relaxation bounds and override files" },
        { "\uD83D\uDDC2", "Ref Maps",   "ID-mapping and pathway-name reference files" },
        { "\u2699",       "Advanced",   "Solver timeouts, solution counts, split thresholds" },
    };
    // Returns 0-6 group index for a canonical param name, or -1 if unknown.
    private int getEntryGroup(String name) {
        if (name == null) return -1;
        switch (name.toUpperCase()) {
            // Group 0 \u2014 Genes
            case "SRC_NODE": case "TGT_NODE": case "NODE_TO_TEST_FOR_SIGNIFICANCE":
                return 0;
            // Group 1 \u2014 Expression
            case "LOG_FOLD_CHANGE_FILE": case "LOGFOLDCHANGES":
            case "UP_REG_THRESH": case "DOWN_REG_THRESH":
            case "COEXPRESSION_CSV": case "COEXP_THRESH": case "FROZEN_THRESH":
                return 1;
            // Group 2 \u2014 Pathway
            case "LIST_OF_MERGED_KEGG_XML_FILE": case "MERGED_KEGG_PATHWAYS_XML_FILE":
            case "ADDITIONAL_EDGES_FILE": case "ADDITIONALEDGES":
            case "HSA_IDS_NOT_TO_BE_MERGED_FILE":
                return 2;
            // Group 3 \u2014 Path Search
            case "REACH_PATH_BOUND": case "EDGES_TO_TARGET":
            case "ESSENTIAL_EDGES_FILE": case "AVOID_EDGES_FILE":
                return 3;
            // Group 4 \u2014 Relaxation
            case "EDGE_RELAX_LB": case "EDGE_RELAX_UB":
            case "NODE_RELAX_LB": case "NODE_RELAX_UB":
            case "RELAXED_EDGES_FILE": case "NONRELAXED_EDGES_FILE":
            case "RELAXED_NODES_FILE": case "NONRELAXED_NODES_FILE":
            case "INACTIVE_NODES_FILE":
            case "CONFIRMED_UP_REG_FILE": case "CONFIRMED_DOWN_REG_FILE":
                return 4;
            // Group 5 \u2014 Ref Maps
            case "HSA_TO_GENE_SYMBOL_MAP_FILE": case "HSA_PATH_TO_PATH_NAME_MAP_FILE":
            case "INTER_DB_MAP_FILE": case "WORK_DIR":
                return 5;
            // Group 6 \u2014 Advanced
            case "NODE_SPLIT_THRESHOLD":
            case "CONSTR_SOLVER_TIMEOUT1": case "CONSTR_SOLVER_TIMEOUT2":
            case "NUM_SOLNS_TO_COUNT": case "NUM_SOLNS_TO_EXPLORE":
                return 6;
            default:
                return -1;
        }
    }

    private JPanel createFilePanel() {
        JPanel panel = new JPanel(new BorderLayout(0, 0));
        panel.setBackground(Theme.BG);
        panel.setBorder(BorderFactory.createMatteBorder(0, 0, 0, 1, Theme.BORDER));
        panel.setMinimumSize(new Dimension(130, 0));
        panel.setPreferredSize(new Dimension(310, 0));

        // ---- Header ----
        JPanel header = new JPanel(new BorderLayout(4, 4));
        header.setBackground(Theme.SURFACE);
        header.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(8, 10, 8, 6)));

        JLabel title = new JLabel("\uD83D\uDCC2  Input Data");
        title.setFont(new Font("SansSerif", Font.BOLD, 13));
        title.setForeground(Color.WHITE);
        header.add(title, BorderLayout.WEST);

        JPanel headerBtns = new JPanel(new FlowLayout(FlowLayout.RIGHT, 3, 0));
        headerBtns.setOpaque(false);

        JButton reloadBtn = new JButton("\u21BB");
        reloadBtn.setFont(new Font("SansSerif", Font.BOLD, 14));
        reloadBtn.setToolTipText("Reload from file");
        reloadBtn.setMargin(new Insets(1, 6, 1, 6));
        reloadBtn.setFocusPainted(false);
        reloadBtn.setBackground(Theme.BORDER_HI);
        reloadBtn.setForeground(Theme.TEXT_DARK);
        reloadBtn.setBorderPainted(false);
        reloadBtn.setOpaque(true);
        reloadBtn.addActionListener(e -> loadAndDisplayInputFile());
        headerBtns.add(reloadBtn);

        JButton browseBtn = new JButton("\uD83D\uDCC1");
        browseBtn.setFont(new Font("SansSerif", Font.PLAIN, 13));
        browseBtn.setToolTipText("Browse for a different file");
        browseBtn.setMargin(new Insets(1, 6, 1, 6));
        browseBtn.setFocusPainted(false);
        browseBtn.setBackground(Theme.BORDER_HI);
        browseBtn.setForeground(Theme.TEXT_DARK);
        browseBtn.setBorderPainted(false);
        browseBtn.setOpaque(true);
        browseBtn.addActionListener(e -> browseInputFile());
        headerBtns.add(browseBtn);

        JButton genDataBtn = new JButton("\u26A1");
        genDataBtn.setFont(new Font("SansSerif", Font.BOLD, 13));
        genDataBtn.setToolTipText("Generate Data_Coming_From_Frontend file from current session");
        genDataBtn.setMargin(new Insets(1, 6, 1, 6));
        genDataBtn.setFocusPainted(false);
        genDataBtn.setBackground(Theme.SUCCESS_DK);
        genDataBtn.setForeground(Color.WHITE);
        genDataBtn.setBorderPainted(false);
        genDataBtn.setOpaque(true);
        genDataBtn.addActionListener(e -> generateDataComingFromFileInteractive());
        headerBtns.add(genDataBtn);

        header.add(headerBtns, BorderLayout.EAST);
        panel.add(header, BorderLayout.NORTH);

        // ---- Source label (bottom) ----
        filePanelSourceLabel = new JLabel(" " + DEFAULT_INPUT_FILE);
        filePanelSourceLabel.setFont(Theme.mono(9));
        filePanelSourceLabel.setForeground(Theme.TEXT_LIGHT);
        filePanelSourceLabel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(1, 0, 0, 0, Theme.BORDER),
            BorderFactory.createEmptyBorder(3, 8, 3, 4)
        ));
        filePanelSourceLabel.setBackground(Theme.SURFACE);
        filePanelSourceLabel.setOpaque(true);
        filePanelSourceLabel.setToolTipText(currentInputFilePath);
        panel.add(filePanelSourceLabel, BorderLayout.SOUTH);

        // ---- Entries container (scrollable) ----
        fileEntriesPanel = new JPanel();
        fileEntriesPanel.setLayout(new BoxLayout(fileEntriesPanel, BoxLayout.Y_AXIS));
        fileEntriesPanel.setBackground(Theme.BG);
        fileEntriesPanel.setBorder(BorderFactory.createEmptyBorder(6, 0, 8, 0));

        JScrollPane scroll = new JScrollPane(fileEntriesPanel);
        scroll.setBorder(null);
        scroll.getVerticalScrollBar().setUnitIncrement(14);
        scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        scroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);

        // ---- Sidebar: group filter buttons ----
        JPanel sidebar = new JPanel();
        sidebar.setLayout(new BoxLayout(sidebar, BoxLayout.Y_AXIS));
        sidebar.setBackground(Theme.TOOLBAR_BG);
        sidebar.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 0, 0, 1, Theme.BORDER),
            BorderFactory.createEmptyBorder(6, 3, 6, 3)));
        sidebar.setPreferredSize(new Dimension(82, 0));
        sidebar.setMinimumSize(new Dimension(82, 0));
        sidebar.setMaximumSize(new Dimension(82, Integer.MAX_VALUE));

        // "All" button + 7 group buttons
        int totalButtons = 1 + GROUP_DEFS.length;
        groupSidebarButtons = new JButton[totalButtons];

        Color sidebarBtnNormal   = Theme.SURFACE;
        Color sidebarBtnSelected = Theme.PRIMARY;
        Color sidebarBtnHover    = Theme.BORDER_HI;
        Color sidebarText        = Theme.TEXT_DARK;

        Runnable[] selectActions = new Runnable[totalButtons];

        for (int bi = 0; bi < totalButtons; bi++) {
            final int groupIdx = bi - 1; // -1 = All, 0..6 = groups
            String icon, label, tip;
            if (bi == 0) {
                icon = "\uD83D\uDDC3"; label = "All"; tip = "Show all inputs";
            } else {
                icon  = GROUP_DEFS[bi - 1][0];
                label = GROUP_DEFS[bi - 1][1];
                tip   = GROUP_DEFS[bi - 1][2];
            }
            JButton btn = new JButton("<html><center>" + icon + "<br>"
                    + "<span style='font-size:9px'>" + label + "</span></center></html>");
            btn.setFont(new Font("SansSerif", Font.PLAIN, 12));
            btn.setForeground(sidebarText);
            btn.setBackground(bi == 0 ? sidebarBtnSelected : sidebarBtnNormal);
            btn.setOpaque(true);
            btn.setBorderPainted(false);
            btn.setFocusPainted(false);
            btn.setToolTipText(tip);
            btn.setMaximumSize(new Dimension(84, 56));
            btn.setPreferredSize(new Dimension(84, 56));
            btn.setAlignmentX(Component.CENTER_ALIGNMENT);
            btn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));

            groupSidebarButtons[bi] = btn;

            final int biFinal = bi;
            selectActions[bi] = () -> {
                selectedGroupIndex = groupIdx;
                for (int k = 0; k < groupSidebarButtons.length; k++)
                    groupSidebarButtons[k].setBackground(k == biFinal ? sidebarBtnSelected : sidebarBtnNormal);
                loadAndDisplayInputFile();
            };

            btn.addActionListener(e -> selectActions[biFinal].run());
            btn.addMouseListener(new java.awt.event.MouseAdapter() {
                public void mouseEntered(java.awt.event.MouseEvent e) {
                    if (btn.getBackground() != sidebarBtnSelected)
                        btn.setBackground(sidebarBtnHover);
                }
                public void mouseExited(java.awt.event.MouseEvent e) {
                    if (btn.getBackground() != sidebarBtnSelected)
                        btn.setBackground(sidebarBtnNormal);
                }
            });

            sidebar.add(btn);
            if (bi < totalButtons - 1)
                sidebar.add(Box.createRigidArea(new Dimension(0, 4)));
        }

        // ---- Center: sidebar + scroll ----
        JPanel centerSplit = new JPanel(new BorderLayout(0, 0));
        centerSplit.add(sidebar, BorderLayout.WEST);
        centerSplit.add(scroll,  BorderLayout.CENTER);
        panel.add(centerSplit, BorderLayout.CENTER);

        loadAndDisplayInputFile();
        return panel;
    }
    private void browseInputFile() {
        JFileChooser chooser = new JFileChooser(new File(currentInputFilePath).getParent());
        chooser.setDialogTitle("Select Input Data File");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter("Text files (*.txt)", "txt"));
        chooser.setAcceptAllFileFilterUsed(true);
        if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            currentInputFilePath = chooser.getSelectedFile().getAbsolutePath();
            filePanelSourceLabel.setToolTipText(currentInputFilePath);
            loadAndDisplayInputFile();
        }
    }

    /**
     * Maps alternate/legacy names (UPPERCASE) → canonical param name (UPPERCASE).
     * Allows values from the original Data_Coming_From_Frontend.txt (which uses
     * short alias names like "log_fold_changes") to resolve into the canonical
     * names used by the generated data file and the pipeline FENTRYs.
     */
    private static java.util.Map<String, String> getParamAliases() {
        java.util.Map<String, String> m = new java.util.HashMap<>();
        m.put("LOG_FOLD_CHANGES",               "LOG_FOLD_CHANGE_FILE");
        m.put("LOG_FOLD_CHANGES_FILE",          "LOG_FOLD_CHANGE_FILE");
        m.put("UP_THRESHOLD",                   "UP_REG_THRESH");
        m.put("UP_REGULATORY_THRESHOLD",        "UP_REG_THRESH");
        m.put("DOWN_THRESHOLD",                 "DOWN_REG_THRESH");
        m.put("DOWN_REGULATORY_THRESHOLD",      "DOWN_REG_THRESH");
        m.put("CROSS_DB_MAP",                   "INTER_DB_MAP_FILE");
        m.put("CROSS_DATABASE_MAP",             "INTER_DB_MAP_FILE");
        m.put("HSA_TO_GENE_MAP",                "HSA_TO_GENE_SYMBOL_MAP_FILE");
        m.put("HSA_TO_GENE_SYMBOL_MAP",         "HSA_TO_GENE_SYMBOL_MAP_FILE");
        m.put("HSA_PATH_MAP",                   "HSA_PATH_TO_PATH_NAME_MAP_FILE");
        m.put("HSA_PATH_TO_PATH_NAME_MAP",      "HSA_PATH_TO_PATH_NAME_MAP_FILE");
        m.put("SIGNALLING_PATH_LENGTH",         "REACH_PATH_BOUND");
        m.put("NODE_RELAX_LOWER",               "NODE_RELAX_LB");
        m.put("NODE_RELAXATION_LOWER_BOUND",    "NODE_RELAX_LB");
        m.put("NODE_RELAX_UPPER",               "NODE_RELAX_UB");
        m.put("NODE_RELAXATION_UPPER_BOUND",    "NODE_RELAX_UB");
        m.put("EDGE_RELAX_LOWER",               "EDGE_RELAX_LB");
        m.put("EDGE_RELAXATION_LOWER_BOUND",    "EDGE_RELAX_LB");
        m.put("EDGE_RELAX_UPPER",               "EDGE_RELAX_UB");
        m.put("EDGE_RELAXATION_UPPER_BOUND",    "EDGE_RELAX_UB");
        m.put("INC_SOLVER_TIMEOUT",             "CONSTR_SOLVER_TIMEOUT1");
        m.put("INCREMENT_SOLVER_TIMEOUT",       "CONSTR_SOLVER_TIMEOUT1");
        m.put("OVER_SOLVER_TIMEOUT",            "CONSTR_SOLVER_TIMEOUT2");
        m.put("OVERALL_SOLVER_TIMEOUT",         "CONSTR_SOLVER_TIMEOUT2");
        m.put("SOLUTION_COUNT",                 "NUM_SOLNS_TO_COUNT");
        m.put("SOLUTIONS_TO_COUNT",             "NUM_SOLNS_TO_COUNT");
        m.put("SOLUTION_EXPLORE",               "NUM_SOLNS_TO_EXPLORE");
        m.put("SOLUTIONS_TO_EXPLORE",           "NUM_SOLNS_TO_EXPLORE");
        m.put("ESSENTIAL_EDGES",                "ESSENTIAL_EDGES_FILE");
        m.put("ESSENTIAL_EDGE_FILE",            "ESSENTIAL_EDGES_FILE");
        m.put("AVOID_EDGES",                    "AVOID_EDGES_FILE");
        m.put("AVOID_EDGE_FILE",                "AVOID_EDGES_FILE");
        m.put("INACTIVE_NODES",                 "INACTIVE_NODES_FILE");
        m.put("INACTIVE_NODE_FILE",             "INACTIVE_NODES_FILE");
        m.put("CONFIRMED_UP_REG",               "CONFIRMED_UP_REG_FILE");
        m.put("CONFIRMED_DOWN_REG",             "CONFIRMED_DOWN_REG_FILE");
        m.put("RELAXED_NODES",                  "RELAXED_NODES_FILE");
        m.put("RELAXED_EDGES",                  "RELAXED_EDGES_FILE");
        m.put("NON_RELAXED_EDGES",              "NONRELAXED_EDGES_FILE");
        m.put("NON_RELAXED_EDGES_FILE",         "NONRELAXED_EDGES_FILE");
        m.put("WORKING_DIRECTORY",              "WORK_DIR");
        m.put("WORK_DIRECTORY",                 "WORK_DIR");
        m.put("KEGG_XML",                       "MERGED_KEGG_PATHWAYS_XML_FILE");
        m.put("MERGED_KEGG_XML_OUTPUT",         "MERGED_KEGG_PATHWAYS_XML_FILE");
        m.put("MICROARRAY_DATA",                "LIST_OF_MERGED_KEGG_XML_FILE");
        m.put("KEGG_XML_INPUT_LIST",            "LIST_OF_MERGED_KEGG_XML_FILE");
        m.put("HSA_IDS_NOT_TO_BE_MERGED",       "HSA_IDS_NOT_TO_BE_MERGED_FILE");
        m.put("EXCEPTION_TO_NODE_MERGE",        "EXCEPTION_TO_NODE_MERGE_FILE");
        m.put("ADDITIONAL_EDGES",               "ADDITIONAL_EDGES_FILE");
        m.put("COEXPRESSION",                   "COEXPRESSION_CSV");
        m.put("COEXP_CSV",                      "COEXPRESSION_CSV");
        return m;
    }

    /** Looks up a canonical param name in existingMap, falling back to aliases. */
    private static String[] lookupInExisting(String canonicalKey,
            java.util.Map<String, String[]> existingMap,
            java.util.Map<String, String> aliases) {
        String[] ex = existingMap.get(canonicalKey);
        if (ex != null && !ex[1].isEmpty()) return ex;
        // try each alias whose canonical matches
        for (java.util.Map.Entry<String, String> ae : aliases.entrySet()) {
            if (ae.getValue().equals(canonicalKey)) {
                String[] alt = existingMap.get(ae.getKey());
                if (alt != null && !alt[1].isEmpty()) return alt;
            }
        }
        return null;
    }

    /**
     * Generates a Data_Coming_From_Frontend file for the current session.
     * Contains all let-variable inputs defined before the cudf block in advtempscript.
     * Uses values already loaded in the file panel; fills defaults for missing ones.
     * Saves to: {pipeline_folder}/data_coming_from/{sessionName}_{yyyyMMdd_HHmmss}.txt
     */
    private void generateDataComingFromFile() {
        // --- Read currently loaded values (name -> value, case-insensitive lookup) ---
        java.util.Map<String, String[]> existing = new java.util.LinkedHashMap<>(); // key=uppercase name, val=[type,value]
        File curFile = new File(currentInputFilePath);
        if (curFile.exists()) {
            try (java.io.BufferedReader br = new java.io.BufferedReader(new java.io.FileReader(curFile))) {
                String line;
                while ((line = br.readLine()) != null) {
                    line = line.trim();
                    if (line.isEmpty() || line.startsWith("#")) continue;
                    String[] parts = line.split(",", 3);
                    if (parts.length >= 3) {
                        existing.put(parts[0].trim().toUpperCase(), new String[]{parts[1].trim(), parts[2].trim()});
                    } else if (parts.length == 2) {
                        existing.put(parts[0].trim().toUpperCase(), new String[]{parts[1].trim(), ""});
                    }
                }
            } catch (Exception ignored) {}
        }

        // --- Frontend-provided inputs only (no constants, no derived paths) ---
        // Format: { canonicalName, type, defaultValue }
        String[][] params = {
            // Files — default empty_file when user provides nothing
            {"LOG_FOLD_CHANGE_FILE",            "file",    "empty_file"},
            {"LIST_OF_MERGED_KEGG_XML_FILE",    "file",    "empty_file"},
            {"MERGED_KEGG_PATHWAYS_XML_FILE",   "file",    "empty_file"},
            {"ADDITIONAL_EDGES_FILE",           "file",    "empty_file"},
            {"ESSENTIAL_EDGES_FILE",            "file",    "empty_file"},
            {"AVOID_EDGES_FILE",                "file",    "empty_file"},
            {"INACTIVE_NODES_FILE",             "file",    "empty_file"},
            {"CONFIRMED_UP_REG_FILE",           "file",    "empty_file"},
            {"CONFIRMED_DOWN_REG_FILE",         "file",    "empty_file"},
            {"RELAXED_NODES_FILE",              "file",    "empty_file"},
            {"RELAXED_EDGES_FILE",              "file",    "empty_file"},
            {"NONRELAXED_EDGES_FILE",           "file",    "empty_file"},
            {"COEXPRESSION_CSV",                "file",    "empty_file"},
            {"HSA_TO_GENE_SYMBOL_MAP_FILE",     "file",    "empty_file"},
            {"HSA_PATH_TO_PATH_NAME_MAP_FILE",  "file",    "empty_file"},
            {"INTER_DB_MAP_FILE",               "file",    "empty_file"},
            {"HSA_IDS_NOT_TO_BE_MERGED_FILE",   "file",    "empty_file"},
            // Node IDs / working directory
            {"WORK_DIR",                        "string",  "/example/workdir"},
            {"SRC_NODE",                        "string",  "hsa0"},
            {"TGT_NODE",                        "string",  "hsa0"},
            {"NODE_TO_TEST_FOR_SIGNIFICANCE",   "string",  "hsa0"},
            // Numeric parameters
            {"REACH_PATH_BOUND",                "integer", "5"},
            {"EDGE_RELAX_LB",                   "integer", "0"},
            {"EDGE_RELAX_UB",                   "integer", "5"},
            {"NODE_RELAX_LB",                   "integer", "0"},
            {"NODE_RELAX_UB",                   "integer", "5"},
            {"NODE_SPLIT_THRESHOLD",            "integer", "50"},
            {"CONSTR_SOLVER_TIMEOUT1",          "integer", "60"},
            {"CONSTR_SOLVER_TIMEOUT2",          "integer", "300"},
            {"NUM_SOLNS_TO_COUNT",              "integer", "3"},
            {"NUM_SOLNS_TO_EXPLORE",            "integer", "10"},
            {"UP_REG_THRESH",                   "float",   "1.5"},
            {"DOWN_REG_THRESH",                 "float",   "-1.5"},
            {"COEXP_THRESH",                    "float",   "0.5"},
            {"FROZEN_THRESH",                   "float",   "0.5"},
            {"EDGES_TO_TARGET",                 "integer", "0"},
        };

        // --- Resolve values (existing wins over default; fall back to aliases) ---
        java.util.Map<String, String> resolved = new java.util.LinkedHashMap<>();
        java.util.Map<String, String> aliases = getParamAliases();
        for (String[] p : params) {
            String key = p[0].toUpperCase();
            String[] ex = lookupInExisting(key, existing, aliases);
            resolved.put(p[0], (ex != null && !ex[1].isEmpty()) ? ex[1] : p[2]);
        }

        // --- Determine output folder: {pipeline_dir}/data_coming_from/ ---
        File pipelineDir;
        if (lastSavedPipelineFile != null && lastSavedPipelineFile.getParentFile() != null) {
            pipelineDir = lastSavedPipelineFile.getParentFile();
        } else {
            pipelineDir = new File(System.getProperty("user.dir"));
        }
        File dataDir = new File(pipelineDir, "data_coming_from");
        dataDir.mkdirs();

        // --- Session name + timestamp ---
        String sessionName = (lastSavedPipelineFile != null)
                ? lastSavedPipelineFile.getName().replaceFirst("\\.[^.]+$", "")
                : "session";
        String timestamp = new java.text.SimpleDateFormat("yyyyMMdd_HHmmss").format(new java.util.Date());
        File outFile = new File(dataDir, sessionName + "_" + timestamp + ".txt");

        // --- Write the file (frontend-provided entries only, no derived paths) ---
        try (PrintWriter w = new PrintWriter(outFile, "UTF-8")) {
            w.println("# Data_Coming_From_Frontend — frontend-provided inputs only");
            w.println("# Session: " + sessionName + "  Generated: " + new java.util.Date());
            w.println("# Format: name,type,value");
            for (String[] p : params) {
                String val = resolved.get(p[0]);
                if (val == null) val = p[2];
                // Only one of the two KEGG XML inputs is selected in Card 3.
                // Skip the one that has no real value so it never appears in the file panel.
                if (("LIST_OF_MERGED_KEGG_XML_FILE".equals(p[0]) || "MERGED_KEGG_PATHWAYS_XML_FILE".equals(p[0]))
                        && "empty_file".equals(val)) continue;
                w.println(p[0] + "," + p[1] + "," + val);
            }
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(getDialogParent(),
                "Error writing data file:\n" + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        // --- Load the generated file into the file panel ---
        currentInputFilePath = outFile.getAbsolutePath();
        filePanelSourceLabel.setToolTipText(currentInputFilePath);
        loadAndDisplayInputFile();
    }

    /**
     * Called by Gui4Panel (embedded mode) to auto-generate the full parameter list
     * from the frontend data file and populate the file panel silently.
     */
    public void autoGenerateFullDataFile() {
        SwingUtilities.invokeLater(this::generateDataComingFromFile);
    }

    private void generateDataComingFromFileInteractive() {
        generateDataComingFromFile();
        JOptionPane.showMessageDialog(getDialogParent(),
            "<html>Data file generated and loaded:<br><b>"
            + new File(currentInputFilePath).getName() + "</b><br><br>"
            + "Location:<br>" + currentInputFilePath + "</html>",
            "Data File Generated", JOptionPane.INFORMATION_MESSAGE);
    }
    /**
     * Parses lines of the form:  name,type,value
     * Blank lines and lines starting with # are ignored.
     */
    private void loadAndDisplayInputFile() {
        fileEntriesPanel.removeAll();
        // Try path as-is, then relative to the class file's location
        File file = new File(currentInputFilePath);
        if (!file.isAbsolute() || !file.exists()) {
            // look next to the running class
            String dir = getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
            File alt = new File(new File(dir).getParent(), currentInputFilePath);
            if (alt.exists()) file = alt;
        }
        if (!file.exists()) {
            showFilePanelPlaceholder(file.getName());
            filePanelSourceLabel.setText(" " + file.getName() + "  ⚠ not found");
            fileEntriesPanel.revalidate();
            fileEntriesPanel.repaint();
            return;
        }
        // Parse name,type,value lines
        List<String[]> entries = new ArrayList<>();   // [name, type, value]
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#")) continue;
                String[] parts = line.split(",", 3);
                if (parts.length >= 3) {
                    entries.add(new String[]{parts[0].trim(), parts[1].trim(), parts[2].trim()});
                } else if (parts.length == 2) {
                    entries.add(new String[]{parts[0].trim(), parts[1].trim(), ""});
                } else {
                    entries.add(new String[]{parts[0].trim(), "string", ""});
                }
            }
        } catch (Exception ex) {
            JLabel err = new JLabel("<html><font color='red'><b>Error reading file:</b><br>" + ex.getMessage() + "</font></html>");
            err.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
            fileEntriesPanel.add(err);
            fileEntriesPanel.revalidate();
            fileEntriesPanel.repaint();
            return;
        }
        if (entries.isEmpty()) {
            JLabel empty = new JLabel("<html><center><font color='#8090B0'>File is empty.</font></center></html>", SwingConstants.CENTER);
            empty.setBorder(BorderFactory.createEmptyBorder(20, 8, 8, 8));
            fileEntriesPanel.add(empty);
        } else {
            // Show group header label when a specific group is selected
            if (selectedGroupIndex >= 0 && selectedGroupIndex < GROUP_DEFS.length) {
                String grpIcon = GROUP_DEFS[selectedGroupIndex][0];
                String grpName = GROUP_DEFS[selectedGroupIndex][1];
                String grpDesc = GROUP_DEFS[selectedGroupIndex][2];

                JPanel grpHdr = new JPanel(new BorderLayout(0, 0));
                grpHdr.setOpaque(true);
                grpHdr.setBackground(new Color(232, 236, 248));
                grpHdr.setBorder(BorderFactory.createCompoundBorder(
                    BorderFactory.createMatteBorder(0, 0, 1, 0, new Color(190, 200, 225)),
                    BorderFactory.createEmptyBorder(7, 10, 7, 10)
                ));
                grpHdr.setMaximumSize(new Dimension(Integer.MAX_VALUE, 46));
                grpHdr.setAlignmentX(Component.LEFT_ALIGNMENT);

                JLabel grpTitle = new JLabel(grpIcon + "  " + grpName);
                grpTitle.setFont(new Font("SansSerif", Font.BOLD, 12));
                grpTitle.setForeground(new Color(20, 45, 110));
                JLabel grpSubtitle = new JLabel(grpDesc);
                grpSubtitle.setFont(new Font("SansSerif", Font.PLAIN, 10));
                grpSubtitle.setForeground(new Color(90, 105, 150));

                JPanel grpText = new JPanel();
                grpText.setLayout(new BoxLayout(grpText, BoxLayout.Y_AXIS));
                grpText.setOpaque(false);
                grpText.add(grpTitle);
                grpText.add(Box.createVerticalStrut(1));
                grpText.add(grpSubtitle);
                grpHdr.add(grpText, BorderLayout.CENTER);

                fileEntriesPanel.add(grpHdr);
                // 6px breathing room between header and first card
                JPanel spacer = new JPanel();
                spacer.setOpaque(false);
                spacer.setPreferredSize(new Dimension(0, 6));
                spacer.setMaximumSize(new Dimension(Integer.MAX_VALUE, 6));
                fileEntriesPanel.add(spacer);
            }

            int shownCount = 0;
            for (int ei = 0; ei < entries.size(); ei++) {
                String[] e = entries.get(ei);
                // Filter: skip entries that don't belong to the selected group.
                // Position number (ei+1) is always the file position so
                // FileEntryConnection badge numbers stay stable.
                if (selectedGroupIndex != -1) {
                    int grp = getEntryGroup(e[0]);
                    if (grp != selectedGroupIndex) continue;
                }
                fileEntriesPanel.add(createFrontendEntryCard(e[0], e[1], e[2], ei + 1));
                shownCount++;
            }
            if (shownCount == 0 && selectedGroupIndex != -1) {
                JLabel none = new JLabel("<html><center><font color='#8090B0'>"
                        + "No inputs in this group.</font></center></html>", SwingConstants.CENTER);
                none.setBorder(BorderFactory.createEmptyBorder(20, 8, 8, 8));
                fileEntriesPanel.add(none);
            }
        }
        filePanelSourceLabel.setText(" " + file.getName() + "  (" + entries.size() + " entries)");
        filePanelSourceLabel.setToolTipText(file.getAbsolutePath());
        // Sync all FileEntryConnections: update entryValue + block inputValues from current data file
        java.util.Set<FunctionBlock> needsBadgeRefresh = new java.util.HashSet<>();
        java.util.Map<String, String> syncAliases = getParamAliases();
        // Build flat name→value lookup from the file entries for fast access
        java.util.Map<String, String> fileEntryValues = new java.util.HashMap<>();
        for (String[] e : entries) fileEntryValues.put(e[0].toUpperCase(), e[2]);

        for (FileEntryConnection fec : fileEntryConnections) {
            String matchName = fec.entryName.toUpperCase();
            String newVal = null;

            // 1. Exact name match
            if (fileEntryValues.containsKey(matchName)) {
                newVal = fileEntryValues.get(matchName);
            }
            // 2. matchName might itself be an alias → look up its canonical, then find that in file
            if (newVal == null) {
                String canonical = syncAliases.get(matchName);
                if (canonical != null && fileEntryValues.containsKey(canonical)) {
                    newVal = fileEntryValues.get(canonical);
                }
            }
            // 3. matchName is canonical → try all aliases of it in the file
            if (newVal == null) {
                for (java.util.Map.Entry<String, String> ae : syncAliases.entrySet()) {
                    if (ae.getValue().equals(matchName) && fileEntryValues.containsKey(ae.getKey())) {
                        newVal = fileEntryValues.get(ae.getKey());
                        break;
                    }
                }
            }

            if (newVal != null) {
                fec.entryValue = newVal;
                if (fec.toBlock != null) {
                    fec.toBlock.inputValues[fec.toInputIndex] = newVal;
                    needsBadgeRefresh.add(fec.toBlock);
                }
            }
        }
        for (FunctionBlock fb : needsBadgeRefresh) fb.refreshInputBadges();
        fileEntriesPanel.revalidate();
        fileEntriesPanel.repaint();
    }
    private void showFilePanelPlaceholder(String fname) {
        JPanel ph = new JPanel(new BorderLayout());
        ph.setOpaque(false);
        ph.setBorder(BorderFactory.createEmptyBorder(20, 10, 10, 10));
        JLabel icon = new JLabel("\uD83D\uDCC4", SwingConstants.CENTER);
        icon.setFont(new Font("SansSerif", Font.PLAIN, 36));
        icon.setForeground(new Color(170, 180, 210));
        ph.add(icon, BorderLayout.NORTH);
        JLabel msg = new JLabel("<html><center><font color='#8090B0' size='3'>"
            + "<b>" + fname + "</b><br>not found.<br><br>"
            + "Click \uD83D\uDCC1 to browse<br>for the data file."
            + "</font></center></html>", SwingConstants.CENTER);
        msg.setBorder(BorderFactory.createEmptyBorder(10, 6, 6, 6));
        ph.add(msg, BorderLayout.CENTER);
        fileEntriesPanel.add(ph);
    }
    /** Maps canonical parameter names to full biologist-readable display names. */
    private static String getDisplayName(String name) {
        return name;
    }

    /**
     * Creates a styled card for one entry: position circle / full display name / type-badge / value + Connect button.
     * position = 1-based index in the file (shown as the connection number on blocks)
     */
    private JPanel createFrontendEntryCard(String name, String type, String value, int position) {
        Color accentColor = getFilePanelTypeColor(type);
        boolean hasValue  = value != null && !value.isEmpty() && !"empty_file".equals(value);
        String displayValue = hasValue
            ? ("file".equalsIgnoreCase(type) ? new java.io.File(value).getName() : value)
            : "(not set)";
        String fullName = getDisplayName(name);

        // ── Outer wrapper: consistent 4px gap below every card ──
        JPanel outer = new JPanel();
        outer.setLayout(new BoxLayout(outer, BoxLayout.Y_AXIS));
        outer.setOpaque(false);
        outer.setBorder(BorderFactory.createEmptyBorder(0, 6, 4, 6));
        outer.setAlignmentX(Component.LEFT_ALIGNMENT);

        // ── Card ──
        JPanel card = new JPanel(new BorderLayout(0, 0));
        card.setBackground(Theme.BG_CARD);
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 3, 0, 0, accentColor),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(Theme.BORDER, 1),
                BorderFactory.createEmptyBorder(5, 7, 5, 5)
            )
        ));
        card.setAlignmentX(Component.LEFT_ALIGNMENT);

        // ── WEST: position circle ──
        JLabel posLabel = new JLabel(String.valueOf(position)) {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(accentColor);
                g2.fillOval(0, 0, getWidth(), getHeight());
                g2.dispose();
                super.paintComponent(g);
            }
        };
        posLabel.setFont(new Font("SansSerif", Font.BOLD, 9));
        posLabel.setForeground(Color.WHITE);
        posLabel.setOpaque(false);
        posLabel.setPreferredSize(new Dimension(18, 18));
        posLabel.setMinimumSize(new Dimension(18, 18));
        posLabel.setMaximumSize(new Dimension(18, 18));
        posLabel.setHorizontalAlignment(SwingConstants.CENTER);
        posLabel.setVerticalAlignment(SwingConstants.CENTER);
        posLabel.setToolTipText("File entry #" + position);
        JPanel posWrapper = new JPanel(new GridBagLayout()); // GridBagLayout centres circle vertically
        posWrapper.setOpaque(false);
        posWrapper.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 7));
        posWrapper.add(posLabel);
        card.add(posWrapper, BorderLayout.WEST);

        // ── CENTER: two rows — name+badge row, then value+connect row ──
        JPanel center = new JPanel();
        center.setLayout(new BoxLayout(center, BoxLayout.Y_AXIS));
        center.setOpaque(false);

        // Row 1: display name  |  type badge (right-aligned)
        JPanel nameRow = new JPanel(new BorderLayout(4, 0));
        nameRow.setOpaque(false);

        JLabel nameLabel = new JLabel(fullName);
        nameLabel.setFont(Theme.monoBold(10));
        nameLabel.setForeground(Theme.TEXT_DARK);
        nameLabel.setToolTipText("<html><b>" + fullName + "</b><br><font color='#6A96C8'>" + name + "</font></html>");
        nameRow.add(nameLabel, BorderLayout.CENTER);

        JLabel typeBadge = new JLabel(type.toLowerCase()) {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(accentColor);
                g2.fillRoundRect(0, 0, getWidth(), getHeight(), 8, 8);
                g2.dispose();
                super.paintComponent(g);
            }
        };
        typeBadge.setFont(new Font("SansSerif", Font.BOLD, 8));
        typeBadge.setForeground(Color.WHITE);
        typeBadge.setOpaque(false);
        typeBadge.setBorder(BorderFactory.createEmptyBorder(2, 5, 2, 5));
        typeBadge.setToolTipText("Type: " + type);
        JPanel badgeWrap = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, 0));
        badgeWrap.setOpaque(false);
        badgeWrap.add(typeBadge);
        nameRow.add(badgeWrap, BorderLayout.EAST);

        center.add(nameRow);
        center.add(Box.createVerticalStrut(3));

        // Row 2: value label (full width)
        JLabel valueLabel = new JLabel(displayValue);
        valueLabel.setFont(Theme.mono(9));
        valueLabel.setForeground(hasValue ? Theme.TEXT_MED : Theme.TEXT_LIGHT);
        valueLabel.setAlignmentX(Component.LEFT_ALIGNMENT);
        center.add(valueLabel);
        card.add(center, BorderLayout.CENTER);

        // ── Borders & colours used for state transitions ──
        javax.swing.border.Border normalBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 3, 0, 0, accentColor),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(Theme.BORDER, 1),
                BorderFactory.createEmptyBorder(5, 7, 5, 5)
            )
        );
        javax.swing.border.Border pendingBorder = BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 3, 0, 0, Theme.WARNING),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(Theme.WARNING_DK, 1),
                BorderFactory.createEmptyBorder(5, 7, 5, 5)
            )
        );
        Color normalBg    = Theme.BG_CARD;
        Color hoverBg     = Theme.CARD_HI;
        Color pendingBg   = new Color(35, 25, 10);
        Color normalNameFg  = Theme.TEXT_DARK;
        Color pendingNameFg = Theme.WARNING;
        final boolean[] isPending = {false};

        card.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));

        String normalTip = "<html><b>" + fullName + "</b>"
            + "<br><font color='#8090B0'>" + name + "</font>"
            + "<br>Type: " + type
            + "<br>Value: " + (hasValue ? value : "(not set)")
            + "<br><i>Click to connect &nbsp;|&nbsp; Right-click to copy</i></html>";
        card.setToolTipText(normalTip);

        // ── Click = connect; right-click = copy value ──
        final String copyVal = value;
        MouseAdapter cardHandler = new MouseAdapter() {
            public void mouseEntered(MouseEvent e) {
                if (!isPending[0]) card.setBackground(hoverBg);
            }
            public void mouseExited(MouseEvent e) {
                if (!isPending[0]) card.setBackground(normalBg);
            }
            public void mousePressed(MouseEvent e) {
                if (SwingUtilities.isRightMouseButton(e)) {
                    // right-click: copy value to clipboard
                    if (copyVal != null && !copyVal.isEmpty()) {
                        java.awt.datatransfer.StringSelection sel =
                            new java.awt.datatransfer.StringSelection(copyVal);
                        java.awt.Toolkit.getDefaultToolkit().getSystemClipboard().setContents(sel, null);
                        card.setBackground(new Color(20, 40, 80));
                        new javax.swing.Timer(300, ev -> {
                            if (!isPending[0]) card.setBackground(normalBg);
                        }) {{ setRepeats(false); }}.start();
                    }
                    return;
                }
                if (isPending[0]) return; // already waiting
                // left-click: activate connect mode
                isPending[0] = true;
                pendingEntryName     = name;
                pendingEntryType     = type;
                pendingEntryValue    = value;
                pendingEntryPosition = position;
                drawingPanel.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
                card.setBorder(pendingBorder);
                card.setBackground(pendingBg);
                nameLabel.setForeground(pendingNameFg);
                valueLabel.setText("Click a block input to connect…");
                valueLabel.setForeground(Theme.WARNING);
                card.setToolTipText("<html><b>Waiting for connection</b>"
                    + "<br>Click any block input to wire <i>" + fullName + "</i>"
                    + "<br><font color='#FBBF24'>Times out in 10 seconds</font></html>");
                new javax.swing.Timer(10000, ev -> {
                    if (pendingEntryName != null) {
                        isPending[0] = false;
                        pendingEntryName = pendingEntryType = pendingEntryValue = null;
                        pendingEntryPosition = 0;
                        drawingPanel.setCursor(Cursor.getDefaultCursor());
                        card.setBorder(normalBorder);
                        card.setBackground(normalBg);
                        nameLabel.setForeground(normalNameFg);
                        valueLabel.setText(displayValue);
                        valueLabel.setForeground(hasValue ? Theme.TEXT_MED : Theme.TEXT_LIGHT);
                        card.setToolTipText(normalTip);
                    }
                }) {{ setRepeats(false); }}.start();
            }
        };
        card.addMouseListener(cardHandler);
        center.addMouseListener(cardHandler);
        nameRow.addMouseListener(cardHandler);
        nameLabel.addMouseListener(cardHandler);
        valueLabel.addMouseListener(cardHandler);
        posWrapper.addMouseListener(cardHandler);

        outer.add(card);

        // ── Connection tags ──
        for (FileEntryConnection fec : fileEntryConnections) {
            if (fec.entryName.equals(name)) {
                outer.add(Box.createVerticalStrut(1));
                outer.add(buildConnectionTag(fec, accentColor));
            }
        }
        return outer;
    }
    /** Small tag below a file-entry card showing which block input it is wired to. */
    private JPanel buildConnectionTag(FileEntryConnection fec, Color accentColor) {
        JPanel tag = new JPanel(new BorderLayout(4, 0));
        tag.setBackground(Theme.CARD_HI);
        tag.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 4, 0, 0, accentColor),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(Theme.BORDER_HI, 1),
                BorderFactory.createEmptyBorder(3, 7, 3, 5)
            )
        ));
        tag.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        tag.setAlignmentX(Component.LEFT_ALIGNMENT);
        // Number badge — shows entry's file position
        JLabel numBadge = new JLabel("[" + fec.entryPosition + "]");
        numBadge.setFont(Theme.monoBold(9));
        numBadge.setForeground(accentColor);
        // Description
        String label = "\u2192 " + fec.toBlock.name + "  in" + (fec.toInputIndex + 1);
        if (fec.fileName != null && !fec.fileName.isEmpty()) label += "  \u2014 " + fec.fileName;
        JLabel connLabel = new JLabel(label);
        connLabel.setFont(Theme.mono(9));
        connLabel.setForeground(Theme.TEXT_MED);
        JPanel left = new JPanel(new FlowLayout(FlowLayout.LEFT, 3, 0));
        left.setOpaque(false);
        left.add(numBadge); left.add(connLabel);
        tag.add(left, BorderLayout.CENTER);
        // Delete button
        JLabel delBtn = new JLabel("\u2715");
        delBtn.setFont(new Font("SansSerif", Font.BOLD, 10));
        delBtn.setForeground(Theme.DANGER);
        delBtn.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        delBtn.setToolTipText("Delete this connection");
        delBtn.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                fileEntryConnections.remove(fec);
                // Also clear block's inputValue if it was set from this entry
                if (fec.toBlock != null && fec.toInputIndex >= 0 &&
                    fec.toInputIndex < fec.toBlock.inputValues.length) {
                    // reset to default
                    fec.toBlock.inputValues[fec.toInputIndex] =
                        fec.toBlock.getDefaultValuePublic(fec.toBlock.template.inputTypes[fec.toInputIndex]);
                    fec.toBlock.refreshInputBadges();
                }
                loadAndDisplayInputFile();
            }
        });
        tag.add(delBtn, BorderLayout.EAST);
        // Right-click menu on tag
        JPopupMenu tagMenu = new JPopupMenu();
        JMenuItem delItem = new JMenuItem("\uD83D\uDDD1 Delete Connection");
        delItem.addActionListener(e -> {
            fileEntryConnections.remove(fec);
            if (fec.toBlock != null && fec.toInputIndex >= 0 &&
                fec.toInputIndex < fec.toBlock.inputValues.length) {
                fec.toBlock.inputValues[fec.toInputIndex] =
                    fec.toBlock.getDefaultValuePublic(fec.toBlock.template.inputTypes[fec.toInputIndex]);
                fec.toBlock.refreshInputBadges();
            }
            loadAndDisplayInputFile();
        });
        tagMenu.add(delItem);
        JMenuItem editName = new JMenuItem("\u270F Edit File Name");
        editName.addActionListener(e -> {
            String cur = fec.fileName != null ? fec.fileName : "";
            String newName = JOptionPane.showInputDialog(gui4.this,
                "Edit file name for this connection:", cur);
            if (newName != null) {
                fec.fileName = newName.trim();
                // Propagate to block's inputValue
                if (fec.toBlock != null && fec.toInputIndex >= 0 &&
                    fec.toInputIndex < fec.toBlock.inputValues.length) {
                    fec.toBlock.inputValues[fec.toInputIndex] = fec.entryValue;
                    fec.toBlock.refreshInputBadges();
                }
                loadAndDisplayInputFile();
            }
        });
        tagMenu.add(editName);
        tag.setComponentPopupMenu(tagMenu);
        left.setComponentPopupMenu(tagMenu);
        return tag;
    }
    /**
     * Called when user clicks a FunctionBlock while in file-entry connection mode.
     * Picks the target input (auto if 1, dialog if >1), validates, creates FileEntryConnection.
     */
    private void connectFileEntryToBlock(FunctionBlock targetBlock) {
        if (pendingEntryName == null) return;
        String entryName  = pendingEntryName;
        String entryType  = pendingEntryType;
        String entryValue = pendingEntryValue;
        // Reset pending state
        pendingEntryName = pendingEntryType = pendingEntryValue = null;
        drawingPanel.setCursor(Cursor.getDefaultCursor());
        // Reload file panel (will reset Connect buttons)
        SwingUtilities.invokeLater(this::loadAndDisplayInputFile);

        if (targetBlock.template == null || targetBlock.template.inputCount == 0) {
            JOptionPane.showMessageDialog(this, "Block '" + targetBlock.name + "' has no inputs.", "No Inputs", JOptionPane.WARNING_MESSAGE);
            return;
        }
        // Find compatible inputs (not already connected)
        List<Integer> compatible = new ArrayList<>();
        for (int i = 0; i < targetBlock.template.inputCount; i++) {
            if (!typesCompatibleFE(targetBlock.template.inputTypes[i], entryType)) continue;
            // Already file-connected?
            boolean fecUsed = false;
            for (FileEntryConnection ex : fileEntryConnections) {
                if (ex.toBlock == targetBlock && ex.toInputIndex == i) { fecUsed = true; break; }
            }
            if (fecUsed) continue;
            // Already block-connected?
            boolean connUsed = false;
            for (Connection c : connections) {
                if (c.to == targetBlock && c.toIdx == i) { connUsed = true; break; }
            }
            if (!connUsed) compatible.add(i);
        }
        if (compatible.isEmpty()) {
            JOptionPane.showMessageDialog(this,
                "<html>No free compatible input found on block '<b>" + targetBlock.name + "</b>'.<br>" +
                "Entry type: <b>" + entryType + "</b></html>",
                "No Compatible Input", JOptionPane.WARNING_MESSAGE);
            return;
        }
        int chosenIdx;
        if (compatible.size() == 1) {
            chosenIdx = compatible.get(0);
        } else {
            // Ask user which input
            String[] opts = new String[compatible.size()];
            for (int k = 0; k < compatible.size(); k++) {
                int i = compatible.get(k);
                opts[k] = "Input " + (i + 1) + "  [" + targetBlock.template.inputTypes[i] + "]";
            }
            int sel = JOptionPane.showOptionDialog(this,
                "Connect '" + entryName + "' to which input of '" + targetBlock.name + "'?",
                "Choose Input", JOptionPane.DEFAULT_OPTION, JOptionPane.QUESTION_MESSAGE,
                null, opts, opts[0]);
            if (sel < 0) return;
            chosenIdx = compatible.get(sel);
        }
        // Create the connection
        FileEntryConnection fec = new FileEntryConnection(
            entryName, entryType, entryValue, pendingEntryPosition, targetBlock, chosenIdx);
        fileEntryConnections.add(fec);
        targetBlock.inputValues[chosenIdx] = entryValue;
        pendingEntryPosition = 0;
        targetBlock.refreshInputBadges();
        // Refresh file panel to show the new tag with the default name
        loadAndDisplayInputFile();
        // Brief visual confirmation on block
        Color origBg = targetBlock.getBackground();
        targetBlock.setBackground(new Color(200, 230, 200));
        new javax.swing.Timer(400, e -> targetBlock.setBackground(origBg)) {{ setRepeats(false); }}.start();
    }
    /** Normalizes a type string for comparison (mirrors FunctionBlock.normalizeType). */
    private static String normalizeTypeFE(String type) {
        if (type == null) return "";
        String n = type.trim().toLowerCase();
        if (n.equals("int")) return "integer";
        if (n.equals("character")) return "char";
        if (n.equals("number") || n.equals("double") || n.equals("long")) return "float";
        if (n.equals("path") || n.equals("filepath") || n.equals("filename")) return "file";
        return n;
    }
    private static boolean typesCompatibleFE(String a, String b) {
        String na = normalizeTypeFE(a);
        String nb = normalizeTypeFE(b);
        if (na.equals(nb)) return true;
        boolean aNum = na.equals("integer") || na.equals("float");
        boolean bNum = nb.equals("integer") || nb.equals("float");
        return aNum && bNum;
    }
    private JPanel createHistoryPanel() {
        JPanel panel = new JPanel(new BorderLayout(10, 10));
        panel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        panel.setBackground(new Color(248, 249, 250));
        JPanel headerPanel = new JPanel(new BorderLayout());
        headerPanel.setOpaque(false);
        
        JLabel titleLabel = new JLabel("Execution History");
        titleLabel.setFont(new Font("SansSerif", Font.BOLD, 18));
        titleLabel.setForeground(new Color(60, 60, 100));
        headerPanel.add(titleLabel, BorderLayout.WEST);
        JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        buttonPanel.setOpaque(false);
        
        JButton refreshBtn = new JButton("\uD83D\uDD04 Refresh");
        refreshBtn.addActionListener(e -> refreshHistoryDisplay());
        buttonPanel.add(refreshBtn);
        
        JButton clearBtn = new JButton("\uD83D\uDDD1\uFE0F Clear All");
        clearBtn.setBackground(new Color(220, 53, 69));
        clearBtn.addActionListener(e -> clearExecutionHistory());
        buttonPanel.add(clearBtn);
        
        headerPanel.add(buttonPanel, BorderLayout.EAST);
        panel.add(headerPanel, BorderLayout.NORTH);
        historyArea = new JTextArea();
        historyArea.setEditable(false);
        historyArea.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        historyArea.setBackground(Color.WHITE);
        JScrollPane historyScroll = new JScrollPane(historyArea);
        historyScroll.setBorder(BorderFactory.createLineBorder(new Color(200, 200, 220), 1));
        panel.add(historyScroll, BorderLayout.CENTER);
        refreshHistoryDisplay();
        return panel;
    }
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
        
        while (executionHistory.size() > 50) {
            executionHistory.remove(0);
        }
        
        saveExecutionHistory();
        refreshHistoryDisplay();
    }
    private void refreshHistoryDisplay() {
        if (historyArea == null) return;
        
        if (executionHistory.isEmpty()) {
            historyArea.setText("No execution history yet.\n\nRun an execution to see history here.");
        } else {
            StringBuilder display = new StringBuilder();
            display.append("=== EXECUTION HISTORY ===\n");
            display.append("Total entries: ").append(executionHistory.size()).append("\n\n");
            
            for (int i = executionHistory.size() - 1; i >= 0; i--) {
                display.append("[Entry #").append(i + 1).append("]\n");
                display.append(executionHistory.get(i));
                display.append("\n\n");
            }
            
            historyArea.setText(display.toString());
            historyArea.setCaretPosition(0);
        }
    }
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
    private boolean isFullScreen = false;
    private java.awt.GraphicsDevice fullScreenDevice =
        java.awt.GraphicsEnvironment.getLocalGraphicsEnvironment().getDefaultScreenDevice();

    private void toggleFullScreen() {
        // In embedded mode the gui4 JFrame is hidden — find the real visible ancestor window.
        java.awt.Window w = SwingUtilities.getWindowAncestor(drawingPanel);
        JFrame frame;
        if (w instanceof JFrame) {
            frame = (JFrame) w;
        } else if (!embeddedMode) {
            frame = this;
        } else {
            return;
        }
        if (isFullScreen) {
            fullScreenDevice.setFullScreenWindow(null);
            frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
            isFullScreen = false;
        } else {
            if (fullScreenDevice.isFullScreenSupported()) {
                fullScreenDevice.setFullScreenWindow(frame);
                isFullScreen = true;
            } else {
                frame.setExtendedState(JFrame.MAXIMIZED_BOTH);
            }
        }
    }

    private Component getDialogParent() {
        if (embeddedMode) {
            Container p = getContentPane().getParent();
            while (p != null) {
                if (p instanceof java.awt.Window && ((java.awt.Window) p).isVisible()) return p;
                p = p.getParent();
            }
            return drawingPanel;
        }
        return this;
    }

    private void setupKeyboardShortcuts() {
        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(e -> {
            if (e.getID() == KeyEvent.KEY_PRESSED) {
                // F11: toggle fullscreen
                if (e.getKeyCode() == KeyEvent.VK_F11) {
                    toggleFullScreen();
                    return true;
                }
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
                        case KeyEvent.VK_S:
                            savePipeline();
                            return true;
                        case KeyEvent.VK_O:
                            loadPipeline();
                            return true;
                    }
                }
            }
            return false;
        });
    }
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
        
        int x = clipboardBlock.getX() + 50;
        int y = clipboardBlock.getY() + 50;
        
        int scaledWidth = (int)(newBlock.getPreferredSize().width * zoomFactor);
        int scaledHeight = (int)(newBlock.getPreferredSize().height * zoomFactor);
        newBlock.setBounds(x, y, scaledWidth, scaledHeight);
        for (int i = 0; i < template.inputCount && i < clipboardBlock.inputValues.length; i++) {
            newBlock.inputValues[i] = clipboardBlock.inputValues[i];
        }
        for (int i = 0; i < template.outputCount && clipboardBlock.outputVarNames != null && i < clipboardBlock.outputVarNames.length; i++) {
            newBlock.outputVarNames[i] = clipboardBlock.outputVarNames[i];
        }
        functionCounter++;
        functionBlocks.add(newBlock);
        drawingPanel.add(newBlock);
        updateCanvasSize();
        drawingPanel.repaint();
    }
    private void DelSelectedBlock() {
        for (FunctionBlock fb : functionBlocks) {
            if (fb.isSelected) {
                functionBlocks.remove(fb);
                drawingPanel.remove(fb);
                connections.removeIf(conn -> conn.from == fb || conn.to == fb);
                drawingPanel.repaint();
                return;
            }
        }
    }
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
    private void newPipeline() {
        int confirm = JOptionPane.showConfirmDialog(this,
            "Start a fresh pipeline?\nAll blocks, connections and file-entry links will be cleared.",
            "New Pipeline", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);
        if (confirm != JOptionPane.YES_OPTION) return;
        // Remove all block panels from canvas
        for (FunctionBlock fb : functionBlocks) drawingPanel.remove(fb);
        // Reset all state
        functionBlocks.clear();
        connections.clear();
        fileEntryConnections.clear();
        dragSource = null;
        dragSourceOutputIndex = -1;
        selectedConnection = null;
        clipboardBlock = null;
        lastSavedPipelineFile = null;
        functionCounter = 1;
        for (String key : instanceCounter.keySet()) instanceCounter.put(key, 0);
        updateCanvasSize();
        drawingPanel.repaint();
    }
    private void showEditBlockDescriptionDialog(String blockName) {
        String baseName = blockName.replaceAll("_\\d+$", "");
        String[] currentDesc = getBlockDescription(baseName);
        
        JTextField fullNameField = new JTextField(currentDesc[0], 30);
        JTextArea descriptionArea = new JTextArea(currentDesc[1], 5, 30);
        descriptionArea.setLineWrap(true);
        descriptionArea.setWrapStyleWord(true);
        
        String[] categories = {"Control Flow", "Graph Algorithms", "Graph Processing", 
                               "Package Management", "Variable Operations", "Execution", 
                               "Analysis", "Output/Aggregation", "Custom"};
        JComboBox<String> categoryCombo = new JComboBox<>(categories);
        categoryCombo.setEditable(true);
        String currentCategory = (currentDesc.length > 2) ? currentDesc[2] : "Custom";
        categoryCombo.setSelectedItem(currentCategory);
        
        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.anchor = GridBagConstraints.WEST;
        
        gbc.gridx = 0; gbc.gridy = 0;
        panel.add(new JLabel("Block Name:"), gbc);
        gbc.gridx = 1;
        JLabel nameLabel = new JLabel(baseName);
        nameLabel.setFont(nameLabel.getFont().deriveFont(Font.BOLD));
        panel.add(nameLabel, gbc);
        
        gbc.gridx = 0; gbc.gridy = 1;
        panel.add(new JLabel("Full Name:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.HORIZONTAL;
        panel.add(fullNameField, gbc);
        
        gbc.gridx = 0; gbc.gridy = 2; gbc.fill = GridBagConstraints.NONE;
        panel.add(new JLabel("Category:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.HORIZONTAL;
        panel.add(categoryCombo, gbc);
        
        gbc.gridx = 0; gbc.gridy = 3; gbc.fill = GridBagConstraints.NONE;
        panel.add(new JLabel("Description:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.BOTH; gbc.weightx = 1; gbc.weighty = 1;
        panel.add(new JScrollPane(descriptionArea), gbc);
        
        int result = JOptionPane.showConfirmDialog(this, panel, 
            "Edit Block Description: " + baseName, 
            JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
        
        if (result == JOptionPane.OK_OPTION) {
            String newFullName = fullNameField.getText().trim();
            String newDescription = descriptionArea.getText().trim();
            String newCategory = (String) categoryCombo.getSelectedItem();
            
            if (newFullName.isEmpty()) newFullName = baseName.toUpperCase();
            if (newDescription.isEmpty()) newDescription = "No description";
            if (newCategory == null || newCategory.isEmpty()) newCategory = "Custom";
            
            BLOCK_DESCRIPTIONS.put(baseName, new String[]{newFullName, newDescription, newCategory});
            
            saveBlockDescriptions();
            
            for (FunctionBlock fb : functionBlocks) {
                if (fb.name.replaceAll("_\\d+$", "").equals(baseName)) {
                    fb.updateBorder();
                }
            }
            
            refreshBlockListPanel();
            
            drawingPanel.repaint();
        }
    }
    private void showAllBlockDescriptions() {
        JDialog dialog = new JDialog(this, "Block Descriptions & Full Forms", true);
        dialog.setLayout(new BorderLayout(10, 10));
        dialog.setSize(700, 500);
        
        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        
        List<String> sortedKeys = new ArrayList<>(BLOCK_DESCRIPTIONS.keySet());
        Collections.sort(sortedKeys);
        
        for (String blockName : sortedKeys) {
            String[] desc = BLOCK_DESCRIPTIONS.get(blockName);
            if (desc == null || desc.length < 2) continue;
            
            JPanel blockPanel = new JPanel(new BorderLayout(10, 5));
            blockPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(new Color(100, 100, 200), 2),
                BorderFactory.createEmptyBorder(10, 10, 10, 10)
            ));
            blockPanel.setBackground(new Color(240, 240, 255));
            blockPanel.setMaximumSize(new Dimension(Integer.MAX_VALUE, 120));
            
            JPanel textPanel = new JPanel();
            textPanel.setLayout(new BoxLayout(textPanel, BoxLayout.Y_AXIS));
            textPanel.setOpaque(false);
            
            String category = (desc.length > 2) ? desc[2] : "Custom";
            JLabel titleLabel = new JLabel("<html><b style='font-size:14px;color:#4040A0;'>" + 
                blockName.toUpperCase() + "</b> - " + desc[0] + 
                " <font color='gray'>[" + category + "]</font></html>");
            textPanel.add(titleLabel);
            
            JTextArea descArea = new JTextArea(desc[1]);
            descArea.setLineWrap(true);
            descArea.setWrapStyleWord(true);
            descArea.setEditable(false);
            descArea.setBackground(new Color(240, 240, 255));
            descArea.setBorder(BorderFactory.createEmptyBorder(5, 0, 0, 0));
            descArea.setRows(2);
            textPanel.add(descArea);
            
            blockPanel.add(textPanel, BorderLayout.CENTER);
            
            JPanel buttonPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 0));
            buttonPanel.setOpaque(false);
            
            JButton editBtn = new JButton("\u270F\uFE0F Edit");
            editBtn.setToolTipText("Edit this block's description");
            final String bn = blockName;
            editBtn.addActionListener(e -> {
                dialog.dispose();
                showEditBlockDescriptionDialog(bn);
                showAllBlockDescriptions();
            });
            buttonPanel.add(editBtn);
            
            JButton deleteBtn = new JButton("\uD83D\uDDD1\uFE0F Delete");
            deleteBtn.setToolTipText("Delete this block template");
            deleteBtn.setBackground(new Color(255, 200, 200));
            deleteBtn.addActionListener(e -> {
                int confirm = JOptionPane.showConfirmDialog(dialog,
                    "Are you sure you want to delete the block template '" + bn + "'?\n" +
                    "This will also remove all instances from the canvas.",
                    "Confirm Delete",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE);
                
                if (confirm == JOptionPane.YES_OPTION) {
                    List<FunctionBlock> toRemove = new ArrayList<>();
                    for (FunctionBlock fb : functionBlocks) {
                        if (fb.template.name.equals(bn)) {
                            toRemove.add(fb);
                        }
                    }
                    for (FunctionBlock fb : toRemove) {
                        functionBlocks.remove(fb);
                        drawingPanel.remove(fb);
                        connections.removeIf(conn -> conn.from == fb || conn.to == fb);
                    }
                    
                    blockLibrary.remove(bn);
                    instanceCounter.remove(bn);
                    BLOCK_DESCRIPTIONS.remove(bn);
                    
                    refreshBlockListPanel();
                    refreshBlockSelector();
                    
                    saveBlockDescriptions();
                    saveBlockTemplates();
                    
                    drawingPanel.repaint();
                    
                    dialog.dispose();
                    showAllBlockDescriptions();
                }
            });
            buttonPanel.add(deleteBtn);
            
            blockPanel.add(buttonPanel, BorderLayout.EAST);
            
            mainPanel.add(blockPanel);
            mainPanel.add(Box.createVerticalStrut(10));
        }
        
        JScrollPane scrollPane = new JScrollPane(mainPanel);
        scrollPane.getVerticalScrollBar().setUnitIncrement(16);
        dialog.add(scrollPane, BorderLayout.CENTER);
        
        JPanel bottomPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
        JButton addNewBtn = new JButton("+ Add New Block");
        addNewBtn.addActionListener(e -> {
            dialog.dispose();
            showNewBlockTemplateDialog();
        });
        bottomPanel.add(addNewBtn);
        
        JButton closeBtn = new JButton("Close");
        closeBtn.addActionListener(e -> dialog.dispose());
        bottomPanel.add(closeBtn);
        
        dialog.add(bottomPanel, BorderLayout.SOUTH);
        
        dialog.setLocationRelativeTo(this);
        dialog.setVisible(true);
    }
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
                        // Show fileName if available for file connections
                        if (c.fileName != null && !c.fileName.trim().isEmpty()) {
                            value = "$" + c.from.name + ".output" + (c.fromIdx + 1) + " [" + c.fileName + "]";
                        } else {
                            value = "$" + c.from.name + ".output" + (c.fromIdx + 1);
                        }
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
        if (type == null) return true;
        String normalizedType = type.trim().toLowerCase();
        switch (normalizedType) {
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
            case "character":
                return value != null && value.length() <= 1;
            case "string":
                return value != null;
            case "file":
                return value == null || value.trim().isEmpty() || (value.contains(".") || value.contains("/") || value.contains("\\"));
            case "graph":
            case "status":
            default:
                return true;
        }
    }
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
        addTemplateInternal(new BlockTemplate("let",  2, 1, new String[]{"string","string"},   new String[]{"var"}));
        addTemplateInternal(new BlockTemplate("exec", 1, 0, new String[]{"string"},             new String[]{}));
        addTemplateInternal(new BlockTemplate("size", 1, 0, new String[]{"integer"},            new String[]{}));
        addTemplateInternal(new BlockTemplate("cudf", 3, 2, new String[]{"file","float","float"}, new String[]{"file","file"}));
        addTemplateInternal(new BlockTemplate("start", 4, 0, new String[]{"file","file","file","file"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("mff", 3, 0,
            new String[]{"file","integer","file"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("wgx", 1, 0,
            new String[]{"file"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("rgx", 2, 0,
            new String[]{"file","integer"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("fb_rch", 4, 0,
            new String[]{"string","string","integer","integer"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("pathz3", 28, 0,
            new String[]{"integer","integer","file","integer","integer","integer","integer","integer",
                         "file","file","file","file","file","file","file","file","file","file","file",
                         "file","file","file","file","file","integer","integer","integer","integer"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("result", 4, 3, new String[]{"float","string","file","graph"}, new String[]{"graph","string","float"}));
        for (Map.Entry<String, BlockTemplate> entry : blockLibrary.entrySet()) {
            if (!isInBlockSelector(entry.getKey())) {
                addTemplateInternal(entry.getValue());
            }
        }
        refreshBlockListPanel();
        refreshBlockSelector();
    }
    private boolean isInBlockSelector(String name) {
        for (int i = 0; i < blockSelector.getItemCount(); i++) {
            if (blockSelector.getItemAt(i).equals(name)) {
                return true;
            }
        }
        return false;
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
    private void addTemplateInternal(BlockTemplate template) {
        if (!blockLibrary.containsKey(template.name)) {
            blockLibrary.put(template.name, template);
            instanceCounter.put(template.name, 0);
        }
        
        if (!BLOCK_DESCRIPTIONS.containsKey(template.name)) {
            BLOCK_DESCRIPTIONS.put(template.name, new String[]{
                template.name.toUpperCase() + " Block",
                "Custom block with " + template.inputCount + " inputs and " + template.outputCount + " outputs.",
                "Custom"
            });
        }
    }
    private void refreshBlockListPanel() {
        blockListPanel.removeAll();
        
        List<String> sortedNames = new ArrayList<>(blockLibrary.keySet());
        Collections.sort(sortedNames);
        
        for (String templateName : sortedNames) {
            BlockTemplate template = blockLibrary.get(templateName);
            String[] desc = getBlockDescription(templateName);
            
            JButton blockBtn = new JButton("<html><center>"
                + "<b style='color:#D6E8FF;font-family:Consolas;'>" + template.name + "</b>"
                + "<br><font size='2' color='#6A96C8'>" + desc[0] + "</font>"
                + "</center></html>");
            blockBtn.setAlignmentX(Component.LEFT_ALIGNMENT);
            blockBtn.setPreferredSize(new Dimension(150, 52));
            blockBtn.setMinimumSize(new Dimension(150, 52));
            blockBtn.setMaximumSize(new Dimension(150, 52));
            blockBtn.setBackground(Theme.SURFACE);
            blockBtn.setForeground(Theme.TEXT_DARK);
            blockBtn.setOpaque(true);
            blockBtn.setBorderPainted(true);
            blockBtn.setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));
            blockBtn.setFocusPainted(false);
            blockBtn.setToolTipText("<html><b style='color:#D6E8FF;'>" + desc[0] + "</b><br><font color='#6A96C8'>" + desc[1] + "</font></html>");
            blockBtn.addActionListener(e -> addBlockInstance(template.name));

            blockListPanel.add(blockBtn);
        }
        
        blockListPanel.revalidate();
        blockListPanel.repaint();
    }
    private void refreshBlockSelector() {
        blockSelector.removeAllItems();
        
        List<String> sortedNames = new ArrayList<>(blockLibrary.keySet());
        Collections.sort(sortedNames);
        
        for (String blockName : sortedNames) {
            blockSelector.addItem(blockName);
        }
        blockSelector.addItem("Add New Block Template...");
    }
    private void addTemplate(BlockTemplate template) {
        addTemplateInternal(template);
        refreshBlockListPanel();
        refreshBlockSelector();
        saveBlockTemplates();
        saveBlockDescriptions();
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
        
        String[] categories = {"Control Flow", "Graph Algorithms", "Graph Processing", 
                               "Package Management", "Variable Operations", "Execution", 
                               "Analysis", "Output/Aggregation", "Custom"};
        JComboBox<String> categoryCombo = new JComboBox<>(categories);
        categoryCombo.setEditable(true);
        categoryCombo.setSelectedItem("Custom");
        
        JSpinner inputSpinner = new JSpinner(new SpinnerNumberModel(1, 0, 10, 1));
        JSpinner outputSpinner = new JSpinner(new SpinnerNumberModel(1, 1, 5, 1));
        JPanel panel = new JPanel(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);
        gbc.anchor = GridBagConstraints.WEST;
        
        gbc.gridx = 0; gbc.gridy = 0;
        panel.add(new JLabel("Function Name (short):"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.HORIZONTAL;
        panel.add(nameField, gbc);
        
        gbc.gridx = 0; gbc.gridy = 1; gbc.fill = GridBagConstraints.NONE;
        panel.add(new JLabel("Full Name:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.HORIZONTAL;
        panel.add(fullNameField, gbc);
        
        gbc.gridx = 0; gbc.gridy = 2; gbc.fill = GridBagConstraints.NONE;
        panel.add(new JLabel("Category:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.HORIZONTAL;
        panel.add(categoryCombo, gbc);
        
        gbc.gridx = 0; gbc.gridy = 3; gbc.fill = GridBagConstraints.NONE;
        panel.add(new JLabel("Description:"), gbc);
        gbc.gridx = 1; gbc.fill = GridBagConstraints.BOTH; gbc.weightx = 1; gbc.weighty = 1;
        panel.add(new JScrollPane(descriptionArea), gbc);
        
        gbc.gridx = 0; gbc.gridy = 4; gbc.fill = GridBagConstraints.NONE; gbc.weightx = 0; gbc.weighty = 0;
        panel.add(new JLabel("# Inputs:"), gbc);
        gbc.gridx = 1;
        panel.add(inputSpinner, gbc);
        
        gbc.gridx = 0; gbc.gridy = 5;
        panel.add(new JLabel("# Outputs:"), gbc);
        gbc.gridx = 1;
        panel.add(outputSpinner, gbc);
        int res = JOptionPane.showConfirmDialog(this, panel, "New Block Template", JOptionPane.OK_CANCEL_OPTION);
        if (res != JOptionPane.OK_OPTION) return;
        int inCount = (int) inputSpinner.getValue();
        int outCount = (int) outputSpinner.getValue();
        String name = nameField.getText().trim();
        if (name.isEmpty() || blockLibrary.containsKey(name)) {
            JOptionPane.showMessageDialog(this, "Invalid or duplicate name");
            return;
        }
        String fullName = fullNameField.getText().trim();
        if (fullName.isEmpty()) fullName = name.toUpperCase() + " Block";
        
        String description = descriptionArea.getText().trim();
        if (description.isEmpty()) description = "Custom block";
        
        String category = (String) categoryCombo.getSelectedItem();
        if (category == null || category.isEmpty()) category = "Custom";
        BLOCK_DESCRIPTIONS.put(name, new String[]{fullName, description, category});
        String[] types = {"float", "string", "file", "graph", "integer", "Status", "char"};
        String[] inTypes = new String[inCount];
        String[] outTypes = new String[outCount];
        JPanel typePanel = new JPanel(new GridLayout(inCount + outCount, 2, 5, 5));
        typePanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        
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
        int typeRes = JOptionPane.showConfirmDialog(this, typePanel, "Select Types for " + name, JOptionPane.OK_CANCEL_OPTION);
        if (typeRes == JOptionPane.OK_OPTION) {
            for (int i = 0; i < inCount; i++) {
                inTypes[i] = (String) inComboBoxes[i].getSelectedItem();
            }
            for (int i = 0; i < outCount; i++) {
                outTypes[i] = (String) outComboBoxes[i].getSelectedItem();
            }
            
            addTemplate(new BlockTemplate(name, inCount, outCount, inTypes, outTypes));
            
            JOptionPane.showMessageDialog(this, 
                "Block template '" + name + "' created successfully!\n" +
                "It has been added to the block list and selector.",
                "Template Created",
                JOptionPane.INFORMATION_MESSAGE);
        }
    }
    private void executeGraph() {
        List<FunctionBlock> order = getTopologicalOrder();
        if (order == null) return;
        List<String> unfilled = getUnfilledInputs(order);
        if (!unfilled.isEmpty()) {
            StringBuilder msg = new StringBuilder("The following inputs have no data:\n\n");
            for (String s : unfilled) msg.append("  • ").append(s).append("\n");
            msg.append("\nRun pipeline anyway?");
            int choice = JOptionPane.showConfirmDialog(getDialogParent(), msg.toString(),
                    "Unfilled Inputs", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);
            if (choice != JOptionPane.YES_OPTION) return;
        }
        String executionContent = saveExecutionPlan(order);
        
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
        resultText.append("\n=== BLOCK CONNECTIONS ===\n");
        for (Connection c : connections) {
            if (c.toIdx == -1) {
                resultText.append("$" + c.from.originalName + ".output" + (c.fromIdx + 1) + " -> $" + c.to.originalName + " (status)\n");
            } else {
                String connLine = "$" + c.from.originalName + ".output" + (c.fromIdx + 1) + " -> $" + c.to.originalName + ".input" + (c.toIdx + 1);
                // Show fileName for file connections in results
                if (c.fileName != null && !c.fileName.trim().isEmpty()) {
                    connLine += " [file: " + c.fileName + "]";
                }
                resultText.append(connLine + "\n");
            }
        }
        
        resultText.append("\n=== FILE PANEL CONNECTIONS ===\n");
        if (fileEntryConnections.isEmpty()) {
            resultText.append("(None)\n");
        } else {
            for (FileEntryConnection f : fileEntryConnections) {
                String valShort = f.entryValue != null && f.entryValue.length() > 30 
                                      ? f.entryValue.substring(0, 27) + "..." : f.entryValue;
                resultText.append(String.format("File Entry [%d] '%s' (%s) -> $%s.input%d",
                    f.entryPosition, f.entryName, f.entryType, f.toBlock.originalName, (f.toInputIndex + 1)));
                if (f.fileName != null && !f.fileName.trim().isEmpty()) {
                    resultText.append(String.format("  [file param: %s]", f.fileName));
                }
                resultText.append(String.format("  [Value: %s]\n", valShort));
            }
        }
        
        resultArea.setText(resultText.toString());
        tabbedPane.setSelectedIndex(1);
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
                        
                        // Name the output variable \u2014 works for ALL connection types
                        contextMenu.addSeparator();
                        final Connection theConn = selectedConnection;
                        JMenuItem setVarItem = new JMenuItem("\uD83D\uDD16 Set Output Variable Name");
                        setVarItem.setToolTipText("Name this output \u2014 becomes $VAR_NAME in generated script");
                        setVarItem.addActionListener(ev -> {
                            String cur = (theConn.fileName != null) ? theConn.fileName : "";
                            JPanel p = new JPanel(new GridBagLayout());
                            GridBagConstraints g = new GridBagConstraints();
                            g.insets = new Insets(4, 5, 4, 5);
                            g.anchor = GridBagConstraints.WEST;
                            g.fill = GridBagConstraints.HORIZONTAL;
                            g.gridx = 0; g.gridy = 0; g.gridwidth = 2;
                            JLabel hdr = new JLabel("<html><b>Output Variable Name</b></html>");
                            hdr.setFont(new Font("SansSerif", Font.BOLD, 13));
                            hdr.setForeground(new Color(80, 40, 140));
                            p.add(hdr, g);
                            g.gridy = 1; g.gridwidth = 1;
                            p.add(new JLabel("Connection:"), g);
                            g.gridx = 1;
                            p.add(new JLabel(theConn.from.name + " \u2192 " + theConn.to.name), g);
                            g.gridx = 0; g.gridy = 2; g.gridwidth = 2;
                            p.add(new JSeparator(), g);
                            g.gridy = 3; g.gridwidth = 1;
                            p.add(new JLabel("Variable name:"), g);
                            g.gridx = 1; g.weightx = 1.0; g.fill = GridBagConstraints.HORIZONTAL;
                            JTextField field = new JTextField(cur, 22);
                            field.setToolTipText("Appears as $NAME in generated script");
                            field.addHierarchyListener(he -> {
                                if ((he.getChangeFlags() & java.awt.event.HierarchyEvent.SHOWING_CHANGED) != 0 && field.isShowing())
                                    SwingUtilities.invokeLater(field::requestFocusInWindow);
                            });
                            p.add(field, g);
                            g.gridx = 0; g.gridy = 4; g.gridwidth = 1; g.weightx = 0; g.fill = GridBagConstraints.NONE;
                            JButton connPasteBtn = new JButton("Paste");
                            connPasteBtn.setMargin(new Insets(2, 8, 2, 8));
                            connPasteBtn.addActionListener(pev -> {
                                try {
                                    java.awt.datatransfer.Clipboard cb = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
                                    String clip = (String) cb.getData(java.awt.datatransfer.DataFlavor.stringFlavor);
                                    if (clip != null) field.setText(clip.trim());
                                } catch (Exception ex) { /* clipboard empty or not text */ }
                                field.requestFocusInWindow();
                            });
                            p.add(connPasteBtn, g);
                            g.gridx = 0; g.gridy = 5; g.gridwidth = 2;
                            p.add(new JLabel("<html><font color='gray'><i>e.g. UP_REG_FILE, MERGED_GRAPH, MY_OUTPUT</i></font></html>"), g);
                            int res = JOptionPane.showConfirmDialog(gui4.this, p,
                                    "Name output of " + theConn.from.name,
                                    JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
                            if (res == JOptionPane.OK_OPTION) {
                                String v = field.getText().trim().toUpperCase().replace(" ", "_").replace("-", "_");
                                theConn.fileName = v.isEmpty() ? null : v;
                                // Sync to source block's outputVarNames so script generator picks it up
                                if (theConn.fromIdx >= 0 && theConn.from.outputVarNames != null
                                        && theConn.fromIdx < theConn.from.outputVarNames.length) {
                                    theConn.from.outputVarNames[theConn.fromIdx] = v.isEmpty() ? null : v;
                                }
                                repaint();
                            }
                        });
                        contextMenu.add(setVarItem);
                        if (selectedConnection.fileName != null && !selectedConnection.fileName.trim().isEmpty()) {
                            JMenuItem curItem = new JMenuItem("  \u2192  " + selectedConnection.fileName);
                            curItem.setEnabled(false);
                            curItem.setFont(curItem.getFont().deriveFont(Font.BOLD));
                            contextMenu.add(curItem);
                            JMenuItem clearItem = new JMenuItem("\u274C Clear Variable Name");
                            clearItem.addActionListener(ev -> { theConn.fileName = null; repaint(); });
                            contextMenu.add(clearItem);
                        }
                        
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
                    zoom(factor, e.getPoint());
                    e.consume();
                } else {
                    // Forward to parent JScrollPane so mouse wheel scrolls the canvas
                    Container p = getParent();
                    while (p != null && !(p instanceof JScrollPane)) p = p.getParent();
                    if (p instanceof JScrollPane) {
                        JScrollPane sp = (JScrollPane) p;
                        JScrollBar bar = e.isShiftDown()
                            ? sp.getHorizontalScrollBar()
                            : sp.getVerticalScrollBar();
                        bar.setValue(bar.getValue() + e.getUnitsToScroll() * bar.getUnitIncrement());
                    }
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
            if (type == null) return Theme.T_OTHER;
            String normalizedType = type.trim().toLowerCase();
            switch (normalizedType) {
                case "float":                   return Theme.T_FLOAT;
                case "integer": case "int":     return Theme.T_INT;
                case "string":                  return Theme.T_STRING;
                case "file":                    return Theme.T_FILE;
                case "graph":                   return Theme.T_GRAPH;
                case "status":                  return Theme.SUCCESS;
                case "character": case "char":  return Theme.WARNING;
                default:                        return Theme.T_OTHER;
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
            // Must call super first so it doesn't erase our custom drawing
            super.paintComponent(g);
            Graphics2D g2 = (Graphics2D) g.create();
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            // Canvas background
            g2.setColor(Theme.CANVAS_BG);
            g2.fillRect(0, 0, getWidth(), getHeight());
            // Dot grid
            g2.setColor(Theme.CANVAS_DOT);
            int gridSize = 24;
            for (int x = 0; x < getWidth(); x += gridSize) {
                for (int y = 0; y < getHeight(); y += gridSize) {
                    g2.fillOval(x - 1, y - 1, 3, 3);
                }
            }
            g2.translate(translateX, translateY);
            g2.scale(zoomFactor, zoomFactor);
            for (Connection conn : connections) {
                Point from = getOutputPoint(conn.from, conn.fromIdx);
                Point to = getInputPoint(conn.to, conn.toIdx);
                // Resolve display name: outputVarNames wins, then conn.fileName, then null
                String displayName = (conn.from.outputVarNames != null
                        && conn.fromIdx < conn.from.outputVarNames.length
                        && conn.from.outputVarNames[conn.fromIdx] != null
                        && !conn.from.outputVarNames[conn.fromIdx].trim().isEmpty())
                        ? conn.from.outputVarNames[conn.fromIdx].trim()
                        : (conn.fileName != null && !conn.fileName.trim().isEmpty() ? conn.fileName.trim() : null);
                drawSmartConnection(g2, from, to, conn.from, conn.to, conn.type, conn == selectedConnection, displayName);
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
        private boolean lineIntersectsBlock(int x1, int y1, int x2, int y2, FunctionBlock block, FunctionBlock fromBlock, FunctionBlock toBlock) {
            if (block == fromBlock || block == toBlock) return false;
            
            Rectangle bounds = block.getBounds();
            int bx = (int)(bounds.x / zoomFactor);
            int by = (int)(bounds.y / zoomFactor);
            int bw = (int)(bounds.width / zoomFactor);
            int bh = (int)(bounds.height / zoomFactor);
            
            int padding = 10;
            bx -= padding;
            by -= padding;
            bw += 2 * padding;
            bh += 2 * padding;
            
            return lineIntersectsRect(x1, y1, x2, y2, bx, by, bw, bh);
        }
        private boolean lineIntersectsRect(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh) {
            if (lineIntersectsLine(x1, y1, x2, y2, rx, ry, rx + rw, ry)) return true;
            if (lineIntersectsLine(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh)) return true;
            if (lineIntersectsLine(x1, y1, x2, y2, rx + rw, ry + rh, rx, ry + rh)) return true;
            if (lineIntersectsLine(x1, y1, x2, y2, rx, ry + rh, rx, ry)) return true;
            
            if (x1 >= rx && x1 <= rx + rw && y1 >= ry && y1 <= ry + rh) return true;
            
            return false;
        }
        private boolean lineIntersectsLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
            double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
            if (Math.abs(denom) < 0.0001) return false;
            
            double ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / denom;
            double ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / denom;
            
            return ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1;
        }
        private boolean needsRouting(Point from, Point to, FunctionBlock fromBlock, FunctionBlock toBlock) {
            for (FunctionBlock block : functionBlocks) {
                if (lineIntersectsBlock(from.x, from.y, to.x, to.y, block, fromBlock, toBlock)) {
                    return true;
                }
            }
            return false;
        }
        // ── Connection drawing ────────────────────────────────────────────────
        private void drawSmartConnection(Graphics2D g2, Point from, Point to,
                FunctionBlock fromBlock, FunctionBlock toBlock,
                String type, boolean isSelected, String fileName) {
            g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2.setRenderingHint(RenderingHints.KEY_STROKE_CONTROL, RenderingHints.VALUE_STROKE_PURE);
            Color base  = getConnectionColor(type);
            Color line  = isSelected ? new Color(210, 40, 40) : base;
            boolean forward = to.x >= from.x + 24;
            if (forward && !needsRouting(from, to, fromBlock, toBlock)) {
                drawCubicConnection(g2, from, to, line, isSelected, fileName);
            } else {
                drawRoutedConnection(g2, from, to, fromBlock, toBlock, line, isSelected, fileName);
            }
        }

        /** Smooth cubic Bezier for forward (left-to-right) connections. */
        private void drawCubicConnection(Graphics2D g2, Point from, Point to,
                Color color, boolean isSelected, String label) {
            int dx = Math.abs(to.x - from.x);
            int ctrl = Math.max(70, dx / 2);
            int cx1 = from.x + ctrl, cy1 = from.y;
            int cx2 = to.x  - ctrl, cy2 = to.y;
            CubicCurve2D curve = new CubicCurve2D.Double(
                    from.x, from.y, cx1, cy1, cx2, cy2, to.x, to.y);
            // Selection glow
            if (isSelected) {
                g2.setColor(new Color(255, 80, 80, 45));
                g2.setStroke(new BasicStroke(8, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
                g2.draw(curve);
            }
            g2.setColor(color);
            g2.setStroke(new BasicStroke(2.2f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            g2.draw(curve);
            drawEndDot(g2, from.x, from.y, color);
            drawArrowDot(g2, cx2, cy2, to.x, to.y, color);
            // Label at curve midpoint
            double midX = (from.x + 3.0*cx1 + 3.0*cx2 + to.x) / 8.0;
            double midY = (from.y + 3.0*cy1 + 3.0*cy2 + to.y) / 8.0;
            drawConnLabel(g2, label, (int)midX, (int)midY, color);
        }

        /** Orthogonal (right-angle) routing for back-connections or blocked paths. */
        private void drawRoutedConnection(Graphics2D g2, Point from, Point to,
                FunctionBlock fromBlock, FunctionBlock toBlock,
                Color color, boolean isSelected, String label) {
            Rectangle fB = fromBlock.getBounds();
            Rectangle tB = toBlock.getBounds();
            int fTop    = (int)(fB.y / zoomFactor);
            int fBottom = (int)((fB.y + fB.height) / zoomFactor);
            int tTop    = (int)(tB.y / zoomFactor);
            int tBottom = (int)((tB.y + tB.height) / zoomFactor);
            int ox = 44, oy = 32, r = 14; // r = corner-rounding radius

            boolean routeAbove = from.y <= to.y;
            int exitX   = from.x + ox;
            int enterX  = to.x  - ox;
            int midY;
            if (from.x > to.x - 24) {
                midY = routeAbove
                        ? Math.min(fTop, tTop)   - oy
                        : Math.max(fBottom, tBottom) + oy;
                // Push midY further if it collides with any block
                for (FunctionBlock blk : functionBlocks) {
                    if (blk == fromBlock || blk == toBlock) continue;
                    Rectangle b = blk.getBounds();
                    int bTop = (int)(b.y / zoomFactor) - 12;
                    int bBot = (int)((b.y + b.height) / zoomFactor) + 12;
                    if (midY >= bTop && midY <= bBot) {
                        midY = routeAbove ? bTop - 16 : bBot + 16;
                    }
                }
            } else {
                midY = routeAbove
                        ? Math.min(fTop, tTop)   - oy
                        : Math.max(fBottom, tBottom) + oy;
            }

            // Build rounded-corner path
            Path2D path = new Path2D.Double();
            path.moveTo(from.x, from.y);
            // Segment 1: from → exit-X bend
            addRoundedTurn(path, from.x, from.y, exitX, from.y, exitX, midY, r);
            // Segment 2: exit-X → enter-X along midY
            addRoundedTurn(path, exitX, from.y, exitX, midY, enterX, midY, r);
            // Segment 3: enter-X bend → destination
            addRoundedTurn(path, exitX, midY, enterX, midY, enterX, to.y, r);
            addRoundedTurn(path, enterX, midY, enterX, to.y, to.x, to.y, r);
            path.lineTo(to.x, to.y);

            if (isSelected) {
                g2.setColor(new Color(255, 80, 80, 45));
                g2.setStroke(new BasicStroke(8, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
                g2.draw(path);
            }
            g2.setColor(color);
            g2.setStroke(new BasicStroke(2.2f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
            g2.draw(path);

            drawEndDot(g2, from.x, from.y, color);
            // Arrowhead enters horizontally from the left
            drawArrowDot(g2, enterX, to.y, to.x, to.y, color);

            // Label at midY between exitX and enterX
            int lx = (exitX + enterX) / 2, ly = midY;
            drawConnLabel(g2, label, lx, ly, color);
        }

        /** Adds a rounded 90° turn to a Path2D. The corner is at (bx,by);
         *  the path comes from direction (ax,ay)→(bx,by) and leaves toward (cx,cy). */
        private void addRoundedTurn(Path2D p, double ax, double ay,
                double bx, double by, double cx, double cy, int r) {
            // Clamp r so it doesn't exceed segment lengths
            double d1 = Math.sqrt((bx-ax)*(bx-ax)+(by-ay)*(by-ay));
            double d2 = Math.sqrt((cx-bx)*(cx-bx)+(cy-by)*(cy-by));
            int rc = (int)Math.min(r, Math.min(d1, d2) * 0.45);
            if (rc < 2) { p.lineTo(bx, by); return; }
            // Point on incoming segment, r before corner
            double t1 = 1 - rc / d1;
            double ix = ax + (bx-ax)*t1, iy = ay + (by-ay)*t1;
            // Point on outgoing segment, r after corner
            double t2 = rc / d2;
            double ox = bx + (cx-bx)*t2, oy2 = by + (cy-by)*t2;
            p.lineTo(ix, iy);
            p.curveTo(bx, by, bx, by, ox, oy2); // quadratic through corner
        }

        /** Filled source dot at connection start. */
        private void drawEndDot(Graphics2D g2, int x, int y, Color color) {
            int r = 5;
            g2.setColor(color);
            g2.fillOval(x - r, y - r, r*2, r*2);
            g2.setColor(new Color(255, 255, 255, 180));
            g2.setStroke(new BasicStroke(1.2f));
            g2.drawOval(x - r, y - r, r*2, r*2);
        }

        /** Filled arrowhead plus destination dot. */
        private void drawArrowDot(Graphics2D g2, int x1, int y1, int x2, int y2, Color color) {
            double angle = Math.atan2(y2 - y1, x2 - x1);
            int aLen = 11;
            int xA = (int)(x2 - aLen * Math.cos(angle - Math.PI/6));
            int yA = (int)(y2 - aLen * Math.sin(angle - Math.PI/6));
            int xB = (int)(x2 - aLen * Math.cos(angle + Math.PI/6));
            int yB = (int)(y2 - aLen * Math.sin(angle + Math.PI/6));
            g2.setColor(color);
            g2.fillPolygon(new int[]{x2,xA,xB}, new int[]{y2,yA,yB}, 3);
            // Small dot at destination
            int r = 4;
            g2.fillOval(x2 - r, y2 - r, r*2, r*2);
        }

        /** Pill-shaped label centred at (lx, ly). */
        private void drawConnLabel(Graphics2D g2, String fileName, int lx, int ly, Color color) {
            if (fileName == null || fileName.trim().isEmpty()) return;
            String text = fileName.trim();
            Font f = new Font("SansSerif", Font.BOLD, 10);
            g2.setFont(f);
            FontMetrics fm = g2.getFontMetrics();
            int tw = fm.stringWidth(text), th = fm.getAscent();
            int pad = 5;
            int rx = lx - tw/2 - pad, ry = ly - th/2 - pad;
            int rw = tw + pad*2,       rh = th + pad*2;
            g2.setColor(new Color(255, 255, 255, 230));
            g2.fillRoundRect(rx, ry, rw, rh, 10, 10);
            g2.setColor(color);
            g2.setStroke(new BasicStroke(1f));
            g2.drawRoundRect(rx, ry, rw, rh, 10, 10);
            g2.setColor(new Color(30, 30, 50));
            g2.drawString(text, lx - tw/2, ly + th/2 - 1);
        }

        // legacy stub kept so callers compile
        private void drawArrowHead(Graphics2D g2, int x1, int y1, int x2, int y2, Color color) {
            drawArrowDot(g2, x1, y1, x2, y2, color);
        }
    }
    Point getOutputPoint(FunctionBlock fb, int outputIndex) {
        if (fb == null) return new Point(0, 0);
        if (fb.outputArrows == null || outputIndex < 0 || outputIndex >= fb.outputArrows.length) {
            return new Point(fb.getX() + fb.getWidth(), fb.getY() + 30);
        }
        JLabel output = fb.outputArrows[outputIndex];
        if (output == null) {
            return new Point(fb.getX() + fb.getWidth(), fb.getY() + 30);
        }
        Point p = SwingUtilities.convertPoint(output, output.getWidth(), output.getHeight() / 2, drawingPanel);
        return new Point((int)(p.x / drawingPanel.zoomFactor), (int)(p.y / drawingPanel.zoomFactor));
    }
    Point getInputPoint(FunctionBlock fb, int inputIndex) {
        if (fb == null) return new Point(0, 0);
        if (inputIndex == -1) {
            Point p = new Point(fb.getX() + fb.getWidth() / 2, fb.getY() + fb.getHeight() / 2);
            return new Point((int)(p.x / drawingPanel.zoomFactor), (int)(p.y / drawingPanel.zoomFactor));
        }
        if (fb.inputArrows == null || inputIndex < 0 || inputIndex >= fb.inputArrows.length) {
            return new Point(fb.getX(), fb.getY() + 30);
        }
        JLabel input = fb.inputArrows[inputIndex];
        if (input == null) {
            return new Point(fb.getX(), fb.getY() + 30);
        }
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
                                "Circular dependency detected:\n" + String.join(" \u2192 ", cyclePath) +
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
        String fileName;  // NEW: file name for file-type connections
        Connection(FunctionBlock from, int fromIdx, FunctionBlock to, int toIdx, String type) {
            this.from = from;
            this.fromIdx = fromIdx;
            this.to = to;
            this.toIdx = toIdx;
            this.type = type;
            this.fileName = null;  // NEW: initialized to null
        }
    }
    class FunctionBlock extends JPanel {
        private static final long serialVersionUID = 1L;
        String name;
        String originalName;
        List<String> nameHistory;
        String[] inputValues;
        String[] outputVarNames; // user-set variable names for each output slot
        JButton[] outputDots;
        transient BlockTemplate template;
        public JLabel[] inputArrows;
        public JLabel[] outputArrows;
        private Component outputPanel;
        boolean isSelected = false;
        // Store reference to inputPanel for reliable badge refresh
        private JPanel inputPanel;
        // Header panel components (persisted across rebuildUI calls)
        private JPanel headerPanel;
        private JLabel headerNameLabel;
        private String getDefaultValue(String type) {
            if (type == null) return "default";
            String normalizedType = type.trim().toLowerCase();
            switch (normalizedType) {
                case "float": return "0.0";
                case "integer": return "0";
                case "int": return "0";
                case "string": return "default_string";
                case "file": return "";
                case "graph": return "default_graph";
                case "status": return "default_status";
                case "character": return "a";
                case "char": return "a";
                default: return "default";
            }
        }
        void updateBorder() {
            setBorder(BorderFactory.createLineBorder(
                isSelected ? Theme.PRIMARY : Theme.BORDER,
                isSelected ? 2 : 1
            ));
            if (headerNameLabel != null) headerNameLabel.setText(name);
            // Re-attach header panel if it was removed by rebuildUI → removeAll()
            if (headerPanel != null && headerPanel.getParent() != this) {
                add(headerPanel, BorderLayout.NORTH);
            }
        }
        /** Public wrapper so FileEntryConnection code can call it. */
        public String getDefaultValuePublic(String type) { return getDefaultValue(type); }
        /**
         * Rebuilds the input slot panels to show/hide file-entry badges.
         * Called after a FileEntryConnection is added or deleted.
         */
        public void refreshInputBadges() {
            if (inputPanel == null) return;
            inputPanel.removeAll();
            for (int i = 0; i < template.inputCount; i++) {
                final int idx = i;
                JPanel inSlot = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
                inSlot.setOpaque(false);
                // Check for block-to-block CONN connection on this input
                Connection connIn = null;
                for (Connection c : connections) {
                    if (c.to == FunctionBlock.this && c.toIdx == idx) { connIn = c; break; }
                }
                // Check for file-entry connection on this input
                FileEntryConnection fec = null;
                for (FileEntryConnection f : fileEntryConnections) {
                    if (f.toBlock == this && f.toInputIndex == idx) { fec = f; break; }
                }
                if (connIn != null) {
                    // Resolve variable name: user-set wins, then outputVarNames, then auto-name
                    String fromBase = connIn.from.originalName.replaceAll("_\\d+$", "");
                    String connName;
                    if (connIn.fileName != null && !connIn.fileName.trim().isEmpty()) {
                        connName = connIn.fileName.trim();
                    } else if (connIn.from.outputVarNames != null
                            && connIn.fromIdx < connIn.from.outputVarNames.length
                            && connIn.from.outputVarNames[connIn.fromIdx] != null
                            && !connIn.from.outputVarNames[connIn.fromIdx].trim().isEmpty()) {
                        connName = connIn.from.outputVarNames[connIn.fromIdx].trim();
                    } else {
                        connName = fromBase + "_output" + (connIn.fromIdx + 1);
                    }
                    String outType = (connIn.from.template != null
                            && connIn.from.template.outputTypes != null
                            && connIn.fromIdx < connIn.from.template.outputTypes.length)
                            ? connIn.from.template.outputTypes[connIn.fromIdx] : "var";
                    Color badgeColor = getFilePanelTypeColor(outType);
                    JLabel badge = new JLabel(connName);
                    badge.setFont(new Font("SansSerif", Font.BOLD, 10));
                    badge.setForeground(Color.WHITE);
                    badge.setOpaque(true);
                    badge.setBackground(badgeColor);
                    badge.setBorder(BorderFactory.createEmptyBorder(2, 6, 2, 6));
                    badge.setToolTipText("<html><b>" + connIn.from.name
                            + " → output " + (connIn.fromIdx + 1) + "</b><br>$" + connName + "</html>");
                    JLabel inArrow = inputArrows[idx];
                    if (inArrow != null) inSlot.add(inArrow);
                    inSlot.add(badge);
                } else if (fec != null) {
                    Color badgeColor = getFilePanelTypeColor(fec.entryType);
                    String badgeText = "[" + fec.entryPosition + "]";
                    JLabel badge = new JLabel(badgeText);
                    badge.setFont(new Font("SansSerif", Font.BOLD, 11));
                    badge.setForeground(Color.WHITE);
                    badge.setOpaque(true);
                    badge.setBackground(badgeColor);
                    badge.setBorder(BorderFactory.createEmptyBorder(2, 6, 2, 6));
                    badge.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                    String valShort = fec.entryValue != null && fec.entryValue.length() > 24
                        ? fec.entryValue.substring(0, 22) + "…" : fec.entryValue;
                    badge.setToolTipText("<html><b>[" + fec.entryPosition + "] " + fec.entryName + "</b><br>" +
                        "Type: " + fec.entryType + "<br>Val: " + valShort + "<br>" +
                        "File: <i>" + (fec.fileName != null && !fec.fileName.isEmpty() ? fec.fileName : "—") + "</i>" +
                        "<br><font color='red'>Right-click → Delete / Edit Name</font></html>");
                    final FileEntryConnection fecFinal = fec;
                    JPopupMenu bMenu = new JPopupMenu();
                    JMenuItem bDel = new JMenuItem("\uD83D\uDDD1 Delete Connection");
                    bDel.addActionListener(e -> {
                        fileEntryConnections.remove(fecFinal);
                        inputValues[idx] = getDefaultValue(template.inputTypes[idx]);
                        refreshInputBadges();
                        loadAndDisplayInputFile();
                    });
                    bMenu.add(bDel);
                    bMenu.addSeparator();
                    JMenuItem bDelInput = new JMenuItem("\u2702 Delete This Input");
                    bDelInput.addActionListener(e -> deleteInput(idx));
                    bMenu.add(bDelInput);
                    JMenuItem bEdit = new JMenuItem("\u270F Edit File Name");
                    bEdit.addActionListener(e -> {
                        String cur = fecFinal.fileName != null ? fecFinal.fileName : "";
                        String nv = JOptionPane.showInputDialog(gui4.this,
                            "<html>Edit the file name for this connection:<br><i>(default: entryName_blockName_inputN)</i></html>",
                            cur);
                        if (nv != null) {
                            fecFinal.fileName = nv.trim();
                            refreshInputBadges();
                            loadAndDisplayInputFile();
                        }
                    });
                    bMenu.add(bEdit);
                    badge.setComponentPopupMenu(bMenu);
                    // Keep the original dot visible alongside the badge
                    JLabel inArrow = inputArrows[idx];
                    if (inArrow != null) inSlot.add(inArrow);
                    inSlot.add(badge);
                } else {
                    JLabel inArrow = inputArrows[idx];
                    if (inArrow != null) inSlot.add(inArrow);
                    // Show an orange badge if a direct value has been entered
                    String directVal = inputValues[idx];
                    String defaultVal = getDefaultValue(template.inputTypes[idx]);
                    boolean hasDirectValue = directVal != null && !directVal.isEmpty()
                            && !directVal.equals(defaultVal);
                    if (hasDirectValue) {
                        String display = directVal.length() > 14
                                ? directVal.substring(0, 12) + "…" : directVal;
                        JLabel valBadge = new JLabel(display);
                        valBadge.setFont(new Font("SansSerif", Font.BOLD, 11));
                        valBadge.setForeground(Color.WHITE);
                        valBadge.setOpaque(true);
                        valBadge.setBackground(Theme.WARNING_DK);
                        valBadge.setBorder(BorderFactory.createEmptyBorder(2, 6, 2, 6));
                        valBadge.setToolTipText("<html><b>Direct value</b><br>"
                                + "Type: " + template.inputTypes[idx]
                                + "<br>Value: " + directVal
                                + "<br><font color='#FBBF24'>Right-click to edit or clear</font></html>");
                        inSlot.add(valBadge);
                    }
                    // Right-click handler shared by both the slot panel and the arrow label
                    // so the menu fires whether the user clicks the dot or the surrounding area.
                    MouseAdapter slotHandler = new MouseAdapter() {
                        public void mouseReleased(MouseEvent e) {
                            if (!SwingUtilities.isRightMouseButton(e)) return;
                            JPopupMenu pm = new JPopupMenu();
                            String type = template.inputTypes[idx].trim().toLowerCase();
                            // ── Set Value ──────────────────────────────────
                            JMenuItem setItem = new JMenuItem(
                                    "✏ Set Value  [" + template.inputTypes[idx] + "]");
                            setItem.addActionListener(ev -> {
                                String newVal = null;
                                if (type.equals("file")) {
                                    JFileChooser chooser = new JFileChooser();
                                    String cur = inputValues[idx];
                                    if (cur != null && !cur.isEmpty()) {
                                        java.io.File f = new java.io.File(cur);
                                        if (f.getParentFile() != null) chooser.setCurrentDirectory(f.getParentFile());
                                    }
                                    if (chooser.showOpenDialog(FunctionBlock.this) == JFileChooser.APPROVE_OPTION) {
                                        newVal = chooser.getSelectedFile().getAbsolutePath();
                                    }
                                } else if (type.equals("integer") || type.equals("int")) {
                                    String raw = JOptionPane.showInputDialog(gui4.this,
                                            "Enter integer value for input " + (idx + 1) + ":",
                                            inputValues[idx]);
                                    if (raw != null) {
                                        try { Integer.parseInt(raw.trim()); newVal = raw.trim(); }
                                        catch (NumberFormatException ex2) {
                                            JOptionPane.showMessageDialog(gui4.this,
                                                    "\"" + raw + "\" is not a valid integer.",
                                                    "Invalid Input", JOptionPane.ERROR_MESSAGE);
                                        }
                                    }
                                } else if (type.equals("float") || type.equals("double")) {
                                    String raw = JOptionPane.showInputDialog(gui4.this,
                                            "Enter float value for input " + (idx + 1) + ":",
                                            inputValues[idx]);
                                    if (raw != null) {
                                        try { Double.parseDouble(raw.trim()); newVal = raw.trim(); }
                                        catch (NumberFormatException ex2) {
                                            JOptionPane.showMessageDialog(gui4.this,
                                                    "\"" + raw + "\" is not a valid number.",
                                                    "Invalid Input", JOptionPane.ERROR_MESSAGE);
                                        }
                                    }
                                } else {
                                    String raw = JOptionPane.showInputDialog(gui4.this,
                                            "Enter " + template.inputTypes[idx]
                                            + " value for input " + (idx + 1) + ":",
                                            inputValues[idx]);
                                    if (raw != null) newVal = raw.trim();
                                }
                                if (newVal != null && !newVal.isEmpty()) {
                                    inputValues[idx] = newVal;
                                    refreshInputBadges();
                                }
                            });
                            pm.add(setItem);
                            // ── Clear Value (only when a direct value exists) ──
                            String curVal = inputValues[idx];
                            String defVal = getDefaultValue(template.inputTypes[idx]);
                            if (curVal != null && !curVal.isEmpty() && !curVal.equals(defVal)) {
                                JMenuItem clearItem = new JMenuItem("✖ Clear Value");
                                clearItem.addActionListener(ev -> {
                                    inputValues[idx] = defVal;
                                    refreshInputBadges();
                                });
                                pm.add(clearItem);
                            }
                            pm.addSeparator();
                            // ── Delete Input ───────────────────────────────
                            JMenuItem delItem = new JMenuItem("✂ Delete Input " + (idx + 1)
                                    + "  [" + template.inputTypes[idx] + "]");
                            delItem.addActionListener(ev -> deleteInput(idx));
                            pm.add(delItem);
                            // show at the actual clicked component so coords are correct
                            pm.show(e.getComponent(), e.getX(), e.getY());
                        }
                    };
                    inSlot.addMouseListener(slotHandler);
                    if (inArrow != null) inArrow.addMouseListener(slotHandler);
                }
                inputPanel.add(inSlot);
            }
            inputPanel.revalidate();
            inputPanel.repaint();
            recomputeSize();
        }
        /** Removes the input at position idx, shifts remaining inputs and connections down. */
        void deleteInput(int idx) {
            if (template.inputCount <= 1) {
                JOptionPane.showMessageDialog(gui4.this,
                    "Cannot delete — block must have at least 1 input.",
                    "Cannot Delete", JOptionPane.WARNING_MESSAGE);
                return;
            }
            // Remove block-to-block connections targeting this input; shift others
            connections.removeIf(c -> c.to == FunctionBlock.this && c.toIdx == idx);
            for (Connection c : connections) {
                if (c.to == FunctionBlock.this && c.toIdx > idx) c.toIdx--;
            }
            // Remove file-entry connections targeting this input; shift others
            fileEntryConnections.removeIf(f -> f.toBlock == FunctionBlock.this && f.toInputIndex == idx);
            for (FileEntryConnection f : fileEntryConnections) {
                if (f.toBlock == FunctionBlock.this && f.toInputIndex > idx) f.toInputIndex--;
            }
            // Build compacted arrays (skip index idx)
            int n = template.inputCount - 1;
            String[] newTypes  = new String[n];
            String[] newVals   = new String[n];
            JLabel[] newArrows = new JLabel[n];
            for (int i = 0, j = 0; i < template.inputCount; i++) {
                if (i == idx) continue;
                newTypes[j]  = template.inputTypes[i];
                newVals[j]   = inputValues[i];
                newArrows[j] = inputArrows[i];
                j++;
            }
            // Replace template (keep same name/outputs, one fewer input)
            template = new BlockTemplate(template.name, n, template.outputCount, newTypes, template.outputTypes);
            inputValues = newVals;
            inputArrows = newArrows;
            // Rebuild input panel with updated slot count
            inputPanel.setLayout(new GridLayout(n, 1, 0, 8));
            refreshInputBadges();
            recomputeSize();
            if (drawingPanel != null) drawingPanel.repaint();
        }
        /** Deletes all inputs that have no block-to-block wire and no file-entry connection. */
        void deleteUnconnectedInputs() {
            // Collect which inputs ARE connected
            java.util.Set<Integer> connected = new java.util.HashSet<>();
            for (Connection c : connections) {
                if (c.to == FunctionBlock.this) connected.add(c.toIdx);
            }
            for (FileEntryConnection f : fileEntryConnections) {
                if (f.toBlock == FunctionBlock.this) connected.add(f.toInputIndex);
            }
            // Count unconnected inputs to delete
            int unconnectedCount = 0;
            for (int i = 0; i < template.inputCount; i++) {
                if (!connected.contains(i)) unconnectedCount++;
            }
            if (unconnectedCount == 0) {
                JOptionPane.showMessageDialog(gui4.this,
                    "All inputs are connected — nothing to delete.",
                    "No Unconnected Inputs", JOptionPane.INFORMATION_MESSAGE);
                return;
            }
            if (template.inputCount - unconnectedCount < 1) {
                JOptionPane.showMessageDialog(gui4.this,
                    "Cannot delete — block must keep at least 1 input.",
                    "Cannot Delete", JOptionPane.WARNING_MESSAGE);
                return;
            }
            int confirm = JOptionPane.showConfirmDialog(gui4.this,
                "Delete " + unconnectedCount + " unconnected input(s) from '" + name + "'?",
                "Delete Unconnected Inputs", JOptionPane.YES_NO_OPTION);
            if (confirm != JOptionPane.YES_OPTION) return;
            // Delete from highest index to lowest so indices stay valid
            for (int i = template.inputCount - 1; i >= 0; i--) {
                if (!connected.contains(i)) deleteInput(i);
                // Rebuild connected set after each deletion (indices shift)
                connected.clear();
                for (Connection c : connections) {
                    if (c.to == FunctionBlock.this) connected.add(c.toIdx);
                }
                for (FileEntryConnection f : fileEntryConnections) {
                    if (f.toBlock == FunctionBlock.this) connected.add(f.toInputIndex);
                }
            }
        }
        FunctionBlock(String name, BlockTemplate template) {
            super();
            this.name = name;
            this.originalName = name;
            this.nameHistory = new ArrayList<>();
            this.nameHistory.add(name);
            this.template = template;
            setLayout(new BorderLayout(8, 0));
            setBorder(BorderFactory.createLineBorder(Theme.BORDER, 1));
            setBackground(Theme.BG_CARD);
            setOpaque(true);

            // ── Header panel ──────────────────────────────────────────────────
            String[] desc = getBlockDescription(name);
            headerNameLabel = new JLabel(name);
            headerNameLabel.setFont(Theme.monoBold(11));
            headerNameLabel.setForeground(Theme.TEXT_DARK);

            JLabel dotLabel = new JLabel("◈ ");
            dotLabel.setForeground(Theme.PRIMARY);
            dotLabel.setFont(new Font("SansSerif", Font.BOLD, 9));

            JLabel ioLabel = new JLabel(" " + template.inputCount + "↓ " + template.outputCount + "↑ ");
            ioLabel.setFont(new Font("SansSerif", Font.PLAIN, 9));
            ioLabel.setForeground(Theme.TEXT_LIGHT);

            JLabel descLabel = new JLabel("<html><i>" + desc[0] + "</i></html>");
            descLabel.setFont(new Font("SansSerif", Font.PLAIN, 9));
            descLabel.setForeground(Theme.TEXT_MED);

            JPanel nameRow = new JPanel(new FlowLayout(FlowLayout.LEFT, 2, 0));
            nameRow.setOpaque(false);
            nameRow.add(dotLabel);
            nameRow.add(headerNameLabel);

            JPanel headerTop = new JPanel(new BorderLayout(2, 0));
            headerTop.setOpaque(false);
            headerTop.add(nameRow, BorderLayout.CENTER);
            headerTop.add(ioLabel, BorderLayout.EAST);

            headerPanel = new JPanel();
            headerPanel.setLayout(new BoxLayout(headerPanel, BoxLayout.Y_AXIS));
            headerPanel.setBackground(Theme.CARD_HI);
            headerPanel.setBorder(BorderFactory.createCompoundBorder(
                BorderFactory.createMatteBorder(0, 0, 1, 0, Theme.BORDER),
                BorderFactory.createEmptyBorder(4, 6, 3, 6)
            ));
            headerPanel.setOpaque(true);
            headerPanel.add(headerTop);
            headerPanel.add(descLabel);

            // Allow dragging and selection via header panel
            final Point[] hdrOffset = { null };
            headerPanel.addMouseListener(new MouseAdapter() {
                public void mousePressed(MouseEvent e) {
                    hdrOffset[0] = SwingUtilities.convertPoint(headerPanel, e.getPoint(), FunctionBlock.this);
                    for (FunctionBlock fb : functionBlocks) { fb.isSelected = false; fb.updateBorder(); }
                    isSelected = true;
                    updateBorder();
                }
            });
            headerPanel.addMouseMotionListener(new MouseMotionAdapter() {
                public void mouseDragged(MouseEvent e) {
                    if (hdrOffset[0] == null) return;
                    Point parent = SwingUtilities.convertPoint(headerPanel, e.getPoint(), drawingPanel);
                    int newX = parent.x - hdrOffset[0].x;
                    int newY = parent.y - hdrOffset[0].y;
                    FunctionBlock.this.setLocation(newX, newY);
                    updateCanvasSize();
                    drawingPanel.repaint();
                }
            });
            add(headerPanel, BorderLayout.NORTH);
            
            setToolTipText("<html><b>" + desc[0] + "</b><br><br>" + 
                desc[1] + "</i><br><br>" +
                "<b>Inputs:</b> " + template.inputCount + " | <b>Outputs:</b> " + template.outputCount + 
                "<br><br><font color='blue'>Right-click for options | Ctrl+C to copy</font></html>");
            inputValues = new String[template.inputCount];
            outputVarNames = new String[template.outputCount];
            inputPanel = new JPanel();
            inputPanel.setLayout(new GridLayout(template.inputCount, 1, 0, 8));
            inputPanel.setOpaque(false);
            inputArrows = new JLabel[template.inputCount];
            for (int i = 0; i < template.inputCount; i++) {
                inputValues[i] = getDefaultValue(template.inputTypes[i]);
                final int idx = i;
                // inSlot wraps the arrow — also handles click events for file-entry connection
                JPanel inSlot = new JPanel(new FlowLayout(FlowLayout.LEFT, 0, 0));
                inSlot.setOpaque(false);
                JLabel inArrow = new JLabel("\u25CF\u2192");
                inArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                inArrow.setForeground(getTypeColor(template.inputTypes[i]));
                inArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                inArrow.setToolTipText("<html><b>Input " + (idx + 1) + "</b><br>Type: " + template.inputTypes[idx] + "</html>");
                inArrow.setBorder(BorderFactory.createEmptyBorder(2, 4, 2, 4));
                // Shared handler for both file-entry mode and normal drag-connect mode
                MouseAdapter inputHandler = new MouseAdapter() {
                    public void mouseReleased(MouseEvent e) {
                        // === File-entry connection mode first ===
                        if (pendingEntryName != null) {
                            String inType = template.inputTypes[idx];
                            if (!typesCompatible(inType, pendingEntryType)) {
                                JOptionPane.showMessageDialog(gui4.this,
                                    "Type mismatch: file entry is '" + pendingEntryType +
                                    "' but input expects '" + inType + "'",
                                    "Type Mismatch", JOptionPane.ERROR_MESSAGE);
                                pendingEntryName = pendingEntryType = pendingEntryValue = null;
                                drawingPanel.setCursor(Cursor.getDefaultCursor());
                                return;
                            }
                            // Already file-connected?
                            for (FileEntryConnection ex : fileEntryConnections) {
                                if (ex.toBlock == FunctionBlock.this && ex.toInputIndex == idx) {
                                    JOptionPane.showMessageDialog(gui4.this,
                                        "Input " + (idx+1) + " of '" + name + "' already has a file-entry connection.",
                                        "Already Connected", JOptionPane.WARNING_MESSAGE);
                                    pendingEntryName = pendingEntryType = pendingEntryValue = null;
                                    drawingPanel.setCursor(Cursor.getDefaultCursor());
                                    return;
                                }
                            }
                            // Already block-connected?
                            for (Connection conn : connections) {
                                if (conn.to == FunctionBlock.this && conn.toIdx == idx) {
                                    JOptionPane.showMessageDialog(gui4.this,
                                        "Input " + (idx+1) + " of '" + name + "' is already wired to a block output.",
                                        "Already Connected", JOptionPane.WARNING_MESSAGE);
                                    pendingEntryName = pendingEntryType = pendingEntryValue = null;
                                    drawingPanel.setCursor(Cursor.getDefaultCursor());
                                    return;
                                }
                            }
                            FileEntryConnection fec = new FileEntryConnection(
                                pendingEntryName, pendingEntryType, pendingEntryValue,
                                pendingEntryPosition, FunctionBlock.this, idx);
                            fileEntryConnections.add(fec);
                            inputValues[idx] = pendingEntryValue;
                            pendingEntryName = pendingEntryType = pendingEntryValue = null;
                            pendingEntryPosition = 0;
                            drawingPanel.setCursor(Cursor.getDefaultCursor());
                            refreshInputBadges();
                            loadAndDisplayInputFile();
                            return;
                        }
                        // === Normal block-output drag connection ===
                        if (dragSource != null && dragSourceOutputIndex >= 0) {
                            String outType = dragSource.template.outputTypes[dragSourceOutputIndex];
                            String inType  = template.inputTypes[idx];
                            if (!typesCompatible(outType, inType)) {
                                JOptionPane.showMessageDialog(gui4.this,
                                    "Type mismatch: Cannot connect output (" + outType + ") to input (" + inType + ")",
                                    "Type Mismatch", JOptionPane.ERROR_MESSAGE);
                                dragSource = null; dragSourceOutputIndex = -1;
                                return;
                            }
                            for (Connection conn : connections) {
                                if (conn.to == FunctionBlock.this && conn.toIdx == idx) {
                                    JOptionPane.showMessageDialog(gui4.this,
                                        "Input " + (idx + 1) + " of block " + name + " is already connected.",
                                        "Connection Error", JOptionPane.ERROR_MESSAGE);
                                    dragSource = null; dragSourceOutputIndex = -1;
                                    return;
                                }
                            }
                            connections.add(new Connection(dragSource, dragSourceOutputIndex, FunctionBlock.this, idx, inType));
                            dragSource = null; dragSourceOutputIndex = -1;
                            drawingPanel.setCursor(Cursor.getDefaultCursor());
                            refreshInputBadges();
                            drawingPanel.repaint();
                        }
                    }
                    public void mouseEntered(MouseEvent e) { inArrow.setFont(new Font("SansSerif", Font.BOLD, 16)); }
                    public void mouseExited(MouseEvent e)  { inArrow.setFont(new Font("SansSerif", Font.BOLD, 14)); }
                };
                inArrow.addMouseListener(inputHandler);
                inSlot.addMouseListener(inputHandler);   // also listen on the slot panel itself
                inputArrows[i] = inArrow;
                inSlot.add(inArrow);
                inputPanel.add(inSlot);
            }
            JPanel outputPanel = new JPanel(new GridLayout(template.outputCount, 1, 0, 8));
            outputPanel.setOpaque(false);
            outputArrows = new JLabel[template.outputCount];
            for (int j = 0; j < template.outputCount; j++) {
                final int oidx = j;
                JLabel outArrow = new JLabel("\u2192\u25CF");
                outArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                outArrow.setForeground(getTypeColor(template.outputTypes[j]));
                outArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                outArrow.setToolTipText("<html><b>Output " + (oidx + 1) + "</b><br>Type: " + template.outputTypes[oidx] + "<br><i>Left-click: start connection | Right-click: name this output</i></html>");
                outArrow.addMouseListener(new MouseAdapter() {
                    public void mouseClicked(MouseEvent e) {
                        if (SwingUtilities.isRightMouseButton(e)) {
                            // Right-click: set output variable name
                            JPopupMenu pm = new JPopupMenu();
                            String cur = (outputVarNames != null && oidx < outputVarNames.length && outputVarNames[oidx] != null)
                                    ? outputVarNames[oidx] : "";
                            JMenuItem nameItem = new JMenuItem("🔖 Set Output Variable Name");
                            nameItem.addActionListener(ev -> {
                                JTextField tf = new JTextField(cur, 20);
                                tf.addHierarchyListener(he -> {
                                    if ((he.getChangeFlags() & java.awt.event.HierarchyEvent.SHOWING_CHANGED) != 0 && tf.isShowing())
                                        SwingUtilities.invokeLater(tf::requestFocusInWindow);
                                });
                                JButton pasteBtn = new JButton("Paste");
                                pasteBtn.setMargin(new Insets(2, 8, 2, 8));
                                pasteBtn.addActionListener(pev -> {
                                    try {
                                        java.awt.datatransfer.Clipboard cb = java.awt.Toolkit.getDefaultToolkit().getSystemClipboard();
                                        String clip = (String) cb.getData(java.awt.datatransfer.DataFlavor.stringFlavor);
                                        if (clip != null) tf.setText(clip.trim());
                                    } catch (Exception ex) { /* clipboard empty or not text */ }
                                    tf.requestFocusInWindow();
                                });
                                JPanel p = new JPanel(new java.awt.GridBagLayout());
                                java.awt.GridBagConstraints gbc = new java.awt.GridBagConstraints();
                                gbc.insets = new Insets(4, 4, 4, 4);
                                gbc.gridx = 0; gbc.gridy = 0; gbc.gridwidth = 2; gbc.anchor = java.awt.GridBagConstraints.WEST;
                                p.add(new JLabel("<html>Variable name for Output " + (oidx+1) + ":<br><i>Will appear as $NAME in generated script</i></html>"), gbc);
                                gbc.gridy = 1; gbc.gridwidth = 1; gbc.fill = java.awt.GridBagConstraints.HORIZONTAL; gbc.weightx = 1.0;
                                p.add(tf, gbc);
                                gbc.gridx = 1; gbc.weightx = 0; gbc.fill = java.awt.GridBagConstraints.NONE;
                                p.add(pasteBtn, gbc);
                                int res = JOptionPane.showConfirmDialog(gui4.this, p,
                                        "Name Output " + (oidx+1) + " of '" + name + "'",
                                        JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
                                if (res == JOptionPane.OK_OPTION) {
                                    String v = tf.getText().trim().toUpperCase().replace(" ", "_").replace("-", "_");
                                    outputVarNames[oidx] = v.isEmpty() ? null : v;
                                    // Update tooltip to show current name
                                    String varDisplay = (outputVarNames[oidx] != null) ? "  [$" + outputVarNames[oidx] + "]" : "";
                                    outArrow.setToolTipText("<html><b>Output " + (oidx+1) + "</b>" + varDisplay + "<br>Type: " + template.outputTypes[oidx] + "<br><i>Left-click: start connection | Right-click: name this output</i></html>");
                                    // Propagate name to any existing outgoing connection
                                    for (Connection c : connections) {
                                        if (c.from == FunctionBlock.this && c.fromIdx == oidx && (c.fileName == null || c.fileName.trim().isEmpty())) {
                                            c.fileName = outputVarNames[oidx];
                                        }
                                    }
                                    drawingPanel.repaint();
                                }
                            });
                            pm.add(nameItem);
                            if (!cur.isEmpty()) {
                                pm.addSeparator();
                                JMenuItem showItem = new JMenuItem("  →  $" + cur);
                                showItem.setEnabled(false);
                                pm.add(showItem);
                                JMenuItem clearItem = new JMenuItem("❌ Clear name");
                                clearItem.addActionListener(ev -> {
                                    outputVarNames[oidx] = null;
                                    outArrow.setToolTipText("<html><b>Output " + (oidx+1) + "</b><br>Type: " + template.outputTypes[oidx] + "<br><i>Left-click: start connection | Right-click: name this output</i></html>");
                                    drawingPanel.repaint();
                                });
                                pm.add(clearItem);
                            }
                            pm.show(outArrow, e.getX(), e.getY());
                            return;
                        }
                        // Left-click: start drag connection
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
            addMouseListener(new MouseAdapter() {
                public void mousePressed(MouseEvent e) {
                    for (FunctionBlock fb : functionBlocks) {
                        fb.isSelected = false;
                        fb.updateBorder();
                    }
                    isSelected = true;
                    updateBorder();
                }
                public void mouseReleased(MouseEvent e) {
                    // === FILE-ENTRY CONNECTION MODE: highest priority ===
                    if (pendingEntryName != null && !SwingUtilities.isRightMouseButton(e)) {
                        connectFileEntryToBlock(FunctionBlock.this);
                        return;
                    }
                    // === Normal status-output drag connection ===
                    if (dragSource != null && dragSourceOutputIndex >= 0) {
                        boolean isStatusOutput = false;
                        if (dragSource.template != null &&
                            dragSourceOutputIndex < dragSource.template.outputTypes.length) {
                            String outType = dragSource.template.outputTypes[dragSourceOutputIndex];
                            isStatusOutput = "Status".equalsIgnoreCase(outType != null ? outType.trim() : "");
                        }
                        if (isStatusOutput) {
                            boolean alreadyConnected = false;
                            for (Connection conn : connections) {
                                if (conn.from == dragSource && conn.to == FunctionBlock.this && conn.toIdx == -1) {
                                    alreadyConnected = true; break;
                                }
                            }
                            if (!alreadyConnected) {
                                connections.add(new Connection(dragSource, dragSourceOutputIndex, FunctionBlock.this, -1, "Status"));
                                dragSource = null; dragSourceOutputIndex = -1;
                                drawingPanel.setCursor(Cursor.getDefaultCursor());
                                drawingPanel.repaint();
                            } else {
                                JOptionPane.showMessageDialog(gui4.this, "Status connection already exists.");
                            }
                        }
                    }
                }
            });
            JPopupMenu menu = new JPopupMenu();
            
            JMenuItem viewInfo = new JMenuItem("\uD83D\uDCCB View Block Info");
            viewInfo.addActionListener(e -> showBlockInfo());
            menu.add(viewInfo);
            
            JMenuItem editDesc = new JMenuItem("\u270F\uFE0F Edit Block Description");
            editDesc.addActionListener(e -> showEditBlockDescriptionDialog(template.name));
            menu.add(editDesc);
            
            menu.addSeparator();
            
            JMenuItem copy = new JMenuItem("\uD83D\uDCC4 Copy (Ctrl+C)");
            copy.addActionListener(e -> {
                clipboardBlock = this;
                JOptionPane.showMessageDialog(gui4.this, "Block copied to clipboard!");
            });
            menu.add(copy);
            
            JMenuItem duplicate = new JMenuItem("\uD83D\uDCCB Duplicate (Ctrl+D)");
            duplicate.addActionListener(e -> {
                clipboardBlock = this;
                pasteBlock();
            });
            menu.add(duplicate);
            
            menu.addSeparator();
            
            JMenuItem rename = new JMenuItem("\u270F\uFE0F Rename Instance");
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
            JMenuItem changeInputs = new JMenuItem("\uD83D\uDD27 Change Input Count");
            changeInputs.addActionListener(e -> {
                try {
                    String countStr = JOptionPane.showInputDialog(gui4.this, "Enter new input count:", template.inputCount);
                    if (countStr == null) return;
                    int newCount = Integer.parseInt(countStr.trim());
                    if (newCount >= 0 && newCount != template.inputCount) {
                        connections.removeIf(conn -> conn.to == FunctionBlock.this);
                        fileEntryConnections.removeIf(fec -> fec.toBlock == FunctionBlock.this);
                        
                        String[] newInputTypes = new String[newCount];
                        for (int i = 0; i < newCount && i < template.inputTypes.length; i++) {
                            newInputTypes[i] = template.inputTypes[i];
                        }
                        for (int i = template.inputTypes.length; i < newCount; i++) {
                            newInputTypes[i] = "float";
                        }
                        
                        // Ask for data types of all inputs
                        if (newCount > 0) {
                            String[] types = {"float", "string", "file", "graph", "integer", "Status", "char"};
                            JPanel typePanel = new JPanel(new GridLayout(newCount, 2, 5, 5));
                            typePanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                            @SuppressWarnings("unchecked")
                            JComboBox<String>[] combos = new JComboBox[newCount];
                            for (int i = 0; i < newCount; i++) {
                                typePanel.add(new JLabel("Input " + (i + 1) + ":"));
                                JComboBox<String> cb = new JComboBox<>(types);
                                cb.setSelectedItem(newInputTypes[i]);
                                combos[i] = cb;
                                typePanel.add(cb);
                            }
                            int typeRes = JOptionPane.showConfirmDialog(gui4.this, typePanel,
                                "Set Input Types for " + name, JOptionPane.OK_CANCEL_OPTION);
                            if (typeRes == JOptionPane.OK_OPTION) {
                                for (int i = 0; i < newCount; i++) {
                                    newInputTypes[i] = (String) combos[i].getSelectedItem();
                                }
                            }
                        }
                        
                        template.inputCount = newCount;
                        template.inputTypes = newInputTypes;
                        template.defaultValues = new String[newCount];
                        for (int i = 0; i < newCount; i++) {
                            template.defaultValues[i] = getDefaultValue(newInputTypes[i]);
                        }
                        
                        rebuildUI();
                        loadAndDisplayInputFile();
                    }
                } catch (NumberFormatException ignored) {}
            });
            menu.add(changeInputs);

            JMenuItem changeOutputs = new JMenuItem("\uD83D\uDD27 Change Output Count");
            changeOutputs.addActionListener(e -> {
                try {
                    String countStr = JOptionPane.showInputDialog(gui4.this, "Enter new output count:", template.outputCount);
                    if (countStr == null) return;
                    int newCount = Integer.parseInt(countStr.trim());
                    if (newCount >= 1 && newCount != template.outputCount) {
                        connections.removeIf(conn -> conn.from == FunctionBlock.this);
                        
                        String[] newOutputTypes = new String[newCount];
                        for (int i = 0; i < newCount && i < template.outputTypes.length; i++) {
                            newOutputTypes[i] = template.outputTypes[i];
                        }
                        for (int i = template.outputTypes.length; i < newCount; i++) {
                            newOutputTypes[i] = "float";
                        }
                        
                        // Ask for data types of all outputs
                        String[] types = {"float", "string", "file", "graph", "integer", "Status", "char"};
                        JPanel typePanel = new JPanel(new GridLayout(newCount, 2, 5, 5));
                        typePanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                        @SuppressWarnings("unchecked")
                        JComboBox<String>[] combos = new JComboBox[newCount];
                        for (int i = 0; i < newCount; i++) {
                            typePanel.add(new JLabel("Output " + (i + 1) + ":"));
                            JComboBox<String> cb = new JComboBox<>(types);
                            cb.setSelectedItem(newOutputTypes[i]);
                            combos[i] = cb;
                            typePanel.add(cb);
                        }
                        int typeRes = JOptionPane.showConfirmDialog(gui4.this, typePanel,
                            "Set Output Types for " + name, JOptionPane.OK_CANCEL_OPTION);
                        if (typeRes == JOptionPane.OK_OPTION) {
                            for (int i = 0; i < newCount; i++) {
                                newOutputTypes[i] = (String) combos[i].getSelectedItem();
                            }
                        }
                        
                        template.outputCount = newCount;
                        template.outputTypes = newOutputTypes;
                        
                        rebuildUI();
                    }
                } catch (NumberFormatException ignored) {}
            });
            menu.add(changeOutputs);

            JMenuItem editInputTypes = new JMenuItem("\uD83C\uDFA8 Edit Input Data Types");
            editInputTypes.addActionListener(e -> {
                if (template.inputCount == 0) {
                    JOptionPane.showMessageDialog(gui4.this, "This block has no inputs.", "No Inputs", JOptionPane.INFORMATION_MESSAGE);
                    return;
                }
                String[] types = {"float", "string", "file", "graph", "integer", "Status", "char"};
                JPanel typePanel = new JPanel(new GridLayout(template.inputCount, 2, 5, 5));
                typePanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                @SuppressWarnings("unchecked")
                JComboBox<String>[] combos = new JComboBox[template.inputCount];
                for (int i = 0; i < template.inputCount; i++) {
                    typePanel.add(new JLabel("Input " + (i + 1) + ":"));
                    JComboBox<String> cb = new JComboBox<>(types);
                    cb.setSelectedItem(template.inputTypes[i]);
                    combos[i] = cb;
                    typePanel.add(cb);
                }
                int typeRes = JOptionPane.showConfirmDialog(gui4.this, typePanel,
                    "Edit Input Data Types for " + name, JOptionPane.OK_CANCEL_OPTION);
                if (typeRes == JOptionPane.OK_OPTION) {
                    boolean changed = false;
                    for (int i = 0; i < template.inputCount; i++) {
                        String newType = (String) combos[i].getSelectedItem();
                        if (!newType.equals(template.inputTypes[i])) {
                            changed = true;
                            // Remove connections to this input whose type no longer matches
                            final int idx = i;
                            connections.removeIf(conn -> conn.to == FunctionBlock.this && conn.toIdx == idx);
                            fileEntryConnections.removeIf(fec -> fec.toBlock == FunctionBlock.this && fec.toInputIndex == idx);
                            template.inputTypes[i] = newType;
                            template.defaultValues[i] = getDefaultValue(newType);
                            inputValues[i] = getDefaultValue(newType);
                        }
                    }
                    if (changed) {
                        rebuildUI();
                        loadAndDisplayInputFile();
                    }
                }
            });
            menu.add(editInputTypes);

            JMenuItem editOutputTypes = new JMenuItem("\uD83C\uDFA8 Edit Output Data Types");
            editOutputTypes.addActionListener(e -> {
                if (template.outputCount == 0) {
                    JOptionPane.showMessageDialog(gui4.this, "This block has no outputs.", "No Outputs", JOptionPane.INFORMATION_MESSAGE);
                    return;
                }
                String[] types = {"float", "string", "file", "graph", "integer", "Status", "char"};
                JPanel typePanel = new JPanel(new GridLayout(template.outputCount, 2, 5, 5));
                typePanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
                @SuppressWarnings("unchecked")
                JComboBox<String>[] combos = new JComboBox[template.outputCount];
                for (int i = 0; i < template.outputCount; i++) {
                    typePanel.add(new JLabel("Output " + (i + 1) + ":"));
                    JComboBox<String> cb = new JComboBox<>(types);
                    cb.setSelectedItem(template.outputTypes[i]);
                    combos[i] = cb;
                    typePanel.add(cb);
                }
                int typeRes = JOptionPane.showConfirmDialog(gui4.this, typePanel,
                    "Edit Output Data Types for " + name, JOptionPane.OK_CANCEL_OPTION);
                if (typeRes == JOptionPane.OK_OPTION) {
                    boolean changed = false;
                    for (int i = 0; i < template.outputCount; i++) {
                        String newType = (String) combos[i].getSelectedItem();
                        if (!newType.equals(template.outputTypes[i])) {
                            changed = true;
                            // Remove connections from this output whose type no longer matches
                            final int oidx = i;
                            connections.removeIf(conn -> conn.from == FunctionBlock.this && conn.fromIdx == oidx);
                            template.outputTypes[i] = newType;
                        }
                    }
                    if (changed) {
                        rebuildUI();
                    }
                }
            });
            menu.add(editOutputTypes);
            menu.addSeparator();

            JMenuItem delUnconnected = new JMenuItem("\u2702 Delete Unconnected Inputs");
            delUnconnected.addActionListener(e -> deleteUnconnectedInputs());
            menu.add(delUnconnected);

            menu.addSeparator();

            JMenuItem delete = new JMenuItem("\uD83D\uDDD1\uFE0F Delete");
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
            recomputeSize();
            enableDrag(this);
        }
        
        /**
         * Computes natural width/height for this block based on input/output count
         * and applies it. Call after construction or whenever the slot content changes.
         */
        public void recomputeSize() {
            int numSlots = Math.max(template.inputCount, template.outputCount);
            // 32px per slot row + 90px for header panel + padding
            int h = Math.max(130, numSlots * 32 + 90);
            // Width: 240 minimum; wider if there are many slots (badge labels need space)
            int w = numSlots > 15 ? 260 : 240;
            setSize(w, h);
            setPreferredSize(new Dimension(w, h));
            if (getParent() != null) {
                setBounds(getLocation().x, getLocation().y, w, h);
                getParent().revalidate();
                getParent().repaint();
            }
        }

        private String normalizeType(String type) {
            if (type == null) return "";
            String normalized = type.trim().toLowerCase();
            if (normalized.equals("int")) return "integer";
            if (normalized.equals("character")) return "char";
            if (normalized.equals("number") || normalized.equals("double") || normalized.equals("long")) return "float";
            if (normalized.equals("path") || normalized.equals("filepath") || normalized.equals("filename")) return "file";
            return normalized;
        }
        private boolean typesCompatible(String a, String b) {
            String na = normalizeType(a);
            String nb = normalizeType(b);
            if (na.equals(nb)) return true;
            // numeric types are mutually compatible
            boolean aNum = na.equals("integer") || na.equals("float");
            boolean bNum = nb.equals("integer") || nb.equals("float");
            return aNum && bNum;
        }
        
        private Color getTypeColor(String type) {
            if (type == null) return Theme.T_OTHER;
            String normalizedType = type.trim().toLowerCase();
            switch (normalizedType) {
                case "float":                return Theme.T_FLOAT;
                case "integer": case "int":  return Theme.T_INT;
                case "string":               return Theme.T_STRING;
                case "file":                        return Theme.T_FILE;
                case "graph":                       return Theme.T_GRAPH;
                case "status":                      return Theme.SUCCESS;
                case "character": case "char":      return Theme.WARNING;
                default:                            return Theme.T_OTHER;
            }
        }
        
        private void showBlockInfo() {
            String[] desc = getBlockDescription(name);
            
            StringBuilder info = new StringBuilder();
            info.append("=== BLOCK INFORMATION ===\n\n");
            info.append("Name: ").append(name).append("\n");
            info.append("Full Name: ").append(desc[0]).append("\n");
            info.append("Category: ").append(desc.length > 2 ? desc[2] : "Custom").append("\n");
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
            this.setLayout(new BorderLayout(8, 0));
            updateBorder();
            this.setBackground(Theme.BG_CARD);
            String[] oldValues = inputValues != null ? inputValues.clone() : null;
            inputValues = new String[template.inputCount];
            
            this.inputPanel = new JPanel();
            JPanel inputPanel = this.inputPanel;
            inputPanel.setLayout(new GridLayout(template.inputCount, 1, 0, 8));
            inputPanel.setOpaque(false);
            inputArrows = new JLabel[template.inputCount];
            for (int i = 0; i < template.inputCount; i++) {
                inputValues[i] = (oldValues != null && i < oldValues.length) ? oldValues[i] : getDefaultValue(template.inputTypes[i]);
                final int idx = i;
                JLabel inArrow = new JLabel("\u25CF\u2192");
                inArrow.setFont(new Font("SansSerif", Font.BOLD, 14));
                inArrow.setForeground(getTypeColor(template.inputTypes[i]));
                inArrow.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
                inArrow.setToolTipText("<html><b>Input " + (idx + 1) + "</b><br>Type: " + template.inputTypes[idx] + "</html>");
                inArrow.setBorder(BorderFactory.createEmptyBorder(6, 5, 6, 20));
                inArrow.addMouseListener(new MouseAdapter() {
                    public void mouseReleased(MouseEvent e) {
                        if (dragSource != null && dragSourceOutputIndex >= 0) {
                            String outType = dragSource.template.outputTypes[dragSourceOutputIndex];
                            String inType = template.inputTypes[idx];
                            if (!typesCompatible(outType, inType)) {
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
                            refreshInputBadges();
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
                JLabel outArrow = new JLabel("\u2192\u25CF");
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
            recomputeSize();
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
                String category = (desc.length > 2) ? desc[2] : "Custom";
                
                String headerLine = "# " + desc[0] + " - " + category;
                content.append(headerLine).append("\n");
                writer.println(headerLine);
                
                String letLine = "let $" + varName;
                content.append(letLine).append("\n");
                writer.println(letLine);
                if (fb.template.outputTypes.length > 0 &&
                    "Status".equalsIgnoreCase(fb.template.outputTypes[0])) {
                    String statusLine = "#status " + fb.originalName;
                    content.append(statusLine).append("\n");
                    writer.println(statusLine);
                }
                for (int i = 0; i < fb.template.inputCount; i++) {
                    String inputValue = "";
                    String connFileName = null;
                    for (Connection c : connections) {
                        if (c.to == fb && c.toIdx == i) {
                            inputValue = "$" + c.from.originalName + ".output" + (c.fromIdx + 1);
                            connFileName = c.fileName;
                            break;
                        }
                    }
                    if (inputValue.isEmpty()) {
                        inputValue = fb.inputValues[i];
                    }
                    String inputLine = "    input" + (i + 1) + " " + inputValue;
                    // Append fileName if set for file connections
                    if (connFileName != null && !connFileName.trim().isEmpty()) {
                        inputLine += " #file=" + connFileName;
                    }
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
                    // Append fileName if set for file connections
                    if (c.fileName != null && !c.fileName.trim().isEmpty()) {
                        connLine += " [file: " + c.fileName + "]";
                    }
                }
                content.append(connLine).append("\n");
                writer.println(connLine);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        return content.toString();
    }

    /** Returns a description line for every block input that has no connection and no typed value. */
    private List<String> getUnfilledInputs(List<FunctionBlock> order) {
        List<String> unfilled = new ArrayList<>();
        for (FunctionBlock fb : order) {
            if (fb.template == null || fb.template.inputCount == 0) continue;
            for (int i = 0; i < fb.template.inputCount; i++) {
                boolean hasConn = false;
                for (Connection c : connections) {
                    if (c.to == fb && c.toIdx == i) { hasConn = true; break; }
                }
                if (!hasConn) {
                    for (FileEntryConnection fec : fileEntryConnections) {
                        if (fec.toBlock == fb && fec.toInputIndex == i) { hasConn = true; break; }
                    }
                }
                if (hasConn) continue;
                String val = (fb.inputValues != null && i < fb.inputValues.length) ? fb.inputValues[i] : "";
                if (val != null && !val.isEmpty() && !val.equals("empty_file")) continue;
                String inputType = (fb.template.inputTypes != null && i < fb.template.inputTypes.length)
                        ? fb.template.inputTypes[i] : "?";
                unfilled.add(fb.name + "  ·  input " + (i + 1) + "  (" + inputType + ")");
            }
        }
        return unfilled;
    }

    private void generateScriptFromPipeline() {
        List<FunctionBlock> order = getTopologicalOrder();
        if (order == null || order.isEmpty()) {
            JOptionPane.showMessageDialog(getDialogParent(),
                "No blocks in pipeline or a cycle was detected.", "Warning", JOptionPane.WARNING_MESSAGE);
            return;
        }
        List<String> unfilled = getUnfilledInputs(order);
        if (!unfilled.isEmpty()) {
            StringBuilder msg = new StringBuilder("The following inputs have no data:\n\n");
            for (String s : unfilled) msg.append("  • ").append(s).append("\n");
            msg.append("\nGenerate script anyway?");
            int choice = JOptionPane.showConfirmDialog(getDialogParent(), msg.toString(),
                    "Unfilled Inputs", JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);
            if (choice != JOptionPane.YES_OPTION) return;
        }
        JFileChooser chooser = new JFileChooser();
        chooser.setDialogTitle("Save Generated Script");
        chooser.setSelectedFile(new java.io.File("generated_script.txt"));
        if (chooser.showSaveDialog(getDialogParent()) != JFileChooser.APPROVE_OPTION) return;
        java.io.File outFile = chooser.getSelectedFile();

        // --- Build auto-names: {blockname}_output{N}  e.g. cudf_output1, pathz3_output2 ---
        java.util.Map<FunctionBlock, String[]> autoNames = new java.util.LinkedHashMap<>();
        for (FunctionBlock fb : order) {
            if (fb.template == null || fb.template.outputCount == 0) {
                autoNames.put(fb, new String[0]);
                continue;
            }
            String base = fb.originalName.replaceAll("_\\d+$", "");
            String[] names = new String[fb.template.outputCount];
            for (int i = 0; i < fb.template.outputCount; i++) {
                names[i] = base + "_output" + (i + 1);
            }
            autoNames.put(fb, names);
        }

        try (PrintWriter w = new PrintWriter(outFile, "UTF-8")) {

            // === PHASE 1: declare {type} {auto_name} ... grouped by type ===
            // Skip file-type vars — file locations come from the backend
            java.util.Map<String, List<String>> declareMap = new java.util.LinkedHashMap<>();
            for (FunctionBlock fb : order) {
                String[] an = autoNames.get(fb);
                if (an == null || an.length == 0) continue;
                for (int i = 0; i < an.length; i++) {
                    String type = (fb.template != null && fb.template.outputTypes != null
                            && i < fb.template.outputTypes.length)
                            ? fb.template.outputTypes[i] : "var";
                    if ("file".equals(type)) continue;  // file paths come from backend
                    declareMap.computeIfAbsent(type, k -> new java.util.ArrayList<>()).add(an[i]);
                }
            }
            for (java.util.Map.Entry<String, List<String>> e : declareMap.entrySet()) {
                w.println("declare " + e.getKey() + " " + String.join(" ", e.getValue()));
            }
            if (!declareMap.isEmpty()) w.println();

            // === PHASE 2: let {auto_name} {user_given_name} (for all named outputs) ===
            boolean anyBinding = false;
            for (FunctionBlock fb : order) {
                String base = fb.originalName.replaceAll("_\\d+$", "");
                if (base.equals("let")) continue;  // let blocks handle themselves in Phase 3
                String[] an = autoNames.get(fb);
                if (an == null || an.length == 0) continue;
                for (int oi = 0; oi < an.length; oi++) {
                    // Prefer outputVarNames; fall back to conn.fileName on any connection from this output
                    String user = (fb.outputVarNames != null && oi < fb.outputVarNames.length
                            && fb.outputVarNames[oi] != null) ? fb.outputVarNames[oi].trim() : "";
                    if (user.isEmpty()) {
                        for (Connection c : connections) {
                            if (c.from == fb && c.fromIdx == oi && c.fileName != null && !c.fileName.trim().isEmpty()) {
                                user = c.fileName.trim(); break;
                            }
                        }
                    }
                    if (!user.isEmpty()) {
                        String outType = (fb.template != null && fb.template.outputTypes != null
                                && oi < fb.template.outputTypes.length)
                                ? fb.template.outputTypes[oi] : "var";
                        if ("file".equals(outType)) {
                            // File outputs: let USER_NAME <actual_path>  (no temp var)
                            // Look up actual value from the loaded data file entries
                            String fileVal = null;
                            java.util.Map<String, String[]> dataMap = getDataFileEntriesMap();
                            java.util.Map<String, String> p2aliases = getParamAliases();
                            String userUp = user.toUpperCase();
                            String[] dataEntry = dataMap.get(userUp);
                            if (dataEntry == null) {
                                // try aliases: canonical→alias or alias→canonical
                                String canon = p2aliases.get(userUp);
                                if (canon != null) dataEntry = dataMap.get(canon);
                                if (dataEntry == null) {
                                    for (java.util.Map.Entry<String, String> ae : p2aliases.entrySet()) {
                                        if (ae.getValue().equals(userUp)) { dataEntry = dataMap.get(ae.getKey()); if (dataEntry != null) break; }
                                    }
                                }
                            }
                            if (dataEntry != null && !dataEntry[1].isEmpty()
                                    && !dataEntry[1].equals("empty_file")) {
                                fileVal = dataEntry[1];
                            }
                            if (fileVal == null) fileVal = "empty_file";
                            w.println("let " + user + " " + fileVal);
                        } else {
                            // Non-file outputs: let auto_name user_name
                            w.println("let " + an[oi] + " " + user);
                        }
                        anyBinding = true;
                    }
                }
            }
            if (anyBinding) w.println();

            // === PHASE 3: commands in topological order ===
            for (FunctionBlock fb : order) {
                String base = fb.originalName.replaceAll("_\\d+$", "");

                if (base.equals("let")) {
                    // let: auto_name first, then the value/ref from port 0
                    String[] an = autoNames.get(fb);
                    String auto = (an != null && an.length > 0) ? an[0] : "";
                    String letVal = resolveScriptInput(fb, 0, autoNames);
                    w.println("let " + auto + " " + letVal);
                    continue;
                }

                StringBuilder line = new StringBuilder(base);
                int inputCount = fb.template != null ? fb.template.inputCount : 0;
                for (int i = 0; i < inputCount; i++) {
                    line.append(" ").append(resolveScriptInput(fb, i, autoNames));
                }
                // Append output var references
                String[] an = autoNames.get(fb);
                if (an != null) {
                    for (int i = 0; i < an.length; i++) {
                        String outType = (fb.template.outputTypes != null && i < fb.template.outputTypes.length)
                                ? fb.template.outputTypes[i] : "var";
                        String user = (fb.outputVarNames != null && i < fb.outputVarNames.length
                                && fb.outputVarNames[i] != null) ? fb.outputVarNames[i].trim() : "";
                        if (user.isEmpty()) {
                            for (Connection c : connections) {
                                if (c.from == fb && c.fromIdx == i && c.fileName != null && !c.fileName.trim().isEmpty()) {
                                    user = c.fileName.trim(); break;
                                }
                            }
                        }
                        if (!user.isEmpty()) {
                            line.append(" $").append(user);
                        } else {
                            line.append(" $").append(an[i]);          // default name when not connected
                        }
                    }
                }
                w.println(line.toString());
            }
            w.println();
            w.println("exit");

        } catch (IOException ex) {
            JOptionPane.showMessageDialog(getDialogParent(),
                "Error writing script: " + ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        JOptionPane.showMessageDialog(getDialogParent(),
            "Script saved to:\n" + outFile.getAbsolutePath(), "Script Generated", JOptionPane.INFORMATION_MESSAGE);
    }

    /** Resolves what goes into input port i of block fb for the generated script. */
    private String resolveScriptInput(FunctionBlock fb, int i,
            java.util.Map<FunctionBlock, String[]> autoNames) {
        // 1. Block-to-block wire connection: carry the source output's effective name
        for (Connection c : connections) {
            if (c.to == fb && c.toIdx == i) {
                FunctionBlock src = c.from;
                int fi = c.fromIdx;
                // User name: check outputVarNames first, then conn.fileName as fallback
                String user = (src.outputVarNames != null && fi < src.outputVarNames.length
                        && src.outputVarNames[fi] != null) ? src.outputVarNames[fi].trim() : "";
                if (user.isEmpty() && c.fileName != null && !c.fileName.trim().isEmpty())
                    user = c.fileName.trim();
                if (!user.isEmpty()) return "$" + user;
                // Fall back to auto-name for all output types
                String[] srcAuto = autoNames.get(src);
                if (srcAuto != null && fi < srcAuto.length && !srcAuto[fi].isEmpty())
                    return "$" + srcAuto[fi];
                return "";
            }
        }
        // Determine if this is a let block (it receives raw values, others receive variable names)
        String fbBase = fb.originalName.replaceAll("_\\d+$", "");
        boolean isLetBlock = fbBase.equals("let");

        // 2. File-entry connection — let blocks use the actual entry value; other blocks use $VAR_NAME
        for (FileEntryConnection fec : fileEntryConnections) {
            if (fec.toBlock == fb && fec.toInputIndex == i) {
                if (isLetBlock) {
                    // Return the actual file path stored in the entry
                    return (fec.entryValue != null && !fec.entryValue.isEmpty())
                            ? fec.entryValue : "";
                }
                String varName = fec.entryName.toUpperCase()
                        .replace(" ", "_").replace("-", "_").replace(".", "_");
                return "$" + varName;
            }
        }
        // 3. No connection — return the literal value typed directly into the block
        String val = (fb.inputValues != null && i < fb.inputValues.length) ? fb.inputValues[i] : "";
        if (val != null && !val.isEmpty() && !val.equals("empty_file")) {
            return val;
        }
        return "";
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
