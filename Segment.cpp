#include "Segment.h"

#include "Versioned.h"
#include "Revision.h"

int Segment::versionCount = 0;

Segment::Segment(Segment* ipParnet)
{
	_pParent = ipParnet;

	if (ipParnet != nullptr)
		ipParnet->_refcount++;

	_version = versionCount++;
	_refcount = 1;
}

void Segment::Release()
{
	if (--_refcount == 0) {
		for (const auto& versioned : _written) {
			versioned->Release(this);
		}
	}

	if (_pParent != nullptr)
		_pParent->Release();
}

void Segment::Collapse(Revision* ipMain)
{
	while (_pParent != ipMain->Root() && _pParent->_refcount == 1) {
		for (const auto& versioned : _pParent->_written) {
			versioned->Collapse(ipMain, _pParent);
		}

		_pParent = _pParent->_pParent;
	}
}