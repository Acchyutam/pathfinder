#include <iostream>
#include<vector>
#include <fstream>
#include <iomanip> // Include the <iomanip> header for formatting
#include <cmath>

using namespace std;

#define MAX 15 // Maximum number of children a node can have
#define MIN 8 // Minimum number of children a node can have
#define RESET   "\033[0m" // Color code reset to default
#define GREEN   "\033[32m" // Color code set to Green
#define RED     "\033[31m" // Color code set to red
// MazeNodes of the maze
struct MazeNode{
    int weight;
    vector<MazeNode*> neighbours;
    int shortestDistance;
    MazeNode *prevMazeNode;
};


struct BTreeNode{
    MazeNode *keys[MAX + 1];         
    int count;                     
    BTreeNode *children[MAX + 1];  
};

BTreeNode *root = nullptr;

BTreeNode *createNewNode(MazeNode* key, BTreeNode *child){

    BTreeNode *newNode = new BTreeNode;
    newNode->keys[1] = key;
    newNode->count = 1;
    newNode->children[0] = root;
    newNode->children[1] = child;
    return newNode;
}

void insertKeyIntoNode(MazeNode* key, int position, BTreeNode *node, BTreeNode *child) {
    int i;
    for ( i = node->count; i > position; i-- ) {
        node->keys[i + 1] = node->keys[i];
        node->children[i + 1] = node->children[i];
    }

    node->keys[i + 1] = key;
    node->children[i + 1] = child;
    node->count++;
}

void splitNode(MazeNode *key, MazeNode **prevKey, int position, BTreeNode *node, BTreeNode *child, BTreeNode **newNode) {
    int splitPoint;
    int median = MIN;

    if (position > MIN)
        median = MIN + 1;

    splitPoint = median + 1;

    *newNode = new BTreeNode;

    while (splitPoint <= MAX) {
        (*newNode)->keys[splitPoint - median]     = node->keys[splitPoint];
        (*newNode)->children[splitPoint - median] = node->children[splitPoint];
        splitPoint++;
    }

    node->count = median;
    (*newNode)->count = MAX - median;

    if (position <= MIN) {
        insertKeyIntoNode(key, position, node, child);
    }

    else {
        insertKeyIntoNode(key, position - median, *newNode, child);
    }

    *prevKey = node->keys[node->count];
    (*newNode)->children[0] = node->children[node->count];

    node->count--;
}

bool AddNewKey(MazeNode *newKey, MazeNode **prevValue, BTreeNode *node, BTreeNode **child) {

    int position;

    if(!node) {
        *prevValue = newKey;
        *child = NULL;
        return true;
    }

    if( newKey->shortestDistance < node->keys[1]->shortestDistance ) {
        position = 0;
    }

    else {
        position = node->count;
        while(newKey->shortestDistance < node->keys[position]->shortestDistance && position > 1)
            position--;
    }

    if ( AddNewKey(newKey, prevValue, node->children[position], child) ) {

        if (node->count < MAX) {
            insertKeyIntoNode(*prevValue, position, node, *child);
        }

        else {
            splitNode(*prevValue, prevValue, position, node, *child, child);
            return true;
        }
    }

    return false;
}

void insert(MazeNode *newMazeNode) {

    bool newRoot;
    MazeNode *i;
    BTreeNode *child;

    newRoot = AddNewKey(newMazeNode, &i, root, &child);

    if(newRoot)
        root = createNewNode(i, child);
}

void removeKey(BTreeNode *node, int position) {

    for(int i= position + 1; i <= node->count; i++){
        node->keys[i - 1]     = node->keys[i];
        node->children[i - 1] = node->children[i];
    }
    node->count--;
}

void shiftKeyToChild(BTreeNode *node, int position, char leftRight) {
    if(leftRight == 'r'){
        BTreeNode *x = node->children[position];
        int j = x->count;

        while (j > 0) {
            x->keys[j + 1]     = x->keys[j];
            x->children[j + 1] = x->children[j];
        }

        x->keys[1] = node->keys[position];
        x->children[1] = x->children[0];
        x->count++;

        x = node->children[position - 1];
        node->keys[position]     = x->keys[x->count];
        node->children[position] = x->children[x->count];
        x->count--;

        return;
    }

    if(leftRight == 'l'){
        int j = 1;
        BTreeNode *x = node->children[position - 1];

        x->count++;
        x->keys[x->count]     = node->keys[position];
        x->children[x->count] = node->children[position]->children[0];

        x = node->children[position];
        node->keys[position] = x->keys[1];
        x->children[0]         = x->children[1];
        x->count--;

        for(j=1; j <= x->count; j++){
            x->keys[j]     = x->keys[j + 1];
            x->children[j] = x->children[j + 1];
        }

        return;
    }
}

