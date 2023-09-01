import java.io.*;

class CellThread extends Thread {
    private Grid grid;
    private int x;
    private int y;

    private boolean result = false;

    public CellThread(Grid grid, int x, int y) {
        this.grid = grid;
        this.x = x;
        this.y = y;
    }

    public boolean getResult() throws InterruptedException {
        try {
            this.join();
        } catch (InterruptedException error) {}

        return this.result;
    }

    @Override
    public void run() {
        if (this.grid.aliveAt(y, x)) {
            if (this.grid.neighborsAt(y, x) < 2 || this.grid.neighborsAt(y, x) > 3) {
                this.result = false;
            } else {
                this.result = true;
            }
        } else if (this.grid.neighborsAt(y, x) == 3) {
            this.result = true;
        } else {
            this.result = false;
        }
    }
}

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

public class GameOfLifeThreads {
    private Grid grid;
    private int generations;

    public GameOfLifeThreads(int size, String filePath) throws IOException {
        this.grid = new Grid(size);

        FileInputStream input = null;

        try {
            input = new FileInputStream(filePath);

            for (int j = 0; j < this.grid.getSize(); j++) {
                for (int i = 0; i < this.grid.getSize() + 1; i++) {
                    char fileChar = (char) input.read();

                    if (fileChar == 'x') {
                        this.grid.activateCell(i, j);
                    } else if (fileChar != '.') {
                        continue;
                    }
                }
            }

            generations = Integer.parseInt(String.valueOf((char) input.read()));
        } catch (IOException exception) {
            System.err.println("File not found\n" + exception);
        } finally {
            if (input != null) {
                input.close();
            }
        }
    }

    public void run() {
        for (int i = 0; i < this.generations; i++) {
            this.iterate();
        }

        this.grid.print();
    }
    
    private void iterate() {
        Grid newGrid = new Grid(this.grid.getSize());

            CellThread[][] threads = new CellThread[this.grid.getSize()][this.grid.getSize()];

        for (int i = 0; i < this.grid.getSize(); i++) {
            for (int j = 0; j < this.grid.getSize(); j++) {
                threads[i][j] = new CellThread(this.grid, i, j);
                threads[i][j].start();

                if (this.grid.aliveAt(i, j)) {
                    newGrid.activateCell(i, j);
                }
            }
        }

        for (int i = 0; i < this.grid.getSize(); i++) {
            for (int j = 0; j < this.grid.getSize(); j++) {
                try {
                    if (threads[j][i].getResult()) {
                        newGrid.activateCell(i, j);
                    } else {
                        newGrid.deactivateCell(i, j);
                    }
                } catch (InterruptedException error) {
                    System.out.println("Error:\n" + error);
                }
            }
        }

        this.grid = newGrid;
    }

    public static void main(String[] args) throws IOException{
        GameOfLifeThreads gameOfLife = null;

        try {
            gameOfLife = new GameOfLifeThreads(20, "start.txt");
        } catch (IOException exception) {
            System.err.println("Failed to initialize GOL\n" + exception);
        }

        if (gameOfLife != null) {
            gameOfLife.run();
        }
    }
}