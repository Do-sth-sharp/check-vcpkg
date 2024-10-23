#include <atomic>

int main() {
	return static_cast<int>(std::atomic<int64_t>{});
}