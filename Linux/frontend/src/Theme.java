import java.awt.*;
import javax.swing.*;

/** Centralised design tokens for the FuSION UI — dark navy theme. */
public final class Theme {

    // ── Dark navy palette ─────────────────────────────────────────────────────
    public static final Color BG          = new Color(10,  22,  40);   // #0A1628 page bg
    public static final Color TOOLBAR_BG  = new Color(7,   16,  30);   // #07101E toolbar
    public static final Color SURFACE     = new Color(19,  34,  64);   // #132240 panels
    public static final Color BG_CARD     = new Color(26,  46,  74);   // #1A2E4A cards
    public static final Color CARD_HI     = new Color(31,  53,  85);   // #1F3555 card header
    public static final Color PRIMARY     = new Color(59,  130, 246);  // #3B82F6 accent blue
    public static final Color PRIMARY_DK  = new Color(37,  99,  235);  // #2563EB hover
    public static final Color PRIMARY_HVR = new Color(96,  165, 250);  // #60A5FA light blue
    public static final Color SUCCESS     = new Color(34,  197, 94);   // #22C55E green
    public static final Color SUCCESS_DK  = new Color(21,  163, 74);   // #15A34A
    public static final Color WARNING     = new Color(251, 191, 36);   // #FBBF24 amber
    public static final Color WARNING_DK  = new Color(161, 107, 10);   // #A16B0A dark amber
    public static final Color DANGER      = new Color(239, 68,  68);   // #EF4444 red
    public static final Color BORDER      = new Color(37,  61,  96);   // #253D60 border
    public static final Color BORDER_HI   = new Color(58,  94,  144);  // #3A5E90 active border

    // ── Canvas ───────────────────────────────────────────────────────────────
    public static final Color CANVAS_BG   = new Color(10,  22,  40);
    public static final Color CANVAS_DOT  = new Color(22,  41,  72);

    // ── Text ─────────────────────────────────────────────────────────────────
    public static final Color TEXT_DARK   = new Color(214, 232, 255);  // #D6E8FF hi text
    public static final Color TEXT_MED    = new Color(106, 150, 200);  // #6A96C8 mid text
    public static final Color TEXT_LIGHT  = new Color(100, 140, 190);  // #648CBE dim text

    // ── Type accent colours ───────────────────────────────────────────────────
    public static final Color T_FILE      = new Color(96,  165, 250);
    public static final Color T_INT       = new Color(167, 139, 250);
    public static final Color T_FLOAT     = new Color(244, 114, 182);
    public static final Color T_STRING    = new Color(52,  211, 153);
    public static final Color T_GRAPH     = new Color(34,  211, 238);
    public static final Color T_OTHER     = new Color(148, 163, 184);

    // ── Responsive scaling ───────────────────────────────────────────────────
    // SCALE: layout/spacing (DPI-based, 1.0 on 96-DPI screens)
    // FONT_SCALE: fonts get an extra 1.25× floor so they stay large and readable
    private static final float SCALE;
    private static final float FONT_SCALE;
    static {
        int dpi     = java.awt.Toolkit.getDefaultToolkit().getScreenResolution();
        int screenW = java.awt.Toolkit.getDefaultToolkit().getScreenSize().width;
        // DPI-based scale (Linux often reports 96 even on large/HiDPI screens)
        float dpiScale = Math.max(1.0f, Math.min(dpi / 96.0f, 2.0f));
        // Resolution-based scale: kicks in only above 1920px so current device is unchanged
        float resScale = Math.max(1.0f, Math.min(screenW / 1920.0f, 2.0f));
        SCALE      = Math.max(dpiScale, resScale);
        FONT_SCALE = Math.max(1.25f, SCALE);
    }
    public static int scale(int px) { return Math.round(px * SCALE); }

