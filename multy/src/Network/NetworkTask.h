#pragma once

#include "Config.h"
#include "../common/ITask.h"
#include "../common/Logger.h"
#include <boost/asio.hpp>
#include <memory>

// Factory for creating task objects
class NetworkTask : public ITask
{
private:
	std::vector<std::shared_ptr<ITask>> task_list_ = {};
	Logger logger_;
	boost::asio::io_context io_context_;

	NetworkTask() : logger_("NetworkTask") {}
public:
	~NetworkTask() = default;
	static std::shared_ptr<ITask> Create(const config::SCommand& command);
	void Run() override;
};
