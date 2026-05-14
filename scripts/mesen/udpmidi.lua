midibuf = 0x3004000
midibufsize = 0xF00
midiptr = 0

socket = require("socket.core")
udp = socket.udp()
udp:settimeout(0)
udp:setsockname("127.0.0.1", 12345)
emu.log("UDP server is listening on 127.0.0.1:12345")

function beginmidiframe()
  midiptr = midibuf
end

function endmidiframe()
  emu.write32(midiptr, 0x2FFF, emu.memType.gbaDebug)
  midiptr = midiptr + 4
end

function writemidi(msg)
  local str = ""
  for i = 1, #msg do
    str = str .. string.format("%02X ", string.byte(msg,i))
	emu.write(midiptr, string.byte(msg, i), emu.memType.gbaDebug)
    midiptr = midiptr + 1
  end
  emu.log(str)
end

function update()
    beginmidiframe()
    local data = udp:receive()
    while data do
	    if midiptr > (midibuf + midibufsize) then
	    	break
	    end
	    writemidi(data)
	    data = udp:receive()
	end
	endmidiframe()
end

function close()
  udp:close()
end

emu.addEventCallback(update, emu.eventType.endFrame)
emu.addEventCallback(close, emu.eventType.scriptEnded)
