/*
 * BADSTÜBER Elian & FOCHEUX Vital, étudiants de l'UFR-ST L2 Info.
 * Projet finalisé le 15/11/2022 à 10h58
 */

#include "big.h"

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BASE 0X100000000


void bign_create_empty(struct bign *self) {
  self->data = NULL;
  self->capacity = 0;
  self->size = 0;
}


void bign_create_from_value(struct bign *self, uint32_t val) {
  self->capacity = 1;
  self->size = 1;
  self->data = malloc(self->capacity * sizeof(uint32_t));
  self->data[0] = val;
}

size_t str_length(const char *str)
{
  if (str == NULL)
  { // vérifie que str est un pointeur NULL ou non
    return 0;
  }
  size_t i = 0;
  while (str[i] != '\0')
  {
    ++i;
  }
  return i;
}

int char_to_int(char str){
  if((str >= '0')&&(str <='9')){
    return (int)(str - '0');
  }
  if((str >= 'a')&&(str <= 'f')){
    return (int)(str - 'a' + 10);
  }
  return (int)(str - 'A' + 10);
}

size_t exponentiation_rapide(size_t base, size_t puissance){
  size_t res = 1;
  while(puissance > 0){
    if(puissance%2 == 1){
      res *= base;
    }
    base *= base;
    puissance /= 2;
  }
  return res;
}

void bign_create_from_string(struct bign *self, const char *str) {
  self->size = 0;
  size_t str_len = str_length(str); //on récupère la taille de str pour plus tard
  size_t res = 0;
  if(str_len <= 8){                 //si le str est composé de 8 chiffres héxadecimal c'est à dire un chiffre en big
    self->capacity = 1;
    self->data = malloc(self->capacity * sizeof(uint32_t));
    for(size_t i = 0; i < str_len; ++i){
      res += exponentiation_rapide(16,i) * char_to_int(str[str_len - 1 - i]); //on calcule le chiffre
    }
    self->data[0] = res;
    ++self->size;
  }else{
    self->capacity = str_len / 8; //la taille de str doit etre un multiple de 8 pour obtenir la capacité sinon on augmente la capacité de 1
    if(str_len % 8 != 0){
      ++self->capacity;
    }
    self->data = malloc(self->capacity * sizeof(uint32_t));
    for(size_t i = 0; i < self->capacity; ++i){ //on va effectuer une boucle pour chaque groupe de 8 héxadecimal
      size_t j = 0;
      res = 0;
      while((j < 8)&&(((int) (str_len - j - 1 - i * 8)) != -1)){ //on va calculer chaque chiffre avec les groupes de 8 héxadecimal
        res += char_to_int(str[str_len - j - 1 - i * 8]) * exponentiation_rapide(16, j);
        ++j;
      }
      self->data[i] = res;
      ++self->size;
    }
    while(self->size > 0){  //on va effectuer une boucle pour normaliser le nombre où on va décrémenter le self->size de 1
      if(self->data[self->size-1] == 0){
        --self->size;
      }
      return;
    }
  }
}

void bign_copy_from_other(struct bign *self, const struct bign *other) {
  //si self n'est pas vide alors on va détruitre le bign
  if(self->capacity != 0){
    bign_destroy(self);
  }
  //on va faire que la capacité et la taille de self soit la même que other et copier le tableau de other dans self indice par indice
  self->capacity = other->capacity;
  self->size = other->size;
  self->data = malloc(self->capacity * sizeof(uint32_t));
  for(size_t i = 0; i < self->size; ++i){
    self->data[i] = other->data[i];
  }
}

void bign_move_from_other(struct bign *self, struct bign *other) {
  //si self n'est pas vide alors on va détruitre le bign
  if(self->capacity != 0){
    bign_destroy(self);
  }
  //on va faire que la taille, la capacité et l'adresse de data soit la même que other
  self->data = other->data;
  self->size = other->size;
  self->capacity = other->capacity;
  //on va mettre l'adresse de other à nul pour qu'il n'est plus accès à l'adresse du data et mettre sa capacité et sa taille à 0
  other->data = NULL;
  other->size = 0;
  other->capacity = 0;
}

void bign_destroy(struct bign *self) {
  //on libère le tableau et met son adresse à null et la taille et capacité de self à 0
  free(self->data);
  self->data = NULL;
  self->size = 0;
  self->capacity = 0;
}

void bign_print(const struct bign *self) {
  if((self->capacity == 1)&&(self->data[0] == 0)){ //si le nombre est égal à 0 on affiche 0
    printf("0");
  }else{                                            //sinon on effectue une division euclidienne pour recupérer la valeur de chaque héxadecimal
    for(size_t i = self->size; i > 0; --i){
      uint32_t res = self->data[i-1];
      for(int j = 7; j > -1; --j){
          size_t p = exponentiation_rapide(16,j);
          int value = res / p;
          if((value >= 10)&&(value < 16)){
            printf("%c",(char)(value) - 10 + 'A');
          }else{
            printf("%c",(char)(value) + '0');
          }
          res = res%p;
      }
    }
  }
}

char int_to_char(int value){
  if((value >= 10)&&(value < 16)){
    return (char)(value - 10) + 'A';
  }
  return (char)(value + '0');
}

int bign_cmp(const struct bign *lhs, const struct bign *rhs) {
  if(lhs->size > rhs->size){
    return 1;
  }
  if(lhs->size < rhs->size){
    return -1;
  }
  for(size_t i = 0; i < lhs->size; ++i){
    if(lhs->data[lhs->size - i - 1] > rhs->data[rhs->size - i - 1]){
      return 1;
    }
    if(lhs->data[lhs->size - i - 1] < rhs->data[rhs->size - i - 1]){
      return -1;
    }
  }
  return 0;
}

