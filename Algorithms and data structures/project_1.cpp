/**
 * This algorithm is a solution to the Domino problem.
 *
 * The program reads array values from input source and calculates the maximum
 * possible sum of a domino tiling over the given array, in which a value
 * is included only if it's covered by a domino in this tiling.
 *
 * Time complexity - O(n * k * 2^k)
 *
 * Author: Kacper Pasinski
 * Date: 11.11.2024
*/

#include <iostream>
#include <vector>
using namespace std;

/* Functions used in the algorithm */

/**
 * Recursively calculates row-by-row all the possible ways to place new domino blocks in the given column
 * (represented by its mask, to know which tiles are covered already). One tiling is represented
 * by the next column's mask (in case some of the blocks were placed horizontally - then they affect
 * the next column that's going to be processed) and the sum of values of all the cells covered
 * by the newly placed dominoes. After a possible tiling is calculated, it is added to the vector
 * called tilings.
 *
 * @param row The current row in the board to process.
 * @param col The current column in the board.
 * @param mask The current mask indicating the placement of dominoes in the column that's being processed.
 * @param nextMask The mask being constructed for the next column.
 * @param tempSum The cumulative sum of values in the board cells covered by newly placed dominoes.
 * @param tilings A vector to store pairs of next masks and their corresponding cumulative sums.
 * @param board A 2D vector representing the board with values in each cell.
 * @param n The total number of columns in the board.
 * @param k The total number of rows in the board.
 */
void calculateTilings(int row, int col, int mask, int nextMask, long long tempSum,
          vector<pair<int, long long>>& tilings, vector<vector<int>>& board, int n, int k) {
    // If all the rows have been processed - the tiling calculation has completed.
    if (row == k) {
        tilings.emplace_back(nextMask, tempSum);
        return;
    }

    // x == 0 if board[row][col] has not been covered by a domino and 1 otherwise.
    // y == 0 if board[row + 1][col] has not been covered by a domino and 1 otherwise.
    int x = (mask >> row) & 1;
    int y = (mask >> (row + 1)) & 1;

    // If board[row][col] would be covered already, no new domino can be placed.
    if (x == 0) {
        // A domino can be placed horizontally or vertically. If it's placed horizontally,
        // it affects the next column. If it's placed vertically, it affects the next row.
        // That's why if the function is processing the last row and/or column, the dominoes
        // can't be placed - the sum is set to zero. Otherwise, it's just the sum of the values
        // from the board under the covered cells.
        int horizontalBlockSum = (col + 1 < n) ? board[row][col] + board[row][col + 1] : 0;
        int verticalBlockSum = (row + 1 < k) ? board[row][col] + board[row + 1][col] : 0;

        // The blocks should be placed only if they add a positive number to the sum
        // Otherwise, it's just better to not place the domino at all, since the goal
        // is to find the max possible sum. It also handles the last row/column edge cases.
        if (horizontalBlockSum > 0)
            calculateTilings(row + 1, col, mask, nextMask | (1 << row),
                tempSum + horizontalBlockSum, tilings, board, n, k);

        // Same as above, but board[row + 1][col] can't be covered if adding a vertical block.
        if (y == 0 && verticalBlockSum > 0)
            calculateTilings(row + 2, col, mask, nextMask,
                tempSum + verticalBlockSum, tilings, board, n, k);
    }

    // It's always possible to just not place a domino in the row being processed.
    calculateTilings(row + 1, col, mask, nextMask, tempSum, tilings, board, n, k);
}

/**
 * Recursively computes the maximum sum achievable by placing dominos from the current column to the
 * last column in the board. Uses dynamic programming to store and retrieve results for each state.
 *
 * @param col The current column in the board being processed.
 * @param mask The mask indicating the placement of dominos in the current column.
 * @param board A 2D vector representing the board with values in each cell.
 * @param dp A 2D vector used to store the maximum sum results for each column and mask combination.
 * @param n The total number of columns in the board.
 * @param k The total number of rows in the board.
 * @return The maximum sum achievable from the current column to the end of the board.
 */
long long maxDominoSuma(int col, int mask, vector<vector<int>>& board, vector<vector<long long>>& dp,
                int n, int k) {
    // No further calculations can be made, since the function has made it past the last column.
    if (col == n)
        return 0;

    // If the function has already calculated the result for this column and mask, there's no need
    // to do it again.
    if (dp[col][mask] != -1)
        return dp[col][mask];

    // Finding all the possible ways one can place dominoes over the column being processed.
    vector<pair<int, long long>> tilings;
    calculateTilings(0, col, mask, 0, 0, tilings, board, n, k);

    // Calculating all the possible scenarios and calculating which one of them yields the max sum.
    long long maxSum = 0;
    for (auto tiling: tilings) {
        maxSum = max(maxSum,
            tiling.second + maxDominoSuma(col + 1, tiling.first, board, dp, n, k));
    }

    return dp[col][mask] = maxSum;
}

int main() {
    int n, k;
    cin >> n >> k;

    vector<vector<int>> board;
    board.resize(k, vector<int> (n));

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            cin >> board[i][j];
        }
    }

    vector<vector<long long>> dp;
    dp.resize(n, vector<long long> (1 << k, -1));

    cout << maxDominoSuma(0, 0, board, dp, n, k);
    return 0;
}