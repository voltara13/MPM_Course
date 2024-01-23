#pragma once

#include <functional>
#include <future>

class Segment;

class Revision
{
public:
	Revision();
	Revision(Segment* ipRoot, Segment* ipCurrent);

	Revision* Fork(std::function<void()> iAction);
	Segment* Root();
	Segment* Current();

	void Join(Revision* ipJoin);

	static Revision* pCurrentRevision;

private:
	Segment* _pRoot;
	Segment* _pCurrent;

	std::future<void> _task;
};

