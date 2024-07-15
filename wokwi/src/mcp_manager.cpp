#include "mcp_manager.h"

Adafruit_MCP23X17 mcp0;
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp_other;

int trackMcp[8] = {0, 0, 0, 0, 1, 1, 1, 1};

Adafruit_MCP23X17 *getMcpForTrack(int track)
{
	return (trackMcp[track] == 0) ? &mcp0 : &mcp1;
}
