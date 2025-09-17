#include "RopeTextBuffer.h"


template <typename T>
SinglyLinkedList<T>::SinglyLinkedList() : length(0)
{
    head= new Node(); // dummy head
    tail= new Node(); // dummy tail
    head->next= tail;
}

template <typename T>
SinglyLinkedList<T>::~SinglyLinkedList() {
    Node* current= head;
    while(current!= nullptr){
        Node* temp= current;
        current= current->next;
        delete temp; 
    }
    head= nullptr; 
    tail= nullptr;
}

template <typename T>
void SinglyLinkedList<T>::insertAtHead(T data){
    Node* newNode= new Node(data, head->next);
    head->next= newNode;
    length++;
}

template <typename T>
void SinglyLinkedList<T>::insertAtTail(T data){
    Node* newNode= new Node(data, tail);
    Node* temp= head;
    while(temp->next != tail) {
        temp= temp->next;
    }
    temp->next= newNode;
    length++;
}

template <typename T>
void SinglyLinkedList<T>::insertAt(int index, T data){
    if(index<0 || index>length) throw out_of_range("Index is invalid!");
    if(index == 0) return insertAtHead(data);
    if(index == length) return insertAtTail(data);
    
    Node* temp= head;
    for(int i=0; i<index; i++){
        temp= temp->next;
    }
    Node* newNode= new Node(data, temp->next);
    temp->next= newNode;
    length++;
}

template<typename T>
void SinglyLinkedList<T>::deleteAt(int index) {
    if(index<0 || index>=length) {
        throw out_of_range("Index is invalid!");
    }
    Node* temp= head;
    for(int i= 0; i<index; i++) {
        temp= temp->next;
    }
    Node* deletenode = temp->next;
    temp->next = deletenode->next;
    delete deletenode;
    length--;
}

template <typename T>
T& SinglyLinkedList<T>::get(int index) const {
    if (index<0 || index>=length) {
        throw out_of_range("Index is invalid!");
    }
    Node* temp= head->next;
    for(int i=0; i<index; i++){
        temp= temp->next;
    }
    return temp->data;
}

template <typename T>
int SinglyLinkedList<T>::indexOf(T item) const {
    Node* temp= head->next;
    int index= 0;
    while(temp != tail) {
        if(temp->data == item) return index;
        temp= temp->next;
        index++;
    }
    return -1;
}

template <typename T>
bool SinglyLinkedList<T>::contains(T item) const {
    Node* temp= head->next;
    while(temp != tail) {
        if(temp->data == item) return true;
        temp= temp->next;
    }
    return false;
}

template <typename T>
int SinglyLinkedList<T>::size() const {
    return length;
}

template <typename T>
void SinglyLinkedList<T>::reverse(){
    if (length <= 1) return;
    Node* prev = nullptr;
    Node* current = head->next;
    Node* next = nullptr;
    while(current != tail) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head->next->next = tail;
    head->next = prev;
}

template <typename T>
string SinglyLinkedList<T>::toString(string (*convert2str)(T &)) const {
    stringstream ss;
    ss << "[";
    Node* temp= head->next;
    while(temp != tail) {
        if(convert2str) {
            ss << convert2str(temp->data);
        }
        else {
            ss << temp->data;   
        }
        if (temp->next != tail) {
            ss << ", ";
        }
        temp= temp->next;
    }
    ss << "]";
    return ss.str();
}

//  NODE IMPLEMENTATION 

Rope::Node::Node() : left(nullptr), right(nullptr), data(""), weight(0), height(1), balance(EH) {}

Rope::Node::Node(const string &s)
{
    this->left= nullptr;
    this->right= nullptr;
    this->data= s;
    this->weight= s.size();
    this->height= 1;
    this->balance= EH;
}

bool Rope::Node::isLeaf() const
{
    if(left == nullptr && right == nullptr) {
        return true;
    }
    return false;
}

//  ROPE IMPLEMENTATION 

Rope::Rope() : root(nullptr) {}

Rope::~Rope()
{
    destroy(root);
}

int Rope::height(Node *node) const {
    if(node == nullptr) return 0;
    else {
        return node->height;
    }
}

int Rope::getTotalLength(Node *node) const {
    if (node == nullptr) return 0;
    if (node->isLeaf()) {
        return node->weight; //có thể dùng staticast 
    }
    int rightlength= getTotalLength(node->right);
    int leftlength= getTotalLength(node->left);
    return leftlength+rightlength; 
}