void mergeNodes(BTreeNode *node, int position) {

    int j;

    BTreeNode *rightNode = node->children[position];
    BTreeNode *leftNode  = node->children[position - 1];

    leftNode->count++;
    leftNode->keys[leftNode->count]     = node->keys[position];
    leftNode->children[leftNode->count] = node->children[0];

    for(j=1; j <= rightNode->count; j++){
        leftNode->count++;
        leftNode->keys[leftNode->count]     = rightNode->keys[j];
        leftNode->children[leftNode->count] = rightNode->children[j];
    }

    for(j=position; j < node->count; j++){
        node->keys[j]     = node->keys[j + 1];
        node->children[j] = node->children[j + 1];
    }

    node->count--;
    free(rightNode);
}

void fixNode(BTreeNode *node, int position) {

    if (!position) {
        if (node->children[1]->count > MIN) {

            shiftKeyToChild(node, position, 'l');
        }
        else {
            mergeNodes(node, 1);
        }
    }

    else {
        if (node->count != position) {
            if (node->children[position - 1]->count > MIN) {
                shiftKeyToChild(node, position, 'r');
            }

            else {
                if (node->children[position + 1]->count > MIN) {
                    shiftKeyToChild(node, position, 'l');
                }

                else {
                    mergeNodes(node, position);
                }
            }
        }

        else {
            if (node->children[position - 1]->count > MIN)
                shiftKeyToChild(node, position, 'r');
            else
                mergeNodes(node, position);
        }
    }
}

void replaceSuccessor(BTreeNode *deadNode, int position) {

    BTreeNode *dummy = deadNode->children[position];

    while(dummy->children[0] != nullptr)
        dummy = dummy->children[0];

    deadNode->keys[position] = dummy->keys[1];

}

bool removeKey(MazeNode *deadMazeNode,BTreeNode *node) {

    bool flag = false;
    int position;

    if (node) { 

        if (deadMazeNode->shortestDistance < node->keys[1]->shortestDistance) {
            position = 0;
            flag = false;
        }

        else {
            position = node->count;
            while((deadMazeNode->shortestDistance  <= node->keys[position]->shortestDistance) &&
                (deadMazeNode  != node->keys[position])  &&
                (position > 1))

                 position--;

            if ((deadMazeNode  == node->keys[position]) &&
                (deadMazeNode->shortestDistance  == node->keys[position]->shortestDistance)) {
                flag = true;
            }
            else {
                flag = false;
            }
        }

        if (flag) {
            if (node->children[position - 1]) {
                replaceSuccessor(node, position);
                flag = removeKey(node->keys[position], node->children[position]);
            }
            else {
                removeKey(node, position);
            }
        }

        else {
            flag = removeKey(deadMazeNode, node->children[position]);
        }

        if (node->children[position] &&
           (node->children[position]->count < MIN)) {
            fixNode(node, position);
        }
    }

    return flag;
}

void remove(MazeNode *deadMazeNode, BTreeNode *node) {

    BTreeNode *temp;

    if(!removeKey(deadMazeNode, node))
        return;

    if( node->count == 0 ){
            temp = node;
            node = node->children[0];
            free(temp);
    }

    root = node;
}
BTreeNode *removeAndGetNewRoot(MazeNode *deadMazeNode, BTreeNode *node) {

    BTreeNode *temp;

    if (!removeKey(deadMazeNode, node))
        return node;

    if (node->count == 0) {
        temp = node;
        node = node->children[0];
        free(temp);
    }

    return node;
}

MazeNode *popMin(BTreeNode *&root) {
    MazeNode *min;
    BTreeNode *ptr = root;

    while (ptr->children[0] != nullptr) {
        ptr = ptr->children[0];
    }

    min = ptr->keys[1];

    root = removeAndGetNewRoot(min, root);
    return min;
}


bool isEmpty(BTreeNode *root) {
    if(root == nullptr)
        return true;
    else
        return false;
}

void setNeighbours(vector<vector<MazeNode*>> &maze, int row, int col) {
    int weight;
    vector<MazeNode*> neighbours;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            weight = maze[i][j]->weight;

            if (weight > 0) { 

                if ((j - 1 >= 0) && (maze[i][j - 1]->weight > 0)) {
                    neighbours.push_back(maze[i][j - 1]); // left neighbour
                }

                if ((j + 1 < col) && (maze[i][j + 1]->weight > 0)) {
                    neighbours.push_back(maze[i][j + 1]); // right neighbour
                }

                if ((i + 1 < row) && (maze[i + 1][j]->weight > 0)) {
                    neighbours.push_back(maze[i + 1][j]); // down neighbour
                }

                maze[i][j]->neighbours = neighbours;
                neighbours.clear();
            }
        }
    }
}


void bTreeDijkstra(vector<vector<MazeNode*>> &maze, MazeNode *source, int row, int col){

    MazeNode *currentMazeNode = nullptr;
    int newDist = 0;

    source->shortestDistance = 0;

    insert(source);

    while( !isEmpty(root) ){
        currentMazeNode = popMin(root);
        

        for( auto neighbour : currentMazeNode->neighbours ){
            newDist = currentMazeNode->shortestDistance + neighbour->weight;

            if( newDist < neighbour->shortestDistance ){
                remove(neighbour, root);
                neighbour->shortestDistance = newDist;
                neighbour->prevMazeNode = currentMazeNode;
                insert(neighbour);
                }
            }
        }
    }

