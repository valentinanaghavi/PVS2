#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define NUM 32767                                             // Elementanzahl

// ---------------------------------------------------------------------------
// Vertausche zwei Zahlen im Feld v an der Position i und j

void swap(float *v, int i, int j)
{
    float t = v[i]; 
    v[i] = v[j];
    v[j] = t;
}

// ---------------------------------------------------------------------------
// Serielle Version von Quicksort (Wirth) 

void quicksort(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;


    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);

   if (start < j)                                        // Teile und herrsche
       quicksort(v, start, j);                      // Linkes Segment zerlegen
   if (i < end)
       quicksort(v, i, end);                       // Rechtes Segment zerlegen


}
// ---------------------------------------------------------------------------
// 1. Versuch

void quicksort_1(float *v, int start, int end, int depth) 
{
    int i = start, j = end;
    float pivot;


    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);

   if (depth<=1)
   {
       if (start < j)                                        // Teile und herrsche
           quicksort(v, start, j);                      // Linkes Segment zerlegen
       if (i < end)
           quicksort(v, i, end);                       // Rechtes Segment zerlegen
   }
   else
   {
       #pragma omp parallel
       {
              
       if (start < j)                                        // Teile und herrsche
           quicksort_1(v, start, j, depth-1);                      // Linkes Segment zerlegen
       if (i < end)
           quicksort_1(v, i, end, depth-1);                       // Rechtes Segment zerlegen
        }
   }
}
// ---------------------------------------------------------------------------
// 2. Versuch

void quicksort_2(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;


    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);
   

   #pragma omp parallel sections 
   {
       #pragma omp section 
       {
           if (start < j)                                        // Teile und herrsche
           quicksort_2(v, start, j);                      // Linkes Segment zerlegen
       }
        #pragma omp section
        {
           if (i < end)
           quicksort_2(v, i, end);                       // Rechtes Segment zerlegen

        }
   }

}
// ---------------------------------------------------------------------------
// 3. Versuch

void quicksort_3(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;


    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);

   if (start < j)                                        // Teile und herrsche
       #pragma omp task
       quicksort_3(v, start, j);                      // Linkes Segment zerlegen
   if (i < end)
        #pragma omp task
       quicksort_3(v, i, end);                       // Rechtes Segment zerlegen


}
// ---------------------------------------------------------------------------
// Testfunktion
bool test(float *v)
{
    for(int i = 0 ; i< NUM; i++)
    {
        if (v[i] <= v[i+1])
        {
            return true;
        }
        else
        {
            return false;
            break;
        }
    }
}
// ---------------------------------------------------------------------------
// Hauptprogramm

int main(int argc, char *argv[])
{
    double t_start, t_end;
    float *v;                                                         // Feld                               
    int iter;                                                // Wiederholungen             

    if (argc != 2) {                                      // Benutzungshinweis
        printf ("Vector sorting\nUsage: %s <NumIter>\n", argv[0]); 
        return 0;
    }
    iter = atoi(argv[1]);                               
    v = (float *) calloc(NUM, sizeof(float));        // Speicher reservieren

    printf("Perform vector sorting %d times...\n", iter);
    printf("\n seriell: \n");
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++){      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();
        }
        
        //serielle Version
        t_start = omp_get_wtime();
        quicksort(v, 0, NUM-1);
        t_end = omp_get_wtime();                                  // Sortierung
        printf("Benoetigte Zeit: %f Sekunden \n", t_end-t_start);
        test(v);
        printf("Test erfolgreich : %d \n", test(v));
    }
    
    printf ("\nDone.\n");
//-------------------------------------------------------------------------------------
    //1. Versuch

    /*  Durch das Verschachtelungsprinzip wurde versucht die Rekusionaufrufe zu parallelisieren, um den Algorithmus zu beschleunigen. Dieser ist jedoch minimal langsamer geworden. Dabei ist unklar ob alle 
    Rekursionsaufrufe parallel laufen oder jeweils nur die beiden(linke und rechte Hälfte des zu sortierenden Bereichs).
    */
    printf("\n 1. Versuch: \n");

    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++){      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();
        }
        
        t_start = omp_get_wtime();
        quicksort_1(v, 0, NUM-1, (int)log(NUM));
        t_end = omp_get_wtime();                                  // Sortierung
        printf("Benoetigte Zeit: %f Sekunden \n", t_end-t_start);
        test(v);
        printf("Test erfolgreich : %d \n", test(v));
    }
    
    printf ("\nDone.\n");
//-------------------------------------------------------------------------
    //2. Versuch

    /*  Die beiden if-Bedingungen in zwei sections aufzuteilen ist deutlich langsamer als den Algorithmus seriell durchlaufen zu lassen. 
    Da die sections von bestimmten Threads abhängig sind, muessen diese warten bis die Threads frei sind.
    */
    printf("\n 2. Versuch: \n");
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++){      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();
        }
        
        t_start = omp_get_wtime();
        quicksort_2(v, 0, NUM-1);
        t_end = omp_get_wtime();                                  // Sortierung
        printf("Benoetigte Zeit: %f Sekunden \n", t_end-t_start);
        test(v);
        printf("Test erfolgreich : %d \n", test(v));
    }
    
    printf ("\nDone.\n");

//-------------------------------------------------------------------------
    //3. Versuch

    /*  Bei dem 3. Versuch wurden die Rekursionaufrufe auf Tasks verteilt. Da diese ungebunden sind kann ein Task den nächstbesten freien Thread nutzen und auf ihm weiterarbeiten.
    Diese Methode ist bisher die, die den Algorithmus am schnellsten durchlaeuft. 
    */
    printf("\n 3. Versuch: \n");
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++){      // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();
        }
        
        t_start = omp_get_wtime();
        #pragma omp parallel
        {
          #pragma omp single  
        quicksort_3(v, 0, NUM-1);
        }
        t_end = omp_get_wtime();                                  // Sortierung
        printf("Benoetigte Zeit: %f Sekunden \n", t_end-t_start);
        test(v);
        printf("Test erfolgreich : %d \n", test(v));
    }
    
    printf ("\nDone.\n");



    return 0;
}