void Rope::update(Node *node) {
    if (node == nullptr) return;
    if (node->isLeaf()) {
        node->weight= node->data.size();
    } else {
        node->weight= getTotalLength(node->left);
    }
    node->height= 1 + max(height(node->left), height(node->right));
    int balance= height(node->left)-height(node->right);
    if(balance>=1) {
        node->balance= Node::LH;
    }
    else if(balance<=-1) {
        node->balance= Node::RH;
    }
    else node->balance= Node::EH;
}

Rope::Node* Rope::rotateLeft(Node *x) {
    if (x == nullptr || x->right == nullptr) {
        return x;
    }
    Node* T1= x->right;
    Node* T2= T1->left;
    T1->left= x;
    x->right= T2;
    update(x);
    update(T1);
    return T1;
}

Rope::Node* Rope::rotateRight(Node *y) {
    if (y == nullptr || y->left == nullptr) {
        return y; 
    }
    Node* T1= y->left;
    Node* T2= T1->right;
    T1->right= y;
    y->left= T2;
    update(y);
    update(T1);
    return T1;
}

Rope::Node* Rope::rebalance(Node *node) {
    if (node == nullptr) return nullptr;
    update(node);
    int bal= height(node->left)-height(node->right);
    if (bal>1) {
        if(node->left->balance==Node::RH) {
            node->left=rotateLeft(node->left);
        }
        return rotateRight(node);
    } 
    else if (bal<-1) {
        if(node->right->balance==Node::LH) {
            node->right=rotateRight(node->right);  
        }
        return rotateLeft(node); 
    }
    return node;
}

void Rope::split(Node *node, int index, Node *&outLeft, Node *&outRight) {
    if(node==nullptr) {
        outRight=nullptr;
        outLeft=nullptr;
        return;
    }
    if(node->isLeaf()) {
        if(index<=0) {//maybe check lại điều kiện
            outLeft= nullptr;
            outRight= node;
            return;
        }
        else if(index>=node->data.size()) {//check lại điều kiện
            outLeft= node;
            outRight= nullptr;
            return;
        }
        else {
            outLeft= new Node(node->data.substr(0, index));
            outRight= new Node(node->data.substr(index));
            delete node;
            return;
        }   
    }

    if(index == node->weight) {
        outLeft= node->left;
        outRight= node->right;
        delete node;
    }
    else if(index<node->weight) {
        Node* leftSite= nullptr;
        Node* rightSite= nullptr;
        split(node->left, index, leftSite, rightSite);//chia 
        outLeft= leftSite;
        outRight= concatNodes(rightSite, node->right);//nối phần dư vào nhánh phải
        delete node;
    }
    else {
        Node* leftSite= nullptr;
        Node* rightSite= nullptr;
        split(node->right, index - node->weight, leftSite, rightSite);//chia
        outLeft= concatNodes(node->left, leftSite);//nối phần bên trái mới tách vào nhánh trái đã lấy trọn
        outRight= rightSite;
        delete node;
    }
    if(outLeft) outLeft= rebalance(outLeft);
    if(outRight) outRight= rebalance(outRight);
}

Rope::Node* Rope::concatNodes(Node* left, Node* right) {
    if (left == nullptr) return right;
    if (right == nullptr) return left;
    Node* temp= new Node("");
    temp->left= left;
    temp->right= right;
    update(temp);
    return rebalance(temp);
}

char Rope::charAt(Node *node, int index) const {
    if(node==nullptr) {
        throw out_of_range("Index is invalid!");
    }
    if(node->isLeaf()) {
        if(index<0 || index>node->data.size()) {
            throw out_of_range("Index is invalid!");
        }
        return node->data[index];
    }
    if(index<node->weight) {
        return charAt(node->left, index);
    }
    else {
        return charAt(node->right, index-node->weight);
    }
}

string Rope::toString(Node *node) const {
    if(node==nullptr) return "";
    if(node->isLeaf()) {
        return node->data;
    }
    return toString(node->left)+toString(node->right);
}

