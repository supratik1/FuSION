import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.ChangeEvent;

public class RelaxationBoundsPanel extends RoundedPanel {

    public RelaxationBoundsPanel(CardLayout cardLayout, JPanel cardPanel, UserInput user) {
        setLayout(new BorderLayout(0, 0));
        setBackground(Theme.BG);

        int[] arr = user.getRelaxationBounds(); // [nodeLB, nodeUB, edgeLB, edgeUB]

        HeaderPanel header = new HeaderPanel(user.getUsername(), cardLayout, cardPanel, user);
        add(header, BorderLayout.NORTH);

        JLabel title = new JLabel("Node and Edge Relaxation Bounds", JLabel.CENTER);
        title.setFont(Theme.title(24));
        title.setForeground(Theme.TEXT_DARK);
        title.setBorder(BorderFactory.createEmptyBorder(Theme.scale(20), 0, Theme.scale(16), 0));

        RangeSlider nodeSlider = makeSlider(arr[0], arr[1]);
        RangeSlider edgeSlider = makeSlider(arr[2], arr[3]);

        JPanel nodeCard = makeBoundCard("Node Relaxation Bounds", arr, 0, 1, nodeSlider);
        JPanel edgeCard = makeBoundCard("Edge Relaxation Bounds", arr, 2, 3, edgeSlider);

        JPanel cardsPanel = new JPanel(new GridLayout(2, 1, 0, Theme.scale(16)));
        cardsPanel.setOpaque(false);
        cardsPanel.add(nodeCard);
        cardsPanel.add(edgeCard);

        JPanel center = new JPanel(new BorderLayout());
        center.setOpaque(false);
        center.setBorder(BorderFactory.createEmptyBorder(0, Theme.scale(48), Theme.scale(16), Theme.scale(48)));
        center.add(title, BorderLayout.NORTH);
        center.add(cardsPanel, BorderLayout.CENTER);
        add(center, BorderLayout.CENTER);

        RoundedButton nextButton = Theme.navBtn("Next »", 110);
        RoundedButton prevButton = Theme.navBtn("« Prev", 110);
        RoundedButton saveButton = Theme.successBtn("Save", 110);
        RoundedButton sessionBtn = Theme.warningBtn("Sessions", 140);

        nextButton.addActionListener(e -> {
            user.setRelaxationBounds(arr);
            IdEntryPanel idEntry = new IdEntryPanel(cardLayout, cardPanel, user);
            cardPanel.add(idEntry, "idEntry");
            cardLayout.show(cardPanel, "idEntry");
        });
        prevButton.addActionListener(e -> cardLayout.show(cardPanel, "reachBound"));
        saveButton.addActionListener(e -> { user.setRelaxationBounds(arr); user.saveData(); });
        sessionBtn.addActionListener(e -> cardLayout.show(cardPanel, "sessions"));

        RoundedPanel buttonPanel = new RoundedPanel();
        buttonPanel.setLayout(new GridLayout(1, 2));
        buttonPanel.setBackground(Theme.BG);
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(sessionBtn, BorderLayout.WEST);
            add(prevButton, BorderLayout.EAST);
            setOpaque(false);
        }});
        buttonPanel.add(new JPanel(new BorderLayout()) {{
            setBorder(BorderFactory.createEmptyBorder(
                Theme.scale(10), Theme.scale(10), Theme.scale(10), Theme.scale(10)));
            add(nextButton, BorderLayout.WEST);
            add(saveButton, BorderLayout.EAST);
            setOpaque(false);
        }});
        add(buttonPanel, BorderLayout.SOUTH);
    }

    private JPanel makeBoundCard(String title, int[] arr, int lbIdx, int ubIdx, RangeSlider slider) {
        JPanel card = new JPanel();
        card.setLayout(new BoxLayout(card, BoxLayout.Y_AXIS));
        card.setBackground(Theme.BG_CARD);
        card.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(
                Theme.scale(16), Theme.scale(20), Theme.scale(18), Theme.scale(20))));

        JLabel titleLabel = new JLabel(title);
        titleLabel.setFont(Theme.title(14));
        titleLabel.setForeground(Theme.TEXT_DARK);
        titleLabel.setAlignmentX(Component.LEFT_ALIGNMENT);

        JPanel steppers = new JPanel(new GridLayout(1, 2, Theme.scale(24), 0));
        steppers.setOpaque(false);
        steppers.setAlignmentX(Component.LEFT_ALIGNMENT);
        steppers.setMaximumSize(new Dimension(Integer.MAX_VALUE, Theme.scale(90)));
        steppers.add(makeStepper("Lower Bound", arr, lbIdx, slider, true));
        steppers.add(makeStepper("Upper Bound", arr, ubIdx, slider, false));

        JLabel minLbl = new JLabel("0");
        JLabel maxLbl = new JLabel("100");
        minLbl.setFont(Theme.body(11));
        maxLbl.setFont(Theme.body(11));
        minLbl.setForeground(Theme.TEXT_LIGHT);
        maxLbl.setForeground(Theme.TEXT_LIGHT);
        JPanel sliderRow = new JPanel(new BorderLayout(Theme.scale(8), 0));
        sliderRow.setOpaque(false);
        sliderRow.setAlignmentX(Component.LEFT_ALIGNMENT);
        sliderRow.add(minLbl,  BorderLayout.WEST);
        sliderRow.add(slider,  BorderLayout.CENTER);
        sliderRow.add(maxLbl,  BorderLayout.EAST);

        card.add(titleLabel);
        card.add(Box.createVerticalStrut(Theme.scale(12)));
        card.add(steppers);
        card.add(Box.createVerticalStrut(Theme.scale(12)));
        card.add(sliderRow);
        return card;
    }

    private JPanel makeStepper(String label, int[] arr, int idx,
                                RangeSlider slider, boolean isLower) {
        JPanel panel = new JPanel(new BorderLayout(0, Theme.scale(6)));
        panel.setOpaque(false);

        JLabel lbl = new JLabel(label);
        lbl.setFont(Theme.body(12));
        lbl.setForeground(Theme.TEXT_MED);

        JTextField field = new JTextField(String.valueOf(arr[idx]), 4);
        field.setFont(Theme.title(15));
        Theme.styleDarkField(field);
        field.setHorizontalAlignment(JTextField.CENTER);
        field.setBorder(BorderFactory.createCompoundBorder(
            BorderFactory.createLineBorder(Theme.BORDER, 1),
            BorderFactory.createEmptyBorder(
                Theme.scale(4), Theme.scale(8), Theme.scale(4), Theme.scale(8))));

        RoundedButton minus = makeStepBtn("−");
        RoundedButton plus  = makeStepBtn("+");

        Runnable syncFromField = () -> {
            try {
                int v = Integer.parseInt(field.getText().trim());
                v = Math.max(0, Math.min(100, v));
                arr[idx] = v;
                field.setText(String.valueOf(v));
                if (isLower) slider.setValue(v);
                else         slider.setUpperValue(v);
            } catch (NumberFormatException ex) {
                field.setText(String.valueOf(arr[idx]));
            }
        };
        field.addActionListener(e -> syncFromField.run());
        field.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) { syncFromField.run(); }
        });

        minus.addActionListener(e -> {
            arr[idx] = Math.max(0, arr[idx] - 1);
            field.setText(String.valueOf(arr[idx]));
            if (isLower) slider.setValue(arr[idx]);
            else         slider.setUpperValue(arr[idx]);
        });
        plus.addActionListener(e -> {
            arr[idx] = Math.min(100, arr[idx] + 1);
            field.setText(String.valueOf(arr[idx]));
            if (isLower) slider.setValue(arr[idx]);
            else         slider.setUpperValue(arr[idx]);
        });

        slider.addChangeListener((ChangeEvent e) -> {
            int v = isLower ? slider.getValue() : slider.getUpperValue();
            arr[idx] = v;
            field.setText(String.valueOf(v));
        });

        JPanel controls = new JPanel(new FlowLayout(FlowLayout.LEFT, Theme.scale(4), 0));
        controls.setOpaque(false);
        controls.add(minus);
        controls.add(field);
        controls.add(plus);

        panel.add(lbl,      BorderLayout.NORTH);
        panel.add(controls, BorderLayout.CENTER);
        return panel;
    }

    private RoundedButton makeStepBtn(String text) {
        RoundedButton btn = new RoundedButton(text, Theme.scale(8),
            new Dimension(Theme.scale(34), Theme.scale(34)));
        btn.setBackground(Theme.PRIMARY);
        btn.setForeground(Color.WHITE);
        btn.setFont(Theme.title(16));
        return btn;
    }

    private RangeSlider makeSlider(int lb, int ub) {
        RangeSlider s = new RangeSlider();
        s.setMinimum(0);
        s.setMaximum(100);
        s.setValue(lb);
        s.setUpperValue(ub);
        s.setOpaque(false);
        s.setPaintTicks(true);
        s.setPaintLabels(true);
        s.setMajorTickSpacing(20);
        s.setMinorTickSpacing(5);
        s.setForeground(Theme.TEXT_MED);
        return s;
    }
}
