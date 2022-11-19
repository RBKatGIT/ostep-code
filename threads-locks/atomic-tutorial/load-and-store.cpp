#include <atomic>
#include <cstdio>
#include <thread>

static std::atomic<int> foobar(8);
static std::atomic<bool> start(false);
// Atomicity - Is a point in time.
// Here two threads are storing and loding the sequence of print output may vary
// When we introduce a loop between store and load in each of the thraed , the thread doesn not get back
// what it had stored in the variable 
// https://blog.devgenius.io/a-simple-guide-to-atomics-in-c-670fc4842c8b

int main() {
    std::thread t1 = std::thread([]{
        int records[10];
        while (!start.load());
        for (size_t i = 0; i < 10; ++i) {
            foobar.store(3);
            for (int k = 0; k < 1000; ++k);
            records[i] = foobar.load();
        }
        for (size_t j = 0; j < 10; ++j) {
            printf("t1 %zu - %d\n", j, records[j]);
        }
    });

    std::thread t2 = std::thread([]{
        int records[10];
        while (!start.load());
        for (size_t i = 0; i < 10; ++i) {
            foobar.store(6);
            for (int k = 0; k < 1000; ++k);
            records[i] = foobar.load();
        }
        for (size_t j = 0; j < 10; ++j) {
            printf("t2 %zu - %d\n", j, records[j]);
        }
    });
    start.store(true);

    t1.join();
    t2.join();
    return 0;
}