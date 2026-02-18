#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_SIZE 100    
#define MAX_OPTIONS 8   


struct expElement{
    char expElm; 
    int type; // 1=operator, 2=operand, 3=unknown, 4=equal sign
    int value; // for operand or unknown operand
};

// Tree structure for building expression tree
struct TreeNode{
    int type;       // 1=Operator, 2=Operand, 3=Unknown-variable, 4=Unknown-operator
    int value;      // For operand
    char op;        // For operator  
    char unknownName;   // For unknown names (a,b,c) 
    struct TreeNode *left;
    struct TreeNode *right;
};
typedef struct TreeNode TreeNode;

// Basic checker functions - could probably combine these but keeping separate for readability
int isOperator(char x){
    if (x=='+' || x=='-' || x=='*' || x=='/' || x=='^'){
        return 1;    	
	}
    return 0;   
}

int isOperand(char x){ 
    // checking if it's a positive integer
    if(x>='0' && x<='9'){
        return 1;
    }
    return 0; 
}

int isUnknown(char x){
    //upper and lowercase letters can be an unknown
    if((x>='a'&& x<='z') || (x>='A' && x<='Z')){
        return 1;    
    }
    return 0; 
}

int isEqual(char x){
    if(x=='='){
		return 1; 
	} 
	return 0; 
}

// This function determines what type each element is
void determineTypes(struct expElement expArr[], int size){
    int i;
    
    for(i=0; i<size; i++){
        char currentChar = expArr[i].expElm;  // rename current element
        
        // Handle negative numbers 
        if(currentChar =='-' && (i + 1)<size && expArr[i+1].expElm>='0' && expArr[i+1].expElm<='9'){
            expArr[i].type = 2; // treat as operand
            expArr[i].value = -(expArr[i+1].expElm - '0'); // make it negative
            i++; // skip next element since we used it as value of negative number
            continue;
        }
        
        if(isOperator(currentChar)){
            expArr[i].type=1; // operator
        } 
        else if(isOperand(currentChar)){
            expArr[i].type=2; // operand
            expArr[i].value = currentChar-'0';  // convert char to int
        } 
        else if (isUnknown(currentChar)){
            expArr[i].type=3; // unknown variable
        } 
        else if (isEqual(currentChar)){
            expArr[i].type=4; // equals sign
        } 
        else{
            expArr[i].type=0; // unrecognized - probably shouldn't happen
        } 
    }
}

// performs basic math operations
int basicOperation(int x, char op, int y){ 
    int result = 0;  // initialize result to 0
    
    switch(op){ 
        case '+': 
            result = x+y; 
            break;
        case '-':
            result = x-y; 
            break;
        case '*': 
            result = x*y; 
            break;
        case '/': 
            if(y==0) {
                printf("Division by zero error\n");  //error message
                return 0;
            }
            result = x/y; 
            break;
        case '^': 
            result = (int)pow(x, y);  // casting to int
            break;
        default:
            printf("Unknown operator: %c\n", op);  // shouldn't happen but just in case
            break;
    }
    return result; 
}

// Simplify expression using consecutively operand operand operator order
void simplifyExp(struct expElement expArr[], int *size){
    int i,j;
    int leftOperand, rightOperand, result;
    char op;
    
    for (i = 0; i < *size; i++){
        if (expArr[i].type == 1){  // found an operator
            // check if we have two operands before this operator
            if (i >= 2 && expArr[i-1].type == 2 && expArr[i-2].type == 2){
                leftOperand = expArr[i-2].value; 
                rightOperand = expArr[i-1].value; 
                op= expArr[i].expElm; 
                
                result = basicOperation(leftOperand, op, rightOperand);
                
                // replace the three elements with just the result
                expArr[i-2].value = result; 
                expArr[i-2].type = 2; 
                
                // shift everything left to remove the used elements
                for (j = i-1; j+2 < *size; j++){
                    expArr[j] = expArr[j+2]; 
                } 
                *size = *size - 2; 
                i = i - 2;  
                
                if (i < 0){  // boundary check
                    i = -1;    
                } 
            }
        }
    }
}

