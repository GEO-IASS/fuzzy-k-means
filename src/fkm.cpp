/*

   Implementation of fuzzy-k-means clustering algorithm multidimensional

   Based on David Robison implementation at https://github.com/drobison/k-means

*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>
#include "point.h"

using namespace std;

ifstream infile;
ofstream outfile;

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

int k = 5; // k = default number of centroids
int dimensions = 5;  // Dimensions
int dataCount = 0;  // dataCount = number of pointes read in input.txt divided by dimension
int numberCount = 0; // numberCount = quantity of numbers read in input file

void calculateWeights(int);
void assignCentroid(PointDataSet* item, int point);
void calculateNewCentroids();

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

  // Initialize counter 
  dataCount = 0;
  numberCount = 0;

  outfile.open(output.c_str());

  // Read in input file.
  // Each line of input file must be two doubles (x and y)
  infile.open(input.c_str());
  if (!infile) {
    cout << "Unable to open input."<< endl;
    return 1;
  }

  srand (time(NULL));

//  for (int i = 0; i < 10; i++) {
//    dataCount++;
//    items.resize(i+1);
//    items[i].coord.resize(dimensions);
//    items[i].weightCentroids.resize(k);
//    for (int j = 0; j < dimensions; j++)
//      items[i].coord[j] = (double)(rand() % 10000) / 100;
//  }

  while(!infile.eof()) {
    // Verifiy if it's necessary create another item
    if (numberCount % dimensions == 0) {
      items.resize(dataCount+1);
      items[dataCount].coord.resize(dimensions);
      items[dataCount].weightCentroids.resize(k);
      dataCount++;
    }
    double newItem = 0;
    infile >> newItem; // read new value
    cout << "Numero do arquivo: " << newItem << endl;
    
    items[dataCount-1].coord[numberCount % dimensions] = newItem; // load new value on position
    // Calculate max and min values for current dimension
    if (newItem > maxOnDim[numberCount % dimensions]) maxOnDim[numberCount % dimensions] = newItem;
    if (newItem < minOnDim[numberCount % dimensions]) minOnDim[numberCount % dimensions] = newItem;
    numberCount++;
  }

  // Close input file
  infile.close();

  // Randomly chose initial centroids.
  for( int i=0; i<k; i++) {
    for (int j = 0; j < dimensions; j++)  
      centroid[i].coord[j] = (double)(rand() % 10000) / 100;
      //  (double)(rand() % (((int)(maxOnDim[j] * 1000) - (int)(minOnDim[j] * 1000)) + (int)(minOnDim[j] * 1000))) / 1000;
    // For debugging
    cout << "Centroid " << i << ", "<< centroid[i] << endl;
    outfile << "Centroid " << i << ", " << centroid[i] << endl;
  }

  // Once define the first set of centroids, 
  // is need to calculate the weights per point, per centroid using fuzzifier 2
  calculateWeights(2);

  // Open output

  // Run 4 times, calculanting new centroids 
  for( int i=0; i < 20; i++) {
    calculateNewCentroids();
    calculateWeights(2); // Calculate Weight with fuzzifier 2 
  }

  outfile.close();

}

// Function calculateWeights
//
// Parameter m indicate fuzzifier
//
// Each point on the space has a degree that express its belonging to each centroid
//
//
void calculateWeights(int m=2) {

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
void calculateNewCentroids() {

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
      Point pW = items[j].weightCentroids[i] * items [j];
      pSum = pSum + pW;
      count += items[j].weightCentroids[i];
    }

    // Calculate new coordinate of centroid i
    centroid[i] = (1.0/count) * pSum;

    // Calculate distance btw new centroid to old centroid
    double movement = centroid[i].dist(oldCentroid[i]);

    cout << "New Centroid " << i << ":" << centroid[i] << "\t";
    outfile << "New Centroid " << i << ":" << centroid[i] << endl;
    cout << "Centroid moved " << movement << endl;
  }
  cout << endl;
  outfile << endl;

}
