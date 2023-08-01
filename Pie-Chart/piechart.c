#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>
#include <string.h>
#include <gdfontg.h>
#include <gdfontmb.h>
#include <gdfontl.h>

// Constantes pour la taille de l'image
const int IMG_LARGEUR = 800;
const int IMG_HAUTEUR = 700;

// Constantes pour la taille de la police
const int TAILLE_POLICE_TITRE = 20;
const int TAILLE_POLICE_LABEL = 10;

// Structure pour stocker les données d'un segment de Pie Chart
typedef struct
{
    double pourcentage;
    const char *etiquette;
    int couleur[3]; // Tableau pour stocker les composantes RVB (rouge, vert, bleu)
} SegmentPie;

// Fonction pour créer un graphique Pie Chart avec les données fournies et les options de personnalisation
void creerPieChart(SegmentPie *segments, int nbSegments, const char *fichierSortie, const char *titre, int couleurFond[3], int couleurBordure[3], int CustomBGColor)
{
    // Vérifier que la somme des pourcentages est égale à 100
    double pourcentageTotal = 0.0;
    for (int i = 0; i < nbSegments; i++)
    {
        pourcentageTotal += segments[i].pourcentage;
    }
    if (fabs(pourcentageTotal - 100.0) > 0.001)
    {
        fprintf(stderr, "Erreur : La somme des pourcentages des segments n'est pas égale à 100.\n");
        return;
    }

    // Créer une image avec fond transparent ou avec la couleur de fond personnalisée
    gdImagePtr img;
    if (CustomBGColor)
    {
        img = gdImageCreateTrueColor(IMG_LARGEUR, IMG_HAUTEUR);
        int couleurFondImage = gdImageColorAllocate(img, couleurFond[0], couleurFond[1], couleurFond[2]);
        gdImageFill(img, 0, 0, couleurFondImage);
    }
    else
    {
        img = gdImageCreateTrueColor(IMG_LARGEUR, IMG_HAUTEUR);
        gdImageSaveAlpha(img, 1);
        int transparent = gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaTransparent);
        gdImageFill(img, 0, 0, transparent);
    }

    // Initialiser les variables pour le dessin du Pie Chart
    int centreX = IMG_LARGEUR / 2;
    int centreY = IMG_HAUTEUR / 2;
    int rayon = IMG_HAUTEUR / 4;
    int angleDepart = 0;
    int angleFin = 0;

    // Dessiner le contour du Pie Chart
    int couleurBordureImage = gdImageColorAllocate(img, couleurBordure[0], couleurBordure[1], couleurBordure[2]);
    gdImageFilledEllipse(img, centreX, centreY, (rayon + 2) * 2, (rayon + 2) * 2, couleurBordureImage);

    // Dessiner le Pie Chart
    for (int i = 0; i < nbSegments; i++)
    {
        SegmentPie segment = segments[i];

        angleFin = angleDepart + (int)(360 * segment.pourcentage / 100);

        int couleurSegment = gdImageColorAllocate(img, segment.couleur[0], segment.couleur[1], segment.couleur[2]);
        gdImageFilledArc(img, centreX, centreY, rayon * 2, rayon * 2, angleDepart, angleFin, couleurSegment, gdPie);

        int angleMilieu = (angleDepart + angleFin) / 2;
        int departX = centreX + (int)(rayon * cos(angleDepart * M_PI / 180));
        int departY = centreY + (int)(rayon * sin(angleDepart * M_PI / 180));
        int finX = centreX + (int)(rayon * cos(angleFin * M_PI / 180));
        int finY = centreY + (int)(rayon * sin(angleFin * M_PI / 180));

        gdImageLine(img, centreX, centreY, departX, departY, couleurBordureImage);
        gdImageLine(img, centreX, centreY, finX, finY, couleurBordureImage);

        // Dessiner le titre à côté du segment
        int rayonTitre = rayon + 70;
        int titreX = centreX + (int)((rayonTitre + TAILLE_POLICE_LABEL) * cos(angleMilieu * M_PI / 180));
        int titreY = centreY + (int)((rayonTitre + TAILLE_POLICE_LABEL) * sin(angleMilieu * M_PI / 180));

        double facteurEchelleEtiquette = 3;
        int taillePoliceEtiquetteEchellee = (int)(TAILLE_POLICE_LABEL * facteurEchelleEtiquette);
        gdImageString(img, gdFontMediumBold, titreX, titreY, (unsigned char *)segment.etiquette, couleurBordureImage);

        int departLigneX = centreX + (int)(rayon * cos(angleMilieu * M_PI / 180));
        int departLigneY = centreY + (int)(rayon * sin(angleMilieu * M_PI / 180));
        gdImageLine(img, departLigneX, departLigneY, titreX, titreY, couleurBordureImage);

        // Afficher le pourcentage à côté du segment
        char pourcentageStr[10];
        snprintf(pourcentageStr, sizeof(pourcentageStr), "%.1f%%", segment.pourcentage);
        int pourcentageX = centreX + (int)((rayonTitre + TAILLE_POLICE_LABEL + 25) * cos(angleMilieu * M_PI / 180));
        int pourcentageY = centreY + (int)((rayonTitre + TAILLE_POLICE_LABEL + 25) * sin(angleMilieu * M_PI / 180));

        gdImageString(img, gdFontMediumBold, pourcentageX, pourcentageY, (unsigned char *)pourcentageStr, couleurBordureImage);

        angleDepart = angleFin;
    }

    // Calculer la largeur du titre global
    int largeurTitre = gdFontGiant->w * strlen(titre);

    // Calculer la position X pour centrer le titre horizontalement
    int titreX = centreX - (largeurTitre / 2);
    int titreY = 30; // Position verticale pour le titre centré en haut de l'image

    // Afficher le titre centré en haut de l'image
    gdImageString(img, gdFontGiant, titreX, titreY, (unsigned char *)titre, couleurBordureImage);

    // Sauvegarder l'image dans le fichier de sortie
    FILE *sortie = fopen(fichierSortie, "wb");
    if (sortie == NULL)
    {
        fprintf(stderr, "Erreur : Impossible de créer le fichier de sortie.\n");
        gdImageDestroy(img);
        return;
    }
    gdImagePng(img, sortie);
    fclose(sortie);

    // Libérer la mémoire
    gdImageDestroy(img);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Utilisation: %s <fichier_sortie.png> <0|1>\n", argv[0]);
        return 1;
    }

    int CustomBGColor = atoi(argv[2]);

    // Exemple de données pour le Pie Chart
    SegmentPie segments[] = {
        {15.0, "Segment 1", {255, 0, 0}},   // Rouge
        {5.0, "Segment 2", {0, 255, 0}},    // Vert
        {15.0, "Segment 3", {0, 0, 255}},   // Bleu
        {15.0, "Segment 4", {255, 255, 0}}, // Jaune
        {50.0, "Segment 5", {255, 0, 255}}  // Violet
    };

    int nbSegments = sizeof(segments) / sizeof(segments[0]);

    // Appeler la fonction pour créer le Pie Chart avec personnalisation supplémentaire
    creerPieChart(segments, nbSegments, argv[1], "Exemple de Pie Chart", (int[3]){255, 255, 255}, (int[3]){0, 0, 0}, CustomBGColor);

    return 0;
}
