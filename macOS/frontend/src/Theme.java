import java.awt.*;
import javax.swing.*;

/** Centralised design tokens for the FuSION UI. */
public final class Theme {

    // ── Palette ──────────────────────────────────────────────────────────────
    public static final Color BG          = new Color(246, 248, 252);   // page surface
    public static final Color BG_CARD     = Color.WHITE;                // card surface
    public static final Color PRIMARY     = new Color(67,  97,  238);   // indigo-blue
    public static final Color PRIMARY_DK  = new Color(50,  76,  200);   // gradient end
    public static final Color PRIMARY_HVR = new Color(88, 116, 245);    // hover tint
    public static final Color SUCCESS     = new Color(16,  185, 129);   // emerald
    public static final Color SUCCESS_DK  = new Color(10,  155, 105);
    public static final Color WARNING     = new Color(245, 158,  11);   // amber
    public static final Color WARNING_DK  = new Color(210, 130,   5);
    public static final Color DANGER      = new Color(220,  53,  69);   // crimson
    public static final Color TEXT_DARK   = new Color( 17,  24,  39);   // near-black
    public static final Color TEXT_MED    = new Color( 75,  85,  99);   // mid-gray
    public static final Color TEXT_LIGHT  = new Color(156, 163, 175);   // hint
    public static final Color BORDER      = new Color(226, 232, 240);   // card outline
    public static final Color CANVAS_BG   = new Color(248, 250, 253);
    public static final Color CANVAS_DOT  = new Color(206, 215, 230);

    // ── Typography ───────────────────────────────────────────────────────────
    public static Font title(int size) { return new Font("Segoe UI", Font.BOLD,  size); }
    public static Font body (int size) { return new Font("Segoe UI", Font.PLAIN, size); }
    public static Font mono (int size) { return new Font("JetBrains Mono", Font.PLAIN, size); }

    // ── Button factories ─────────────────────────────────────────────────────
    public static RoundedButton navBtn(String text, int w) {
        return makeBtn(text, w, PRIMARY, Color.WHITE);
    }
    public static RoundedButton successBtn(String text, int w) {
        return makeBtn(text, w, SUCCESS, Color.WHITE);
    }
    public static RoundedButton warningBtn(String text, int w) {
        return makeBtn(text, w, WARNING, Color.WHITE);
    }
    public static RoundedButton dangerBtn(String text, int w) {
        return makeBtn(text, w, DANGER, Color.WHITE);
    }
    private static RoundedButton makeBtn(String text, int w, Color bg, Color fg) {
        RoundedButton b = new RoundedButton(text, 8, new Dimension(w, 38));
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
