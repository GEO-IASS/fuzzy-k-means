/*

   Implementation of fuzzy-k-means clustering algorithm multidimensional

   Based on David Robison implementation at https://github.com/drobison/k-means

*/

#include <iostream>
#include <cstdio> 
#include <cstdlib> 
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>
#include "point.h"

using namespace std;

ifstream infile;

class PointDataSet : public Point {

  // Just include a vector to register the wheigt for each centroid
  // The dimension of this vector must be the number of centroids
  public:

    vector<double> weightCentroids;

};

// Define the vector that contais the entire dataset
vector<PointDataSet> items;
// Vectors to store centroids
vector<Point> centroid;
vector<Point> oldCentroid;

int k = 2; // k = default number of centroids
double m = 1.01; // m = fuzzifier
int dimensions = 34;  // Dimensions
int dataCount = 0;  // dataCount = number of pointes read in input.txt divided by dimension
int numberCount = 0; // numberCount = quantity of numbers read in input file

void calculateWeights(double m);
void assignCentroid(PointDataSet* item, int point);
void calculateNewCentroids(double m);

int main(int argc, char* argv[]) {

  string input = "input.txt";
  string output = "output.txt";

  // Initialize max and min vector to store values of each dimnensio
  // Used to help random routine
  vector<double> maxOnDim;
  vector<double> minOnDim;

  // Set max and min vectors based on qtd of dimensions
  maxOnDim.resize(dimensions);
  minOnDim.resize(dimensions);

  // Set dimensions for centroids vectors
  centroid.resize(k);
  oldCentroid.resize(k);

  for (int i = 0; i < k; i++) {
    centroid[i].coord.resize(dimensions);
    oldCentroid[i].coord.resize(dimensions);
  }

  // Set max and min for each dimensions
  // These values will limit 
  for (int i = 0; i < dimensions; i++) {
    maxOnDim[i] = 0;
    minOnDim[i] = 9.99e+20;
  }

  cout << "Fuzzy k-means algorithm" << endl;
  cout << "developed by ricardo brandao - https://github.com/programonauta/fuzzy-k-means" << endl;
  cout << "=============================================================================" << endl;

  // Initialize counter 
  dataCount = 0;
  numberCount = 0;


  // Read in input file.
  // Each line of input file must be two doubles (x and y)
  infile.open(input.c_str());
  if (!infile) {
    cout << "Unable to open input.txt file"<< endl << endl;
    return 1;
  }

  cout << "Input file read successfully" << endl; 

  srand (time(NULL));
    
  double newItem = 0;

  infile >> newItem; // read new value

  while(!infile.eof()) {
    // Verifiy if it's necessary create another item
    if (numberCount % dimensions == 0) {
      items.resize(dataCount+1);
      items[dataCount].coord.resize(dimensions);
      items[dataCount].weightCentroids.resize(k);
      dataCount++;
    }
    
    items[dataCount-1].coord[numberCount % dimensions] = newItem; // load new value on position
    // Calculate max and min values for current dimension
    if (newItem > maxOnDim[numberCount % dimensions]) maxOnDim[numberCount % dimensions] = newItem;
    if (newItem < minOnDim[numberCount % dimensions]) minOnDim[numberCount % dimensions] = newItem;
    numberCount++;
    infile >> newItem; // read new value
  }

  cout << dataCount << " data read" << endl;

  // Close input file
  infile.close();

  // Randomly chose initial centroids.
  for( int i=0; i<k; i++) {
    for (int j = 0; j < dimensions; j++)  
      //centroid[i].coord[j] = (double)(rand() % 10000) / 100;
      centroid[i].coord[j] =  (double)(rand() % (((int)(maxOnDim[j] * 1000000) - (int)(minOnDim[j] * 1000000)) + (int)(minOnDim[j] * 1000000))) / 1000000;
    // For debugging
    cout << "Centroid " << i << ", "<< centroid[i] << endl;
  }

  // Once define the first set of centroids, 
  // is need to calculate the weights per point, per centroid using fuzzifier 2
  calculateWeights(1.1);

  // Run calculanting new centroids 
  for( int i=0; i < 20; i++) {
    calculateNewCentroids(m);
    calculateWeights(m); // Calculate Weight with fuzzifier 2 
  }

  // Open edges.txt file
  output = "edges.txt";
  FILE* pFile = fopen("edges.txt", "w");
  
  // Write header of edge file

  // For each point in dataset define edges for centroids
 
  fprintf(pFile, "Id,Source,Target,Type\n");

  int id;

  for (int i = 0; i < dataCount; i++) 
    for (int j = 0; j < dimensions; j++)
      if (items[i].coord[j])
        fprintf(pFile, "%d,%d,%d,Undirected,%d\n", 1+id++, i+1, j+1, 1); 

  fclose(pFile);

  pFile = fopen("nodes.txt", "w");

  fprintf(pFile, "Id,Label,Pertinency\n");

  for (int i = 0; i < dataCount; i++)
    fprintf(pFile,"%d,%d,%f\n", i+1, i+1, items[i].weightCentroids[0]);

  fclose(pFile);
  
}

// Function calculateWeights
//
// Parameter m indicate fuzzifier
//
// Each point on the space has a degree that express its belonging to each centroid
//
//
void calculateWeights(double m=2.0) {

  double sumW = 0; // sum of weights

  // Iterate all points, for each point calculate the weight
  for (int i = 0; i < dataCount; i++) {
  
    // Iterate all centroids
    for (int j = 0; j < k; j++) {
      // Iterated centroids again to calculate the weight

      sumW = 0;
      for (int intK = 0; intK < k; intK++) {
        sumW += pow((items[i].dist(centroid[j])/items[i].dist(centroid[intK])), (2 / (m-1))); 
      }

      items[i].weightCentroids[j] = 1 / sumW;

    }
 
    sumW = 0;
    for (int j = 0; j < k; j++)
      sumW += items[i].weightCentroids[j];

  }

}
void calculateNewCentroids(double m = 2.0) {

  // iterate centroids (k = number of centroids)
  for( int i=0; i<k; i++)
  {
    // save old k th centroid
    oldCentroid[i] = centroid[i];

    Point pSum;;
    double count = 0;

    pSum.coord.resize(dimensions);
    // Initialize pSum
    for (int i = 0; i < dimensions; i++)
      pSum.coord[i] = 0.0;

    // Calculate center of mass of centroid k, calculating average of x and y
    // Iterate items
    for( int j=0; j < dataCount; j++)
    {
      // Sum the weighted vector of item j, to centroid i  
      Point pW = pow(items[j].weightCentroids[i], m) * items [j];
      pSum = pSum + pW;
      count += pow(items[j].weightCentroids[i], 2);
    }

    // Calculate new coordinate of centroid i
    centroid[i] = (1.0/count) * pSum;

    // Calculate distance btw new centroid to old centroid
    double movement = centroid[i].dist(oldCentroid[i]);

    cout << "New Centroid " << i << ":" << centroid[i] << "\t";
    cout << "Centroid moved " << movement << endl;
  }
  cout << endl;

}
