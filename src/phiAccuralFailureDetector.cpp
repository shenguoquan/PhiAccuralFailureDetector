#include "../headers/phiAccuralFailureDetector.h"

//---------------PhiAccuralFailureDetector definition----------------------
PhiAccuralFailureDetector::PhiAccuralFailureDetector() : m_heartbeatHistory(200){
  m_threshold = 8.0;
  m_acceptableHeartbeatPauseMillis = 0;
  m_minStdDeviationMillis = 500;
  long firstHeartbeatEstimateMillis = 500;
  long stdDeviationMillis = firstHeartbeatEstimateMillis / 4;
  m_heartbeatHistory.add(firstHeartbeatEstimateMillis - stdDeviationMillis);
  m_heartbeatHistory.add(firstHeartbeatEstimateMillis + stdDeviationMillis);
}

PhiAccuralFailureDetector::PhiAccuralFailureDetector(double threshold, int maxSampleSize, double minStdDeviationMillis,
                                                     long acceptableHeartbeatPauseMillis,
                                                     long firstHeartbeatEstimateMillis) : m_heartbeatHistory(maxSampleSize) {
  assert(threshold > 0);
  assert(minStdDeviationMillis > 0);
  assert(acceptableHeartbeatPauseMillis > 0);
  assert(firstHeartbeatEstimateMillis > 0);
  m_threshold = threshold;
  m_acceptableHeartbeatPauseMillis = acceptableHeartbeatPauseMillis;
  m_minStdDeviationMillis = minStdDeviationMillis;

  long stdDeviationMillis = firstHeartbeatEstimateMillis / 4;
  m_heartbeatHistory.add(firstHeartbeatEstimateMillis - stdDeviationMillis);
  m_heartbeatHistory.add(firstHeartbeatEstimateMillis + stdDeviationMillis);

}

double PhiAccuralFailureDetector::ensureValidStdDeviation(double stdDeviationMillis) {
  return max(stdDeviationMillis, m_minStdDeviationMillis);
}

double PhiAccuralFailureDetector::phi(long timestampMillis) {
  double result = 0.0;
  try {
    //m_mutex.lock();
    if (m_lastTimestampMillis == 0) {
      return 0.0;
    }

    long timeDiffMillis = timestampMillis - m_lastTimestampMillis;
    double meanMillis = m_heartbeatHistory.mean() + m_acceptableHeartbeatPauseMillis;
    double stdDeviationMillis = ensureValidStdDeviation(m_heartbeatHistory.stdDeviation());
    double y = (timeDiffMillis - meanMillis) / stdDeviationMillis;
    double e = exp(-y * (1.5976 + 0.070566 * y * y));
    if (timeDiffMillis > meanMillis) {
      result = -log10(e / (1.0 + e));
    } else {
      result = -log10(1.0 - 1.0 / (1.0 + e));
    }
    //m_mutex.unlock();
  } catch (...) {
    //m_mutex.unlock();
  }
  return result;
}

bool PhiAccuralFailureDetector::isAvailable(long timestampMillis) {
  return phi(timestampMillis) < m_threshold;
}

void PhiAccuralFailureDetector::heartbeat(long timestampMillis) {
  try {
    //m_mutex.lock();
    long lastTimestampMillis = m_lastTimestampMillis;
    m_lastTimestampMillis = timestampMillis;
    long interval = timestampMillis - lastTimestampMillis;
    if (lastTimestampMillis == 0) {
      return;
    }
    if (isAvailable(timestampMillis)) {
      m_heartbeatHistory.add(interval);
    }
    //m_mutex.unlock();
  } catch (...) {
    //m_mutex.unlock();
  }
}

//---------------HeartbeatHistory definition----------------------

HeartbeatHistory::HeartbeatHistory(int maxSampleSize) {
  assert(maxSampleSize > 1);
  m_maxSampleSize = maxSampleSize;
}

double HeartbeatHistory::mean() {
  return (double) m_intervalSum / m_intervals.size();
}

double HeartbeatHistory::variance() {
  double  mean = this->mean();
  return ((double) m_squaredIntervalSum) / m_intervals.size() - mean * mean;
}

double HeartbeatHistory::stdDeviation() {
  return sqrt(variance());
}

double HeartbeatHistory::pow2(long x) {
  return x * x;
}

void HeartbeatHistory::add(long interval) {
  if (m_intervals.size() >= m_maxSampleSize) {
    long first_interval = m_intervals.front();
    m_intervals.pop_front();
    m_intervalSum -= first_interval;
    m_squaredIntervalSum -= pow2(first_interval);
  }

  m_intervals.push_back(interval);
  m_intervalSum += interval;
  m_squaredIntervalSum += pow2(interval);
}