// Create a tree node - basic tree operations
TreeNode* createNode(int nodeType, int val, char op, char name){
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if(newNode == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    
    newNode->type = nodeType;
    newNode->value = val;
    newNode->op = op;
    newNode->unknownName = name;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Check if operation is mathematically valid
int checkOp(int a, int b, char op, int *valid) {
    int result = 0;
    
    switch(op) {
        case '+': 
            result = a + b; 
            break;
        case '-': 
            result = a - b;
            break;
        case '*': 
            result = a * b;     
            break;
        case '/': 
            if (b == 0 || a % b != 0){  // no division by zero, only exact division
                *valid = 0; 
                return 0; 
            }
            result = a / b; 
            break;
        case '^': 
            if (b < 0 || b > 10) {  // power limitation
                *valid = 0; 
                return 0; 
            } 
            result = (int)pow(a, b); 
            break;
        default:
            *valid = 0;
            return 0;
    }
    return result;
}

// Recursively evaluate the expression tree
int computeTree(TreeNode* root, int *isValidPath) {
    if (!root || !(*isValidPath)) {
        return 0;
    }
    
    // just have 1 element in tree
    if (root->left == NULL && root->right == NULL){
        return root->value;
    } 
    
    // evaluate left and right subtrees
    int leftValue = computeTree(root->left, isValidPath);
    int rightValue = computeTree(root->right, isValidPath);

    if (!(*isValidPath)) {
        return 0;
    }
    
    // perform operation on the results
    int result = checkOp(leftValue, rightValue, root->op, isValidPath);
    return result;
}

// Try all possible values for unknowns
void solveTree(TreeNode* root, TreeNode* unknownArr[], int count, int index) {
    if (index == count){
        int valid = 1;
        int result = computeTree(root, &valid);
        
        // if computation is valid and equals zero, we found a solution
        if (valid && result == 0) {
            printf("(");
            for(int i=0; i<count; i++) {
                if(unknownArr[i]->type == 2)
                    printf("%d", unknownArr[i]->value);
                else
                    printf("%c", unknownArr[i]->op);
                
                if (i < count - 1) printf(", ");
            }
            printf(")\n");
        }
        return;
    }

    TreeNode* current = unknownArr[index];

    if (current->type == 4) { // Unknown OPERATOR
        char operators[] = {'+', '-', '*', '/'}; // try these operators
        for(int i=0; i<4; i++) {
            current->op = operators[i];
            current->type = 1; 
            solveTree(root, unknownArr, count, index + 1);
            current->type = 4; // reset for next iteration
        }
    } 
    else if (current->type == 3) { // Unknown OPERAND  
        // try numbers 1 to 100 - might be overkill but covers most cases
        for(int val=1; val<=100; val++) { 
            current->value = val;
            current->type = 2; 
            solveTree(root, unknownArr, count, index + 1);
            current->type = 3; // reset for next iteration
        }
    }
}

// Build tree from postfix expression and try different combinations
void tryCombination(struct expElement expArr[], int size, char unknownArr[], int unknownCount, int currentMap[]) {
    TreeNode* nodeStack[100];  // stack for building tree
    int stackTop = -1;
    TreeNode* unknownNodes[10];
    int unkNodeCount = 0;
    int i,j;
    
    // build the expression tree using stack
    for(i=0; i<size; i++) {
        if(expArr[i].type == 4) break;  // stop at equals sign
        
        TreeNode* node = NULL;
        int isOperator = (expArr[i].type == 1);
        int isUnknownVariable = (expArr[i].type == 3);
        
        if (isUnknownVariable) {
            // figure out if this unknown is mapped as operator or operand
            for(j=0; j<unknownCount; j++) {
                if(unknownArr[j] == expArr[i].expElm){
                    isOperator = currentMap[j]; 
                    break;
                } 
            }
            
            if(isOperator) {
                node = createNode(4, 0, '?', expArr[i].expElm); 
            } else {
                node = createNode(3, 0, '?', expArr[i].expElm); 
            }    
            unknownNodes[unkNodeCount++] = node;
        } 
        else if(expArr[i].type == 2) {
            node = createNode(2, expArr[i].value, 0, 0);
        } 
        else if(expArr[i].type == 1) {
            node = createNode(1, 0, expArr[i].expElm, 0);
        } 

        if(isOperator) { 
            // operators need two operands
            node->right = nodeStack[stackTop--];
            node->left = nodeStack[stackTop--];
            nodeStack[++stackTop] = node;
        } else {
            nodeStack[++stackTop] = node;
        }
    }
    
    TreeNode* root = nodeStack[0];
    solveTree(root, unknownNodes, unkNodeCount, 0);
}

// Find all unique unknown variables in the expression
void findOptions(struct expElement expArr[], int size, char variableArr[], int *unknownCount) {
    *unknownCount = 0;
    int i,j;
    
    for(i=0; i<size; i++){
        if(expArr[i].type == 3){ 
            int alreadyExists = 0;
            
            // check if we've already seen this variable
            for(j=0; j<*unknownCount; j++){
                if(variableArr[j] == expArr[i].expElm){
                    alreadyExists = 1;
                    break;
                } 
            } 
            
            if(!alreadyExists){
                variableArr[(*unknownCount)++] = expArr[i].expElm;    
            } 
        }
    }
}

// Main analysis function - tries all possible combinations
void analyzeCombinations(struct expElement expArr[], int size, char variableArr[], int unknownCount) {
    int totalCombinations = (int)pow(2, unknownCount);  // 2^n combinations
    int i,j,k,m;
    
    // Try every possible binary combination
    for(i=0; i < totalCombinations; i++){
        int currentCombination[MAX_OPTIONS]; 
        
        // convert number to binary representation
        for(j=0; j < unknownCount; j++){
            currentCombination[j] = (i >> j) & 1;
        } 

        // Check if this combination gives us valid postfix expression
        int stackHeight = 0;
        int isValidExpression = 1;
        
        for(k=0; k<size; k++){
            if(expArr[k].type == 4) break;  // stop at equals sign
            
            int isOperator = (expArr[k].type == 1);
            
            if(expArr[k].type == 3) {
                // look up what this unknown is mapped to
                for(m=0; m<unknownCount; m++){
                    if(variableArr[m] == expArr[k].expElm){
                        isOperator = currentCombination[m]; 
                        break;
                    }
                }       
            }
            
            if(isOperator){
                if(stackHeight < 2){  // operators need 2 operands
                    isValidExpression = 0; 
                    break; 
                }
                stackHeight--;
            } else {
                stackHeight++;
            }
        }
        
        // valid postfix should end with exactly one element on stack
        if(isValidExpression && stackHeight != 1) {
             isValidExpression = 0;
        }

        if(isValidExpression){
            tryCombination(expArr, size, variableArr, unknownCount, currentCombination);
        }
    }
}

int main(){
    // The input expression in postfix notation
    char input[] = "3 1 + 2 3 2 ^ a 2 / 4 3 - b - c * 2 4 1 + ^ 3 4 * 2 2 * - / 6 2 * - + = 0";
    struct expElement exprArr[MAX_SIZE];
    int size = 0;
    int i = 0;
    
    // Parse the input string into array elements
    for(i=0; input[i] != '\0'; i++){
        if(input[i] == ' ') continue;  // skip spaces
        
        exprArr[size].expElm = input[i];
        exprArr[size].type = 0;  // will be determined later
        size++;
    }
    
    // Show original expression
    printf("Original Expression:\n");       
    for (int i = 0; i < size; i++) {
        if (exprArr[i].type == 2) {
            printf("%d ", exprArr[i].value);
        } else {
            printf("%c ", exprArr[i].expElm);
        } 
    }
    printf("\n\n"); 

    // Process the expression
    determineTypes(exprArr, size);
    simplifyExp(exprArr, &size);
    
    // Show simplified version
    printf("Simplified Expression:\n");     
    for (i = 0; i < size; i++) {
        if (exprArr[i].type == 2) {
            printf("%d ", exprArr[i].value);
        } else {
            printf("%c ", exprArr[i].expElm);
        }
    }
    printf("\n\n");
    
    char unknownVariables[MAX_OPTIONS];
    int unknownCount = 0;
    
    printf("Possible value combinations for (a, b, c)\n");

    findOptions(exprArr, size, unknownVariables, &unknownCount);
    analyzeCombinations(exprArr, size, unknownVariables, unknownCount);

    return 0;
}
