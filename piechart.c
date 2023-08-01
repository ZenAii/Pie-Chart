#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <gdfontmb.h>
#include <gdfontl.h>
#include <gdfontt.h>
#include <math.h>
#include <time.h>
#include <string.h>

// Nouvelle taille de police pour le titre du segment (personnalisable)
#define TITLE_SIZE_SEGMENT 20
// Nouvelle taille de police pour le titre global (personnalisable)
#define TITLE_SIZE_GLOBAL 20
#define TITLE_FONT "Roboto-Black.ttf" // Chemin vers votre fichier .ttf

// Structure pour stocker les données d'un segment de Pie Chart
typedef struct
{
    double percentage;
    char *label;
    int red;
    int green;
    int blue;
} PieSegment;

// Fonction pour dessiner du texte avec un effet de gras (bordure)
void gdImageStringBold(gdImagePtr im, gdFontPtr f, int x, int y, unsigned char *s, int color, int borderColor)
{
    // Dessiner le texte avec la couleur de la bordure
    gdImageString(im, f, x + 1, y, s, borderColor);
    gdImageString(im, f, x - 1, y, s, borderColor);
    gdImageString(im, f, x, y + 1, s, borderColor);
    gdImageString(im, f, x, y - 1, s, borderColor);

    // Dessiner le texte avec la couleur du texte
    gdImageString(im, f, x, y, s, color);
}

// Fonction pour créer un graphique Pie Chart avec les données fournies et les options de personnalisation
void createPieChart(PieSegment *segments, int numSegments, const char *outputFile, const char *title, int legendX, int legendY)
{
    // Vérifier que la somme des pourcentages est égale à 100
    double totalPercentage = 0.0;
    for (int i = 0; i < numSegments; i++)
    {
        totalPercentage += segments[i].percentage;
    }
    if (fabs(totalPercentage - 100.0) > 0.001)
    {
        printf("Erreur : La somme des pourcentages des segments n'est pas égale à 100.\n");
        return;
    }

    // Taille de l'image (personnalisable)
    int imgWidth = 1600;  // Augmenter la largeur de l'image
    int imgHeight = 1400; // Augmenter la hauteur de l'image

    // Créer une image avec fond transparent
    gdImagePtr img = gdImageCreateTrueColor(imgWidth, imgHeight);
    int bgColor = gdImageColorAllocateAlpha(img, 255, 255, 255, 127);
    gdImageFill(img, 0, 0, bgColor);

    // Initialiser les variables pour le dessin du Pie Chart
    int centerX = imgWidth / 2;
    int centerY = imgHeight / 2;
    int radius = imgHeight / 4;
    int startAngle = 0;
    int endAngle = 0;

    // Bords + épais
    gdImageSetThickness(img, 2);

    gdImageFilledEllipse(img, centerX, centerY, (radius + 2) * 2, (radius + 2) * 2, gdImageColorAllocate(img, 0, 0, 0));

    // Dessiner le Pie Chart
    for (int i = 0; i < numSegments; i++)
    {
        PieSegment segment = segments[i];

        endAngle = startAngle + (int)(360 * segment.percentage / 100);

        // Utiliser une couleur aléatoire si aucune n'est spécifiée
        int color = gdImageColorAllocate(img, segment.red, segment.green, segment.blue);
        int borderColor = gdImageColorAllocate(img, 0, 0, 0); // Black color for the border

        gdImageFilledArc(img, centerX, centerY, radius * 2, radius * 2, startAngle, endAngle, color, gdPie);

        // Dessiner un contour noir entre les segments
        int midAngle = (startAngle + endAngle) / 2;
        int startX = centerX + (int)(radius * cos(startAngle * M_PI / 180));
        int startY = centerY + (int)(radius * sin(startAngle * M_PI / 180));
        int endX = centerX + (int)(radius * cos(endAngle * M_PI / 180));
        int endY = centerY + (int)(radius * sin(endAngle * M_PI / 180));

        gdImageLine(img, centerX, centerY, startX, startY, borderColor);
        gdImageLine(img, centerX, centerY, endX, endY, borderColor);

        // Dessiner le titre à côté du segment (ajustement pour une meilleure position)
        int titleRadius = radius + 50;
        int titleX = centerX + (int)(titleRadius * cos(midAngle * M_PI / 180));
        int titleY = centerY + (int)(titleRadius * sin(midAngle * M_PI / 180));

        // Ajuster l'alignement horizontal en fonction de la direction du segment
        if (midAngle < 90 || midAngle > 270)
        {
            // Utiliser gdFontLarge pour le titre du segment
            gdImageString(img, gdFontLarge, titleX, titleY, (unsigned char *)segment.label, borderColor);
        }
        else
        {
            // Utiliser gdFontLarge pour le titre du segment
            gdImageString(img, gdFontLarge, titleX - TITLE_SIZE_SEGMENT * strlen(segment.label), titleY, (unsigned char *)segment.label, borderColor);
        }

        // Dessiner une ligne reliant le texte du bord du cercle au titre
        int lineStartX = centerX + (int)(radius * cos(midAngle * M_PI / 180));
        int lineStartY = centerY + (int)(radius * sin(midAngle * M_PI / 180));
        gdImageLine(img, lineStartX, lineStartY, titleX, titleY, borderColor);

        startAngle = endAngle;
    }

    gdImageSetThickness(img, 1);

    // Calculer la largeur du titre global
    int titleWidth = gdFontMediumBold->w * strlen(title);

    // Calculer la position X pour centrer le titre horizontalement
    int titleX = centerX - (titleWidth / 2);
    int titleY = 100; // Position verticale fixe pour le titre

    // Utiliser la police TrueType pour le titre global
    gdImageStringFT(img, &brect, 0, TITLE_FONT, TITLE_SIZE_GLOBAL, 0, titleX, titleY, (unsigned char *)title);

    // Sauvegarder l'image dans le fichier de sortie
    FILE *output = fopen(outputFile, "wb");
    gdImagePng(img, output);
    fclose(output);

    // Libérer la mémoire
    gdImageDestroy(img);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Utilisation: %s <fichier_sortie.png>\n", argv[0]);
        return 1;
    }

    // Exemple de données pour le Pie Chart
    PieSegment segments[] = {
        {15.0, "Segment 1", 255, 0, 0},  // Rouge
        {15.0, "Segment 2", 0, 255, 0},  // Vert
        {50.0, "Segment 3", 0, 0, 255},  // Bleu
        {5.0, "Segment 4", 255, 255, 0}, // Jaune
        {15.0, "Segment 5", 255, 0, 255} // Violet
    };

    int numSegments = sizeof(segments) / sizeof(segments[0]);

    // Appeler la fonction pour créer le Pie Chart avec personnalisation supplémentaire
    createPieChart(segments, numSegments, argv[1], "Exemple de Pie Chart", 1200, 200);

    return 0;
}
