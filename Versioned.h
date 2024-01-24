#pragma once

#include "Revision.h"
#include "Segment.h"

#include <iostream>
#include <map>

class default_t
{
public:
	template<typename T>
	operator T() const { return T(); }
};

default_t const default_v = default_t();

class IVersioned
{
private:
	friend class Revision;
	friend class Segment;

	virtual void Release(Segment* ipRelease) = 0;
	virtual void Collapse(Revision* ipMain, Segment* ipParent) = 0;
	virtual void Merge(Revision* ipMain, Revision* ipJoinRev, Segment* ipJoin) = 0;
};

template<typename T>
class Versioned : public IVersioned
{
public:
	Versioned() 
	{
		Set(default_v);
	}

	T Get() const
	{
		return Get(Revision::pCurrentRevision);
	}

	void Set(T iValue)
	{
		Set(Revision::pCurrentRevision, iValue);
	}

	void Collapse(Revision* ipMain, Segment* ipParent) override
	{
		if (!_versions.contains(ipMain->Current()->Version())) {
			Set(ipMain, _versions[ipParent->Version()]);
		}

		Release(ipParent);
	}

	T operator * () const { return Get(); }

	void operator = (const T& iValue) { Set(iValue); }

private:
	T Get(Revision* ipRevision) const
	{
		auto segment = ipRevision->Current();
		while (!_versions.contains(segment->Version())) {
			segment = segment->Parent();
		}

		return (*_versions.find(segment->Version())).second;
	}

	void Set(Revision* ipRevision, T iValue)
	{
		if (!_versions.contains(ipRevision->Current()->Version())) {
			ipRevision->Current()->_written.push_back(this);
		}

		_versions[ipRevision->Current()->Version()] = iValue;
	}

	void Release(Segment* ipRelease) override
	{
		_versions.erase(ipRelease->Version());
	}

	void Merge(Revision* ipMain, Revision* ipJoinRev, Segment* ipJoin) override
	{
		auto segment = ipJoinRev->Current();
		while (!_versions.contains(segment->Version())) {
			segment = segment->Parent();
		}

		if (segment = ipJoin) {
			Set(ipMain, _versions[ipJoin->Version()]);
		}
	}

	std::map<int, T> _versions;
};