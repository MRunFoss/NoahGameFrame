NoahGameFrame
=============
######Copyright © http://www.yowoyo.com

#####交流QQ群：341159815

##What is it?

NF is a lightweight, fast, scalable, distributed plugin framework.

The features:
-------------------

*  it is easy to use interface oriented design minimise the effort
*  extensible plugin framework makes getting your application running is quick and simple
*  clean, uncluttered design, stable engine used in several commercial products
*  using the actor model has very high performance(by theron);</p>
*  based on the event-driven and attribute-driver can make business more clearly and easy to maintenance
*  based on the standard c + + development, cross-platform support
*  with existing c++, c# game client for rapid development
*  Cross-platform


Manual
---------------
https://github.com/ketoo/NoahGameFrame/blob/develop/Introduce/NFrame_Introduction_CHS.pdf



Architecture:
-------------------

Architecture:

![Architecture](https://github.com/ketoo/NoahGameFrame/blob/develop/Introduce/20150511205235.jpg)
![Architecture](https://github.com/ketoo/NoahGameFrame/blob/develop/Introduce/20150511205320.jpg)

Event Driver:

![Event Driver](https://github.com/ketoo/NoahGameFrame/blob/develop/Introduce/20150511205210.jpg)

Logic Class:

![Logic Class](https://github.com/ketoo/NoahGameFrame/blob/develop/Introduce/20150511205321.png)




Get the Sources:
-------------------

git clone https://github.com/ketoo/NoahGameFrame.git

or

svn checkout https://github.com/ketoo/NoahGameFrame


Dependencies:
-------------------

Need build sub librares:

* Boost.System
* Boost.Chrono
* Boost.Thread

* libevent-2.0.21-stable
* protobuf-2.5.0
* redis-cplusplus-client
* Theron-6.00.01
* easylogging++

Supported Compilers
-------------------

* GCC >= 4.8
* VC >= 9.0 (sp1)


How to build:
-------------------

## Windows[VS2012]

* 1:download all source;

* 2:extract nessasery lib: 	Dependencies\boost_1_53_0.zip to Dependencies\

* 3:extract nessasery lib: 	Dependencies\lib.zip to Dependencies\

* 4:open the solution:		trunk\NoahFrame.sln

* 5:build the solution 

* 6: run the binary file by trunk\_Out\Server\Debug\Run.bat


## Linux[CodeBlock 13.1]

* 1:download all source;

* 2:extract nessasery lib: 	Dependencies\boost_1_53_0.zip to Dependencies\

* 3:extract nessasery lib: 	Dependencies\lib.zip to Dependencies\

* 4:open the solution by code::block:		trunk\NF.workspace

* 5:add c++11 optional for every project of this solution

* 6:build library: NFNet, NFCore, NFEventTriggerPlugin, NFKernelPlugin, Theron, libprotocolbuf[libprotocolbuf-lite], NFLogPlugin, NFPluginLoader

* 7:build all other plugin

* 8: run the binary file by trunk\_Out\Server\Debug\Run.sh



Tutorial:
-------------------
### [01-Hello world, add a module](https://github.com/ketoo/NoahGameFrame/tree/develop/Tutorial/Tutorial1)


```cpp

// -------------------------------------------------------------------------
//    @FileName      :    HelloWorld1.h
//    @Author           :    ketoo
//    @Date             :    2014-05-01 08:51
//    @Module           :   HelloWorld1
//
// -------------------------------------------------------------------------

#ifndef _NFC_HELLO_WORLD1_H_
#define _NFC_HELLO_WORLD1_H_

#include "NFComm/NFPluginModule/NFIPluginManager.h"

class HelloWorld1
    : public NFILogicModule
{
public:
    HelloWorld1(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool AfterInit();

    virtual bool Execute(const float fLasFrametime, const float fStartedTime);

    virtual bool BeforeShut();
    virtual bool Shut();

protected:

};

#endif


#include "HelloWorld1.h"

bool HelloWorld1::Init()
{
    // Use this for initialization
	
    std::cout << "Hello, world1, Init" << std::endl;

    return true;
}

bool HelloWorld1::AfterInit()
{
    // AfterInit is called after Init
	
    std::cout << "Hello, world1, AfterInit" << std::endl;

    return true;
}

bool HelloWorld1::Execute( const float fLasFrametime, const float fStartedTime )
{
    // Execute is called once per frame
	
    //std::cout << "Hello, world1, Execute" << std::endl;

    return true;
}

bool HelloWorld1::BeforeShut()
{
    //before final
	
    std::cout << "Hello, world1, BeforeShut" << std::endl;

    return true;
}

bool HelloWorld1::Shut()
{
    //final
	
    std::cout << "Hello, world1, Shut" << std::endl;

    return true;
}

```


-------------------
### [02-Hello world, test data driver](https://github.com/ketoo/NoahGameFrame/tree/develop/Tutorial/Tutorial2)

* how to use the world's most advanced data engine 

-------------------
### [03-Hello world, test heartbeat and event system](https://github.com/ketoo/NoahGameFrame/tree/develop/Tutorial/Tutorial3)

* how to use the synchronous events

-------------------
### [04-Hello actor, test actor model(async event system)](https://github.com/ketoo/NoahGameFrame/tree/develop/Tutorial/Tutorial4ActorPlugin)

* how to use the asynchronous events
* use multiple cpus to get high performance

-------------------









