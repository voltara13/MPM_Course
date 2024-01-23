#include "Revision.h"

#include "Segment.h"
#include "Versioned.h"

Revision* Revision::pCurrentRevision = new Revision();

Revision::Revision()
{
	_pRoot = new Segment();
	_pCurrent = new Segment(_pRoot);
}

Revision::Revision(Segment* ipRoot, Segment* ipCurrent)
{
	_pRoot = ipRoot;
	_pCurrent = ipCurrent;
}

Revision* Revision::Fork(std::function<void()> iAction)
{
	auto revision = new Revision(_pCurrent, new Segment(_pCurrent));
	_pCurrent->Release();
	_pCurrent = new Segment(_pCurrent);

	revision->_task = std::async(std::launch::async, [&]() {
		const auto previous = pCurrentRevision;

		pCurrentRevision = revision;

		__try {
			iAction();
		}
		__finally {
			pCurrentRevision = previous;
		}
	});

	return revision;
}

void Revision::Join(Revision* ipJoin)
{
	__try {
		ipJoin->_task.wait();
		auto segment = ipJoin->_pCurrent;

		while (segment != ipJoin->_pRoot) {
			for (const auto& versioned : segment->_written) {
				versioned->Merge(this, ipJoin, segment);
			}

			segment = segment->_pParent;
		}
	}
	__finally {
		ipJoin->_pCurrent->Release();
		_pCurrent->Collapse(this);
	}
}

Segment* Revision::Root()
{
	return _pRoot;
}

Segment* Revision::Current()
{
	return _pCurrent;
}
