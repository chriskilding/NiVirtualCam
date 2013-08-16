// NiVirtualCam.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "NiStream.h"
#include "NiSpout.h"
#include "NiteSpout.h"
#include "NiteJsonWriter.h"
#include "ZigFuJsonWriter.h"
#include "SharedDevice.h"
#include "WebSocketServer.h"
#include "WebSocketClient.h"
#include "NiDirectShowDepthStream.h"

using namespace openni;
using namespace nite;
using namespace csp;

int main(int argc, const char* argv[]) {
    std::cout << "NIVirtualCam" << std::endl;

    Start_CPPCSP();

    // Establish shared device
    SharedDevice sd;

    // OpenNI channels
    One2OneChannel<VideoFrameRef> depthSpout;
	NiStream str(sd.getDevice(), openni::SENSOR_DEPTH);
    
    // Nite channels
    One2OneChannel<nite::UserData> niteSpout;
    One2OneChannel<std::string> json;

	/*
	 * You can't have multiple Run(InParallel...) statements
	 * as each of them blocks until it finishes.
	 * Instead, you have to squeeze all of your concurrent processes
	 * (whether or not they logically belong together)
	 * into the same Run(InParallel...) block
	 */
	// Expecting 1 argument - the WS server URI
	if (argc == 2) {
		const std::string serverUri = argv[1];

		std::cout << "Running OpenNI and NiTE\n";
		std::cout << "Server URI is " << serverUri << "\n";

		Run(InParallel
			// The depth chain
			(new NiSpout(str, depthSpout.writer()))
			(new NiDirectShowDepthStream(depthSpout.reader()))
			// The skeleton tracking chain
			(new NiteSpout(niteSpout.writer(), sd.getDevice()))
			(new NiteJsonWriter(niteSpout.reader(), json.writer(), ZigFuJsonWriter()))
			(new WebSocketClient(json.reader(), serverUri))
			// (new WebSocketServer(json.reader(), 443))
		);
	} else {
		std::cout << "Only running the OpenNI chain\n";

		Run(InParallel
			// The depth chain
			(new NiSpout(str, depthSpout.writer()))
			(new NiDirectShowDepthStream(depthSpout.reader()))
		);

	}
    
    End_CPPCSP();
	
	std::cout << "Closing NiVirtualCam\n";
	return 0;
}