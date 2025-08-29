#ifndef I2CIP_BST_H_
#define I2CIP_BST_H_

#include <debug.h>

template <typename K, typename T> class BSTNode {
  public:
    BSTNode(K key, T value, BSTNode<K,T>* left = nullptr, BSTNode<K,T>* right = nullptr);
    ~BSTNode();

    K key;                // Mutable (copied) key - passed in
    T value;              // REFERENCE to Value
    BSTNode<K,T>* left;   // Pointer to left (lesser) node
    BSTNode<K,T>* right;  // Pointer to right (greater) node
};

template <typename K, typename T> class BST {
  public:
    BST();

    ~BST();

    // Pointer to root node - all nodes de/allocated on insert()/remove()
    BSTNode<K,T>* root = nullptr;

    /**
     * Recursively finds a spot to insert a new node.
     * @param key
     * @param value
     * @param root - Pass-by-ref, to be modified on insertion
     * @return Pointer to the inserted node
     */
    static BSTNode<K,T>* insert(K key, T value, BSTNode<K,T>*& root, bool overwrite = true);
    BSTNode<K,T>* insert(K key, T value, bool overwrite = true);

    /**
     * Recursively finds a node by key and removes it.
     * @param key
     * @param root
     */
    static BSTNode<K,T>* remove(K key, BSTNode<K,T>*& root);
    BSTNode<K,T>* remove(K key);

    /**
     * Recursively finds a node by key. Breadth-first.
     * @param key
     * @param root
     */
    static BSTNode<K,T>* find(K key, BSTNode<K,T>* const root);
    BSTNode<K,T>* find(K key) const;

    T* operator[](K key) const;

    /**
     * Recursively finds the least-key node. Depth-first.
     * @param root
     */
    static BSTNode<K,T>* findMin(BSTNode<K,T>* root);
    BSTNode<K,T>* findMin(void);

    /**
     * Recursively finds the greatest-key node. Depth-first.
     * @param root
     */
    static BSTNode<K,T>* findMax(BSTNode<K,T>* root);
    BSTNode<K,T>* findMax(void);
};

#include <bst.tpp>

#endif