    // ── Platform font detection (picks best available sans/mono) ─────────────
    private static final String SANS_FONT;
    private static final String MONO_FONT;
    static {
        java.util.Set<String> fam = new java.util.HashSet<>(java.util.Arrays.asList(
            java.awt.GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames()));
        String s = "SansSerif", m = "Monospaced";
        for (String f : new String[]{"Inter","Roboto","Ubuntu","Noto Sans","Segoe UI","SansSerif"})
            if (fam.contains(f)) { s = f; break; }
        for (String f : new String[]{"JetBrains Mono","Fira Code","Ubuntu Mono","DejaVu Sans Mono","Consolas","Monospaced"})
            if (fam.contains(f)) { m = f; break; }
        SANS_FONT = s;
        MONO_FONT = m;
    }

    // ── Scaled spacing constants ──────────────────────────────────────────────
    public static final int GAP_XS = scale(4);
    public static final int GAP_SM = scale(8);
    public static final int GAP_MD = scale(16);
    public static final int GAP_LG = scale(24);
    public static final int GAP_XL = scale(40);

    // ── Scaled radius constants ───────────────────────────────────────────────
    public static final int RADIUS    = scale(8);
    public static final int RADIUS_LG = scale(12);
    public static final int RADIUS_XL = scale(20);

    // ── Convenience helpers ───────────────────────────────────────────────────
    /** Standard field internal margin: 8 top/bottom, 12 left/right — all scaled. */
    public static java.awt.Insets fieldInsets() {
        return new java.awt.Insets(scale(8), scale(12), scale(8), scale(12));
    }
    /** Empty border with every value individually scaled. */
    public static javax.swing.border.Border emptyBorder(int t, int l, int b, int r) {
        return BorderFactory.createEmptyBorder(scale(t), scale(l), scale(b), scale(r));
    }
    /** Dark-themed text field: background, foreground, caret. */
    public static void styleDarkField(javax.swing.text.JTextComponent f) {
        f.setBackground(BG);
        f.setForeground(TEXT_DARK);
        f.setCaretColor(PRIMARY);
        f.setOpaque(true);
    }

    // ── Typography ───────────────────────────────────────────────────────────
    public static Font title(int size)    { return new Font(SANS_FONT, Font.BOLD,  Math.round(size * FONT_SCALE)); }
    public static Font body (int size)    { return new Font(SANS_FONT, Font.PLAIN, Math.round(size * FONT_SCALE)); }
    public static Font mono (int size)    { return new Font(MONO_FONT, Font.PLAIN, Math.round(size * FONT_SCALE)); }
    public static Font monoBold(int size) { return new Font(MONO_FONT, Font.BOLD,  Math.round(size * FONT_SCALE)); }

    // ── Button factories ─────────────────────────────────────────────────────
    public static RoundedButton navBtn(String text, int w) {
        return makeBtn(text, w, PRIMARY, java.awt.Color.WHITE);
    }
    public static RoundedButton successBtn(String text, int w) {
        return makeBtn(text, w, SUCCESS, java.awt.Color.WHITE);
    }
    public static RoundedButton warningBtn(String text, int w) {
        return makeBtn(text, w, WARNING, new java.awt.Color(26, 17, 0));
    }
    public static RoundedButton dangerBtn(String text, int w) {
        return makeBtn(text, w, DANGER, java.awt.Color.WHITE);
    }
    private static RoundedButton makeBtn(String text, int w, Color bg, Color fg) {
        RoundedButton b = new RoundedButton(text, 8, new Dimension(scale(w), scale(38)));
        b.setBackground(bg); b.setForeground(fg); b.setFont(title(14));
        return b;
    }

    // ── Border helpers ───────────────────────────────────────────────────────
    public static javax.swing.border.Border section(String title) {
        return BorderFactory.createTitledBorder(
            BorderFactory.createLineBorder(BORDER, 1), title, 0, 0, body(12), TEXT_MED);
    }
    public static javax.swing.border.Border card() {
        return BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(BORDER, 1),
            BorderFactory.createEmptyBorder(10, 14, 10, 14));
    }

    private Theme() {}
}
