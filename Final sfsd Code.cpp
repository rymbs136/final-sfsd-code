#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Constants
#define MAX_FILE_NAME 50 // Longueur maximale pour le nom de fichier
#define Fact_Blockage 10  // Facteur de blockage
#define MAX_BLOCKS 100

// Structure definitions

// Structure pour reprÃ©senter un enregistrement
typedef struct {
    int id;  // Identifiant unique
    char data[100];  // Donnes de l'enregistrement
    int activ;   // 0 = deleted
} Record; 

// Structure pour reprÃ©senter un bloc de memoire
typedef struct MemoryBlock {
    int id;    // Identifiant du bloc
    int is_occupied;  // 0 = libre, 1 = occuper
    char file_name[MAX_FILE_NAME];  // Nom du fichier associÃ© (si occupÃ©)
    int record_count;  // Nombre d'enregistrements
    Record r[Fact_Blockage]; // Vecteur des enregistrement
    int next;   // 0 = reprÃ©sentation contigue
} MemoryBlock;

// Structure pour representer les metadonnes
typedef struct {
    char *file_name;
    int adresse_premier;
    int record_count;
    int bloc_count;
    int mode;     // 1 = contigue, 0 = chinee
    int etat;   // 1 = actif, 0 = supprimer          
} meta;

// tableau qui reprÃ©sente les metadonnÃ©es
meta liste_metadonnes[100];
int index_metadonnes = 0;

// Global variables
MemoryBlock *memory;
int total_blocks;
int block_size;

// Validation functions
int validate_filename(const char *filename) {
    if (!filename) {
        printf("Nom de fichier invalide!\n");
        return 0;
    }
    if (strlen(filename) >= MAX_FILE_NAME) {
        printf("Nom de fichier trop long (max %d caractÃ¨res)\n", MAX_FILE_NAME);
        return 0;
    }
    return 1;
}

int validate_record_count(int count) {
    if (count <= 0) {
        printf("Le nombre d'enregistrements doit Ãªtre positif\n");
        return 0;
    }
    if (count > MAX_BLOCKS * Fact_Blockage) {
        printf("Nombre d'enregistrements trop grand pour la mÃ©moire disponible\n");
        return 0;
    }
    return 1;
}

int validate_mode(int mode) {
    if (mode != 0 && mode != 1) {
        printf("Mode invalide! Utilisez 0 pour chaÃ®nÃ© ou 1 pour contigu\n");
        return 0;
    }
    return 1;
}

// Fonction pour afficher la liste des mÃ©tadonnÃ©es
void afficher_metadonnes() {
	int i;
    printf("Liste des mÃ©tadonnÃ©es : \n");
    for (i = 0; i < index_metadonnes; i++) {
        if (liste_metadonnes[i].etat == 1) {
            printf("Fichier : %s \n", liste_metadonnes[i].file_name);
            printf("Adresse premier bloc : %d\n", liste_metadonnes[i].adresse_premier);
            printf("Record count : %d\n", liste_metadonnes[i].record_count);
            printf("Bloc count : %d\n", liste_metadonnes[i].bloc_count);
            if (liste_metadonnes[i].mode == 0) {
                printf("Mode : chaÃ®nÃ© \n");
            } else {
                printf("Mode : contigu \n");
            }
        }
    }
}

// Memory initialization
void initialize_memory() {
	int i;
    printf("Entrez le nombre total de blocs : ");
    scanf("%d", &total_blocks);

    if (total_blocks <= 0 || total_blocks > MAX_BLOCKS) {
        printf("Nombre de blocs invalide. Utilisation de la valeur par dÃ©faut: %d\n", MAX_BLOCKS);
        total_blocks = MAX_BLOCKS;
    }

    printf("Entrez la taille d'un bloc (en octets) : ");
    scanf("%d", &block_size);

    if (block_size <= 0) {
        printf("Taille de bloc invalide. Utilisation de la valeur par dÃ©faut: 1024\n");
        block_size = 1024;
    }

    memory = (MemoryBlock *)malloc(total_blocks * sizeof(MemoryBlock));
    if (!memory) {
        printf("Erreur d'allocation mÃ©moire!\n");
        exit(1);
    }

    for ( i = 0; i < total_blocks; i++) {
        memory[i].id = i;
        memory[i].is_occupied = 0;
        strcpy(memory[i].file_name, ""); // Aucun fichier associÃ©
        memory[i].record_count = 0;     // Aucun enregistrement 
        memory[i].next = -1;
    }

    printf("\nMÃ©moire initialisÃ©e avec %d blocs de taille %d octets.\n", total_blocks, block_size);
}

