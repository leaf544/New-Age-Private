#ifndef READER_H
#define READER_H

#include <vector>

template <typename T>

class Reader {
public:
    int i = 0;
    int limit = 0;
    std::vector<T>* list;
    void attatch (std::vector<T>* l) {this->list = l; this->limit = l->size();}
    T* at (int pos) {return &list->at(pos);}
    void set (int pos) {this->i = pos;}
    void progress () {this->i++;}
    void regress () {this->i--;}
};

#endif
