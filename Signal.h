#pragma once

#include "Callable.h"

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <cassert>

template<class... Args>
class Signal
{
public:
	using CallableType = Callable<Args...>;

	void operator()(Args... args)
	{
		for (CallableType &c : callbacks_)
		{
			c(std::forward<Args>(args)...);
		}
	}

	void operator()(Args... args) const
	{
		for (const CallableType &c : callbacks_)
		{
			c(std::forward<Args>(args)...);
		}
	}

	int add(CallableType callback)
	{
		const int id = generate_id();
		callbacks_.emplace_back(std::move(callback));
		id_to_index_[id] = callbacks_.size() - 1;
		return id;
	}

	void remove(int deleted_id)
	{
        const auto deleted_it = id_to_index_.find(deleted_id);

		assert(deleted_it != id_to_index_.end());

		const int deleted_index = deleted_it->second;
		id_to_index_.erase(deleted_it);
		callbacks_.erase(callbacks_.begin() + deleted_index);

		std::unordered_map<int, int> id_to_new_index;
        for (const auto& it : id_to_index_)
        {
            const int id = it.first;
            int index = it.second;
            assert(index != deleted_index);
            if (index > deleted_index)
            {
                --index;
            }
            id_to_new_index[id] = index;
        }
        id_to_index_ = std::move(id_to_new_index);
	}

private:
	int generate_id() const
	{
		int id = 0;
		while (id_to_index_.find(id) != id_to_index_.end())
		{
			++id;
		}
		return id;
	}

private:
	std::vector<CallableType> callbacks_;
	std::unordered_map<int, int> id_to_index_;
};