// Metadata management functions
int fichier_existe(const char *file_name) {
	int i;
    for ( i = 0; i < index_metadonnes; i++) {
        if (liste_metadonnes[i].etat == 1 &&
            strcmp(liste_metadonnes[i].file_name, file_name) == 0) {
            return i;   // si le fichier trouver on retourne l'adresse du premier bloc i
        }
    }
    return -1;
}
// fonction pour ajouter un element Ã  la liste des metadonnes
void ajouter_metadonne(const char *file_name, int adr_pr, int record_count, int mode) {
    liste_metadonnes[index_metadonnes].file_name = strdup(file_name);
    liste_metadonnes[index_metadonnes].adresse_premier = adr_pr;
    liste_metadonnes[index_metadonnes].record_count = record_count;
    liste_metadonnes[index_metadonnes].bloc_count = (record_count + Fact_Blockage - 1) / Fact_Blockage;
    liste_metadonnes[index_metadonnes].mode = mode;
    liste_metadonnes[index_metadonnes].etat = 1;
    index_metadonnes++;
}


// function to read carcteristics
int read_metadonne(const char *file_name, int type) {
    int idx = fichier_existe(file_name);
    if (idx == -1) return -1;

    switch (type) {
        case 1: return liste_metadonnes[idx].adresse_premier;
        case 2: return liste_metadonnes[idx].record_count;
        case 3: return liste_metadonnes[idx].bloc_count;
        case 4: return liste_metadonnes[idx].mode;
        default: return -1;
    }
}
// fontion pour modifier metadonnes
void modifier_metadonne(const char *file_name, int type, int value) {
    int idx = fichier_existe(file_name);
    if (idx == -1) return;

    switch (type) {
        case 1: liste_metadonnes[idx].adresse_premier = value; break;
        case 2: liste_metadonnes[idx].record_count = value; break;
        case 3: liste_metadonnes[idx].bloc_count = value; break;
        case 4: liste_metadonnes[idx].mode = value; break;
    }
}
// fontion pour supprimer metadonnÃ©es
void supp_metadonnes(const char *file_name) {
    int idx = fichier_existe(file_name);
    if (idx != -1) {
        free(liste_metadonnes[idx].file_name);
        liste_metadonnes[idx].etat = 0;
    }
}

// Block allocation functions
// function to allocate space in contiguous mode
int Contigue(MemoryBlock *memory, int fileSize) {
    int Cmp = 0; // Compter le nombre de blocs consecutifs qui sont libres
    int j = -1;  // Enregistrer l'indice du premier bloc libre
    int i;
 
 // Parcours des blocs pour trouver un espace contigu
    for ( i = 0; i < total_blocks; i++) {
        if (memory[i].is_occupied == 0) {
            if (j == -1) {
                j = i;
            }
            Cmp++;
            if (Cmp == fileSize) break;
        } else {
            j = -1;
            Cmp = 0;
        }
    }
// Verifier si l'espace contigu est disponible
    if (Cmp < fileSize) {
        printf("Espace contigu insuffisant\n");
        return -1;
    }
 // Allouer les blocs
    for (i = j; i < j + fileSize; i++) {
        memory[i].is_occupied = 1;
    }

    printf("Allocation contiguÃ« rÃ©ussie, dÃ©but au bloc %d.\n", j);
    return j;
}
// function to allocate space in chained mode
int Chained(MemoryBlock *memory, int fileSize) {
    int Cmp = 0;  // Compter le nombre de blocs allouer
    int j = -1;   // Indice du premier bloc allouer
    int k = -1;   // Indice du bloc precedent
    int i; 

    for ( i = 0; i < total_blocks; i++) {
        if (memory[i].is_occupied == 0) {
            if (j == -1) {
                j = i;
                memory[i].is_occupied = 1;
            }

            if (k != -1) {
                memory[k].next = i; // Chainage entre les blocs
            }
            k = i;
            Cmp++;

            if (Cmp == fileSize) {
                memory[i].next = -1;  // Fin de la chaine
                break;
            }
        }
    }

    if (Cmp < fileSize) {
        printf("Blocs insuffisants pour allocation chaÃ®nÃ©e\n");
        return -1;
    }

    printf("Allocation chaÃ®nÃ©e rÃ©ussie, dÃ©but au bloc %d.\n", j);
    return j;
}

