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
    public static final Color CANVAS_BG   = new Color(10,  22,  40);   // same as BG
    public static final Color CANVAS_DOT  = new Color(22,  41,  72);   // subtle grid dots

    // ── Text ─────────────────────────────────────────────────────────────────
    public static final Color TEXT_DARK   = new Color(214, 232, 255);  // #D6E8FF hi text
    public static final Color TEXT_MED    = new Color(106, 150, 200);  // #6A96C8 mid text
    public static final Color TEXT_LIGHT  = new Color(49,  79,  120);  // #314F78 dim text

    // ── Type accent colours ───────────────────────────────────────────────────
    public static final Color T_FILE      = new Color(96,  165, 250);  // #60A5FA blue
    public static final Color T_INT       = new Color(167, 139, 250);  // #A78BFA purple
    public static final Color T_FLOAT     = new Color(244, 114, 182);  // #F472B6 pink
    public static final Color T_STRING    = new Color(52,  211, 153);  // #34D399 emerald
    public static final Color T_GRAPH     = new Color(34,  211, 238);  // #22D3EE cyan
    public static final Color T_OTHER     = new Color(148, 163, 184);  // #94A3B8 slate

    // ── Typography ───────────────────────────────────────────────────────────
    public static Font title(int size)    { return new Font("Segoe UI",  Font.BOLD,  size); }
    public static Font body (int size)    { return new Font("Segoe UI",  Font.PLAIN, size); }
    public static Font mono (int size)    { return new Font("Consolas",  Font.PLAIN, size); }
    public static Font monoBold(int size) { return new Font("Consolas",  Font.BOLD,  size); }

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
