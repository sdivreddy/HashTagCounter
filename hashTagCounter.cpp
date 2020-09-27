/************************************************************************************************
* To find n most popular hash tags using max priority structure. Implementing using fibonacci
* heap and hash table. Connecting data using circular doubly linked list at each level.
*************************************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <regex> 
#include <sstream>
#include <map> 
#include <string>
#include <vector>

using namespace std;

// Node Structure for each node in doubly linkde list and in Max-Fibonacci Heap
class HashTagNode {
    public:
        int data;                           // Hash Tag count
        HashTagNode *leftSibling;           // Pointer to Left Sibling 
        HashTagNode *rightSibling;          // Pointer to Right Sibling
        HashTagNode *parentNode;            // Pointer to Parent Node 
        HashTagNode *childNode;             // Pointer to Child Node
        int degree;                         // Number of children
        bool cascadeCut;                    // Cascade Cut possible or not
        string name;                        // Hash Tag name
};

// To maintain Circular Doubly Linked List for Fibonacci Heap 
class CDLkdListFibHeap {
    public:
        // Creates new Hash Tag Node if not present and max pointer points exactly to maximum value node
        void createHastTagNode(HashTagNode** htMaxPtr, HashTagNode* newHTNode) {
            // If the list is empty
            if (*htMaxPtr == NULL) {                                                
                newHTNode->leftSibling = newHTNode->rightSibling = newHTNode;
                *htMaxPtr = newHTNode;                                              
            }
            // When new node data is more than max
            else if (newHTNode->data > (*htMaxPtr)->data) {                         
                HashTagNode* lastHTNode = (*htMaxPtr)->leftSibling;                 
                newHTNode->rightSibling = *htMaxPtr;                                
                newHTNode->leftSibling = lastHTNode;                                
                lastHTNode->rightSibling = (*htMaxPtr)->leftSibling = newHTNode;    
                *htMaxPtr = newHTNode;
            }
            // When new node data is less than max 
            else {
                HashTagNode* tempHTNode = (*htMaxPtr)->rightSibling;               
                while ((tempHTNode->data > newHTNode->data) && (tempHTNode != *htMaxPtr)) {
                    tempHTNode = tempHTNode->rightSibling;
                }
                tempHTNode->leftSibling->rightSibling = newHTNode;                
                newHTNode->leftSibling = tempHTNode->leftSibling;
                tempHTNode->leftSibling = newHTNode;
                newHTNode->rightSibling = tempHTNode;
            }
        }

        // Cascade Cut operation to be performed on fibonacci heap
        void cacadeCutHashTagNode(HashTagNode** htMaxPtr, HashTagNode* htcNode) {
            // Reduce degree of parent node and get parent pointer
            htcNode->parentNode->degree = htcNode->parentNode->degree - 1;
            HashTagNode* parent = htcNode->parentNode;

            // Set Parent and Siblings to NULL and insert this node at top level
            if (htcNode->parentNode->childNode == htcNode) {
                if (htcNode->parentNode->childNode->rightSibling != htcNode->parentNode->childNode) {
                    htcNode->parentNode->childNode = htcNode->rightSibling;
                }
                else {
                    htcNode->parentNode->childNode = NULL;
                }
            }
            htcNode->parentNode = NULL;
            htcNode->rightSibling->leftSibling = htcNode->leftSibling;
            htcNode->leftSibling->rightSibling = htcNode->rightSibling;
            htcNode->leftSibling = htcNode->rightSibling = htcNode;
            createHastTagNode(&(*htMaxPtr), htcNode);

            // Set cascade cut values for parent and perform further if needed
            if (!parent->cascadeCut)
                parent->cascadeCut = true;
            else if(parent->parentNode != NULL)
                cacadeCutHashTagNode(&(*htMaxPtr), parent);
        }

        // Increase Hash Tag Node value in heap to given value and set max pointer accordingly
        void increaseHashTagValue(HashTagNode** htMaxPtr, HashTagNode* htcNode, int incValue) {
            // Increase value of current node
            htcNode->data += incValue;                                           
            if (htcNode != *htMaxPtr) {
                // If parent is present and it's data is less than current data then should do cascade cut for same
                if (htcNode->parentNode != NULL) {
                    if (htcNode->parentNode->data < htcNode->data) {
                        cacadeCutHashTagNode(&(*htMaxPtr), htcNode);
                    }
                }
                else {  
                    htcNode->rightSibling->leftSibling = htcNode->leftSibling;
                    htcNode->leftSibling->rightSibling = htcNode->rightSibling;
                    createHastTagNode(&(*htMaxPtr), htcNode);
                }
            }
        }  

        // PairWise Combine the nodes with same degree and return parent node
        HashTagNode* combinePairWise(HashTagNode* alreadyNode, HashTagNode* newNode) {
            // New node as child to already node and insert in appropriate position
            if (alreadyNode->data >= newNode->data) {
                newNode->parentNode = alreadyNode;
                if (alreadyNode->degree > 0) {
                    // New node will be current child and already node's child will be sibling
                    if (newNode->data > alreadyNode->childNode->data) {
                        HashTagNode* lastHTNode = alreadyNode->childNode->leftSibling;
                        newNode->rightSibling = alreadyNode->childNode;
                        newNode->leftSibling = lastHTNode;
                        lastHTNode->rightSibling = alreadyNode->childNode->leftSibling = newNode;    
                        alreadyNode->childNode = newNode;
                    }
                    // New node will be siblings to already node's child
                    else {
                        HashTagNode* tempHTNode = alreadyNode->childNode->rightSibling;
                        while ((tempHTNode->data > newNode->data) && (tempHTNode != alreadyNode->childNode)) {
                            tempHTNode = tempHTNode->rightSibling;
                        }
                        tempHTNode->leftSibling->rightSibling = newNode;
                        newNode->leftSibling = tempHTNode->leftSibling;
                        tempHTNode->leftSibling = newNode;
                        newNode->rightSibling = tempHTNode;
                    }
                }
                else {
                    newNode->leftSibling = newNode->rightSibling = newNode;
                    alreadyNode->childNode = newNode;                    
                }
                // Increase degree of parent by 1 and return
                alreadyNode->degree = alreadyNode->degree+1;
                return alreadyNode;
            }
            // Already node as child to new node and insert in appropriate position
            else {
                alreadyNode->parentNode = newNode;
                if (newNode->degree > 0) {
                    // Already node will be current child and new node's child will be sibling
                    if (alreadyNode->data > newNode->childNode->data) {
                        HashTagNode* lastHTNode = newNode->childNode->leftSibling;
                        alreadyNode->rightSibling = newNode->childNode;
                        alreadyNode->leftSibling = lastHTNode;
                        lastHTNode->rightSibling = newNode->childNode->leftSibling = alreadyNode;
                        newNode->childNode = alreadyNode;
                    }
                    // ALready node will be siblings to new node's child
                    else {
                        HashTagNode* tempHTNode = newNode->childNode->rightSibling;
                        while ((tempHTNode->data > alreadyNode->data) && (tempHTNode != newNode->childNode)) {
                            tempHTNode = tempHTNode->rightSibling;
                        }
                        tempHTNode->leftSibling->rightSibling = alreadyNode;
                        alreadyNode->leftSibling = tempHTNode->leftSibling;
                        tempHTNode->leftSibling = alreadyNode;
                        alreadyNode->rightSibling = tempHTNode;
                    }
                }
                else {
                    alreadyNode->leftSibling = alreadyNode->rightSibling = alreadyNode;                    
                    newNode->childNode = alreadyNode;
                }
                // Increase degree of parent by 1 and return
                newNode->degree = newNode->degree+1;
                return newNode;
            }
        }

        // Extract and return top n hashtags and pairwise combine is done for each time
        HashTagNode* pickTopNHashTags(HashTagNode** htMaxPtr) {
            // Get temporary pointer for pointing to Max pointer
            HashTagNode* tempMaxPtr = *htMaxPtr;

            while (tempMaxPtr->degree > 0) {
                HashTagNode* tempChildNodePtr = tempMaxPtr->childNode;
                tempMaxPtr->childNode = tempChildNodePtr->rightSibling;
                tempChildNodePtr->parentNode = NULL;
                tempChildNodePtr->rightSibling->leftSibling = tempChildNodePtr->leftSibling;
                tempChildNodePtr->leftSibling->rightSibling = tempChildNodePtr->rightSibling;
                tempChildNodePtr->leftSibling = tempChildNodePtr->rightSibling = tempChildNodePtr;
                createHastTagNode(&(*htMaxPtr), tempChildNodePtr);
                tempMaxPtr->degree = tempMaxPtr->degree - 1;
            }
            // Removed all children and are re-inserted
            tempMaxPtr->childNode = NULL;

            if (tempMaxPtr->rightSibling != tempMaxPtr) {
                // Assigning temporary max pointer
                (*htMaxPtr)->rightSibling->leftSibling = (*htMaxPtr)->leftSibling;
                (*htMaxPtr)->leftSibling->rightSibling = (*htMaxPtr)->rightSibling;
                *htMaxPtr = tempMaxPtr->rightSibling;

                // Pairwise combine all hash tag nodes and insert extracted max pointer node based on degree
                map<int, HashTagNode*> htDegree;
                HashTagNode* movingPtrNode = *htMaxPtr;
                do {
                    int degree = movingPtrNode->degree;
                    HashTagNode* existingHTNode = movingPtrNode;
                    movingPtrNode = movingPtrNode->rightSibling;
                    existingHTNode->leftSibling = existingHTNode->rightSibling = existingHTNode;
                    
                    while(htDegree.find(degree) != htDegree.end()) {    
                        HashTagNode* alreadyHTNode = htDegree.at(degree);
                        existingHTNode = combinePairWise(alreadyHTNode, existingHTNode);
                        htDegree.erase(degree);
                        degree += 1;
                    }
                    htDegree.insert(make_pair(degree, existingHTNode)); 
                } while (movingPtrNode != *htMaxPtr);
                        
                // Recreate heap with all node objects present in degree table
                (*htMaxPtr) = NULL;
                map<int, HashTagNode*>::iterator itr;
                for (itr = htDegree.begin(); itr != htDegree.end(); itr++ ) {
                    createHastTagNode(&(*htMaxPtr), itr->second);
                }
            }
            else {
                (*htMaxPtr) = NULL;
            }
            return tempMaxPtr;
        }  
};

// Main Entry
int main(int argc, char** argv) {
   // Input and outfput File Handlers
    ifstream hashTagsFile;
    ofstream topNHashTagsFile;
    hashTagsFile.open(argv[1]);     // Open input file and read from it
    bool checkFirst = true;         // To maintain output file structure
    bool outputFile = false;        // To check for output file name

    if (argc > 2) {
        // Append top n hash tags to output file
        topNHashTagsFile.open(argv[2], ios_base::app);
        outputFile = true;
    }
    
    
    if (hashTagsFile.is_open()) {
        // Create object of HashTagNode                         
        HashTagNode *htc = new HashTagNode(); 
        // To map each hashTag and it's node in heap
        map<string, HashTagNode*> frequencies;                  
        // Object of CDLkdListFibHeap class 
        CDLkdListFibHeap *cdll = new CDLkdListFibHeap();            
        HashTagNode *start = NULL;                   
        string line;
        while (getline(hashTagsFile, line)) {  
            // Insert the hashTag and it's frequency in hash map and in fibonacci heap as required
            if (line[0] == '#') {
                vector<string> tokens;
                istringstream iss(line);
                for(std::string s; iss >> s; ) 
                    tokens.push_back(s);
                /* Check if hash tag is already present in hash map. If present call increaseHashTagValue() and 
                * update hash table. Else call createHashTagNode() and update hash table */ 
                if (frequencies.find(tokens[0]) == frequencies.end()) {
                    // Create new object for hash Tag and node for same in Fibonacci Heap & set to default values
                    HashTagNode* newHTNode = new HashTagNode();
                    newHTNode->data = stoi(tokens[1]);
                    newHTNode->degree = 0;
                    newHTNode->cascadeCut = false;
                    newHTNode->parentNode = NULL;
                    newHTNode->name = tokens[0];
                    newHTNode->childNode = NULL;
                    newHTNode->leftSibling = newHTNode->rightSibling = newHTNode;
                    cdll->createHastTagNode(&start, newHTNode);
                    // Insert pair of hash tag and it's object in hash map
                    frequencies.insert(make_pair(tokens[0], newHTNode)); 
                }
                else { 
                    // Increase hash tag value and set max pointer accordingly
                    cdll->increaseHashTagValue(&start, frequencies.at(tokens[0]), stoi(tokens[1]));
                }
            }
            // Get the top N hashtags from fibonacci heap and output it to a file
            else if (isdigit(line[0])) {
                HashTagNode* pickedHTNode;
                string topNHashTags;
                vector<HashTagNode*> removedNodes;
                
                int nTops = (stoi(line) > frequencies.size())? frequencies.size() : stoi(line);
                for (int i = 0; i < nTops; i++) {
                    pickedHTNode = cdll->pickTopNHashTags(&start);
                    removedNodes.push_back(pickedHTNode);
                    string hashTagKey = pickedHTNode->name.substr(1, pickedHTNode->name.length());
                    topNHashTags += hashTagKey + ',';
                }

                if (outputFile) {
                    if (!topNHashTags.empty()) {
                        topNHashTags.pop_back();
                    }
                    if (checkFirst == true) {
                        topNHashTagsFile << topNHashTags;
                        checkFirst = false;
                    }
                    else {
                        topNHashTagsFile << "\n" << topNHashTags;
                    }
                }
                else {
                    // Write output to stdout
                    if (!topNHashTags.empty()) {
                        topNHashTags.pop_back();
                    }
                    cout << topNHashTags << "\n";
                }
                
                // Re-Insert all picked top n Nodes
                for (int j = 0; j < removedNodes.size(); j++) {
                    cdll->createHastTagNode(&start, removedNodes[j]);
                }
            }
            else {
                // When "stop"/empty line is input, exit program
                break;
            }
        }
    }
    if (outputFile) {
        topNHashTagsFile.close();
    }
    hashTagsFile.close();       
    return 0;   
}