void solveMaze(vector<vector<MazeNode*>> &maze, MazeNode *source, int row, int col) {
    int steps = 0 ;
    bTreeDijkstra(maze, source, row, col);

    int i = row-1;
    int j = col-1 ;
    MazeNode *goal;

    while( maze[i][j]->weight==0 ){
        j--;
    }

    goal = maze[i][j];

    MazeNode *ptr = goal;

    while( ptr->prevMazeNode != nullptr ){
        ptr->weight = -1;
        ptr = ptr->prevMazeNode;
        steps++;
    }

    source->weight = -1;
    cout << GREEN << "\n\n\t\t\tPath cost: " << steps << " steps" << RESET << endl;
}

void printInputMaze(vector<vector<MazeNode*>> &maze, int row, int col) {
    // Find the maximum number of digits in the maze values
    int maxDigits = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int weight = maze[i][j]->weight;
            int digits = 1;
            if (weight != -1) {
                // Calculate the number of digits in the weight
                digits = (weight == 0) ? 2 : (int)log10(abs(weight)) + 1;
            }
            if (digits > maxDigits) {
                maxDigits = digits;
            }
        }
    }

    // Print the input maze with symmetrical formatting
    for (int i = 0; i < row; i++) {
        cout << "\t|";
        for (int j = 0; j < col; j++) {
            int weight = maze[i][j]->weight;
            // Pad the output with spaces to match the maxDigits
            if (weight == 0) {
                cout << RED << setw(maxDigits) << weight << " " << RESET;
            } else {
                cout << setw(maxDigits) << weight << " ";
            }
        }
        cout << "|" << endl;
    }
}


void printSolvedMaze(vector<vector<MazeNode*>> &maze, int row, int col) {
    // Find the maximum number of digits in the maze values
    int maxDigits = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int weight = maze[i][j]->weight;
            int digits = 1;
            if (weight != -1) {
                // Calculate the number of digits in the weight
                digits = (weight == 0) ? 2 : (int)log10(abs(weight)) + 1;
            }
            if (digits > maxDigits) {
                maxDigits = digits;
            }
        }
    }

    // Print the maze with symmetrical formatting
    for (int i = 0; i < row; i++) {
        cout << "\t|";
        for (int j = 0; j < col; j++) {
            int weight = maze[i][j]->weight;
            // Pad the output with spaces to match the maxDigits
            if (weight == -1) {
                cout << GREEN << setw(maxDigits) << weight << " " << RESET;
            } else if (weight == 0) {
                cout << RED << setw(maxDigits) << weight << " " << RESET;
            } else {
                cout << setw(maxDigits) << weight << " ";
            }
        }
        cout << "|" << endl;
    }
}





// Function to take input for the maze
void takeInput(vector<vector<MazeNode*>>& maze, MazeNode*& source, int& row, int& col, ifstream& inputFile) {
    inputFile >> row;
    inputFile >> col;

    vector<MazeNode*> tempRow;

    // Inputs the plain maze into a 2D vector of pointers to MazeNodes
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            int weight;
            inputFile >> weight;

            MazeNode* newMazeNode = new MazeNode;
            newMazeNode->weight = weight;
            newMazeNode->prevMazeNode = nullptr;
            newMazeNode->shortestDistance = 100000000;
            newMazeNode->neighbours = {};
            tempRow.push_back(newMazeNode);
        }
        maze.push_back(tempRow);
        tempRow.clear();
    }

    // Finding the source (aka starting) MazeNode
    for (int i = 0; i < col; i++) {
        if (maze[0][i]->weight == 1) {
            source = maze[0][i];
            break;
        }
    }
}


int main() {
    int col, row;
    MazeNode* source;
    
    vector<vector<MazeNode*>> maze;
    
    // Create an input stream for reading from a text file
    ifstream inputFile("maze.txt"); // Replace "maze.txt" with your file path
    
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open the input file." << endl;
        return 1; // Exit the program with an error code
    }
    
    // Read input from the file
    takeInput(maze, source, row, col, inputFile);
    
    //Printing input maze
    cout << GREEN << "\n\n\t\t\tInput Maze:" << RESET << endl;
    printInputMaze(maze, row, col);

    // Find the neighbors for each node
    setNeighbours(maze, row, col);
    
    // Solve the maze using Dijkstra's algorithm
    solveMaze(maze, source, row, col);
    
    cout << GREEN << "\t\t-------------------------" << endl;
    cout << "\t\t------|Maze Solved|------" << endl;
    cout << "\t\t-------------------------" << RESET << endl;
    
    // Print the solved maze
    printSolvedMaze(maze, row, col);
    // Close the input file stream
    inputFile.close();
    
    return 0;
}

