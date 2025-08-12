import java.awt.*;
import javax.swing.*;

public class RoundedButton extends JButton {

    private int radius;
    private Dimension d;

    public RoundedButton(String text, int radius, Dimension d) {
        super(text);
        this.radius = radius;
        this.d= d;
        setOpaque(false); // We’ll handle painting ourselves
        setFocusPainted(false); // Optional: removes focus ring
        setContentAreaFilled(false); // We paint the button
        setBorderPainted(false); // Optional: no default border
    }

    @Override
    protected void paintComponent(Graphics g) {
        Graphics2D g2 = (Graphics2D) g.create();

        // Antialiasing for smooth corners
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        // Button background
        if (getModel().isArmed()) {
            g2.setColor(Color.LIGHT_GRAY);
        } else {
            g2.setColor(getBackground());
        }

        // Draw rounded rectangle
        g2.fillRoundRect(0, 0, getWidth(), getHeight(), radius, radius);

        // Draw button text
        FontMetrics fm = g2.getFontMetrics();
        Rectangle stringBounds = fm.getStringBounds(getText(), g2).getBounds();
        int textX = (getWidth() - stringBounds.width) / 2;
        int textY = (getHeight() - stringBounds.height) / 2 + fm.getAscent();
        g2.setColor(getForeground());
        g2.drawString(getText(), textX, textY);

        g2.dispose();
    }

    @Override
    protected void paintBorder(Graphics g) {
        // Optional: draw a rounded border
        Graphics2D g2 = (Graphics2D) g.create();
        g2.setColor(getForeground());
        g2.setStroke(new BasicStroke(1.5f));
        g2.drawRoundRect(0, 0, getWidth() - 1, getHeight() - 1, radius, radius);
        g2.dispose();
    }

    @Override
    public Dimension getPreferredSize() {
        return d;
    }
}
