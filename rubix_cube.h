#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

typedef struct RubiksCube
{
    int n;         
    char **cube[6];
    char *colours; 
} RubiksCube;

RubiksCube *init_rubiks_cube(int n, char *colours, char *state);
void free_rubiks_cube(RubiksCube *rc);
void transpose_face(char **face, int n);
void reset_rubiks_cube(RubiksCube *rc);
int is_solved(RubiksCube *rc);
char *stringify(RubiksCube *rc);
void shuffle(RubiksCube *rc, int l_rot, int u_rot);
void show(RubiksCube *rc);
void visualize(RubiksCube *rc);
void horizontal_twist(RubiksCube *rc, int row, int direction);
void vertical_twist(RubiksCube *rc, int column, int direction);
void side_twist(RubiksCube *rc, int face, int direction);

RubiksCube *init_rubiks_cube(int n, char *colours, char *state)
{
    RubiksCube *rc = (RubiksCube *)malloc(sizeof(RubiksCube));
    rc->n = n;
    for (int i = 0; i < 6; i++)
    {
        rc->cube[i] = (char **)malloc(n * sizeof(char *));
        for (int j = 0; j < n; j++)
        {
            rc->cube[i][j] = (char *)malloc(n * sizeof(char));
        }
    }

    rc->colours = (char *)malloc(6 * sizeof(char));

    for (int i = 0; i < 6; i++)
        rc->colours[i] = colours[i];

    if (state == NULL)
        reset_rubiks_cube(rc);
    else
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < n; j++)
            {
                for (int k = 0; k < n; k++)
                    rc->cube[i][j][k] = state[i * (n * n) + j * n + k];
            }
        }
    }

    return rc;
}

RubiksCube *create_copy(RubiksCube rc)
{
    RubiksCube *cp = init_rubiks_cube(rc.n, rc.colours, NULL);
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < rc.n; j++)
        {
            for (int k = 0; k < rc.n; k++)
                cp->cube[i][j][k] = rc.cube[i][j][k];
        }
    }
    return cp;
}

void free_rubiks_cube(RubiksCube *rc)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < rc->n; j++)
        {
            free(rc->cube[i][j]);
        }
        free(rc->cube[i]);
    }
    free(rc->colours);
    free(rc);
}

void reset_rubiks_cube(RubiksCube *rc)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < rc->n; j++)
        {
            for (int k = 0; k < rc->n; k++)
            {
                rc->cube[i][j][k] = rc->colours[i];
            }
        }
    }
}

void transpose_face(char **face, int n)
{
    char temp;
    for (int i = 0; i < n / 2; i++)
    {
        for (int j = i; j < n - i - 1; j++)
        {
            temp = face[i][j];
            face[i][j] = face[j][n - i - 1];
            face[j][n - i - 1] = face[n - i - 1][n - j - 1];
            face[n - i - 1][n - j - 1] = face[n - j - 1][i];
            face[n - j - 1][i] = temp;
        }
    }
}

int is_solved(RubiksCube *rc)
{
    for (int i = 0; i < 6; i++)
    {
        char color = rc->cube[i][0][0];
        for (int j = 0; j < rc->n; j++)
        {
            for (int k = 0; k < rc->n; k++)
            {
                if (rc->cube[i][j][k] != color)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

char *stringify(RubiksCube *rc)
{
    int len = 6 * rc->n * rc->n;
    char *result = (char *)malloc(len + 1);

    int index = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < rc->n; j++)
        {
            for (int k = 0; k < rc->n; k++)
            {
                result[index++] = rc->cube[i][j][k];
            }
        }
    }
    result[len] = '\0';
    return result;
}

void shuffle(RubiksCube *rc, int l_rot, int u_rot)
{
    int moves = rand() % (u_rot - l_rot + 1) + l_rot;
    for (int i = 0; i < moves; i++)
    {
        int action = rand() % 3;
        int index = rand() % rc->n;
        int direction = rand() % 2;

        switch (action)
        {
        case 0:
            horizontal_twist(rc, index, direction);
            break;
        case 1:
            vertical_twist(rc, index, direction);
            break;
        case 2:
            side_twist(rc, index, direction);
            break;
        }
    }
}

void show(RubiksCube *rc)
{
    for (int i = 0; i < 6; i++)
    {
        printf("Face %d:\n", i);
        for (int j = 0; j < rc->n; j++)
        {
            for (int k = 0; k < rc->n; k++)
            {
                printf("%c ", rc->cube[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

void visualize(RubiksCube *rc)
{
    int n = rc->n;

    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
            printf("  ");
        printf("\t");
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[4][j][k]);
        printf("\n");
    }
    printf("\n");
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[3][j][k]);
        printf("\t");
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[0][j][k]);
        printf("\t");
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[2][j][k]);
        printf("\n");
    }
    printf("\n");
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
            printf("  ");
        printf("\t");
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[5][j][k]);
        printf("\n");
    }
    printf("\n");
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
            printf("  ");
        printf("\t");
        for (int k = 0; k < n; k++)
            printf("%c ", rc->cube[1][n - j - 1][n - k - 1]);
        printf("\n");
    }
    printf("\n");
}

