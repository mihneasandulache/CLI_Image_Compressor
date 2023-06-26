/*SÂNDULACHE Mihnea-Ștefan - 312CC*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct quadtree {
    struct quadtree *top_left;
    struct quadtree *top_right;
    struct quadtree *bottom_right;
    struct quadtree *bottom_left;
    unsigned char r, g, b;
    unsigned char node_type;
}quadtree;
/*declaram structura aferenta unui nod din arborele cuaternar*/

typedef struct pixel {
    unsigned char r, g, b;
} pixel;
/*declaram structura aferenta unui pixel din imagine*/

typedef struct queue {
    quadtree *node;
    struct queue *next;
}queue;
/*declaram structura aferenta cozii pentru parcurgerea BFS*/

void init(quadtree **root) {
    (*root)->top_left = NULL;
    (*root)->top_right = NULL;
    (*root)->bottom_right = NULL;
    (*root)->bottom_left = NULL;
    (*root)->r = (*root)->g = (*root)->b = 0;
    (*root)->node_type = 0;
}
/*functie pentru initializarea nodurilor din arbore*/

void add_queue(queue **front, queue **rear, quadtree *node) {
    if(node == NULL) {
        return;
    }
    queue *newnode = (queue *) malloc(sizeof(queue));
    newnode->node = node;
    newnode->next = NULL;
    if ((*rear) == NULL) {
        *front = newnode;
        *rear = newnode;
    } else {
        (*rear)->next = newnode;
        *rear = newnode;
    }
}
/*functie pentru adaugarea unui nod in coada pentru parcurgerea BFS*/

void del_queue(queue **front, queue **rear) {
    if(!(*front) && !(*rear)) {
        return;
    }
    /*daca coada este vida se iese din functie*/
    queue *aux = *front;
    *front = (*front)->next;
    free(aux);
    /*se elibereaza memoria pentru nodul eliminat*/
}

void quadtree_construction(quadtree **root, pixel **image, unsigned int factor,
                           unsigned int *leaves, unsigned int *max_side,
                           unsigned int width,unsigned int height,
                           unsigned int x, unsigned int y) {
    unsigned int i, j;
    unsigned long long mean = 0, red = 0, green = 0, blue = 0;
    for (i = x; i < x + height; i++) {
        for (j = y; j < y + width; j++) {
            red = red + (unsigned long long)image[i][j].r;
            green = green + (unsigned long long)image[i][j].g;
            blue = blue + (unsigned long long)image[i][j].b;
            /*se calculeaza sumele pentru media fiecarei culori in subimaginea
            curenta*/
        }
    }
    red = red / (width * height);
    green = green / (width * height);
    blue = blue / (width * height);
    for(i = x; i < x + height; i++) {
        for(j = y; j < y + width; j++) {
            mean = mean + (red - image[i][j].r) * (red - image[i][j].r) +
                   (green - image[i][j].g) * (green - image[i][j].g) +
                   (blue - image[i][j].b) * (blue - image[i][j].b);
                   /*se calculeaza valoarea scorului ce va fi comparat
                   cu factorul furnizat pentru a determina descompunerea
                   imaginii in blocuri*/
        }
    }
    mean = mean / (3 * width * height);
    if (mean > factor) {
        quadtree *top_left = (quadtree *) malloc(sizeof(quadtree));
        init(&top_left);
        (*root)->top_left = top_left;
        quadtree_construction(&top_left, image, factor, leaves, max_side,
                              width / 2, height / 2, x, y);
        quadtree *top_right = (quadtree *) malloc(sizeof(quadtree));
        init(&top_right);
        (*root)->top_right = top_right;
        quadtree_construction(&top_right, image, factor, leaves, max_side,
                              width / 2, height / 2, x, y + width / 2);
        quadtree *bottom_right = (quadtree *) malloc(sizeof(quadtree));
        init(&bottom_right);
        (*root)->bottom_right = bottom_right;
        quadtree_construction(&bottom_right, image, factor, leaves, max_side,
                              width / 2, height / 2, x + height / 2,
                              y + width / 2);
        quadtree *bottom_left = (quadtree *) malloc(sizeof(quadtree));
        init(&bottom_left);
        (*root)->bottom_left = bottom_left;
        quadtree_construction(&bottom_left, image, factor, leaves, max_side,
                              width / 2, height / 2, x + height / 2, y);
        /*in cazul in care scorul este mai mare decat factorul furnizat
        divizam in continuarea subimaginea curenta in alte blocuri,
        corespunzatoare cate unui nod din arbore, pe care le initilizam*/
    } else {
        /*in cazul in care scorul calculat este mai mic sau egal decat
        pragul impus, nu mai este nevoie de divizarea subimaginii*/
        if(width > *max_side) {
            *max_side = width;
        }
        /*calculam blocul cu lungimea laturii maxima*/
        *leaves = *leaves + 1;
        /*calculam numarul de blocuri care nu au necesitat divizari
        suplimentare, echivalent cu numarul de frunze ale arborelui*/
        (*root)->r = red;
        (*root)->g = green;
        (*root)->b = blue;
        (*root)->node_type = 1;
        /*setam valorile R, G, B cu mediile calculate anterior*/
    }
}

