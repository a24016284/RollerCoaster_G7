#include <iostream>
#include <thread>
#include <semaphore>
#include <vector>

using namespace std;

// commentaire à mettre
mutex mutexBoard;
mutex mutexUnboard;

counting_semaphore<> boardQueue{0};
counting_semaphore<> unBoardQueue{0};

counting_semaphore<> allAboard{0};
counting_semaphore<> allAshore{0};

unsigned boarders (0);
unsigned unboarders (0);

void run(){
    cout << "Eh zeeeee partiiiii" << endl;
}

void load(){
    cout << "Le train en direction du bonheur est entré en gare" << endl;
}

void unload(){
    cout << "Vous etes bien arrivé en gare dite du bonheur" << endl;
}

void board(){
    cout << "Un passager rentre dans le train, helloooooooo mister passager" << endl;
}

void unboard(){
    cout << "Un passager descend du train, bye bye le passager" << endl;
}

void car_function(unsigned C){
    load();
    boardQueue.release(C); //signal
    allAboard.acquire(); //wait

    run();

    unload();
    unBoardQueue.release(C);
    allAshore.acquire();

}

void passenger_function(unsigned C){
    cout << "test";
    boardQueue.acquire();
    board();

    mutexBoard.lock();
    boarders +=1;
    if (boarders==C){
        allAboard.release();
        boarders=0;
    }
    mutexBoard.unlock();

    unBoardQueue.acquire();
    unboard();

    mutexUnboard.lock();
    unboarders +=1;
    if (unboarders==C){
        allAshore.release();
        unboarders=0;
    }
    mutexUnboard.unlock();
}


int main()
{
    unsigned C (2);
    unsigned nbPassengers(10);
    if (nbPassengers <  C )
    {
        cerr << "Le nombre de passagers est pas bon." << endl;
        return 1;
    }
    thread wagon(car_function,C);


    vector<thread> passengers;
    cout << "test2";
    for (unsigned n=0; nbPassengers>n; n++){
        passengers.emplace_back(passenger_function, C);
    }

    for (thread & passenger : passengers){
        passenger.join();
    }

    wagon.join();

    return 0;

}
