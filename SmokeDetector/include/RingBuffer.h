#pragma once

template <typename T, int size> class RingBuffer {
private:
  T data_[size];
  int head;
  int tail;

public:
  RingBuffer() : head(0), tail(0) {}

  RingBuffer(RingBuffer &&move) = delete;
  RingBuffer(RingBuffer const &copy) = delete;

  bool push(T value) {
    if ((tail + 1) % size == head) {
      // We're full
      return false;
    }

    data_[tail] = value;
    tail = (tail + 1) % size;
    return true;
  }

  bool pop(T &output) {
    if (head == tail) {
      // We're empty
      return false;
    }

    output = data_[head];
    head = (head + 1) % size;
    return true;
  }
};
