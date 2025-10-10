#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long i64;

typedef struct Node {
    int key;
    int val;
    struct Node* next;
} Node;

#define HSIZE 8192

static Node* H[HSIZE];

static unsigned hash_int(int x){ 
    unsigned u = (unsigned)x;
    u ^= u >> 16; u *= 0x7feb352d; u ^= u >> 15; u *= 0x846ca68b; u ^= u >> 16;
    return u & (HSIZE-1);
}

static void hclear(){
    for(int i=0;i<HSIZE;i++){
        Node* p = H[i];
        while(p){ Node* q=p->next; free(p); p=q; }
        H[i]=NULL;
    }
}

static int hget(int k){
    Node* p = H[hash_int(k)];
    while(p){ if(p->key==k) return p->val; p=p->next; }
    return 0;
}

static void hadd(int k, int delta){
    unsigned b = hash_int(k);
    Node* p = H[b];
    while(p){
        if(p->key==k){ p->val += delta; return; }
        p=p->next;
    }
    Node* n = (Node*)malloc(sizeof(Node));
    n->key=k; n->val=delta; n->next=H[b]; H[b]=n;
}

static void hdec(int k){
    unsigned b = hash_int(k);
    Node* p = H[b], *prev=NULL;
    while(p){
        if(p->key==k){
            if(--p->val==0){
                if(prev) prev->next = p->next; else H[b]=p->next;
                free(p);
            }
            return;
        }
        prev=p; p=p->next;
    }
}

long long conta_ternos(const int *a, int n, long long *comparacoes){
    *comparacoes = 0;
    if(n<3) return 0;

    long long total = 0;
    hclear();

    hadd(a[0], 1);
    hadd(a[1], 1);

    for(int k=2; k<n; k++){
        for(int i=0; i<k; i++){
            hdec(a[i]);
            int need = a[k] - a[i];
            (*comparacoes)++;
            total += hget(need);
        }
        for(int i=0; i<=k; i++) hadd(a[i], 1);
    }

    hclear();
    return total;
}

int main(){
    int a[] = {1, 3, 2, 5, 4, 7};
    int n = sizeof(a)/sizeof(a[0]);

    long long comps=0;
    long long ternos = conta_ternos(a, n, &comps);
    printf("Ternos = %lld\nComparacoes = %lld\n", ternos, comps);
    return 0;
}
