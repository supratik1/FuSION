import java.io.*;
import java.nio.file.*;
import java.util.*;

public class ScriptFile{

    public ScriptFile(UserInput user) throws IOException{

        int relax[]= user.getRelaxationBounds();
        int solver[]= user.getSolverConfig();
        String edges[] = user.getEdgesFile();

        // Load template
        String template = Files.readString(Path.of("advtempscript.txt")); // path to your template

        // Example input map (this can be built from your UserInput object)
        Map<String, String> replacements = new HashMap<>();
        replacements.put("%__1__%", user.getWorkingDirectory());
        replacements.put("%__2__%", "Source hsa id");
        replacements.put("%__3__%", "Target hsa id");
        replacements.put("%__4__%", "Candidate Implicated Node");
        replacements.put("%__5__%", ""+user.getSignallingPathLength());
        replacements.put("%__6__%", ""+relax[2]);

        replacements.put("%__7__%", ""+relax[3]);
        replacements.put("%__8__%", ""+relax[0]);
        replacements.put("%__9__%", ""+relax[1]);
        replacements.put("%__10__%", user.getLogFoldChangesFile());

        replacements.put("%__11__%", edges[0]);
        replacements.put("%__12__%", ""+user.getUpThreshold());
        replacements.put("%__13__%", ""+user.getDownThreshold());
        replacements.put("%__14__%", "$ONLY_ACT_EDGES_TO_TARGET");
        replacements.put("%__lf__%", user.getLogFoldChangesFile());
        
        replacements.put("%__ef__%", "Additional Edges");

        replacements.put("%__a1__%", ""+user.getNodeSplitThreshold());
        replacements.put("%__a2__%", ""+solver[0]);
        replacements.put("%__a3__%", ""+solver[1]);
        replacements.put("%__a4__%", ""+solver[2]);
        replacements.put("%__a5__%", ""+solver[3]);

        replacements.put("%__a6__%", edges[0]);
        replacements.put("%__a7__%", edges[1]);
        replacements.put("%__a8__%", edges[2]);
        replacements.put("%__a9__%", "Inactive Node File");
        replacements.put("%__a10__%", "Confirmed Up reg File");

        replacements.put("%__a11__%", "Confirmed Down Reg File");
        replacements.put("%__a12__%", "Relaxed Nodes File");
        replacements.put("%__a13__%", "Relaxed Edges File");
        replacements.put("%__a14__%", "Non Relaxed Edges File");
        replacements.put("%__a15__%", "Cross DataBase Map");

        replacements.put("%__a16__%", "HSA to gene Symbol map");
        replacements.put("%__a17__%", "Hsa Path to path name");
        replacements.put("%__a18__%", user.getTxtFile());
        replacements.put("%__a19__%", ""+user.getXMLFile());
        replacements.put("%__a20__%", "Hsa ids not to be merged");


        // Replace all placeholders
        for (Map.Entry<String, String> entry : replacements.entrySet()) {
            String key =  entry.getKey();
            template = template.replace(key, entry.getValue());
        }

        // Save the result to output file
        Files.writeString(Path.of("output_script.txt"), template);
        System.out.println("Template filled and saved to output_script.txt");
    }
}

