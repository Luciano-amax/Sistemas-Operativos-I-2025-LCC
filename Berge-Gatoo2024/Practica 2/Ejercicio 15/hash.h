#include <stdlib.h>
#include <string.h>
#include <assert.h>

unsigned long hash(char *str)
{
    unsigned long hash = 7;

    do
        hash = (hash << 5) - hash + (unsigned long)*str; /* hash * 31 + *str */
    while(*(++str));
    return hash;
}

typedef struct _node{
    char* key;
    char* value;
    struct _node *next;
} Node;

typedef Node* List;

List list_init() {
    return NULL;
}

List list_add(List inicio, char *value, char *key){
    Node *temp= malloc(sizeof(Node));
    
    temp->key = malloc(sizeof(char) * (strlen(key) + 1));
    temp->value = malloc(sizeof(char) * (strlen(value) + 1));
    
    strcpy(temp->key, key);
    strcpy(temp->value, value);
    temp->next = inicio;

    return temp;
}

List list_delete(List init, char* key){
    Node* prev = NULL, *curr = init;

    for(; curr != NULL && strcmp(key, curr->key); prev = curr, curr = curr->next);
    
    if(curr != NULL)
    {
        if(prev == NULL)
            init = init->next;
        else
            prev->next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
    }
    
    return init;
}

char* list_find_value(List i, char* key){
    for( ; i != NULL; i = i->next)
        if(!strcmp(i->key, key)) {
            size_t sz = strlen(i->value)+1;
            return strcpy(
                malloc(sizeof(char) * sz),
                i->value
            );
        }
    
    return NULL;
}

List list_destroy(List i) {
    while(i != NULL)
    {
        Node* temp = i;
        i = i->next;
        free(temp->key);
        free(temp->value);
        free(temp);
    }
    return NULL;
}

typedef List HashCell;

typedef struct {
    HashCell* cells;
    size_t size;
    size_t count_cells;
} HashTable;

HashTable hashtable_init(size_t count_cells) {
    HashTable t;
    t.count_cells = count_cells;
    t.size = 0;
    t.cells = (HashCell*) calloc(count_cells,sizeof(HashCell));
    for(int i=0;i<count_cells;i++)
        assert(t.cells[i] == NULL);
    return t;
}

HashCell* _hasthable_get_cell(HashTable table, char* key) {
    return table.cells + hash(key)%table.count_cells;
}

HashTable hashtable_add(HashTable table, char *value, char *key) {
    HashCell* cell = _hasthable_get_cell(table, key);

    // Remove the key if it already exists
    *cell = list_delete(*cell, key);
    *cell = list_add(*cell, value, key);
    table.size++;
    return table;
}

char* hashtable_find_value(HashTable table, char* key) {
    HashCell* cell = _hasthable_get_cell(table, key);
    return list_find_value(*cell, key);
}

HashTable hashtable_delete(HashTable table, char* key) {
    HashCell* cell = _hasthable_get_cell(table, key);
    *cell = list_delete(*cell, key);
    table.size--;
    return table;
}

HashTable hashtable_destroy(HashTable table) {
    for(int i=0;i<table.count_cells;i++)
        table.cells[i] = list_destroy(table.cells[i]);
    table.size = 0;
    return table;
}