// Display memory state
void display_memory_state() {
	int i;
    printf("\nÃ‰tat actuel de la mÃ©moire secondaire :\n");
    for ( i = 0; i < total_blocks; i++) {
        if (memory[i].is_occupied) {
            printf("\033[1;31m"); // Couleur rouge pour les blocs occupÃ©s
            printf("Bloc %d : OccupÃ© | Fichier : %s | Enregistrements : %d\n",
                   memory[i].id, memory[i].file_name, memory[i].record_count);
            printf("\033[0m"); // RÃ©initialiser la couleur
        } else {
            printf("\033[1;32m"); // Couleur verte pour les blocs libres
            printf("Bloc %d : Libre\n", memory[i].id);
            printf("\033[0m"); // RÃ©initialiser la couleur
        }
    }
}

// Record management functions
void crier_fichier(char *file_name, int record_count, int mode) {
	int i;
	int j;
    if (!validate_filename(file_name) || !validate_record_count(record_count) || !validate_mode(mode)) {
        return;
    }

    if (fichier_existe(file_name) >= 0) {
        printf("Erreur : Un fichier avec ce nom existe dÃ©jÃ .\n");
        return;
    }

    int bloc_count = (record_count + Fact_Blockage - 1) / Fact_Blockage;
    int adr_pr = (mode == 1) ? Contigue(memory, bloc_count) : Chained(memory, bloc_count);

    if (adr_pr == -1) {
        printf("Erreur d'allocation des blocs.\n");
        return;
    }

    ajouter_metadonne(file_name, adr_pr, record_count, mode);

    int current = adr_pr; // Bloc actuel
    int records_added = 0; // Nombre total d'enregistrements ajoutÃ©s

    while (records_added < record_count) {
        // VÃ©rifier si le bloc actuel est plein
        if (memory[current].record_count == Fact_Blockage) {
            if (mode == 0) { // Mode chaÃ®nÃ©
                // Trouver le prochain bloc libre
                int next_block = -1;
                for ( i = 0; i < total_blocks; i++) {
                    if (memory[i].is_occupied == 0) {
                        next_block = i;
                        break;
                    }
                }
                if (next_block == -1) {
                    printf("Erreur : Plus de blocs disponibles.\n");
                    return;
                }
                memory[current].next = next_block; // Lier le bloc actuel au suivant
                current = next_block; // Passer au bloc suivant
                memory[current].is_occupied = 1; // Marquer le bloc comme occupÃ©
                strcpy(memory[current].file_name, file_name); // Associer le fichier au bloc
                memory[current].record_count = 0; // RÃ©initialiser le compteur d'enregistrements
            } else { // Mode contigu
                printf("Erreur : Impossible d'ajouter plus d'enregistrements en mode contigu.\n");
                return;
            }
        }

        // Ajouter un enregistrement au bloc actuel
        int pos = memory[current].record_count;
        memory[current].r[pos].id = records_added + 1;
        memory[current].r[pos].activ = 1;
        printf("Entrez les donnÃ©es de l'enregistrement %d du bloc %d : ",
               memory[current].r[pos].id, current);
        scanf(" %s", memory[current].r[pos].data);
        memory[current].record_count++;
        records_added++;
    }

    // Associer le fichier au premier bloc
    strcpy(memory[adr_pr].file_name, file_name);

    printf("Fichier '%s' crÃ©Ã© avec succÃ¨s.\n", file_name);
}