int quadtree_height(quadtree *root) {
    if(root == NULL) {
        return 0;
    }
    int height = 0;
    int height_top_left = quadtree_height(root->top_left);
    int height_top_right = quadtree_height(root->top_right);
    int height_bottom_right = quadtree_height(root->bottom_right);
    int height_bottom_left = quadtree_height(root->bottom_left);
    if(height_top_left > height_top_right) {
        height = height_top_left;
    } else {
        height = height_top_right;
    }
    if(height_bottom_right > height_bottom_left) {
        if(height_bottom_right > height) {
            height = height_bottom_right;
        }
    } else {
        if(height_bottom_left > height) {
            height = height_bottom_left;
        }
    }
    return height + 1;
    /*functie recursiva pentru calcularea inaltimii arborelui*/
}

void bfs(quadtree *root, FILE *g) {
    queue *front = NULL;
    queue *rear = NULL;
    add_queue(&front, &rear, root);
    while(front) {
        fwrite(&front->node->node_type, sizeof(unsigned char), 1, g);
        if(front->node->node_type == 1) {
            fwrite(&front->node->r, sizeof(unsigned char), 1, g);
            fwrite(&front->node->g, sizeof(unsigned char), 1, g);
            fwrite(&front->node->b, sizeof(unsigned char), 1, g);
        } else {
            add_queue(&front, &rear, front->node->top_left);
            add_queue(&front, &rear, front->node->top_right);
            add_queue(&front, &rear, front->node->bottom_right);
            add_queue(&front, &rear, front->node->bottom_left);
        }
        del_queue(&front, &rear);
    }
    /*parcurgere in latime a arborelui pentru a crea fisierul comprimat
    corespunzator imaginii*/
}

void quadtree_reconstruction(quadtree **root, FILE *f) {
    queue *front = NULL;
    queue *rear = NULL;
    add_queue(&front, &rear, *root);
    while(front) {
        fread(&front->node->node_type, sizeof(unsigned char), 1, f);
        /*citim din fisierul comprimat datele despre nodul curent pentru
        a putea reconstrui arborele si a realiza decompresia imaginii*/
        if(front->node->node_type == 1) {
            fread(&front->node->r, sizeof(unsigned char), 1, f);
            fread(&front->node->g, sizeof(unsigned char), 1, f);
            fread(&front->node->b, sizeof(unsigned char), 1, f);
            /*daca intalnim un nod frunza, citim informatiile R, G, B si le
            stocam in nodul curent din arbore*/
        } else {
            quadtree *top_left = (quadtree *) malloc(sizeof(quadtree));
            init(&top_left);
            front->node->top_left = top_left;
            add_queue(&front, &rear, top_left);
            quadtree *top_right = (quadtree *) malloc(sizeof(quadtree));
            init(&top_right);
            front->node->top_right = top_right;
            add_queue(&front, &rear, top_right);
            quadtree *bottom_right = (quadtree *) malloc(sizeof(quadtree));
            init(&bottom_right);
            front->node->bottom_right = bottom_right;
            add_queue(&front, &rear, bottom_right);
            quadtree *bottom_left = (quadtree *) malloc(sizeof(quadtree));
            init(&bottom_left);
            front->node->bottom_left = bottom_left;
            add_queue(&front, &rear, bottom_left);
            /*in cazul in care intalnim un nod neterminal, il adaugam in coada,
            deoarece reconstructia arborelui se realizeaza tot pe nivel pentru
            a putea ajunge la arborele initial*/
        }
        del_queue(&front, &rear);
    }
}

void image_reconstruction(quadtree *root, pixel **image, unsigned int width,
                          unsigned int height, unsigned int x,
                          unsigned int y) {
    if(root->node_type == 1) {
        unsigned int i, j;
        for(i = x; i < x + height; i++) {
            for(j = y; j < y + width; j++) {
                image[i][j].r = root->r;
                image[i][j].g = root->g;
                image[i][j].b = root->b;
            }
        }
        /*parcurgem arborele pentru a reconstrui imaginea ,astfel ca
        in cazul in care intalnim un nod frunza, umplem subimaginea
        curenta cu valorile R, G, B stocate in functia anterioara*/
    } else {
        image_reconstruction(root->top_left, image, width / 2,
                             height / 2, x, y);
        image_reconstruction(root->top_right, image, width / 2, height / 2,
                             x, y + width / 2);
        image_reconstruction(root->bottom_right, image, width / 2, height / 2,
                             x + height / 2, y + width / 2);
        image_reconstruction(root->bottom_left, image, width / 2, height / 2,
                             x + height / 2, y);
        /*in cazul in care intalnim un nod neterminal, continuam divizarea si 
        parcurgerea arborelui pana la intalnirea unui nod frunza*/
    }
}

