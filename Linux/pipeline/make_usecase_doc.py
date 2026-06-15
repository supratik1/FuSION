import zipfile, os

OUT = "/home/linux/Arpit/FuSION/Linux/pipeline/FuSION_UseCase_Document.docx"

# ── XML helpers ───────────────────────────────────────────────────────────────

def esc(t): return t.replace("&","&amp;").replace("<","&lt;").replace(">","&gt;")

def run(text, bold=False, size=24, color=None):
    rpr = "<w:rPr>"
    if bold: rpr += "<w:b/>"
    rpr += f"<w:sz w:val='{size}'/><w:szCs w:val='{size}'/>"
    if color: rpr += f"<w:color w:val='{color}'/>"
    rpr += "</w:rPr>"
    return f"<w:r>{rpr}<w:t xml:space='preserve'>{esc(text)}</w:t></w:r>"

def para(text, style="Normal", bold=False, size=24, color=None, spacing_after=120):
    return (f"<w:p><w:pPr><w:pStyle w:val='{style}'/>"
            f"<w:spacing w:after='{spacing_after}'/></w:pPr>"
            + run(text, bold=bold, size=size, color=color) + "</w:p>")

def heading(text, level=1):
    sz = {1:36, 2:28, 3:24}.get(level, 24)
    col = {1:"1F497D", 2:"2E74B5", 3:"404040"}.get(level, "000000")
    return (f"<w:p><w:pPr><w:pStyle w:val='Heading{level}'/>"
            f"<w:spacing w:before='200' w:after='100'/></w:pPr>"
            + run(text, bold=True, size=sz, color=col) + "</w:p>")

def table(headers, rows, col_widths=None):
    n = len(headers)
    if not col_widths:
        w = 9000 // n
        col_widths = [w] * n
    def cell(text, w, is_hdr=False):
        shading = '<w:shd w:val="clear" w:color="auto" w:fill="2E74B5"/>' if is_hdr else \
                  '<w:shd w:val="clear" w:color="auto" w:fill="F2F6FB"/>'
        col = "FFFFFF" if is_hdr else "1A1A2E"
        sz = 18 if is_hdr else 18
        b = True if is_hdr else False
        return (f"<w:tc><w:tcPr><w:tcW w:w='{w}' w:type='dxa'/>"
                f"<w:tcBorders>"
                f"<w:top w:val='single' w:sz='4' w:color='BDD7EE'/>"
                f"<w:bottom w:val='single' w:sz='4' w:color='BDD7EE'/>"
                f"<w:left w:val='single' w:sz='4' w:color='BDD7EE'/>"
                f"<w:right w:val='single' w:sz='4' w:color='BDD7EE'/>"
                f"</w:tcBorders>{shading}</w:tcPr>"
                f"<w:p><w:pPr><w:spacing w:after='60'/><w:jc w:val='left'/></w:pPr>"
                + run(text, bold=b, size=sz, color=col) + "</w:p></w:tc>")
    xml = "<w:tbl><w:tblPr><w:tblW w:w='9000' w:type='dxa'/><w:tblBorders>"
    xml += "<w:insideH w:val='single' w:sz='4' w:color='BDD7EE'/>"
    xml += "<w:insideV w:val='single' w:sz='4' w:color='BDD7EE'/>"
    xml += "</w:tblBorders><w:tblLook w:val='04A0'/></w:tblPr>"
    xml += "<w:tr>" + "".join(cell(h, col_widths[i], True) for i,h in enumerate(headers)) + "</w:tr>"
    for row in rows:
        xml += "<w:tr>" + "".join(cell(str(v), col_widths[i]) for i,v in enumerate(row)) + "</w:tr>"
    xml += "</w:tbl>"
    return xml

def spacer():
    return "<w:p><w:pPr><w:spacing w:after='80'/></w:pPr></w:p>"

def hline():
    return ("<w:p><w:pPr><w:pBdr>"
            "<w:bottom w:val='single' w:sz='6' w:space='1' w:color='2E74B5'/>"
            "</w:pBdr><w:spacing w:after='100'/></w:pPr></w:p>")

# ── Document body ─────────────────────────────────────────────────────────────

body = ""