int bign_cmp_zero(const struct bign *self) {
  if(self->size > 1){
    return 1;
  }
  if(self->data[0] > 0){
    return 1;
  }
  return 0;
}

static uint32_t bign_get(const struct bign *self, size_t i){
  if(i >= self->size){
    return 0;
  }
  return self->data[i];
}

static void bign_normalize(struct bign *self){
  uint32_t i = self->size - 1;
  uint32_t selftmp = self->size;
  while((i > 0)&&(self->data[i] == 0)){ //on va calculer la taille que doit être self pour être normaliser
    --selftmp;
    --i;
  }
  uint32_t *datatemp = malloc(selftmp * sizeof(uint32_t)); //ensuite on alloue un tableau de la taille qu'on a calculer avant
  for(uint32_t i = 0; i < selftmp; ++i){                   //puis on va copier les éléments de self->data dans le nouveau tableau jusqu'à cet indice
    datatemp[i] = self->data[i];
  }
  free(self->data);             //on peut ensuite libérer le tableau de self et mettre à jour les informations de selmf avec celles qu'on a calculer auparavant
  self->data = datatemp;
  self->size = selftmp;
  self->capacity = selftmp;
}

void bign_add(struct bign *self, const struct bign *lhs, const struct bign *rhs) {
  //on va allouer un bign temporaire pour calculer lhs + rhs au cas où self est égal à lhs ou rhs
  struct bign *res = malloc(sizeof(struct bign));
  bign_create_empty(res);
  
  //on initialise la taille maximale en fonction de la taille de lhs et rhs
  size_t max = lhs->size;
  if (rhs->size > max) {
    max = rhs->size;
  }
  
  //on va allouer un tableau de taille max+1 au bign temporaire et sa taille et sa capacité sera aussi egal à max+1
  res->data = calloc(max+1, sizeof(uint32_t));
  res->capacity = max+1;
  res->size = max+1;
  
  uint64_t carry = 0;
  uint64_t somme = 0;
  for (size_t i = 0; i < max; ++i) {
    somme = (uint64_t)bign_get(lhs,i) + (uint64_t) bign_get(rhs,i) + carry; //on calcule la somme de chaque chiffre
    res->data[i] = somme % BASE;                     //ensuite on va effectuer un modulo pour récupérer un chiffre
    carry = somme / BASE;                            //enfin on va effectuer une division entière pour savoir si on a une retenue
  }
  res->data[max] = carry;                            //on ajoute la dernière retenue à la fin

  //ensuite on va normaliser res, puis le copier dans self pour enfin le détruire et libérer sa place mémoire
  bign_normalize(res);
  bign_copy_from_other(self, res);
  bign_destroy(res);
  free(res);
}

void bign_soustraction(struct bign *res, const struct bign *sub){
  uint32_t i = 0;
  while(i < sub->size){                       //on va effectuer une soustraction entre chaque chiffre de res et rhs
    if(res->data[i] < sub->data[i]){          //si le chiffre de res est plus petit que celui de rhs
      if(res->data[i+1] == 0){                //si le chiffre suivant de res est un 0
        uint32_t j = i+1;
        while((j < res->size)&&(res->data[j] == 0)){  //tant que si le chiffre suivant de i de res est un 0 alors on va effectuer la soustraction de la retenue à chaque fois
          --res->data[j];
          ++j;
        }
        --res->data[j];
      }else{                                  //sinon on va décrémenter le chiffre suivant de res pour la retenue
        --res->data[i+1];
      }
    }
    res->data[i] -= sub->data[i];             //on va effectuer la soustraction entre le chiffre de res et rhs
    ++i;
  }
}

void bign_sub(struct bign *self, const struct bign *lhs, const struct bign *rhs){
  //on va allouer un bign temporaire pour calculer lhs - rhs au cas où self est égal à lhs ou rhs
  struct bign *res = malloc(sizeof(struct bign));
  bign_create_empty(res);
  if((lhs->size > rhs->size)||(bign_cmp(lhs, rhs) >= 0)){ //si lhs est plus grand que rhs alors on va copier lhs dans res et effectuer la soustraction
    bign_copy_from_other(res, lhs);
    bign_soustraction(res, rhs);
  }else{                                                  //sinon on va copier rhs dans res pour avoir une soustraction avec un resultat positif
    bign_copy_from_other(res, rhs);
    bign_soustraction(res, lhs);
  }
  //on va copier res dans self, puis détruire et libérer la place mémoire de res et enfin normaliser self
  bign_copy_from_other(self, res);
  bign_destroy(res);
  free(res);
  bign_normalize(self);
}

void bign_mul(struct bign *self, const struct bign *lhs, const struct bign *rhs) {
  //on va allouer un bign temporaire pour calculer lhs * rhs au cas où self est égal à lhs ou rhs
  struct bign *res = malloc(sizeof(struct bign));
  bign_create_empty(res);
  
  //on va allouer un tableau d'une capacité de la somme de la capacité de lhs et rhs et pour la taille et la capacité c'est aussi égal à la somme de celle de lhs et rhs
  res->data = calloc((lhs->capacity + rhs->capacity), sizeof(uint32_t));
  res->capacity = lhs->capacity + rhs->capacity;
  res->size = lhs->size + rhs->size;
  
  uint64_t operande1;
  uint64_t operande2;
  uint64_t res_temp;
  uint64_t carry;
  for (size_t i = 0; i < lhs->size; ++i) {
    carry = 0;
    for (size_t j = 0; j < rhs->size; ++j) {
      operande1 = lhs->data[i];                                   //on récupère le chiffre de lhs
      operande2 = rhs->data[j];                                   //on récupère le chiffre de rhs
      res_temp = operande1 * operande2 + res->data[i+j] + carry;  //on calcule le nombre qui va être égal
      res->data[i+j] = res_temp % 0x100000000;                    //si avec le calcul on a eu un nombre alors on va effectuer un modulo pour avoir un chiffre
      carry = res_temp / 0x100000000;                             //on va aussi calculer si on a besoin d'une retenue avec une division
    }
    if (carry > 0) {                                              //si on finis avec une retenue alors on l'ajoute à la dernière place du tableau
      res->data[i + rhs->size] = carry;
    }
  }
  
  //on va normaliser res, puis le copier res dans self pour enfin le détruire et libérer sa place mémoire
  bign_normalize(res);
  bign_copy_from_other(self, res);
  bign_destroy(res);
  free(res);
}

