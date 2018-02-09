#ifndef __ETERNAL_TCP_BACKED_READER__
#define __ETERNAL_TCP_BACKED_READER__

#include "Headers.hpp"

#include "CryptoHandler.hpp"
#include "SocketHandler.hpp"

namespace et {
class BackedReader {
 public:
  BackedReader(shared_ptr<SocketHandler> socketHandler,
               shared_ptr<CryptoHandler> cryptoHandler, int socketFd);

  bool hasData();
  int read(string* buf);

  void revive(int newSocketFd, vector<string> localBuffer_);

  inline void invalidateSocket() {
    lock_guard<std::mutex> guard(recoverMutex);
    socketFd = -1;
  }

  inline int64_t getSequenceNumber() { return sequenceNumber; }

 protected:
  std::mutex recoverMutex;
  shared_ptr<SocketHandler> socketHandler;
  shared_ptr<CryptoHandler> cryptoHandler;
  volatile int socketFd;
  int64_t sequenceNumber;
  deque<string> localBuffer;

  string partialMessage;

  void init(int64_t firstSequenceNumber);
  int getPartialMessageLength();
  void constructPartialMessage(string* buf);
};
}  // namespace et

#endif  // __ETERNAL_TCP_BACKED_READER__
