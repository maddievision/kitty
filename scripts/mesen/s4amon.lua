
beatnum = 1
tock = 0
isen = false
leading = false

freq_modes = {
	5734,
	7884,
	10512,
	13379,
	15768,
	18157,
	21024,
	26758,
	31536,
	36314,
	40137,
	42048
}

memaccc = 0x30075F0

rom_song_table = 0x807667C
sndarea_loc = 0x30062F0
cgbchans_loc = 0x3007330
rom_mplay_table = 0

enmelo = false
melomem = 0x0

override_song = nil
active_player = 0

w = 68
m = 4

sfx_overlay = true
music_name_overlay = true
track_overlay = false
song_overlay = true
full_overlay = false


keystate = {F = false}

notes = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }
player_loc = 0
tracks_loc = { 0, 0, 0, 0, 0, 0, 0, 0 }
updatemax = 16
s4a_update = { updatemax, updatemax, updatemax, updatemax, updatemax, updatemax, updatemax, updatemax }
last_s4a = { 0, 0, 0, 0, 0, 0, 0, 0 }

ply_trackcount = 2
ply_t = 4
ply_tracks = 0x34
ply_ppqn = 0x834
ply_miditracks = 0x840

trk_size = 0x40
trk_status = 0
trk_note = 1
trk_wait = 8
trk_inst_ptr = 4
trk_data_ptr = 0xC
trk_pbr = 0x1B
trk_pbsemi = 0x26
trk_pbfp = 0x27
trk_voll = 0x22
trk_volr = 0x23
trk_bankmsb = 0x36
trk_program = 0x38

snd_maxvoice = 6
snd_freq_mode = 8
snd_voices = 0x50
snd_buffer = 0xFD0
snd_buffer_size = 0xC60

vce_max = 0

vce_size = 0x40
vce_status = 0
vce_rv = 0x2
vce_lv = 0x3
vce_at = 0x4
vce_de = 0x5
vce_su = 0x6
vce_re = 0x7
vce_note = 0x8
vce_env = 0x9

vce_wave = 0x24
vce_wave_pos = 0x28
vce_track = 0x2C

s4a_voices = {}
s4a_cgb_voices = {}

next_op = nil

function resolve()
	sndarea_loc = 0x3000144
	cgbchans_loc = sndarea_loc + 0x1C30
	player_loc = 0x3001E74
	tracks_loc = player_loc + 0x74
	mctracks_loc =player_loc + 0x840
end

resolve()

-- https://github.com/Wavalab/rgb-hsl-rgb/blob/master/rgbhsl.lua
local function hslToRgb(h, s, l)
	if s == 0 then return l, l, l end
	local function to(p, q, t)
			if t < 0 then t = t + 1 end
			if t > 1 then t = t - 1 end
			if t < .16667 then return p + (q - p) * 6 * t end
			if t < .5 then return q end
			if t < .66667 then return p + (q - p) * (.66667 - t) * 6 end
			return p
	end
	local q = l < .5 and l * (1 + s) or l + s - l * s
	local p = 2 * l - q
	return to(p, q, h + .33334), to(p, q, h), to(p, q, h - .33334)
end

function hsl(h, s, l)
	r, g, b = hslToRgb(h / 360, s, l)
--	emu.log(string.format("%f %f %f",r,g,b ))
	return (math.floor(r * 255) << 16) | (math.floor(g * 255) << 8) | math.floor(b * 255)
end

