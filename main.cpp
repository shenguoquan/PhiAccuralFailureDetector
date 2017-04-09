#include <iostream>
#include <chrono>
#include <thread>
#include "headers/phiAccuralFailureDetector.h"

using namespace std;

using namespace std::chrono;

static long currentTime() {
  milliseconds ms = duration_cast< milliseconds >(
          system_clock::now().time_since_epoch()
  );
  return ms.count();
}

int main() {

  PhiAccuralFailureDetector phiAccuralFailureDetector = PhiAccuralFailureDetector();

  for (int i = 0; i < 5; i++) {
    phiAccuralFailureDetector.heartbeat(currentTime());
    if (phiAccuralFailureDetector.isAvailable(currentTime())) {
      cout << "The node is alive." << endl;
    } else {
      cout << "The node got crashed." << endl;
    }
    //cout << phiAccuralFailureDetector.phi(currentTime()) << endl;
    //if (phiAccuralFailureDetector.isAvailable())
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  for (int i = 0; i < 5; i++) {
    //phiAccuralFailureDetector.heartbeat(currentTime());
    if (phiAccuralFailureDetector.isAvailable(currentTime())) {
      cout << "The node is alive." << endl;
    } else {
      cout << "The node got crashed." << endl;
    }
    // cout << phiAccuralFailureDetector.phi(currentTime()) << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  for (int i = 0; i < 5; i++) {
    phiAccuralFailureDetector.heartbeat(currentTime());
    //cout << phiAccuralFailureDetector.phi(currentTime()) << endl;
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (phiAccuralFailureDetector.isAvailable(currentTime())) {
      cout << "The node is alive." << endl;
    } else {
      cout << "The node got crashed." << endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}