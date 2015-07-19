#pragma once

//
// WRL is blocked in C++/CLI projects, so split media sinks out into a static lib
//

void CreateVideoSink(unsigned int width, unsigned int height, const std::function<void(IMFSample*)>& sampleHandler, _COM_Outptr_ IMFMediaSink** sink);