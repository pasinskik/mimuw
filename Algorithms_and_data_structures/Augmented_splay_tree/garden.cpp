/**
 * This algorithm is a solution to the Garden problem.
 *
 * The program stores a dynamic sequence in an augmented splay tree.
 * It reads from the file and performs two action - adding a value
 * to all elements from a continuous interval and finding max non-decreasing
 * consecutive subsequence in a continuous interval.
 *
 * Time complexity - O(m * log(n))
 *
 * Author: Kacper Pasinski
 * Date: 04.01.2025
*/

#include <iostream>
using namespace std;

/* Structure representing a node of the augmented splay tree. */

struct Node {
    /* Attributes only applicable to this node. */
    int key;
    Node* left;
    Node* right;

    /* Attributes applicable to the subtree that has this node as the root. */
    int subtreeSize;  // Size of the subtree.
    int addToSubtree; // This number is to be added to all keys in the subtree (lazy propagation).
    int firstNumInSubtree; // First element (maximally left) of the subsequence given by the subtree.
    int lastNumInSubtree; // Last element (maximally right) of the subsequence given by the subtree.
    int maxNonDecPrefInSubtree; // Maximum non-decreasing subsequence that includes the first number in the subtree.
    int maxNonDecSufInSubtree; // Maximum non-decreasing subsequence that includes the last number in the subtree.
    int maxNonDecSubSeqInSubtree; // Maximum non-decreasing subsequence in the subtree.

    Node(int value)
        : key(value),
          subtreeSize(1),
          addToSubtree(0),
          firstNumInSubtree(value),
          lastNumInSubtree(value),
          maxNonDecPrefInSubtree(1),
          maxNonDecSufInSubtree(1),
          maxNonDecSubSeqInSubtree(1),
          left(nullptr),
          right(nullptr) {}
};

/* Class representing the augmented splay tree. */

class SplayTree {
private:
    Node* treeRoot;
    int treeSize;

    /* Lazy propagation of the addToSubtree attribute to the children of the node. */
    void propagate(Node* node) {
        if (!node || node->addToSubtree == 0) return;

        node->key += node->addToSubtree;
        node->lastNumInSubtree += node->addToSubtree;
        node->firstNumInSubtree += node->addToSubtree;

        if (node->left) node->left->addToSubtree += node->addToSubtree;
        if (node->right) node->right->addToSubtree += node->addToSubtree;

        node->addToSubtree = 0;
    }

    /* Update subtree attributes. */
    void updateAttributes(Node* node) {
        if (!node) return;

        // Propagating the nodes so that key values are correct.
        propagate(node);
        propagate(node->right);
        propagate(node->left);

        node->subtreeSize = 1;
        if (node->left) node->subtreeSize += node->left->subtreeSize;
        if (node->right) node->subtreeSize += node->right->subtreeSize;

        node->lastNumInSubtree = node->right ? node->right->lastNumInSubtree : node->key;
        node->firstNumInSubtree = node->left ? node->left->firstNumInSubtree : node->key;

        // To calculate the length of max non-decreasing subsequences there's a need to check if
        // subsequence containing current node's key are non-decreasing, that's why the program calculates
        // the elements adjacent to the current node (if there are none, then we suppose that the
        // previous element is bigger than the current node key to indicate that there can't be a
        // non-decreasing subsequence using elements from the left subtree; and next element accordingly).
        int previousElement = node->left ? node->left->lastNumInSubtree : node->key + 1;
        int nextElement = node->right ? node->right->firstNumInSubtree : node->key - 1;

        node->maxNonDecSufInSubtree = node->right ? node->right->maxNonDecSufInSubtree : 0;
        if (!node->right || (node->right->subtreeSize == node->right->maxNonDecSufInSubtree && node->key <= nextElement)) {
            node->maxNonDecSufInSubtree++; // Max suffix contains the current node.

            if (previousElement <= node->key)
                node->maxNonDecSufInSubtree += node->left->maxNonDecSufInSubtree; // Max suffix contains the suffix of the left node.
        }

        node->maxNonDecPrefInSubtree = node->left ? node->left->maxNonDecPrefInSubtree : 0;
        if (!node->left || (node->left->subtreeSize == node->left->maxNonDecPrefInSubtree && node->key >= previousElement)) {
            node->maxNonDecPrefInSubtree++; // Max prefix contains the current node.
            
            if (nextElement >= node->key)
                node->maxNonDecPrefInSubtree += node->right->maxNonDecPrefInSubtree; // Max prefix contains the prefix of the right node.
        }

        // Maximum non-decreasing subsequence in the subtree could be its max non-decreasing suffix, prefix
        // or be entirely situated in the left or right subtree.
        node->maxNonDecSubSeqInSubtree = max(node->maxNonDecSufInSubtree, node->maxNonDecPrefInSubtree);
        if (node->left)
            node->maxNonDecSubSeqInSubtree = max(node->maxNonDecSubSeqInSubtree, node->left->maxNonDecSubSeqInSubtree);
        if (node->right)
            node->maxNonDecSubSeqInSubtree = max(node->maxNonDecSubSeqInSubtree, node->right->maxNonDecSubSeqInSubtree);

        // We shall also calculate the length of max non-decreasing subsequence that is neither a prefix nor a suffix
        // but contains the current node's key because it could be the max non-decreasing subsequence in general.
        int tempSequence = 1;
        if (nextElement >= node->key)
            tempSequence += node->right->maxNonDecPrefInSubtree;
        if (previousElement <= node->key)
            tempSequence += node->left->maxNonDecSufInSubtree;

        node->maxNonDecSubSeqInSubtree = max(node->maxNonDecSubSeqInSubtree, tempSequence);
    }

