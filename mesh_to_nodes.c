/* data format translator script */
/* all rights reserved to Dr. Ron Barron, MAME*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_NODES 20000
#define POSITION_TOLERANCE 1e-6
#define K_NEIGHBORS 8
#define DEBUG 1

typedef struct {
    int node_number;
    double x, y;
    int node_id;
} Node;

typedef struct {
    int W, E, S, N;
} Conn;

// KD-TREE Build
typedef struct KDNode {
    Node point;
    struct KDNode *left, *right;
    int axis;
} KDNode;

double squared_distance(Node a, Node b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return dx*dx + dy*dy;
}

int partition(Node arr[], int l, int r, int axis) {
    double pivot = (axis == 0) ? arr[r].x : arr[r].y;
    int i = l;

    for (int j = l; j < r; j++) {
        double val = (axis == 0) ? arr[j].x : arr[j].y;
        if (val < pivot) {
            Node tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
        }
    }

    Node tmp = arr[i];
    arr[i] = arr[r];
    arr[r] = tmp;

    return i;
}

void quickselect(Node arr[], int l, int r, int k, int axis) {
    if (l >= r) return;

    int pivot = partition(arr, l, r, axis);

    if (k == pivot) return;
    else if (k < pivot)
        quickselect(arr, l, pivot - 1, k, axis);
    else
        quickselect(arr, pivot + 1, r, k, axis);
}

KDNode* build_kdtree(Node arr[], int l, int r, int depth) {
    if (l > r) return NULL;

    int axis = depth % 2;
    int mid = (l + r) / 2;

    quickselect(arr, l, r, mid, axis);

    KDNode* node = (KDNode*)malloc(sizeof(KDNode));
    node->point = arr[mid];
    node->axis = axis;

    node->left  = build_kdtree(arr, l, mid - 1, depth + 1);
    node->right = build_kdtree(arr, mid + 1, r, depth + 1);

    return node;
}


typedef struct {
    double dist;
    Node node;
} Neighbor;

void update_best(Neighbor best[], int *size, Node candidate, double dist) {

    if (*size < K_NEIGHBORS) {
        best[*size].dist = dist;
        best[*size].node = candidate;
        (*size)++;
        return;
    }

    int max_idx = 0;
    for (int i = 1; i < K_NEIGHBORS; i++)
        if (best[i].dist > best[max_idx].dist)
            max_idx = i;

    if (dist < best[max_idx].dist) {
        best[max_idx].dist = dist;
        best[max_idx].node = candidate;
    }
}

void knn_search(KDNode* root, Node target, Neighbor best[], int *size) {
    if (!root) return;

    double d = squared_distance(root->point, target);
    update_best(best, size, root->point, d);

    int axis = root->axis;
    double diff = (axis == 0) ? target.x - root->point.x : target.y - root->point.y;

    KDNode *near = (diff < 0) ? root->left : root->right;
    KDNode *far  = (diff < 0) ? root->right : root->left;

    knn_search(near, target, best, size);

    double maxDist = 0;
    for (int i = 0; i < *size; i++)
        if (best[i].dist > maxDist)
            maxDist = best[i].dist;

    if (*size < K_NEIGHBORS || diff*diff < maxDist)
        knn_search(far, target, best, size);
}

Conn conn[MAX_NODES];
Node nodes[MAX_NODES];
Node sorted[MAX_NODES];

int totalNodes;


// ===================== Compare =====================
int compare_x(const void *a, const void *b) {
    Node *n1 = (Node *)a;
    Node *n2 = (Node *)b;

    if (fabs(n1->x - n2->x) > POSITION_TOLERANCE)
        return (n1->x < n2->x) ? -1 : 1;

    return 0;
}

int compare_y(const void *a, const void *b) {
    Node *n1 = (Node *)a;
    Node *n2 = (Node *)b;

    if (fabs(n1->y - n2->y) > POSITION_TOLERANCE)
        return (n1->y < n2->y) ? -1 : 1;

    return 0;
}


// ===================== COLUMN SORT =====================
void column_sort() {

    qsort(nodes, totalNodes, sizeof(Node), compare_x);

    int index = 0;
    int i = 0;

    while (i < totalNodes) {

        double current_x = nodes[i].x;

        Node column[1000];
        int count = 0;

        while (i < totalNodes &&
            fabs(nodes[i].x - current_x) < POSITION_TOLERANCE) {

            column[count++] = nodes[i];
            i++;
        }


        qsort(column, count, sizeof(Node), compare_y);

        for (int j = 0; j < count; j++) {
            sorted[index++] = column[j];
        }
    }

    for (int i = 0; i < totalNodes; i++) {
        nodes[i] = sorted[i];
        nodes[i].node_number = i + 1;
    }
}



void compute_connectivity() {

    Node temp[MAX_NODES];
    for (int i = 0; i < totalNodes; i++)
        temp[i] = nodes[i];

    KDNode* root = build_kdtree(temp, 0, totalNodes-1, 0);

    for (int i = 0; i < totalNodes; i++) {

        Node current = nodes[i];

        Neighbor best[K_NEIGHBORS];
        int size = 0;

        knn_search(root, current, best, &size);

        double minW = 1e9, minE = 1e9;
        double minS = 1e9, minN = 1e9;

        conn[i].W = conn[i].E = conn[i].S = conn[i].N = 0;

        for (int k = 0; k < size; k++) {

            Node n = best[k].node;

            if (n.node_number == current.node_number) continue;

            double dx = n.x - current.x;
            double dy = n.y - current.y;
            double d  = sqrt(dx*dx + dy*dy);

            // Decide dominant direction
            if (fabs(dx) > fabs(dy)) {
                // Horizontal dominance
                if (dx < 0 && d < minW) {
                    minW = d;
                    conn[i].W = n.node_number;
                }
                else if (dx > 0 && d < minE) {
                    minE = d;
                    conn[i].E = n.node_number;
                }
            } else {
                // Vertical dominance
                if (dy < 0 && d < minS) {
                    minS = d;
                    conn[i].S = n.node_number;
                }
                else if (dy > 0 && d < minN) {
                    minN = d;
                    conn[i].N = n.node_number;
                }
            }
        }
    }
}

int read_nodes_from_mesh(const char *filename) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Cannot open mesh file: %s\n", filename);
        return 0;
    }

    char line[256];
    int numBlocks, minTag, maxTag;

    // -------- READ NODES --------
    while (fgets(line, sizeof(line), file)) {

        if (strncmp(line, "$Nodes", 6) == 0) {

            fgets(line, sizeof(line), file);
            sscanf(line, "%d %d %d %d", &numBlocks, &totalNodes, &minTag, &maxTag);

            int idx = 0;

            for (int b = 0; b < numBlocks; b++) {

                int entityDim, entityTag, parametric, numNodesInBlock;

                fgets(line, sizeof(line), file);
                sscanf(line, "%d %d %d %d", &entityDim, &entityTag, &parametric, &numNodesInBlock);

                // skip IDs
                for (int i = 0; i < numNodesInBlock; i++)
                    fgets(line, sizeof(line), file);

                // read coordinates
                for (int i = 0; i < numNodesInBlock; i++) {

                    double x, y, z;
                    fgets(line, sizeof(line), file);
                    sscanf(line, "%lf %lf %lf", &x, &y, &z);

                    nodes[idx].node_number = idx + 1;
                    nodes[idx].x = x;
                    nodes[idx].y = y;
                    nodes[idx].node_id = entityDim;

                    idx++;
                }
            }

            break;
        }
    }

    fclose(file);

    return 1;
}

int output() {
    FILE *node_out = fopen("nodes0.dat", "w");

    if (node_out == NULL) {
        printf("Cannot open nodes0.dat for writing.\n");
        return 0;
    }
    fprintf(node_out, "%d\n", totalNodes);

    for (int i = 0; i < totalNodes; i++) {
        fprintf(node_out, "%-6d %-12.4f %-12.4f %-2d\n",
                nodes[i].node_number,
                nodes[i].x,
                nodes[i].y,
                nodes[i].node_id);
    }

    fclose(node_out);

    FILE *conn_out = fopen("connectivity0.dat", "w");

    if (conn_out == NULL) {
        printf("Cannot open connectivity0.dat for writing.\n");
        return 0;
    }

    fprintf(conn_out, "\n");
    fprintf(conn_out, "node  w     e     n     s\n");

    for (int i = 0; i < totalNodes; i++) {
        fprintf(conn_out, "%-6d %-5d %-5d %-5d %-5d\n",
                nodes[i].node_number,
                conn[i].W,
                conn[i].E,
                conn[i].N,
                conn[i].S);
    }

    fclose(conn_out);
    return 1;
}

/**
 * Main function to read mesh, sort nodes, compute connectivity, and output results.
 */
int main(void) {

    printf("[DEBUG] Program started.\n");
    printf("[DEBUG] MAX_NODES=%d, K_NEIGHBORS=%d.\n",
                MAX_NODES, K_NEIGHBORS);

    if (!read_nodes_from_mesh("circular2.msh")) {
    return EXIT_FAILURE;
    }


    // -------- SORT --------
    printf("[DEBUG] Beginning sorting.\n");
    column_sort();
    printf("[DEBUG] Sorting finished.\n");


    // -------- CONNECTIVITY --------
    printf("[DEBUG] Beginning connectivity calculation.\n");
    compute_connectivity();
    if (!output()) {
        return EXIT_FAILURE;
    }
    printf("[DEBUG] Connectivity calculation finished.\n");


    printf("[DEBUG] Beginning output.\n");

    output();

    printf("[DEBUG] Output generation succeeded.\n");
    printf("[DEBUG] Program completed successfully.\n");


    return EXIT_SUCCESS;


}