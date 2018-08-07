#pragma once


#include <base/Base.h>
#include <map>

#if defined(WIN32) || defined(_WIN64)
#include <tchar.h>
#endif // defined


namespace grid {

	class Config {

	public:
		tstring Get(const tstring& strKey, const tstring& val = _T("")) const;

		template<typename Ret>
		Ret GetT(const tstring& strKey, Ret val = Ret()) const {
			tstring strValue = this->Get(strKey);
			if (strValue == _T("")) {

				return val;
			}
			else {

				tistringstream iss(strValue);

				Ret ret;
				if (iss >> ret) {

					return ret;
				}
				else {

					return val;
				}
			}
		}

		void Set(const tstring& strKey, const tstring& strValue);

		template<typename Ret>
		void SetT(const tstring& strKey, Ret val) {

			this->Set(strKey, to_tstring(val));
		}

	private:
		std::map<tstring, tstring> _pairs;
	};
}
