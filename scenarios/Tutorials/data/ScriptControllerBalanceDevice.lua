-- SCONE script that simulates a device that generates an external moment at a specific condition.
-- See Tutorial 6b - Script - Balance Device

function init( model, par )
	-- get the 'target_body' parameter from ScriptController, or set to "pelvis"
	target_body = model:find_body( scone.target_body or "pelvis" )

	-- get 'min_deg' and 'max_deg' parameters from ScriptController (required!)
	if ( scone.min_deg and scone.min_deg ) then
		body_min = par:create_from_string( "min_deg", scone.min_deg )
		body_max = par:create_from_string( "max_deg", scone.max_deg )
	else
		-- produce an error and abort the simulation
		error( "Must set min_deg and max_deg parameters!" )
	end

	-- get 'device_moment' and 'device_duration' parameters from ScriptController (not required!)
	device_moment_mag = scone.moment and par:create_from_string( "device_moment", scone.moment ) or 100
	device_duration = scone.duration and par:create_from_string( "device_duration", scone.duration ) or 0.2

	-- initialize global variables that keep track of the device state
	device_start = 0
	device_moment = 0
end

function update( model )
	local t = model:time()

	-- check if the device is currently active
	if device_moment ~= 0 then
		-- check if the device should deactivate
		if t - device_start > device_duration then
			-- add opposite external moment to deactivate device
			target_body:add_external_moment( 0, 0, -device_moment )
			device_moment = 0
			-- print a message to the scone messages window
			scone.debug( "device deactivated at " .. t )
		end
	-- device is not active, check if we activate it
	elseif device_start == 0 then -- device was not activated before
		-- get the current body orientation in degrees and check with body_min / body_max
		local ori = 57.3 * target_body:ang_pos().z
		if ori < body_min then
			device_moment = device_moment_mag
		elseif ori > body_max then
			device_moment = -device_moment_mag
		end

		-- check if device_moment was set
		if device_moment ~= 0 then
			device_start = t
			target_body:add_external_moment( 0, 0, device_moment )

			-- print a message to the scone messages window
			scone.debug( "device activated at t=" .. t .. " ori=" .. ori .. " moment=" .. device_moment )
		end
	end

	-- return false to keep going
	return false;
end
