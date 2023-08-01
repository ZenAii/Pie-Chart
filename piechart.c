#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>
#include <string.h>
#include <gdfontg.h>
#include <gdfontmb.h>
#include <gdfontl.h>

// Constantes pour la taille de l'image
const int imgWidth = 800;
const int imgHeight = 700;

// Constantes pour la taille de la police
const int titleFontsize = 20;
const int labelFontsize = 10;

// Structure pour stocker les données d'un segment de Pie Chart
typedef struct
{
    double percentage;
    char *label;
    int red;
    int green;
    int blue;
} PieSegment;

// Fonction pour libérer la mémoire allouée aux labels des segments
void freeSegmentLabels(PieSegment *segments, int numSegments)
{
    for (int i = 0; i < numSegments; i++)
    {
        free(segments[i].label);
    }
}

// Fonction pour créer un graphique Pie Chart avec les données fournies et les options de personnalisation
void createPieChart(PieSegment *segments, int numSegments, const char *outputFile, const char *title, int bgColorRed, int bgColorGreen, int bgColorBlue, int borderColorRed, int borderColorGreen, int borderColorBlue, int useCustomBgColor)
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

    // Créer une image avec fond transparent ou avec la couleur de fond personnalisée
    gdImagePtr img;
    if (useCustomBgColor)
    {
        img = gdImageCreateTrueColor(imgWidth, imgHeight);
        int bgColor = gdImageColorAllocate(img, bgColorRed, bgColorGreen, bgColorBlue);
        gdImageFill(img, 0, 0, bgColor);
    }
    else
    {
        img = gdImageCreateTrueColor(imgWidth, imgHeight);
        gdImageSaveAlpha(img, 1);
        int transparent = gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaTransparent);
        gdImageFill(img, 0, 0, transparent);
    }

    // Initialiser les variables pour le dessin du Pie Chart
    int centerX = imgWidth / 2;
    int centerY = imgHeight / 2;
    int radius = imgHeight / 4;
    int startAngle = 0;
    int endAngle = 0;

    // Dessiner le contour du Pie Chart
    int borderColor = gdImageColorAllocate(img, borderColorRed, borderColorGreen, borderColorBlue);
    gdImageFilledEllipse(img, centerX, centerY, (radius + 2) * 2, (radius + 2) * 2, borderColor);

    // Dessiner le Pie Chart
    for (int i = 0; i < numSegments; i++)
    {
        PieSegment segment = segments[i];

        endAngle = startAngle + (int)(360 * segment.percentage / 100);

        int color = gdImageColorAllocate(img, segment.red, segment.green, segment.blue);

        gdImageFilledArc(img, centerX, centerY, radius * 2, radius * 2, startAngle, endAngle, color, gdPie);

        int midAngle = (startAngle + endAngle) / 2;
        int startX = centerX + (int)(radius * cos(startAngle * M_PI / 180));
        int startY = centerY + (int)(radius * sin(startAngle * M_PI / 180));
        int endX = centerX + (int)(radius * cos(endAngle * M_PI / 180));
        int endY = centerY + (int)(radius * sin(endAngle * M_PI / 180));

        gdImageLine(img, centerX, centerY, startX, startY, borderColor);
        gdImageLine(img, centerX, centerY, endX, endY, borderColor);

        // Dessiner le titre à côté du segment
        int titleRadius = radius + 70;
        int titleX = centerX + (int)((titleRadius + labelFontsize) * cos(midAngle * M_PI / 180));
        int titleY = centerY + (int)((titleRadius + labelFontsize) * sin(midAngle * M_PI / 180));

        double labelScaleFactor = 3;
        int labelFontSizeScaled = (int)(labelFontsize * labelScaleFactor);
        gdImageString(img, gdFontMediumBold, titleX, titleY, (unsigned char *)segment.label, borderColor);

        int lineStartX = centerX + (int)(radius * cos(midAngle * M_PI / 180));
        int lineStartY = centerY + (int)(radius * sin(midAngle * M_PI / 180));
        gdImageLine(img, lineStartX, lineStartY, titleX, titleY, borderColor);

        // Afficher le pourcentage à côté du segment
        char percentageStr[10];
        snprintf(percentageStr, sizeof(percentageStr), "%.1f%%", segment.percentage);
        int percentageX = centerX + (int)((titleRadius + labelFontsize + 25) * cos(midAngle * M_PI / 180));
        int percentageY = centerY + (int)((titleRadius + labelFontsize + 25) * sin(midAngle * M_PI / 180));

        gdImageString(img, gdFontMediumBold, percentageX, percentageY, (unsigned char *)percentageStr, borderColor);

        startAngle = endAngle;
    }

    // Calculer la largeur du titre global
    int titleWidth = gdFontGiant->w * strlen(title);

    // Calculer la position X pour centrer le titre horizontalement
    int titleX = centerX - (titleWidth / 2);
    int titleY = 30; // Position verticale pour le titre centré en haut de l'image

    // Afficher le titre centré en haut de l'image
    gdImageString(img, gdFontGiant, titleX, titleY, (unsigned char *)title, borderColor);

    // Sauvegarder l'image dans le fichier de sortie
    FILE *output = fopen(outputFile, "wb");
    if (output == NULL)
    {
        printf("Erreur : Impossible de créer le fichier de sortie.\n");
        gdImageDestroy(img);
        return;
    }
    gdImagePng(img, output);
    fclose(output);

    // Libérer la mémoire
    gdImageDestroy(img);
    freeSegmentLabels(segments, numSegments);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Utilisation: %s <fichier_sortie.png> <0|1>\n", argv[0]);
        return 1;
    }

    int useCustomBgColor = atoi(argv[2]);

    // Exemple de données pour le Pie Chart
    PieSegment segments[] = {
        {15.0, strdup("Segment 1"), 255, 0, 0},   // Rouge
        {5.0, strdup("Segment 2"), 0, 255, 0},    // Vert
        {15.0, strdup("Segment 3"), 0, 0, 255},   // Bleu
        {15.0, strdup("Segment 4"), 255, 255, 0}, // Jaune
        {50.0, strdup("Segment 5"), 255, 0, 255}  // Violet
    };

    int numSegments = sizeof(segments) / sizeof(segments[0]);

    // Appeler la fonction pour créer le Pie Chart avec personnalisation supplémentaire
    createPieChart(segments, numSegments, argv[1], "Exemple de Pie Chart", 255, 255, 255, 0, 0, 0, useCustomBgColor);

    return 0;
}
