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

        Color fill;
        if (!isEnabled()) {
            fill = new Color(base.getRed(), base.getGreen(), base.getBlue(), 70);
        } else if (getModel().isPressed()) {
            fill = base.darker();
        } else if (hovered) {
            fill = base.brighter();
        } else {
            fill = base;
        }

        g2.setColor(fill);
        g2.fillRoundRect(0, 0, getWidth(), getHeight(), radius, radius);

        // Subtle top-highlight shimmer (skip when disabled or pressed)
        if (isEnabled() && !getModel().isPressed()) {
            g2.setColor(new Color(255, 255, 255, 25));
            g2.fillRoundRect(1, 1, getWidth() - 2, getHeight() / 2, radius, radius);
        }

        // Keyboard focus ring
        if (isFocusOwner() && isEnabled()) {
            g2.setColor(new Color(255, 255, 255, 180));
            g2.setStroke(new BasicStroke(2f));
            g2.drawRoundRect(2, 2, getWidth() - 5, getHeight() - 5, radius, radius);
        }

        // Label
        g2.setFont(getFont() != null ? getFont() : Theme.title(14));
        FontMetrics fm = g2.getFontMetrics();
        int tx = (getWidth()  - fm.stringWidth(getText())) / 2;
        int ty = (getHeight() - fm.getHeight()) / 2 + fm.getAscent();
        Color fg = getForeground() != null ? getForeground() : Color.WHITE;
        g2.setColor(!isEnabled() ? new Color(fg.getRed(), fg.getGreen(), fg.getBlue(), 120) : fg);
        g2.drawString(getText(), tx, ty);
        g2.dispose();
    }

    @Override protected void paintBorder(Graphics g) { /* no border */ }

    @Override
    public Dimension getPreferredSize() {
        // When width is Integer.MAX_VALUE the button is meant to fill its container.
        // Returning MAX_VALUE as preferred width breaks GridBagLayout centering (integer
        // overflow), so fall back to the natural text-based width for preferred size only.
        if (d.width >= Integer.MAX_VALUE / 2) {
            Dimension nat = super.getPreferredSize();
            return new Dimension(nat.width, d.height);
        }
        return d;
    }

    @Override
    public Dimension getMaximumSize() {
        // Max size controls how far BoxLayout stretches the button — keep the original d.
        return d;
    }
}
