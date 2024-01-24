#pragma once

#include <list>

class Revision;
class IVersioned;

class Segment
{
public:
	Segment(Segment* ipParnet = nullptr);

	std::list<IVersioned*> _written;
	Segment* _pParent;

	int _version;

private:
	friend Revision;
	friend IVersioned;

	void Release();
	void Collapse(Revision* ipMain);

	int _refcount;

	static int versionCount;
};
