#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>
#include <atomic>

using namespace std;

atomic<bool> closed_deadlock(false);

// Protect the counters used during entrance and exit
mutex mutexEntrance_deadlock;
mutex mutexExit_deadlock;

// Control how many spectators are allowed to enter or exit the theater
counting_semaphore<> entranceQueue_deadlock{0};
counting_semaphore<> exitQueue_deadlock{0};

// Notify the theater when all spectators have entered or exited
counting_semaphore<> allInside_deadlock{0};
counting_semaphore<> allOutside_deadlock{0};

// Track the number of inbounds and outbounds
unsigned inbounds_deadlock(0);
unsigned outbounds_deadlock(0);

void play_deadlock()
{
    // Simulate the beginning of the movie
    cout << "Nous vous souhaitons une bonne séance" << endl;
}

void startSession_deadlock(unsigned places_deadlock)
{
    // Announce that the session is ready to start
    cout << "Prenez place ! La séance démarrera juste après la pub ! "
         << endl;

    // deadlock is here
    allInside_deadlock.acquire();
    entranceQueue_deadlock.release(places_deadlock);

}

void stopSession_deadlock(unsigned places_deadlock)
{
    // Announce that the session is over
    cout << "Merci de vous diriger vers la sortie, n'oubliez pas de jeter vos déchets."
         << endl;

    exitQueue_deadlock.release(places_deadlock);
    allOutside_deadlock.acquire();
}

void takeASeat_deadlock(unsigned places_deadlock)
{
    // Simulate a spectator taking a seat for the session
    // Update the number of spectators who have entered

    mutexEntrance_deadlock.lock();

    cout << "Un spectateur rentre dans la salle de cinéma et s'installe confortablement."
         << endl;

    inbounds_deadlock += 1;

    // Notify the theater when all spectators have entered
    if (inbounds_deadlock == places_deadlock)
    {
        allInside_deadlock.release();
        inbounds_deadlock = 0;
    }

    mutexEntrance_deadlock.unlock();
}

void leaveASeat_deadlock(unsigned places_deadlock)
{
    // Simulate a spectator leaving their seat at the end of the session
    // Update the number of spectators who have left

    mutexExit_deadlock.lock();

    cout << "Un spectateur sort de la salle de cinéma après avoir jeté ses déchets."
         << endl;

    outbounds_deadlock += 1;

    // Notify the theater when all spectators have left
    if (outbounds_deadlock == places_deadlock)
    {
        allOutside_deadlock.release();
        outbounds_deadlock = 0;
    }

    mutexExit_deadlock.unlock();
}

void theater_function_deadlock(
    unsigned places_deadlock,
    unsigned nbViewers_deadlock)
{
    // Number of full sessions we can run
    unsigned nbSessions_deadlock =
        nbViewers_deadlock / places_deadlock;

    for (unsigned session_deadlock = 0;
         session_deadlock < nbSessions_deadlock;
         ++session_deadlock)
    {
        startSession_deadlock(places_deadlock);
        play_deadlock();
        stopSession_deadlock(places_deadlock);
    }

    // Not enough spectators left: close the theater
    closed_deadlock = true;

    unsigned remaining_deadlock =
        nbViewers_deadlock % places_deadlock;

    if (remaining_deadlock > 0)
    {
        entranceQueue_deadlock.release(remaining_deadlock);
    }

    cout << "Le cinéma ferme, pas assez de spectateurs pour une nouvelle séance."
         << endl;
}

void spectator_function_deadlock(unsigned places_deadlock)
{
    // Wait for permission to enter the theater
    entranceQueue_deadlock.acquire();

    // Woken up after closing: go home without entering
    if (closed_deadlock)
    {
        cout << "Un spectateur repart, le cinéma est fermé." << endl;
        return;
    }

    takeASeat_deadlock(places_deadlock);

    exitQueue_deadlock.acquire();

    leaveASeat_deadlock(places_deadlock);
}

int main_deadlock()
{
    // Define the number of seats and spectators
    unsigned places_deadlock(5);
    unsigned nbViewers_deadlock(12);

    // Check that there are at least as many spectators as seats
    if (nbViewers_deadlock < places_deadlock)
    {
        cerr << "Condition non respectée : places < nombre de spectateurs"
             << endl;

        return 1;
    }

    // Create the theater thread
    thread theater_deadlock(
        theater_function_deadlock,
        places_deadlock,
        nbViewers_deadlock);

    // Create one thread for each spectator
    vector<thread> spectators_deadlock;

    for (unsigned spectatorNumber_deadlock = 0;
         nbViewers_deadlock > spectatorNumber_deadlock;
         spectatorNumber_deadlock++)
    {
        spectators_deadlock.emplace_back(
            spectator_function_deadlock,
            places_deadlock);
    }

    // Wait for all spectator threads to finish
    for (thread& viewer_deadlock : spectators_deadlock)
    {
        viewer_deadlock.join();
    }

    // Wait for the theater thread to finish
    theater_deadlock.join();

    return 0;
}