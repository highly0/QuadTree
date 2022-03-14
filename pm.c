#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#define SW 0
#define SE 1
#define NW 2
#define NE 3

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


typedef struct point {
    double x, y;
} point;

typedef struct edge {
    point *a, *b;
    char name[8];
} edge;

typedef struct list {
    edge *e;
    struct list *next;
} list;

typedef struct point_list {
    point *p;
    struct point_list *next;
} point_list;

typedef struct quad {
    char type;
    struct quad *child[4];
    point A, B;
    list *edges;
    point_list *l;
    int edge_null;
} quad;

typedef struct bst {
    int in_quad;
    edge *e;
    struct bst *left, *right;
} bst;


void print_tree(quad *quad);
quad *divide_quad(double n);
quad *construct_tree_helper(char *input, int *idx_ptr, double ax, double ay, double bx, double by);
quad *construct_tree(char *input);
void add_edges(quad *q, char *input);
int check_tree(quad *temp);
int command_decoder(char *cmd);

void BUILD_QUADTREE(int width);
void ARCHIVE_QUADTREE();
void INIT_QUADTREE(int width);
void CREATE_LINE(char *line_name, int ax, int ay, int bx, int by);
void INSERT(char *line_name);
int LINE_SEARCH (char *line_name);
void LIST_LINES();
void DISPLAY();
void DELETE_LINE(char *line_name);
void DELETE_POINT(int px, int py);
void WINDOW(int ax, int ay, int bx, int by);
void WINDOW_DISPLAY(int ax, int ay, int bx, int by);
void NEIGHBOR(int px, int py);
void KTH_NEIGHBOR(int px, int py, int k);
void FIND_POLYGON(int px, int py);

// helper functions for the dictionary bst
bst* insert_dictionary(bst *node, edge *e);
bst* new_dictionary_node(edge *e);
bst* search_dictionary_node(bst *node, char *name);
void printInorder(bst* node);
void turn_on(bst *node, char *name);
void turn_off(bst *node, char *name);
int str_cmp (char *a, char*b);

// helper functions for line_search
void line_search_helper(quad *q, edge *e, list **result);
int intersect(point p1, point q1, point p2, point q2);
int intersect_quad(point quad_btm, point quad_top, point b1, point b2);
int onSegment(point p, point q, point r);
int orientation(point p, point q, point r);
int intersect_end_points(point quad_top, point quad_btm, point p_top, point p_btm);

// for the result linked list in line_search
int contains(list *result, edge *e);
void append(list **result, edge *e);
int total(list *result);

// helper functions for the point linked list in insert
int contains_point(point_list *result, point *p);
void append_point(point_list **result, point *p);
int total_point(point_list *result);
void add_points_to_list(quad *q);

// helper functions for insert
void append_list(list **l1, list *l2);
void insert_helper(quad *q, list *l);
int quadtree_valid(quad *q);
int is_single_point(point quad_btm, point quad_top, point p_btm, point p_top);
int within_quad(quad *q, point *p);

// helper functions for delete
quad* delete_line_helper(quad *q, edge *e);
void delete(list **result, edge *e);
int compare_edge(edge *e1, edge *e2);
quad *merge(quad *q);
int num_q_children(quad *q);
int quadtree_valid(quad *q);

// helper functions for delete_point
void find_edges(bst *node, list **result, point *p);

// helper functions for neighbor
//void neighbor_helper(quad *q, point *p, double *closest, list **result);

double MAX_DEPTH;
// global variable for the quadtree
quad *q = NULL;
bst *dictionary = NULL;

int main(int argc, char *argv[]) {
    char user_input[100000] = {'\0'};
    char *temp = (char*)malloc(sizeof(char) *100000);
    int counter = 0, idx = 0, failure = 0;
    while (fgets(user_input, 100000, stdin) != NULL) {
        command_decoder(user_input);
        fflush(stdout);
    } 
    free(temp);
}

