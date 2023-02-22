/*
    Austin J. Cash
    CS 344

    This flie contains the struct defintion and function 
    implementations relating to the PID list. This file was
    created because using an LLL is a more appropriate data 
    structure to store child PID's, as child PID's must be
    frequently removed from the middle of the list, which
    contiguous arrays are bad at handling.

    LAST EDIT: 2/21/2023
*/

typedef struct myLLL {
    pid_t child_PID;
    struct myLLL * next;
} list_of_children;



//Takes in a PID and adds a new node to the head of the list. This
//algorithm is done in O(1) time. 
list_of_children * push(pid_t data, list_of_children * head) {
    list_of_children * new_child = (list_of_children*) malloc(sizeof(list_of_children));
    new_child -> child_PID = data;
    new_child -> next = head;
    head = new_child;
    return head;
}



//Deletes the passed-in node by copying the data/pointer
//in the next node, setting the next pointer to the node
//after next, and deleting the next node.
//
//This might seem like a strange way to delete a node,
//but all of the computations are done in constant time,
//and the only time our program deletes a single node is when
//it is already pointing to it, so our usual O(n) search + delete
//algorithm can be replaced by this O(1) algorithm.
void delete_child(list_of_children * to_delete) {
    if(!to_delete) {return;}
    if(!to_delete -> next) {
        free(to_delete);
        to_delete = NULL;
        return;
    }
    list_of_children * temp = to_delete -> next;
    to_delete -> child_PID = temp -> child_PID;
    to_delete -> next = temp -> next;
    free(temp);
    return;
}