/*
 * la fonction partition permet de créer deux partition de tab de l'index 0 à index et de index
   à size
 */
void partition(uint32_t *tab, size_t size, size_t index, uint32_t *part1, uint32_t *part2){
  size_t i = 0;
  size_t j = 0;
  while(i < index){                   //on effectuer une boucle jusqu'à index - 1 de la première partie de tab pour la mettre dans le tableau part1
    part1[i] = tab[i];
    ++i;
  }
  while(i < size){                    //on effectuer une boucle de index jusqu'à size - 1 de tab pour la mettre dans le tableau part2
    part2[j] = tab[i];
    ++i;
    ++j;
  }
}

void bign_mul_karatsuba(struct bign *self, const struct bign *lhs, const struct bign *rhs) {
  /* 
   * Nous avons décidé de travailler directement avec des bign.
   * Après différents échanges, peut-être aurait-il été plus simple 
     de travailler avec des uint32_t*.
   * En effet l'un des problèmes de l'usage direct des bign est l'allocation
     et la libération de beaucoup de mémoire. Ce qui demande du temps et de l'espace.
   */

  // Dans le cas où l'un des deux nombres est inférieur à BASE. On  utilise bign_mul
  if ((lhs->size == 1) || (rhs->size == 1)){
    bign_mul(self,rhs,lhs);
  }
  else {
    size_t min_len = lhs->size;
    if (rhs->size < min_len) {
      min_len = rhs->size;
    }
    size_t k = min_len / 2;

    struct bign *res = malloc(sizeof(struct bign));
    bign_create_empty(res);

    res->data = calloc(lhs->size+rhs->size+1, sizeof(uint32_t));
    res->capacity = lhs->size + rhs->size + 1;
    res->size = lhs->size + rhs->size;

    // On crée les différentes parties de nos nombres.
    struct bign *a0 = malloc(sizeof(struct bign)); // grand lhs
    bign_create_empty(a0);

    a0->data = calloc(lhs->size-k+1, sizeof(uint32_t));
    a0->capacity = lhs->size - k + 1;
    a0->size = lhs->size - k;

    struct bign *a1 = malloc(sizeof(struct bign)); // petit lhs
    bign_create_empty(a1);

    a1->data = calloc(k+1, sizeof(uint32_t));
    a1->capacity = k + 1;
    a1->size = k;


    struct bign *b0 = malloc(sizeof(struct bign)); // grand rhs
    bign_create_empty(b0);

    b0->data = calloc(rhs->size-k+1, sizeof(uint32_t));
    b0->capacity = rhs->size - k + 1;
    b0->size = rhs->size - k;

    struct bign *b1 = malloc(sizeof(struct bign)); // petit rhs
    bign_create_empty(b1);

    b1->data = calloc(k+1, sizeof(uint32_t));
    b1->capacity = k + 1;
    b1->size = k;
    
    // On initialise ces parties à l'aide de la fonction partition
    partition(lhs->data,lhs->size,k,a1->data,a0->data);
    partition(rhs->data,rhs->size,k,b1->data,b0->data);
    
    //S'en suivent les produits que l'on calcule récursivement à l'aide de bign_mul_karatsuba
    struct bign *a0b0 = malloc(sizeof(struct bign)); // a0 * b0
    bign_create_empty(a0b0);
    struct bign *a1b1 = malloc(sizeof(struct bign)); // a1 * b1
    bign_create_empty(a1b1);
    
    bign_mul_karatsuba(a0b0,a0,b0);
    bign_mul_karatsuba(a1b1,a1,b1);
    
    //Puis les additions à l'aide de bign_add
    struct bign *a0a1 = malloc(sizeof(struct bign)); // a0 + a1
    bign_create_empty(a0a1);
    struct bign *b0b1 = malloc(sizeof(struct bign)); // b0 + b1 
    bign_create_empty(b0b1);
    
    bign_add(a0a1,a0,a1);
    bign_add(b0b1,b0,b1);
    
    // Et le produit des seux sommes, encore karatsuba
    struct bign *a0a1b0b1 = malloc(sizeof(struct bign)); // (a0 + a1)(b0 + b1)
    bign_create_empty(a0a1b0b1);
    
    bign_mul_karatsuba(a0a1b0b1,a0a1,b0b1);
    
    // on doit maintenant effectuer le calcul suivant :
    // a0b0 * base^2k + (a0a1b0b1 - a0b0 - a1b1) * base^k + a1b1
    // avec base = UINT32_MAX+1 = BASE
    
    struct bign *basek = malloc(sizeof(struct bign)); // 0x1 suivi de k 00000000
    basek->data = calloc(k+1,sizeof(uint32_t)); // 1,0
    basek->data[k] = 1;
    basek->capacity = k+1;
    basek->size = k+1;
    
    struct bign *base2k = malloc(sizeof(struct bign)); // 0x1 suivi de 2k 00000000
    base2k->data = calloc(2*k+1,sizeof(uint32_t)); // 1,0,0
    base2k->data[2*k] = 1;
    base2k->capacity = 2*k+1;
    base2k->size = 2*k+1;
    
    // part1 : a0b0 * base⁴
    struct bign *part1 = malloc(sizeof(struct bign));
    bign_create_empty(part1);
    bign_copy_from_other(part1,a0b0);
    bign_mul(part1,part1,base2k);
    
    // part2 : (a0a1b0b1 - a0b0 - a1b1) * base²
    struct bign *part2 = malloc(sizeof(struct bign));
    bign_create_empty(part2);
    bign_copy_from_other(part2,a0a1b0b1);
    bign_sub(part2,part2,a0b0);
    bign_sub(part2,part2,a1b1);
    bign_mul(part2,part2,basek);
    
    // On ajoute dans self
    bign_add(res,part1,part2);
    bign_add(res,res,a1b1);
    bign_normalize(res);
    bign_copy_from_other(self, res);
    
    
    // On clear tous les bign alloués dans la fonction.
    bign_destroy(res);      free(res);
    bign_destroy(a0);       free(a0);
    bign_destroy(a1);       free(a1);
    bign_destroy(b0);       free(b0);
    bign_destroy(b1);       free(b1);
    bign_destroy(a0b0);     free(a0b0);
    bign_destroy(a1b1);     free(a1b1);
    bign_destroy(a0a1);     free(a0a1);
    bign_destroy(b0b1);     free(b0b1);
    bign_destroy(a0a1b0b1); free(a0a1b0b1);
    bign_destroy(basek);    free(basek);
    bign_destroy(base2k);   free(base2k);
    bign_destroy(part1);    free(part1);
    bign_destroy(part2);    free(part2);
  }
}


