

// Data of each octree member
struct Node {
    int layer;		// Depth into tree (root = 0...)

    int width;		// Size of the node
    int center[3];	// Center of node (x, y, z)
    // TODO Use density as a proxy for when to show far away objects
    int density;	// Number of children of node
    Cube_T *cube;	// NULL if non-leaf node
    struct Node *children[8];	// Non-null if non-leaf node
};
typedef struct Node Node;

void OCT_DrawMap(windowContext *winParam);
int OCT_LoadMap(const char *filePath);
