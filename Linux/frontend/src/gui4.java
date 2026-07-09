import java.awt.*;
import java.awt.event.*;
import java.awt.geom.AffineTransform;
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
    private static final Map<String, String[]> BLOCK_SCRIPT_COMMENTS = new HashMap<>();
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
        BLOCK_SCRIPT_COMMENTS.put("let",    new String[]{"Variable assignment"});
        BLOCK_SCRIPT_COMMENTS.put("exec",   new String[]{"Shell command execution"});
        BLOCK_SCRIPT_COMMENTS.put("cudf",   new String[]{"create up-reg and down-reg files from fold change file and thresholds"});
        BLOCK_SCRIPT_COMMENTS.put("start",  new String[]{"Read the following global files before starting execution of the pipeline"});
        BLOCK_SCRIPT_COMMENTS.put("mff",    new String[]{"Merging KEGG pathways to obtain merged master network", "Fifth argument is n: if these files were not previously written by the tool, otherwise 'y'"});
        BLOCK_SCRIPT_COMMENTS.put("wgx",    new String[]{"Writing merged graph into xml file, end with string for pathway.", "Currently using 'KEGG'."});
        BLOCK_SCRIPT_COMMENTS.put("size",   new String[]{"Get size of current graph"});
        BLOCK_SCRIPT_COMMENTS.put("rgx",    new String[]{"Read file written in xml format"});
        BLOCK_SCRIPT_COMMENTS.put("fb_rch", new String[]{"Reachability (forward and backward) based pruning of paths", "Retain only those nodes that appear in some path of length <=", "$REACH_PATH_BOUND from $SRC_NODE to $TGT_NODE"});
        BLOCK_SCRIPT_COMMENTS.put("pathz3", new String[]{"Find PO points", "$PO_SEARCH_MODE 0 means that PO points must be explicitly computed.", "$PO_SEARCH_MODE 1 means that we simply want to check if there are", "solutions at any point of a (previously computed) PO curve."});
        BLOCK_SCRIPT_COMMENTS.put("result", new String[]{"Final result / output aggregation"});
    }
    // Maps Data_Coming_From_Frontend.txt entry names -> exact script variable names
    private static final Map<String, String> ENTRY_VAR = new HashMap<>();
    static {
        ENTRY_VAR.put("log_fold_changes",                "LOG_FOLD_CHANGE_FILE");
        ENTRY_VAR.put("microarray_data",                 "MICROARRAY_DATA_FILE");
        ENTRY_VAR.put("kegg_xml",                        "KEGG_XML_FILE");
        ENTRY_VAR.put("merged_kegg_pathways_xml_file",   "MERGED_KEGG_PATHWAYS_XML_FILE");
        ENTRY_VAR.put("list_of_merged_kegg_xml_file",    "LIST_OF_MERGED_KEGG_XML_FILE");
        ENTRY_VAR.put("exception_to_node_merge_file",    "EXCEPTION_TO_NODE_MERGE_FILE");
        ENTRY_VAR.put("additional_edges",                "ADDITIONAL_EDGES_FILE");
        ENTRY_VAR.put("essential_edges",                 "ESSENTIAL_EDGES_FILE");
        ENTRY_VAR.put("avoid_edges",                     "AVOID_EDGES_FILE");
        ENTRY_VAR.put("relaxed_edges",                   "RELAXED_EDGES_FILE");
        ENTRY_VAR.put("non_relaxed_edges",               "NONRELAXED_EDGES_FILE");
        ENTRY_VAR.put("inactive_nodes_file",             "INACTIVE_NODES_FILE");
        ENTRY_VAR.put("confirmed_up_reg_file",           "CONFIRMED_UP_REG_FILE");
        ENTRY_VAR.put("confirmed_down_reg_file",         "CONFIRMED_DOWN_REG_FILE");
        ENTRY_VAR.put("relaxed_nodes_file",              "RELAXED_NODES_FILE");
        ENTRY_VAR.put("hsa_to_gene_map",                 "HSA_TO_GENE_SYMBOL_MAP_FILE");
        ENTRY_VAR.put("hsa_path_map",                    "HSA_PATH_TO_PATH_NAME_MAP_FILE");
        ENTRY_VAR.put("cross_db_map",                    "INTER_DB_MAP_FILE");
        ENTRY_VAR.put("hsa_not_merged",                  "HSA_IDS_NOT_TO_BE_MERGED_FILE");
        ENTRY_VAR.put("signalling_path_length",          "REACH_PATH_BOUND");
        ENTRY_VAR.put("node_relax_lower",                "NODE_RELAX_LB");
        ENTRY_VAR.put("node_relax_upper",                "NODE_RELAX_UB");
        ENTRY_VAR.put("edge_relax_lower",                "EDGE_RELAX_LB");
        ENTRY_VAR.put("edge_relax_upper",                "EDGE_RELAX_UB");
        ENTRY_VAR.put("node_split_threshold",            "NODE_SPLIT_THRESHOLD");
        ENTRY_VAR.put("inc_solver_timeout",              "CONSTR_SOLVER_TIMEOUT1");
        ENTRY_VAR.put("over_solver_timeout",             "CONSTR_SOLVER_TIMEOUT2");
        ENTRY_VAR.put("solution_count",                  "NUM_SOLNS_TO_COUNT");
        ENTRY_VAR.put("solution_explore",                "NUM_SOLNS_TO_EXPLORE");
        ENTRY_VAR.put("up_threshold",                    "UP_REG_THRESH");
        ENTRY_VAR.put("down_threshold",                  "DOWN_REG_THRESH");
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
        legendPanel.setBackground(new Color(245, 245, 250, 230));
        legendPanel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(new Color(100, 100, 150), 1),
            BorderFactory.createEmptyBorder(5, 10, 5, 10)
        ));
        JLabel legendLabel = new JLabel("Legend: ");
        legendLabel.setFont(new Font("SansSerif", Font.BOLD, 12));
        legendLabel.setForeground(new Color(50, 50, 100));
        legendPanel.add(legendLabel);
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
            
            JLabel label = new JLabel(typeName);
            label.setFont(new Font("SansSerif", Font.BOLD, 11));
            label.setForeground(color.darker());
            
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
        JPanel controlsPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 4, 3));
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
            
            menu.addSeparator();
            
            JMenuItem savePipelineItem = new JMenuItem("\uD83D\uDCBE Save Pipeline (Ctrl+S)");
            savePipelineItem.addActionListener(ev -> savePipeline());
            menu.add(savePipelineItem);
            
            JMenuItem loadPipelineItem = new JMenuItem("\uD83D\uDCC2 Load Pipeline (Ctrl+O)");
            loadPipelineItem.addActionListener(ev -> loadPipeline());
            menu.add(loadPipelineItem);
            
            menu.show(hamburgerButton, 0, hamburgerButton.getHeight());
        });
        controlsPanel.add(hamburgerButton);
        blockSelector = new JComboBox<String>();
        blockSelector.setRenderer(new BlockSelectorRenderer());
        blockSelector.setPreferredSize(new Dimension(120, 24));
        blockSelector.setMaximumSize(new Dimension(120, 24));
        controlsPanel.add(new JLabel("Block:"));
        controlsPanel.add(blockSelector);
        JButton addBlockBtn = new JButton("Add");
        addBlockBtn.setToolTipText("Add selected block to canvas");
        addBlockBtn.addActionListener(e -> addSelectedBlockInstance());
        controlsPanel.add(addBlockBtn);
        JButton addTemplateBtn = new JButton("+Tmpl");
        addTemplateBtn.setBackground(new Color(138, 43, 226));
        addTemplateBtn.setForeground(Color.WHITE);
        addTemplateBtn.setToolTipText("New Block Template");
        addTemplateBtn.addActionListener(e -> showNewBlockTemplateDialog());
        controlsPanel.add(addTemplateBtn);
        JTextField searchField = new JTextField(8);
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
        JButton zoomInButton = new JButton("\uD83D\uDD0D+");
        zoomInButton.addActionListener(e -> drawingPanel.zoom(1.2));
        JButton zoomOutButton = new JButton("\uD83D\uDD0D\u2212");
        zoomOutButton.addActionListener(e -> drawingPanel.zoom(0.8));
        controlsPanel.add(zoomInButton);
        controlsPanel.add(zoomOutButton);
        JButton executeBtn = new JButton("\u25B6 Execute");
        executeBtn.setBackground(new Color(34, 139, 34));
        executeBtn.setForeground(Color.WHITE);
        executeBtn.addActionListener(e -> executeGraph());
        controlsPanel.add(executeBtn);

        JButton genScriptBtn = new JButton("\uD83D\uDCC4 Gen Script");
        genScriptBtn.setBackground(new Color(100, 60, 160));
        genScriptBtn.setForeground(Color.WHITE);
        genScriptBtn.setToolTipText("Generate advtempscript from current pipeline");
        genScriptBtn.addActionListener(e -> generateScriptFromPipeline());
        controlsPanel.add(genScriptBtn);

        JButton saveBtn = new JButton("\uD83D\uDCBE Save");
        saveBtn.setBackground(new Color(0, 120, 215));
        saveBtn.setForeground(Color.WHITE);
        saveBtn.setToolTipText("Save Pipeline (Ctrl+S)");
        saveBtn.addActionListener(e -> savePipeline());
        controlsPanel.add(saveBtn);

        JButton loadBtn = new JButton("\uD83D\uDCC2 Load");
        loadBtn.setBackground(new Color(200, 120, 0));
        loadBtn.setForeground(Color.WHITE);
        loadBtn.setToolTipText("Load Pipeline (Ctrl+O)");
        loadBtn.addActionListener(e -> loadPipeline());
        controlsPanel.add(loadBtn);

        // Fix preferred size so FlowLayout never wraps inside the scroll pane
        controlsPanel.setPreferredSize(new Dimension(1280, 36));
        JScrollPane ctrlScroll = new JScrollPane(controlsPanel,
            JScrollPane.VERTICAL_SCROLLBAR_NEVER,
            JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        ctrlScroll.setBorder(BorderFactory.createMatteBorder(0, 0, 1, 0, new java.awt.Color(200, 205, 220)));
        ctrlScroll.setMaximumSize(new Dimension(Integer.MAX_VALUE, 44));
        ctrlScroll.setMinimumSize(new Dimension(0, 36));
        topPanel.add(ctrlScroll);
        
        blockListPanel = new JPanel();
        blockListPanel.setLayout(new FlowLayout(FlowLayout.LEFT, 5, 5));
        JScrollPane blockScrollPane = new JScrollPane(blockListPanel);
        blockScrollPane.setPreferredSize(new Dimension(1280, 80));
        blockScrollPane.setMaximumSize(new Dimension(Integer.MAX_VALUE, 80));
        blockScrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        blockScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_NEVER);
        topPanel.add(blockScrollPane);
        populateBlockLibrary();
        add(topPanel, BorderLayout.NORTH);
        tabbedPane = new JTabbedPane();
        
        JPanel editorPanel = new JPanel(new BorderLayout());
        
        JPanel legendContainer = new JPanel(new FlowLayout(FlowLayout.RIGHT, 0, 0));
        legendContainer.setBackground(new Color(248, 248, 252));
        legendContainer.add(createLegendPanel());
        editorPanel.add(legendContainer, BorderLayout.NORTH);
        
        // === FILE PANEL (left side, 1/6 width) ===
        JPanel filePanel = createFilePanel();
        
        JScrollPane canvasScroll = new JScrollPane(drawingPanel);
        canvasScroll.getVerticalScrollBar().setUnitIncrement(16);
        
        // Split pane: file panel on left, canvas on right
        JSplitPane editorSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, filePanel, canvasScroll);
        editorSplit.setOneTouchExpandable(true);
        editorSplit.setContinuousLayout(true);
        editorSplit.setBorder(null);
        editorSplit.setDividerSize(6);
        // Proportional divider: left panel gets ~18%, canvas gets the rest
        editorSplit.setResizeWeight(0.18);
        editorSplit.addComponentListener(new java.awt.event.ComponentAdapter() {
            private boolean initialized = false;
            @Override
            public void componentResized(java.awt.event.ComponentEvent e) {
                if (!initialized && editorSplit.getWidth() > 0) {
                    editorSplit.setDividerLocation(0.18);
                    initialized = true;
                }
            }
        });
        editorPanel.add(editorSplit, BorderLayout.CENTER);
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
    }
    // =====================================================
    // FILE PANEL METHODS  (reads Data_Coming_From_Frontend.txt)
    // Format: name,type,value  (comma-separated, 3 columns)
    // =====================================================
    /** Returns the accent color for a given type string (mirrors graph editor colours). */
    private Color getFilePanelTypeColor(String type) {
        if (type == null) return new Color(100, 100, 100);
        switch (type.trim().toLowerCase()) {
            case "float":   return new Color(139, 0, 0);
            case "integer": case "int": return new Color(0, 0, 139);
            case "string":  return new Color(200, 100, 0);
            case "file":    return new Color(199, 21, 133);
            case "graph":   return new Color(0, 139, 139);
            case "status":  return new Color(0, 100, 0);
            case "boolean": return new Color(80, 140, 40);
            case "char": case "character": return new Color(184, 134, 11);
            default:        return new Color(80, 80, 80);
        }
    }
    private JPanel createFilePanel() {
        JPanel panel = new JPanel(new BorderLayout(0, 0));
        panel.setBackground(new Color(240, 242, 248));
        panel.setBorder(BorderFactory.createMatteBorder(0, 0, 0, 1, new Color(180, 190, 210)));
        panel.setMinimumSize(new Dimension(130, 0));
        panel.setPreferredSize(new Dimension(230, 0));

        // ---- Header ----
        JPanel header = new JPanel(new BorderLayout(4, 4));
        header.setBackground(new Color(45, 65, 120));
        header.setBorder(BorderFactory.createEmptyBorder(8, 10, 8, 6));

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
        reloadBtn.setBackground(new Color(70, 100, 170));
        reloadBtn.setForeground(Color.WHITE);
        reloadBtn.setBorderPainted(false);
        reloadBtn.addActionListener(e -> loadAndDisplayInputFile());
        headerBtns.add(reloadBtn);

        JButton browseBtn = new JButton("\uD83D\uDCC1");
        browseBtn.setFont(new Font("SansSerif", Font.PLAIN, 13));
        browseBtn.setToolTipText("Browse for a different file");
        browseBtn.setMargin(new Insets(1, 6, 1, 6));
        browseBtn.setFocusPainted(false);
        browseBtn.setBackground(new Color(70, 100, 170));
        browseBtn.setForeground(Color.WHITE);
        browseBtn.setBorderPainted(false);
        browseBtn.addActionListener(e -> browseInputFile());
        headerBtns.add(browseBtn);

        header.add(headerBtns, BorderLayout.EAST);
        panel.add(header, BorderLayout.NORTH);

        // ---- Source label (bottom) ----
        filePanelSourceLabel = new JLabel(" " + DEFAULT_INPUT_FILE);
        filePanelSourceLabel.setFont(new Font("SansSerif", Font.ITALIC, 10));
        filePanelSourceLabel.setForeground(new Color(90, 100, 130));
        filePanelSourceLabel.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(1, 0, 0, 0, new Color(200, 205, 220)),
            BorderFactory.createEmptyBorder(3, 8, 3, 4)
        ));
        filePanelSourceLabel.setBackground(new Color(232, 235, 245));
        filePanelSourceLabel.setOpaque(true);
        filePanelSourceLabel.setToolTipText(currentInputFilePath);
        panel.add(filePanelSourceLabel, BorderLayout.SOUTH);

        // ---- Entries container (scrollable) ----
        fileEntriesPanel = new JPanel();
        fileEntriesPanel.setLayout(new BoxLayout(fileEntriesPanel, BoxLayout.Y_AXIS));
        fileEntriesPanel.setBackground(new Color(240, 242, 248));
        fileEntriesPanel.setBorder(BorderFactory.createEmptyBorder(6, 0, 6, 0));

        JScrollPane scroll = new JScrollPane(fileEntriesPanel);
        scroll.setBorder(null);
        scroll.getVerticalScrollBar().setUnitIncrement(14);
        scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        scroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        panel.add(scroll, BorderLayout.CENTER);

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
            for (int ei = 0; ei < entries.size(); ei++) {
                String[] e = entries.get(ei);
                fileEntriesPanel.add(createFrontendEntryCard(e[0], e[1], e[2], ei + 1));
            }
        }
        filePanelSourceLabel.setText(" " + file.getName() + "  (" + entries.size() + " entries)");
        filePanelSourceLabel.setToolTipText(file.getAbsolutePath());
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
    /**
     * Creates a styled card for one entry: position number / name / type-badge / value + Connect button.
     * position = 1-based index in the file (shown as the connection number on blocks)
     */
    private JPanel createFrontendEntryCard(String name, String type, String value, int position) {
        Color typeColor = getFilePanelTypeColor(type);
        Color accentColor = typeColor;
        String displayValue = (value == null || value.isEmpty()) ? "(empty)" :
            ("file".equalsIgnoreCase(type) ? new java.io.File(value).getName() : value);

        // Outer wrapper
        JPanel outer = new JPanel();
        outer.setLayout(new BoxLayout(outer, BoxLayout.Y_AXIS));
        outer.setOpaque(false);
        outer.setBorder(BorderFactory.createEmptyBorder(0, 6, 0, 6));
        outer.setAlignmentX(Component.LEFT_ALIGNMENT);

        // === Main card ===
        JPanel card = new JPanel(new BorderLayout(4, 0));
        card.setBackground(Color.WHITE);
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 4, 0, 0, accentColor),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(new Color(215, 220, 235), 1),
                BorderFactory.createEmptyBorder(5, 7, 5, 5)
            )
        ));
        card.setAlignmentX(Component.LEFT_ALIGNMENT);
        card.setMaximumSize(new Dimension(Integer.MAX_VALUE, 62));

        // Far left: position number circle
        JLabel posLabel = new JLabel(String.valueOf(position)) {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(accentColor);
                int d = Math.min(getWidth(), getHeight());
                g2.fillOval((getWidth()-d)/2, (getHeight()-d)/2, d, d);
                g2.dispose();
                super.paintComponent(g);
            }
        };
        posLabel.setFont(new Font("SansSerif", Font.BOLD, 10));
        posLabel.setForeground(Color.WHITE);
        posLabel.setOpaque(false);
        posLabel.setPreferredSize(new Dimension(22, 22));
        posLabel.setHorizontalAlignment(SwingConstants.CENTER);
        posLabel.setVerticalAlignment(SwingConstants.CENTER);
        posLabel.setToolTipText("Entry #" + position + " in file — this number appears on the block when connected");
        JPanel posWrapper = new JPanel(new FlowLayout(FlowLayout.CENTER, 0, 0));
        posWrapper.setOpaque(false);
        posWrapper.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 4));
        posWrapper.add(posLabel);
        card.add(posWrapper, BorderLayout.WEST);

        // Center: name + value
        JLabel nameLabel = new JLabel(name);
        nameLabel.setFont(new Font("SansSerif", Font.BOLD, 11));
        nameLabel.setForeground(new Color(30, 50, 110));
        JLabel valueLabel = new JLabel(displayValue);
        valueLabel.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 10));
        valueLabel.setForeground(value == null || value.isEmpty() ? new Color(170, 170, 170) : new Color(50, 60, 80));
        JPanel textPanel = new JPanel();
        textPanel.setLayout(new BoxLayout(textPanel, BoxLayout.Y_AXIS));
        textPanel.setOpaque(false);
        textPanel.add(nameLabel);
        textPanel.add(Box.createVerticalStrut(2));
        textPanel.add(valueLabel);
        card.add(textPanel, BorderLayout.CENTER);

        // Right: type badge + Connect button stacked
        JLabel typeBadge = new JLabel(type.toLowerCase()) {
            @Override protected void paintComponent(Graphics g) {
                Graphics2D g2 = (Graphics2D) g.create();
                g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
                g2.setColor(accentColor); g2.fillRoundRect(0, 0, getWidth(), getHeight(), 10, 10);
                g2.dispose(); super.paintComponent(g);
            }
        };
        typeBadge.setFont(new Font("SansSerif", Font.BOLD, 9));
        typeBadge.setForeground(Color.WHITE); typeBadge.setOpaque(false);
        typeBadge.setBorder(BorderFactory.createEmptyBorder(2, 5, 2, 5));
        typeBadge.setToolTipText("Type: " + type);

        // Connect button — small, in right panel
        JButton connectBtn = new JButton("\u2192 Connect");
        connectBtn.setFont(new Font("SansSerif", Font.PLAIN, 9));
        connectBtn.setMargin(new Insets(1, 4, 1, 4));
        connectBtn.setFocusPainted(false);
        connectBtn.setBackground(accentColor);
        connectBtn.setForeground(Color.WHITE);
        connectBtn.setBorderPainted(false);
        connectBtn.setToolTipText("Click then click a block input to connect");
        connectBtn.addActionListener(ev -> {
            // Enter file-entry connection mode
            pendingEntryName     = name;
            pendingEntryType     = type;
            pendingEntryValue    = value;
            pendingEntryPosition = position;
            drawingPanel.setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
            connectBtn.setText("\u23F3 [" + position + "] ...");
            connectBtn.setBackground(new Color(200, 100, 0));
            // Reset after 10 s if no connection made
            new javax.swing.Timer(10000, e2 -> {
                if (pendingEntryName != null) {
                    pendingEntryName = pendingEntryType = pendingEntryValue = null;
                    pendingEntryPosition = 0;
                    drawingPanel.setCursor(Cursor.getDefaultCursor());
                    connectBtn.setText("\u2192 Connect");
                    connectBtn.setBackground(accentColor);
                }
            }) {{ setRepeats(false); }}.start();
        });

        JPanel rightPanel = new JPanel();
        rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));
        rightPanel.setOpaque(false);
        rightPanel.add(typeBadge);
        rightPanel.add(Box.createVerticalStrut(3));
        rightPanel.add(connectBtn);
        card.add(rightPanel, BorderLayout.EAST);

        // Hover + click-to-copy on text area
        final String copyVal = value;
        MouseAdapter hover = new MouseAdapter() {
            public void mouseEntered(MouseEvent e) { card.setBackground(new Color(238, 243, 255)); }
            public void mouseExited(MouseEvent e)  { card.setBackground(Color.WHITE); }
            public void mouseClicked(MouseEvent e) {
                java.awt.datatransfer.StringSelection sel = new java.awt.datatransfer.StringSelection(copyVal);
                java.awt.Toolkit.getDefaultToolkit().getSystemClipboard().setContents(sel, null);
                card.setBackground(new Color(210, 228, 255));
                new javax.swing.Timer(350, ev -> card.setBackground(Color.WHITE)).start();
            }
        };
        card.addMouseListener(hover);
        textPanel.addMouseListener(hover);
        nameLabel.addMouseListener(hover);
        valueLabel.addMouseListener(hover);

        String tooltipValue = (value == null || value.isEmpty()) ? "(empty)" : value;
        card.setToolTipText("<html><b>" + name + "</b><br>Type: <font color='" +
            String.format("#%02x%02x%02x", accentColor.getRed(), accentColor.getGreen(), accentColor.getBlue()) +
            "'>" + type + "</font><br>Value: " + tooltipValue + "<br><i>Click to copy | Use → Connect to wire to a block</i></html>");

        outer.add(card);

        // === Connection tags (one per FileEntryConnection for this entry) ===
        for (FileEntryConnection fec : fileEntryConnections) {
            if (fec.entryName.equals(name)) {
                outer.add(Box.createVerticalStrut(2));
                outer.add(buildConnectionTag(fec, accentColor));
            }
        }
        outer.add(Box.createVerticalStrut(3));
        return outer;
    }
    /** Small tag below a file-entry card showing which block input it is wired to. */
    private JPanel buildConnectionTag(FileEntryConnection fec, Color accentColor) {
        JPanel tag = new JPanel(new BorderLayout(4, 0));
        tag.setBackground(new Color(245, 248, 255));
        tag.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createMatteBorder(0, 4, 0, 0, accentColor.brighter()),
            BorderFactory.createCompoundBorder(
                BorderFactory.createLineBorder(new Color(200, 210, 235), 1),
                BorderFactory.createEmptyBorder(3, 7, 3, 5)
            )
        ));
        tag.setMaximumSize(new Dimension(Integer.MAX_VALUE, 30));
        tag.setAlignmentX(Component.LEFT_ALIGNMENT);
        // Number badge — shows entry's file position
        JLabel numBadge = new JLabel("[" + fec.entryPosition + "]");
        numBadge.setFont(new Font("SansSerif", Font.BOLD, 10));
        numBadge.setForeground(accentColor.darker());
        // Description
        String label = "\u2192 " + fec.toBlock.name + "  input" + (fec.toInputIndex + 1);
        if (fec.fileName != null && !fec.fileName.isEmpty()) label += "  \u2014 " + fec.fileName;
        JLabel connLabel = new JLabel(label);
        connLabel.setFont(new Font("SansSerif", Font.PLAIN, 10));
        connLabel.setForeground(new Color(50, 70, 120));
        JPanel left = new JPanel(new FlowLayout(FlowLayout.LEFT, 3, 0));
        left.setOpaque(false);
        left.add(numBadge); left.add(connLabel);
        tag.add(left, BorderLayout.CENTER);
        // Delete button
        JLabel delBtn = new JLabel("\u2715");
        delBtn.setFont(new Font("SansSerif", Font.BOLD, 10));
        delBtn.setForeground(new Color(180, 50, 50));
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
        if (!isFullScreen && fullScreenDevice.isFullScreenSupported()) {
            setVisible(false);
            dispose();
            setUndecorated(true);
            fullScreenDevice.setFullScreenWindow(this);
            setVisible(true);
            isFullScreen = true;
        } else {
            fullScreenDevice.setFullScreenWindow(null);
            setVisible(false);
            dispose();
            setUndecorated(false);
            Dimension screen = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
            setSize(screen.width, screen.height);
            setExtendedState(JFrame.MAXIMIZED_BOTH);
            setVisible(true);
            isFullScreen = false;
        }
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
        addTemplateInternal(new BlockTemplate("let", 2, 1, new String[]{"integer","integer"}, new String[]{"file"}));
        addTemplateInternal(new BlockTemplate("cudf", 3, 2, new String[]{"file", "float","float"}, new String[]{"file","file"}));
        addTemplateInternal(new BlockTemplate("exec", 2, 1, new String[]{"integer","integer"}, new String[]{"file"}));
        addTemplateInternal(new BlockTemplate("start", 4, 1, new String[]{"file", "file","file","file"}, new String[]{"Status"}));
        addTemplateInternal(new BlockTemplate("mff", 6, 0,
            new String[]{"file","string","integer","string","file","integer"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("wgx", 3, 0,
            new String[]{"integer","file","string"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("rgx", 3, 0,
            new String[]{"file","string","integer"}, new String[]{}));
        addTemplateInternal(new BlockTemplate("fb_rch", 8, 0,
            new String[]{"integer","string","integer","string","integer","integer","integer","integer"}, new String[]{}));
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
            
            JButton blockBtn = new JButton("<html><center><b>" + template.name + "</b><br><font size='2'>" + 
                desc[0] + "</font></center></html>");
            blockBtn.setAlignmentX(Component.LEFT_ALIGNMENT);
            blockBtn.setPreferredSize(new Dimension(150, 50));
            blockBtn.setMinimumSize(new Dimension(150, 50));
            blockBtn.setMaximumSize(new Dimension(150, 50));
            blockBtn.setToolTipText("<html><b>" + desc[0] + "</b><br>" + desc[1] + "</html>");
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
                            g.gridx = 1;
                            JTextField field = new JTextField(cur, 22);
                            field.setToolTipText("Appears as $NAME in generated script — Ctrl+V to paste");
                            field.addHierarchyListener(he -> {
                                if ((he.getChangeFlags() & java.awt.event.HierarchyEvent.SHOWING_CHANGED) != 0 && field.isShowing())
                                    SwingUtilities.invokeLater(field::requestFocusInWindow);
                            });
                            p.add(field, g);
                            g.gridx = 0; g.gridy = 4; g.gridwidth = 2;
                            p.add(new JLabel("<html><font color='gray'><i>e.g. UP_REG_FILE, MERGED_GRAPH, MY_OUTPUT</i></font></html>"), g);
                            int res = JOptionPane.showConfirmDialog(gui4.this, p,
                                    "Name output of " + theConn.from.name,
                                    JOptionPane.OK_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE);
                            if (res == JOptionPane.OK_OPTION) {
                                String v = field.getText().trim().toUpperCase().replace(" ", "_").replace("-", "_");
                                theConn.fileName = v.isEmpty() ? null : v;
                                repaint();
                            }
                        });
                        contextMenu.add(setVarItem);
                        if (selectedConnection.fileName != null && !selectedConnection.fileName.trim().isEmpty()) {
                            JMenuItem curItem = new JMenuItem("  \u2192  $" + selectedConnection.fileName);
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
            if (type == null) return Color.DARK_GRAY;
            String normalizedType = type.trim().toLowerCase();
            switch (normalizedType) {
                case "float": return new Color(139, 0, 0);
                case "integer": return new Color(0, 0, 139);
                case "int": return new Color(0, 0, 139);
                case "string": return new Color(255, 140, 0);
                case "file": return new Color(199, 21, 133);
                case "graph": return new Color(0, 139, 139);
                case "status": return new Color(0, 100, 0);
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
                drawSmartConnection(g2, from, to, conn.from, conn.to, conn.type, conn == selectedConnection, conn.fileName);
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
        private void drawSmartConnection(Graphics2D g2, Point from, Point to, FunctionBlock fromBlock, FunctionBlock toBlock, String type, boolean isSelected, String fileName) {
            Color connColor = getConnectionColor(type);
            g2.setColor(isSelected ? Color.RED : connColor);
            g2.setStroke(new BasicStroke(2));
            boolean needsDetour = (from.x > to.x - 30) || needsRouting(from, to, fromBlock, toBlock);
            if (needsDetour) {
                drawOrthogonalConnection(g2, from, to, fromBlock, toBlock, type, isSelected, fileName);
            } else {
                drawCurvedArrow(g2, from, to, type, isSelected, fileName);
            }
        }
        private void drawOrthogonalConnection(Graphics2D g2, Point from, Point to, FunctionBlock fromBlock, FunctionBlock toBlock, String type, boolean isSelected, String fileName) {
            Color connColor = getConnectionColor(type);
            g2.setColor(isSelected ? Color.RED : connColor);
            g2.setStroke(new BasicStroke(2));
            Rectangle fromBounds = fromBlock.getBounds();
            Rectangle toBounds = toBlock.getBounds();
            
            int fromTop = (int)(fromBounds.y / zoomFactor);
            int fromBottom = (int)((fromBounds.y + fromBounds.height) / zoomFactor);
            int toTop = (int)(toBounds.y / zoomFactor);
            int toBottom = (int)((toBounds.y + toBounds.height) / zoomFactor);
            int offsetX = 40;
            int offsetY = 30;
            
            List<Point> points = new ArrayList<>();
            points.add(from);
            boolean routeAbove = (from.y < to.y) || (fromTop < toTop);
            
            if (from.x > to.x - 30) {
                int midX1 = from.x + offsetX;
                int midX2 = to.x - offsetX;
                
                int minTop = Math.min(fromTop, toTop) - offsetY;
                int maxBottom = Math.max(fromBottom, toBottom) + offsetY;
                
                int midY = routeAbove ? minTop : maxBottom;
                
                for (FunctionBlock block : functionBlocks) {
                    if (block != fromBlock && block != toBlock) {
                        Rectangle bounds = block.getBounds();
                        int bTop = (int)(bounds.y / zoomFactor) - 15;
                        int bBottom = (int)((bounds.y + bounds.height) / zoomFactor) + 15;
                        int bLeft = (int)(bounds.x / zoomFactor) - 15;
                        int bRight = (int)((bounds.x + bounds.width) / zoomFactor) + 15;
                        
                        if (midY >= bTop && midY <= bBottom) {
                            if (routeAbove) {
                                midY = Math.min(midY, bTop - 20);
                            } else {
                                midY = Math.max(midY, bBottom + 20);
                            }
                        }
                    }
                }
                
                points.add(new Point(midX1, from.y));
                points.add(new Point(midX1, midY));
                points.add(new Point(midX2, midY));
                points.add(new Point(midX2, to.y));
            } else {
                int midX = (from.x + to.x) / 2;
                int minTop = Math.min(fromTop, toTop) - offsetY;
                int maxBottom = Math.max(fromBottom, toBottom) + offsetY;
                
                int midY = routeAbove ? minTop : maxBottom;
                
                points.add(new Point(midX, from.y));
                points.add(new Point(midX, midY));
                points.add(new Point(midX, to.y));
            }
            
            points.add(to);
            Path2D path = new Path2D.Double();
            path.moveTo(points.get(0).x, points.get(0).y);
            for (int i = 1; i < points.size(); i++) {
                path.lineTo(points.get(i).x, points.get(i).y);
            }
            g2.draw(path);
            for (int i = 0; i < points.size() - 1; i++) {
                Point p1 = points.get(i);
                Point p2 = points.get(i + 1);
                drawDirectionMarkers(g2, p1, p2, isSelected ? Color.RED : connColor);
            }
            Point lastPoint = points.get(points.size() - 1);
            Point secondLast = points.get(points.size() - 2);
            drawArrowHead(g2, secondLast.x, secondLast.y, lastPoint.x, lastPoint.y, isSelected ? Color.RED : connColor);
            int midIdx = points.size() / 2;
            Point midP1 = points.get(midIdx - 1);
            Point midP2 = points.get(midIdx);
            int labelX = (midP1.x + midP2.x) / 2;
            int labelY = (midP1.y + midP2.y) / 2;
            
            Font baseFont = new Font("SansSerif", Font.PLAIN, 11);
            g2.setFont(baseFont);
            
            // Build label: show type, and fileName if available for file connections
            String label = type != null ? type : "unknown";
            if (fileName != null && !fileName.trim().isEmpty()) {
                label = label + ": " + fileName;
            }
            
            FontMetrics fm = g2.getFontMetrics();
            int labelWidth = fm.stringWidth(label);
            int labelHeight = fm.getHeight();
            g2.setColor(new Color(255, 255, 255, 200));
            g2.fillRect(labelX - labelWidth/2 - 2, labelY - labelHeight/2 - 2, labelWidth + 4, labelHeight + 4);
            g2.setColor(isSelected ? Color.RED : connColor);
            g2.drawString(label, labelX - labelWidth/2, labelY + labelHeight/4);
            
            // Draw file icon indicator if fileName is set
            if (fileName != null && !fileName.trim().isEmpty()) {
                drawFileIcon(g2, labelX - labelWidth/2 - 16, labelY - 6, isSelected ? Color.RED : connColor);
            }
        }
        private void drawDirectionMarkers(Graphics2D g2, Point from, Point to, Color color) {
            double dx = to.x - from.x;
            double dy = to.y - from.y;
            double length = Math.sqrt(dx * dx + dy * dy);
            
            if (length < 30) return;
            
            double angle = Math.atan2(dy, dx);
            
            int numMarkers = Math.max(1, (int)(length / 50));
            
            g2.setColor(color);
            g2.setStroke(new BasicStroke(2));
            
            for (int i = 1; i <= numMarkers; i++) {
                double t = i / (double)(numMarkers + 1);
                double mx = from.x + dx * t;
                double my = from.y + dy * t;
                
                int chevronSize = 6;
                double chevronAngle = Math.PI / 6;
                
                int x1 = (int)(mx - chevronSize * Math.cos(angle - chevronAngle));
                int y1 = (int)(my - chevronSize * Math.sin(angle - chevronAngle));
                int x2 = (int)(mx - chevronSize * Math.cos(angle + chevronAngle));
                int y2 = (int)(my - chevronSize * Math.sin(angle + chevronAngle));
                
                g2.drawLine(x1, y1, (int)mx, (int)my);
                g2.drawLine(x2, y2, (int)mx, (int)my);
            }
        }
        private void drawCurvedArrow(Graphics2D g2, Point from, Point to, String type, boolean isSelected, String fileName) {
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
            drawCurveDirectionMarkers(g2, from, to, ctrlX, ctrlY, isSelected ? Color.RED : connColor);
            double t = 0.5;
            double oneMinusT = 1 - t;
            double midX = oneMinusT * oneMinusT * from.x + 2 * oneMinusT * t * ctrlX + t * t * to.x;
            double midY = oneMinusT * oneMinusT * from.y + 2 * oneMinusT * t * ctrlY + t * t * to.y;
            double dxTangent = 2 * (1 - t) * (ctrlX - from.x) + 2 * t * (to.x - ctrlX);
            double dyTangent = 2 * (1 - t) * (ctrlY - from.y) + 2 * t * (to.y - ctrlY);
            double angle = Math.atan2(dyTangent, dxTangent);
            Font baseFont = new Font("SansSerif", Font.PLAIN, 11);
            g2.setFont(baseFont);
            AffineTransform old = g2.getTransform();
            g2.translate(midX, midY);
            g2.rotate(angle);
            
            // Build label: show type, and fileName if available for file connections
            String label = type != null ? type : "unknown";
            if (fileName != null && !fileName.trim().isEmpty()) {
                label = label + ": " + fileName;
            }
            
            FontMetrics fm = g2.getFontMetrics();
            int labelWidth = fm.stringWidth(label);
            g2.setColor(new Color(255, 255, 255, 200));
            g2.fillRect(-labelWidth/2 - 2, -fm.getHeight() + 2, labelWidth + 4, fm.getHeight() + 2);
            g2.setColor(isSelected ? Color.RED : connColor);
            g2.drawString(label, -labelWidth/2, -3);
            
            // Draw file icon indicator if fileName is set
            if (fileName != null && !fileName.trim().isEmpty()) {
                drawFileIcon(g2, -labelWidth/2 - 16, -10, isSelected ? Color.RED : connColor);
            }
            
            g2.setTransform(old);
            
            drawArrowHead(g2, ctrlX, ctrlY, to.x, to.y, isSelected ? Color.RED : connColor);
        }
        
        // Draw a small file icon to visually indicate a named file connection
        private void drawFileIcon(Graphics2D g2, int x, int y, Color color) {
            g2.setColor(color);
            g2.setStroke(new BasicStroke(1.2f));
            // Draw file shape
            int w = 10, h = 12;
            int fold = 3;
            int[] xPoints = {x, x + w - fold, x + w, x + w, x};
            int[] yPoints = {y, y, y + fold, y + h, y + h};
            g2.drawPolygon(xPoints, yPoints, 5);
            // Draw fold corner
            g2.drawLine(x + w - fold, y, x + w - fold, y + fold);
            g2.drawLine(x + w - fold, y + fold, x + w, y + fold);
            // Draw lines on file
            g2.drawLine(x + 2, y + 5, x + w - 2, y + 5);
            g2.drawLine(x + 2, y + 8, x + w - 2, y + 8);
        }
        
        private void drawCurveDirectionMarkers(Graphics2D g2, Point from, Point to, int ctrlX, int ctrlY, Color color) {
            g2.setColor(color);
            g2.setStroke(new BasicStroke(2));
            
            double[] positions = {0.2, 0.4, 0.6, 0.8};
            
            for (double t : positions) {
                double oneMinusT = 1 - t;
                
                double mx = oneMinusT * oneMinusT * from.x + 2 * oneMinusT * t * ctrlX + t * t * to.x;
                double my = oneMinusT * oneMinusT * from.y + 2 * oneMinusT * t * ctrlY + t * t * to.y;
                
                double dxTangent = 2 * (1 - t) * (ctrlX - from.x) + 2 * t * (to.x - ctrlX);
                double dyTangent = 2 * (1 - t) * (ctrlY - from.y) + 2 * t * (to.y - ctrlY);
                double angle = Math.atan2(dyTangent, dxTangent);
                
                int chevronSize = 6;
                double chevronAngle = Math.PI / 6;
                
                int x1 = (int)(mx - chevronSize * Math.cos(angle - chevronAngle));
                int y1 = (int)(my - chevronSize * Math.sin(angle - chevronAngle));
                int x2 = (int)(mx - chevronSize * Math.cos(angle + chevronAngle));
                int y2 = (int)(my - chevronSize * Math.sin(angle + chevronAngle));
                
                g2.drawLine(x1, y1, (int)mx, (int)my);
                g2.drawLine(x2, y2, (int)mx, (int)my);
            }
        }
        private void drawArrowHead(Graphics2D g2, int x1, int y1, int x2, int y2, Color color) {
            double angle = Math.atan2(y2 - y1, x2 - x1);
            int arrowLength = 12;
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
            String[] desc = getBlockDescription(name);
            setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(isSelected ? Color.BLUE : Color.GRAY, isSelected ? 2 : 1),
                "<html><b>" + name + "</b><br><font size='2' color='gray'>" + desc[0] + "</font></html>"
            ));
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
                // Check for file-entry connection on this input
                FileEntryConnection fec = null;
                for (FileEntryConnection f : fileEntryConnections) {
                    if (f.toBlock == this && f.toInputIndex == idx) { fec = f; break; }
                }
                if (fec != null) {
                    // Show badge: [entryFilePosition] — the sequential number from the data file
                    Color badgeColor = getFilePanelTypeColor(fec.entryType);
                    JLabel badge = new JLabel("[" + fec.entryPosition + "]");
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
                    inSlot.add(badge);
                } else {
                    JLabel inArrow = inputArrows[idx];
                    if (inArrow != null) inSlot.add(inArrow);
                }
                inputPanel.add(inSlot);
            }
            inputPanel.revalidate();
            inputPanel.repaint();
            recomputeSize();
        }
        FunctionBlock(String name, BlockTemplate template) {
            super();
            this.name = name;
            this.originalName = name;
            this.nameHistory = new ArrayList<>();
            this.nameHistory.add(name);
            this.template = template;
            setLayout(new BorderLayout(10, 10));
            
            String[] desc = getBlockDescription(name);
            setBorder(BorderFactory.createTitledBorder(
                BorderFactory.createLineBorder(Color.GRAY, 1),
                "<html><b>" + name + "</b><br><font size='2' color='gray'>" + desc[0] + "</font></html>"
            ));
            
            setBackground(new Color(230, 230, 250));
            setOpaque(true);
            
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
                                JPanel p = new JPanel(new java.awt.GridBagLayout());
                                java.awt.GridBagConstraints gbc = new java.awt.GridBagConstraints();
                                gbc.insets = new Insets(4, 4, 4, 4);
                                gbc.gridx = 0; gbc.gridy = 0; gbc.anchor = java.awt.GridBagConstraints.WEST;
                                p.add(new JLabel("<html>Variable name for Output " + (oidx+1) + ":<br><i>Will appear as $NAME in generated script</i></html>"), gbc);
                                gbc.gridy = 1; gbc.fill = java.awt.GridBagConstraints.HORIZONTAL; gbc.weightx = 1.0;
                                p.add(tf, gbc);
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
            // 32px per slot row + 80px for title border + padding
            int h = Math.max(120, numSlots * 32 + 80);
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
            if (type == null) return Color.DARK_GRAY;
            String normalizedType = type.trim().toLowerCase();
            switch (normalizedType) {
                case "float": return new Color(139, 0, 0);
                case "integer": case "int": return new Color(0, 0, 139);
                case "string": return new Color(255, 140, 0);
                case "file": return new Color(199, 21, 133);
                case "graph": return new Color(0, 139, 139);
                case "status": return new Color(0, 100, 0);
                case "character": case "char": return new Color(184, 134, 11);
                default: return Color.DARK_GRAY;
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
            this.setLayout(new BorderLayout(10, 10));
            updateBorder();
            this.setBackground(new Color(230, 230, 250));
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
    private void generateScriptFromPipeline() {
        List<FunctionBlock> order = getTopologicalOrder();
        if (order == null) return;
        generateAdvtempScript(order);
    }

    private String guiCmd(java.util.Map<String, java.util.Deque<FunctionBlock>> q, String blockType, String defaultCmd) {
        java.util.Deque<FunctionBlock> deq = q.get(blockType);
        if (deq == null || deq.isEmpty()) return defaultCmd;
        FunctionBlock fb = deq.poll();
        String[] inputs = resolveInputsForScript(fb);
        StringBuilder cmd = new StringBuilder(blockType);
        for (String inp : inputs) cmd.append(" ").append(inp);
        if (!blockType.equals("start")) {
            for (String out : resolveOutputsForScript(fb)) cmd.append(" ").append(out);
        }
        return cmd.toString();
    }

    private void generateAdvtempScript(List<FunctionBlock> order) {
        JFileChooser chooser = new JFileChooser();
        chooser.setDialogTitle("Save Generated Script");
        chooser.setSelectedFile(new java.io.File("generated_script.txt"));
        if (chooser.showSaveDialog(this) != JFileChooser.APPROVE_OPTION) return;
        java.io.File outFile = chooser.getSelectedFile();
        try (PrintWriter w = new PrintWriter(outFile, "UTF-8")) {
            w.println(";; ");
            w.println(";; To display SVG file on browser, use \"dre 0 t 0 0 0 0 0 0 graph\"");
            w.println(";; To display on xdot (searchable), use \"exec xdot t_graph.dot\" after");
            w.println(";; \"dre 0 t 0 0 0 0 0 0 graph\"");
            w.println(";;");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; DO NOT CHANGE THE NEXT FEW LINES.  DO NOT MOVE THEM LATER IN THIS FILE.");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $ONLY_ACT_EDGES_TO_TARGET 1");
            w.println("let $ONLY_EXPR_EDGES_TO_TARGET 2");
            w.println("let $BOTH_ACT_EXPR_EDGES_TO_TARGET 0");
            w.println();
            w.println(";; ***********");
            w.println(";; Basic inputs read from GUI");
            w.println(";; Potentially changes for every run");
            w.println(";; ***********");
            w.println();
            w.println("let $WORK_DIR %__1__%");
            w.println("let $SRC_NODE hsa%__2__%");
            w.println("let $TGT_NODE hsa%__3__%");
            w.println("let $NODE_TO_TEST_FOR_SIGNIFICANCE hsa%__4__%");
            w.println("let $REACH_PATH_BOUND %__5__%");
            w.println("let $EDGE_RELAX_LB %__6__%");
            w.println("let $EDGE_RELAX_UB %__7__%");
            w.println("let $NODE_RELAX_LB %__8__%");
            w.println("let $NODE_RELAX_UB %__9__%");
            w.println("let $LOGFOLDCHANGES %__10__%");
            w.println("let $ADDITIONALEDGES %__11__%");
            w.println("let $UP_REG_THRESH %__12__%");
            w.println("let $DOWN_REG_THRESH %__13__%");
            w.println();
            w.println("let $EDGES_TO_TARGET %__14__%");
            w.println();
            w.println("let $LOG_FOLD_CHANGE_FILE %__lf__%");
            w.println("let $ADDITIONAL_EDGES_FILE %__ef__%");
            w.println();
            w.println(";; ************");
            w.println(";; Advanced inputs read from GUI or from last run");
            w.println(";; May not change for every run");
            w.println(";; ************");
            w.println();
            w.println("let $NODE_SPLIT_THRESHOLD %__a1__%");
            w.println("let $CONSTR_SOLVER_TIMEOUT1 %__a2__%");
            w.println("let $CONSTR_SOLVER_TIMEOUT2 %__a3__%");
            w.println("let $NUM_SOLNS_TO_COUNT %__a4__%");
            w.println("let $NUM_SOLNS_TO_EXPLORE %__a5__%");
            w.println(";; let $COMMON_FILES_DIR %__a6__%");
            w.println("let $ESSENTIAL_EDGES_FILE %__a7__%");
            w.println("let $AVOID_EDGES_FILE %__a8__%");
            w.println("let $INACTIVE_NODES_FILE %__a9__%");
            w.println("let $CONFIRMED_UP_REG_FILE %__a10__%");
            w.println("let $CONFIRMED_DOWN_REG_FILE %__a11__%");
            w.println("let $RELAXED_NODES_FILE %__a12__%");
            w.println("let $RELAXED_EDGES_FILE %__a13__%");
            w.println("let $NONRELAXED_EDGES_FILE %__a14__%");
            w.println("let $INTER_DB_MAP_FILE %__a15__%");
            w.println("let $HSA_TO_GENE_SYMBOL_MAP_FILE %__a16__%");
            w.println("let $HSA_PATH_TO_PATH_NAME_MAP_FILE %__a17__%");
            w.println("%__c__%let $LIST_OF_MERGED_KEGG_XML_FILE %__a18__%");
            w.println("let $MERGED_KEGG_PATHWAYS_XML_FILE %__a19__%");
            w.println("let $HSA_IDS_NOT_TO_BE_MERGED_FILE %__a20__%");
            w.println("let $COEXPRESSION_CSV %__a21__%");
            w.println("let $COEXP_THRESH %__a22__%");
            w.println("let $FROZEN_THRESH %__a23__%");
            w.println();
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; DO NOT CHANGE THE ABOVE FEW LINES.  DO NOT MOVE THEM LATER IN THIS FILE.");
            w.println(";; **********************************************************************");
            w.println();
            w.println();
            w.println(";; ********************");
            w.println(";; All template files should have the same part above this point");
            w.println(";; Template files should only differ in the part below this point");
            w.println(";; ********************");
            w.println();
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Coefficients and exponents for edge weights in min-cut based analysis");
            w.println(";; Currently not used.");
            w.println(";; DO NOT CHANGE UNLESS YOU ARE SURE");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $COEFF1 1.0");
            w.println("let $EXP1 -1.0");
            w.println("let $COEFF2 1.0");
            w.println("let $EXP2 -1.0");
            w.println("let $COEFF3 1.0");
            w.println("let $EXP3 -1.0");
            w.println("let $COEFF4 1.0");
            w.println("let $EXP4 -1.0");
            w.println();
            w.println(";; **************");
            w.println(";; Main script starts here");
            w.println(";; **************");
            w.println();
            w.println();
            w.println("let $WRITE_SUBDIR $SRC_NODE_to_$TGT_NODE_fsc_$NODE_TO_TEST_FOR_SIGNIFICANCE_$LOGFOLDCHANGES_b$REACH_PATH_BOUND");
            w.println("let $WRITE_DIR $WORK_DIR/$WRITE_SUBDIR");
            w.println();
            w.println("exec if [ -d $WRITE_DIR ] ; then /bin/rm -rf $WRITE_DIR; fi");
            w.println("exec mkdir $WRITE_DIR");
            w.println();
            w.println("let $NEW_LIST_OF_MERGED_XML_FILE $WRITE_DIR/list_of_xml_files_new.txt");
            w.println("let $MERGED_PATHWAYS_XML_FILE $WRITE_DIR/merged_master_graph.xml");
            w.println("let $ADDITIONAL_EDGES_XML_FILE $WRITE_DIR/additional_edges.xml");
            w.println("let $EXCEPTION_TO_NODE_MERGE_FILE $WRITE_DIR/exception_file.txt");
            w.println("let $UP_REG_FILE $WRITE_DIR/up_reg");
            w.println("let $DOWN_REG_FILE $WRITE_DIR/down_reg");
            w.println("let $ESSENTIAL_NODES_FILE_WITH_FSC $WRITE_DIR/essential_nodes_file_w_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $ESSENTIAL_NODES_FILE_WITHOUT_FSC $WRITE_DIR/essential_nodes_file_wo_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $ACTIVE_NODES_FILE_WITH_FSC $WRITE_DIR/active_nodes_file_w_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $ACTIVE_NODES_FILE_WITHOUT_FSC $WRITE_DIR/active_nodes_file_wo_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $NONRELAXED_NODES_FILE_WITH_FSC $WRITE_DIR/nonrelaxed_nodes_file_w_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $NONRELAXED_NODES_FILE_WITHOUT_FSC $WRITE_DIR/nonrelaxed_nodes_file_wo_$NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println("let $CONNECT_PAIRS_FILE $WRITE_DIR/connect_pairs");
            w.println("let $SRC_FILE $WRITE_DIR/src_file");
            w.println("let $TGT_FILE $WRITE_DIR/tgt_file");
            w.println();
            // Build per-type queues from the pipeline order
            java.util.Set<String> STANDARD_TYPES = new java.util.HashSet<>(java.util.Arrays.asList(
                    "start", "mff", "wgx", "rgx", "fb_rch", "pathz3", "cudf"));
            java.util.Map<String, java.util.Deque<FunctionBlock>> guiQ = new java.util.LinkedHashMap<>();
            java.util.List<FunctionBlock> extraBlocks = new java.util.ArrayList<>();
            for (FunctionBlock fb : order) {
                String baseName = fb.originalName.replaceAll("_\\d+$", "");
                if (STANDARD_TYPES.contains(baseName)) {
                    guiQ.computeIfAbsent(baseName, k -> new java.util.ArrayDeque<>()).add(fb);
                } else {
                    extraBlocks.add(fb);
                }
            }
            w.println(";; **********************************************************************");
            w.println(";; create up-reg and down-reg files from fold change file and thresholds");
            w.println(";; **********************************************************************");
            w.println();
            // cudf — only if a cudf GUI block is connected
            { String c = guiCmd(guiQ, "cudf", null);
              if (c != null) { w.println(c); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Now create some necessary files from src_node and tgt_node information");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec cd $WRITE_DIR; echo $SRC_NODE > $SRC_FILE; echo $TGT_NODE > $TGT_FILE");
            w.println("exec cd $WRITE_DIR;");
            w.println("exec echo $SRC_NODE $TGT_NODE > $CONNECT_PAIRS_FILE");
            w.println("exec cat $UP_REG_FILE $DOWN_REG_FILE $SRC_FILE $TGT_FILE $HSA_IDS_NOT_TO_BE_MERGED_FILE > $EXCEPTION_TO_NODE_MERGE_FILE");
            w.println("exec touch $WRITE_DIR/empty_file");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Read the following global files before starting execution of the pipeline");
            w.println(";; **********************************************************************");
            w.println();
            // start — only if GUI block connected
            { String c = guiCmd(guiQ, "start", null);
              if (c != null) { w.println(c); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Merging KEGG pathways to obtain merged KEGG (only) master network");
            w.println(";; If this has already been done, this step can be skipped.");
            w.println(";;");
            w.println(";; Fifth argument is n: if these files were not previously written by the tool, otherwise 'y'");
            w.println(";; **********************************************************************");
            w.println();
            // first mff — consume GUI block, print exact template line with %__c__%
            { java.util.Deque<FunctionBlock> mffQ = guiQ.get("mff");
              if (mffQ != null && !mffQ.isEmpty()) mffQ.poll();
              w.println("%__c__%mff $LIST_OF_MERGED_KEGG_XML_FILE xml $NODE_SPLIT_THRESHOLD n $INTER_DB_MAP_FILE -1");
              w.println(); }
            w.println(";; **********************************************************************");
            w.println(";; Writing merged graph into xml file, end with string for pathway.");
            w.println(";; Currently using 'KEGG'.");
            w.println(";; This file can be later read in instead of merging KEGG pathways afresh.");
            w.println(";; **********************************************************************");
            w.println();
            // first wgx — consume GUI block, print exact template line with %__c__%
            { java.util.Deque<FunctionBlock> wgxQ = guiQ.get("wgx");
              if (wgxQ != null && !wgxQ.isEmpty()) wgxQ.poll();
              w.println("%__c__%wgx 0 $MERGED_KEGG_PATHWAYS_XML_FILE KEGG");
              w.println(); }
            w.println(";; **********************************************************************");
            w.println(";; Processing information about new edges to be added, if needed.");
            w.println(";; If no new edges are being added, skip this step");
            w.println(";; **********************************************************************");
            w.println();
            w.println("%__c__%exec add $ADDITIONAL_EDGES_FILE $ADDITIONAL_EDGES_XML_FILE");
            w.println("exec if [ -f $NEW_LIST_OF_MERGED_XML_FILE ] ; then /bin/rm $NEW_LIST_OF_MERGED_XML_FILE; fi");
            w.println("exec echo $MERGED_KEGG_PATHWAYS_XML_FILE | cat > $NEW_LIST_OF_MERGED_XML_FILE");
            w.println("%__c__%exec echo $ADDITIONAL_EDGES_XML_FILE | cat >>  $NEW_LIST_OF_MERGED_XML_FILE");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Merging KEGG pathways and user provided additional edges to obtain");
            w.println(";; master network on which analysis is to be done.");
            w.println(";; Fifth argument is n: if these files were not previously written by the tool, otherwise 'y'");
            w.println(";; **********************************************************************");
            w.println();
            // second mff — only if GUI block connected
            { String c = guiCmd(guiQ, "mff", null);
              if (c != null) { w.println(c); w.println("size 0"); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Writing merged graph into xml file, end with string for pathway.");
            w.println(";; Currently using 'KEGG' as pathway name.");
            w.println(";; Helps for debugging to have the graph on which the final analysis is done.");
            w.println(";; **********************************************************************");
            w.println();
            // second wgx — only if GUI block connected
            { String c = guiCmd(guiQ, "wgx", null);
              if (c != null) { w.println(c); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Saving information about the merged edges");
            w.println(";; May be helpful for debugging purposes");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec mv edges_merged_filename $WRITE_DIR/merged_edges_file");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Starting analysis with $NODE_TO_TEST_FOR_SIGNIFICANCE included");
            w.println(";; **********************************************************************");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Read file written in xml format");
            w.println(";; **********************************************************************");
            w.println();
            // rgx — only if GUI block connected
            { String c = guiCmd(guiQ, "rgx", null);
              if (c != null) {
                  w.println(c); w.println("size 0"); w.println();
                  w.println(";; **********************************************************************");
                  w.println(";; Save the graph number");
                  w.println(";; **********************************************************************");
                  w.println();
                  w.println("let $MERGED_GRAPH_NUM #LASTGRAPHNUM"); w.println();
              } }
            w.println(";; **********************************************************************");
            w.println(";; Reachability (forward and backward) based pruning of paths");
            w.println(";; Retain only those nodes that appear in some path of length <=");
            w.println(";; $REACH_PATH_BOUND from $SRC_NODE to $TGT_NODE");
            w.println(";; **********************************************************************");
            w.println();
            // first fb_rch — only if GUI block connected
            { String c = guiCmd(guiQ, "fb_rch", null);
              if (c != null) { w.println(c); w.println("size 0"); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Prepare for finding PO points in the above pruned graph");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $ACTIVE_NODES_FILE_WITH_FSC");
            w.println("exec echo $NODE_TO_TEST_FOR_SIGNIFICANCE >> $ACTIVE_NODES_FILE_WITH_FSC");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $NONRELAXED_NODES_FILE_WITH_FSC");
            w.println("exec echo $NODE_TO_TEST_FOR_SIGNIFICANCE >> $NONRELAXED_NODES_FILE_WITH_FSC");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $ESSENTIAL_NODES_FILE_WITH_FSC");
            w.println("exec echo $NODE_TO_TEST_FOR_SIGNIFICANCE >> $ESSENTIAL_NODES_FILE_WITH_FSC");
            w.println();
            w.println("let $AVOID_NODES_FILE_WITH_FSC $WRITE_DIR/empty_file");
            w.println();
            w.println("let $FILENAME_PREFIX_WITH_FSC $WRITE_DIR/temp_w");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Find PO points");
            w.println(";; $PO_SEARCH_MODE_WITH_FSC 0 means that PO points must be explicitly computed.");
            w.println(";; $PO_SEARCH_MODE_WITH_FSC 1 means that we simply want to check if there are");
            w.println(";; solutions at any point of a (previously computed) PO curve.");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $PO_SEARCH_MODE_WITH_FSC 0");
            w.println();
            // pathz3 with FSC — only if GUI block connected
            { String c = guiCmd(guiQ, "pathz3", null);
              if (c != null) { w.println(c); w.println(); } }
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Save PO points to enable plotting of two PO curves (with and without");
            w.println(";; FSC node) together.  Save limits_timefile.txt for debugging purposes");
            w.println(";; (shows how the binary search proceeded when finding PO points)");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $PO_POINTS_WITH_FSC $SRC_NODE_to_$TGT_NODE_w_$NODE_TO_TEST_FOR_SIGNIFICANCE_$LOGFOLDCHANGES_b$REACH_PATH_BOUND");
            w.println("exec mv limits_timefile.txt $WRITE_DIR/$PO_POINTS_WITH_FSC_limits_timefile.txt");
            w.println("exec mv PO.dat $WRITE_DIR/$PO_POINTS_WITH_FSC.dat");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Starting analysis with $NODE_TO_TEST_FOR_SIGNIFICANCE excluded");
            w.println(";; **********************************************************************");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Reachability (forward and backward) based pruning of paths");
            w.println(";; Retain only those nodes that appear in some path of length <=");
            w.println(";; $REACH_PATH_BOUND from $SRC_NODE to $TGT_NODE");
            w.println(";; Reconstructing the same graph as was done earlier.");
            w.println(";; **********************************************************************");
            w.println();
            // second fb_rch — only if GUI block connected
            { String c = guiCmd(guiQ, "fb_rch", null);
              if (c != null) { w.println(c); w.println("size 0"); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Saving information about the forward-backward reachability search");
            w.println(";; May be helpful for debugging purposes");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec mv fb_rch $WRITE_DIR/fb_rch");
            w.println("exec mv fb_rch_back $WRITE_DIR/fb_rch_back");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Prepare for finding PO points");
            w.println(";; Find PO points after asserting that $NODE_TO_TEST_FOR_SIGNIFICANCE");
            w.println(";; is not present (node to be avoided).");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $ACTIVE_NODES_FILE_WITHOUT_FSC");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $NONRELAXED_NODES_FILE_WITHOUT_FSC");
            w.println();
            w.println("exec cat $SRC_FILE $TGT_FILE > $ESSENTIAL_NODES_FILE_WITHOUT_FSC");
            w.println();
            w.println("let $AVOID_NODES_FILE_WITHOUT_FSC $WRITE_DIR/avoid_$NODE_TO_TEST_FOR_SIGNIFICANCE_file");
            w.println("exec if [ -f $AVOID_NODES_FILE_WITHOUT_FSC ] ; then /bin/rm $AVOID_NODES_FILE_WITHOUT_FSC; fi");
            w.println("exec echo $NODE_TO_TEST_FOR_SIGNIFICANCE > $AVOID_NODES_FILE_WITHOUT_FSC");
            w.println();
            w.println("let $FILENAME_PREFIX_WITHOUT_FSC $WRITE_DIR/temp_wo");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Find PO points");
            w.println(";; $PO_SEARCH_MODE_WITHOUT_FSC 0 means that PO points must be explicitly computed.");
            w.println(";; $PO_SEARCH_MODE_WITHOUT_FSC 1 means that we simply want to check if there are");
            w.println(";; solutions at any point of a (previously computed) PO curve.");
            w.println(";; **********************************************************************");
            w.println();
            w.println();
            w.println("let $PO_SEARCH_MODE_WITHOUT_FSC 0");
            w.println();
            // pathz3 without FSC — only if GUI block connected
            { String c = guiCmd(guiQ, "pathz3", null);
              if (c != null) { w.println(c); w.println(); } }
            w.println(";; **********************************************************************");
            w.println(";; Save PO points to enable plotting of two PO curves (with and without");
            w.println(";; FSC node) together.  Save limits_timefile.txt for debugging purposes");
            w.println(";; (shows how the binary search proceeded when finding PO points)");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $PO_POINTS_WITHOUT_FSC $SRC_NODE_to_$TGT_NODE_wo_$NODE_TO_TEST_FOR_SIGNIFICANCE_$LOGFOLDCHANGES_b$REACH_PATH_BOUND");
            w.println("exec mv limits_timefile.txt $WRITE_DIR/$PO_POINTS_WITHOUT_FSC_limits_timefile.txt");
            w.println("exec mv PO.dat $WRITE_DIR/$PO_POINTS_WITHOUT_FSC.dat");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Now plot the PO points");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec cd $WRITE_DIR; gnuplot -p -e \"set xlabel 'Node relaxn'; set ylabel 'Edge relaxn'; set xrange [-0.5:$NODE_RELAX_UB+0.5]; set yrange [-0.5:$EDGE_RELAX_UB+0.5]; plot '$PO_POINTS_WITH_FSC.dat' with lp linecolor 'blue', '$PO_POINTS_WITHOUT_FSC.dat' with lp linecolor 'red'; exit\"");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Save the plot in a file");
            w.println(";; **********************************************************************");
            w.println();
            w.println("let $PLOT_FILE $WRITE_DIR/$SRC_NODE_to_$TGT_NODE_fsc_$NODE_TO_TEST_FOR_SIGNIFICANCE_$LOGFOLDCHANGES_b$REACH_PATH_BOUND.png");
            w.println("exec echo \"Saving plot in $PLOT_FILE ...\"");
            w.println("exec cd $WRITE_DIR; gnuplot -p -e \"set term png; set output '$PLOT_FILE'; set xlabel 'Node relaxn'; set ylabel 'Edge relaxn'; set xrange [-0.5:$NODE_RELAX_UB+0.5]; set yrange [-0.5:$EDGE_RELAX_UB+0.5]; plot '$PO_POINTS_WITH_FSC.dat' with lp linecolor 'blue', '$PO_POINTS_WITHOUT_FSC.dat' with lp linecolor 'red'; exit\"");
            w.println();
            w.println(";; **********************************************************************");
            w.println(";; Saving debug log.");
            w.println(";; May be helpful for debugging purposes");
            w.println(";; **********************************************************************");
            w.println();
            w.println("exec mv mylog.txt $WRITE_DIR/mylog.txt");
            w.println();
            // Extra non-standard GUI blocks and remaining unconsumed standard blocks
            for (FunctionBlock fb : extraBlocks) {
                String baseName = fb.originalName.replaceAll("_\\d+$", "");
                String[] inputs = resolveInputsForScript(fb);
                StringBuilder cmd = new StringBuilder(baseName);
                for (String inp : inputs) cmd.append(" ").append(inp);
                for (String out : resolveOutputsForScript(fb)) cmd.append(" ").append(out);
                w.println(cmd.toString());
                w.println();
            }
            for (java.util.Deque<FunctionBlock> deq : guiQ.values()) {
                while (!deq.isEmpty()) {
                    FunctionBlock fb = deq.poll();
                    String baseName = fb.originalName.replaceAll("_\\d+$", "");
                    String[] inputs = resolveInputsForScript(fb);
                    StringBuilder cmd = new StringBuilder(baseName);
                    for (String inp : inputs) cmd.append(" ").append(inp);
                    if (!baseName.equals("start")) {
                        for (String out : resolveOutputsForScript(fb)) cmd.append(" ").append(out);
                    }
                    w.println(cmd.toString());
                    w.println();
                }
            }
            w.println("exit");
            w.println();
            JOptionPane.showMessageDialog(this, "Script saved to:\n" + outFile.getAbsolutePath(),
                    "Script Generated", JOptionPane.INFORMATION_MESSAGE);
        } catch (IOException ex) {
            JOptionPane.showMessageDialog(this, "Error writing script: " + ex.getMessage(),
                    "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private String[] resolveInputsForScript(FunctionBlock fb) {
        int n = (fb.template != null) ? fb.template.inputCount : 0;
        String[] resolved = new String[n];
        for (int i = 0; i < n; i++) {
            resolved[i] = (fb.inputValues != null && i < fb.inputValues.length)
                    ? fb.inputValues[i] : "";
            for (Connection c : connections) {
                if (c.to == fb && c.toIdx == i) {
                    if (c.fileName != null && !c.fileName.trim().isEmpty()) {
                        resolved[i] = "$" + c.fileName;
                    } else {
                        // Fall back to source block's outputVarNames
                        String srcVar = (c.from.outputVarNames != null && c.fromIdx < c.from.outputVarNames.length)
                                ? c.from.outputVarNames[c.fromIdx] : null;
                        resolved[i] = (srcVar != null && !srcVar.trim().isEmpty())
                                ? "$" + srcVar : "$" + c.from.originalName + "_out" + (c.fromIdx + 1);
                    }
                    break;
                }
            }
            for (FileEntryConnection fec : fileEntryConnections) {
                if (fec.toBlock == fb && fec.toInputIndex == i) {
                    String var = ENTRY_VAR.getOrDefault(fec.entryName,
                            fec.entryName.toUpperCase().replace(" ", "_").replace("-", "_"));
                    resolved[i] = "$" + var;
                    break;
                }
            }
        }
        return resolved;
    }

    private String[] resolveOutputsForScript(FunctionBlock fb) {
        if (fb.template == null || fb.template.outputCount == 0) return new String[0];
        int n = fb.template.outputCount;
        String[] resolved = new String[n];
        for (int i = 0; i < n; i++) {
            // Priority 1: block's own outputVarNames
            if (fb.outputVarNames != null && i < fb.outputVarNames.length
                    && fb.outputVarNames[i] != null && !fb.outputVarNames[i].trim().isEmpty()) {
                resolved[i] = "$" + fb.outputVarNames[i];
                continue;
            }
            // Priority 2: any outgoing connection that has a fileName set
            String fromConn = null;
            for (Connection c : connections) {
                if (c.from == fb && c.fromIdx == i && c.fileName != null && !c.fileName.trim().isEmpty()) {
                    fromConn = "$" + c.fileName;
                    break;
                }
            }
            if (fromConn != null) { resolved[i] = fromConn; continue; }
            // Default: blockname_out<n>
            resolved[i] = "$" + fb.originalName + "_out" + (i + 1);
        }
        return resolved;
    }

    private String getVariableForPosition(int position) {
        switch (position) {
            case 1:  return "$WORK_DIR";
            case 2:  return "$SRC_NODE";
            case 3:  return "$TGT_NODE";
            case 4:  return "$NODE_TO_TEST_FOR_SIGNIFICANCE";
            case 5:  return "$REACH_PATH_BOUND";
            case 6:  return "$EDGE_RELAX_LB";
            case 7:  return "$EDGE_RELAX_UB";
            case 8:  return "$NODE_RELAX_LB";
            case 9:  return "$NODE_RELAX_UB";
            case 10: return "$LOGFOLDCHANGES";
            case 11: return "$ADDITIONALEDGES";
            case 12: return "$UP_REG_THRESH";
            case 13: return "$DOWN_REG_THRESH";
            case 14: return "$EDGES_TO_TARGET";
            default: return "%__" + position + "__%";
        }
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
