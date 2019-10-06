#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/********************************************************************************/
/*				TRAVAIL SUR LA NOTION DE TABLE DE HACHAGE (HASH)				*/
/*		Utilisation du programme :												*/
/*			Entrer une commande :												*/
/*				- init <size>  Création d'une table de hachage de taille donnée	*/
/*				- insert <key> <value>	Insertion d'une entrée clé-valeur		*/
/*				- delete <key>			Suppression de l'entrée correspondante	*/
/*				- query <key>			Affichage de la valeur associée à key	*/
/*				- stats					Statistiques de la table (occupation)	*/
/*				- destroy				Libération de la mémoire				*/
/*				- bye			 		Fin du programme						*/
/*																				*/
/*	NB : Focntions de hachage données, définies plus bas						*/
/********************************************************************************/

/********************************************************************************/
/*																				*/
/*					STRUCTURES ET PROTOTYPES DE FONCTIONS						*/
/*																				*/
/********************************************************************************/

/*			TYPES ET STRUCTURES			*/


/*	Nouveau type Key : clé <=> pointeur sur caractères	*/
typedef char * Key;

/*	Enumération Statut : état d'une case de la table	*/
typedef enum Statut 
{
	VIDE, REMPLIE, SUPPRIMEE
} Statut ;

/*	Structure Cell : Cellule de la table 	*/
typedef struct Cell 
{
	Statut statut ; /* vide, remplie ou supprimée */
	Key key ;		/* clef */
	char* value ;	/* valeur associée */
} Cell ;

/*	Structure Hash : Table de hachage	*/
typedef struct Hash
{
	int size ;		/* taille de la table (nombre de cellules) */
	Cell * table ;	/* pointeur sur tableau de cellules */
} Hash ;


/*			FONCTIONS				*/

/* Fonction en cas d'erreur de saisie */
void error(void);

/* Fonction d'initialisation de la table de hachage
 * allocation du tableau de cellules, initialisation
 * des cellules à un état vide */
Hash* initHash(int size) ;

/* Fonction d'insertion dans la table
 * utilise la fonction de hachage afin d'associer
 * une position à la clef donnée, et remplie la cellule
 * correspondante avec la valeur entrée */
void insert(Hash* hash, Key key, char * value) ;

/* Fonction permettant l'affichage des couples
 * "clef : valeur" contenus dans la table */
void affichage(Hash* hash) ;

/* Fonction de suppression de la clef donnée
 * et de sa valeur associée */
void supprimer(Hash* hash, Key key) ;

/* Fonction de libération de la mémoire */
void liberer(Hash * hash) ;

/* Fonction de recherche d'une valeur à partir
 * de la clef donnée
 * Recherche par l'indice de la cellule, passage
 * de la clef par la fonction de hachage */
void recherche(Hash * hash, Key key) ;

/* Fonction parcourant la table afin de relever
 * le nombre de cellules pour chaque statut possible */
void statistiques(Hash* hash) ;


/********************************************************************************/
/*																				*/
/*										MAIN									*/
/*																				*/
/********************************************************************************/
int main(void) 
{
   int size;
   char lecture[100];
   char * key;
   char * val;
   Hash * hash ;

   if (fscanf(stdin,"%99s",lecture)!=1)
      error();
   
   while (strcmp(lecture,"bye")!=0)
   {
      if (strcmp(lecture,"init")==0)
      {	//Initialisation de la table de hachage
         
         if (fscanf(stdin,"%99s",lecture)!=1)
            error();
            
         size = atoi(lecture);
         hash = initHash(size) ;
      }
      
      else if (strcmp(lecture,"insert")==0)
      {	//Insertion dans la table
        
         if (fscanf(stdin,"%99s",lecture)!=1)
            error();
         
         //On récupère la valeur entrée pour la clef
         key = strdup(lecture);
         
         if (fscanf(stdin,"%99s",lecture)!=1)
            error();
            
         //On récupère la valeur associée entrée   
         val = strdup(lecture);
         
         insert(hash, key, val) ;
      }
      
      else if (strcmp(lecture,"delete")==0)
      {	//Suppression d'un couple à partir d'une clef
         
         if (fscanf(stdin,"%99s",lecture)!=1)
            error();
            
         //On récupère la clef et on supprime   
         key = strdup(lecture);
         supprimer(hash, key) ;
      }
      
      else if (strcmp(lecture,"query")==0)
      {	//Recherche d'une valeur associée
         
         if (fscanf(stdin,"%99s",lecture)!=1)
            error();
         
         //On récupère la clef et on fait la recherche
         key = strdup(lecture);
         recherche(hash, key) ;
      }
      
      else if (strcmp(lecture,"destroy")==0)
      {	//Libération de la mémoire
	      liberer(hash) ;
      }
      
      else if (strcmp(lecture,"stats")==0)
      {	//Statistiques de la table
         statistiques(hash) ;
      }

      if (fscanf(stdin,"%99s",lecture)!=1)
         error();
   }
   
   return 0;
}

