

#include "initgl.h"
#include "cube.h"
#include "display.h"
#include "log.h"
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *root = NULL;

static int addBlock(Cube_T *cube);
static int initializeTree(Cube_T *cube);
static int findQuadrant(Cube_T *cube, Node *curNode);
static void calculateCenter(Node *child, Node *parent, int childQuadrant);
static int allocateNode(Node *parent, int quadrant);
static int recAddBlock(Cube_T *cube, Node *curNode);

static void depthFirstSearch(windowContext *winParam, Node *node) {
    // Base Case 0:  node is a leaf
    if (node->cube != NULL) {
	displayCube(winParam, node->cube);
	return;
    }

    for (int i = 0; i < 8; i++) {
        
	// Base Case 1: Child is NULL
	if (node->children[i] == NULL) {
            continue;
	}

	// Recusive case
	depthFirstSearch(winParam, node->children[i]);
    }
}

void OCT_DrawMap(windowContext *winParam) {
    // Depth first search 
    if (root == NULL) {
        LOG("Root is null.  Cannot draw map");
    }
    depthFirstSearch(winParam, root);
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
	    Cube_T *cube = CUBE_GenerateCube(x, y, z, GROUND); 
            addBlock(cube);
	}

	z += 1;
    } while (1);

    fclose(fp);
    
    return 1;
}

static int initializeTree(Cube_T *cube) {

    root = calloc(1, sizeof(Node));
    if (root == NULL) {
        fprintf(stderr, "Cannot allocate map\n");
        return 0;
    }
    root->layer = 0;
    root->width = MAPSIZE / 2;
    root->center[0] = MAPSIZE / 2;
    root->center[1] = MAPSIZE / 2;
    root->center[2] = MAPSIZE / 2;
    root->cube = cube;
    memset(root->children, 0, sizeof(root->children));
    
    return 1;
}

static int findQuadrant(Cube_T *cube, Node *curNode) {
    
    if (cube->x < curNode->center[0] && cube-> y < curNode->center[1] && cube->z < curNode->center[2]) {
        return 0;
    }
    if (cube->x < curNode->center[0] && cube-> y < curNode->center[1] && cube->z >= curNode->center[2]) {
        return 1;
    }
    if (cube->x < curNode->center[0] && cube-> y >= curNode->center[1] && cube->z < curNode->center[2]) {
        return 2;
    }
    if (cube->x < curNode->center[0] && cube-> y >= curNode->center[1] && cube->z >= curNode->center[2]) {
        return 3;
    }
    if (cube->x >= curNode->center[0] && cube-> y < curNode->center[1] && cube->z < curNode->center[2]) {
        return 4;
    }
    if (cube->x >= curNode->center[0] && cube-> y < curNode->center[1] && cube->z >= curNode->center[2]) {
        return 5;
    }
    if (cube->x >= curNode->center[0] && cube-> y >= curNode->center[1] && cube->z < curNode->center[2]) {
        return 6;
    }
    if (cube->x >= curNode->center[0] && cube->y >=curNode->center[1] && cube->z >= curNode->center[2]) {
        return 7;
    }

    fprintf(stderr, "ERROR: Could not find a quadrant for cube coords (%d, %d, %d)\n", 
     cube->x, cube->y, cube->z);
    return -1;
}

static void calculateCenter(Node *child, Node *parent, int childQuadrant) {
    int moveAmt = parent->width / 2;

    if (childQuadrant == 0) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 1) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 2) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 3) {
        child->center[0] = parent->center[0] - moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 4) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 5) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] - moveAmt;
	child->center[2] = parent->center[2] + moveAmt;
        return;
    }

    if (childQuadrant == 6) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
	child->center[2] = parent->center[2] - moveAmt;
        return;
    }

    if (childQuadrant == 7) {
        child->center[0] = parent->center[0] + moveAmt;
        child->center[1] = parent->center[1] + moveAmt;
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

static int recAddBlock(Cube_T *cube, Node *curNode) {
    int quadrant;

    // Case 0: curNode is leaf node - split node if possible
    if (curNode->cube != NULL) {
        // Move the cube in the current node to the children of current node
	quadrant = findQuadrant(curNode->cube, curNode);

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

    quadrant = findQuadrant(cube, curNode);
    
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

static int addBlock(Cube_T *cube) {

    // Case 0: Root is null - Initialize Tree
    if (root == NULL) {
         if (!initializeTree(cube)) {
             fprintf(stderr, "ERROR: Could not allocate block\n");
	     return 0;
	 }
	 return 1;
    }
    return recAddBlock(cube, root);    
}

void cleanUpOctree(void) { 
    // TODO Free all memory
    ;
}
