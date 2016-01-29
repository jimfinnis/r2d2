/**
 * @file cmdlist.h
 * @brief  Brief description of file.
 *
 */

#ifndef __CMDLIST_H
#define __CMDLIST_H

/// this is the command list, which sends commands from the main program
/// to the process() thread

class CmdList {
    NoteCmd *head,*tail;
    pthread_mutex_t lock;
public:
    CmdList() : head(NULL),tail(NULL) {
        pthread_mutex_init(&lock,NULL);
    }
    
    ~CmdList(){
        pthread_mutex_destroy(&lock);
    }
    
    bool empty(){
        return head == NULL;
    }
    
    /// add to the list
    void add(NoteCmd *c){
        pthread_mutex_lock(&lock);
        c->next = NULL;
        if(head){
            tail->next = c;
            tail = c;
        } else {
            head = tail = c;
        }
        pthread_mutex_unlock(&lock);
    }
    
    /// get the next command. This will be deleted
    /// when it completes.
    NoteCmd *next(){
        NoteCmd *rv;
        pthread_mutex_lock(&lock);
        rv = head;
        if(head){
            NoteCmd *p = head->next;
            head = p;
            if(!head)tail=NULL;
        }
        pthread_mutex_unlock(&lock);
        return rv;
    }
};       


#endif /* __CMDLIST_H */