    /* Single right rotation in splay tree. */
    Node* rotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        updateAttributes(y);
        updateAttributes(x);
        return x;
    }

    /* Single left rotation in splay tree. */
    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        updateAttributes(x);
        updateAttributes(y);
        return y;
    }

    /* Splay function */
    Node* splay(Node* node, int index, bool isRoot) {
        if (!node) return nullptr;

        propagate(node);

        int leftSubtreeSize = node->left ? node->left->subtreeSize : 0;

        // Separate code if the node is the root of the splay tree at the time.
        // This is basically the lecture slides rewritten into C++.
        if (isRoot) {
            if (index <= leftSubtreeSize) {
                if (!node->left) return node;
                propagate(node->left);

                node->left = splay(node->left, index, false);
                node = rotateRight(node);
            }
            if (index > leftSubtreeSize + 1) {
                if (!node->right) return node;
                propagate(node->right);

                node->right = splay(node->right, index - leftSubtreeSize - 1, false);
                node = rotateLeft(node);
            }
            return node;
        }

        // The code for non-roots.
        if (index <= leftSubtreeSize) {
            if (!node->left) return node;
            propagate(node->left);

            int leftOfLeftSubtreeSize = node->left->left ? node->left->left->subtreeSize : 0;

            if (index <= leftOfLeftSubtreeSize) {
                node->left->left = splay(node->left->left, index, false);
                node = rotateRight(node);
            }
            if (index > leftOfLeftSubtreeSize + 1) {
                node->left->right = splay(node->left->right, index - leftOfLeftSubtreeSize - 1, false);
                if (node->left->right)
                    node->left = rotateLeft(node->left);
            }
            return (node->left) ? rotateRight(node) : node;
        }
        if (index > leftSubtreeSize + 1) {
            if (!node->right) return node;
            propagate(node->right);

            int leftOfRightSubtreeSize = node->right->left ? node->right->left->subtreeSize : 0;

            if (index <= leftSubtreeSize + leftOfRightSubtreeSize + 1) {
                node->right->left = splay(node->right->left, index - leftSubtreeSize - 1, false);
                if (node->right->left)
                    node->right = rotateRight(node->right);
            }
            if (index > leftSubtreeSize + leftOfRightSubtreeSize + 2) {
                node->right->right = splay(node->right->right, index - leftSubtreeSize - leftOfRightSubtreeSize - 2, false);
                node = rotateLeft(node);
            }
            return (node->right) ? rotateLeft(node) : node;
        }
        return node;
    }

    /* Insert value at the k-th position. */
    void insert(int value, int k) {
        ++treeSize;
        if (!treeRoot) {
            treeRoot = new Node(value);
            return;
        }

        treeRoot = splay(treeRoot, k, true);

        Node* newNode = new Node(value);

        if (k == treeSize) {
            newNode->left = treeRoot;
            treeRoot = newNode;
            updateAttributes(treeRoot);
        }
        else {
            newNode->left = treeRoot->left;
            treeRoot->left = nullptr;
            newNode->right = treeRoot;
            treeRoot = newNode;
            updateAttributes(treeRoot->right);
            updateAttributes(treeRoot);
        }
    }

    /* Removing elements from 1 to index - 1 from the tree (they are not deleted though). */
    Node* cutAtBeginning(int index) {
        Node* otherTree = nullptr;
        if (index > treeSize) {
            otherTree = treeRoot;
            treeSize = 0;
            treeRoot = nullptr;
        }
        else if (index > 1) {
            treeRoot = splay(treeRoot, index, true);
            otherTree = treeRoot->left;
            if (otherTree)
                otherTree->addToSubtree += treeRoot->addToSubtree;
            treeRoot->left = nullptr;
            treeSize -= otherTree->subtreeSize;
            updateAttributes(treeRoot);
        }
        return otherTree;
    }

    /* Removing elements from index + 1 to treeSize from the tree (they are not deleted though). */
    Node* cutAtEnd(int index) {
        Node* otherTree = nullptr;
        if (index < 1) {
            otherTree = treeRoot;
            treeSize = 0;
            treeRoot = nullptr;
        }
        else if (index < treeSize) {
            treeRoot = splay(treeRoot, index, true);
            otherTree = treeRoot->right;
            if (otherTree)
                otherTree->addToSubtree += treeRoot->addToSubtree;
            treeRoot->right = nullptr;
            treeSize -= otherTree->subtreeSize;
            updateAttributes(treeRoot);
        }
        return otherTree;
    }

    /* Joining a tree given by a Node* on the left side (at the beginning of the sequence). */
    void joinAtBeginning(Node* otherTree) {
        if (!otherTree) return;
        treeRoot = splay(treeRoot, 1, true);
        treeRoot->left = otherTree;
        treeSize += otherTree->subtreeSize;
        updateAttributes(treeRoot);
    }

    /* Joining a tree given by a Node* on the right side (at the end of the sequence). */
    void joinAtEnd(Node* otherTree) {
        if (!otherTree) return;
        treeRoot = splay(treeRoot, treeSize, true);
        treeRoot->right = otherTree;
        treeSize += otherTree->subtreeSize;
        updateAttributes(treeRoot);
    }

