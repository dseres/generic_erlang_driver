/* 
 * File:   MyMyBriefTestProgressListener.cpp
 * Author: dseres
 * 
 * Created on July 7, 2016, 12:25 PM
 */

#include "NetbeansBriefTestProgressListener.h"

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>

CPPUNIT_NS_BEGIN

NetbeansTestProgressListener::NetbeansTestProgressListener()
: m_lastTestFailed(false) {
}

NetbeansTestProgressListener::~NetbeansTestProgressListener() {
}

void
NetbeansTestProgressListener::startTest(Test *test) {
    stdCOut() << "Starting " << test->getName() << " test.\n";
    m_lastTestFailed = false;
}

void
NetbeansTestProgressListener::addFailure(const TestFailure &failure) {
    stdCOut() << failure.failedTestName() << " : " << (failure.isError() ? "error" : "assertion") << "\n";
    stdCOut() << (failure.isError() ? "ERROR" : "ASSERTION") << "\n";
    m_lastTestFailed = true;
}

void
NetbeansTestProgressListener::endTest(Test *test) {
    if (!m_lastTestFailed) {
        stdCOut() << test->getName() << " : OK" << "\n";
        stdCOut() << "OK\n";
    }
    stdCOut() << "\n";
}

CPPUNIT_NS_END