void horizontal_twist(RubiksCube *rc, int row, int direction)
{
    int n = rc->n;
    char temp[n];

    // Rotate front-left-back-right
    if (direction == 0)
    { // Clockwise
        // Copy front row
        for (int i = 0; i < n; i++)
        {
            temp[i] = rc->cube[0][row][i];
        }
        // Front <- Left
        for (int i = 0; i < n; i++)
        {
            rc->cube[0][row][i] = rc->cube[3][row][i];
        }
        // Left <- Back
        for (int i = 0; i < n; i++)
        {
            rc->cube[3][row][i] = rc->cube[1][row][n - i - 1]; // Reverse
        }
        // Back <- Right
        for (int i = 0; i < n; i++)
        {
            rc->cube[1][row][n - i - 1] = rc->cube[2][row][i];
        }
        // Right <- temp (was Front)
        for (int i = 0; i < n; i++)
        {
            rc->cube[2][row][i] = temp[i];
        }
    }
    else
    { // Counterclockwise
        // Copy front row
        for (int i = 0; i < n; i++)
        {
            temp[i] = rc->cube[0][row][i];
        }
        // Front <- Right
        for (int i = 0; i < n; i++)
        {
            rc->cube[0][row][i] = rc->cube[2][row][i];
        }
        // Right <- Back
        for (int i = 0; i < n; i++)
        {
            rc->cube[2][row][i] = rc->cube[1][row][n - i - 1]; // Reverse
        }
        // Back <- Left
        for (int i = 0; i < n; i++)
        {
            rc->cube[1][row][n - i - 1] = rc->cube[3][row][i];
        }
        // Left <- temp (was Front)
        for (int i = 0; i < n; i++)
        {
            rc->cube[3][row][i] = temp[i];
        }
    }

    // Rotate the top or bottom face if necessary
    if (row == 0)
    {
        transpose_face(rc->cube[4], n); // Top face clockwise
        if (direction != 0)
        {
            transpose_face(rc->cube[4], n); // Bottom face clockwise
            transpose_face(rc->cube[4], n); // Bottom face clockwise
        }
    }
    else if (row == n - 1)
    {
        transpose_face(rc->cube[5], n); // Bottom face clockwise
        if (direction == 0)
        {
            transpose_face(rc->cube[5], n); // Bottom face clockwise
            transpose_face(rc->cube[5], n); // Bottom face clockwise
        }
    }
}

void vertical_twist(RubiksCube *rc, int column, int direction)
{
    int n = rc->n;
    char temp[n];

    // Rotate top-front-bottom-back
    if (direction == 0)
    { // Clockwise
        // Copy top column
        for (int i = 0; i < n; i++)
        {
            temp[i] = rc->cube[4][i][column];
        }
        // Top <- Front
        for (int i = 0; i < n; i++)
        {
            rc->cube[4][i][column] = rc->cube[0][i][column];
        }
        // Front <- Bottom
        for (int i = 0; i < n; i++)
        {
            rc->cube[0][i][column] = rc->cube[5][n - i - 1][column];
        }
        // Bottom <- Back
        for (int i = 0; i < n; i++)
        {
            rc->cube[5][n - i - 1][column] = rc->cube[1][i][n - column - 1]; // Reverse
        }
        // Back <- temp (was Top)
        for (int i = 0; i < n; i++)
        {
            rc->cube[1][i][n - column - 1] = temp[i];
        }
    }
    else
    { // Counterclockwise
        // Copy top column
        for (int i = 0; i < n; i++)
        {
            temp[i] = rc->cube[4][i][column];
        }
        // Top <- Back
        for (int i = 0; i < n; i++)
        {
            rc->cube[4][i][column] = rc->cube[1][i][n - column - 1]; // Reverse
        }
        // Back <- Bottom
        for (int i = 0; i < n; i++)
        {
            rc->cube[1][i][n - column - 1] = rc->cube[5][n - i - 1][column];
        }
        // Bottom <- Front
        for (int i = 0; i < n; i++)
        {
            rc->cube[5][n - i - 1][column] = rc->cube[0][i][column];
        }
        // Front <- temp (was Top)
        for (int i = 0; i < n; i++)
        {
            rc->cube[0][i][column] = temp[i];
        }
    }

    // Rotate left or right face if necessary
    if (column == 0)
    {
        transpose_face(rc->cube[3], n);
        if (direction != 0)
        {
            transpose_face(rc->cube[3], n);
            transpose_face(rc->cube[3], n);
        }
    }
    else if (column == n - 1)
    {
        transpose_face(rc->cube[2], n); // Right face clockwise
        if (direction == 0)
        {
            transpose_face(rc->cube[2], n);
            transpose_face(rc->cube[2], n);
        }
    }
}