void bign_mul_short(struct bign *self, const struct bign *lhs, uint32_t rhs) {
  //on va allouer un bign temporaire pour calculer lhs * rhs au cas où self est égal à lhs ou rhs
  struct bign *res = malloc(sizeof(struct bign));
  //on va créer le tableau à partir d'une valeur car rhs est un chiffre
  bign_create_from_value(res,rhs);
  
  //puis on effectuer la multiplication naïve
  bign_mul(self, res, lhs); 

  //enfin on va détruire et libérer la place mémoire de res
  bign_destroy(res);
  free(res);
}

// https://janmr.com/blog/2012/11/basic-multiple-precision-short-division/
void bign_div_short(struct bign *quo, uint32_t *rem, const struct bign *lhs, uint32_t rhs) {
  uint32_t r = 0;
  for(size_t i = 0 ; i < lhs->size; ++i){               //on va effectuer une boucle jusqu'à la taille de lhs - 1
    uint64_t t = (uint64_t)(r) * 0x100000000 + (uint64_t)(lhs->data[lhs->size - 1 - i]);  //on va calculer le nombre pour la division euclidienne
    quo->data[lhs->size - 1 - i] = (uint32_t)(t / rhs); //on va ensuite faire la division avec le rhs en castant en uint32_t pour qu'il aie le bon type
    r = (uint32_t)(t % rhs);                            //on va ensuite calculer le reste de cette division en castant uint32_t pour qu'il aie le bon type
  }
  rem[0] = r; //enfin on va mettre le reste dans la première case du tableau car le reste est un chiffre
}

/*
 * La fonction bign_copy_from_other_part permet de copier une partie d'un bign sans destroy self au préalable, ce qui permet de garder la capcité et la size actuelle de self (sauf si sa capacité est inférieur à end-start)
 */
void bign_copy_from_other_part(struct bign *self, const struct bign *other, size_t start, size_t end) {
  size_t size = end-start;
  if (self->capacity < size) { // On set self aux bonnes taille et capacité
    bign_destroy(self);
    self->data = malloc(sizeof(uint32_t)*size);
    self->capacity = size;
    self->size = size;
  }
  else if (self->size < size) { // On set self à la bonne taille
      self->size = size;
  }
  for (size_t i = start; i < end; ++i) {
    self->data[i-start] = bign_get(other,i);
  } 
}

/*
 * bign_create_from_uint64 permet d'initialiser un bign avec comme un uint64_t.
 * Si val < BASE on utilise bign_create_from_value. 
 */
void bign_create_from_uint64(struct bign *self, uint64_t val) {
  if (val < BASE) {
    bign_create_from_value(self,(uint32_t) val);
  }
  else {
    self->capacity = 2;
    self->size = 2;
    self->data = malloc(self->capacity * sizeof(uint32_t));
    self->data[1] = (uint32_t) (val / BASE);
    self->data[0] = (uint32_t) (val % BASE);
  }
}

