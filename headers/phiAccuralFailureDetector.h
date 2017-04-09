#ifndef PHIACCURALFAILUREDETECTOR_PHIACCURALFAILUREDETECTOR_H
#define PHIACCURALFAILUREDETECTOR_PHIACCURALFAILUREDETECTOR_H

#include <list>
#include <cassert>
#include <cmath>
#include <mutex>
#include <time.h>

using namespace std;

class HeartbeatHistory {

private:
  int m_maxSampleSize;
  list<long> m_intervals = list<long>();
  long m_intervalSum = 0;
  long m_squaredIntervalSum = 0;

  double pow2(long x);

public:
  HeartbeatHistory(int maxSampleSize);

  double mean();

  double variance();

  double stdDeviation();

  void add(long interval);
};

/**
 * This is a port of https://github.com/akka/akka/blob/master/akka-remote/src/main/scala/akka/remote/PhiAccrualFailureDetector.scala
 *
 *
 * The suspicion level of failure is given by a value called φ (phi).
 * The basic idea of the φ failure detector is to express the value of φ on a scale that
 * is dynamically adjusted to reflect current network conditions. A configurable
 * threshold is used to decide if φ is considered to be a failure.
 *
 * The value of φ is calculated as:
 *
 * {{{
 * φ = -log10(1 - F(timeSinceLastHeartbeat)
 * }}}
 * where F is the cumulative distribution function of a normal distribution with mean
 * and standard deviation estimated from historical heartbeat inter-arrival times.
 */
class PhiAccuralFailureDetector {

private:
  double m_threshold;
  double m_minStdDeviationMillis;
  long m_acceptableHeartbeatPauseMillis;
  long m_lastTimestampMillis = 0;
  HeartbeatHistory m_heartbeatHistory;
  //mutex m_mutex;

  double ensureValidStdDeviation(double stdDeviationMillis);

public:
  PhiAccuralFailureDetector();

  PhiAccuralFailureDetector(double threshold, int maxSampleSize,
                            double minStdDeviationMillis,
                            long acceptableHeartbeatPauseMillis,
                            long firstHeartbeatEstimateMillis);

  double phi(long timestampMillis);

  bool isAvailable(long timestampMillis);

  void heartbeat(long timestampMillis);
};

#endif //PHIACCURALFAILUREDETECTOR_PHIACCURALFAILUREDETECTOR_H
