#include <base/Config.h>
#include <cassert>


using namespace grid;

tstring Config::Get(const tstring& strKey, const tstring& strDefaultValue) const {

	auto it = _pairs.find(strKey);
	if (it != _pairs.end()) {

		return it->second;
	}
	else {

		return strDefaultValue;
	}
}

void Config::Set(const tstring& strKey, const tstring& strValue) {

	assert(strKey != _T(""));

	_pairs[strKey] = strValue;
}

