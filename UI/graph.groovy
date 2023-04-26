import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

public class GraphSample {
    private JFrame frame;

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new GraphSample()::createAndShowGui);
    }

    private void createAndShowGui() {
        frame = new JFrame(getClass().getSimpleName());

        GraphDrawer drawer = new GraphDrawer(new int[] {0, 3, 4, 7, 5, 10, 3});

        frame.add(drawer);
        frame.pack();
        frame.setVisible(true);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    @SuppressWarnings("serial")
    class GraphDrawer extends JPanel {
        private int[] yCoords;
        private int startX = 100;
        private int startY = 100;
        private int endX = 400;
        private int endY = 400;
        private int unitX = (endX - startX) / 10;
        private int unitY = (endY - startY) / 10;
        private int prevX = startX;
        private int prevY = endY;

        public GraphDrawer(int[] yCoords) {
            this.yCoords = yCoords;
        }

        @Override
        protected void paintComponent(Graphics g) {
            super.paintComponent(g);
            Graphics2D g2d = (Graphics2D) g;

            //We draw in the following 2 loops the grid so it's visible what I explained before about each "unit"
            g2d.setColor(Color.BLUE);
            for (int i = startX; i <= endX; i += unitX) {
                g2d.drawLine(i, startY, i, endY);
            }

            for (int i = startY; i <= endY; i += unitY) {
                g2d.drawLine(startX, i, endX, i);
            }

            //We draw the axis here instead of before because otherwise they would become blue colored.
            g2d.setColor(Color.BLACK);
            g2d.drawLine(startX, startY, startX, endY);
            g2d.drawLine(startX, endY, endX, endY);

            //We draw each of our coords in red color
            g2d.setColor(Color.RED);
            for (int y : yCoords) {
                g2d.drawLine(prevX, prevY, prevX += unitX, prevY = endY - (y * unitY));
            }
        }

        @Override
        public Dimension getPreferredSize() {
            return new Dimension(endX + 100, endY + 100);
        }
    }
}