# Cover
body += para("FuSION", bold=True, size=52, color="1F497D", spacing_after=40)
body += para("Functional Signalling Integration & Optimization Network", bold=False, size=26, color="2E74B5", spacing_after=40)
body += para("Use Case Document — Frontend UI", bold=True, size=28, color="404040", spacing_after=40)
body += para("Version 2.0  |  June 2026", size=20, color="666666", spacing_after=20)
body += para("Prepared by: Development Team", size=20, color="666666", spacing_after=200)
body += hline()
body += spacer()

# Section 1 – Introduction
body += heading("1.  Introduction", 1)
body += hline()
body += para(
    "This document describes the use cases for the updated FuSION frontend (Version 2.0). "
    "It covers all user-facing interactions introduced in the current release, including the "
    "UI redesign, new burger-menu navigation, coexpression input parameters, relaxation bound "
    "steppers, and session-level pipeline persistence. Each use case is presented from the "
    "perspective of the Researcher interacting with the system.", size=22, spacing_after=160)

# Section 2 – Actors
body += heading("2.  Actors", 1)
body += hline()
body += table(
    ["Actor", "Description"],
    [["Researcher", "Primary user — configures inputs and executes pathway analyses."],
     ["System",     "FuSION application — frontend UI and backend engine."]],
    col_widths=[2000, 7000])
body += spacer()

# Section 3 – Use Cases
body += heading("3.  Use Cases", 1)
body += hline()

