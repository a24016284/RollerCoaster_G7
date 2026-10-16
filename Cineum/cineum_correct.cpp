#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>
#include <atomic>

using namespace std;

atomic<bool> closed(false);

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

void startSession(unsigned places){
    //Annouce that the session is ready to be start
    cout << "Prenez place ! La séance démarrera juste après la pub ! " << endl;
    entranceQueue.release(places);
    allInside.acquire();
}

void stopSession(unsigned places){
    //Annouce that the session is over
    cout << "Merci de vous diriger vers la sortie, n'oubliez pas de jeter vos dechets." << endl;
    exitQueue.release(places);
    allOutside.acquire();
}

void takeASeat(unsigned places){
    //Simulate a spectator taking a seat for the session
    //Update the number of spectators who have entered
    mutexEntrance.lock();
    cout << "Un spectateur rentre dans la salle de cinéma et s'installe confortablement." << endl;
    inbounds +=1;
    //Notify the theater when all C spectators have entered
    if (inbounds==places){
        allInside.release();
        inbounds=0;
    }
    mutexEntrance.unlock();
}

void leaveASeat(unsigned places){
    //Simulate a spectator leaving their seat at the end of the session
    //Update the number of spectators who have left
    mutexExit.lock();
    cout << "Un spectateur sort de la salle de cinéma après avoir jeter ses déchets." << endl;
    outbounds +=1;

    //Notify the theater when all C spectators have left
    if (outbounds==places){
        allOutside.release();
        outbounds=0;
    }
    mutexExit.unlock();
}

void theater_function(unsigned places, unsigned nbViewers){
    // Number of full sessions we can run
    unsigned nbSessions = nbViewers / places;

    for (unsigned s = 0; s < nbSessions; ++s) {
        startSession(places);
        play();
        stopSession(places);
    }

    // Not enough spectators left: close the theater
    closed = true;
    unsigned remaining = nbViewers % places;
    if (remaining > 0) {
        entranceQueue.release(remaining);   // wake up the remaining spectators
    }
    cout << "Le cinéma ferme, pas assez de spectateurs pour une nouvelle séance." << endl;
}

void spectator_function(unsigned places){
    // Wait for permission to enter the theater
    entranceQueue.acquire();

    // Woken up after closing: go home without entering
    if (closed) {
        cout << "Un spectateur repart, le cinéma est fermé." << endl;
        return;
    }

    takeASeat(places);
    exitQueue.acquire();
    leaveASeat(places);
}

int main_correct()
{
    //Define the number of seats and spectators
    unsigned places (5);
    unsigned nbViewers(12);
    //Check that there are at least as many spectators as seats
    if (nbViewers <  places )
    {
        cerr << "Condition non respectée : places < nombre de spectateurs" << endl;
        return 1;
    }
    //Create the theater thread
    thread theater(theater_function,places, nbViewers);


    //Create one thread for each spectator
    vector<thread> spectators;
    for (unsigned n=0; nbViewers>n; n++){
        spectators.emplace_back(spectator_function, places);
    }

    //Wait for all spectator threads to finish
    for (thread & viewer : spectators){
        viewer.join();
    }

    //Wait for the theater thread to finish
    theater.join();

    return 0;

}
