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

	void Collapse(Revision* ipMain, Segment* ipParent) override
	{
		if (!_versions.contains(ipMain->Current()->_version)) {
			Set(ipMain, _versions[ipParent->_version]);
		}

		Release(ipParent);
	}

	T operator * () { return Get(); }

	void operator = (const T& iValue) { Set(iValue); }

private:
	T Get()
	{
		return Get(Revision::pCurrentRevision);
	}

	void Set(T iValue)
	{
		Set(Revision::pCurrentRevision, iValue);
	}

	T Get(Revision* ipRevision)
	{
		auto segment = ipRevision->Current();
		while (!_versions.contains(segment->_version)) {
			segment = segment->_pParent;
		}

		return _versions[segment->_version];
	}

	void Set(Revision* ipRevision, T iValue)
	{
		if (!_versions.contains(ipRevision->Current()->_version)) {
			ipRevision->Current()->_written.push_back(this);
		}

		_versions[ipRevision->Current()->_version] = iValue;
	}

	void Release(Segment* ipRelease) override
	{
		_versions.erase(ipRelease->_version);
	}

	void Merge(Revision* ipMain, Revision* ipJoinRev, Segment* ipJoin) override
	{
		auto segment = ipJoinRev->Current();
		while (!_versions.contains(segment->_version)) {
			segment = segment->_pParent;
		}

		if (segment == ipJoin) {
			Set(ipMain, _versions[ipJoin->_version]);
		}
	}

	std::map<int, T> _versions;
};