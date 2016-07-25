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
ofstream logfile;

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

int k = 4; // k = default number of centroids
double m = 2.0; // m = fuzzifier
int dimensions = 5;  // Dimensions
int iterations = 20; // Max number of iterations
int dataCount = 0;  // dataCount = number of pointes read in input.txt divided by dimension
int numberCount = 0; // numberCount = quantity of numbers read in input file

void calculateWeights(double m);
void assignCentroid(PointDataSet* item, int point);
void calculateNewCentroids(double m);

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

double fRand(double fMin, double fMax)
{
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

int main(int argc, char* argv[]) {

  string input = "input.txt";
  string output = "output.txt";

  // Initialize max and min vector to store values of each dimnensio
  // Used to help random routine
  vector<double> maxOnDim;
  vector<double> minOnDim;

  cout << "Fuzzy k-means algorithm" << endl;
  cout << "developed by ricardo brandao - https://github.com/programonauta/fuzzy-k-means" << endl;
  cout << "=============================================================================" << endl;
  cout << "Log file: output.log" << endl;
  
  // Verify command line options
  
  // Show help message if -h or there is no command options
  if (argc <= 1 || cmdOptionExists(argv, argv+argc, "-h")) {
    cout << "OVERVIEW: Algorithm to clustering data assigning data points to clusters" << endl;
    cout << "          The program output node and edges file in a format to be imported by Gephi software" << endl;
    cout << endl;
    cout << "USAGE: fkm <options>" << endl;
    cout << endl;
    cout << "OPTIONS: " << endl;
    cout << "-k <value>\tnumber of clusters or centroids" << endl;
    cout << "-m <value>\tfuzzifier" << endl;
    cout << "-d <value>\tdimension, or number of attributes" << endl;
    cout << "-i <value>\tmax number of iterations" << endl;
    cout << "-if <file>\tinput file" << endl;
    cout << "-nf <file>\tnode suffix file: suffix of output file which will have nodes." << endl;
    cout << "          \t                  Will be generated one file for each cluster. The files names: <suffix>knnn.txt where nnn is the number of cluster." << endl;
    cout << "-ef <file>\tedge file: file generated with edges to be imported by Gephi software" << endl;
    return 0;
  }

  // Open log file
  logfile.open("output.log");

  logfile << "Fuzzy k-means algorithm" << endl;
  logfile << "developed by ricardo brandao - https://github.com/programonauta/fuzzy-k-means" << endl;
  logfile << "=============================================================================" << endl;
  logfile << "Log file: output.log" << endl;

  // Option -k: Number of centroids
  if (cmdOptionExists(argv, argv+argc, "-k"))
    k = atoi(getCmdOption(argv, argv+argc, "-k"));

  if (k < 1)
    k = 2;

  cout << "Number of centroids: " << k << endl;
  logfile<< "Number of centroids: " << k << endl;
  
  // Option -m: Fuzzifier
  if (cmdOptionExists(argv, argv+argc, "-m")) 
    m = atof(getCmdOption(argv, argv+argc, "-m"));

  if (m <= 1)
    m = 2.0;

  cout << "Fuzzifier: " << m << endl;
  logfile << "Fuzzifier: " << m << endl;

  // Option -d: dimension
  if (cmdOptionExists(argv, argv+argc, "-d"))
    dimensions = atoi(getCmdOption(argv, argv+argc, "-d"));

  if (dimensions < 2)
    dimensions = 5;

  cout << "Dimension: " << dimensions << endl;
  logfile << "Dimension: " << dimensions << endl;
 
  // Option -i: iterations
  if (cmdOptionExists(argv, argv+argc, "-i"))
    iterations = atoi(getCmdOption(argv, argv+argc, "-i"));

  if (iterations < 1)
    iterations = 10;

  cout << "Iterations: " << iterations << endl;
  logfile << "Iterations: " << iterations << endl;
 
  if (cmdOptionExists(argv, argv+argc, "-if")) {
    input = std::string(getCmdOption(argv, argv+argc, "-if"));

    if (input == "")
      input = "input.txt";
  }
    
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

  // Read in input file.
  // Each line of input file must be two doubles (x and y)
  infile.open(input.c_str());
  if (!infile) {
    cout << "Unable to open " << input << " file"<< endl << endl;
    logfile << "Could not open file: " << input << endl;
    logfile.close();
    return 1;
  }

  cout << "-----------------------------------------------------------------------------" << endl;
  cout << "Input file " << input << " read successfully" << endl; 
  logfile << "Input file " << input << " read successfully" << endl; 

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
  logfile << dataCount << " data read" << endl;

  // Close input file
  infile.close();

  // Randomly chose initial centroids.
  for( int i=0; i<k; i++) {
    for (int j = 0; j < dimensions; j++)  
      centroid[i].coord[j] = fRand(minOnDim[j], maxOnDim[j]); 

    logfile << "Centroid " << i << ", "<< centroid[i] << endl;
  }

  // Once define the first set of centroids, 
  // is need to calculate the weights per point, per centroid using fuzzifier 2
  calculateWeights(m);

  // Run calculanting new centroids 
  for( int i=0; i < iterations; i++) {
    // Logging iterations
    logfile << "Iteration " << i+1 << endl;
    calculateNewCentroids(m);
    calculateWeights(m); // Calculate Weight with fuzzifier 2 
  }

  output = "edges.txt";
  // Open edges.txt file
  if (cmdOptionExists(argv, argv+argc, "-ef")) {
    output = std::string(getCmdOption(argv, argv+argc, "-ef"));

    if (output == "")
      output = "edges.txt";
  }
    
  FILE* pFile = fopen(output.c_str(), "w");
  
  cout << "-----------------------------------------------------------------------------" << endl;
  cout << "writing edges on file " << output << endl;
  logfile << "writing edges on file " << output << endl;
  // Write header of edge file

  // For each point in dataset define edges for centroids
 
  fprintf(pFile, "Id,Source,Target,Type\n");

  int id=0;
  
  for (int i = 0; i < dataCount; i++) 
    for (int j = 0; j < dimensions; j++)
      if (items[i].coord[j])
        fprintf(pFile, "%d,%d,%d,Undirected,%d\n", 1+id++, i+1, j+1, 1); 

  fclose(pFile);

  char buffer[256];

  output = "nodes";
  // Open edges.txt file
  if (cmdOptionExists(argv, argv+argc, "-nf")) {
    output = std::string(getCmdOption(argv, argv+argc, "-nf"));

    if (output == "")
      output = "nodes";
  }

  for (int i = 0; i < k; i++) {
    snprintf(buffer, 256, "%sk%03d.txt", output.c_str(), i+1);
    pFile = fopen(buffer, "w");
    cout << "writing nodes with pertinence of cluster " << i+1 << " on file " << buffer << endl;
    logfile << "writing nodes with pertinence of cluster " << i+1 << " on file " << buffer << endl;
    fprintf(pFile, "Id,Label,Pertinence\n");
    for (int j = 0; j < dataCount; j++) { 
      fprintf(pFile,"%d,%d,%f\n", j+1, j+1, items[j].weightCentroids[i]);
    }
    fclose(pFile);
  }

  logfile << "data\tweights" << endl;
  for (int i=0; i<dataCount; i++) {
    logfile << i+1 << "\t";
    for (int j=0; j<k; j++)
      logfile << (j>0? ", ": "") << items[i].weightCentroids[j];
    logfile << endl;
  }
  logfile.close();

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

      items[i].weightCentroids[j] = ((1 / sumW <= 1.0e-4)?0:1/sumW);

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

    logfile << "New Centroid " << i << ":" << centroid[i] << "\t";
    logfile << "Centroid moved " << movement << endl;
  }
  logfile << endl;

}
