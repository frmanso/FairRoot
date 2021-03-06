/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * runTestPub.cxx
 *
 * @since 2015-09-05
 * @author A. Rybalchenko
 */

#include "FairMQLogger.h"
#include "FairMQTestPub.h"

#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
    reinit_logger(false);

    FairMQTestPub testPub;
    testPub.CatchSignals();

    std::string transport;
    if ( (argc != 2) || (argv[1] == NULL) )
    {
        LOG(ERROR) << "Transport for the test not specified!";
        return 1;
    }

    if ( strncmp(argv[1],"zeromq",6) == 0 )
    {
        transport = "zeromq";
        testPub.SetTransport(transport);
    }
    else if ( strncmp(argv[1],"nanomsg",7) == 0 )
    {
        transport = "nanomsg";
        testPub.SetTransport(transport);
    }
    else
    {
        LOG(ERROR) << "Incorrect transport requested! Expected 'zeromq' or 'nanomsg', found: " << argv[1];
        return 1;
    }

    testPub.SetProperty(FairMQTestPub::Id, "testPub");

    FairMQChannel controlChannel("pull", "bind", "tcp://127.0.0.1:5555");
    if (transport == "nanomsg")
    {
        controlChannel.UpdateAddress("tcp://127.0.0.1:5755");
    }
    controlChannel.UpdateRateLogging(0);
    testPub.fChannels["control"].push_back(controlChannel);

    FairMQChannel pubChannel("pub", "bind", "tcp://127.0.0.1:5556");
    if (transport == "nanomsg")
    {
        pubChannel.UpdateAddress("tcp://127.0.0.1:5756");
    }
    pubChannel.UpdateRateLogging(0);
    testPub.fChannels["data"].push_back(pubChannel);

    testPub.ChangeState("INIT_DEVICE");
    testPub.WaitForEndOfState("INIT_DEVICE");

    testPub.ChangeState("INIT_TASK");
    testPub.WaitForEndOfState("INIT_TASK");

    testPub.ChangeState("RUN");
    testPub.WaitForEndOfState("RUN");

    // nanomsg does not implement the LINGER option. Give the sockets some time before their queues are terminated
    if (transport == "nanomsg")
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    testPub.ChangeState("RESET_TASK");
    testPub.WaitForEndOfState("RESET_TASK");

    testPub.ChangeState("RESET_DEVICE");
    testPub.WaitForEndOfState("RESET_DEVICE");

    testPub.ChangeState("END");

    return 0;
}