/* return 0 if the command is invalid, 1 if valid*/
int command_decoder(char *cmd) {
    char *tok, *tok2;
    char comm[251] = {'\0'}, args[100000] = {'\0'};
    int counter = 0, idx = 0, failure = 0;

    tok = strtok(cmd, "(");
    while(tok != NULL) {
        counter++;
        if(counter == 1) {
            strncpy(comm, tok, strlen(tok));
        } else {
            strncpy(args, tok, strlen(tok));
            break;
        }
        tok = strtok(NULL, ")");
    }

    if(strcmp(comm, "INIT_QUADTREE") == 0) {
        INIT_QUADTREE(atoi(args));
    } else if(strcmp(comm, "BUILD_QUADTREE") == 0){
        BUILD_QUADTREE(atoi(args));
    } else if (strcmp(comm, "CREATE_LINE") == 0) {
        tok2 = strtok(args, ",");
        char *line_name;
        int ax, ay, bx, by;

        while (tok2 != NULL) {
            idx++;
            if(idx == 1) {
                line_name = tok2;
            } else if(idx == 2) {
                ax = atoi(tok2);
            } else if(idx == 3) {
                ay = atoi(tok2);
            } else if(idx == 4) {
                bx = atoi(tok2);
            } else if(idx == 5) {
                by = atoi(tok2);
            }
            tok2 = strtok(NULL, ",");
        } 
        CREATE_LINE(line_name, ax, ay, bx, by);
    } else if(strcmp(comm, "LIST_LINES") == 0) {
        LIST_LINES();
    } else if(strcmp(comm, "DELETE") == 0) {
        DELETE_LINE(args);
    } else if(strcmp(comm, "DELETE_POINT") == 0) {
        tok2 = strtok(args, ",");
        int px, py;

        while (tok2 != NULL) {
            idx++;
            if(idx == 1) {
                px = atoi(tok2);
            } else if(idx == 2) {
                py = atoi(tok2);
            }
            tok2 = strtok(NULL, ",");
        }
        DELETE_POINT(px,py);
    } else if (strcmp(comm, "NEIGHBOR") == 0) {
        tok2 = strtok(args, ",");
        int px, py;

        while (tok2 != NULL) {
            idx++;
            if(idx == 1) {
                px = atoi(tok2);
            } else if(idx == 2) {
                py = atoi(tok2);
            } 
            tok2 = strtok(NULL, ",");
        } 
        NEIGHBOR(px, py);
    } else if (strcmp(comm, "KTH_NEIGHBOR") == 0) {
        tok2 = strtok(args, ",");
        int px, py, k;

        while (tok2 != NULL) {
            idx++;
            if(idx == 1) {
                px = atoi(tok2);
            } else if(idx == 2) {
                py = atoi(tok2);
            } 
            else if(idx == 3) {
                k = atoi(tok2);
            } 
            tok2 = strtok(NULL, ",");
        } 
        KTH_NEIGHBOR(px, py, k);
    } else if (strcmp(comm, "FIND_POLYGON") == 0) {
        tok2 = strtok(args, ",");
        int px, py;

        while (tok2 != NULL) {
            idx++;
            if(idx == 1) {
                px = atoi(tok2);
            } else if(idx == 2) {
                py = atoi(tok2);
            } 
            tok2 = strtok(NULL, ",");
        } 
        FIND_POLYGON(px, py);
    }else if(strcmp(comm, "LINE_SEARCH") == 0) {
        LINE_SEARCH(args);
    } else if (strcmp(comm, "DISPLAY") == 0) {
        DISPLAY();
    }  
    return 0;
}

void INIT_QUADTREE(int width) {
    MAX_DEPTH = pow(2, width);

    q =  (quad *)malloc(sizeof(quad));
    q->A.x = q->A.y = 0;
    q->B.x = q->B.y = MAX_DEPTH;
    q->type = 'W';
    q->edges = (list*)malloc(sizeof(list));
    q->edges = NULL;

    q->edge_null = 1;
    q->l = (point_list *)malloc(sizeof(point_list));
    q->l = NULL;
}
/*BUILD_QUADTREE FUNCTIONS*/
/******************************************************************************************************/
void BUILD_QUADTREE(int width) {
    char user_input[100000] = {'\0'};
    MAX_DEPTH = pow(2, (float) width);

    q = (quad *)malloc(sizeof(quad));

    if(fgets(user_input, 100000, stdin) != NULL) {
        ARCHIVE_QUADTREE(user_input);
        fflush(stdout);
    }
    //printf("finish build_quadtree\n");
}

//outputting the tree representation
void ARCHIVE_QUADTREE(char *input) {
    q = construct_tree(input);
}

quad *construct_tree(char *input) {
    int idx = 0;   
    return construct_tree_helper(input, &idx, 0.0, 0.0, MAX_DEPTH, MAX_DEPTH);
}

quad *construct_tree_helper(char *input, int *idx_ptr, double ax, double ay, double bx, double by) {
    int idx = *idx_ptr;
    char *tok, *ptr;

    // base case: when all the nodes have been inputted to the tree
    if (idx == strlen(input)) {
       return NULL;
    } 
    
    quad *temp = (quad *)malloc(sizeof(quad));
    point *point_a = (point *)malloc(sizeof(point));
    point *point_b = (point *)malloc(sizeof(point));
    point_a->x = ax;
    point_a->y = ay;
    point_b->x = bx;
    point_b->y = by;

    temp->A = *point_a;
    temp->B = *point_b;

    
    (*idx_ptr)++;
    if (input[idx] == 'G') {
        temp->type = 'G';
        temp->edge_null = 1;

        temp->child[0] = construct_tree_helper(input, idx_ptr, ax, ay, ax + (bx - ax)/2.0, ay + (bx - ax)/2.0);
        temp->child[1] = construct_tree_helper(input, idx_ptr, ax + (bx - ax)/2.0, ay, ax + (bx - ax), ay + (bx - ax)/2.0);
        temp->child[2] = construct_tree_helper(input, idx_ptr, ax, ay + (bx - ax)/2.0, ax + (bx - ax)/2.0, ay + (bx-ax));
        temp->child[3] = construct_tree_helper(input, idx_ptr, ax + (bx - ax)/2.0, ay + (bx - ax)/2.0, ax + (bx - ax), ay + (bx - ax));   
    } else if (input[idx] == 'B') {
        //input[idx++] will be [, input[idx+2] will be the edges
        temp->type = 'B';
        temp->edge_null = 0;
        temp->l = (point_list *)malloc(sizeof(point_list));
        temp->l = NULL;

        int curr = strlen(input) - idx;
        char substr[curr];
        memcpy(substr, &input[idx + 1], curr);

        // getting the current set of edges for the B
        tok = strtok(substr, "]");
        // setting the pointer AFTER the set of edges
        (*idx_ptr) += strlen(tok) + 1;

        // allocating memories for edge linked list, ready to get added
        temp->edges = NULL;
        add_edges(temp, strcat(substr, "]"));
        // adding the points existing in this quad
        //add_points_to_list(temp);
    } else if (input[idx] == 'W') {
        temp->edge_null = 1;
        temp->type = 'W';
    } 
    return temp;
}

