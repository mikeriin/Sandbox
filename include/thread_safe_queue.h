#ifndef MK_THREAD_SAFE_QUEUE
#define MK_THREAD_SAFE_QUEUE


#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <thread>
#include <stop_token>


namespace mk
{


	template<typename T>
	class ThreadSafeQueue
	{
	private:
		std::queue<T> m_queue;
		std::mutex m_mutex;
		std::condition_variable_any m_conditionVarAny;


	public:
		inline void Push(T value)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(std::move(value));
			m_conditionVarAny.notify_one();
		}


		inline std::optional<T> PopWait(std::stop_token stopToken)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_conditionVarAny.wait(lock, stopToken, [this] {return !m_queue.empty(); });

			if (m_queue.empty()) return std::nullopt;

			T value = std::move(m_queue.front());
			m_queue.pop();

			return value;
		}


		inline std::optional<T> TryPop()
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_queue.empty()) return std::nullopt;

			T value = std::move(m_queue.front());
			m_queue.pop();

			return value;
		}


		inline bool Empty() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_queue.empty();
		}


	};



}





#endif // !MK_THREAD_SAFE_QUEUE
