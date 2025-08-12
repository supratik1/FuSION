import java.io.*;
import java.nio.file.*;
import java.util.*;

public class ScriptFile{

    public ScriptFile(UserInput user) throws IOException{

        int relax[]= user.getRelaxationBounds();
        int solver[]= user.getSolverConfig();
        String edges[] = user.getEdgesFile();
        String restrict[]= {"$BOTH_ACT_EXPR_EDGES_TO_TARGET", "$ONLY_ACT_EDGES_TO_TARGET", "$ONLY_EXPR_EDGES_TO_TARGET"};

        // Load template
        String template = Files.readString(Path.of("advtempscript.txt")); // path to your template

        // Example input map (this can be built from your UserInput object)
        Map<String, String> replacements = new HashMap<>();
        replacements.put("%__1__%", user.getWorkingDirectory());
        replacements.put("%__2__%", user.getSourceHsaID());
        replacements.put("%__3__%", user.getTargeHsaID());
        replacements.put("%__4__%",  user.getCandidateID());
        replacements.put("%__5__%", ""+user.getSignallingPathLength());
        replacements.put("%__6__%", ""+relax[2]);

        replacements.put("%__7__%", ""+relax[3]);
        replacements.put("%__8__%", ""+relax[0]);
        replacements.put("%__9__%", ""+relax[1]);
        replacements.put("%__10__%", user.getLogFoldChangesFile());

        replacements.put("%__11__%", edges[0]);
        replacements.put("%__12__%", ""+user.getUpThreshold());
        replacements.put("%__13__%", ""+user.getDownThreshold());
        replacements.put("%__14__%", restrict[user.getEdgeRestriction()]);
        replacements.put("%__lf__%", user.getLogFoldChangesFile());
        
        replacements.put("%__ef__%", edges[0]);

        replacements.put("%__a1__%", ""+user.getNodeSplitThreshold());
        replacements.put("%__a2__%", ""+solver[0]);
        replacements.put("%__a3__%", ""+solver[1]);
        replacements.put("%__a4__%", ""+solver[2]);
        replacements.put("%__a5__%", ""+solver[3]);

        replacements.put("%__a6__%", edges[0]);
        replacements.put("%__a7__%", edges[1]);
        replacements.put("%__a8__%", edges[2]);
        replacements.put("%__a9__%", user.isOverrideEmpty(4)?"":String.join("," ,user.getOverride(4)));
        replacements.put("%__a10__%", user.isOverrideEmpty(0)?"":String.join("," ,user.getOverride(0)));

        replacements.put("%__a11__%", user.isOverrideEmpty(2)?"":String.join("," ,user.getOverride(2)));
        replacements.put("%__a12__%", user.isOverrideEmpty(5)?"":String.join("," ,user.getOverride(5)));
        replacements.put("%__a13__%", edges[3]);
        replacements.put("%__a14__%", edges[4]);
        
        String[] map= user.getMappingFile();

        replacements.put("%__a15__%", map[2]);

        replacements.put("%__a16__%", map[0]);
        replacements.put("%__a17__%", map[1]);
        replacements.put("%__a18__%",""+ user.getTxtFile());
        replacements.put("%__a19__%", ""+user.getXMLFile());
        replacements.put("%__a20__%", user.gethsaNotFile());


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

