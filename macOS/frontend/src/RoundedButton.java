import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class RoundedButton extends JButton {

    private final int radius;
    private final Dimension d;
    private boolean hovered = false;

    public RoundedButton(String text, int radius, Dimension d) {
        super(text);
        this.radius = radius;
        this.d = d;
        setOpaque(false);
        setFocusPainted(false);
        setContentAreaFilled(false);
        setBorderPainted(false);
        setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
        addMouseListener(new MouseAdapter() {
            public void mouseEntered(MouseEvent e) { hovered = true;  repaint(); }
            public void mouseExited (MouseEvent e) { hovered = false; repaint(); }
        });
    }

    @Override
    protected void paintComponent(Graphics g) {
        Graphics2D g2 = (Graphics2D) g.create();
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,      RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
        g2.setRenderingHint(RenderingHints.KEY_RENDERING,         RenderingHints.VALUE_RENDER_QUALITY);

        Color base = getBackground() != null ? getBackground() : Theme.PRIMARY;
        Color fill = getModel().isPressed() ? base.darker()
                   : hovered               ? base.brighter()
                   :                         base;

        g2.setColor(fill);
        g2.fillRoundRect(0, 0, getWidth(), getHeight(), radius, radius);

        // Subtle top-highlight shimmer
        if (!getModel().isPressed()) {
            g2.setColor(new Color(255, 255, 255, 25));
            g2.fillRoundRect(1, 1, getWidth() - 2, getHeight() / 2, radius, radius);
        }

        g2.setFont(getFont() != null ? getFont() : Theme.title(14));
        FontMetrics fm = g2.getFontMetrics();
        int tx = (getWidth()  - fm.stringWidth(getText())) / 2;
        int ty = (getHeight() - fm.getHeight()) / 2 + fm.getAscent();
        g2.setColor(getForeground() != null ? getForeground() : Color.WHITE);
        g2.drawString(getText(), tx, ty);
        g2.dispose();
    }

    @Override protected void paintBorder(Graphics g) { /* no border */ }

    @Override public Dimension getPreferredSize() { return d; }
}