void free_image(pixel **image, unsigned int width) {
    unsigned int i;
    for(i = 0; i < width; i++) {
        free(image[i]);
    }
    free(image);
}
/*functie pentru eliberarea memoriei imaginii*/

void free_quadtree(quadtree **root) {
    if(*root) {
        free_quadtree(&(*root)->top_left);
        free_quadtree(&(*root)->top_right);
        free_quadtree(&(*root)->bottom_right);
        free_quadtree(&(*root)->bottom_left);
        free(*root);
    }
}
/*functie pentru eliberarea memoriei arborelui*/

int main(int argc, char **argv) {
    unsigned int width, height, max_value, max_side = 0, leaves = 0, factor;
    char file_format[3];
    if(argc <= 1) {
        printf("Usage: ./tema2 [-c1 / -c2 factor] / -d input_file output_file");
        return 0;
    }
    /*in cazul in care numarul de argumente este mai mic decat 1, se
    afiseaza un mesaj cu privire la modul de folosire al programului*/
    if (strcmp(argv[1], "-c1") == 0 || strcmp(argv[1], "-c2") == 0) {
        FILE *f;
        f = fopen(argv[3], "rb");
        /*se deschide fisierul de citire pentru primele doua cerinte*/
        factor = atoi(argv[2]);
        /*se converteste factorul citit din linia de comanda*/
        fscanf(f, "%s", file_format);
        fscanf(f, "%u %u", &width, &height);
        fscanf(f, "%u", &max_value);
        /*citim informatiile initiale*/
        fseek(f, 1, SEEK_CUR);
        /*ne situam la inceputul urmatoarei linii*/
        quadtree *root = (quadtree *) malloc(sizeof(quadtree));
        init(&root);
        /*initializam radacina arborelui*/
        pixel **image = (pixel **)malloc(width * sizeof(pixel *));
        unsigned int i;
        for(i = 0; i < width; i++) {
            image[i] = (pixel *)malloc(height * sizeof(pixel));
        }
        /*alocam memorie pentru imagine*/
        unsigned int j;
        for(i = 0; i < width; i++) {
            for(j = 0; j < height; j++) {
                fread(&image[i][j].r, sizeof(unsigned char), 1, f);
                fread(&image[i][j].g, sizeof(unsigned char), 1, f);
                fread(&image[i][j].b, sizeof(unsigned char), 1, f);
            }
        }
        /*citim valorile R, G, B ale fiecarui pixel*/
        quadtree_construction(&root, image, factor, &leaves, &max_side, width,
                              height, 0, 0);
        if(strcmp(argv[1], "-c1") == 0) {
            FILE *g;
            g = fopen(argv[4], "w");
            fprintf(g, "%d\n", quadtree_height(root));
            fprintf(g, "%u\n", leaves);
            fprintf(g, "%u\n", max_side);
            fclose(g);
        } else {
            if(strcmp(argv[1], "-c2") == 0) {
                FILE *g;
                g = fopen(argv[4], "wb");
                fwrite(&width, sizeof(unsigned int), 1, g);
                bfs(root, g);
                fclose(g);
            } 
        }
        fclose(f);
        free_quadtree(&root);
        free_image(image, width); 
    } else {
        FILE *f;
        f = fopen(argv[2], "rb");
        FILE *h = fopen(argv[3], "wb");
        /*deschidem fisierele pentru a treia cerinta*/
        quadtree *root = (quadtree *) malloc(sizeof(quadtree));
        init(&root);
        fread(&width, sizeof(unsigned int), 1, f);
        pixel **image = (pixel **)malloc(width * sizeof(pixel *));
        unsigned int i;
        for(i = 0; i < width; i++) {
            image[i] = (pixel *)malloc(width * sizeof(pixel));
        }
        fprintf(h, "P6\n");
        fprintf(h, "%u %u\n", width, width);
        max_value = 255;
        fprintf(h, "%u\n", max_value);
        quadtree_reconstruction(&root, f);
        image_reconstruction(root, image, width, width, 0, 0);
        unsigned int j;
        for(i = 0; i < width; i++) {
            for(j = 0; j < width; j++) {
                fwrite(&image[i][j].r, sizeof(unsigned char), 1, h);
                fwrite(&image[i][j].g, sizeof(unsigned char), 1, h);
                fwrite(&image[i][j].b, sizeof(unsigned char), 1, h);
            }
        }
        free_quadtree(&root);
        free_image(image, width);
        fclose(f);
        fclose(h);
    }

   return 0;

}