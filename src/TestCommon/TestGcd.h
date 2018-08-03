#pragma once

#include <memory>

namespace grid {
	class Gcd;
}

class TestGcd {
	using Gcd = grid::Gcd;
	using GcdPtr = std::shared_ptr<Gcd>;
public:
	TestGcd();
	virtual ~TestGcd();

	void DoTest();

private:
	void _TestMoveValue(GcdPtr gcd);
	void _TestCopyValue(GcdPtr gcd);
	void _TestFuncMoveValue(GcdPtr gcd);


	GcdPtr _gcd;
};

