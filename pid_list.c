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
//it is already pointing to it, so the typical O(n) search + delete
//algorithm can be replaced by this O(1) algorithm.
void delete_child(list_of_children * to_delete) {
    //Unbracketed, single-line guard clauses are a huge soruce of
    //error. This is not bad style, I will argue with anyone who
    //says otherwise!
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


//Recurses through the passed-in list and checks if they are 
//still alive. Deletes any nodes that contain a PID corresponding.
//to a dead child process. Done in O(n) time, which is optimal given
//we need to iterate through each node.
void check_background_processes(list_of_children * children) {
    int erval;

    if(!children) {return;} 

    //We need to perform the recursive call here, or else the function
    //that we use to delete the nodes could interfere with the normal execution
    //of this function.
    check_background_processes(children -> next);

    waitpid(children -> child_PID, &erval, WNOHANG);
    if(WIFEXITED(erval)) {
        printf("Background process %d exited with value %d", children -> child_PID, WEXITSTATUS(erval));
        delete_child(children);
    }
    else if(WIFSIGNALED(erval)){
        printf("Background process %d terminated by signal %d", children -> child_PID, WTERMSIG(erval));
        delete_child(children);
    }
    return;
}