void add_edges(quad *q, char *input) {
    char *tok, *tok2, *curr;
    int counter = 0, total = 0, flag = 0;

    //getting the list of edges within []
    tok = strtok(input, "[");
    tok = strtok(tok, "]");
    // separating the different edges (from the commas)
    tok2 = strtok(tok, ",");
    while (tok2 != NULL) {
        list *temp = q->edges;

        // searching for the edge in the bst
        bst *node = search_dictionary_node(dictionary, tok2);
        turn_on(dictionary,tok2); // this node in the dictionary is now in the quadtree

        // creating a new node with the new edge
        list *new_node = (list *)malloc(sizeof(list));
        new_node->e = (edge *)malloc(sizeof(edge));
        new_node->e->a = (point *)malloc(sizeof(point));
        new_node->e->b = (point *)malloc(sizeof(point));

        strcpy(new_node->e->name, node->e->name);
        new_node->e->a->x = node->e->a->x;
        new_node->e->a->y= node->e->a->y;
        new_node->e->b->x = node->e->b->x;
        new_node->e->b->y = node->e->b->y;

        new_node->next = NULL;

        // head of the linked list
        if (q->edges == NULL) {
            q->edges = new_node;
        } else { // putting edge to the end of the list
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = new_node;
        }
        tok2 = strtok(NULL, ",");
    } 
}

/*CREATE_LINE FUNCTION*/
/******************************************************************************************************/
void CREATE_LINE(char *line_name, int ax, int ay, int bx, int by) {
    edge *new_edge = NULL;
    new_edge = (edge *)malloc(sizeof(edge));
    new_edge->a = (point *)malloc(sizeof(point));
    new_edge->b = (point *)malloc(sizeof(point));
    //char *name = (char *)malloc(strlen(line_name) + 1);
    //strncpy(name, line_name, strlen(line_name));
    strcpy(new_edge->name, line_name);
    new_edge->a->x = ax;
    new_edge->a->y = ay;
    new_edge->b->x = bx;
    new_edge->b->y = by;

    dictionary = insert_dictionary(dictionary, new_edge);
    printf("LINE %s IS CREATED\n", new_edge->name);
}

/*DICTIONARY FUNCTIONS*/
/******************************************************************************************************/

bst* insert_dictionary(bst *node, edge *e) {
    // dictionary is empty, enter in the root
    if(node == NULL) {
        return new_dictionary_node(e);
    }

    if(str_cmp(node->e->name, e->name) > 0) {
        node->left  = insert_dictionary(node->left, e);
    } else if(str_cmp(node->e->name, e->name) < 0) {
        node->right = insert_dictionary(node->right, e);
    }
    return node;
}

bst* new_dictionary_node(edge *e) {
    bst *new_bst_node = (bst *)malloc(sizeof(bst));
    new_bst_node->e = e;
    new_bst_node->in_quad = 0;
    new_bst_node->left = new_bst_node->right = NULL;

    return new_bst_node;
}

bst* search_dictionary_node(bst *node, char *name) {
    // if the node doesn't exist
    if(node == NULL) return NULL;

    if(str_cmp(node->e->name, name) == 0) {
        return node;
    } 

    if(str_cmp(node->e->name, name) < 0) {
        return search_dictionary_node(node->right, name);
    } 
    return search_dictionary_node(node->left, name);
}

int str_cmp(char *a, char*b) {
    int i = 0;
    while (a[i] && b[i]) {
        char a0 = a[i];
        char b0 = b[i];

        int adig = isalpha(a0);
        int bdig = isalpha(b0);

        if (adig == 0 && bdig == 0) {
            int ad = (int) a0;
            int bd = (int) b0;

            if (ad > bd)
                return 1;
            else if (ad < bd)
                return -1;

        } else if (adig == 0) {
            return 1;
        } else if (bdig == 0) {
            return -1;
        }
        i++;
    }
    return strcmp(a,b);
}
// flag all the nodes in the dictionary that had been inserted into the quadtree
void turn_on(bst *node, char *name) {
    // if the node doesn't exist
    if(node == NULL) return;

    if(str_cmp(node->e->name, name) == 0) {
        node->in_quad = 1;
        return;
    } 

    if(str_cmp(node->e->name, name) < 0) {
        turn_on(node->right, name);
    } 
    turn_on(node->left, name);

}

// flag all the nodes in the dictionary that had been deleted from the quadtree
void turn_off(bst *node, char *name) {
    // if the node doesn't exist
    if(node == NULL) return;

    if(str_cmp(node->e->name, name) == 0) {
        node->in_quad = 0;
        return;
    } 

    if(str_cmp(node->e->name, name) < 0) {
        turn_off(node->right, name);
    } 
    turn_off(node->left, name);
}

/*LIST_LINES FUNCTIONS*/
/******************************************************************************************************/