// Record operations and search functions
Record *searchRecord(const char *file, int id) {
	int i;
	int j;
    int adr_pr = read_metadonne(file, 1);
    int bloc_count = read_metadonne(file, 3);
    int mode = read_metadonne(file, 4);

    if (adr_pr == -1 || bloc_count <= 0) {
        printf("Erreur : MÃ©tadonnÃ©es introuvables ou fichier vide.\n");
        return NULL;
    }

    printf("Recherche dans le fichier '%s', ID = %d, Mode = %d\n", file, id, mode);

    if (mode == 1) {
        for ( i = adr_pr; i < adr_pr + bloc_count; i++) {
            for ( j = 0; j < memory[i].record_count; j++) {
                Record *r = &memory[i].r[j];
                if (r->id == id && r->activ == 1) {
                    return r;
                }
            }
        }
    } else {
        for (i = adr_pr; i != -1; i = memory[i].next) {
            for ( j = 0; j < memory[i].record_count; j++) {
                Record *r = &memory[i].r[j];
                if (r->id == id && r->activ == 1) {
                    return r;
                }
            }
        }
    }
    return NULL;
}
// Insert a record into the file
void insertRecord(const char *file_name, int id, const char *data) {
	int i;
    int adr_pr = read_metadonne(file_name, 1);
    int mode = read_metadonne(file_name, 4);

    if (adr_pr == -1) {
        printf("Fichier non trouvÃ©\n");
        return;
    }

    int current = adr_pr;
    while (1) {
        // VÃ©rifier si le bloc actuel est plein
        if (memory[current].record_count == Fact_Blockage) {
            if (mode == 0) { // Mode chaÃ®nÃ©
                // Trouver le prochain bloc libre
                int next_block = -1;
                for ( i = 0; i < total_blocks; i++) {
                    if (memory[i].is_occupied == 0) {
                        next_block = i;
                        break;
                    }
                }
                if (next_block == -1) {
                    printf("Erreur : Plus de blocs disponibles.\n");
                    return;
                }

                // Lier le bloc actuel au suivant
                memory[current].next = next_block;

                // Passer au bloc suivant
                current = next_block;

                // Marquer le bloc comme occupÃ© et l'associer au fichier
                memory[current].is_occupied = 1;
                strcpy(memory[current].file_name, file_name);
                memory[current].record_count = 0; // Initialiser le compteur d'enregistrements
            } else { // Mode contigu
                printf("Erreur : Impossible d'ajouter plus d'enregistrements en mode contigu.\n");
                return;
            }
        }

        // Ajouter l'enregistrement au bloc actuel
        int pos = memory[current].record_count;
        memory[current].r[pos].id = id;
        strcpy(memory[current].r[pos].data, data);
        memory[current].r[pos].activ = 1;
        memory[current].record_count++;

        // Mettre Ã  jour les mÃ©tadonnÃ©es du fichier
        int idx = fichier_existe(file_name);
        if (idx != -1) {
            liste_metadonnes[idx].record_count++; // IncrÃ©menter le nombre d'enregistrements
            liste_metadonnes[idx].bloc_count = (liste_metadonnes[idx].record_count + Fact_Blockage - 1) / Fact_Blockage; // Recalculer le nombre de blocs
        }

        break;
    }

    printf("Enregistrement insÃ©rÃ© avec succÃ¨s dans le fichier '%s'.\n", file_name);
}