# ── UC-01 ────────────────────────────────────────────────────────────────────
body += heading("UC-01  —  Login and Account Access", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-01"],
     ["Name",        "Login and Account Access"],
     ["Actor",       "Researcher"],
     ["Precondition","Application is installed and running"],
     ["Trigger",     "User launches FuSION"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays the login screen — centered card with FuSION branding, username and password fields (labels above fields), Sign In button, and a Sign Up link.", size=20)
body += para("2.  User enters credentials and clicks Sign In.", size=20)
body += para("3.  System validates credentials and navigates to the Sessions screen.", size=20)
body += spacer()
body += para("Alternate Flow — Sign Up:", bold=True, size=20)
body += para("1a. User clicks 'Don't have an account? Sign up.'", size=20)
body += para("1b. System shows the Sign Up card with Username, Email, Password, Confirm Password fields.", size=20)
body += para("1c. User fills in details and clicks Sign Up. System creates the account.", size=20)
body += spacer()
body += para("UI Notes:", bold=True, size=20)
body += para("Window supports drag-to-move (drag the header), maximize via the □ button, and free resize by dragging the window edges. Both login and signup cards include maximize and close controls.", size=20)
body += para("Postcondition: Researcher is authenticated and on the Sessions screen.", bold=True, size=20)
body += spacer()

# ── UC-02 ────────────────────────────────────────────────────────────────────
body += heading("UC-02  —  Session Management", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-02"],
     ["Name",        "Manage Sessions"],
     ["Actor",       "Researcher"],
     ["Precondition","Researcher is logged in"],
     ["Trigger",     "Sessions screen is displayed after login"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays all saved sessions in a scrollable list with name and timestamp.", size=20)
body += para("2.  Researcher selects a session and chooses one of the following actions:", size=20)
body += spacer()
body += table(
    ["Button", "Action"],
    [["Configure →", "Opens full configuration flow starting from Working Directory."],
     ["▶  Submit",   "Skips all configuration screens; opens Pipeline Editor with saved pipeline pre-loaded."],
     ["Delete",      "Prompts confirmation, then permanently removes the session."],
     ["New Session", "Prompts for a session name, creates it, and opens the Pipeline Editor."],
     ["Log Out",     "Returns to the Login screen."]],
    col_widths=[2000, 7000])
body += spacer()
body += para("Alternate Flow — No Sessions Exist:", bold=True, size=20)
body += para("3a. The list is empty. Researcher clicks New Session to create the first session.", size=20)
body += para("Postcondition: Researcher enters the desired flow for the selected session.", bold=True, size=20)
body += spacer()

# ── UC-03 ────────────────────────────────────────────────────────────────────
body += heading("UC-03  —  Direct Navigation via Burger Menu", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-03"],
     ["Name",        "Navigate Directly Between Configuration Steps"],
     ["Actor",       "Researcher"],
     ["Precondition","Researcher is on any post-login configuration panel"],
     ["Trigger",     "Researcher clicks the ☰ hamburger button in the header"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  Researcher clicks ☰ in the top-left of the header bar (visible on all panels).", size=20)
body += para("2.  System displays a dropdown listing all 9 configuration steps.", size=20)
body += spacer()
body += table(
    ["#", "Step"],
    [["1", "📁  Working Directory"],
     ["2", "📄  Log Fold Changes"],
     ["3", "🔗  KEGG / XML Files"],
     ["4", "🔢  Reach Path Bound"],
     ["5", "⚖️   Relaxation Bounds"],
     ["6", "🔍  Threshold Filter"],
     ["7", "➡️   Edge Files"],
     ["8", "🔬  Node IDs"],
     ["9", "⚙️   Pipeline Editor"]],
    col_widths=[700, 8300])
body += spacer()
body += para("3.  Researcher selects a step. System navigates directly to that panel, initialised with the current session's saved data.", size=20)
body += para("Postcondition: Researcher is on the selected step without stepping through intermediate screens.", bold=True, size=20)
body += spacer()

# ── UC-04 ────────────────────────────────────────────────────────────────────
body += heading("UC-04  —  Configure Working Directory", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-04"], ["Name", "Set Working Directory"],
     ["Actor", "Researcher"], ["Precondition", "Researcher has entered the configuration flow"],
     ["Trigger", "Configuration flow begins or Researcher navigates here via burger menu"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays the current working directory path inside a bordered card.", size=20)
body += para("2.  Researcher clicks Change Directory. System opens a native folder browser dialog.", size=20)
body += para("3.  Researcher selects a directory. System updates the displayed path.", size=20)
body += para("4.  Researcher clicks Next → to proceed or Save to persist without navigating.", size=20)
body += para("Postcondition: Working directory is set and stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-05 ────────────────────────────────────────────────────────────────────
body += heading("UC-05  —  Select Log Fold Changes File", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-05"], ["Name", "Select Log Fold Changes File"],
     ["Actor", "Researcher"], ["Precondition", "UC-04 completed"],
     ["Trigger", "Researcher arrives at the Log Fold Changes panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays a file card showing the currently selected filename or 'No file selected'.", size=20)
body += para("2.  Researcher clicks Choose File. System opens a file browser.", size=20)
body += para("3.  Researcher selects a .tsv file. System updates the card.", size=20)
body += para("4.  Researcher clicks Next →.", size=20)
body += para("Postcondition: Log fold changes file path is stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-06 ────────────────────────────────────────────────────────────────────
body += heading("UC-06  —  Configure KEGG / XML Input", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-06"], ["Name", "Configure KEGG Pathway Source"],
     ["Actor", "Researcher"], ["Precondition", "UC-05 completed"],
     ["Trigger", "Researcher arrives at the KEGG / XML Files panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays two options via styled radio buttons:", size=20)
body += para("    •  Use an existing merged KEGG XML file", size=20)
body += para("    •  Merge fresh from a list of KEGG XML files", size=20)
body += para("2.  Researcher selects an option. System shows the relevant file chooser card(s).", size=20)
body += para("3.  Researcher chooses files via Choose buttons and sets the Node Split Threshold.", size=20)
body += para("4.  Researcher clicks Next →.", size=20)
body += para("Postcondition: XML/KEGG source files and node split threshold stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-07 ────────────────────────────────────────────────────────────────────
body += heading("UC-07  —  Set Reach Path Bound and Solver Parameters", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-07"], ["Name", "Configure Reach Path Bound"],
     ["Actor", "Researcher"], ["Precondition", "UC-06 completed"],
     ["Trigger", "Researcher arrives at the Reach Path Bound panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays the current reach path bound value and a Configure button.", size=20)
body += para("2.  Researcher clicks Configure. System opens a dialog with four parameter fields:", size=20)
body += para("    •  Incremental Solver Timeout", size=20)
body += para("    •  Overall Solver Timeout", size=20)
body += para("    •  Number of Solutions to Count", size=20)
body += para("    •  Number of Solutions to Explore", size=20)
body += para("3.  Researcher fills in values and clicks OK. Researcher clicks Next →.", size=20)
body += para("Postcondition: Reach path bound and solver parameters stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-08 ────────────────────────────────────────────────────────────────────
body += heading("UC-08  —  Set Relaxation Bounds", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-08"], ["Name", "Set Node and Edge Relaxation Bounds"],
     ["Actor", "Researcher"], ["Precondition", "UC-07 completed"],
     ["Trigger", "Researcher arrives at the Relaxation Bounds panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays two cards: Node Relaxation Bounds and Edge Relaxation Bounds.", size=20)
body += para("2.  Each card contains:", size=20)
body += para("    •  Lower Bound — editable numeric field with − and + buttons", size=20)
body += para("    •  Upper Bound — editable numeric field with − and + buttons", size=20)
body += para("    •  A dual-thumb range slider synced live with the fields", size=20)
body += para("3.  Researcher sets values using any of three methods:", size=20)
body += para("    •  Typing directly into the field and pressing Enter", size=20)
body += para("    •  Clicking − / + to step by 1", size=20)
body += para("    •  Dragging the slider thumbs", size=20)
body += para("4.  All three controls remain in sync. Values are clamped to 0–100.", size=20)
body += para("5.  Researcher clicks Next →.", size=20)
body += para("Postcondition: Node and edge relaxation lower and upper bounds stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-09 ────────────────────────────────────────────────────────────────────
body += heading("UC-09  —  Threshold Filter and Gene Classification", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-09"],
     ["Name",        "Apply Regulatory Thresholds and Classify Genes"],
     ["Actor",       "Researcher"],
     ["Precondition","UC-08 completed; Log Fold Changes file has been selected (UC-05)"],
     ["Trigger",     "Researcher arrives at the Threshold Filter panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Panel Layout:", bold=True, size=20)
body += para("The panel is divided into two areas:", size=20)
body += para("    •  Top bar — threshold input fields, Filter button, Search field, Search button, Override button", size=20)
body += para("    •  Main area — seven scrollable result columns displayed side by side", size=20)
body += spacer()
body += para("Main Flow — Filter:", bold=True, size=20)
body += para("1.  System displays two threshold fields: Down Threshold and Up Threshold, pre-filled with the values saved in the session.", size=20)
body += para("2.  Researcher adjusts the threshold values as needed and clicks Filter.", size=20)
body += para("3.  System reads the Log Fold Changes file (TSV format: HSA ID, score per line).", size=20)
body += para("4.  System classifies each gene/node and populates the seven result columns:", size=20)
body += spacer()
body += table(
    ["Column", "Condition", "Description"],
    [["Up Regulatory Threshold",      "Score >= Up Threshold",       "Genes up-regulated above the upper threshold"],
     ["Not Differentially Expressed", "Lower < Score < Upper",       "Genes between both thresholds; not significantly regulated"],
     ["Down Regulatory Threshold",    "Score <= Down Threshold",     "Genes down-regulated below the lower threshold"],
     ["Active Node File",             "Override assignment",         "Nodes explicitly marked as active by the researcher"],
     ["Inactive Node File",           "Override assignment",         "Nodes explicitly marked as inactive by the researcher"],
     ["Relaxed Node File",            "Override assignment",         "Nodes assigned to the relaxed category"],
     ["Non-Relaxed Node File",        "Override assignment",         "Nodes assigned to the non-relaxed category"]],
    col_widths=[2800, 2200, 4000])
body += spacer()
body += para("5.  Each entry in a column is displayed as: HSA_ID - GeneName (score) if a gene symbol mapping file is loaded; otherwise as HSA_ID (score).", size=20)
body += para("6.  Researcher reviews the classification results.", size=20)
body += spacer()
body += para("Alternate Flow A — Search and Highlight:", bold=True, size=20)
body += para("1.  Researcher types a gene name or HSA ID into the Search field and clicks Search.", size=20)
body += para("2.  System scans all seven columns and highlights any matching entries in green.", size=20)
body += para("3.  If exactly one unique match is found, the Override button becomes enabled.", size=20)
body += para("4.  If multiple different matches are found, the Override button remains disabled (ambiguous entry).", size=20)
body += spacer()
body += para("Alternate Flow B — Override (Manual Re-classification):", bold=True, size=20)
body += para("1.  After searching and finding a unique match, Researcher clicks Override.", size=20)
body += para("2.  System opens the Override dialog showing three groups of radio buttons:", size=20)
body += spacer()
body += table(
    ["Group", "Options"],
    [["Regulatory Threshold",  "Above Up Regulatory Threshold  |  Not Differentially Expressed  |  Below Down Regulatory Threshold"],
     ["Node Activity",         "Active Node File  |  Inactive Node File"],
     ["Node Relaxation",       "Relaxed Node  |  Non-Relaxed Node"]],
    col_widths=[2600, 6400])
body += spacer()
body += para("3.  If the entry was previously overridden, the existing selections are pre-checked.", size=20)
body += para("4.  Researcher selects the desired destination categories across the three groups and clicks OK.", size=20)
body += para("5.  System removes the entry from its current column(s) and appends it to the selected column(s).", size=20)
body += para("6.  Researcher can click Clear to remove all overrides for that entry and revert it to the Filter-computed classification.", size=20)
body += spacer()
body += para("Saving:", bold=True, size=20)
body += para("Researcher clicks Save to persist the current threshold values and all override assignments to the session. Clicking Next → also saves overrides before navigating to the Edge Files panel.", size=20)
body += para("Postcondition: Threshold values and all manual gene/node overrides are stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-10 ────────────────────────────────────────────────────────────────────
body += heading("UC-10  —  Configure Edge Files and Coexpression Settings", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-10"], ["Name", "Select Edge Files and Coexpression Parameters"],
     ["Actor", "Researcher"], ["Precondition", "UC-09 completed"],
     ["Trigger", "Researcher arrives at the Edge Files panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays 8 rows in a scrollable panel, each styled as a card:", size=20)
body += spacer()
body += table(
    ["Row", "Input Type", "Description"],
    [["Additional Edges File",  "Any file",    "Extra edges to add to the network"],
     ["Essential Edge File",    "Any file",    "Edges that must be present"],
     ["Avoid Edge File",        "Any file",    "Edges to exclude from the network"],
     ["Relaxed Edges File",     "Any file",    "Edges with relaxed constraints"],
     ["Non-Relaxed Edges File", "Any file",    "Edges not to be relaxed"],
     ["Coexpression CSV File",  ".csv only",   "Coexpression network data (maps to %__a21__%)"],
     ["COEXP_THRESH",           "Integer",     "Coexpression threshold value (maps to %__a22__%)"],
     ["FROZEN_THRESH",          "Integer",     "Frozen threshold value (maps to %__a23__%)"]],
    col_widths=[2800, 1800, 4400])
body += spacer()
body += para("2.  Researcher selects files via Choose buttons.", size=20)
body += para("3.  For COEXPRESSION_CSV, the file dialog is filtered to .csv files only.", size=20)
body += para("4.  For COEXP_THRESH and FROZEN_THRESH, researcher types a value or uses − / + buttons. Invalid input reverts to the last valid value on focus loss.", size=20)
body += para("5.  Researcher clicks Next →.", size=20)
body += para("Postcondition: All edge file paths and coexpression parameters stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-11 ────────────────────────────────────────────────────────────────────
body += heading("UC-11  —  Enter Node IDs", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-11"], ["Name", "Enter Source, Target, and Candidate HSA IDs"],
     ["Actor", "Researcher"], ["Precondition", "UC-10 completed"],
     ["Trigger", "Researcher arrives at the Node IDs panel"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System displays input rows for Source HSA ID, Target HSA ID, and Candidate Node HSA ID — each with a label above the field and a type dropdown.", size=20)
body += para("2.  Researcher enters numeric HSA IDs.", size=20)
body += para("3.  A file chooser row allows selecting the HSA IDs Not To Be Merged file.", size=20)
body += para("4.  A mapping section shows current mapping files with a Change Mapping Files button.", size=20)
body += para("5.  Researcher clicks Next → to proceed to the Pipeline Editor.", size=20)
body += para("Postcondition: All node IDs and mapping files stored in the session.", bold=True, size=20)
body += spacer()

# ── UC-12 ────────────────────────────────────────────────────────────────────
body += heading("UC-12  —  Save Complete Session Including Pipeline", 2)
body += table(
    ["Field", "Detail"],
    [["Use Case ID", "UC-12"], ["Name", "Save Session with Pipeline State"],
     ["Actor", "Researcher"], ["Precondition", "Researcher is on the Pipeline Editor panel"],
     ["Trigger", "Researcher clicks 💾 Save Session"]],
    col_widths=[2400, 6600])
body += spacer()
body += para("Main Flow:", bold=True, size=20)
body += para("1.  System serializes the current pipeline canvas — all blocks, connections, wire labels, block positions, and zoom level.", size=20)
body += para("2.  System stores the serialized pipeline alongside all configuration parameters in the session JSON file.", size=20)
body += para("3.  System displays confirmation: 'Session saved — pipeline included.'", size=20)
body += para("4.  On next Submit from the Sessions screen, the full pipeline is restored automatically.", size=20)
body += para("Postcondition: Session JSON contains the complete configuration and pipeline state.", bold=True, size=20)
body += spacer()

# Section 4 – Non-Functional Requirements
body += heading("4.  Non-Functional Requirements Addressed", 1)
body += hline()
body += table(
    ["Requirement", "Implementation"],
    [["Usability",       "Burger menu (☰) allows direct navigation to any of the 9 configuration steps from any screen — no forced linear flow."],
     ["Consistency",     "All panels share the same Theme design system — fonts, colours, spacing, and button styles are uniform throughout."],
     ["Responsiveness",  "Window is freely resizable; toolbar in the pipeline editor scrolls horizontally on smaller screens; split pane divider is proportional."],
     ["Persistence",     "All inputs and the full pipeline canvas state are saved per session in a JSON file."],
     ["Precision",       "Relaxation bounds and coexpression thresholds support direct text entry as well as increment/decrement buttons, giving exact control."],
     ["Accessibility",   "Labels appear above input fields for clarity; tooltips are present on all icon buttons; window supports drag, maximize, and resize."]],
    col_widths=[2400, 6600])
body += spacer()

# ── Assemble XML ──────────────────────────────────────────────────────────────

CONTENT_TYPES = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml"  ContentType="application/xml"/>
  <Override PartName="/word/document.xml"
    ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
  <Override PartName="/word/styles.xml"
    ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
</Types>'''

RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1"
    Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument"
    Target="word/document.xml"/>
</Relationships>'''

WORD_RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1"
    Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles"
    Target="styles.xml"/>
</Relationships>'''

STYLES = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:style w:type="paragraph" w:styleId="Normal"><w:name w:val="Normal"/>
    <w:rPr><w:sz w:val="22"/><w:szCs w:val="22"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/>
    <w:pPr><w:keepNext/><w:spacing w:before="240" w:after="120"/></w:pPr>
    <w:rPr><w:b/><w:sz w:val="36"/><w:szCs w:val="36"/><w:color w:val="1F497D"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="Heading2"><w:name w:val="heading 2"/>
    <w:pPr><w:keepNext/><w:spacing w:before="200" w:after="80"/></w:pPr>
    <w:rPr><w:b/><w:sz w:val="28"/><w:szCs w:val="28"/><w:color w:val="2E74B5"/></w:rPr></w:style>
  <w:style w:type="paragraph" w:styleId="Heading3"><w:name w:val="heading 3"/>
    <w:pPr><w:keepNext/><w:spacing w:before="160" w:after="60"/></w:pPr>
    <w:rPr><w:b/><w:sz w:val="24"/><w:szCs w:val="24"/><w:color w:val="404040"/></w:rPr></w:style>
</w:styles>'''

DOCUMENT = f'''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
            xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
  <w:body>
    <w:sectPr>
      <w:pgSz w:w="12240" w:h="15840"/>
      <w:pgMar w:top="1080" w:right="1080" w:bottom="1080" w:left="1080"/>
    </w:sectPr>
    {body}
  </w:body>
</w:document>'''

# ── Write .docx ───────────────────────────────────────────────────────────────

with zipfile.ZipFile(OUT, 'w', zipfile.ZIP_DEFLATED) as z:
    z.writestr("[Content_Types].xml",       CONTENT_TYPES)
    z.writestr("_rels/.rels",               RELS)
    z.writestr("word/_rels/document.xml.rels", WORD_RELS)
    z.writestr("word/styles.xml",           STYLES)
    z.writestr("word/document.xml",         DOCUMENT)

print(f"Created: {OUT}")
