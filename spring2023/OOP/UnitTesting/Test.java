public class Test {
    public static void main(String[] args) {
        // testEmptyGrid();
        // testFullGrid();
        // testZeroNeighbours();
        // testMaxNeighbours();
    }

    public static void testMaxNeighbours() {
        boolean[][] grid = {
            { true, true, true, true},
            { true, false, true, true},
            { true, true, true, true},
            { true, true, true, true},
        };

        NC1 testGrid = new NC1(grid);

        int testNeighbors = testGrid.Count(1, 1);

        if (testNeighbors != 8) {
            System.out.println("NC1.count(1, 1) is " + testNeighbors + " instead of 8");
        }
    }

    public static void testZeroNeighbours() {
        boolean[][] grid = {
            { false, false, false, false},
            { false, true, false, false},
            { false, false, false, false},
            { false, false, false, false},
        };

        NC1 testGrid = new NC1(grid);

        int testNeighbors = testGrid.Count(1, 1);

        if (testNeighbors != 0) {
            System.out.println("NC1.count(1, 1) is " + testNeighbors + " instead of 0");
        }
    }

    public static void testEmptyGrid() {
        boolean[][] grid = {
            { false, false, false, false},
            { false, false, false, false},
            { false, false, false, false},
            { false, false, false, false},
        };

        NC1 testGrid = new NC1(grid);

        int testNeighbors = 0;

        for (int i = 0; i < grid.length; i++) {
            for (int j = 0; j < grid.length; j++) {
                testNeighbors = testGrid.Count(i,j);

                if (testNeighbors != 0) {
                    System.out.println("testGrid.Count(" + i + ", " + j + ") is " + testNeighbors + " instead of 0");
                }
            }
        }
    }
    
    public static void testFullGrid() {
        boolean[][] grid = {
            { true, true, true, true},
            { true, true, true, true},
            { true, true, true, true},
            { true, true, true, true},
        };

        NC1 testGrid = new NC1(grid);

        int test1Neighbors = 0;

        for (int i = 0; i < grid.length; i++) {
            for (int j = 0; j < grid.length; j++) {
                test1Neighbors = testGrid.Count(i,j);
                System.out.println("NC1.Count(" + i + ", " + j + ") has " + test1Neighbors + " neighbors");
            }
        }
    }
}

interface INeighborCounter {
    int Count(int row, int col);
}

class NC1 implements INeighborCounter {
    boolean[][] cells;
    int rows = 0;
    int cols = 0;

    public NC1(boolean[][] cells) {
        this.cells = cells;
        rows = cells.length;
        cols = cells[0].length;
    }

    public int Count(int row, int col) {
        int n = 0;
        n += live(row-1, col-1);
        n += live(row-1, col);
        n += live(row-1, col+1);
        n += live(row, col+1);
        n += live(row+1, col+1);
        n += live(row+1, col);
        n += live(row+1, col-1);
        n += live(row, col-1);
        return n;
    }
    
    public int live(int r, int c) {
        if (r < 0 || r >= rows) return 0;
        if (c < 0 || c >= cols) return 0;
        return (!cells[r][c]) ? 1 : 0;
    }
}