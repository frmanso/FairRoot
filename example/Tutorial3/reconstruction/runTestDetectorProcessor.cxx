/*
 * runTestDetectorProcessor.cxx
 *
 *  Created on: Oct 26, 2012
 *      Author: dklein
 */

#include <iostream>
#include <csignal>


#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "FairMQLogger.h"
#include "FairMQProcessor.h"

#include "FairTestDetectorMQRecoTask.h"
#include "FairTestDetectorHit.h"
#include "FairTestDetectorDigi.h"
#include "FairTestDetectorPayload.h"

#ifdef PROTOBUF
  #include "FairTestDetectorPayload.pb.h"
#endif

#ifdef NANOMSG
  #include "nanomsg/FairMQTransportFactoryNN.h"
#else
  #include "zeromq/FairMQTransportFactoryZMQ.h"
#endif

using std::cout;
using std::cin;
using std::endl;
using std::stringstream;
  // class to serialize/deserialize
  typedef FairTestDetectorDigi TDigi;
  typedef FairTestDetectorHit THit;
  // ProtoBuff payload
#ifdef PROTOBUF
  typedef TestDetectorProto::DigiPayload TProtoDigiPayload;
  typedef TestDetectorProto::HitPayload TProtoHitPayload;
#endif
  // Binary payload
  typedef TestDetectorPayload::TestDetectorDigi TBinPayloadIn;   // binary
  typedef TestDetectorPayload::TestDetectorHit TBinPayloadOut;   // binary
  // Boost payload
  typedef boost::archive::binary_iarchive TBoostBinPayloadIn;     // boost binary format
  typedef boost::archive::text_iarchive TBoostTextPayloadIn;      // boost text format
  typedef boost::archive::binary_oarchive TBoostBinPayloadOut;   // boost binary format
  typedef boost::archive::text_oarchive TBoostTextPayloadOut;     // boost text format
  
#ifdef PROTOBUF
  typedef FairTestDetectorMQRecoTask<TDigi,THit,TProtoDigiPayload, TProtoHitPayload> TProcessorTask;
#else  
  typedef FairTestDetectorMQRecoTask<TDigi,THit,TBoostBinPayloadIn,TBoostBinPayloadOut> TProcessorTask;
#endif

FairMQProcessor processor;

static void s_signal_handler (int signal)
{
  cout << endl << "Caught signal " << signal << endl;

  processor.ChangeState(FairMQProcessor::STOP);
  processor.ChangeState(FairMQProcessor::END);

  cout << "Shutdown complete. Bye!" << endl;
  exit(1);
}

static void s_catch_signals (void)
{
  struct sigaction action;
  action.sa_handler = s_signal_handler;
  action.sa_flags = 0;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);
}

int main(int argc, char** argv)
{
  if ( argc != 12 ) {
    cout << "Usage: testDetectorProcessor \tID processorTask numIoTreads\n"
              << "\t\tinputSocketType inputRcvBufSize inputMethod inputAddress\n"
              << "\t\toutputSocketType outputSndBufSize outputMethod outputAddress\n" << endl;
    return 1;
  }

  s_catch_signals();

  LOG(INFO) << "PID: " << getpid();

#ifdef NANOMSG
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryNN();
#else
  FairMQTransportFactory* transportFactory = new FairMQTransportFactoryZMQ();
#endif

  processor.SetTransport(transportFactory);

  int i = 1;

  processor.SetProperty(FairMQProcessor::Id, argv[i]);
  ++i;

  if (strcmp(argv[i], "FairTestDetectorMQRecoTask") == 0) {
    TProcessorTask* task = new TProcessorTask();
    processor.SetTask(task);
  } else {
    LOG(ERROR) << "task not supported.";
    exit(1);
  }
  ++i;

  int numIoThreads;
  stringstream(argv[i]) >> numIoThreads;
  processor.SetProperty(FairMQProcessor::NumIoThreads, numIoThreads);
  ++i;

  processor.SetProperty(FairMQProcessor::NumInputs, 1);
  processor.SetProperty(FairMQProcessor::NumOutputs, 1);


  processor.ChangeState(FairMQProcessor::INIT);


  processor.SetProperty(FairMQProcessor::InputSocketType, argv[i], 0);
  ++i;
  int inputRcvBufSize;
  stringstream(argv[i]) >> inputRcvBufSize;
  processor.SetProperty(FairMQProcessor::InputRcvBufSize, inputRcvBufSize, 0);
  ++i;
  processor.SetProperty(FairMQProcessor::InputMethod, argv[i], 0);
  ++i;
  processor.SetProperty(FairMQProcessor::InputAddress, argv[i], 0);
  ++i;

  processor.SetProperty(FairMQProcessor::OutputSocketType, argv[i], 0);
  ++i;
  int outputSndBufSize;
  stringstream(argv[i]) >> outputSndBufSize;
  processor.SetProperty(FairMQProcessor::OutputSndBufSize, outputSndBufSize, 0);
  ++i;
  processor.SetProperty(FairMQProcessor::OutputMethod, argv[i], 0);
  ++i;
  processor.SetProperty(FairMQProcessor::OutputAddress, argv[i], 0);
  ++i;


  processor.ChangeState(FairMQProcessor::SETOUTPUT);
  processor.ChangeState(FairMQProcessor::SETINPUT);
  processor.ChangeState(FairMQProcessor::RUN);


  char ch;
  cin.get(ch);

  processor.ChangeState(FairMQProcessor::STOP);
  processor.ChangeState(FairMQProcessor::END);

  return 0;
}