// Record deletion functions
void deleteRecordLogical(const char *file, int id) {
	int i;
	int j;
    int adr_pr = read_metadonne(file, 1); // Adresse du premier bloc du fichier
    int mode = read_metadonne(file, 4); // Mode d'allocation (0 = chaÃ®nÃ©, 1 = contigu)

    if (adr_pr == -1) {
        printf("Erreur : fichier '%s' introuvable ou dÃ©jÃ  supprimÃ©.\n", file);
        return;
    }

    int current = adr_pr;
    while (current != -1) {
        for ( j = 0; j < memory[current].record_count; j++) {
            if (memory[current].r[j].id == id && strcmp(memory[current].file_name, file) == 0) {
                memory[current].r[j].activ = 0; // Suppression logique
                printf("Enregistrement supprimÃ© logiquement (ID: %d).\n", id);

                // Mettre Ã  jour les mÃ©tadonnÃ©es du fichier
                int idx = fichier_existe(file);
                if (idx != -1) {
                    liste_metadonnes[idx].record_count--; // DÃ©crÃ©menter le nombre d'enregistrements
                    liste_metadonnes[idx].bloc_count = (liste_metadonnes[idx].record_count + Fact_Blockage - 1) / Fact_Blockage; // Recalculer le nombre de blocs
                }

                display_memory_state(); // Afficher l'Ã©tat de la mÃ©moire aprÃ¨s la suppression
                return;
            }
        }
        current = memory[current].next; // Passer au bloc suivant
    }

    printf("Erreur : Enregistrement non trouvÃ© dans le fichier '%s'.\n", file);
}
// Physically delete a record by ID
void deleteRecordPhysical(const char *file, int id) {
	int j;
	int k;
    int adr_pr = read_metadonne(file, 1); // Adresse du premier bloc du fichier
    int mode = read_metadonne(file, 4); // Mode d'allocation (0 = chaÃ®nÃ©, 1 = contigu)

    if (adr_pr == -1) {
        printf("Erreur : fichier '%s' introuvable ou dÃ©jÃ  supprimÃ©.\n", file);
        return;
    }

    int current = adr_pr;
    while (current != -1) {
        for ( j = 0; j < memory[current].record_count; j++) {
            if (memory[current].r[j].id == id && strcmp(memory[current].file_name, file) == 0) {
                // DÃ©caler les enregistrements pour supprimer physiquement
                for ( k = j; k < memory[current].record_count - 1; k++) {
                    memory[current].r[k] = memory[current].r[k + 1];
                }
                memory[current].record_count--;

                // Mettre Ã  jour les mÃ©tadonnÃ©es du fichier
                int idx = fichier_existe(file);
                if (idx != -1) {
                    liste_metadonnes[idx].record_count--; // DÃ©crÃ©menter le nombre d'enregistrements
                    liste_metadonnes[idx].bloc_count = (liste_metadonnes[idx].record_count + Fact_Blockage - 1) / Fact_Blockage; // Recalculer le nombre de blocs
                }

                printf("Enregistrement supprimÃ© physiquement (ID: %d).\n", id);
                display_memory_state(); // Afficher l'Ã©tat de la mÃ©moire aprÃ¨s la suppression
                return;
            }
        }
        current = memory[current].next; // Passer au bloc suivant
    }

    printf("Erreur : Enregistrement non trouvÃ© dans le fichier '%s'.\n", file);
}

// Memory optimization functions
void compactMemory(MemoryBlock *memory) {
    int writeIndex = 0;
    int readIndex;
    int i;
    for ( readIndex = 0; readIndex < total_blocks; readIndex++) {
        if (memory[readIndex].is_occupied) {
            if (writeIndex != readIndex) {
                memory[writeIndex] = memory[readIndex];
                memory[readIndex].is_occupied = 0;
                memory[readIndex].next = -1;
            }
            writeIndex++;
        }
    }

    for ( i = writeIndex; i < total_blocks; i++) {
        memory[i].is_occupied = 0;
        memory[i].next = -1;
    }

    printf("Compactage de la mÃ©moire terminÃ©.\n");
    display_memory_state(); // Afficher l'Ã©tat de la mÃ©moire aprÃ¨s le compactage
}

