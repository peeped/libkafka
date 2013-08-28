//////////////////////////////////////////////////////////////////
//                                                              //
// libkafka - C/C++ client for Apache Kafka v0.8+               //
//                                                              //
// David Tompkins -- 8/8/2013                                   //
// http://dt.org/                                               //
//                                                              //
// Copyright (c) 2013 by David Tompkins.                        //
//                                                              //
//////////////////////////////////////////////////////////////////
//                                                              //
// This program is free software; you can redistribute it       //
// and/or modify it under the terms of the GNU General Public   //
// License as published by the Free Software Foundation.        //
//                                                              //
// This program is distributed in the hope that it will be      //
// useful, but WITHOUT ANY WARRANTY; without even the implied   //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      //
// PURPOSE. See the GNU General Public License for more details //
//                                                              //
// You should have received a copy of the GNU General Public    //
// License along with this program; if not, write to the Free   //
// Software Foundation, Inc., 59 Temple Place, Suite 330,       //
// Boston, MA 02111-1307 USA                                    //
//                                                              //
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "MetadataRequest.h"
#include "../ApiConstants.h"

using namespace std;

namespace LibKafka {

MetadataRequest::MetadataRequest(unsigned char *buffer, bool releaseBuffer) : Request(buffer, releaseBuffer)
{
  D(cout.flush() << "--------------MetadataRequest(buffer)\n";)

  // Kafka Protocol: string[] topic_name
  this->topicNameArraySize = this->packet->readInt32();
  this->topicNameArray = new string[this->topicNameArraySize];
  for (int i=0 ; i<this->topicNameArraySize; i++) {
    this->topicNameArray[i] = this->packet->readString();
  }
  this->releaseArrays = true;
}

MetadataRequest::MetadataRequest(int correlationId, string clientId, int topicNameArraySize, string topicNameArray[], bool releaseArrays) : Request(ApiConstants::METADATA_REQUEST_KEY, ApiConstants::API_VERSION, correlationId, clientId)
{
  D(cout.flush() << "--------------MetadataRequest(params)\n";)

  // Kafka Protocol: string[] topicName
  this->topicNameArraySize = topicNameArraySize;
  this->topicNameArray = topicNameArray;
  this->releaseArrays = releaseArrays;
}

MetadataRequest::~MetadataRequest()
{
  if (this->releaseArrays)
  {
    delete[] this->topicNameArray;
  }
}

unsigned char* MetadataRequest::toWireFormat(bool updatePacketSize)
{
  unsigned char* buffer = this->Request::toWireFormat(false);

  D(cout.flush() << "--------------MetadataRequest::toWireFormat()\n";)

  // Kafka Protocol: string[] topicName
  this->packet->writeInt32(this->topicNameArraySize);
  for (int i=0; i<this->topicNameArraySize; i++) {
    this->packet->writeString(this->topicNameArray[i]);
  }

  if (updatePacketSize) this->packet->updatePacketSize();
  return buffer;
}

int MetadataRequest::getWireFormatSize(bool includePacketSize)
{
  D(cout.flush() << "--------------MetadataRequest::getWireFormatSize()\n";)

  // Request.getWireFormatSize
  // string[] topicName

  int size = Request::getWireFormatSize(includePacketSize);
  size += sizeof(int);
  for (int i=0; i<topicNameArraySize; i++) {
    size += sizeof(short int) + topicNameArray[i].length();
  }
  return size;
}

ostream& operator<< (ostream& os, const MetadataRequest& mr)
{
  os << (const Request&)mr;
  os << "MetadataRequest.topicNameArraySize:" << mr.topicNameArraySize << "\n";
  for (int i=0; i<mr.topicNameArraySize; i++) {
    os << "MetadataRequest.topicNameArray[" << i << "]:" << mr.topicNameArray[i] << "\n";
  }
  return os;
}

}; // namespace LibKafka