// Scanline Polygon fill Algorithm code found at https://www.geeksforgeeks.org/scan-line-polygon-filling-using-opengl-c/
// converted to Raylib and mucked about with by me (Les Farrell)

#include <stdio.h>
#include <math.h>
#include <raylib.h>

#define PolygonFill_MaxHeight 800
#define PolygonFill_MaxVertices 1000

// Start from lower left corner
typedef struct PolygonFill_EdgeBucket
{
    int ymax;      // max y-coordinate of edge
    float xofymin; // x-coordinate of lowest edge point updated only in aet
    float slopeinverse;
} PolygonFill_EdgeBucket;

typedef struct PolygonFill_edgetabletup
{
    // The array will give the scanline number
    // The edge table (ET) with edges entries sorted in increasing y and x of the lower end
    int countPolygonFill_EdgeBucket; // No. of PolygonFill_EdgeBuckets
    PolygonFill_EdgeBucket buckets[PolygonFill_MaxVertices];
} PolygonFill_PolygonFill_edgetabletuple;

PolygonFill_PolygonFill_edgetabletuple EdgeTable[PolygonFill_MaxHeight], ActiveEdgeTuple;

// Scanline Function
void PolygonFill_InitEdgeTable()
{
    for (int i = 0; i < PolygonFill_MaxHeight; i++)
    {
        EdgeTable[i].countPolygonFill_EdgeBucket = 0;
    }
    ActiveEdgeTuple.countPolygonFill_EdgeBucket = 0;
}

/* Function to sort an array using insertion sort*/
void PolygonFill_InsertionSort(PolygonFill_PolygonFill_edgetabletuple *ett)
{
    int j = 0;
    PolygonFill_EdgeBucket temp;

    for (int i = 1; i < ett->countPolygonFill_EdgeBucket; i++)
    {
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

        while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0))
        {
            ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
            ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
            ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
            j = j - 1;
        }
        ett->buckets[j + 1].ymax = temp.ymax;
        ett->buckets[j + 1].xofymin = temp.xofymin;
        ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}

void PolygonFill_StoreEdgeInTuple(PolygonFill_PolygonFill_edgetabletuple *receiver, int ym, int xm, float slopInv)
{
    // Both used for edgetable and active edge table..
    // The edge tuple sorted in increasing ymax and x of the lower end.
    (receiver->buckets[(receiver)->countPolygonFill_EdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countPolygonFill_EdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countPolygonFill_EdgeBucket]).slopeinverse = slopInv;

    // Sort the buckets
    PolygonFill_InsertionSort(receiver);

    (receiver->countPolygonFill_EdgeBucket)++;
}

void PolygonFill_StoreEdgeInTable(int x1, int y1, int x2, int y2)
{
    float m = 0.0, minv = 0.0;
    int ymaxTS, xwithyminTS, scanline; // ts stands for to store

    if (x2 == x1)
    {
        minv = 0.000000;
    }
    else
    {
        m = ((float)(y2 - y1)) / ((float)(x2 - x1));

        // horizontal lines are not stored in edge table
        if (y2 == y1)
            return;

        minv = (float)1.0 / m;
    }

    if (y1 > y2)
    {
        scanline = y2;
        ymaxTS = y1;
        xwithyminTS = x2;
    }
    else
    {
        scanline = y1;
        ymaxTS = y2;
        xwithyminTS = x1;
    }

    // The assignment part is done..now storage..
    PolygonFill_StoreEdgeInTuple(&EdgeTable[scanline], ymaxTS, xwithyminTS, minv);
}

void PolygonFill_RemoveEdgeByYmax(PolygonFill_PolygonFill_edgetabletuple *Tup, int yy)
{
    for (int i = 0; i < Tup->countPolygonFill_EdgeBucket; i++)
    {
        if (Tup->buckets[i].ymax == yy)
        {
            for (int j = i; j < Tup->countPolygonFill_EdgeBucket - 1; j++)
            {
                Tup->buckets[j].ymax = Tup->buckets[j + 1].ymax;
                Tup->buckets[j].xofymin = Tup->buckets[j + 1].xofymin;
                Tup->buckets[j].slopeinverse = Tup->buckets[j + 1].slopeinverse;
            }
            Tup->countPolygonFill_EdgeBucket--;
            i--;
        }
    }
}

