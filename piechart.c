#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>
#include <string.h>
#include <gdfontg.h>
#include <gdfontmb.h>
#include <gdfontl.h>

// Structure pour stocker les données d'un segment de Pie Chart
typedef struct
{
    double percentage;
    char *label;
    int red;
    int green;
    int blue;
} PieSegment;

// Fonction pour créer un graphique Pie Chart avec les données fournies et les options de personnalisation
void createPieChart(PieSegment *segments, int numSegments, const char *outputFile, const char *title, int titleFontsize, int labelFontsize)
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

    // Taille de l'image
    int imgWidth = 800;
    int imgHeight = 700;

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

    gdImageFilledEllipse(img, centerX, centerY, (radius + 2) * 2, (radius + 2) * 2, gdImageColorAllocate(img, 0, 0, 0));

    // Dessiner le Pie Chart
    for (int i = 0; i < numSegments; i++)
    {
        PieSegment segment = segments[i];

        endAngle = startAngle + (int)(360 * segment.percentage / 100);

        int color = gdImageColorAllocate(img, segment.red, segment.green, segment.blue);
        int borderColor = gdImageColorAllocate(img, 0, 0, 0);

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
        gdImageString(img, gdFontMediumBold, titleX, titleY, (unsigned char *)segment.label, gdImageColorAllocate(img, 0, 0, 0));

        int lineStartX = centerX + (int)(radius * cos(midAngle * M_PI / 180));
        int lineStartY = centerY + (int)(radius * sin(midAngle * M_PI / 180));
        gdImageLine(img, lineStartX, lineStartY, titleX, titleY, borderColor);

        // Afficher le pourcentage à côté du segment
        char percentageStr[10];
        snprintf(percentageStr, sizeof(percentageStr), "%.1f%%", segment.percentage);
        int percentageX = centerX + (int)((titleRadius + labelFontsize + 25) * cos(midAngle * M_PI / 180));
        int percentageY = centerY + (int)((titleRadius + labelFontsize + 25) * sin(midAngle * M_PI / 180));

        gdImageString(img, gdFontMediumBold, percentageX, percentageY, (unsigned char *)percentageStr, gdImageColorAllocate(img, 0, 0, 0));

        startAngle = endAngle;
    }

    // Calculer la largeur du titre global
    int titleWidth = gdFontMediumBold->w * strlen(title);

    // Calculer la position X pour centrer le titre horizontalement
    int titleX = centerX - (titleWidth / 2);
    int titleY = 100; // Position verticale fixe pour le titre

    double titleScaleFactor = 1.5;
    int titleFontSizeScaled = (int)(titleFontsize * titleScaleFactor);
    gdImageString(img, gdFontGiant, titleX, titleY, (unsigned char *)title, gdImageColorAllocate(img, 0, 0, 0));

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

    int labelFontsize = 10;
    int titleFontsize = 20;

    // Appeler la fonction pour créer le Pie Chart avec personnalisation supplémentaire
    createPieChart(segments, numSegments, argv[1], "Exemple de Pie Chart", titleFontsize, labelFontsize);

    return 0;
}