function dos4aupdate()
	vce_max =	emu.read(sndarea_loc + snd_maxvoice, emu.memType.gbaDebug)

	for i = 0, vce_max - 1 do
		vce_ptr = sndarea_loc + snd_voices + vce_size * i
		status = emu.read(vce_ptr + vce_status, emu.memType.gbaDebug)
		--emu.log(string.format("%d: %08X", i, vce_ptr))
		rv = emu.read(vce_ptr + vce_rv, emu.memType.gbaDebug)
		lv = emu.read(vce_ptr + vce_lv, emu.memType.gbaDebug)
		at = emu.read(vce_ptr + vce_at, emu.memType.gbaDebug)
		de = emu.read(vce_ptr + vce_de, emu.memType.gbaDebug)
		su = emu.read(vce_ptr + vce_su, emu.memType.gbaDebug)
		re = emu.read(vce_ptr + vce_re, emu.memType.gbaDebug)
		note = emu.read(vce_ptr + vce_note, emu.memType.gbaDebug)
		env = emu.read(vce_ptr + vce_env, emu.memType.gbaDebug)
		wave_ptr = emu.read32(vce_ptr + vce_wave, emu.memType.gbaDebug)
		wave_pos = emu.read32(vce_ptr + vce_wave_pos, emu.memType.gbaDebug)
		track_ptr = emu.read32(vce_ptr + vce_track, emu.memType.gbaDebug)
		s4a_voices[i + 1] = {
			status = status,
			note = note,			
			rv = rv,
			lv = lv,
			at = at,
			de = de,
			su = su,
			re = re,
			env = env,
			
			wave_ptr = wave_ptr,
			wave_pos = wave_pos,
			track_ptr = track_ptr
		}
  end

	for i = 0, 3 do
		vce_ptr = cgbchans_loc + vce_size * i
		status = emu.read(vce_ptr + vce_status, emu.memType.gbaDebug)
		rv = emu.read(vce_ptr + vce_rv, emu.memType.gbaDebug)
		lv = emu.read(vce_ptr + vce_lv, emu.memType.gbaDebug)
		at = emu.read(vce_ptr + vce_at, emu.memType.gbaDebug)
		de = emu.read(vce_ptr + vce_de, emu.memType.gbaDebug)
		su = emu.read(vce_ptr + vce_su, emu.memType.gbaDebug)
		re = emu.read(vce_ptr + vce_re, emu.memType.gbaDebug)
		note = emu.read(vce_ptr + vce_note, emu.memType.gbaDebug)
		env = emu.read(vce_ptr + vce_env, emu.memType.gbaDebug)
		wave_ptr = emu.read32(vce_ptr + vce_wave, emu.memType.gbaDebug)
		wave_pos = emu.read32(vce_ptr + vce_wave_pos, emu.memType.gbaDebug)
		track_ptr = emu.read32(vce_ptr + vce_track, emu.memType.gbaDebug)
		s4a_cgb_voices[i + 1] = {
			status = status,
			note = note,
			rv = rv,
			lv = lv,
			at = at,
			de = de,
			su = su,
			re = re,
			env = env,
			wave_ptr = wave_ptr,
			wave_pos = wave_pos,
			track_ptr = track_ptr
		}
  end
--emu.log(dump(s4a_voices[1]))
end

-- Source - https://stackoverflow.com/a/27028488
-- Posted by hookenz, modified by community. See post 'Timeline' for change history
-- Retrieved 2026-05-01, License - CC BY-SA 4.0
function dump(o)
	if type(o) == 'table' then
		local s = '{ '
			for k,v in pairs(o) do
				if type(k) ~= 'number' then k = '"'..k..'"' end
				s = s .. '['..k..'] = ' .. dump(v) .. ','
			end
		return s .. '} '
	else
		return tostring(o)
	end
end

