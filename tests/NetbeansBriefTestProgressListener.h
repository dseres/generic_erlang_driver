/* 
 * File:   MyBriefTestProgressListener.h
 * Author: dseres
 *
 * Created on July 7, 2016, 12:25 PM
 */

#ifndef MYBRIEFTESTPROGRESSLISTENER_H
#define	MYBRIEFTESTPROGRESSLISTENER_H


#include <cppunit/TestListener.h>

CPPUNIT_NS_BEGIN

/*! \brief TestListener that prints the name of each test before running it.
 * \ingroup TrackingTestExecution
 */
class CPPUNIT_API NetbeansTestProgressListener : public TestListener {
public:
    /*! Constructs a BriefTestProgressListener object.
     */
    NetbeansTestProgressListener();

    /// Destructor.
    virtual ~NetbeansTestProgressListener();

    void startTest(Test *test);

    void addFailure(const TestFailure &failure);

    void endTest(Test *test);

private:
    /// Prevents the use of the copy constructor.
    NetbeansTestProgressListener(const NetbeansTestProgressListener &copy);

    /// Prevents the use of the copy operator.
    void operator=(const NetbeansTestProgressListener &copy);

private:
    bool m_lastTestFailed;
};

CPPUNIT_NS_END

#endif	/* MYBRIEFTESTPROGRESSLISTENER_H */

