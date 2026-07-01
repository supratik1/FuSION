import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class HeaderPanel extends JPanel {

    private final String username;

    /** Used by SessionFrame — no navigation menu. */
    public HeaderPanel(String username) {
        this.username = username;
        setOpaque(false);
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createEmptyBorder(8, 16, 8, 12));
        setPreferredSize(new Dimension(0, 52));
        build(null, null, null);
    }

    /** Used by all post-login panels — includes burger navigation menu. */
    public HeaderPanel(String username, CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        this.username = username;
        setOpaque(false);
        setLayout(new BorderLayout());
        setBorder(BorderFactory.createEmptyBorder(8, 16, 8, 12));
        setPreferredSize(new Dimension(0, 52));
        build(cardLayout, cardPanel, user);
    }

    private void build(CardLayout cl, JPanel cp, UserInput user) {

        // ── Left: burger (optional) + brand ──────────────────────────────
        JPanel left = new JPanel(new FlowLayout(FlowLayout.LEFT, 8, 0));
        left.setOpaque(false);

        if (cl != null && cp != null && user != null) {
            RoundedButton burgerBtn = new RoundedButton("☰", 6, new Dimension(34, 30));
            burgerBtn.setBackground(new Color(255, 255, 255, 40));
            burgerBtn.setForeground(Color.WHITE);
            burgerBtn.setFont(new Font("SansSerif", Font.BOLD, 16));
            burgerBtn.setToolTipText("Navigate to any step");
            burgerBtn.addActionListener(e -> showNavMenu(burgerBtn, cl, cp, user));
            left.add(burgerBtn);
        }

        JLabel brand = new JLabel("FuSION");
        brand.setFont(new Font("Segoe UI", Font.BOLD, 22));
        brand.setForeground(Color.WHITE);
        left.add(brand);

        // ── Right: username chip + window controls ────────────────────────
        JLabel userChip = new JLabel("  👤  " + username + "  ");
        userChip.setFont(new Font("Segoe UI", Font.PLAIN, 12));
        userChip.setForeground(new Color(210, 220, 255));

        RoundedButton minBtn = new RoundedButton("—", 6, new Dimension(30, 24));
        minBtn.setBackground(new Color(255, 255, 255, 35));
        minBtn.setForeground(Color.WHITE);
        minBtn.setFont(new Font("Segoe UI", Font.BOLD, 13));
        minBtn.setToolTipText("Minimize");

        RoundedButton maxBtn = new RoundedButton("□", 6, new Dimension(30, 24));
        maxBtn.setBackground(new Color(255, 255, 255, 35));
        maxBtn.setForeground(Color.WHITE);
        maxBtn.setFont(new Font("Segoe UI", Font.BOLD, 13));
        maxBtn.setToolTipText("Maximize");

        RoundedButton closeBtn = new RoundedButton("✕", 6, new Dimension(30, 24));
        closeBtn.setBackground(Theme.DANGER);
        closeBtn.setForeground(Color.WHITE);
        closeBtn.setFont(new Font("Segoe UI", Font.BOLD, 12));
        closeBtn.setToolTipText("Close");

        minBtn.addActionListener(e -> {
            JFrame f = (JFrame) SwingUtilities.getWindowAncestor(this);
            if (f != null) f.setState(JFrame.ICONIFIED);
        });
        maxBtn.addActionListener(e -> toggleMaximize(maxBtn));
        closeBtn.addActionListener(e -> System.exit(0));

        // Drag-to-move
        final int[] dragStart = {0, 0};
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e)  { dragStart[0] = e.getX(); dragStart[1] = e.getY(); }
            public void mouseClicked(MouseEvent e)  { if (e.getClickCount() == 2) toggleMaximize(maxBtn); }
        });
        addMouseMotionListener(new MouseMotionAdapter() {
            public void mouseDragged(MouseEvent e) {
                JFrame f = (JFrame) SwingUtilities.getWindowAncestor(HeaderPanel.this);
                if (f != null && (f.getExtendedState() & JFrame.MAXIMIZED_BOTH) == 0) {
                    Point loc = f.getLocation();
                    f.setLocation(loc.x + e.getX() - dragStart[0],
                                  loc.y + e.getY() - dragStart[1]);
                }
            }
        });

        JPanel right = new JPanel(new FlowLayout(FlowLayout.RIGHT, 6, 0));
        right.setOpaque(false);
        right.add(userChip);
        right.add(minBtn);
        right.add(maxBtn);
        right.add(closeBtn);

        add(left,  BorderLayout.WEST);
        add(right, BorderLayout.EAST);
    }

    // ── Navigation menu ───────────────────────────────────────────────────

    private static final String[] NAV_LABELS = {
        "📁  Working Directory",
        "📄  Log Fold Changes",
        "🔗  KEGG / XML Files",
        "🔢  Reach Path Bound",
        "⚖️  Relaxation Bounds",
        "🔍  Threshold Filter",
        "➡️  Edge Files",
        "🔬  Node IDs",
        "⚙️  Pipeline Editor"
    };

    private void showNavMenu(Component anchor, CardLayout cl, JPanel cp, UserInput user) {
        JPopupMenu menu = new JPopupMenu();
        menu.setBackground(Color.WHITE);
        menu.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(4, 0, 4, 0)));

        for (int i = 0; i < NAV_LABELS.length; i++) {
            final int idx = i;
            JMenuItem item = new JMenuItem(NAV_LABELS[i]);
            item.setFont(new Font("Segoe UI", Font.PLAIN, 13));
            item.setForeground(Theme.TEXT_DARK);
            item.setBackground(Color.WHITE);
            item.setOpaque(true);
            item.setBorder(BorderFactory.createEmptyBorder(6, 16, 6, 24));
            item.addMouseListener(new MouseAdapter() {
                public void mouseEntered(MouseEvent e) { item.setBackground(new Color(240, 244, 255)); }
                public void mouseExited(MouseEvent e)  { item.setBackground(Color.WHITE); }
            });
            item.addActionListener(e -> navigateTo(idx, cl, cp, user));
            menu.add(item);
        }

        menu.show(anchor, 0, anchor.getHeight());
    }

    private void navigateTo(int idx, CardLayout cl, JPanel cp, UserInput user) {
        JPanel panel;
        switch (idx) {
            case 0: panel = new MainPanel(user.getUsername(), user, cl, cp); break;
            case 1: panel = new FileSelectionCard(cl, cp, user); break;
            case 2: panel = new XMLSelection(cl, cp, user); break;
            case 3: panel = new ReachPathBoundCard(cl, cp, user); break;
            case 4: panel = new RelaxationBoundsPanel(cl, cp, user); break;
            case 5: panel = new ThresholdFilterPanel(cl, cp, user); break;
            case 6: panel = new EdgesChooserPanel(cl, cp, user); break;
            case 7: panel = new IdEntryPanel(cl, cp, user); break;
            case 8: panel = new EdgesChooserPanel(cl, cp, user); break;
            default: return;
        }
        navigate(cp, cl, panel, "nav_" + idx);
    }

    /** Removes any panel previously navigated to with this name, then shows the new one. */
    public static void navigate(JPanel cardPanel, CardLayout cardLayout, JPanel newPanel, String name) {
        for (Component c : cardPanel.getComponents()) {
            if (name.equals(c.getName())) { cardPanel.remove(c); break; }
        }
        newPanel.setName(name);
        cardPanel.add(newPanel, name);
        cardLayout.show(cardPanel, name);
        cardPanel.revalidate();
        cardPanel.repaint();
    }

    // ── Window controls ───────────────────────────────────────────────────

    private void toggleMaximize(RoundedButton maxBtn) {
        JFrame f = (JFrame) SwingUtilities.getWindowAncestor(this);
        if (f == null) return;
        if ((f.getExtendedState() & JFrame.MAXIMIZED_BOTH) == JFrame.MAXIMIZED_BOTH) {
            f.setExtendedState(JFrame.NORMAL);
            maxBtn.setText("□");
            maxBtn.setToolTipText("Maximize");
        } else {
            f.setExtendedState(JFrame.MAXIMIZED_BOTH);
            maxBtn.setText("❯❯");
            maxBtn.setToolTipText("Restore");
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        Graphics2D g2 = (Graphics2D) g.create();
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        GradientPaint gp = new GradientPaint(0, 0, Theme.PRIMARY, getWidth(), 0, Theme.PRIMARY_DK);
        g2.setPaint(gp);
        g2.fillRect(0, 0, getWidth(), getHeight());
        g2.setColor(new Color(0, 0, 0, 40));
        g2.fillRect(0, getHeight() - 1, getWidth(), 1);
        g2.dispose();
        super.paintComponent(g);
    }
}
