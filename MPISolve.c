#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include "rubix_cube.h"

#define MAX_MOVES 13
#define MAX_DEPTH 13
#define MAX_THRESHOLD 4
#define PRINT_REQ 0

int ans_len;

int solve(RubiksCube *rc, int g_score, char *path, int depth, char *ans_arr)
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

                int result = solve(cp, g_score + 1, curr, depth + 3, ans_arr);
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
            int face = rand() % 6;
            printf("Side twist on side %d, direction %d\n", face, direction);
            side_twist(rc, face, direction);
            break;
        }
    }
    printf("\n");
}

void execute_moves(RubiksCube *cube, char path[MAX_DEPTH])
{
    if(path[0]=='.'){
        printf("No result.\n");
        return;
    }
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

int main(int argc, char *argv[])
{
    int rank, size;
    char path[MAX_MOVES];
    char state[54];
    int *all_lengths=NULL;
    char *all_strings = NULL;

    char empty[MAX_MOVES];
    for(int i=0; i<MAX_MOVES; i++)
        empty[i]='.';

    ans_len = MAX_DEPTH;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char colours[] = "ROYGBW";
    RubiksCube *cube = NULL;

    if (rank == 0)
    {
        cube = init_rubiks_cube(3, colours, NULL);
        customShuffle(cube, 3, 3);
        printf("Shuffled cube :\n");
        visualize(cube);
        strncpy(state, stringify(cube), 54);
        printf("STATE:%s\n", state);
    }

    MPI_Bcast(state, 54, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank!=0){
        cube = init_rubiks_cube(3, colours, state);
    }

    if (is_solved(cube))
    {
        printf("THIS IS ALREADY SOLVED.\n");
        return 0;
    }

    if (rank == 0)
    {
        all_strings = (char *)malloc(MAX_MOVES * size * sizeof(char));
        all_lengths = (int *)malloc(size * sizeof(char));
    }


    char temp[MAX_MOVES];
    strcpy(path, empty);

    for(int i=rank; i<2*(2*cube->n+6); i+=size){
        int dir = i / (2 * cube->n + 6);
        int type = i;
        type-=dir*(2 * cube->n + 6);
        if(type>=(2*cube->n))
            type=2;
        else if(type>=(cube->n))
            type=1;
        else
            type=0;
        int value = i;
        if(dir==1)
            value-=(2 * cube->n + 6);
        value-= type*(cube->n);
        if (PRINT_REQ)
            printf("%d]%d-%d-%d\n", rank, type, value, dir);
        char temp[MAX_MOVES];
        switch(type){
            case 0:
                horizontal_twist(cube, value, dir);
                temp[0] = 'h';
                break;
            case 1:
                vertical_twist(cube, value, dir);
                temp[0] = 'v';
                break;
            case 2:
                side_twist(cube, value, dir);
                temp[0] = 's';
                break;
            }
            temp[1] = '0' + value;
            temp[2] = '0' + dir;
            solve(cube, 1, temp, 3, path);
            if (PRINT_REQ)
                printf("%d]%s\n", rank, path);
            switch (type)
            {
            case 0:
                horizontal_twist(cube, value, 1 - dir);
                break;
            case 1:
                vertical_twist(cube, value, 1 - dir);
                break;
            case 2:
                side_twist(cube, value, 1 - dir);
                break;
        }
    }

    MPI_Gather(path, MAX_MOVES, MPI_CHAR, all_strings, MAX_MOVES, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Gather(&ans_len, 1, MPI_INT, all_lengths, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        char final_ans[MAX_MOVES];
        int len=MAX_MOVES;
        printf("All strings gathered at process 0:\n");
        for (int i = 0; i < size; i++)
        {
            printf("String from process %d-[%d]: %.20s\n", i, all_lengths[i], &all_strings[i * MAX_MOVES]);
            if(all_lengths[i]<len){
                len=all_lengths[i];
                strcpy(final_ans, empty);
                strncpy(final_ans, &all_strings[i*MAX_MOVES], len);
            }
        }
        final_ans[len]='\0';

        execute_moves(cube, final_ans);

        free(all_strings);
    }

    MPI_Finalize();
    return 0;
}