void LIST_LINES() { 
    printInorder(dictionary);
}

/* Given a binary tree, print its nodes in inorder*/
void printInorder(bst* node) { 
     if (node == NULL) 
        return; 
  
     printInorder(node->left); 
     printf("%s\n", node->e->name);   
     printInorder(node->right); 
} 

/* DELETE FUNCTIONS*/
/******************************************************************************************************/
quad *merge(quad *q) {
    if(num_q_children(q) == 4) {
        return q;
    } else {
        for(int i = 0; i < 4; i ++) {
            if(q->child[i]->type == 'G') {
                q->child[i] = merge(q->child[i]);
            }
        }
        if(num_q_children(q) > 0) {
            return q;
        } else {
            quad *new_quad = (quad *)malloc(sizeof(quad));
            new_quad->l = (point_list *)malloc(sizeof(point_list));
            new_quad->l = NULL;

            new_quad->A.x = q->A.x;
            new_quad->A.y = q->A.y;
            new_quad->B.x = q->B.x;
            new_quad->B.y = q->B.y;
            new_quad->edges = (list *)malloc(sizeof(list)); 
            new_quad->edges = NULL;
            new_quad->type = 'W';
            new_quad->edge_null = 0;

            // new_quad.children <- empty
            for(int i = 0; i < 4; i++) {
                new_quad->child[i] = NULL;
            }
            for(int i = 0; i < 4; i++) {
                if(q->child[i]->edge_null == 0) {
                    list *current = q->child[i]->edges;
                    while(current != NULL) {
                        if(contains(new_quad->edges, current->e) == 0) {
                            append(&new_quad->edges, current->e);
                        }
                        current = current->next;
                    }
                }
                
            }
            if(quadtree_valid(new_quad) == 1) {
                return new_quad; // success
            } else {
                return q;
            }
        }
    }
}

int num_q_children(quad *q){
    int counter = 0;
    for(int i = 0; i < 4; i ++) {
        if(q->child[i]->type == 'G') {
            counter ++;
        }
    }
    return counter;
}

void DELETE_LINE(char *line_name) {
    bst *node = search_dictionary_node(dictionary, line_name);
    if(node != NULL && node->in_quad == 1) {
        q = delete_line_helper(q, node->e);
        printf("%s IS DELETED\n", line_name);
        turn_off(dictionary, line_name);
    } else {
        printf("%s DOES NOT EXIST\n", line_name);
    }   
}

quad* delete_line_helper(quad *q, edge *e) {
    if(q->type == 'G') {
        for(int i = 0; i < 4; i ++) {
            if(intersect_quad(q->child[i]->A, q->child[i]->B, *e->a, *e->b) == 1) {
                q->child[i] = delete_line_helper(q->child[i], e);
            }
        }
        q = merge(q);
    } else {
        delete(&q->edges, e);
        if(q->edges == NULL) {
            q->edge_null = 1;
            q->type = 'W';
        } else {
            q->edge_null = 0;
            q->type = 'B';
        }
    }
    return q;
} 

// compare if 2 edges are the same, return 1 if true, 0 otherwise
int compare_edge(edge *e1, edge *e2) {
    if(strcmp(e1->name, e2->name) == 0  
        && (e1->a->x == e2->a->x && e1->a->y == e2->a->y)
        && (e1->b->x == e2->b->x && e1->b->y == e2->b->y)){
            return 1;
    }
    return 0;
}

void delete(list **result, edge *e) {
    list *prev, *current = *result;

    // deleting the head
    if(current != NULL && compare_edge(current->e, e) == 1) {
        *result = current->next;
        free(current); 
        return;
    }  
    while(current != NULL && compare_edge(current->e, e) == 0) {
        prev = current;
        current = current->next;
    }

    prev->next = current->next;
 
    free(current);
    return;
}

int quadtree_valid(quad *q) {
    if(q->type == 'G') {
        int a = quadtree_valid(q->child[0]);
        int b = quadtree_valid(q->child[1]);
        int c = quadtree_valid(q->child[2]);
        int d = quadtree_valid(q->child[3]);

        if(a == 1 && b == 1 && c == 1 && d == 1) {
            return 1;
        } else return 0;
    } else { // a leaf node
        add_points_to_list(q);
        point_list *curr = NULL;
        curr = q->l;

        // if there are multiple edges and 1 points
        if(total_point(q->l) == 1 && total(q->edges) > 1) {
            list *curr = q->edges;
            point *p_btm, *p_top;
            while(curr!= NULL) {
                // if one of the edge is not within the quad
                if(within_quad(q, curr->e->a) == 0 && within_quad(q, curr->e->b) == 0) {
                    return 0;
                } 
                curr = curr->next;
            }
        }
        if(total_point(q->l) > 1 || (total(q->edges) > 1 && total_point(q->l) != 1)) {
            return 0;
        } return 1;
        
    }
}

/*
loop through the bst (only the inserted node) and turn off -> delete those 
*/
/*DELETE POINT FUNCTIONS*/
/******************************************************************************************************/

