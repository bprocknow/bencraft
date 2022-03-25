
#include "initgl.h"
#include "world.h"
#include "cube.h"
#include "display.h"
#include "log.h"
#include "octree.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

Node *root = NULL;

static void depthFirstDraw(windowContext *winParam, Node *node);
static int initializeTree(Cube_T *cube);
static int findQuadrant(int x, int y, int z, Node *curNode);
static void calculateCenter(Node *child, Node *parent, int childQuadrant);
static int allocateNode(Node *parent, int quadrant);
static int recAddBlock(Cube_T *cube, Node *curNode);

static Cube_T *findCube(int x, int y, int z) {
    // Case 0: Root is null -> no cubes
    if (root == NULL) {
        return NULL;
    }
    
    // Case 1: Root is leaf 
    if (root->cube) {
        if (root->cube->x == x && root->cube->y == y && root->cube->z == z) {
            return root->cube;
        }
    }

    // Case 2: Root has children -> search down the tree
    Node *curNode = root;
    while (1) {
	int quadrant = findQuadrant(x, y, z, curNode);
        
	if (curNode->children[quadrant] == NULL) {
            return NULL;
	}

        curNode = curNode->children[quadrant];

        if (curNode->cube) {
            // Found leaf
	    if (curNode->cube->x == x && curNode->cube->y == y && curNode->cube->z == z) {
                return curNode->cube;
	    } else {
                return NULL;
	    }
	}
    }

    return 0;
}

int OCT_LoadMap(const char *filePath) {
    
    FILE *fp;

    fp = fopen(filePath, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open file %s\n", filePath);
	return 0;
    }
    
    int x, y, z = 0;
    do {
        char c = fgetc(fp);
	if (feof(fp)) {
            break;
	}
        
	if (c == '\n') {
	    z = 0;
	    y += 1;
	    continue;
	} 
        
        if (y % MAPSIZE == 0 && y != 0) {
            x += 1;
	    z = 0;
	    y = 0;
	}
       	
	if (x == 4 || y == 4 || z ==4) {
            printf("Wrong %c: %d, %d, %d\n", c, x, y, z);
	    return 0;
	}
	// TODO Make map's numbers contain which type of block should be created
        if (c == '1') {
	    Cube_T *cube;
	    if (x == 0 || y == 0 || z == 0) {
	        cube = CUBE_GenerateCube(x, y, z, GROUND); 
	    } else {
	        cube = CUBE_GenerateCube(x, y, z, GRASS); 
	    }
            OCT_AddBlock(cube);
	}

	z += 1;
    } while (1);

    fclose(fp);
    
    return 1;
}

/**
    Perform depth first search on the octree, passing the neighbors (positive x (Px), negative x (Nx)...) of each cube while recursing down the tree.
    
    Base case 0: node is a leaf.  Draw the node if it is visible
    Recursive case: Starting from the highest y-value cube, continue depth first search until the cubes are no longer visible
    Base case 1: If the cubes in the current node cover the entire ground, return True.  Else false and keep searching downwards.

*/
static bool depthFirstDraw(windowContext *winParam, Node *node, Node *nodePx, Node *nodeMx, Node *nodePz, Node *nodeMz, Node *nodeT) {
    // Base case 0: Node is a leaf
    if (node->cube) {
        return drawLeaf(winParam, node, nodePx, nodeNx, nodePz, nodeMz, nodeT);
    }
    // Loop through the four columns of the octree (Indicies [0, 4], [1, 5], [2, 6], [3, 7])
    for (int i = 0; i < 4; i++) {
        // Find if the upper y-value node is visible
	if (node->child[i]) {
	    // Node above this node is not the surface
            if (!nodeT || !nodeT->child[i+4] || !nodeT->child[i+4].surface) {
                node->child[i].surface = passNeighbor(x, node, nodePx, nodeMx, nodePz, nodeMz, nodeT);
	    } else {
                node->child[i].surface = true;
	    }
	}
	// Lower y-value node
	if (node->child[i+4]) {
	    if (node->child[i] == NULL || !node->child[x].surface) {
	        // The top node to the lower y-value node is the upper y-value node in the same column
                node->child[i].surface = passNeighbor(x+4, node, nodePx, nodeMx, nodePz, nodeMz, NULL);
	    } else {
                node->child[i].surface = true;
	    }
	}
    }
    
    // Base case 1: Do the cubes in this node cover the ground of the oct-block
    if ((node->child[0] || node->child[4]) && (node->child[1] || node->child[5]) &&
      (node->child[2] || node->child[6]) && (node->child[3] || node->child[7])) {
        return true;
    }
    return false;
}

void OCT_DrawMap(windowContext *winParam) {
    // Depth first search 
    if (root == NULL) {
        LOG("Root is null.  Cannot draw map");
    }

    // Root doesn't have any neighbors
    depthFirstDraw(winParam, root, NULL, NULL, NULL, NULL, NULL);
}

static int initializeTree(Cube_T *cube) {

    root = calloc(1, sizeof(Node));
    if (root == NULL) {
        fprintf(stderr, "Cannot allocate map\n");
        return 0;
    }
    root->layer = 0;
    root->width = gMapSize / 2;
    root->center[0] = gMapSize / 2;
    root->center[1] = gMapSize / 2;
    root->center[2] = gMapSize / 2;
    root->cube = cube;
    memset(root->children, 0, sizeof(root->children));
    
    return 1;
}