void bign_div_knuth(struct bign *quo, struct bign *rem, const struct bign *lhs, const struct bign *rhs) {
  /*
   * Même commentaire que pour bign_mul_karatsuba sur l'usage des bign.
   */
  
  
  // on utilise un bin pour les différentes fois où
  // l'on appelle des fonctions (notamment div_short) et
  // dont l'une des parties ne nous intéresse pas.
  struct bign *bin = malloc(sizeof(struct bign));
  bign_create_empty(bin);
  bin->data = malloc((lhs->size+rhs->size)*sizeof(uint32_t));
  bin->capacity = lhs->size+rhs->size;
  bin->size = lhs->size+rhs->size;
  
  struct bign *res_quo = malloc(sizeof(struct bign));
  bign_create_empty(res_quo);
  res_quo->data = calloc(lhs->size, sizeof(uint32_t));
  res_quo->capacity = lhs->size;
  res_quo->size = lhs->size;
  
  struct bign *res_rem = malloc(sizeof(struct bign));
  bign_create_empty(res_rem);
  res_rem->data = calloc(lhs->size, sizeof(uint32_t));
  res_rem->capacity = lhs->size;
  res_rem->size = lhs->size;
  
  // u = lhs (copie) 
  struct bign *u = malloc(sizeof(struct bign));
  u->data = calloc(lhs->size+1, sizeof(uint32_t));
  u->capacity = lhs->size + 1;
  u->size = lhs->size;
  
  bign_copy_from_other_part(u,lhs,0,lhs->size);
  
  // v = rhs (copie)
  struct bign *v = malloc(sizeof(struct bign));
  bign_create_empty(v);
  bign_copy_from_other(v,rhs);
  
  // On crée w ici, et il sera calculé dans la boucle w = v *q_c
  struct bign *w = malloc(sizeof(struct bign));
  bign_create_empty(w);
  
  size_t m = u->size;
  size_t n = v->size;
  
  uint32_t d = BASE / ((uint64_t) v->data[v->size -1] + 1);
  
  bign_mul_short(v,v,d);
  
  size_t size_av_mul = u->size;
  bign_mul_short(u,u,d);
  size_t size_ap_mul = u->size;
  
  // Si la size n'a pas changé après la multiplication on ajoute 1 élément à u
  // (un zero à la fin de data)
  if (size_ap_mul == size_av_mul) {
    
    uint32_t *temp_data = calloc(u->size+1,sizeof(uint32_t));
    for (size_t i = 0; i < u->size; ++i) {
      temp_data[i] = bign_get(u,i);
    }
    temp_data[u->size] = 0;
    free(u->data);
    u->data = temp_data;
    u->size++;
    u->capacity++;
  }
  
  uint64_t q_c;
  uint64_t r_c;
  uint64_t t;
  
  size_t k = m - n;
  for (size_t i = k+1; i > 0; --i) { // penser à utiliser i-1 et pas uniquement i !!
    
    struct bign *u_prime = malloc(sizeof(struct bign));
    // u_prime à une taille de i + n - i + 1, soit n + 1, avec n = v->size
    u_prime->data = calloc(n+1, sizeof(uint32_t)); 
    u_prime->capacity = n + 1;
    u_prime->size = n + 1;
    
    bign_copy_from_other_part(u_prime, u, i-1, i + n);
    
    // u_prime_normalized est utilisé pour les calculs avec u_prime
    // car u_prime doit être normalisé pour les différents appels
    // de fonctions mais doit gardé sa capacité et size.
    struct bign *u_prime_normalized = malloc(sizeof(struct bign));
    bign_create_empty(u_prime_normalized);
    bign_copy_from_other(u_prime_normalized,u_prime);
    bign_normalize(u_prime_normalized);
    
    // On utilise l'algorithme de division
    if (v->data[n-1] == u_prime->data[n]) {
      q_c = BASE - 1;
      r_c = (uint64_t) u_prime->data[n] + (uint64_t) u_prime->data[n-1];
    }
    else {
      t = (uint64_t) u_prime->data[n] * BASE + (uint64_t) u_prime->data[n-1]; 
      q_c = t / (uint64_t) v->data[n-1];
      r_c = t % (uint64_t) v->data[n-1];
    }
    
    while ((r_c < BASE) && ((q_c * (uint64_t) v->data[n-2]) > (BASE * r_c + (uint64_t) u_prime->data[n-2]))) {
      q_c--;
      r_c += (uint64_t) v->data[n-1];
    }
    bign_mul_short(w, v,(uint32_t) q_c);
    
    if (bign_cmp(w,u_prime_normalized) > 0) {
      q_c--;
      bign_sub(w,w,v);
    }
    
    bign_sub(u_prime,u_prime_normalized,w);
    
    
    for (size_t j = i-1; j < n+i; ++j) {
      u->data[j] = bign_get(u_prime,j-(i-1));
    }
    
    res_quo->data[i-1] = (uint32_t) q_c;
    
    bign_destroy(u_prime); free(u_prime);
    bign_destroy(u_prime_normalized); free(u_prime_normalized);
  }
  
  // Même raisons que pour u_prime_normalized
  struct bign *u_normalized = malloc(sizeof(struct bign));
  bign_create_empty(u_normalized);
  bign_copy_from_other(u_normalized,u);
  bign_normalize(u_normalized);
  
  bign_div_short(res_rem,bin->data,u_normalized,d);
  
  bign_normalize(res_quo);
  bign_normalize(res_rem);
  bign_copy_from_other(quo,res_quo);
  bign_copy_from_other(rem,res_rem);
  
  bign_destroy(bin);          free(bin);
  bign_destroy(res_quo);      free(res_quo);
  bign_destroy(res_rem);      free(res_rem);
  bign_destroy(u);            free(u);
  bign_destroy(v);            free(v);
  bign_destroy(w);            free(w);
  bign_destroy(u_normalized); free(u_normalized);
}

// https://janmr.com/blog/2014/04/basic-multiple-precision-long-division/
void bign_div(struct bign *quo, struct bign *rem, const struct bign *lhs, const struct bign *rhs) {
  if(quo->capacity != 0){ // On détruit quo si non-vide 
    bign_destroy(quo);
  }
  if(rem->capacity != 0){ // On détruit res si non-vide
    bign_destroy(rem);
  }
  
  quo->data = calloc(lhs->capacity, sizeof(uint32_t));
  quo->capacity = lhs->capacity;
  quo->size = lhs->size;
  rem->data = calloc(rhs->capacity, sizeof(uint32_t));
  rem->capacity = rhs->capacity;
  rem->size = rhs->size;

  if(bign_cmp(lhs, rhs) < 0){ // lhs < rhs => quo = 0, rem = lhs
    quo->data[0] = 0;
    quo->size = 1;
    bign_copy_from_other(rem, lhs);
  }
  else if (rhs->size == 1) { // rhs < BASE => DivShort(lhs,rhs)
    bign_div_short(quo, rem->data, lhs, rhs->data[0]);
  }
  else {
    bign_div_knuth(quo, rem, lhs, rhs);
  }
  
  bign_normalize(quo);
  bign_normalize(rem);
}