void DELETE_POINT(int px, int py) {
    point *p = (point*) malloc(sizeof(point));
    p->x = px;
    p->y = py;
    list *result = (list*)malloc(sizeof(list));
    result = NULL;
    find_edges(dictionary, &result, p);
    list *current = result;

    while(current != NULL) {
        turn_off(dictionary, current->e->name);
        delete_line_helper(q, current->e);
        current = current ->next;
    }
    printf("%d LINE(S) DELETED\n", total(result));

    free(p);
    free(result);
    //print_tree(q);
}

// find all inserted edges with the same end points
void find_edges(bst *node, list **result, point *p) {
    if(node == NULL) return;

    find_edges(node->left, result, p);
    if(node->in_quad == 1) {
        if((node->e->a->x == p->x && node->e->a->y == p->y)
            || (node->e->b->x == p->x && node->e->b->y == p->y)) {
                if(contains(*result, node->e) == 0) {
                    append(result, node->e);
                } 
        }   
    }
    find_edges(node->right, result, p);
}

// returns true if the point is within the quad, 0 otherwise
int within_quad(quad *q, point *p) {
    if((p->x >= q->A.x && p->y >= q->A.y) && (p->x <= q->B.x && p->y <= q->B.y)) {
        return 1;
    } 
    return 0; 
}

void add_points_to_list(quad *q) {
    point *btm = NULL, *top = NULL;
    point *p_btm = NULL, *p_top = NULL;
    list *current = q->edges;
    while(current != NULL) {
        if(current->e->a->y > current->e->b->y) {
            p_top = current->e->a;
            p_btm = current->e->b; 
        } else {
            p_btm = current->e->a;
            p_top = current->e->b; 
        }     
        
        // checking if x or y is within the quad
        if(within_quad(q, p_btm) == 1 && contains_point(q->l, p_btm) == 0) {
                //printf("got here\n");
            append_point(&q->l, p_btm);
        } 
        if(within_quad(q, p_top) == 1 && contains_point(q->l, p_top) == 0) {
            append_point(&q->l, p_top);
        }  
        current = current->next;
    }
}

/* HELPER FUNCTIONS FOR POINT_LIST*/
/******************************************************************************************************/

int contains_point(point_list *result, point *p) {
    point_list *current = result;
    while(current != NULL) {
        if(current->p->x == p->x && current->p->y == p->y) {
            return 1;
        }
        if (current->next != NULL && current->p != NULL){
            current = current->next;
        } else return 0;
    }
    return 0;
}

void append_point(point_list **result, point *p) {
    point_list *last = *result;
    point_list *new_node = NULL;
    new_node = (point_list*) malloc(sizeof(point_list));

    point *new_point = (point *)malloc(sizeof(point));
    new_point = p;
    new_node->p = new_point;
    new_node->next = NULL;

    if(*result == NULL) {
        *result = new_node;
        return;
    }

    while(last->next != NULL) {
        last = last->next;
    } 
    last->next = new_node;
    return;
}

int total_point(point_list *result) {
    point_list *current = result;
    int total = 0;

    while(current != NULL) {
        total++;
        current = current->next;
    }
    return total;
}


/* LINE_SEARCH FUNCTIONS*/
/******************************************************************************************************/
int orientation(point p, point q, point r) { 
 
    double val = (q.y - p.y) * (r.x - q.x) - 
              (q.x - p.x) * (r.y - q.y); 
  
    if (val == 0.0) return 0;  // colinear 
  
    return (val > 0)? 1: 2; // clock or counterclock wise 
} 

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
int onSegment(point p, point q, point r) { 
    if (q.x <= MAX(p.x, r.x) && q.x >= MIN(p.x, r.x) && 
        q.y <= MAX(p.y, r.y) && q.y >= MIN(p.y, r.y)) 
       return 1; 
  
    return 0; 
} 

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
int intersect(point p1, point q1, point p2, point q2) { 
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 
  
    // General case 
    if (o1 != o2 && o3 != o4) 
        return 1; 
  
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1) == 1) return 1; 
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1) == 1) return 1; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2) == 1) return 1; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2) == 1) return 1; 
  
    return 0; // Doesn't fall in any of the above cases 
} 

// returns if only one of the end points is within the quad
int intersect_quad(point quad_btm, point quad_top, point b1, point b2) {
    double curr_x, curr_y;
    double slope = (double)(b1.y - b2.y) / (double)(b1.x - b2.x);
    double b = (double) b1.y - (slope * ((double)b1.x));


    // if either one of the point exist within the quad
    if((b1.x >= quad_btm.x && b1.y >= quad_btm.y) && (b1.x <= quad_top.x && b1.y <= quad_top.y)) {
        return 1;
    } 
    if((b2.x >= quad_btm.x && b2.y >= quad_btm.y) && (b2.x <= quad_top.x && b2.y <= quad_top.y)) {
        return 1;
    } 

    point *p1 = (point*)malloc(sizeof(point)), 
    *p2 = (point*)malloc(sizeof(point)), 
    *p3 = (point*)malloc(sizeof(point)), 
    *p4 = (point*)malloc(sizeof(point));

    p1 = &quad_btm;

    p2->x = quad_btm.x;
    p2->y = quad_top.y;

    p3->x = quad_top.x;
    p3->y = quad_btm.y;

    p4 = &quad_top;


    int bool1 = intersect(*p1, *p2, b1, b2);
    int bool2 = intersect(*p1, *p3, b1, b2);
    int bool3 = intersect(*p2, *p4, b1, b2);
    int bool4 = intersect(*p3, *p4, b1, b2);

    if ((bool1 == 1 && bool2 == 1) || (bool1 ==1 && bool3 ==1) 
    || (bool1==1 && bool4 ==1) || (bool2 == 1 && bool4 ==1) 
    || (bool2 ==1  && bool3 ==1 ) || (bool3 ==1 && bool4==1)) {
        return 1;
    }
    return 0;
}

