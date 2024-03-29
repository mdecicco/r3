#pragma once
#include <r3/common/Types.h>
#include <chrono>

namespace r3 {
	class Timer {
		public:
			Timer();
			~Timer();

			void start();
			void pause();
			void reset();

			bool stopped() const;
			f32 elapsed() const;
			operator f32() const;

		protected:
			std::chrono::high_resolution_clock::time_point m_startPoint;
			f32 m_pausedAt;
			bool m_stopped;
	};
};