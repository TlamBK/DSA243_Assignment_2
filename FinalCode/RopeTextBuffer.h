
#ifndef ROPE_TEXTBUFFER_H
#define ROPE_TEXTBUFFER_H

#include "main.h"

/*
    Student can define other list data structures here
*/
template <typename T>
class SinglyLinkedList
{
private:
    struct Node
    {
        T data;
        Node *next;
        Node() : next(nullptr) {}
        Node(const T &val, Node *next = nullptr) : data(val), next(next) {}
    };

    Node *head; // Dummy head
    Node *tail; // Dummy tail
    int length;

public:
    SinglyLinkedList();
    ~SinglyLinkedList();

    void insertAtHead(T data);
    void insertAtTail(T data);
    void insertAt(int index, T data);
    void deleteAt(int index);
    T &get(int index) const;
    int indexOf(T item) const;
    bool contains(T item) const;
    int size() const;
    void reverse();
    string toString(string (*convert2str)(T &) = nullptr) const;

    class Iterator
    {
    private:
        Node *current;

    public:
        Iterator(Node *node) : current(node) {}

        T &operator*() const
        {
            return current->data;
        }

        Iterator &operator++()
        {
            current = current->next;
            return *this;
        }

        bool operator==(const Iterator &other) const
        {
            return current == other.current;
        }

        bool operator!=(const Iterator &other) const
        {
            return current != other.current;
        }
    };

    Iterator begin() const
    {
        return Iterator(head->next);
    }

    Iterator end() const
    {
        return Iterator(tail);
    }
};
/**
 * Rope (AVL-based, fixed leaf chunk size = 8)
 */
class Rope {
public:
    static const int CHUNK_SIZE = 8;
private:
    class Node {
    public:
        enum BalanceFactor { LH = 1, EH = 0, RH = -1 };
        friend class Rope;
    private:
        Node* left;
        Node* right;
        string data;
        int weight;
        int height;
        BalanceFactor balance;

        Node();
        explicit Node(const string& s);
        bool isLeaf() const;
        friend class TestHelper;
    };

    Node* root;

    int height(Node* node) const;
    int getTotalLength(Node* node) const;
    void update(Node* node);
    Node* rotateLeft(Node* x);
    Node* rotateRight(Node* y);
    Node* rebalance(Node* node);
    void split(Node* node, int index, Node*& outLeft, Node*& outRight);
    Node* concatNodes(Node* left, Node* right);
    char charAt(Node* node, int index) const;
    string toString(Node* node) const;
    void destroy(Node*& node);
    string substringHelper(Node *node, int start, int end) const;

public:
    Rope();
    ~Rope();

    int length() const;
    bool empty() const;
    char charAt(int index) const;
    string substring(int start, int length) const;
    void insert(int index, const string& s);
    void deleteRange(int start, int length);
    string toString() const;

#ifdef TESTING
    friend class TestHelper;
#endif
};

class RopeTextBuffer {
public:
    class HistoryManager; 

private:
    Rope rope;
    int cursorPos;
    HistoryManager* history;

public:
    RopeTextBuffer();
    ~RopeTextBuffer();

    void insert(const string& s);
    void deleteRange(int length);
    void replace(int length, const string& s);
    void moveCursorTo(int index);
    void moveCursorLeft();
    void moveCursorRight();
    int  getCursorPos() const;
    string getContent() const;
    int findFirst(char c) const;
    int* findAll(char c) const;
    void undo();
    void redo();
    void printHistory() const;
    void clear();
#ifdef TESTING
    friend class TestHelper;
#endif
};


class RopeTextBuffer::HistoryManager {
public:
    struct Action {
        string actionName;
        int cursorBefore;
        int cursorAfter;
        string data;
        string data_replace = "";
    };

    SinglyLinkedList<Action> actions;
    int currentIndex;

public:
    HistoryManager();
    ~HistoryManager();
    void addAction(const Action& a);
    bool canUndo() const;
    bool canRedo() const;
    void printHistory() const;
    Action getUndo()
    {
        currentIndex--;
        return actions.get(currentIndex);
    }
    Action getRedo()
    {
        Action a = actions.get(currentIndex);
        currentIndex++;
        return a;
    }
#ifdef TESTING
    friend class TestHelper;
#endif
};

#endif // ROPE_TEXTBUFFER_H

