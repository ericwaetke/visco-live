#ifndef MCP_MANAGER_H
#define MCP_MANAGER_H

#include <Adafruit_MCP23X17.h>

extern Adafruit_MCP23X17 mcp0;
extern Adafruit_MCP23X17 mcp1;
extern Adafruit_MCP23X17 mcp_other;

Adafruit_MCP23X17 *getMcpForTrack(int track);

#endif // MCP_MANAGER_H
