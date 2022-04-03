
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

static bool depthFirstDraw(windowContext *winParam, Node *node, Node *nodePx, Node *nodeMx, Node *nodePz, Node *nodeMz, Node *nodeT);
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
    char str[8];
    fgets(str, 8, fp);
    gMapSize = atoi(str);
    printf("Map size: %d\n", gMapSize);

    int x, y, z = 0;
    do {
        char c = fgetc(fp);
	if (feof(fp)) {
            break;
	}
        
	if (c == '\n') {
	    z = 0;
	    y += 1;
            printf("\n");
	    continue;
	} 
        
        if (y % gMapSize == 0 && y != 0) {
            x += 1;
	    z = 0;
	    y = 0;
	}
       	
	if (x == gMapSize || y == gMapSize || z == gMapSize) {
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

// Return true if node is surrounded.  Takes into account map edge edge case
static bool isSurround(Node *node, bool bPx, bool bMx, bool bPz, bool bMz) {
    if ((node->cube->x == 0 	    ? true : bMx) &&
      (node->cube->x == (gMapSize-1) ? true : bPx) &&
      (node->cube->z == 0 	    ? true : bMz) &&
      (node->cube->z == (gMapSize-1) ? true : bPz)) {
        return true;
    }
    return false;
}

static bool drawLeaf(windowContext *winParam, Node *node, Node *nodePx, Node *nodeMx, Node *nodePz, Node *nodeMz, Node *nodeT) {
    // If node is surrounded by cubes (not visible), don't draw, but the node still covers the ground
    if (nodeT && isSurround(node, nodePx?true:false, nodeMx?true:false, nodePz?true:false, nodeMz?true:false)) {
        return true;
    }
    
    // Node is visible, draw
    displayCube(winParam, node->cube);
    
    // If the node is at the edge, don't count NULL edge nodes
    bool boolMx = (nodeMx && nodeMx->cube) ? true : false;
    bool boolPx = (nodePx && nodePx->cube) ? true : false;
    bool boolMz = (nodeMz && nodeMz->cube) ? true : false;
    bool boolPz = (nodePz && nodePz->cube) ? true : false;
    
    if (isSurround(node, boolPx, boolMx, boolPz, boolMz)) {
	return true;
    }

    // This is a visible cube and there have to be cubes below it that are visible
    return false;
}

// Depending on the quadrant, find the correct neighbors of node to continue recursing down the tree to draw visible children
static bool passNeighbor(windowContext *winParam, int quadrant, Node *node, Node *nodePx, Node *nodeMx, Node *nodePz, Node *nodeMz, Node *nodeT) {
    switch (quadrant) {
        case 0:
	    return depthFirstDraw(winParam, node->children[0], node->children[2], nodeMx ? nodeMx->children[2] : NULL, 
				  node->children[1], nodeMz ? nodeMz->children[1] : NULL, nodeT ? nodeT->children[4] : NULL);
	case 1:
	    return depthFirstDraw(winParam, node->children[1], node->children[3], nodeMx ? nodeMx->children[3] : NULL,
	    			  nodePz ? nodePz->children[0] : NULL, node->children[0], nodeT ? nodeT->children[5] : NULL);
        case 2:
	    return depthFirstDraw(winParam, node->children[2], nodePx ? nodePx->children[0] : NULL, node->children[0],
	    			  node->children[3], nodeMz ? nodeMz->children[3] : NULL, nodeT ? nodeT->children[6] : NULL);
        case 3:
	    return depthFirstDraw(winParam, node->children[3], nodePx ? nodePx->children[1] : NULL, node->children[1],
	    			  nodePz ? nodePz->children[2] : NULL, node->children[2], nodeT ? nodeT->children[7] : NULL);
        case 4:
	    return depthFirstDraw(winParam, node->children[4], node->children[6], nodeMx ? nodeMx->children[6] : NULL,
	    			  node->children[5], nodeMz ? nodeMz->children[5] : NULL, node->children[0]);
        case 5:
	    return depthFirstDraw(winParam, node->children[5], node->children[7], nodeMx ? nodeMx->children[7] : NULL,
	    			  nodePz ? nodePz->children[4] : NULL, node->children[4], node->children[1]);
        case 6:
	    return depthFirstDraw(winParam, node->children[6], nodePx ? nodePx->children[4] : NULL, node->children[4],
	    			  node->children[7], nodeMz ? nodeMz->children[7] : NULL, node->children[2]);
        case 7:
	    return depthFirstDraw(winParam, node->children[7], nodePx ? nodePx->children[5] : NULL, node->children[5],
	    			  nodePz ? nodePz->children[6] : NULL, node->children[6], node->children[3]);
	default:
            LOG("passNeighbor unknown quadrant: %d", quadrant);
	    break;
    }
    LOG("passNeighbor has failed\n");
    return false;
}

/**
    Perform depth first search on the octree, passing the neighbors (positive x (Px), negative x (Nx)...) of each cube while recursing down the tree.
    
    Base case 0: node is NULL. 
    Base case 1: node is a leaf.  Draw the node if it is visible
    Recursive case: Starting from the highest y-value cube, continue depth first search until the cubes are no longer visible
    Merge up: If the cubes in the current node cover the entire ground, return True.  Else false and keep searching downwards.

*/
static bool depthFirstDraw(windowContext *winParam, Node *node, Node *nodePx, Node *nodeMx, Node *nodePz, Node *nodeMz, Node *nodeT) {
    // Base case 0: Node is NULL
    if (node == NULL) {
        return false;
    }
    // Base case 1: Node is a leaf
    if (node->leaf) {
	return drawLeaf(winParam, node, nodePx, nodeMx, nodePz, nodeMz, nodeT);
    }
    // Loop through the four columns of the octree (Indicies [0, 4], [1, 5], [2, 6], [3, 7])
    for (int i = 0; i < 4; i++) {
	// Find if the upper y-value node is visible
	if (node->children[i]) {
	    // Node above this node is not the surface
            if (!nodeT || !nodeT->children[i+4] || !nodeT->children[i+4]->surface) {
                node->children[i]->surface = passNeighbor(winParam, i, node, nodePx, nodeMx, nodePz, nodeMz, nodeT);
	    } else {
                node->children[i]->surface = true;
	    }
	}
	// Lower y-value node
	if (node->children[i+4]) {
	    if (node->children[i] == NULL || !node->children[i]->surface) {
	        // The top node to the lower y-value node is the upper y-value node in the same column
                node->children[i+4]->surface = passNeighbor(winParam, i+4, node, nodePx, nodeMx, nodePz, nodeMz, NULL);
	    } else {
                node->children[i+4]->surface = true;
	    }
	}
    }
    
    // Merge up: Do the cubes in this node cover the ground of the oct-block (child is not NULL and child surface is true for all four columns)
    if (((node->children[0] && node->children[0]->surface) || (node->children[4] && node->children[4]->surface)) && 
      ((node->children[1] && node->children[1]->surface) || (node->children[5] && node->children[5]->surface)) &&
      ((node->children[2] && node->children[2]->surface) || (node->children[6] && node->children[6]->surface)) && 
      ((node->children[3] && node->children[3]->surface) || (node->children[7] && node->children[7]->surface))) {
        return true;
    }
    // This oct-node does not have its surface completely full.  Return false to set surface
    return false;
}

void OCT_DrawMap(windowContext *winParam) {
    // Depth first search 
    if (root == NULL) {
        LOG("Root is null.  Cannot draw map");
    }
    // Root doesn't have any neighbors
    if (!depthFirstDraw(winParam, root, NULL, NULL, NULL, NULL, NULL)) {
        LOG("Root node doesn't have a completely filled in surface.  Map error");
    }
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
        
	// Newly created node is a leaf
        curNode->children[quadrant]->cube = curNode->cube;
	curNode->children[quadrant]->leaf = true;
	
	// curNode is now a internal node
	curNode->cube = NULL;
	curNode->leaf = false;
        
	// Now that the current block is split, call recursive again for case 1 or 2 
	return recAddBlock(cube, curNode);
    }

    quadrant = findQuadrant(cube->x, cube->y, cube->z, curNode);
    
    // Case 1: curNode child is empty - allocate new node, add cube as leaf
    if (curNode->children[quadrant] == NULL) {
        if (!allocateNode(curNode, quadrant)) {
            return 0;
	}
	curNode->children[quadrant]->cube = cube;
	curNode->children[quadrant]->leaf = true;
	return 1;
    }

    // Case 2: curNode child is not empty 
    return recAddBlock(cube, curNode->children[quadrant]);
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
    //setFaces(cube);

    return ret;
}

void cleanUpOctree(void) { 
    // TODO Free all memory
    ;
}