void bign_exp(struct bign *self, const struct bign *lhs, const struct bign *rhs) {
  ////on va allouer un bign temporaire pour calculer lhs ^ rhs au cas où self est égal à lhs ou rhs
  struct bign *res = malloc(sizeof(struct bign));
  //on initalise res à 1 si rhs = 0
  bign_create_from_value(res, 1); 

  //on initialise des bign pour les quotient et les reste de la puissance, aussiun bign pour le chiffre 2, ainsi que 2 bign pour la puissance et la base
  struct bign *quo = malloc(sizeof(struct bign));
  struct bign *rem = malloc(sizeof(struct bign));
  struct bign *two = malloc(sizeof(struct bign));
  struct bign *base = malloc(sizeof(struct bign));
  struct bign *puissance = malloc(sizeof(struct bign));
  bign_create_empty(quo);
  bign_create_from_value(rem, 0);
  bign_create_from_value(two, 2);
  bign_create_empty(base);
  bign_create_empty(puissance);

  //on copie lhs dans la base ainsi que rhs dans la base pour ne pas avoir à les modifier
  bign_copy_from_other(base, lhs);      
  bign_copy_from_other(puissance, rhs);

  //on effectue l'algorithme de l'exponentiation rapide mais en l'adaptant au bign
  while(bign_cmp_zero(puissance) == 1){
    bign_div(quo, rem, puissance, two);
    if(rem->data[0] == 1){
      bign_mul_karatsuba(res, res, base);
    }
    bign_mul_karatsuba(base, base, base);
    bign_copy_from_other(puissance, quo);
  }

  //on normalise res puis le copie dans self et enfin on va détruire et libérer la place mémoire de tous les bign allouer dans cette fonction
  bign_normalize(res);
  bign_copy_from_other(self, res);
  bign_destroy(res);
  bign_destroy(quo);
  bign_destroy(rem);
  bign_destroy(two);
  bign_destroy(base);
  bign_destroy(puissance);
  free(res);
  free(quo);
  free(rem);
  free(two);
  free(base);
  free(puissance);
}

/*
 *
 * bigz
 *
 */

void bigz_create_empty(struct bigz *self) {
  self->positive = true;
  bign_create_empty(&self->n);
}

void bigz_create_from_value(struct bigz *self, int32_t val) {
  if(val < 0){
    self->positive = false;
    val = -val;
  }else{
    self->positive = true;
  }
  self->n.capacity = 1;
  self->n.size = 1;
  self->n.data = malloc(sizeof(uint32_t));
  self->n.data[0] = val;
}

void str16(struct bigz *self, char *str, size_t str_len){
  //on initialise une chaine de caractère à la taille de str_len + 1 
  //où str_len est égal à la taille du str à la base
  char *str16 = malloc((str_len+1) * sizeof(char));
  size_t i = 0;
  
  //on va effectuer une boucle jusqu'à ce que le str_len soit égal à 2 
  //et que le nombre pour la division soit inférieur à 16
  while((str_len > 2)||((str_len == 2)&&(char_to_int(str[0])*10 + char_to_int(str[1]) > 15))){
    int nb = char_to_int(str[0]) * 10 + char_to_int(str[1]);		//on  va initialiser le nombre avec les 2 premiers éléments du str en tant que chiffre
    size_t j = 0;
    size_t k = 2;					
    //on effectuer une boucle où k va être l'indice de la chaine str
    while((k < str_len)||((str_len == k)&&(nb > 16))){
      //si le nombre est inférieur à 16 et que le k n'a pas atteint la fin de la chaine 
      //on va ajouter le chiffre de l'indice k du str
      if((nb < 16)&&(k < str_len)){
        nb = nb * 10 + char_to_int(str[k]);
        ++k;
      }
      //on va mettre dans le str le nombre de la division entiere entre nb et 16
      //en le transformant en caractère
      str[j] = int_to_char(nb / 16);
      nb = nb - (nb  / 16) * 16;
      ++j;
    }
    //on va ajouter à la fin du str le caractère de fin de ligne qui va nous permettre d'avoir la nouvelle taille
    //on va aussi ajouter le nombre transformer en caracètre dans le str16 
    str[j] = '\0';
    str16[i] = int_to_char(nb);
    ++i;
    str_len = str_length(str);
  }
  if(str_len == 1){
    str16[i] = str[0];
  }else{
    str16[i] = int_to_char(char_to_int(str[0]) * 10 + char_to_int(str[1]));
  }
  str16[i + 1] = '\0';
  //une fois le str16 final obtenu on va effectuer le create_from_string comme pour la base 2 et la base 16
  //sauf qu'on va partir du début de la chaine pour calculer les nombres
  str_len = str_length(str16);
  self->n.capacity = str_len / 8;
  if(str_len % 8 != 0){
    self->n.capacity += 1;
  }
  self->n.data = malloc(self->n.capacity * sizeof(uint32_t));
  for(size_t i = 0; i < self->n.capacity; ++i){
    size_t j = 0;
    uint32_t res = 0;
    while((j < 8)&&((i * 8 + j) < str_len)){
      res += char_to_int(str16[i * 8 + j]) * exponentiation_rapide(16, j);
      ++j;
    }
    self->n.data[i] = res;
    self->n.size += 1;
  }
  while(self->n.capacity > 0){
    if(self->n.data[self->n.size-1] == 0){
      --self->n.size;
    }
    free(str16);
    return;
  }
  free(str16);
}

void bigz_create_from_string_with_base(struct bigz *self, const char *str, unsigned base, size_t nb_bit, size_t str_len){
  //meme algo effectuer que dans le bign_create_from_string()
  self->n.capacity = str_len / nb_bit;
  if(str_len % nb_bit != 0){
    self->n.capacity += 1;
  }
  self->n.data = malloc(self->n.capacity * sizeof(uint32_t));
  for(size_t i = 0; i < self->n.capacity; ++i){
    size_t j = 0;
    uint32_t res = 0;
    while((j < nb_bit)&&((int)(str_len - j - 1 - i * nb_bit) != -1)&&(str[str_len - j - 1 - i * nb_bit] != '-')){
      res += char_to_int(str[str_len - j - 1 - i * nb_bit]) * exponentiation_rapide(base, j);
      ++j;
    }
    self->n.data[i] = res;
    self->n.size += 1;
  }
  while(self->n.capacity > 0){
    if(self->n.data[self->n.size-1] == 0){
      --self->n.size;
    }
    return;
  }
}

