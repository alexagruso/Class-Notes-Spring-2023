import java.io.*;

class Grid {
    private int size;
    private boolean[][] grid;

    public Grid(int size) {
        this.size = size;
        this.grid = new boolean[size][size];

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                this.grid[i][j] = false;
            }
        }
    }

    public void print() {
        for (int j = 0; j < size; j++) {
            for (int i = 0; i < size; i++) {
                System.out.print(this.grid[i][j] ? 'x' : '.');
            }

            System.out.println();
        }
    }

    public void activateCell(int x, int y) {
        assert(x >= 0 && x < size);
        assert(y >= 0 && y < size);

        this.grid[x][y] = true;
    }

    public void deactivateCell(int x, int y) {
        assert(x >= 0 && x < size);
        assert(y >= 0 && y < size);

        this.grid[x][y] = false;
    }

    public boolean aliveAt(int x, int y) {
        assert(x >= 0 && x < size);
        assert(y >= 0 && y < size);

        if (validAt(x, y) && grid[x][y]) {
            return true;
        }

        return false;
    }

    public int neighborsAt(int x, int y) {
        assert(x >= 0 && x < size);
        assert(y >= 0 && y < size);

        int neighbors = 0;
    
        for (int i = x - 1; i <= x + 1; i++) {
            for (int j = y - 1; j <= y + 1; j++) {
                if (validAt(i, j) && (i != x || j != y) && this.grid[i][j]) {
                    neighbors++;
                }
            }
        }

        return neighbors;
    }

    public int getSize() {
        return this.size;
    }

    private boolean validAt(int x, int y) {
        if (x < 0 || y < 0) return false;

        if (x >= size || y >= size) return false;

        return true;
    }
}

public class GOL {
    private Grid grid;
    private int iterations;

    public GOL(int size, String filePath) throws IOException {
        this.grid = new Grid(size);

        FileInputStream input = null;

        try {
            input = new FileInputStream(filePath);

            int fileByte;

            int xPos = 0;
            int yPos = 0;

            while ((fileByte = input.read()) != -1) {
                char fileChar = (char) fileByte;
                switch(fileChar) {
                    case '.':
                        xPos++;
                        break;
                    case 'x':
                        this.grid.activateCell(xPos, yPos);
                        xPos++;
                        break;
                    case '\n':
                        xPos = 0;
                        yPos++;
                        break;
                    default:
                        // this.iterations = Integer.parseInt(); TODO broken
                        return;
                }
            }
        } catch (IOException exception) {
            System.err.println("File not found\n" + exception);
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    public void run() {
        grid.print();

        for (int i = 0; i < 10; i++) {
            this.iterate();
        }

        grid.print();
    }
    
    private void iterate() {
        Grid newGrid = new Grid(this.grid.getSize());

        for (int j = 0; j < this.grid.getSize(); j++) {
            for (int i = 0; i < this.grid.getSize(); i++) {
                if (this.grid.aliveAt(i, j)) {
                    System.out.print("x");
                } else {
                    System.out.print(".");
                }
                if (this.grid.aliveAt(i, j)) {
                    if (this.grid.neighborsAt(i, j) < 2 || this.grid.neighborsAt(i, j) > 3) {
                        this.grid.deactivateCell(i, j);
                    }
                } else if (this.grid.neighborsAt(i, j) == 3) {
                    this.grid.activateCell(i, j);
                }
            }

            System.out.println();
        }
    }

    public static void main(String[] args) throws IOException{
        GOL gol = null;

        try {
            gol = new GOL(20, "start.txt");
        } catch (IOException exception) {
            System.err.println("Failed to initialize GOL\n" + exception);
        }

        if (gol != null) {
            gol.run();
        }
    }
}