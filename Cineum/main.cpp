#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>

using namespace std;

// protect the counters used during entrance and exit
mutex mutexEntrance;
mutex mutexExit;

//Control how many spectators are allowed to enter or exit the theater
counting_semaphore<> entranceQueue{0};
counting_semaphore<> exitQueue{0};

// Notify the theater when all the spectators have enter or exit
counting_semaphore<> allInside{0};
counting_semaphore<> allOutside{0};

// Track the number of the inbounds and outbounds
unsigned inbounds (0);
unsigned outbounds (0);

void play(){
    // simulate the beginning of the movie
    cout << "Nous vous souhaitons une bonne séance" << endl;
}

void startSession(){
    //Annouce that the session is ready to be start
    cout << "Prenez place ! La séance démarrera juste après la pub ! " << endl;
}

void stopSession(){
    //Annouce that the session is over
    cout << "Merci de vous diriger vers la sortie, n'oubliez pas de jeter vos dechets." << endl;
}

void takeASeat(){
    //Simulate a spectator taking a seat for the session
    cout << "Un spectateur rentre dans la salle de cinéma et s'installe confortablement." << endl;
}

void leaveASeat(){
    //simulate a spectator leaving a seat at the end of the session
    cout << "Un spectateur sort de la salle de cinéma après avoir jeter ses déchets." << endl;
}

void theater_function(unsigned C){

    startSession();
    entranceQueue.release(C); //signal
    allInside.acquire(); //wait

    play();

    stopSession();
    exitQueue.release(C);
    allOutside.acquire();

}

void spectator_function(unsigned C){
    entranceQueue.acquire();
    takeASeat();

    mutexEntrance.lock();
    inbounds +=1;
    if (inbounds==C){
        allInside.release();
        inbounds=0;
    }
    mutexEntrance.unlock();

    exitQueue.acquire();
    leaveASeat();

    mutexExit.lock();
    outbounds +=1;
    if (outbounds==C){
        allOutside.release();
        outbounds=0;
    }
    mutexExit.unlock();
}


int main()
{
    unsigned places (5);
    unsigned nbViewers(11);
    if (nbViewers <  places )
    {
        cerr << "Condition non respectée : places < nombre de spectateurs" << endl;
        return 1;
    }
    thread theater(theater_function,places);


    vector<thread> spectators;
    for (unsigned n=0; nbViewers>n; n++){
        spectators.emplace_back(spectator_function, places);
    }

    for (thread & viewer : spectators){
        viewer.join();
    }

    theater.join();

    return 0;

}
