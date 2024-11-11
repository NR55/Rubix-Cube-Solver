#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <omp.h>
#include "rubix_cube.h"

#define MAX_MOVES 13
#define MAX_DEPTH 13
#define MAX_THRESHOLD 4
#define PRINT_REQ 0

#define MIN_SHUF 1
#define MAX_SHUF 2

char ans_arr[19];
int ans_len;

int linear_solve(RubiksCube *rc, int g_score, char *path, int depth)
{
    if (g_score > ans_len / 3)
    {
        return g_score;
    }

    if (is_solved(rc))
    {
        path[depth] = '\0';
        if (PRINT_REQ)
            printf("%s %s %d\n", ans_arr, path, depth);
        if (depth <= ans_len)
        {
            // printf("%s\n", path);
            strcpy(ans_arr, path);
            ans_len = depth;
        }
        return -1;
    }

    int min_threshold = INT_MAX;

    int possible_moves[3] = {'h', 'v', 's'};
    int num_moves[3] = {rc->n, rc->n, 6};
    int directions[2] = {1, 0};
    for (int move = 0; move < 3; move++)
    {
        for (int ele = 0; ele < num_moves[move]; ele++)
        {
            for (int dir = 0; dir < 2; dir++)
            {
                if (g_score > 0 && (path[depth - 3] == possible_moves[move] && (path[depth - 2] - '0') == ele && (path[depth - 1] - '0') == (1 - dir)))
                    continue;
                RubiksCube *cp = create_copy(*rc);
                if (possible_moves[move] == 'h')
                {
                    horizontal_twist(cp, ele, directions[dir]);
                }
                else if (possible_moves[move] == 'v')
                {
                    vertical_twist(cp, ele, directions[dir]);
                }
                else if (possible_moves[move] == 's')
                {
                    side_twist(cp, ele, directions[dir]);
                }

                char curr[19];
                strcpy(curr, path);

                curr[depth] = possible_moves[move];
                curr[depth + 1] = ele + '0';
                curr[depth + 2] = (directions[dir] == 1) ? '1' : '0';
                curr[depth + 3] = '\0';

                int result = linear_solve(cp, g_score + 1, curr, depth + 3);
                free(cp);
                // free(curr);

                if (result == -1)
                {
                    int temp = 1 + 1;
                    // return -1;
                }

                if (result == -1)
                {
                    // return -1; // If solution is found, return success
                    int temp = 1 + 1;
                }

                if (result < min_threshold)
                {
                    min_threshold = result;
                }
            }
        }
    }
    return min_threshold;
}

int parallel_solve(RubiksCube *rc, int g_score, char *path, int depth)
{
    if (g_score > ans_len / 3)
    {
        return g_score;
    }

    if (is_solved(rc))
    {
        path[depth] = '\0';
        if (PRINT_REQ)
            printf("%s %s %d\n", ans_arr, path, depth);
        if (depth <= ans_len)
        {
            // printf("%s\n", path);
            strcpy(ans_arr, path);
            ans_len = depth;
        }
        return -1;
    }

    int min_threshold = INT_MAX;

    int possible_moves[3] = {'h', 'v', 's'};
    int num_moves[3] = {rc->n, rc->n, 6};
    int directions[2] = {1, 0};
    int maxMoves = (rc->n>6)?rc->n:6;
    #pragma omp parallel for collapse(3)
    for (int move = 0; move < 3; move++)
    {
        for (int ele = 0; ele < maxMoves; ele++)
        {
            for (int dir = 0; dir < 2; dir++)
            {
                if (ele >= num_moves[move] || (g_score > 0 && (path[depth - 3] == possible_moves[move] && (path[depth - 2] - '0') == ele && (path[depth - 1] - '0') == (1 - dir))))
                    continue;
                RubiksCube *cp = create_copy(*rc);
                if (possible_moves[move] == 'h')
                {
                    horizontal_twist(cp, ele, directions[dir]);
                }
                else if (possible_moves[move] == 'v')
                {
                    vertical_twist(cp, ele, directions[dir]);
                }
                else if (possible_moves[move] == 's')
                {
                    side_twist(cp, ele, directions[dir]);
                }

                char curr[19];
                strcpy(curr, path);

                curr[depth] = possible_moves[move];
                curr[depth + 1] = ele + '0';
                curr[depth + 2] = (directions[dir] == 1) ? '1' : '0';
                curr[depth + 3] = '\0';

                int result = parallel_solve(cp, g_score + 1, curr, depth + 3);
                free(cp);
                // free(curr);

                if (result == -1)
                {
                    int temp = 1 + 1;
                    // return -1;
                }

                if (result == -1)
                {
                    // return -1; // If solution is found, return success
                    int temp = 1 + 1;
                }

                if (result < min_threshold)
                {
                    min_threshold = result;
                }
            }
        }
    }
    return min_threshold;
}

void execute_moves(RubiksCube *cube, char path[MAX_DEPTH])
{
    printf("\n\nSolution moves:%s\n", path);
    for (int i = 0; path[i] != '\0'; i += 3)
    {
        char move_type = path[i];
        int side = path[i + 1] - '0';
        int direction = (path[i + 2] == '1') ? 1 : 0;

        if (move_type == 'h')
        {
            printf("Horizontal twist on row %d, direction %d\n", side, direction);
            horizontal_twist(cube, side, direction);
        }
        else if (move_type == 'v')
        {
            printf("Vertical twist on column %d, direction %d\n", side, direction);
            vertical_twist(cube, side, direction);
        }
        else if (move_type == 's')
        {
            printf("Side twist on side %d, direction %d\n", side, direction);
            side_twist(cube, side, direction);
        }
    }
    printf("\nCube after solving :\n");
    visualize(cube);
}

void customShuffle(RubiksCube *rc, int min_steps, int max_steps)
{
    srand(time(0));
    int moves = rand() % (max_steps - min_steps + 1) + min_steps;
    printf("\nShuffling %d times : \n", moves);
    for (int i = 0; i < moves; i++)
    {
        int action = rand() % 3;
        int index = rand() % rc->n;
        int direction = rand() % 2;
        int face = rand() % 6;

        switch (action)
        {
        case 0:
            printf("Horizontal twist on row %d, direction %d\n", index, direction);
            horizontal_twist(rc, index, direction);
            break;
        case 1:
            printf("Vertical twist on column %d, direction %d\n", index, direction);
            vertical_twist(rc, index, direction);
            break;
        case 2:
            printf("Side twist on side %d, direction %d\n", face, direction);
            side_twist(rc, face, direction);
            break;
        }
    }
    printf("\n");
}

int main()
{
    char colours[] = "ROYGBW";
    RubiksCube *cube = init_rubiks_cube(3, colours, NULL);

    customShuffle(cube, MIN_SHUF, MAX_SHUF);
    printf("Shuffled cube :\n");
    visualize(cube);

    char path[100]; // Path to store moves
    int result;

    ans_len = MAX_DEPTH;

    clock_t t;
    t = clock();
    // result = linear_solve(cube, 0, path, 0);
    // t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    // strcpy(path, ans_arr);
    // printf("Linear solution : %s\n",path);
    // printf("Linear took %f seconds to execute \n", time_taken);

    // printf("\n");
    t = clock();
    result = parallel_solve(cube, 0, path, 0);
    t = clock() - t;
    printf("Parallel solution : %s\n", path);
    time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    strcpy(path, ans_arr);
    printf("Parallel took %f seconds to execute \n", time_taken);
    strcpy(path, ans_arr);

    execute_moves(cube, path);

    return 0;
}