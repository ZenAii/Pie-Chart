#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>
#include <string.h>
#include <gdfontg.h>
#include <gdfontmb.h>
#include <gdfontl.h>

// Constantes pour la taille de l'image
const int imgLargeur = 800;
const int imgHauteur = 700;

// Constantes pour la taille de la police
const int taillePoliceTitre = 20;
const int taillePoliceLabel = 10;

// Structure pour stocker les données d'un segment de Pie Chart
typedef struct
{
    double pourcentage;
    char *etiquette;
    int rouge;
    int vert;
    int bleu;
} SegmentPie;

// Fonction pour libérer la mémoire allouée aux étiquettes des segments
void libererEtiquettesSegments(SegmentPie *segments, int nbSegments)
{
    for (int i = 0; i < nbSegments; i++)
    {
        free(segments[i].etiquette);
    }
}

// Fonction pour créer un graphique Pie Chart avec les données fournies et les options de personnalisation
void creerPieChart(SegmentPie *segments, int nbSegments, const char *fichierSortie, const char *titre, int couleurFondRouge, int couleurFondVert, int couleurFondBleu, int couleurBordureRouge, int couleurBordureVert, int couleurBordureBleu, int utiliserCouleurFondPersonnalisee)
{
    // Vérifier que la somme des pourcentages est égale à 100
    double pourcentageTotal = 0.0;
    for (int i = 0; i < nbSegments; i++)
    {
        pourcentageTotal += segments[i].pourcentage;
    }
    if (fabs(pourcentageTotal - 100.0) > 0.001)
    {
        printf("Erreur : La somme des pourcentages des segments n'est pas égale à 100.\n");
        return;
    }

    // Créer une image avec fond transparent ou avec la couleur de fond personnalisée
    gdImagePtr img;
    if (utiliserCouleurFondPersonnalisee)
    {
        img = gdImageCreateTrueColor(imgLargeur, imgHauteur);
        int couleurFond = gdImageColorAllocate(img, couleurFondRouge, couleurFondVert, couleurFondBleu);
        gdImageFill(img, 0, 0, couleurFond);
    }
    else
    {
        img = gdImageCreateTrueColor(imgLargeur, imgHauteur);
        gdImageSaveAlpha(img, 1);
        int transparent = gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaTransparent);
        gdImageFill(img, 0, 0, transparent);
    }

    // Initialiser les variables pour le dessin du Pie Chart
    int centreX = imgLargeur / 2;
    int centreY = imgHauteur / 2;
    int rayon = imgHauteur / 4;
    int angleDepart = 0;
    int angleFin = 0;

    // Dessiner le contour du Pie Chart
    int couleurBordure = gdImageColorAllocate(img, couleurBordureRouge, couleurBordureVert, couleurBordureBleu);
    gdImageFilledEllipse(img, centreX, centreY, (rayon + 2) * 2, (rayon + 2) * 2, couleurBordure);

    // Dessiner le Pie Chart
    for (int i = 0; i < nbSegments; i++)
    {
        SegmentPie segment = segments[i];

        angleFin = angleDepart + (int)(360 * segment.pourcentage / 100);

        int couleur = gdImageColorAllocate(img, segment.rouge, segment.vert, segment.bleu);

        gdImageFilledArc(img, centreX, centreY, rayon * 2, rayon * 2, angleDepart, angleFin, couleur, gdPie);

        int angleMilieu = (angleDepart + angleFin) / 2;
        int departX = centreX + (int)(rayon * cos(angleDepart * M_PI / 180));
        int departY = centreY + (int)(rayon * sin(angleDepart * M_PI / 180));
        int finX = centreX + (int)(rayon * cos(angleFin * M_PI / 180));
        int finY = centreY + (int)(rayon * sin(angleFin * M_PI / 180));

        gdImageLine(img, centreX, centreY, departX, departY, couleurBordure);
        gdImageLine(img, centreX, centreY, finX, finY, couleurBordure);

        // Dessiner le titre à côté du segment
        int rayonTitre = rayon + 70;
        int titreX = centreX + (int)((rayonTitre + taillePoliceLabel) * cos(angleMilieu * M_PI / 180));
        int titreY = centreY + (int)((rayonTitre + taillePoliceLabel) * sin(angleMilieu * M_PI / 180));

        double facteurEchelleEtiquette = 3;
        int taillePoliceEtiquetteEchellee = (int)(taillePoliceLabel * facteurEchelleEtiquette);
        gdImageString(img, gdFontMediumBold, titreX, titreY, (unsigned char *)segment.etiquette, couleurBordure);

        int departLigneX = centreX + (int)(rayon * cos(angleMilieu * M_PI / 180));
        int departLigneY = centreY + (int)(rayon * sin(angleMilieu * M_PI / 180));
        gdImageLine(img, departLigneX, departLigneY, titreX, titreY, couleurBordure);

        // Afficher le pourcentage à côté du segment
        char pourcentageStr[10];
        snprintf(pourcentageStr, sizeof(pourcentageStr), "%.1f%%", segment.pourcentage);
        int pourcentageX = centreX + (int)((rayonTitre + taillePoliceLabel + 25) * cos(angleMilieu * M_PI / 180));
        int pourcentageY = centreY + (int)((rayonTitre + taillePoliceLabel + 25) * sin(angleMilieu * M_PI / 180));

        gdImageString(img, gdFontMediumBold, pourcentageX, pourcentageY, (unsigned char *)pourcentageStr, couleurBordure);

        angleDepart = angleFin;
    }

    // Calculer la largeur du titre global
    int largeurTitre = gdFontGiant->w * strlen(titre);

    // Calculer la position X pour centrer le titre horizontalement
    int titreX = centreX - (largeurTitre / 2);
    int titreY = 30; // Position verticale pour le titre centré en haut de l'image

    // Afficher le titre centré en haut de l'image
    gdImageString(img, gdFontGiant, titreX, titreY, (unsigned char *)titre, couleurBordure);

    // Sauvegarder l'image dans le fichier de sortie
    FILE *sortie = fopen(fichierSortie, "wb");
    if (sortie == NULL)
    {
        printf("Erreur : Impossible de créer le fichier de sortie.\n");
        gdImageDestroy(img);
        return;
    }
    gdImagePng(img, sortie);
    fclose(sortie);

    // Libérer la mémoire
    gdImageDestroy(img);
    libererEtiquettesSegments(segments, nbSegments);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Utilisation: %s <fichier_sortie.png> <0|1>\n", argv[0]);
        return 1;
    }

    int utiliserCouleurFondPersonnalisee = atoi(argv[2]);

    // Exemple de données pour le Pie Chart
    SegmentPie segments[] = {
        {15.0, strdup("Segment 1"), 255, 0, 0},   // Rouge
        {5.0, strdup("Segment 2"), 0, 255, 0},    // Vert
        {15.0, strdup("Segment 3"), 0, 0, 255},   // Bleu
        {15.0, strdup("Segment 4"), 255, 255, 0}, // Jaune
        {50.0, strdup("Segment 5"), 255, 0, 255}  // Violet
    };

    int nbSegments = sizeof(segments) / sizeof(segments[0]);

    // Appeler la fonction pour créer le Pie Chart avec personnalisation supplémentaire
    creerPieChart(segments, nbSegments, argv[1], "Exemple de Pie Chart", 255, 255, 255, 0, 0, 0, utiliserCouleurFondPersonnalisee);

    return 0;
}