public:
    SplayTree() : treeRoot(nullptr), treeSize(0) {}

    /* Initialize the tree (in this problem at first the sequence contains n ones). */
    void initialize(int n) {
        for (int i = 0; i < n; ++i) {
            insert(1, 1);
        }
    }

    /* Get the value of the i-th element in the sequence. */
    int get(int i) {
        treeRoot = splay(treeRoot, i, true);
        return treeRoot->key;
    }

    /* Fertilize the tree (add k to all elements from a-th to b-th in the sequence). */
    void fertilize(int a, int b, int k) {
        Node* firstPart = cutAtBeginning(a);
        Node* secondPart = cutAtEnd(b - a + 1);
        propagate(treeRoot);
        treeRoot->addToSubtree += k;
        joinAtBeginning(firstPart);
        joinAtEnd(secondPart);
    }

    /* Print the length of the max non-decreasing consecutive subsequence of elements from a-th to b-th. */
    void find(int a, int b) {
        Node* firstPart = cutAtBeginning(a);
        Node* secondPart = cutAtEnd(b - a + 1);
        propagate(treeRoot);
        cout << treeRoot->maxNonDecSubSeqInSubtree << endl;
        joinAtBeginning(firstPart);
        joinAtEnd(secondPart);
    }

};

int main() {
    SplayTree tree;

    int n, m;
    cin >> n >> m;
    tree.initialize(n);

    char c;
    int a, b, k;
    for (int i = 0; i < m; ++i) {
        cin >> c;
        if (c == 'N') {
            cin >> a >> b >> k;
            tree.fertilize(a, b, k);
        }
        else {
            cin >> a >> b;
            tree.find(a, b);
        }
    }

    return 0;
}