void PolygonFill_UpdateXbySlopeinv(PolygonFill_PolygonFill_edgetabletuple *Tup)
{
    for (int i = 0; i < Tup->countPolygonFill_EdgeBucket; i++)
    {
        (Tup->buckets[i]).xofymin = (Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}

void PolygonFill_ScanlineFill()
{
    /* Follow the following rules: 
	1. Horizontal edges: Do not include in edge table 
	2. Horizontal edges: Drawn either on the bottom or on the top. 
	3. Vertices: If local max or min, then count twice, else count once. 
	4. Either vertices at local minima or at local maxima are drawn.*/
    int i = 0, j = 0, x1 = 0, ymax1 = 0, x2 = 0, ymax2 = 0, FillFlag = 0, coordCount = 0;

    // we will start from scanline 0;
    // Repeat until last scanline:
    for (i = 0; i < PolygonFill_MaxHeight; i++) //4. Increment y by 1 (next scan line)
    {

        // 1. Move from ET bucket y to the
        // AET those edges whose ymin = y (entering edges)
        for (j = 0; j < EdgeTable[i].countPolygonFill_EdgeBucket; j++)
        {
            PolygonFill_StoreEdgeInTuple(&ActiveEdgeTuple, EdgeTable[i].buckets[j].ymax, EdgeTable[i].buckets[j].xofymin,
                                         EdgeTable[i].buckets[j].slopeinverse);
        }

        // 2. Remove from AET those edges for which y=ymax (not involved in next scan line)
        PolygonFill_RemoveEdgeByYmax(&ActiveEdgeTuple, i);

        // sort AET (remember: ET is presorted)
        PolygonFill_InsertionSort(&ActiveEdgeTuple);

        // 3. Fill lines on scan line y by using pairs of x-coords from AET
        j = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;
        FillFlag = 0;
        coordCount = 0;
        while (j < ActiveEdgeTuple.countPolygonFill_EdgeBucket)
        {
            if (coordCount % 2 == 0)
            {
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1 == x2)
                {
                    /* three cases can arrive- 
					1. lines are towards top of the intersection 
					2. lines are towards bottom 
					3. one line is towards top and other is towards bottom 
					*/
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else
                    {
                        coordCount++;
                    }
                }
                else
                {
                    coordCount++;
                }
            }
            else
            {
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;

                FillFlag = 0;

                // Checking for intersection...
                if (x1 == x2)
                {
                    /* Three cases can arise - 
					1. Lines are towards top of the intersection 
					2. Lines are towards bottom 
					3. One line is towards top and other is towards bottom 
					*/
                    if (((x1 == ymax1) && (x2 != ymax2)) || ((x1 != ymax1) && (x2 == ymax2)))
                    {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else
                    {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else
                {
                    coordCount++;
                    FillFlag = 1;
                }

                if (FillFlag)
                {
                    DrawLine(x1, i, x2, i, GRAY);
                }
            }

            j++;
        }

        // 5. For each nonvertical edge remaining in AET, update x for new y
        PolygonFill_UpdateXbySlopeinv(&ActiveEdgeTuple);
    }
}

void PolygonFill_DrawPolyDino()
{
    FILE *fp;

    int count = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    PolygonFill_InitEdgeTable();

    fp = fopen("PolyDino.txt", "r");
    while (!feof(fp))
    {
        count++;
        if (count > 2)
        {
            x1 = x2;
            y1 = y2;
            count = 2;
        }
        if (count == 1)
        {
            fscanf(fp, "%d,%d", &x1, &y1);
        }
        else
        {
            fscanf(fp, "%d,%d", &x2, &y2);
            PolygonFill_StoreEdgeInTable(x1, PolygonFill_MaxHeight - y1, x2, PolygonFill_MaxHeight - y2); //storage of edges in edge table.
        }
    }
    fclose(fp);
}

int main(int argc, char **argv)
{
    InitWindow(800, 600, "Polygon Filler");

    PolygonFill_InitEdgeTable();

    PolygonFill_DrawPolyDino();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        PolygonFill_ScanlineFill();

        EndDrawing();
    }
    CloseWindow();

    return 0;
}