void bigz_create_from_string(struct bigz *self, const char *str, unsigned base) {
  self->n.size = 0;
  //on va calculer la taille de str qui va nous servir pour plus tard
  //on regarder le premier caractère de str pour savoir si self va être positif ou non
  size_t str_len = str_length(str);
  if(str[0] == '-'){
    self->positive = false;
  }else{
    self->positive = true;
  }
  //on effectue le cas où str représente 0 ou -0
  if(((str_len == 1)&&(str[0] == '0'))||((str_len == 2)&&(str[0] == '-')&&(str[1] == '0'))){
    self->n.capacity = 1;
    self->n.size = 1;
    self->n.data = malloc(self->n.capacity * sizeof(uint32_t));
    self->n.data[0] = 0;
  }else{
    if(base == 2){							//si la base est 2 ou 16 on va effectuer le bigz_create_from_string() avec la base 2 ou 16
      bigz_create_from_string_with_base(self, str, base, 32, str_len);
    }else if(base == 10){						//sinon si la base est 10 alors on créer une nouvelle chaine de caractère avec que les chiffres de str
      char *str2 = malloc((str_len + 1) * sizeof(char));		//qui va nous servir à calculer une chaine de caractère écrite en héxadécimal dans str16
      if(self->positive){
        for(size_t i = 0; i < str_len; ++i){
          str2[i] = str[i];
        }
        str2[str_len] = '\0';
      }else{
        for(size_t i = 0; i < str_len; ++i){
          str2[i] = str[i+1];
        }
        str2[str_len] = '\0';
        --str_len;
      }
      str16(self, str2, str_len);
      free(str2);
    }else if(base == 16){
      bigz_create_from_string_with_base(self, str, base, 8, str_len);
    }
  }
}

void bigz_copy_from_other(struct bigz *self, const struct bigz *other) {
  //si self n'est pas vide alors on va le détruire
  if(self->n.capacity != 0){
    bigz_destroy(self);
  }
  //on va faire que le signe, la capacité et la taille de self soit la même que other et copier le tableau de other dans self indice par indice
  self->positive = other->positive;
  self->n.capacity = other->n.capacity;
  self->n.size = other->n.size;
  self->n.data = malloc(self->n.capacity * sizeof(uint32_t));
  for(size_t i = 0; i < self->n.size; ++i){
    self->n.data[i] = other->n.data[i];
  }
}

void bigz_move_from_other(struct bigz *self, struct bigz *other) {
  //si self n'est pas vide alors on va le détruire
  if(self->n.capacity != 0){
    bigz_destroy(self);
  }
  //on va faire que le signe, la taille, la capacité et l'adresse de data soit la même que other
  self->positive = other->positive;
  self->n.data = other->n.data;
  self->n.size = other->n.size;
  self->n.capacity = other->n.capacity;
  //on va mettre l'adresse de other à nul pour qu'il n'est plus accès à l'adresse du data et mettre sa capacité et sa taille à 0
  other->n.data = NULL;
  other->n.size = 0;
  other->n.capacity = 0;
}

void bigz_destroy(struct bigz *self) {
  free(self->n.data);
  self->n.data = NULL;
  self->n.capacity = 0;
  self->n.size = 0;
}

void bigz_print(const struct bigz *self) {
  if(!self->positive){
    printf("-");
  }
  bign_print(&self->n);
}

int bigz_cmp(const struct bigz *lhs, const struct bigz *rhs) {
  if((lhs->n.size == 1)&&(rhs->n.size == 1)&&(lhs->n.data[0] == 0)&&(rhs->n.data[0] == 0)){ //cas où si on a un -0 dans rhs ou lhs
    return 0;
  }
  if((lhs->positive)&&(!rhs->positive)){						    //cas où on a lhs positif et rhs négatif
    return 1;
  }
  if((!lhs->positive)&&(rhs->positive)){						   //cas où on a lhs négatif et rhs positif00
    return -1;
  }
  if(lhs->n.size > rhs->n.size){							//cas où la taille de lhs > la taille de rhs
    return 1;
  }
  if(lhs->n.size < rhs->n.size){							//cas où la taille de lhs < la taille de rhs 
    return -1;
  }
  if(((lhs->positive)&&(rhs->positive))||((!lhs->positive)&&(!rhs->positive))){		//si lhs et rhs sont de meme signe et de meme taille
    for(size_t i = 0; i < lhs->n.size; ++i){						//alors on va regarder indice par indice dans lhs et rhs pour voir lequel est le plus grand
      if(lhs->n.data[lhs->n.size - i - 1] > rhs->n.data[rhs->n.size - i - 1]){
        return 1;
      }
      if(lhs->n.data[lhs->n.size - i - 1] < rhs->n.data[rhs->n.size - i - 1]){
        return -1;
      }
    }
  }
  return 0;
}

int bigz_cmp_zero(const struct bigz *self) {
  if((self->n.size == 1)&&(self->n.data[0] == 0)){
    return 0;
  }
  if(self->positive){
    return 1;
  }
  return -1;
}