void side_twist(RubiksCube *rc, int face, int direction)
{
    int n = rc->n;
    if (face == 0 || face == 1)
    {
        // Rotating the actual face itself
        if (direction == 0)
        { // Clockwise
            transpose_face(rc->cube[face], n);
        }
        else
        { // Counterclockwise
            // Rotate counterclockwise is equivalent to 3 clockwise rotations
            for (int i = 0; i < 3; i++)
            {
                transpose_face(rc->cube[face], n);
            }
        }

        // Rotating the adjacent rows of faces 2, 3, 4, 5
        char temp_row[n];

        if (face == 0)
        {
            if (direction != 0) // Clockwise
            {
                // Save top row of face 4
                for (int i = 0; i < n; i++)
                    temp_row[i] = rc->cube[4][n - 1][i];

                // Face 4 top row <- Face 3 top row
                for (int i = 0; i < n; i++)
                    rc->cube[4][n - 1][i] = rc->cube[3][i][n - 1];

                // Face 3 top row <- Face 5 top row
                for (int i = 0; i < n; i++)
                    rc->cube[3][i][n - 1] = rc->cube[5][0][i];

                // Face 5 top row <- Face 2 top row
                for (int i = 0; i < n; i++)
                    rc->cube[5][0][i] = rc->cube[2][i][0];

                // Face 2 top row <- temp (original Face 4 top row)
                for (int i = 0; i < n; i++)
                    rc->cube[2][i][0] = temp_row[i];
            }
            else // Counterclockwise
            {
                // Save top row of face 4
                for (int i = 0; i < n; i++)
                    temp_row[i] = rc->cube[4][n - 1][i];

                // Face 4 top row <- Face 3 top row
                for (int i = 0; i < n; i++)
                    rc->cube[4][n - 1][i] = rc->cube[2][i][0];

                // Face 3 top row <- Face 5 top row
                for (int i = 0; i < n; i++)
                    rc->cube[2][i][0] = rc->cube[5][0][i];

                // Face 5 top row <- Face 2 top row
                for (int i = 0; i < n; i++)
                    rc->cube[5][0][i] = rc->cube[3][i][n - 1];

                // Face 2 top row <- temp (original Face 4 top row)
                for (int i = 0; i < n; i++)
                    rc->cube[3][i][n - 1] = temp_row[i];
            }
        }
        else if (face == 1)
        {
            if (direction != 0) // Clockwise
            {
                // Save top row of face 4
                for (int i = 0; i < n; i++)
                    temp_row[i] = rc->cube[4][0][i];

                // Face 4 top row <- Face 3 top row
                for (int i = 0; i < n; i++)
                    rc->cube[4][0][i] = rc->cube[3][i][0];

                // Face 3 top row <- Face 5 top row
                for (int i = 0; i < n; i++)
                    rc->cube[3][i][0] = rc->cube[5][n - 1][i];

                // Face 5 top row <- Face 2 top row
                for (int i = 0; i < n; i++)
                    rc->cube[5][n - 1][i] = rc->cube[2][i][n - 1];

                // Face 2 top row <- temp (original Face 4 top row)
                for (int i = 0; i < n; i++)
                    rc->cube[2][i][n - 1] = temp_row[i];
            }
            else // Counterclockwise
            {
                // Save top row of face 4
                for (int i = 0; i < n; i++)
                    temp_row[i] = rc->cube[4][0][i];

                // Face 4 top row <- Face 3 top row
                for (int i = 0; i < n; i++)
                    rc->cube[4][0][i] = rc->cube[2][i][n - 1];

                // Face 3 top row <- Face 5 top row
                for (int i = 0; i < n; i++)
                    rc->cube[2][i][n - 1] = rc->cube[5][n - 1][i];

                // Face 5 top row <- Face 2 top row
                for (int i = 0; i < n; i++)
                    rc->cube[5][n - 1][i] = rc->cube[3][i][0];

                // Face 2 top row <- temp (original Face 4 top row)
                for (int i = 0; i < n; i++)
                    rc->cube[3][i][0] = temp_row[i];
            }
        }
    }
    else
    {
        if (face == 3)
            vertical_twist(rc, 0, direction);
        else if (face == 2)
            vertical_twist(rc, n - 1, direction);
        else if (face == 4)
            horizontal_twist(rc, 0, direction);
        else if (face == 5)
            horizontal_twist(rc, n - 1, direction);
    }
}