void Rope::destroy(Node *&node) {
    if(node==nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
    node=nullptr;//tránh treo con trỏ
}

string Rope::substringHelper(Node *node, int start, int end) const {
    if (node == nullptr || start > end) return "";
    if (node->isLeaf()) {
        int s= max(0,start); //TH start âm thì đảm bảo vẫn cắt ở đầu
        int e= (node->data.size() < end) ? node->data.size():end; //TH end quá size vấn đảm bảo cắt tới cuối
        if(s>=e) return "";
        else return node->data.substr(s,e-s);
    }
    string ss;
    if (start < node->weight) {
        ss += substringHelper(node->left, start, min(end, node->weight));
    }
    if (end > node->weight) {
        ss += substringHelper(node->right, max(0, start - node->weight), end - node->weight);
    }
    return ss;
}

//  PUBLIC INTERFACE

string Rope::substring(int start, int length) const {
    if(start<0  || start>this->length()) {
        throw out_of_range("Index is invalid!");
    }
    if(length<0 || start+length>this->length()){
        throw out_of_range("Length is invalid!");
    }
    return substringHelper(root, start, start+length);
}

int Rope::length() const
{
    return getTotalLength(root);
}

char Rope::charAt(int index) const {
    return charAt(root,index);
}

bool Rope::empty() const {
    return (root==nullptr) ? true:false;
}

void Rope::insert(int index, const string &s) {
    if(s.empty()) return;
    if(index<0 || index>length()) {
        throw out_of_range("Index is invalid!");
    }
    Node* R1= nullptr;
    Node* R2= nullptr;
    split(root, index, R1, R2);//chia cây
    Node* subTree= nullptr;
    int pos= 0;
    while (pos < (int)s.size()) {
        Node* leaf= new Node(s.substr(pos, CHUNK_SIZE)); //tạo rope 
        if (subTree == nullptr) {
            subTree= leaf;
        } else {
            subTree= concatNodes(subTree, leaf);
        }
        pos+=CHUNK_SIZE;
    }// tạo 1 subtree mới luôn thay vì tách thành các lá rồi nối, kết quả cũng giống y hệt

    Node* temp= nullptr;
    if (R1 == nullptr) temp= subTree;
    else if (subTree == nullptr) temp= R1;
    else temp= concatNodes(R1, subTree); //nối R1

    if (R2 != nullptr) {
        temp= concatNodes(temp, R2);
    }
    root= temp;
}

void Rope::deleteRange(int start, int length) {
    if(start<0  || start>this->length()) {
        throw out_of_range("Index is invalid!");
    }
    if(length<0 || start+length>this->length()){
        throw out_of_range("Length is invalid!");
    }
    Node* R1= nullptr;
    Node* R2= nullptr;
    split(root, start, R1, R2);
    Node* R3= nullptr;
    Node* R4= nullptr;
    split(R2, length, R3, R4);
    root= concatNodes(R1,R4);
}

string Rope::toString() const {
    return toString(root);
}

//  ROPETEXTBUFFER IMPLEMENTATION 
RopeTextBuffer::RopeTextBuffer()
    : cursorPos(0)
{
    history= new HistoryManager();
}

RopeTextBuffer::~RopeTextBuffer()
{
    delete history;
}

void RopeTextBuffer::insert(const string &s)
{
    if (s.empty()) return;
    HistoryManager::Action a;
    a.actionName = "insert";
    a.data = s;
    a.cursorBefore= this->cursorPos;
    rope.insert(this->cursorPos,s);
    this->cursorPos= this->cursorPos+s.size();
    a.cursorAfter= this->cursorPos;
    history->addAction(a);
}

void RopeTextBuffer::deleteRange(int length)
{
    if(length+this->cursorPos > rope.length() || length<0) {
        throw out_of_range("Length is invalid!");
    }
    HistoryManager::Action a;
    a.actionName= "delete";
    a.data= rope.substring(this->cursorPos,length);
    a.cursorBefore= this->cursorPos;
    rope.deleteRange(this->cursorPos,length);
    a.cursorAfter= this->cursorPos;
    history->addAction(a);
}

void RopeTextBuffer::replace(int length, const string &s)
{
    if(length+this->cursorPos > rope.length() || length<0) {
        throw out_of_range("Length is invalid!");
    }
    HistoryManager::Action a;
    a.actionName= "replace";
    a.data= rope.substring(this->cursorPos,length);
    a.data_replace= s;
    a.cursorBefore= this->cursorPos;
    rope.deleteRange(this->cursorPos,length);
    rope.insert(this->cursorPos,s);
    this->cursorPos= this->cursorPos+s.size();
    a.cursorAfter= this->cursorPos;
    history->addAction(a);
}

void RopeTextBuffer::moveCursorTo(int index)
{
    if (index <0 || index > rope.length()) {
        throw out_of_range("Index is invalid!");
    }
    HistoryManager::Action a;
    a.actionName= "move";
    a.cursorBefore= cursorPos;
    a.data = "J"; 
    a.cursorAfter= index;
    this->cursorPos= index;
    history->addAction(a);
}

void RopeTextBuffer::moveCursorLeft()
{
    if (this->cursorPos <= 0) {
        throw cursor_error();
    }
    HistoryManager::Action a;
    a.actionName= "move";
    a.cursorBefore= cursorPos;
    a.data= "L"; 
    a.cursorAfter= this->cursorPos - 1;
    this->cursorPos--;
    history->addAction(a);
}

void RopeTextBuffer::moveCursorRight()
{
    if (this->cursorPos >= rope.length()) {
        throw cursor_error();
    }
    HistoryManager::Action a;
    a.actionName = "move";
    a.cursorBefore = cursorPos;
    a.data = "R"; 
    a.cursorAfter =this->cursorPos +1;
    this->cursorPos++;
    history->addAction(a);
}

int RopeTextBuffer::getCursorPos() const
{
    return this->cursorPos;
}

string RopeTextBuffer::getContent() const
{
    stringstream ss;
    for (int i = 0; i < rope.length(); ++i) {
        ss << rope.charAt(i);
    }  
    return ss.str();
}

int RopeTextBuffer::findFirst(char c) const
{
    for(int i=0; i< rope.length(); ++i) {
        if(rope.charAt(i)==c) return i;
    }
    return -1;
}

int *RopeTextBuffer::findAll(char c) const
{
    int count= 0;
    for (int i= 0; i<rope.length(); ++i) {
        if (rope.charAt(i) == c) {
            count++;
        }
    }
    if (count == 0) return nullptr;
    int* rs= new int[count + 1];
    int j= 0;
    for (int i= 0; i<rope.length(); ++i) {
        if (rope.charAt(i) == c) {
            rs[j++]= i;
        }
    }
    rs[j]= -1; 
    return rs;
}

void RopeTextBuffer::undo()
{
    if(!history->canUndo())
        return;
    HistoryManager::Action a = history->getUndo();
    if(a.actionName == "insert") {
        rope.deleteRange(a.cursorBefore, a.data.size());
        this->cursorPos=a.cursorBefore;
    }
    else if(a.actionName == "delete") {
        rope.insert(a.cursorBefore, a.data);
        this->cursorPos=a.cursorBefore;
    }
    else if(a.actionName == "move") {
        this->cursorPos=a.cursorBefore;
    }
    else if(a.actionName == "replace") {
        rope.deleteRange(a.cursorBefore, a.data_replace.size());
        rope.insert(a.cursorBefore, a.data);
        this->cursorPos = a.cursorBefore;
    }
}    

void RopeTextBuffer::redo()
{
    if(!history->canRedo())
        return;
    HistoryManager::Action a = history->getRedo();
    if(a.actionName == "insert") {
        rope.insert(a.cursorBefore, a.data);
        this->cursorPos=a.cursorAfter;
    }
    if(a.actionName == "delete") {
        rope.deleteRange(a.cursorBefore, a.data.size());
        this->cursorPos=a.cursorAfter;
    }
    if(a.actionName == "move") {
        this->cursorPos=a.cursorAfter;
    }
    if(a.actionName == "replace") {
        rope.deleteRange(a.cursorBefore, a.data.size());
        rope.insert(a.cursorBefore, a.data_replace);
        this->cursorPos=a.cursorAfter;
    }
}

void RopeTextBuffer::printHistory() const
{
    history->printHistory();
}

void RopeTextBuffer::clear()
{
    rope = Rope();  
    cursorPos = 0;
    delete history;              
    history = new HistoryManager();
}


//  HistoryManager 
RopeTextBuffer::HistoryManager::HistoryManager() : currentIndex(0) {}

RopeTextBuffer::HistoryManager::~HistoryManager() {}

void RopeTextBuffer::HistoryManager::addAction(const Action &a)
{
   while (actions.size()>currentIndex) {
        actions.deleteAt(actions.size() - 1);
    }
    actions.insertAtTail(a);
    currentIndex = actions.size();
}

bool RopeTextBuffer::HistoryManager::canUndo() const
{
    return currentIndex > 0;
}

bool RopeTextBuffer::HistoryManager::canRedo() const
{
    return currentIndex < actions.size();
}

void RopeTextBuffer::HistoryManager::printHistory() const
{
    cout << "[";
    auto it = actions.begin();
    int index = 0;
    while (it != actions.end() && index < currentIndex) {
        const Action& a = *it;
        cout << "(" << a.actionName << ", " << a.cursorBefore << ", " << a.cursorAfter << ", " << a.data << ")";
        ++it;
        ++index;
        if (it != actions.end() && index < currentIndex) cout << ", ";
    }
    cout << "]";
}