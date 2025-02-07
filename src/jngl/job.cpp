// Copyright 2012-2023 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt

#include "job.hpp"

#include "../windowptr.hpp"

namespace jngl {

Job::~Job() = default;

void addJob(std::shared_ptr<Job> job) {
	return pWindow->addJob(std::move(job));
}

std::shared_ptr<Job> getJob(const std::function<bool(Job&)>& predicate) {
	return pWindow->getJob(predicate);
}

} // namespace jngl