void bigz_add(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs) {
  if(((!lhs->positive)&&(!rhs->positive))||((lhs->positive)&&(rhs->positive))){
    if((lhs->positive)&&(rhs->positive)){     //si lhs et rhs sont positives self sera positif
      self->positive = true;
    }else{                                    //sinon self sera négatif
      self->positive = false;
    }
    bign_add(&self->n, &lhs->n, &rhs->n);     //ensuite on effectue une addition en bign
  }else if(bign_cmp(&lhs->n, &rhs->n) == 1){  //sinon si lhs > rhs
    if((lhs->positive)&&(!rhs->positive)){      //si lhs est positif et rhs est négatif self sera positif
      self->positive = true;
      bign_sub(&self->n, &lhs->n, &rhs->n);   //ensuite on effectue une soustraction en bign entre lhs et rhs
    }else{                                      //sinon self sera négatif
      self->positive = false;
      bign_sub(&self->n, &lhs->n, &rhs->n);   //ensuite on effectue une soustraction en bign entre lhs et rhs
    }
  }else{                                      //sinon
    if((lhs->positive)&&(!rhs->positive)){      //si lhs est positif et rhs est négatif self sera négatif
      self->positive = false;
      bign_sub(&self->n, &rhs->n, &lhs->n);   //ensuite on effectue une soustraction en bign entre rhs et lhs
    }else{                                    //sinon self sera positif
      self->positive = true;
      bign_sub(&self->n, &rhs->n, &lhs->n);   //ensuite on effectue une soustraction en bign entre rhs et lhs
    }
  }
}

void bigz_sub(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs) {
  if((lhs->positive)&&(rhs->positive)){           //si lhs et rhs sont positives
    if(bign_cmp(&lhs->n, &rhs->n) == 1){            //si lhs > rhs self sera positif
      self->positive = true;
      bign_sub(&self->n, &lhs->n, &rhs->n);       //ensuite on effectue une soustraction en bign entre lhs et rhs
    }else{                                          //sinon self sera négatif
      self->positive = false;
      bign_sub(&self->n, &rhs->n, &lhs->n);       //ensuite on effectue une soustraction en bign entre rhs et lhs
    }
  }else if((!lhs->positive)&&(!rhs->positive)){   //sinon si lhs et rhs sont négatives
    if(bign_cmp(&lhs->n, &rhs->n) == 1){            //si lhs > rhs self sera négatif
      self->positive = false;
      bign_sub(&self->n, &lhs->n, &rhs->n);       //ensuite on effectue une soustraction en bign entre lhs et rhs
    }else{                                          //sinon self sera positif
      self->positive = true;
      bign_sub(&self->n, &rhs->n, &lhs->n);       //ensuite on effectue une soustraction en bign entre rhs et lhs
    }
  }else if((lhs->positive)&&(!rhs->positive)){    //sinon si lhs est positif et rhs est négatif self sera positif
    self->positive = true;
    bign_add(&self->n, &lhs->n, &rhs->n);         //ensuite on effectue une addition en bign
  }else{                                          //sinon self sera négatif
    self->positive = false;   
    bign_add(&self->n, &lhs->n, &rhs->n);         //ensuite on effectue une addition en bign
  }
}

void bigz_mul(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs) {
  if(((lhs->positive)&&(rhs->positive))||((!lhs->positive)&&(!rhs->positive))){ //si lhs et rhs sont de meme signe alors self sera positif
    self->positive = true;
  }else{                                                                        //sinon self sera négatif
    self->positive = false;
  }
  bign_mul_karatsuba(&self->n, &lhs->n, &rhs->n);                               //enfin on effectue la multiplication de karatsuba en bign
}

void bigz_div(struct bigz *quo, struct bigz *rem, const struct bigz *lhs, const struct bigz *rhs) {
  // On suit l'algorithme de division euclidienne entre deux entiers relatifs
  if (!rhs->positive) {
    struct bigz *oppo_rhs = malloc(sizeof(struct bigz));
    bigz_create_empty(oppo_rhs);
    bigz_copy_from_other(oppo_rhs,rhs);
    oppo_rhs->positive = true;
    
    bigz_div(quo,rem,lhs,oppo_rhs);
    quo->positive = !quo->positive;
    
    bigz_destroy(oppo_rhs);
    free(oppo_rhs);
    return;
  }
  if (!lhs->positive) {
    struct bigz *oppo_lhs = malloc(sizeof(struct bigz));
    bigz_create_empty(oppo_lhs);
    bigz_copy_from_other(oppo_lhs,lhs);
    
    oppo_lhs->positive = true;
    
    bigz_div(quo,rem,oppo_lhs,rhs);
    
    quo->positive = false;
    
    bigz_destroy(oppo_lhs);
    free(oppo_lhs);
    
    if (bigz_cmp_zero(rem) == 0) { // on a deja le bon resultat
      return;
    }
    
    struct bigz *un = malloc(sizeof(struct bigz));
    bigz_create_empty(un);
    bigz_create_from_value(un,1);
    
    bigz_sub(quo,quo,un);
    
    bigz_sub(rem,rhs,rem);
    
    bigz_destroy(un);
    free(un);
    return;
  }
  
  bign_div(&quo->n,&rem->n,&lhs->n,&rhs->n);
}

void bigz_gcd(struct bigz *self, const struct bigz *lhs, const struct bigz *rhs) {
  // On suit l'algorithme d'Euclide
  if(bigz_cmp_zero(rhs) == 0){
    bigz_copy_from_other(self, lhs);
    return;
  }
  struct bigz *quo = malloc(sizeof(struct bigz));
  struct bigz *rem = malloc(sizeof(struct bigz));
  bigz_create_empty(quo);
  bigz_create_empty(rem);
  bigz_div(quo, rem, lhs, rhs);
  
  if (bigz_cmp_zero(rem) == 0) {
    bigz_copy_from_other(self, rhs);
  }
  else {
    bigz_gcd(self, rhs, rem);
  }

  bigz_destroy(quo);
  bigz_destroy(rem);
  free(quo);
  free(rem);
  bign_normalize(&self->n);

}
