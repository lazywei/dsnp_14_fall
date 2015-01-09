#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
using namespace std;
 
enum GateType { PI, AIG, UNDEF };
 
class Gate
{
public:
   Gate(GateType _type = UNDEF): type(_type)
   {
      fanin[0] = fanin[1] = -1;
   }
   void randomizeFanin(int* randMap)
   {
      if(type == AIG)
      {
         fanin[0] = 2*randMap[fanin[0]/2]+(fanin[0]%2);
         fanin[1] = 2*randMap[fanin[1]/2]+(fanin[1]%2);
      }
   }
   GateType type;
   int fanin[2];
};
 
int main(int argc, char* argv[])
{
   srand(time(NULL));
   bool randomize = false;
   if(argc <= 2 || argc >= 5)
   {
      cerr << "usage: aagGen N output_file [-r]\n";
      return 1;
   }
   if(argc == 4)
   {
      if(!strcmp(argv[3], "-r"))
      {
         randomize = true;
      }
   }
   ofstream ofile(argv[2]);
   if(!ofile)
   {
      cerr << "Error opening " << argv[2] << " for writing!\n";
      return 1;
   }
   int N = atoi(argv[1]), M = 0, I = 0, O = 0, A = 0;
   stringstream aigStream(""), poStream(""); // for saving gates temporarily
   Gate* gates = new Gate[N]();
   I = N * (20 + rand()%10) / 100; // 15~25% are PIs
   for(int i=1;i<=I;i++)
   {
      gates[i].type = PI;
   }
   // others are AIGs and UNDEFs
   for(int i=I+1;i<N;i++)
   {
      int P1 = rand()%100;
      if(P1 < 0)
      {
         gates[i].type = UNDEF;
      }
      else
      {
         gates[i].type = AIG; // some AIGs may have no fanins, so not count here
      }
   }
   // generate random links
   for(int i=1;i<N;i++)
   {
      if(gates[i].type == AIG)
      {
         int N1 = rand()%i, N2 = rand()%i;
         gates[i].fanin[0] = 2*N1+rand()%2;
         gates[i].fanin[1] = 2*N2+rand()%2;
      }
   }
   // generate a random map
   // i => randMap[i]
   int* randMap = NULL;
   if(randomize)
   {
      randMap = new int[N+1]();
      for(int i=1;i<N;i++)
      {
         randMap[i] = i;
      }
      for(int i=N-1;i>=2;i--)
      {
         int idx = rand()%(i-1)+1; // 0 (CONST) is not touched
         // swapping
         int temp = randMap[idx];
         randMap[idx] = randMap[i];
         randMap[i] = temp;
      }
      for(int i=1;i<N;i++)
      {
         gates[i].randomizeFanin(randMap);
      }
   }
   if(randomize)
   {
      for(int i=1;i<N;i++)
      {
         // count AIGs and output to a string stream
         if(gates[i].type == AIG && gates[i].fanin[0] != -1)
         {
            A++;
            aigStream << 2*randMap[i] << ' ' << gates[i].fanin[0] << ' ' << gates[i].fanin[1] << "\n";
         }
         // find maximum gate number
         if(randMap[i] > M)
         {
            M = randMap[i];
         }
      }
   }
   else
   {
      for(int i=1;i<N;i++)
      {
         // count AIGs and output to a string stream
         if(gates[i].type == AIG && gates[i].fanin[0] != -1)
         {
            A++;
            aigStream << 2*i << ' ' << gates[i].fanin[0] << ' ' << gates[i].fanin[1] << "\n";
         }
         // find maximum gate number
         if(i > M)
         {
            M = i;
         }
      }
   }
   // find "defined but not used"
   bool* noFanout = new bool[N]();
   for(int i=1;i<N;i++)
   {
      noFanout[i] = true;
   }
   for(int i=1;i<N;i++)
   {
      if(gates[i].fanin[0] != -1)
      {
         noFanout[gates[i].fanin[0]/2] = false;
         noFanout[gates[i].fanin[1]/2] = false;
      }
   }
   if(randomize)
   {
      for(int i=1;i<N;i++)
      {
         if(noFanout[i] && (rand()%10 < 10))
         {
            O++;
            poStream << 2*randMap[i] << "\n";
         }
      }
   }
   else
   {
      for(int i=1;i<N;i++)
      {
         if(noFanout[i] && (rand()%10 < 10))
         {
            O++;
            poStream << 2*i << "\n";
         }
      }
   }
   // output header
   ofile << "aag " << M << ' ' << I << " 0 " <<  O << ' ' << A << "\n";
   // output PIs
   if(randomize)
   {
      for(int i=1;i<N;i++)
      {
         if(gates[i].type == PI)
         {
            ofile << 2*randMap[i] << "\n";
         }
      }
   }
   else
   {
      for(int i=1;i<N;i++)
      {
         if(gates[i].type == PI)
         {
            ofile << 2*i << "\n";
         }
      }
   }
   ofile << poStream.str() << aigStream.str().substr(0, aigStream.str().size());
   ofile.close();
   if(randomize)
   {
      delete [] randMap;
   }
   delete [] noFanout;
   delete [] gates;
   return 0;
}