function track_display(trk_ptr, y, i)
	if emu.read(trk_ptr + trk_status, emu.memType.gbaDebug) == 1 or i == -1 then
		color = 0x10FFFFFF

		if i > -1 then
			addr = emu.read32(trk_ptr + trk_data_ptr, emu.memType.gbaDebug)
				emu.drawString(18, y, string.format("%05X", addr & 0xFFFFF),
				color, 0xFF404040
				)
		emu.drawString(1, y, string.format("%2d:", i + 1),
			color, 0xFF404040
		)

	end
		
		
		msb = emu.read(trk_ptr + trk_bankmsb, emu.memType.gbaDebug)
		drums = msb == 0x7F
		program = emu.read(trk_ptr + trk_program, emu.memType.gbaDebug)
		pbr = emu.read(trk_ptr + trk_pbr, emu.memType.gbaDebug)
		pbsemi = emu.read(trk_ptr + trk_pbsemi, emu.memType.gbaDebug)
		pbfp = emu.read(trk_ptr + trk_pbfp, emu.memType.gbaDebug)
		if pbsemi >= 0x80 then
		  pbsemi = pbsemi - 0x100
		end
		pbfp = pbfp / 0xFF
		
		for v = 0, vce_max - 1 do
		  vce = s4a_voices[v + 1]
		  if vce.track_ptr == trk_ptr then
				n = vce.note + pbsemi + pbfp
				x = math.floor((n * 1.5) + 20)
				
				vol1 = ((vce.lv / 2) + (vce.rv / 2)) 
				-- vol1 = (vce.env / 0xFF) * ((vce.lv / 2) + (vce.rv / 2)) 
				vol = math.floor(((vol1 / 128) ) * 512) + 8
				--vol = math.floor((math.sqrt(math.floor(vce.lv / 2) + math.floor(vce.rv / 2)) / 64) * 255)
				if vol > 255 then
					vol = 255
				end
				vol2 = vol * 2
				if vol2 > 255 then
					vol2 = 255
				end
				vol3 = math.floor(vol * 0.25)
				alpha = (255 - vol) << 24
				alpha2 = (255 - vol2) << 24
				alpha3 = (255 - vol3) << 24
				
				if drums then
					wid = vce.note % 12
				else
					wid = program % 12
				end

				xcolor = hsl(30 * wid, 0.5, 0.5)
			
				xcolor2 = hsl(30 * wid, 0.5, 0.75)
				
				xcolor3 = 0x808080
				
				notecolor = alpha | (xcolor & 0xFFFFFF)
				notecolor2 = alpha2 | (xcolor2 & 0xFFFFFF)
				notecolor3 = alpha3 | (xcolor & 0xFFFFFF)
--				    emu.drawRectangle(x, y + 1, 3, 6, notecolor2, false)				  
				emu.drawRectangle(x - 1, y, 5, 6, notecolor3, false)				  
				emu.drawRectangle(x, y + 1, 3, 4, notecolor, false)				  
				emu.drawRectangle(x + 1, y + 2, 1, 2, notecolor2, true)
		  end -- vce.track_ptr == trk_ptr
		end -- v = 0, vce_max - 1 

		for v = 0, 3 do
		  vce = s4a_cgb_voices[v + 1]
		  if vce.track_ptr == trk_ptr then
				n = vce.note + pbsemi + pbfp
				x = math.floor((n * 1.5) + 20)
				vol1 = 0x80 --(vce.env / 0x10) * 64 -- * ((vce.lv / 2) + (vce.rv / 2))
				vol = math.floor(((vol1 / 64)) * 512) + 8

	--vol = math.floor((math.sqrt(math.floor(vce.lv / 2) + math.floor(vce.rv / 2)) / 64) * 255)

				if vol > 255 then
					vol = 255
				end
				vol2 = vol * 4
				if vol2 > 255 then
					vol2 = 255
				end
				vol3 = math.floor(vol * 0.25)
				alpha = (255 - vol) << 24
				alpha2 = (255 - vol2) << 24
				alpha3 = (255 - vol3) << 24
				xcolor1 = 0x10FFFFFF
				if v == 0 then
					xcolor1 = 0x1044FF44				    
				elseif v == 1 then
					xcolor1 = 0x10FFFF44				    
				elseif v == 2 then
					xcolor1 = 0x10FF4444				    
				else
					xcolor1 = 0x10FF44FF				    
				end
				xcolor2 = 0x10FF0000
				notecolor = alpha | (xcolor2 & 0xFFFFFF)
				notecolor2 = alpha2 | (xcolor1 & 0xFFFFFF)
				notecolor3 = alpha3 | (xcolor2 & 0xFFFFFF)
				emu.drawRectangle(x - 1, y, 5, 6, notecolor3, false)				  
				emu.drawRectangle(x, y + 1, 3, 4, notecolor, false)				  
				emu.drawRectangle(x + 1, y + 2, 1, 2, notecolor2, true)
		  end -- vce.track_ptr == trk_ptr
		end -- v = 0, 3
	end -- track status == 1
end

