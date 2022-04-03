#ifndef OCT_H
#define OCT_H

// Data of each octree member
struct Node {
    int layer;		// Depth into tree (root = 0...)
    bool leaf;          // Is the lowest level of the tree holding a cube (or NULL)
    bool surface;	// Node has surface (highest y-value) completely filled in
    int width;		// Size of the node
    int center[3];	// Center of node (x, y, z)
    // TODO Use density as a proxy for when to show far away objects
    int density;	// Number of children of node
    Cube_T *cube;	// NULL if non-leaf node
    struct Node *children[8];	// Non-null if non-leaf node
};
typedef struct Node Node;

int OCT_LoadMap(const char *filePath);
int OCT_AddBlock(Cube_T *cube);
void OCT_DrawMap(windowContext *winParam);

#endif
