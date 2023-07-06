#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

class HashTable
{
private:
    struct KeyValuePair
    {
        char *key;
        char *value;
    };
    struct LinkedList
    {
        KeyValuePair *pair;
        LinkedList *next;
    };

    struct KeyValuePair **pairs;
    struct LinkedList **overflow_buckets;
    int size;
    int count;

    KeyValuePair *allocate_key_pair(char *key, char *value)
    {
        KeyValuePair *pair = (KeyValuePair *)malloc(sizeof(KeyValuePair));
        pair->key = (char *)malloc(strlen(key) + 1);
        pair->value = (char *)malloc(strlen(value) + 1);
        strcpy(pair->key, key);
        strcpy(pair->value, value);
        return pair;
    }

    unsigned long hash_function(char *str)
    {
        unsigned long i = 0;

        for (int j = 0; str[j]; j++)
        {
            i += str[j];
        }

        return i % size;
    }

    LinkedList *linkedlist_insert(LinkedList *list, KeyValuePair *pair)
    {
        if (!list)
        {
            LinkedList *head = (LinkedList *)malloc(sizeof(LinkedList));
            head->pair = pair;
            head->next = NULL;
            list = head;
            return list;
        }
        else if (list->next == NULL)
        {
            LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
            node->pair = pair;
            node->next = NULL;
            list->next = node;
            return list;
        }

        LinkedList *temp = list;

        while (temp->next->next)
        {
            temp = temp->next;
        }

        LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
        node->pair = pair;
        node->next = NULL;
        temp->next = node;
        return list;
    }
    void free_memory(KeyValuePair *pair)
    {
        free(pair->key);
        free(pair->value);
        free(pair);
    }
    void free_memory_linkedlist(LinkedList *list)
    {
        LinkedList *temp = list;

        while (list)
        {
            temp = list;
            list = list->next;
            free(temp->pair->key);
            free(temp->pair->value);
            free(temp->pair);
            free(temp);
        }
    }

public:
    HashTable(int initialSize = 16)
    {
        size = initialSize;
        count = 0;
        pairs = (KeyValuePair **)calloc(size, sizeof(KeyValuePair *));
        overflow_buckets = (LinkedList **)calloc(size, sizeof(LinkedList *));
        for (int i = 0; i < size; i++)
        {
            pairs[i] = NULL;
        }
        for (int i = 0; i < size; i++)
        {
            overflow_buckets[i] = NULL;
        }
    }

    void insert(char *key, char *val)
    {
        KeyValuePair *pair = allocate_key_pair(key, val);

        int idx = hash_function(key);

        KeyValuePair *cur = pairs[idx];

        if (cur == NULL)
        {
            if (count >= size)
            {
                cout << "Hash Table size limit reached"
                     << "\n";
                return;
            }
            pairs[idx] = pair;
            count++;
        }
        else
        {
            if (strcmp(cur->key, key) == 0)
            {
                strcpy(cur->value, val);
                return;
            }
            else
            {
                LinkedList *head = overflow_buckets[idx];
                if (head == NULL)
                {
                    head = (LinkedList *)malloc(sizeof(LinkedList));
                    head->pair = pair;
                    overflow_buckets[idx] = head;
                }
                else
                {
                    overflow_buckets[idx] = linkedlist_insert(head, pair);
                }
                return;
            }
        }
    }

    char *search_key(char *key)
    {
        int idx = hash_function(key);
        KeyValuePair *pair = pairs[idx];
        LinkedList *head = overflow_buckets[idx];
        if (pair != NULL)
        {
            if (strcmp(pair->key, key) == 0)
            {
                return pair->value;
            }

            if (head == NULL)
            {
                return NULL;
            }

            pair = head->pair;
            head = head->next;
        }
        return NULL;
    }

    void tb_delete(char *key)
    {
        int idx = hash_function(key);
        KeyValuePair *pair = pairs[idx];
        LinkedList *head = overflow_buckets[idx];
        if (pair == NULL)
        {
            return;
        }
        else
        {
            if (head == NULL && strcmp(pair->key, key) == 0)
            {
                pairs[idx] = NULL;
                free_memory(pair);
                count--;
                return;
            }
            else if (head != NULL)
            {
                if (strcmp(pair->key, key) == 0)
                {
                    free_memory(pair);
                    LinkedList *node = head;
                    head = head->next;
                    node->next = NULL;
                    pairs[idx] = allocate_key_pair(node->pair->key, node->pair->value);
                    free_memory_linkedlist(node);
                    overflow_buckets[idx] = head;
                    return;
                }

                LinkedList *cur = head;
                LinkedList *prev = NULL;

                while (cur != NULL)
                {
                    if (strcmp(cur->pair->key, key) == 0)
                    {
                        if (prev == NULL)
                        {
                            free_memory_linkedlist(head);
                            overflow_buckets[idx] = NULL;
                            return;
                        }
                        else
                        {
                            prev->next = cur->next;
                            cur->next = NULL;
                            free_memory_linkedlist(cur);
                            overflow_buckets[idx] = head;
                            return;
                        }
                    }
                    cur = cur->next;
                    prev = cur;
                }
            }
        }
    }
};

void print_val(char *val)
{

    if (val == NULL)
    {
        cout << "Not found\n";
    }
    else
    {
        cout << val << "\n";
    }
}
int main()
{
    HashTable hs;
    hs.insert("Test1", "1");
    hs.insert("Test2", "2");
    hs.insert("Test3", "3");
    print_val(hs.search_key("Test1"));
    hs.tb_delete("Test1");
    print_val(hs.search_key("Test1"));
    return 0;
}