// Find the quadrant [0, 7] that x,y,z belong to with origin curNode
static int findQuadrant(int x, int y, int z, Node *curNode) {
    
    if (x < curNode->center[0] && y >= curNode->center[1] && z < curNode->center[2]) {
        return 0;
    }
    if (x < curNode->center[0] && y >= curNode->center[1] && z >= curNode->center[2]) {
        return 1;
    }
    if (x >= curNode->center[0] && y >= curNode->center[1] && z < curNode->center[2]) {
        return 2;
    }
    if (x >= curNode->center[0] && y >= curNode->center[1] && z >= curNode->center[2]) {
        return 3;
    }
    if (x < curNode->center[0] && y < curNode->center[1] && z < curNode->center[2]) {
        return 4;
    }
    if (x < curNode->center[0] && y < curNode->center[1] && z >= curNode->center[2]) {
        return 5;
    }
    if (x >= curNode->center[0] && y < curNode->center[1] && z < curNode->center[2]) {
        return 6;
    }
    if (x >= curNode->center[0] && y < curNode->center[1] && z >= curNode->center[2]) {
        return 7;
    }

    fprintf(stderr, "ERROR: Could not find a quadrant for cube coords (%d, %d, %d)\n", 
      x, y, z);
    return -1;
}

static void calculateCenter(Node *child, Node *parent, int childQuadrant) {
    int moveAmt = parent->width / 2;

    if (childQuadrant == 0) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 1) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 2) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 3) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 4) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 5) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 6) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 7) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    fprintf(stderr, "ERROR:  Calculate Center failed.  Invalid child quadrant: %d\n", childQuadrant);
}

static int allocateNode(Node *parent, int quadrant) {
    
    parent->children[quadrant] = calloc(1, sizeof(Node));
    
    if (parent->children[quadrant] == NULL) {
        fprintf(stderr, "ERROR: Out of memory, could not allocate new node.  Map too large\n");
        return 0;
    }

    parent->children[quadrant]->layer = parent->layer + 1;
    parent->children[quadrant]->density = 0;
    
    parent->children[quadrant]->width = parent->width / 2;
    calculateCenter(parent->children[quadrant], parent, quadrant);
    memset(parent->children[quadrant]->children, '\0', sizeof(parent->children[quadrant]->children));

    return 1;
}

// Find the neighbors of the inputted cube and set their visible faces to 0 (false)
static void setFaces(Cube_T *cube) {
    Cube_T *neighbor;

    // Cube doesn't have neighbors if it is the edge of world
    if (cube->z > 0) {
        neighbor = findCube(cube->x, cube->y, cube->z - 1);
	if (neighbor) {
	    neighbor->face[2] = true;
	    cube->face[0] = true;
	}
    }
    if (cube->z < gMapSize - 1) {
        neighbor = findCube(cube->x, cube->y, cube->z + 1);
        if (neighbor) {
            neighbor->face[0] = true;
	    cube->face[2] = true;
	}
    }
    if (cube->x  > 0) {
        neighbor = findCube(cube->x - 1, cube->y, cube->z);
        if (neighbor) {
            neighbor->face[1] = true;
	    cube->face[3] = true;
	}
    }
    if (cube->x < gMapSize - 1) {
        neighbor = findCube(cube->x + 1, cube->y, cube->z);
        if (neighbor) {
            neighbor->face[3] = true;
	    cube->face[1] = true;
	}
    }
    if (cube->y > 0) {
        neighbor = findCube(cube->x, cube->y - 1, cube->z);
        if (neighbor) {
            neighbor->face[5] = true;
	    cube->face[4] = true;
	}
    }
    if (cube->y < gMapSize) {
        neighbor = findCube(cube->x, cube->y + 1, cube->z);
        if (neighbor) {
            neighbor->face[4] = true;
	    cube->face[5] = true;
	}
    }
}

static int recAddBlock(Cube_T *cube, Node *curNode) {
    int quadrant;

    // Case 0: curNode is leaf node - split node if possible
    if (curNode->cube != NULL) {
        // Move the cube in the current node to the children of current node
	quadrant = findQuadrant(curNode->cube->x, curNode->cube->y, curNode->cube->z, curNode);

	if (quadrant < 0) {
            fprintf(stderr, "ERROR: Failed to add block\n");
	    return 0;
	}

	if (curNode->children[quadrant] != NULL) {
            fprintf(stderr, "ERROR: Leaf node has previously allocated children\n");
	}

        if (!allocateNode(curNode, quadrant)) {
            return 0;
	}
        
        curNode->children[quadrant]->cube = curNode->cube;
	curNode->cube = NULL;
        
	// Now that the current block is split, call recursive again for case 1 or 2 
	// TODO Handle return - add ret variable
	recAddBlock(cube, curNode);
	return 1;
    }

    quadrant = findQuadrant(cube->x, cube->y, cube->z, curNode);
    
    // Case 1: curNode child is empty - allocate new node, add cube as leaf
    if (curNode->children[quadrant] == NULL) {
        if (!allocateNode(curNode, quadrant)) {
            return 0;
	}
	curNode->children[quadrant]->cube = cube;
	return 1;
    }

    // Case 2: curNode child is not empty 
    recAddBlock(cube, curNode->children[quadrant]);
    return 1;
}

int OCT_AddBlock(Cube_T *cube) {
    int ret; 
    
    // Case 0: Root is null - Initialize Tree
    if (root == NULL) {
         if (!initializeTree(cube)) {
             fprintf(stderr, "ERROR: Could not allocate block\n");
	     return 0;
	 }
	 return 1;
    }

    if (findCube(cube->x, cube->y, cube->z)) {
        fprintf(stderr, "ERROR: Cube already added to tree\n");
	return 0;
    }

    ret = recAddBlock(cube, root);    
    
    // If block has neighbors, turn mutual faces off from being rendered
    setFaces(cube);

    return ret;
}

void cleanUpOctree(void) { 
    // TODO Free all memory
    ;
}