int intersect_end_points(point p_top, point p_btm, point p2_top, point p2_btm) {
    if((p_top.x == p2_btm.x && p_top.y == p2_btm.y) 
        || (p_top.x ==  p2_top.x && p_top.y == p2_top.y)
        || (p_btm.x == p2_btm.x && p_btm.y == p2_btm.y) 
        || (p_btm.x ==  p2_top.x && p_btm.y == p2_top.y)) {
            return 1;
    }
    return 0;
}

int LINE_SEARCH(char *line_name) { 
    list *result = (list *) malloc(sizeof(result));
    result = NULL;

    bst *node = search_dictionary_node(dictionary, line_name);
    //line_search_helper(dictionary, node->e, &result);
    line_search_helper(q, node->e, &result);
    if(result != NULL) {
        printf("%s INTERSECTS %d LINE(S)\n", line_name, total(result));
        return 1;
    } else {
        printf("%s DOES NOT INTERSECT ANY EXISTING LINE\n", line_name);
        return 0;
    } 
}

void line_search_helper(quad *q, edge *e, list **result) {
    if (q->type == 'G') {
        for(int i = 0; i < 4; i ++) {         
            if(intersect_quad(q->child[i]->A, q->child[i]->B, *e->a, *e->b) == 1) {
                line_search_helper(q->child[i], e, result);
            }
        }
    } else {
        if(q->edge_null == 0) {
            list *curr = q->edges;
            while(curr != NULL) {
                if(intersect(*curr->e->a, *curr->e->b, *e->a, *e->b) == 1
                && intersect_end_points(*curr->e->a, *curr->e->b, *e->a, *e->b) == 0
                && contains(*result, curr->e) == 0) {
                    append(result, curr->e);
                } 
                curr = curr->next;
            }
        }     
    }
}  