function update()
	dos4aupdate()

	if full_overlay then
		color = 0x14031C
		emu.drawRectangle(0, 0, 240, 160, color, true)
	elseif track_overlay then
		color = 0x4014031C
		emu.drawRectangle(0, 0, 240, 160, color, true)
	end
	
	y = 4
		
	if track_overlay then
		x = 6
		tcnt = 0
		ccnt = 0
		cnt = 0
		
		for v = 0, vce_max - 1 do
		  vce = s4a_voices[v + 1]
		  if vce.status > 0 then
				cnt = cnt + 1
		  end
		end
		for v = 0, 3 do
		  vce = s4a_cgb_voices[v + 1]
		  if vce.status > 0 then
		  	ccnt = ccnt + 1
		  end
		end

		if cnt > 0 then
			emu.drawString(x - 16, y - 10, string.format("%2d", cnt), 0x108888FF, 0xFF404040)
			tcnt = tcnt + cnt
		end
		x = 240 - 60
		stroke = 0x502F75
		fill = 0x402B1841
		textcolor = 0x10FFFFFF
		vcecolor = 0x1061E2FF
		cgbcolor = 0x10FF63BC
		freqcolor = 0x10FAC800
		beatcolor = 0x1097FF55
		emu.drawRectangle(x-2, y-2, 60, 21, stroke, false)
		emu.drawRectangle(x-2, y-2, 60, 21, fill, true)
		emu.drawString(x, y, string.format("Voice:", tcnt), textcolor, 0xFF404040)
		emu.drawString(x - 4, y, string.format("    %3d", tcnt), vcecolor, 0xFF404040)
		emu.drawString(x - 4, y, string.format("       /%2d", vce_max), textcolor, 0xFF404040)
		y = y + 10
		emu.drawString(x, y, string.format("CGB:", tcnt), textcolor, 0xFF404040)
		emu.drawString(x - 4, y, string.format("    %3d", ccnt), cgbcolor, 0xFF404040)
		y = y - 10
		x = 240 - 60 - 64
		emu.drawRectangle(x-2, y-2, 60, 21, stroke, false)
		emu.drawRectangle(x-2, y-2, 60, 21, fill, true)
		emu.drawString(x, y, string.format("Rate:", tcnt), textcolor, 0xFF404040)
		y = y + 10
		fm = emu.read(sndarea_loc + snd_freq_mode, emu.memType.gbaDebug)
		emu.drawString(x - 4, y, string.format("%8dhz", freq_modes[fm]), freqcolor, 0xFF404040)

		y = y - 10
		ticks = emu.read32(player_loc + ply_t, emu.memType.gbaDebug)
		ppqn = emu.read16(player_loc + ply_ppqn, emu.memType.gbaDebug)
		beats = math.floor(ticks / ppqn)
		tsnum = 4
		tsden = 4
		measure = math.floor(beats / (tsnum * (tsden / 4))) + 1
		beats = (beats % tsnum) + 1
		x = 4
		emu.drawRectangle(x-2, y-2, 60, 21, stroke, false)
		emu.drawRectangle(x-2, y-2, 60, 21, fill, true)
		emu.drawString(x, y, string.format("Beat:", tcnt), 0x10FFFFFF, 0xFF404040)
		y = y + 10
		fm = emu.read(sndarea_loc + snd_freq_mode, emu.memType.gbaDebug)
		emu.drawString(x - 4, y, string.format("%6d:%2d", measure, beats), beatcolor, 0xFF404040)
		emu.drawString(x - 4, y, string.format("      :", measure, beats), textcolor, 0xFF404040)


		y = 18

		track_count = emu.read16(player_loc + ply_trackcount, emu.memType.gbaDebug)
--		emu.log(string.format("player loc: %8X", player_loc))
--		emu.log(string.format("track count: %03d", track_count))
		for i = 0, track_count - 1 do
			trk_ptr = player_loc + ply_tracks + i * trk_size		
			track_display(trk_ptr, y, i)
			y = y + 8
		end -- i = 0, track_count - 1
		
		y = 18 + 8
		for i = 0, 15 do
			trk_ptr = player_loc + ply_miditracks + i * trk_size		
			track_display(trk_ptr, y, -1)
			y = y + 8
		end -- i = 0, track_count - 1

	end -- track_overlay

	if keydown("9") then
		track_overlay = not track_overlay
	end
	if keydown("0") then
		full_overlay = not full_overlay
	end

end

function keydown(key)
	kp = emu.isKeyPressed(key)
	state = false
	if kp and not keystate[key] then
		state = true
	end
	keystate[key] = kp
	
	return state
end

emu.addEventCallback(update, emu.eventType.endFrame)
