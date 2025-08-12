import java.util.HashMap;
import java.util.Map;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import org.json.JSONArray;
import org.json.JSONObject;

public class UserInput {
    private int index;

    private String userName;
    private String sessionName;
    private String workingDirectory;
    private String logFoldChangesFile;
    private double downThreshold , upThreshold ;
    private String[] mappingFile;

    private int nodeLBound, nodeUBound, edgeLBound, edgeUBound;

    private int incSolver, overSolver, solCount, solExplore, nodeSplitThreshold;
    private int edgeRestriction;
    private int signallingPathLength;

    private String txtFile, xmlFile;

    private String additionalEdge, essentialEdge, avoidEdge, relaxedEdge, nonrelaxedEdge;

    private String sourceHsaID, targetHsaID, candidateID, hsaNotMerged;

    private Map<Integer, java.util.List<String>> over = new HashMap<>();

    // public UserInput(int index){
    //     this.index= index;
    // }

    public UserInput(String fileName, int index) {
        this.index= index;

        try {
            String json = Files.readString(Path.of(fileName));
            JSONArray array = new JSONArray(json);
            
                JSONObject obj = array.getJSONObject(index);
                
                String name = obj.optString("sessionName");
                String workDir = obj.optString("workingDirectory", System.getProperty("user.home"));
                String logFold = obj.optString("logFold", "");
                
                String dThres = obj.optString("dThreshold", ""+0);
                String uThres = obj.optString("uThreshold", "0");
                String nodeLB = obj.optString("nodeLBound", "20");
                String nodeUB = obj.optString("nodeUBound", "80");
                String edgeLB = obj.optString("edgeLBound", "20");
                String edgeUB = obj.optString("edgeUBound", "70");

                String incSol = obj.optString("incSolv", "60000");
                String ovSol = obj.optString("overSolv", "150000");
                String solC = obj.optString("solCount", "1");
                String solE = obj.optString("solExplore", "1");

                String pathLength = obj.optString("SignallingPath","50");
                String txt = obj.optString("TxtFile", "");
                String xml = obj.optString("XmlFile","");
                String nodeSplit = obj.optString("nodeSplitThreshold", "15000");
                String restrict= obj.optString("EdgeRestriction", "-1");

                String additionalEdge= obj.optString("AdditionalEdge", "");
                String essentialEdge= obj.optString("EssentialEdge", "");
                String avoidEdge= obj.optString("AvoidEdge", "");
                String relaxedEdge= obj.optString("RelaxedEdge", "");
                String nonrelaxedEdge= obj.optString("NonrelaxedEdge", "");

                String sourcehsaID= obj.optString("SourceHsaID", "");
                String targethsaID= obj.optString("TargetHsaID", "");
                String candidateID= obj.optString("CandidateID", "");
                String hsaNot = obj.optString("HSANotMerged", "");

                String hsaToGene= obj.optString("HsaToGene", "/home/neeraj/FinalFuSION/hsa_to_gene_symbol_map_6334.txt");
                String hsaToPath = obj.optString("HsaToPath", " /home/neeraj/FinalFuSION/hsa_path_to_path_name_map_2822.txt");

                String interDB= obj.optString("InterDB", "");



                this.userName= name;
                this.workingDirectory= workDir;
                this.logFoldChangesFile= logFold;
                this.downThreshold= Double.parseDouble(dThres);
                this.upThreshold= Double.parseDouble(uThres);
                this.incSolver= Integer.parseInt(incSol);
                this.overSolver= Integer.parseInt(ovSol);
                this.solCount= Integer.parseInt(solC);
                this.solExplore= Integer.parseInt(solE);
                this.signallingPathLength= Integer.parseInt(pathLength);
                this.txtFile= txt;
                this.xmlFile= xml;
                this.nodeSplitThreshold= Integer.parseInt(nodeSplit);
                this.nodeLBound= Integer.parseInt(nodeLB);
                this.nodeUBound= Integer.parseInt(nodeUB);
                this.edgeLBound= Integer.parseInt(edgeLB);
                this.edgeUBound= Integer.parseInt(edgeUB);
                this.edgeRestriction= Integer.parseInt(restrict);
                this.additionalEdge= additionalEdge;
                this.essentialEdge= essentialEdge;
                this.avoidEdge= avoidEdge;
                this.relaxedEdge= relaxedEdge;
                this.nonrelaxedEdge= nonrelaxedEdge;
                this.sourceHsaID= sourcehsaID;
                this.targetHsaID= targethsaID;
                this.candidateID= candidateID;
                String[] mappingFile= {hsaToGene, hsaToPath, interDB};
                this.mappingFile= mappingFile;

                this.hsaNotMerged= hsaNot;

            
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    public int getIndex(){
        return index;
    }
    public void setIndex(int index){
        this.index= index;
    }

    public String getUsername() {
        return userName;
    }

    public void setUsername(String username) {
        this.userName = username;
    }

    public String getSessionname() {
        return sessionName;
    }

    public void setSessionname(String sessionname) {
        this.sessionName = sessionname;
    }

    public String getWorkingDirectory() {
        return workingDirectory;
    }

    public void setWorkingDirectory(String directory) {
        this.workingDirectory = directory;
    }

    public String getLogFoldChangesFile() {
        return logFoldChangesFile;
    }

    public void setLogFoldChangesFile(String file) {
        this.logFoldChangesFile = file;
    }

    public String getXMLFile() {
        return xmlFile;
    }

    public void setXMLFile(String xmlFile) {
        this.xmlFile = xmlFile;
    }

    public String getTxtFile() {
        return txtFile;
    }

    public void setTxtFile(String txtFile) {
        this.txtFile = txtFile;
    }

    public int[] getSolverConfig() {
        int arr[] = {incSolver, overSolver, solCount, solExplore};
        return arr;
    }

    public void setSolverConfig(int[] arr) {
        this.incSolver = arr[0];
        this.overSolver = arr[1];
        this.solCount = arr[2];
        this.solExplore = arr[3];
    }

    public int getSignallingPathLength() {
        return signallingPathLength;
    }

    public void setSignallingPathLength(int value) {
        this.signallingPathLength = value;
    }

    public int[] getRelaxationBounds() {
        int arr[] = {nodeLBound, nodeUBound, edgeLBound, edgeUBound};
        return arr;
    }

    public void setRelaxationBounds(int[] arr) {
        this.nodeLBound = arr[0];
        this.nodeUBound = arr[1];
        this.edgeLBound = arr[2];
        this.edgeUBound = arr[3];
    }

    public double getDownThreshold() {
        return downThreshold;
    }

    public void setDownThreshold(double value) {
        this.downThreshold = value;
    }

    public double getUpThreshold() {
        return upThreshold;
    }

    public void setUpThreshold(double value) {
        this.upThreshold = value;
    }

    public int getEdgeRestriction(){
        return edgeRestriction;
    }
    public void setEdgeRestriction(int i){
        this.edgeRestriction= i;
    }

    public int getNodeSplitThreshold(){
        return nodeSplitThreshold;
    }
    public void setNodeSplitThreshold(int node){
        this.nodeSplitThreshold= node;
    }
    
    public String[] getEdgesFile(){
        String[] edges={additionalEdge, essentialEdge, avoidEdge, relaxedEdge, nonrelaxedEdge};
        return edges;
    }
    public void setEdgesFile(String[] edges){
        this.additionalEdge= edges[0];
        this.essentialEdge= edges[1];
        this.avoidEdge= edges[2];
        this.relaxedEdge= edges[3];
        this.nonrelaxedEdge= edges[4];
    }

    public String getSourceHsaID(){
        return sourceHsaID;
    }
    public void setSourceHsaID(String id){
        this.sourceHsaID= id;
    }

    public String getTargeHsaID(){
        return targetHsaID;
    }
    public void setTargetHsaID(String id){
        this.targetHsaID= id;
    }

    public String getCandidateID(){
        return candidateID;
    }
    public void setCandidateID(String id){
        this.candidateID= id;
    }

    public String[] getMappingFile(){
        return mappingFile;
    }
    public void setMappingFile(String[] map){
        this.mappingFile= map;
    }

    public String gethsaNotFile(){
        return hsaNotMerged;
    }
    public void sethsaNotFile(String id){
        this.hsaNotMerged= id;
    }

    public void addOverride(Map<Integer, java.util.List<String>> map){
        over=map;
    }
    public java.util.List<String> getOverride(Integer i){
        return over.get(i);
    }
    public boolean isOverrideEmpty(int i){
        if(over.isEmpty()) return true;
        if(!over.containsKey(i)) return true;

        return false;
    }

    public void clearOverride(){
        over.clear();
    }

    public void saveData(){
        String fileName = "sessions/" + getUsername() + ".json";
            
            try {
                String json = Files.readString(Path.of(fileName));
                JSONArray sessionsArray = new JSONArray(json);
                // Session to find

                JSONObject sessionData = sessionsArray.getJSONObject(getIndex());

                sessionData.put("dThreshold", downThreshold);
                sessionData.put("uThreshold", upThreshold);
                
                sessionData.put("AdditionalEdge", additionalEdge);
                sessionData.put("EssentialEdge", essentialEdge);
                sessionData.put("AvoidEdge", avoidEdge);
                sessionData.put("RelaxedEdge", relaxedEdge);
                sessionData.put("NonrelaxedEdge", nonrelaxedEdge);

                sessionData.put("logFold", logFoldChangesFile);

                sessionData.put("workingDirectory", workingDirectory);

                sessionData.put("SignallingPath", signallingPathLength);
                sessionData.put("EdgeRestriction", edgeRestriction);
                sessionData.put("incSolv", incSolver);
                sessionData.put("overSolv", overSolver);
                sessionData.put("solCount", solCount);
                sessionData.put("solExplore", solExplore);

                sessionData.put("nodeLBound", nodeLBound);
                sessionData.put("nodeUBound", nodeUBound);
                sessionData.put("edgeLBound", edgeLBound);
                sessionData.put("edgeUBound", edgeUBound);

                sessionData.put("nodeSplitThreshold", nodeSplitThreshold);
                sessionData.put("XmlFile", xmlFile);
                sessionData.put("TxtFile", txtFile);

                sessionData.put("SourceHsaID", sourceHsaID);
                sessionData.put("TargetHsaID", targetHsaID);
                sessionData.put("CandidateID", candidateID);

                sessionData.put("HSANotMerged", hsaNotMerged);

                sessionData.put("HsaToGene", mappingFile[0]);
                sessionData.put("HsaToPath", mappingFile[1]);
                sessionData.put("InterDB", mappingFile[2]);
                 
                try (FileWriter fw = new FileWriter(fileName)) {
                    fw.write(sessionsArray.toString(2)); // Pretty print with 2 spaces
                } catch (IOException er) {
                    er.printStackTrace();
                }

            } catch (Exception err) {
                err.printStackTrace();
            }
    }

    @Override
    public String toString() {
        return "" + userName + " " + workingDirectory + " " + logFoldChangesFile + " " + downThreshold + " " + upThreshold + " " + nodeLBound + " " + nodeUBound + " " + edgeLBound + " " + " " + edgeUBound + " ";
    }

}
