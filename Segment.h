#pragma once

#include <list>

class Revision;
class IVersioned;

class Segment
{
public:
	Segment(Segment* ipParnet = nullptr);

	Segment* Parent();
	int Version();

	std::list<IVersioned*> _written;

private:
	friend Revision;
	friend IVersioned;

	void Release();
	void Collapse(Revision* ipMain);

	Segment* _pParent;

	int _version;
	int _refcount;

	static int versionCount;
};
