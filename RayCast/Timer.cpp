#include "pch.h"
#include "Timer.h"

#include <iostream>
#include <numeric>


namespace rc {
	Timer::Timer() : running_sum(0), sample_count(0)
	{}

	void Timer::start() noexcept  {
		start_time = now();
	}

	void Timer::end() noexcept {
		const timer_point end_time = now();
		running_sum += std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
		++sample_count;
		start_time = end_time;
	}

	void Timer::dump(const std::string& title) {
		std::cout << title
			<< "Total time " << running_sum << "\n"
			<< "Samples " << sample_count << "\n"
 			<< "Average " << running_sum / sample_count << std::endl;
	}


	timer_point Timer::now() noexcept
	{
		return  std::chrono::high_resolution_clock::now();
	}
}