// File defragmentation
void defragmenter_fichier(const char *file_name) {
	int i;
    int adr_pr = read_metadonne(file_name, 1); // Adresse du premier bloc du fichier
    int bloc_count = read_metadonne(file_name, 3); // Nombre de blocs du fichier
    int mode = read_metadonne(file_name, 4); // Mode d'allocation (0 = chaÃ®nÃ©, 1 = contigu)

    if (adr_pr == -1 || bloc_count <= 0) {
        printf("Erreur : fichier '%s' introuvable ou dÃ©jÃ  supprimÃ©.\n", file_name);
        return;
    }

    // VÃ©rifier si le fichier est dÃ©jÃ  contigu
    if (mode == 1) {
        int is_contiguous = 1;
        for ( i = 0; i < bloc_count - 1; i++) {
            if (memory[adr_pr + i].next != adr_pr + i + 1) {
                is_contiguous = 0;
                break;
            }
        }

        if (is_contiguous) {
            printf("Le fichier '%s' est dÃ©jÃ  contigu. Aucune dÃ©fragmentation nÃ©cessaire.\n", file_name);
            return;
        }
    }

    // Trouver un espace contigu suffisant pour le fichier
    int new_adr_pr = Contigue(memory, bloc_count);
    if (new_adr_pr == -1) {
        printf("Erreur : Impossible de trouver un espace contigu pour dÃ©fragmenter le fichier '%s'.\n", file_name);
        return;
    }

    // Copier les blocs du fichier vers le nouvel emplacement contigu
    int current = adr_pr;
    int new_current = new_adr_pr;
    while (current != -1) {
        memory[new_current] = memory[current]; // Copier le bloc
        memory[new_current].id = new_current; // Mettre Ã  jour le numÃ©ro du bloc
        memory[new_current].next = (new_current + 1 < new_adr_pr + bloc_count) ? new_current + 1 : -1; // Mettre Ã  jour le lien
        memory[current].is_occupied = 0; // LibÃ©rer l'ancien bloc
        strcpy(memory[current].file_name, ""); // Effacer le nom du fichier de l'ancien bloc
        memory[current].record_count = 0; // RÃ©initialiser le compteur d'enregistrements
        memory[current].next = -1; // RÃ©initialiser le lien
        current = memory[current].next; // Passer au bloc suivant
        new_current++;
    }

    // Mettre Ã  jour les mÃ©tadonnÃ©es du fichier
    modifier_metadonne(file_name, 1, new_adr_pr); // Nouvelle adresse de dÃ©part
    

    printf("DÃ©fragmentation du fichier '%s' terminÃ©e. Nouvelle adresse de dÃ©part : %d\n", file_name, new_adr_pr);
}

// Fonction pour supprimer un fichier
void supprimer_fichier(const char *file_name) {
	int i;
    // Vérifier si le fichier existe
    int adr_pr = read_metadonne(file_name, 1);  // Adresse du premier bloc
    int bloc_count = read_metadonne(file_name, 3);  // Nombre de blocs
    int mode = read_metadonne(file_name, 4);  // Mode d'allocation (1 = contigu, 0 = chaîné)

    if (adr_pr == -1 || bloc_count <= 0) {
        printf("Erreur : fichier '%s' introuvable ou déjà supprimé.\n", file_name);
        return;
    }

    // Libérer les blocs
    if (mode == 1) {
        // Mode contigu
        for ( i = adr_pr; i < adr_pr + bloc_count; i++) {
            memory[i].is_occupied = 0;  // Marquer le bloc comme libre
            strcpy(memory[i].file_name, "");  // Effacer le nom de fichier
            memory[i].record_count = 0;  // Réinitialiser le compteur d'enregistrements
        }
    } else if (mode == 0) {
        // Mode chaîné
        int current = adr_pr;
        while (current != -1) {
            int next = memory[current].next;  // Sauvegarder l'adresse du bloc suivant
            memory[current].is_occupied = 0;  // Libérer le bloc actuel
            strcpy(memory[current].file_name, "");  // Effacer le nom de fichier
            memory[current].record_count = 0;  // Réinitialiser le compteur d'enregistrements
            memory[current].next = -1;  // Réinitialiser le chaînage
            current = next;  // Passer au bloc suivant
        }
    }

    // Mettre à jour les métadonnées pour marquer le fichier comme supprimé
    supp_metadonnes(file_name);

    printf("Fichier '%s' supprimé avec succès.\n", file_name);
}