/*LIST HELPER FUNCTIONS*/
/******************************************************************************************************/
int contains(list *result, edge *e) {
    list *current = result;
    while(current != NULL) {
        if(strcmp(current->e->name, e->name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void append(list **result, edge *e) {
    list *new_node = (list*) malloc(sizeof(list));
    new_node->e = e;

    if(*result == NULL) {
        *result = new_node;
        new_node->next = NULL;
        return;
    } else {
        // adding the node as the head of the linked list
        new_node->next = *result;
        *result = new_node;
        return;
    }
}

int total(list *result) {
    list *current = result;
    int total = 0;

    while(current != NULL) {
        total++;
        current = current->next;
    }
    return total;
}

/*NEIGHBOR FUNCTIONS*/
/******************************************************************************************************/
// Node 
typedef struct queue { 
    quad *q; 
    edge *e;

    double distance; 
    struct queue* left;
    struct queue* right;
} queue; 

double distance(point *p, edge *line);
double distance_quad(point *p, quad *q);
queue *new_queue_node(quad *q, edge *e, double distance);
queue *enqueue(queue *node, quad *q, edge *e, double distance);
queue *dequeue(queue *node);
queue *neighbor_helper(quad *q, point *p, int k);
void printQueue(queue *q);
void remove_queue_node(queue **node);


queue *new_queue_node(quad *q, edge *e, double distance) {
    queue *new = (queue*)malloc(sizeof(queue));
    new->q = q;
    new->e = e;
    new->distance = distance;
    new->left = new->right = NULL;
    return new;
}

queue *enqueue(queue *node, quad *q, edge *e, double distance) {
    if(node == NULL) {
        return new_queue_node(q, e, distance);
    }
    if(distance < node->distance) {
        node->left = enqueue(node->left, q, e, distance);
    } else if(distance > node->distance){
        node->right = enqueue(node->right, q, e, distance);
    } else {
        if(node->e != NULL && e!= NULL) {
            if(strcmp(node->e->name, e->name) > 0) {
                node->left = enqueue(node->left, q, e, distance);
            } else {
                node->right = enqueue(node->right, q, e, distance);
            }
        } else if (node->q != NULL && q!= NULL) {
            node->right = enqueue(node->right, q, e, distance);
        }       
    }
    return node;
}

queue *dequeue(queue *node) {
    if(node->left != NULL) {
        return dequeue(node->left);
    }
    return node;  
} 

void remove_queue_node(queue **node) {
    queue *current = *node, *prev = *node;
    if(current->left == NULL) {
        *node = current->right;
        return;
    }
    while(current != NULL) {
        if(current->left == NULL) {
            if(current->right != NULL) {
                prev->left = current->right;
            } else {
                prev->left = NULL;
            }
            break;          
        } 
        prev = current;
        current = current->left;  
    } 
} 

void NEIGHBOR(int px, int py) {
    point *p = (point *)malloc(sizeof(point));
    p->x = px;
    p->y = py;

    queue *result = neighbor_helper(q, p, 1);
    if(result != NULL) printf("THE NEAREST NEIGHBOR IS %s\n", result->e->name);
    else printf("NULL\n"); 
    free(p);
}

void printQueue(queue *q) {
    if(q == NULL) {
        return;
    }
    printQueue(q->left);
    if(q->e != NULL) {
        printf("%s[%f]\n", q->e->name, q->distance);
    } else if(q->q != NULL) {
        printf("%c[%f]\n", q->q->type, q->distance);
    }
    printQueue(q->right);
}

queue *neighbor_helper(quad *q, point *p, int k) {
    queue *priority_queue = (queue *)malloc(sizeof(queue));
    priority_queue = NULL;
    priority_queue = enqueue(priority_queue, q, NULL, 0.0);
    int counter = 0;
    while(priority_queue != NULL) {
        queue *element = dequeue(priority_queue);
        remove_queue_node(&priority_queue);

        if(element->e != NULL) {
            if(counter >= k - 1) {
                return element;
            } else {
                while(dequeue(priority_queue)->e != NULL
                && strcmp(dequeue(priority_queue)->e->name, element->e->name) == 0) {
                    remove_queue_node(&priority_queue);
                }
                counter ++; 
            }
        } else if(element->q != NULL) {
            if(element->q->type != 'G') {
                list *current = element->q->edges;
                while(current != NULL) {
                    if(element->distance < distance(p, current->e)) {
                        priority_queue = enqueue(priority_queue, NULL, current->e, distance(p, current->e));
                    }
                    current = current->next;
                }
            } else if(element->q->type == 'G') {
                for(int i = 0; i < 4; i ++) {
                    double distance = distance_quad(p, element->q->child[i]);
                    priority_queue = enqueue(priority_queue, element->q->child[i], NULL, distance);
                }
            }
        }
    }   
    return NULL;
}

// returns the distance of the line and the point
double distance(point *p, edge *line) {
    double A = p->x - line->a->x;
    double B = p->y - line->a->y;
    double C = line->b->x - line->a->x;
    double D = line->b->y - line->a->y;

    double dot = A * C + B * D;
    double len_sq = C * C + D * D;
    double param = -1;
    if(len_sq != 0) {
        param = dot / len_sq;
    }
    double xx, yy;

    if(param < 0) {
        xx = line->a->x;
        yy = line->a->y;
    } else if(param > 1) {
        xx = line->b->x;
        yy = line->b->y;
    } else {
        xx = line->a->x + param * C;
        yy = line->a->y + param * D;
    }

    double dx = p->x - xx;
    double dy = p->y - yy;
    return pow((dx * dx + dy * dy), .5);
}

double distance_quad(point *p, quad *q) {
    if(within_quad(q, p) == 1) {
        return 0.0;
    } else {
        double result, min1, min2;
        edge *e1 = (edge*) malloc(sizeof(edge)), *e2 = (edge*) malloc(sizeof(edge)), 
        *e3 = (edge*) malloc(sizeof(edge)), *e4 = (edge*) malloc(sizeof(edge));

        e1->a = (point*) malloc(sizeof(point)); e1->b = (point*) malloc(sizeof(point));
        e2->a = (point*) malloc(sizeof(point)); e2->b = (point*) malloc(sizeof(point));
        e3->a = (point*) malloc(sizeof(point)); e3->b = (point*) malloc(sizeof(point));
        e4->a = (point*) malloc(sizeof(point)); e4->b = (point*) malloc(sizeof(point));

        e1->a->x = q->A.x; e1->a->y = q->A.y; e1->b->x = q->A.x; e1->b->y = q->B.y;
        e2->a->x = q->A.x; e2->a->y = q->A.y; e2->b->x = q->B.x; e2->b->y = q->A.y;
        e3->a->x = q->A.x; e3->a->y = q->B.y; e3->b->x = q->B.x; e3->b->y = q->B.y;
        e4->a->x = q->B.x; e4->a->y = q->A.y; e4->b->x = q->B.x; e4->b->y = q->B.y; 

        double d1 = distance(p, e1);
        double d2 = distance(p, e2);
        double d3 = distance(p, e3);
        double d4 = distance(p, e4);

        min1 = MIN(d1, d2);
        min2 = MIN(d3, d4);
        result = MIN(min1, min2);   

        free(e1->a); free(e1->b);
        free(e2->a); free(e2->b);
        free(e3->a); free(e3->b);
        free(e4->a); free(e4->b);
        free(e1); free(e2); free(e3); free(e4); 
        return result;
    } 
}
/* KTH_NEIGHBOR FUNCTIONS*/
/******************************************************************************************************/
void KTH_NEIGHBOR(int px, int py, int k) {
    point *p = (point *)malloc(sizeof(point));
    p->x = px;
    p->y = py;

    queue *result = neighbor_helper(q, p, k);
    if(result != NULL) printf("THE KTH NEAREST NEIGHBOR IS %s\n", result->e->name);
    else printf("NULL\n"); 
    free(p);
}

/* POLYGON FUNCTIONS*/
/******************************************************************************************************/
void intersect_edges(quad *q, point *p, list **result);
void print_edge_list(list *node);
double area(int x1, int y1, int x2, int y2, int x3, int y3);
int within_polygon(point *p, point *a, point *b, point *c);
int compare_points(point *p1, point *p2);
double distance_points(point *a, point *b);

void print_edge_list(list *node) {
    list *current = node;
    while(current != NULL) {
        printf("edge %s: [%f, %f], [%f, %f]\n", current->e->name, current->e->a->x, 
            current->e->a->y, current->e->b->x, current->e->b->y);
        current = current->next;
    }
}

double distance_points(point *a, point *b) {
    double x1 = a->x, y1 = a->y;
    double x2 = b->x, y2 = b->y;
    return ((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1));
}

void FIND_POLYGON(int px, int py) {
    double result = 0.0;
    point *p = (point *)malloc(sizeof(point));
    p->x = px;
    p->y = py;

    edge *e = neighbor_helper(q, p, 1)->e;
    point *a = e->a;
    point *b = e->b;
    point *c, *c_dup, *c_final;
    double c_closest = 0.0;


    list *l1 = (list*)malloc(sizeof(list));
    l1 = NULL;
    list *l2 = (list*)malloc(sizeof(list));
    l2 = NULL;

    intersect_edges(q, a, &l1);
    intersect_edges(q, b, &l2);

    int flag = 0;
    list *ptr1 = l1, *ptr2 = l2;
    while(ptr1 != NULL) {
        if(compare_points(ptr1->e->a, a) == 1) {
            c = ptr1->e->b;
        } else {
            c = ptr1->e->a;
        }
        while(ptr2 != NULL) {
            if(compare_points(ptr2->e->a, b) == 1) {
                c_dup = ptr2->e->b;
            } else {
                c_dup = ptr2->e->a;
            }
            if(compare_points(c, c_dup) == 1 && within_polygon(p, a, b, c) == 1 ) {
                if(c_closest == 0.0) {
                    c_closest = distance_points(p, c);
                }
                if(distance_points(p, c) <= c_closest)
                    result = area(a->x, a->y, b->x, b->y, c->x, c->y);
            }
            ptr2 = ptr2->next;
        }
        ptr2 = l2;
        ptr1 = ptr1->next;
    } 
    if(result != 0.0) {
        printf("POLYGON FOUND: %d/2 PIXELS\n", (int) result);
    } else {
        printf("NO ENCLOSING POLYGON\n");
    }

    free(l1);
    free(l2);
    free(p);
}

int compare_points(point *p1, point *p2) {
    if(p1->x == p2->x && p1->y == p2->y) {
        return 1;
    }
    return 0;
}
// find all of the  edges that intersect point p
void intersect_edges(quad *q, point *p, list **result) {
    if(q->type == 'G') {
        for(int i = 0; i < 4; i ++) {
            if(within_quad(q->child[i], p) == 1) {
                intersect_edges(q->child[i], p, result);
            }
        }   
    } else {
        list *current = q->edges;
        while(current != NULL) {
            if(((current->e->a->x == p->x && current->e->a->y == p->y)
            || (current->e->b->x == p->x && current->e->b->y == p->y))
            && contains(*result, current->e) == 0) {
                append(result, current->e);
            }
            current = current->next;
        }
    }
}


double area(int x1, int y1, int x2, int y2, int x3, int y3) { 
   return abs((x1*(y2-y3) + x2*(y3-y1)+ x3*(y1-y2))); 
} 

int within_polygon(point *p, point *a, point *b, point *c) {
    double x1 = a->x, y1 = a->y;
    double x2 = b->x, y2 = b->y;
    double x3 = c->x, y3 = c->y;
    double x = p->x, y = p->y;

   double A = area (x1, y1, x2, y2, x3, y3);  
   double A1 = area (x, y, x2, y2, x3, y3); 
   double A2 = area (x1, y1, x, y, x3, y3);  
   double A3 = area (x1, y1, x2, y2, x, y); 
    
   /* Check if sum of A1, A2 and A3 is same as A */ 
   if (A == A1 + A2 + A3) {
       return 1;
   } else return 0; 
}


/* DISPLAY FUNCTIONS*/
/******************************************************************************************************/
void print_edges(bst *node) {
    if(node == NULL)
        return;

    print_edges(node->left);
    print_edges(node->right);
    if(node->in_quad == 1) {
        printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", 
        node->e->a->x, node->e->a->y, node->e->b->x, node->e->b->y);
    }
}

void print_quad(quad *q) {
    if (q != NULL) {
        printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", q->A.x, q->A.y, q->A.x, q->B.y);
        printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", q->A.x, q->A.y, q->B.x, q->A.y);
        printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", q->A.x, q->B.y, q->B.x, q->B.y);
        printf("DL(%.2lf,%.2lf,%.2lf,%.2lf)\n", q->B.x, q->A.y, q->B.x, q->B.y);
    }
    if(q->type == 'G') {
        print_quad(q->child[0]);
        print_quad(q->child[1]);
        print_quad(q->child[2]);
        print_quad(q->child[3]);
    } 
}

void DISPLAY() {
    printf("$$$$ SP(%.2lf,%.2lf)\n", q->B.x, q->B.y);
    printf("LD(%d,%d)\n", 1, 0);
    print_edges(dictionary);
    //printf("finish printing the edges\n");
    printf("LD(%d,%d)\n", 1, 1);
    print_quad(q);
    printf("EP\n");
}

// 


//./pm > output_display.txt
//./printquad output_display.txt > output_display.ps

// ./pm < input-42-3.txt > output_display.txt
// ./pm < testing_big.txt > debug.txt