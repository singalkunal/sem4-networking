#include <bits/stdc++.h>
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;

unsigned short cksum(unsigned short *buf,int count){

 unsigned long sum=0;
while(count--){
    sum+=*buf++;
    if(sum&0xFFFF0000){
        sum&=0xFFFF;
        sum++;
    }
    }

return ~(sum & 0xFFFF);
}
int main() {

int k=0;

   ifstream binFile("icmp.pcap", ios::in | ios::binary);

    unsigned short* bits= new unsigned short;

   char *Byte = new char; 
   binFile.read(Byte, 1);                   
   cout << setfill('0') << uppercase;       

   int byteCount = 0;

          int c=0;                          
   while ( !binFile.eof() ) {                     

    if(byteCount>=54&&c<20){
        cout << "  " << hex << setw(2)<< (short(*Byte) & 0x00FF);
        unsigned short temp=(short(*Byte) & 0x00FF);
        temp*=256;
        binFile.read(Byte, 1);
        cout << "  " << hex << setw(2)<< (short(*Byte) & 0x00FF);
        temp+= (short(*Byte) & 0x00FF);
        bits[k++]=temp;
                           
      c+=2;
      if (c % 8 == 0) {
         if (c % 16 == 0)
            cout << endl;
         else
            cout << "    ";
      }
      binFile.read(Byte, 1);
    }
    else{
         byteCount++;
    binFile.read(Byte, 1);
    }

   }
   cout << endl;
    cout<<"Check Sum:"<<cksum(bits,10)<<endl;

   binFile.close();
   return 0;
}
