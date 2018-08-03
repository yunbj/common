#include <base/RunLoopForSignal.h>
#include <base/Base.h>
#include <thread>
#include <csignal>
#include <setjmp.h>

namespace grid
{
	static volatile std::sig_atomic_t s_nQuit = 0;
    jmp_buf sigJumpBuf;

	static void SignalHandler(int signum) {
#ifdef SIGBREAK
        if (signum == SIGINT || signum == SIGBREAK || signum == SIGUSR2) {
#else
        if (signum == SIGINT || signum == SIGUSR2) {
#endif // SIGBREAK
			s_nQuit = 1;

            longjmp(grid::sigJumpBuf, 1);
		}
		else {
			tcerr << _T("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ Unexpected signal(") << signum << _T(") @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@") << std::endl;

			std::_Exit(EXIT_FAILURE);
		}
	}

	void RunLoopForSignal::Run() {
		std::signal(SIGINT, SignalHandler);
#ifdef SIGBREAK
		std::signal(SIGBREAK, SignalHandler);
#endif
		std::signal(SIGABRT, SignalHandler);

        setjmp(grid::sigJumpBuf);

		while (!s_nQuit) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}
