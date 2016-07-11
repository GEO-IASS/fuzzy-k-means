/*

   Implementation of fuzzy-k-means clustering algorithm multidimensional

   Based on David Robison implementation at https://github.com/drobison/k-means

*/

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <math.h>

using namespace std;

ifstream infile;
ofstream outfile;

// Class to represent points 

class Point {

  public:
    double x, y;
    double weightCentroid[10]; // The weight (degree) to which element belongs to centroid

    // Constructor with default values 0.0, 0.0
    Point(double x=0.0, double y=0.0):x(x),y(y) {};

    // Method to calculate the distance to another point

    double dist(Point other) {
      double xd = x - other.x;
      double yd = y - other.y;
      return sqrt(xd*xd + yd*yd);
    };
};

// Operator << 
ostream& operator<< (ostream&out, const Point& p) {
  out << "( " << p.x << ", " << p.y << " )";
  return out;
}

// Operator +
Point operator+ (Point& p1, Point& p2) {
  Point sum = {p1.x + p2.x, p1.y + p2.y};
  return sum;
}

// Operator *
Point operator* (double m1, Point& p1) {
  Point mult = {m1 * p1.x, m1 * p1.y};
  return mult;
}

Point items[1000];
Point centroid[10];
Point oldCentroid[10];

int k = 4;   // k = default number of centroids
int centroidCount[10];  // number of items by centroid
int dataCount = 0;  // dataCount = number of lines input.txt

void calculateWeights(int);
void assignCentroid(Point* item, int point);
void calculateNewCentroids();

int main(int argc, char* argv[])
{
  string input = "input.txt";
  string output = "output.txt";

  // Initialize max and min values to help random routine
  double minX = 9.99e+20, minY = 9.99e+20;
  double maxX = 0, maxY = 0;

  // Initialize counter 
  dataCount = 0;

  outfile.open(output.c_str());

  // Read in input file.
  // Each line of input file must be two doubles (x and y)
  infile.open(input.c_str());
  if (!infile)
  {
    cout << "Unable to open input."<< endl;
  }
  while(!infile.eof())		
  {
    infile >> items[dataCount].x >> items[dataCount].y;
    if (items[dataCount].x > maxX) maxX = items[dataCount].x;
    if (items[dataCount].x < minX) minX = items[dataCount].x;
    if (items[dataCount].y > maxY) maxY = items[dataCount].y;
    if (items[dataCount].y < minY) minY = items[dataCount].y;
    dataCount++;
  }

  // Close input file
  infile.close();

  srand (time(NULL));

  // Randomly chose initial centroids.
  for( int i=0; i<k; i++)
  {
    centroid[i].x = (double)(rand() % (((int)(maxX * 1000) - (int)(minX * 1000)) + (int)(minX * 1000))) / 1000;
    centroid[i].y = (double)(rand() % (((int)(maxY * 1000) - (int)(minY * 1000)) + (int)(minY * 1000))) / 1000;

    // For debugging
    cout << "Centroid " << i << ", "<< centroid[i] << endl;
    outfile << "Centroid " << i << ", " << centroid[i] << endl;
  }

  // Once define the first set of centroids, 
  // is need to calculate the weights per point, per centroid
  calculateWeights(2);

  // Show items by centroid
  for( int i=0; i<k; i++)
  {
    cout << "Number of items centroid " << i << ": " << centroidCount[i] << endl;
  }
  // Open output

  // Run 4 times, calculanting new centroids 
  for( int i=0; i < 20; i++)
  {
    calculateNewCentroids();
    calculateWeights(2); 
  }

  outfile.close();

}

// Function calculateWeights
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

      items[i].weightCentroid[j] = 1 / sumW;

    }
  
  }

}
void calculateNewCentroids()
{

  // iterate centroids (k = number of centroids)
  for( int i=0; i<k; i++)
  {
    // save old k th centroid
    oldCentroid[i] = centroid[i];

    Point pSum = {0.0, 0.0};
    double count = 0;

    // Calculate center of mass of centroid k, calculating average of x and y
    // Iterate items
    for( int j=0; j < dataCount; j++)
    {
      // Sum the weighted vector of item j, to centroid i  
      Point pW = items[j].weightCentroid[i] * items [j];
      pSum = pSum + pW;
      count += items[j].weightCentroid[i];
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
