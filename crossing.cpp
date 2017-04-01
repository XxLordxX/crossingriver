#include <thread> 
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <mutex> 
#include <vector>
class person //Each thread is a person
{

   protected: 
   int gen;

   public:
   person () {
       gen = rand() % 2; //GEN = 1 (Windows) else Linux
   }

   int whoAreYou () {
       return gen; 
   }

   int timeWait () {	//A randon number to thread wait
       return rand () % 7;
   } 

};




class riverCrossing
{

private:

int windowsUsers; // windows and linux count 
int linuxUsers;

std::mutex mtx; //Mutex - Não sei se vale;


public:

riverCrossing() {
   windowsUsers = 0;
   linuxUsers = 0;
}


void* crossing (void *) {

   person x;

   int gen  = x.whoAreYou();
   int time = x.timeWait();

   sleep(time);  

   buyTicket(gen);


	//implementar a barreira
   std::cout<<"Bye Bye " << gen <<std::endl;
}

void nowBoarding() { // When all pre-requisites are ok 
    std::cout << "Windows and Linux Users Welcome a board, here is the Captain" << std::endl; 

}

void buyTicket (int gen) {

    if(gen == 1){ //Windows
	
	mtx.lock();
	windowsUsers++;
	
	if(windowsUsers >= 2) {
	    if(windowsUsers >=3) {
		windowsUsers = windowsUsers -3;
	 	nowBoarding();
	    }
	    else if(linuxUsers >= 1) {
		windowsUsers = windowsUsers -2;
		linuxUsers   = linuxUsers -1;
		nowBoarding(); 
	    }
	}
	mtx.unlock();	
    }
    else {
	mtx.lock();
	if (linuxUsers >= 2) {
	 
	   if(linuxUsers >= 3) {
		linuxUsers = linuxUsers -3;
		nowBoarding();
	    } else if (windowsUsers >= 1) {
		linuxUsers = linuxUsers - 2;
		windowsUsers = windowsUsers -1;
	    } 
	
	}
	mtx.unlock();
    }
}

};

int main() 

{

   riverCrossing Arizona;

   std::vector<pthread_t*> people;
   people.resize(15);

   for (int i = 0; i < people.size(); i++){
	pthread_create(people[i], NULL, Arizona.crossing, (void*) 0);	
   }
   
   for (int i = 0; i < people.size(); i++)
        pthread_join(people[i], NULL);


}


