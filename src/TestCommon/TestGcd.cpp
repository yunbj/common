#include "stdafx.h"
#include "TestGcd.h"

#include <memory>
#include <atomic>
#include <assert.h>

#include <thread>

#include <base/Gcd.h>

using namespace std;
using namespace grid;

class TestClass {
public:
	TestClass(bool allowCopy)
		: _allowCopy(allowCopy) {
	}

    TestClass(const TestClass& rhs) {
		value = rhs.value;
		printf(">>>> copy constructor\n");

		if (!_allowCopy) {
			assert(!"called copy constructor");
		}
	}

	TestClass(TestClass&& rhs) {
		value = rhs.value;
		rhs.value = 0;
		printf(">>>> move constructor\n");
	}

	TestClass& operator= (const TestClass& rhs) {
		value = rhs.value;
		printf(">>>> copy assign\n");

		if (!_allowCopy) {
			assert(!"called copy assign");
		}
        
        return *this;
	}
    
	TestClass& operator= (TestClass&& rhs) {
		value = rhs.value;
		rhs.value = 0;
		printf(">>>> move assign\n");
        
        return *this;
	}

	int value = 0;
	bool _allowCopy = true;
};

class CallbackClass : public enable_shared_from_this<CallbackClass> {
	//
public:

	void call(int value) {
		printf(">>>> called  method function callback.(value : %d)\n", value);
	}

	void TestCallback(std::shared_ptr<Gcd> gcd) {
		gcd->DispatchAsync(&CallbackClass::call, shared_from_this(), 10);
	}

	int value = 0;
};

void TestGcd::_TestMoveValue(std::shared_ptr<Gcd> gcd) {
	TestClass c(false);
	auto ret = 0;

	printf("=================  move test =====================\n");
	c.value = 10;
	printf("move dispatch async.\n");
	gcd->DispatchAsync([c = std::move(c)]{

		printf("******* moved dispatch async value : %d\n", c.value);

	});

	c.value = 15;
	printf("\nmove dispatch sync.\n");
	ret = gcd->DispatchSync([c = std::move(c)]{

		printf("******* moved dispatch sync value : %d\n", c.value);
	return 0;

	});


	c.value = 20;
	printf("\nmove create once timer.\n");
	gcd->CreateTimer(1s, false, [c = std::move(c)]{
		printf("******* moved once timer value : %d\n", c.value);
	});

	c.value = 30;
	printf("\nmove create repeat timer.\n");
	auto tId = gcd->CreateTimer(1s, true, [c = std::move(c)]{
		printf("******* moved repeat timer value : %d\n", c.value);
	});
	this_thread::sleep_for(3s);

	gcd->DestroyTimer(tId);
}

void TestGcd::_TestCopyValue(std::shared_ptr<Gcd> gcd) {
	TestClass c(true);
	auto ret = 0;

	printf("=================  copy test =====================\n");
	c.value = 10;

	printf("copy dispatch async.\n");
	gcd->DispatchAsync([c = c] {

		printf("******* copy dispatch async value : %d\n", c.value);

	});

	assert(c.value == 10);

	c.value = 15;
	printf("\ncopy dispatch sync.\n");
	ret = gcd->DispatchSync([c = c] {

		printf("******* copy dispatch sync value : %d\n", c.value);
		return 0;

	});
	assert(c.value == 15);

	c.value = 20;
	printf("\ncopy create once timer.\n");
	gcd->CreateTimer(1s, false, [c = c] {
		printf("******* copy once timer value : %d\n", c.value);
	});

	assert(c.value == 20);

	c.value = 30;
	printf("\ncopy create repeat timer.\n");
	auto tId = gcd->CreateTimer(1s, true, [c = c] {
		printf("*******  copy repeat timer value : %d\n", c.value);
	});

	assert(c.value == 30);

	this_thread::sleep_for(3s);

	gcd->DestroyTimer(tId);
}

void TestGcd::_TestFuncMoveValue(std::shared_ptr<Gcd> gcd)
{
	std::shared_ptr<CallbackClass> cClass = make_shared<CallbackClass>();
	cClass->TestCallback(gcd);
}
/////////////////////////////////////////////////////////////////////////////////////////////
TestGcd::TestGcd()
	: _gcd(make_shared<Gcd>()) {

}

TestGcd::~TestGcd() {
	if (_gcd) {
		_gcd->Fini();
	}
}

void TestGcd::DoTest() {
	printf("============= start Gcd test ===============\n");

	_gcd->Init();

	_TestMoveValue(_gcd);
	_TestCopyValue(_gcd);

	_TestFuncMoveValue(_gcd);

	printf("============= complete Gcd test ===============\n");
}