/********************************************************************************/
/*																				*/
/*							DEFINITIONS DES FONCTIONS							*/
/*																				*/
/********************************************************************************/

/* fonction de décalage de bit circulaire */
unsigned int shift_rotate(unsigned int val, unsigned int n)
{
  n = n%(sizeof(unsigned int)*8);
  return (val<<n) | (val>> (sizeof(unsigned int)*8-n));
}

/* fonction d'encodage d'une chaîne de caractères */
unsigned int Encode(Key key)
{
   unsigned int i;
   unsigned int val = 0;
   unsigned int power = 0;
   for (i=0;i<strlen(key);i++)
   {
     val += shift_rotate(key[i],power*7);
     power++;
   }
   return val;
}

/* fonction de hachage simple qui prend le modulo */
unsigned int hash(unsigned int val, unsigned int size)
{
   return val%size;
}

/* fonction de hachage complète à utiliser */
unsigned int HashFunction(Key key, unsigned int size)
{
   return hash(Encode(key),size);
}

/* fonction d'interruption du programme en cas de caractère entré incorrect */
void error(void)
{
   printf("input error\r\n");
   exit(0);
}

/* fonction d'initialisation de la table de hachage */
Hash* initHash(int size)
{
	int i ;
	Hash * hash ;
	
	//Allocation de la table de hachage
	hash = malloc(sizeof(Hash)) ;
	
	hash->size = size ;
	hash->table = malloc(size*sizeof(Cell)) ; //Allocation du tableau de Cell
	
	//Initialisation du statut des cellules
	for (i=0; i<size; i++)
	{
		hash->table[i].statut = VIDE ;
	}
	
	return hash ;
}

/* fonction d'insertion de couple clé-valeur */
void insert(Hash* hash, Key key, char* value) 
{
	int position ;
	//On récupère la position correspondant à la clef donnée
	position = HashFunction(key, hash->size) ;
	
	//Si la case est déjà remplie, on ne fait rien
	if (hash->table[position].statut == REMPLIE && strcmp(key, hash->table[position].key))
	{
		return ;
	}
	
	//On ajoute
	hash->table[position].key = key;
	hash->table[position].value = value;
	hash->table[position].statut = REMPLIE;
	
}

/* affichage de la table de hachage (taille et couples "clef : valeur") */
void affichage(Hash* hash) 
{
	int pos ;
	Cell c ;
	
	printf("Size : %d\n", hash->size) ;
	
	for (pos=0; pos<hash->size; pos++)
	{
		c= hash->table[pos];
		if (c.statut == REMPLIE)
		{
			printf("%s :\t%s\n", c.key, c.value) ;
		}
	}
}

/* fonction de suppression de l'entrée correspondant à la clé entrée */
void supprimer(Hash* hash, Key key)
{
	int position = HashFunction(key, hash->size) ;
	
	//S'il n'y a pas de collision, on supprime
	//(il y a collision si deux clefs différentes donnent la même valeur)
	if (!strcmp(key, hash->table[position].key))
	{
		hash->table[position].statut = SUPPRIMEE ;		
	}
}

/* fonction de recherche d'un élément à partir de sa clé */
void recherche(Hash * hash, Key key)
{
	int i ;
	
	//Parcours de la table
	for (i=0; i<hash->size; i++) {
		//Si on a bien la bonne clé et que la case est bien remplie
		if (hash->table[i].statut == REMPLIE && !strcmp(key, hash->table[i].key))
		{
			printf("%s\r\n", hash->table[i].value) ; //On affiche la valeur trouvée
			return ;
		}
	}
	
	//Si la recherche n'aboutit pas
	printf("Not found\r\n") ;
}

/* fonction qui affiche les statistiques de la table (nb de cases total, nb de cases remplies, nb de cases vides, nb de cases supprimées) */
void statistiques(Hash* hash)
{
	int i, nb_pleines=0, nb_vides=0, nb_supp=0 ;
	
	printf("size    : %d\r\n", hash->size) ;
	
	//Parcours de la table
	for(i=0; i<hash->size; i++)
	{
		if (hash->table[i].statut == VIDE)
		{
			nb_vides++ ;
		}
		else if (hash->table[i].statut == REMPLIE)
		{
			nb_pleines++ ;
		}
		else {
			nb_supp++ ;
		}
	}
	
	printf("empty   : %d\r\ndeleted : %d\r\nused    : %d\r\n", nb_vides, nb_supp, nb_pleines) ;
}

/* fonction de libération de la mémoire */
void liberer(Hash* hash)
{
	int i ;
	
	//On libère chacune des cellules
	for (i=0; i<hash->size; i++)
	{
		if (hash->table[i].statut != VIDE)
		{
			free(hash->table[i].key) ;
			free(hash->table[i].value) ;
		}
	}
	
	//On libère le tableau
	free(hash->table) ;
	
	//On libère la table de hachage
	free(hash) ;
}