// Clear memory
void clear_memory() {
	int i;
	int j;
    for ( i = 0; i < total_blocks; i++) {
        memory[i].is_occupied = 0;  // Marquer comme libre
        strcpy(memory[i].file_name, "");  // Effacer le nom du fichier
        memory[i].record_count = 0;  // Reinitialiser le nombre d'enregistrements
        memory[i].next = -1;
        for ( j = 0; j < Fact_Blockage; j++) {
            memory[i].r[j].activ = 0;
            strcpy(memory[i].r[j].data, "");
        }
    }
    printf("\nMÃ©moire vidÃ©e avec succÃ¨s.\n");
}

// Menu functions
void afficher_menu() {
    printf("\n=================== MENU ===================\n");
    printf("1. Initialiser la mÃ©moire\n");
    printf("2. Afficher l'Ã©tat de la mÃ©moire\n");
    printf("3. Vider la mÃ©moire\n");
    printf("4. CrÃ©er un fichier\n");
    printf("5. InsÃ©rer un enregistrement\n");
    printf("6. Chercher un enregistrement\n");
    printf("7. Supprimer un enregistrement (logique)\n");
    printf("8. Supprimer un enregistrement (physique)\n");
    printf("9. Compactage de la mÃ©moire\n");
    printf("10. DÃ©fragmenter un fichier\n");
    printf("11. Afficher les mÃ©tadonnÃ©es\n");
    printf("12. Supprimer un fichier\n");
    printf("0. Quitter\n");
    printf("===========================================\n");
    printf("Choisissez une option : ");
}

void menu() {
    int choix, id, record_count, mode;
    char file_name[MAX_FILE_NAME];
    char data[100];
    Record *record;

    do {
        afficher_menu();
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                initialize_memory();
                break;
            case 2:
                display_memory_state();
                break;
            case 3:
                clear_memory();
                break;
            case 4:
                printf("Nom du fichier : ");
                scanf("%s", file_name);
                printf("Nombre d'enregistrements : ");
                scanf("%d", &record_count);
                printf("Mode (1 = contigu, 0 = chaÃ®nÃ©) : ");
                scanf("%d", &mode);
                crier_fichier(file_name, record_count, mode);
                break;
            case 5:
                printf("Nom du fichier : ");
                scanf("%s", file_name);
                printf("ID de l'enregistrement : ");
                scanf("%d", &id);
                printf("DonnÃ©es : ");
                scanf("%s", data);
                insertRecord(file_name, id, data);
                break;
            case 6:
                printf("Nom du fichier : ");
                scanf("%s", file_name);
                printf("ID de l'enregistrement : ");
                scanf("%d", &id);
                record = searchRecord(file_name, id);
                if (record) {
                    printf("Enregistrement trouvÃ© : ID = %d, Data = %s\n",
                           record->id, record->data);
                }
                break;
            case 7:
                printf("Nom du fichier : ");
                scanf("%s", file_name);
                printf("ID de l'enregistrement : ");
                scanf("%d", &id);
                deleteRecordLogical(file_name, id);
                break;
            case 8:
                printf("Nom du fichier : ");
                scanf("%s", file_name);
                printf("ID de l'enregistrement : ");
                scanf("%d", &id);
                deleteRecordPhysical(file_name, id);
                break;
            case 9:
                compactMemory(memory);
                break;
            case 10:
                printf("Nom du fichier Ã  dÃ©fragmenter : ");
                scanf("%s", file_name);
                defragmenter_fichier(file_name);
                break;
            case 11:
                afficher_metadonnes();
                break;
            case 12:
                printf("Nom du fichier Ã  supprimer : ");
                scanf("%s", file_name);
                supprimer_fichier(file_name);
                break;
            case 0:
                printf("Au revoir!\n");
                break;
            default:
                printf("Option invalide!\n");
        }
    } while (choix != 0);
}

int main() {
    menu();
    return 0;
}
